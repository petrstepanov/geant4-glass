#include <TFile.h>
#include <TApplication.h>
#include <iomanip>
#include <RooConstVar.h>
#include <TList.h>
#include <TVector3.h>
#include <TSystem.h>
#include <TROOT.h>
#include "include/utilities.h"

#define USEDEBUG 1;

TH1* getHistogram(const char* fileName, const char* histName){
  // Open ROOT File
  TFile* file = openFile(fileName);

  // Delete old name histogram from ROOT memory
//  TObject* oldHist = gROOT->FindObjectAnyFile(histName);
//  if (oldHist != NULL){
//    oldHist->Delete();
//  }

  TH1* hist = NULL;
  file->GetObject(histName, hist);
  hist->Print();

  if (!hist){
    std::cout << "Cannot find histogram with name \"" << histName << "\". Aborting." << std::endl;
    return NULL;
  }

  // Clone a histogram from the file
  TString newHistName = ((TObjString*)file->Get("crystalMaterial"))->GetString();
  TString newHistTitle = "";
  // Obtain GPS position
  if (file->Get("gpsPosition") != NULL){
    RooConstVar* gpsPosition = ((RooConstVar*)file->Get("gpsPosition"));
    newHistName += "+";
    newHistName += gpsPosition->getVal();
    newHistName += "mm";
    newHistTitle += gpsPosition->getVal();
    newHistTitle += " mm";
  }
  else {
    // Previously we did not save this variable in the ROOT file.
    // So guesstimating it goes 10, 30, 50, .. 190
    static int distance = 10;
    newHistName += "+";
    newHistName += distance;
    newHistName += "mm";
    newHistTitle += distance;
    newHistTitle += " mm";
    distance +=20;
  }

  // Clone histogram
  // TH1* newHist = cloneHist(hist, newHistName);
  TH1* newHist = (TH1*)hist->Clone(newHistName);
  newHist->SetTitle(newHistTitle);

  // Program crashes when I close the input file??
  // file->Close();

  // Return cloned hist
  return newHist;
}

Int_t uuid = 0;
Int_t wlMin = 200;
Int_t wlMax = 800;

void lumiUnitVolumesOutput(){
  // Show file picker

  TString text = "Select series of spectra with detected optical output from the luminescence spectra of each unit volumes";
  TList* fileNames = getFileNamesList(text);
  fileNames->Sort(kSortAscending);

  if (fileNames->GetSize() <= 0) return;

  // Set current ROOT working directory to first file path
  TString firstFileName = ((TObjString*)fileNames->At(0))->String();
  TFile* firstFile = openFile(firstFileName); // For future use
  TString directory = firstFile->GetPath();
  gSystem->cd(directory);

  // Open material file
  const char* materialName = ((TObjString*)firstFile->Get("crystalMaterial"))->GetString().Data();
  TString materialFileName = TString::Format("materials/%s.root", materialName);
  TFile* materialFile = new TFile(materialFileName.Data());

  // Open detector file
  const char* detectorName = ((TObjString*)firstFile->Get("detectorName"))->GetString().Data();
  TString detectorFileName = TString::Format("detectors/%s.root", detectorName);
  TFile* detectorFile = new TFile(detectorFileName.Data());

  // Prepare canvas
  TCanvas* canvas = new TCanvas("canvas", "Emission, Absorption Length and Quantum Efficincy", 800, 900);
  canvas->Divide(1, 3);

  // Add 4th pad to canvas overlays 3rd - refer to TPad::Divide()
  TString padName = TString::Format("%s_%d", canvas->GetName(), 3);
  TVirtualPad* p = (TVirtualPad*)canvas->GetListOfPrimitives()->FindObject(padName.Data());
  TPad* pad = new TPad(padName.Data(), "", p->GetXlowNDC(), p->GetYlowNDC(), p->GetXlowNDC()+p->GetWNDC(), p->GetYlowNDC()+p->GetHNDC());
  pad->SetNumber(4);
  pad->SetFillStyle(0);
  pad->SetFrameFillStyle(0);
  pad->Draw();
  canvas->Modified();

  Float_t scaleFactor = 2;

  // Plot emission
  TVirtualPad* pad1 = canvas->cd(1);
  addGrid(pad1);
  const Double_t leftMargin = 0.15;
  const Double_t rightMargin = 0.05;
  pad1->SetLeftMargin(leftMargin);
  pad1->SetRightMargin(rightMargin);
  pad1->SetBottomMargin(0.2);
  TGraph *emission = (TGraph*)materialFile->Get("FASTCOMPONENT");
  increaseGraphFontSize(emission, scaleFactor);
  emission->GetXaxis()->SetLimits(wlMin, wlMax);
  emission->GetXaxis()->SetTitleOffset(1);
  emission->GetYaxis()->SetTitle("Fast Component, a.u.");
  emission->Draw();

  // Plot absorption length
  TVirtualPad* pad2 = canvas->cd(2);
  addGrid(pad2);
  pad2->SetLeftMargin(leftMargin);
  pad2->SetRightMargin(rightMargin);
  pad2->SetBottomMargin(0.2);
  TGraph *absLength = (TGraph*)materialFile->Get("ABSLENGTH");
  increaseGraphFontSize(absLength, scaleFactor);
  absLength->GetXaxis()->SetLimits(wlMin, wlMax);
  absLength->GetXaxis()->SetTitleOffset(1);
  absLength->GetYaxis()->SetTitle("Absorption Length, mm");
  absLength->Draw();
  // Draw region fading the crystal length
  TVector3* crystalSize = (TVector3*)firstFile->Get("crystalSize");
  TBox* box = new TBox(wlMin, 0, wlMax, crystalSize->Z());
  box->SetFillStyle(3244); // https://root.cern.ch/doc/master/classTAttFill.html
  box->SetFillColor(kBlack);
  box->Draw();

  TLegend* l = new TLegend();
  l->AddEntry(box, "Crystal length", "f");
  l->Draw();

  alignLegend(pad2);

  // Plot quantum efficiency
  TVirtualPad* pad3 = canvas->cd(3);
  addGrid(pad3);
  pad3->SetLeftMargin(leftMargin);
  pad3->SetRightMargin(rightMargin);
  pad3->SetBottomMargin(0.2);
  TGraph *efficiency = (TGraph*)detectorFile->Get("efficiency");
  increaseGraphFontSize(efficiency, scaleFactor);
  efficiency->GetXaxis()->SetLimits(wlMin, wlMax);
  TString newGraphTitle = efficiency->GetTitle();
  newGraphTitle += ". OP Spectra On Photocathode. ";
  newGraphTitle += formatMaterialAndSize(firstFile);
  newGraphTitle += ".";
  efficiency->SetTitle(newGraphTitle);
  efficiency->Draw();

  // Prepare OP histograms at photocathodes for every unit volume
  std::vector<TH1*> opHistList = {};
  for (int i=0; i < fileNames->GetSize(); i++){
    TObject* obj = fileNames->At(i);
    if (!obj->InheritsFrom(TObjString::Class())) continue;
    TObjString *objString = (TObjString*)obj;

    TH1* opHist = getHistogram(objString->GetString().Data(), "photonWlReachedDetector");
    // opSpectrum->SetLineColorAlpha(kBlue, alpha);
    // Double_t alpha = (i+1.)/(fileNames->GetSize()+1.);
    opHistList.push_back(opHist);
  }

  // Draw photons arriving at the photocathode
  TVirtualPad* padOP = canvas->cd(4);
  padOP->SetLeftMargin(leftMargin);
  padOP->SetRightMargin(rightMargin);
  padOP->SetBottomMargin(0.2);

  Int_t histIndex = 0;
  // Set maximum to first hist to ensure correct plotting of others
  // Otherwise last hists can exceed top graph limit;
  opHistList[0]->SetMaximum(findHistsMax(opHistList));

  for (TH1* hist : opHistList){
    // Calculate avg OP per event
    TTree* treeOptical = (TTree*)firstFile->Get("tree_optical");
    LongDouble_t opTotal = 0;
    Int_t op;
    treeOptical->SetBranchAddress("op", &op);
    for (Int_t i = 0; i < (Int_t)treeOptical->GetEntries(); i++){
      treeOptical->GetEntry(i);
      opTotal += op;
    }
    Double_t opMean = opTotal/treeOptical->GetEntries();
    TString histTitle = TString::Format("%s. OP on cathode %.2f/event", hist->GetTitle(), opMean);

    hist->SetTitle(histTitle.Data());
    hist->GetXaxis()->SetRangeUser(wlMin, wlMax);
    hist->GetXaxis()->SetLabelSize(0);
    hist->GetXaxis()->SetTickLength(0);
    hist->GetYaxis()->SetLabelSize(0);
    hist->GetYaxis()->SetTickLength(0);
    // wlHist->GetYaxis()->SetTitle("OP at Cathode, a.u.");

    // wlHist->GetXaxis()->SetTitle("Photon wavelength, nm");
    // wlHist->GetYaxis()->SetTitle("OP reached photo-cathode");

    increaseAxisFontSize(hist->GetYaxis(), scaleFactor);
    hist->SetStats(0);

    Color_t color = getGradientColor(histIndex, (Int_t)opHistList.size(), EColorPalette::kGreyScale, kTRUE);
    hist->SetLineColor(color);

    // hist->SetFillStyle(EFillStyle::kFDotted2);
    // hist->SetFillColor(EColor::kBlack);
    // hist->Draw("Y+");

    hist->Draw(histIndex++ == 0?"":"SAME");
  }

  TLegend* opLegend = padOP->BuildLegend();
  opLegend->SetHeader("Particle gun position", "C");
  alignLegend(padOP, Alignment::TOP_RIGHT, Decoration::DEFAULT, 0.05);

  // Set canvas pe title (hack - after all legends were drawn)
  // https://root-forum.cern.ch/t/change-histograms-title-on-canvas/17854/9
  setCanvasTitle(padOP, opHistList[0], "");

  //------------------------------------------------------------------------------------------


  // Find joint histograms range (here in bins)
  int minWavelengthBin = findFirstNonZeroBin(opHistList);
  int maxWavelengthBin = findLastNonZeroBin(opHistList);

  // Plot OP spectrum reached detector
  // TCanvas* canvas = new TCanvas("canvas", "", 640, 800);
  // canvas->Divide(0,2);

  // Plot emission spectrum ?
  // ------------------------

  // TVirtualPad* padOP = canvas->cd(1);
  // Int_t i = 0;
/*
  for (TH1* hist : opHistList){
    Color_t color = getGradientColor(i, (Int_t)opHistList.size(), EColorPalette::kAvocado);
    hist->SetLineColor(color);
    hist->GetXaxis()->SetTitle("Photon wavelength, nm");
    hist->GetYaxis()->SetTitle("OP reached photo-cathode");
    std::cout << "plotting hist " << hist->GetName() << std::endl;
    hist->GetXaxis()->SetRange(minWavelengthBin, maxWavelengthBin);
    if (i==0) {
      hist->Draw();
    } else {
      hist->Draw("SAME");
    }
    i++;
  }
*/
  // Build legend
//  TLegend* opLegend = padOP->BuildLegend();
//  opLegend->SetHeader("Particle gun position", "C");
//  alignLegend(padOP, Alignment::TOP_RIGHT);
//
//  // Plot efficiency curve
//  TVirtualPad* padEfficiency = canvas->cd(2);
//  TString firstFileName = ((TObjString*)(fileNames->At(0)))->GetString();
//  TFile* firstFile = openFile(firstFileName);
//  TString detectorName = ((TObjString*)firstFile->Get("detectorName"))->GetString();
//  TGraph* efficiencyGraph = getEfficiencyGraph(detectorName);
//  efficiencyGraph->Draw();

  // Plot PE on the right
  /*
  TVirtualPad* padPE = canvas->cd(3);
  i = 0;
  std::vector<TH1*> peHistList = {};
  for (TH1* histOP : opHistList){
    TH1* histPE = convertOpToPe(histOP, detectorName);
    Color_t color = getGradientColor(i, (Int_t)opHistList.size(), EColorPalette::kSolar);
    histPE->SetLineColor(color);
    histPE->GetXaxis()->SetTitle("Photon wavelength, nm");
    histPE->GetYaxis()->SetTitle("PE registered by the detector");
    // histPE->SetFillStyle(EFillStyle::kFEmpty);
    histPE->GetXaxis()->SetRange(minWavelengthBin, maxWavelengthBin);
    peHistList.push_back(histPE);
    if (i==0) {
      histPE->Draw();
    } else {
      histPE->Draw("SAME");
    }
    i++;
  }
  TLegend* peLegend = padPE->BuildLegend();
  peLegend->SetHeader("Particle gun position", "C");
  alignLegend(padPE, Alignment::TOP_RIGHT);
  */

  // Set canvas op title (hack - after all legends were drawn)
  // https://root-forum.cern.ch/t/change-histograms-title-on-canvas/17854/9
  // setCanvasTitle(padOP, opHistList[0], "Optical photon (OP) spectra reached photocathode");

  // Set canvas pe title (hack - after all legends were drawn)
  // https://root-forum.cern.ch/t/change-histograms-title-on-canvas/17854/9
  // setCanvasTitle(padPE, peHistList[0], "Photo-electron (PE) spectra output from detector");

  // 3D Canvas with OP that reached detector
  // TVector3* crystalSize = (TVector3*)firstFile->Get("crystalSize");
  TCanvas* canvasOP = new TCanvas("canvasOP3D");
  canvasOP->SetLeftMargin(0.115);
  canvasOP->SetRightMargin(0.04);
  canvasOP->SetPhi(30-90); // Default is 30 deg

  TH2* histQueue = makeHistQueue(opHistList, "opAtDetector", crystalSize->Z());

  // Assign histogram axis titles
  TString title ("OP on photo-cathode in ");
  title += formatMaterialAndSize(firstFile);
  histQueue->SetTitle(title);

  histQueue->GetXaxis()->SetTitle("Photon wavelength, nm");
  histQueue->GetYaxis()->SetTitle("Emission position, mm");
  histQueue->GetZaxis()->SetTitle("Number of optical photons");

  histQueue->SetStats(0);
  histQueue->SetLineColor(kBlack);
  TColor::InvertPalette();

  histQueue->GetXaxis()->SetRangeUser(minWavelengthBin, maxWavelengthBin);
  histQueue->GetXaxis()->SetTitleOffset(2);
  histQueue->GetYaxis()->SetTitleOffset(2);
  histQueue->GetZaxis()->SetTitleOffset(1.5);

  histQueue->Draw("LEGO2");

  // canvasOP->Modified();
  // canvasOP->Update();
  // Canvas with PE output by the detector
  // TCanvas* canvas = new TCanvas();

  // Plot PE distribution
  // Get detector efficiency curve

}

#ifndef __CINT__

int main(int argc, char **argv) {
  // Instantiate TApplication
  TApplication* app = new TApplication("lumiUnitVolumesOutput", &argc, argv);

  lumiUnitVolumesOutput();

  // Enter the event loop
  app->Run();
  // Return success
  return 0;
}

#endif
