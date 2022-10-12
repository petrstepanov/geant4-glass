#include <CanvasHelper.h>
#include <FileUtils.h>
#include <FitUtils.h>
#include <UiUtils.h>
#include <StringUtils.h>

#include <Rtypes.h>
#include <TString.h>
#include <TObjArray.h>
#include <TObjString.h>
#include <TPRegexp.h>
#include <TTreeReader.h>
#include <TTreeReaderValue.h>
#include <TTreeReaderArray.h>
#include <TFile.h>
#include <TH1.h>
#include <TCanvas.h>
//#include <TROOT.h>
#include <TMath.h>
#include <TVirtualPad.h>
#include <TPaveStats.h>
#include <TLegend.h>
#include <TStyle.h>
#include <TPie.h>
#include <TROOT.h>
#include <TPieSlice.h>
#include <TGMsgBox.h>
#include <TMap.h>
#include <iomanip>
#include <stdalign.h>
#include <limits>  // for INT_MIN, INT_MAX
#include <stdio.h>
#include <stdlib.h>
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpedantic"
#include <RooConstVar.h>
#pragma GCC diagnostic pop
#include <TVector2.h>
#include <TVector3.h>
#include <TVectorD.h>
#include <TLatex.h>
#include <TMathBase.h>
#include <TF1.h>
#include <TGFileDialog.h>
#include <TSystem.h>
#include <TGraphErrors.h>
#include <TPaletteAxis.h>
#include <TRandom1.h>
#include <TRandom2.h>
#include <TRandom3.h>
#include <TApplication.h>
#include <cstring>
#include <string>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <fstream>
#include <functional>

// ------------------------------------------------
// Plot particles escaping the world (for one file)
// ------------------------------------------------

void plotEscapeParticles(const char *fileName) {
  TFile *file = FileUtils::openFile(fileName);
  TTree *tree = FileUtils::getTree(file, "tree_escape");
  if (tree->GetEntries() == 0) {
    Error("plotEscapeParticles()", "\"tree_escape\" has zero entries");
    return;
  }

  Int_t maxEntries = std::min(50000, (Int_t) tree->GetEntries());

  // Get number of events, GPS particle energy and particle name
  Double_t particleEnergy = ((RooConstVar*) file->Get("gpsParticleEnergy"))->getVal();
  Int_t numberOfEvents = ((RooConstVar*) file->Get("numberOfEvents"))->getVal();
  const char *particleName = ((TObjString*) file->Get("gpsParticleName"))->GetName();
  const char *crystalMaterial = ((TObjString*) file->Get("crystalMaterial"))->GetName();
  TVector2 *crystalsNumber = (TVector2*) file->Get("crystalsNumber");
  TVector3 *crystalSize = (TVector3*) file->Get("crystalSize");
  Double_t totalGPSEnergy = numberOfEvents * particleEnergy;

  // Set Marker Style
  TCanvas *canvas = new TCanvas("escapeCanvas", "", 1280, 520);
  canvas->Divide(3, 1, 1E-6, 1E-6);

  // PAD 1: Draw world escape locations
  tree->SetMarkerStyle(6);
  TVirtualPad *pad1 = canvas->cd(1);
  // Int_t maxEntries = TMath::Min((Double_t)tree->GetEntries(), 1.E4);
  // tree->Draw("x:y:z:energy", "", "COLZ", maxEntries);
  pad1->SetMargin(0.15, 0.2, 0.15, 0.15);
  gStyle->SetPalette(kTemperatureMap);
  tree->Draw("y:x:z:energy", "", "COLZ");
  TH1 *htemp1 = (TH1*) pad1->GetListOfPrimitives()->FindObject("htemp");
  // ROOT bug:
  htemp1->SetTitle("Escape Locations;Z, mm; X, mm; Energy, MeV");
  pad1->Update();
  TPaletteAxis *palette = (TPaletteAxis*) htemp1->GetListOfFunctions()->FindObject("palette");
  palette->SetX1NDC(0.85);
  palette->SetX2NDC(0.9);
  palette->SetTitleOffset(-1.5);
  htemp1->GetXaxis()->SetTitleOffset(2);
  htemp1->GetYaxis()->SetTitleOffset(2);
  htemp1->GetZaxis()->SetTitleOffset(2);
  htemp1->GetZaxis()->SetTitle("Y, mm");

  // PAD 2: Draw Energy distribution
  TVirtualPad *pad2 = canvas->cd(2);
  pad2->SetLogy();
  gStyle->SetOptStat(10); // https://root.cern.ch/doc/master/classTPaveStats.html
  tree->Draw("energy", "", "", maxEntries);
  TH1 *htemp2 = (TH1*) pad2->GetListOfPrimitives()->FindObject("htemp");
  htemp2->SetTitle("Kinetic Energy Distribution;Kinetic energy, MeV;Events");
  htemp2->SetFillColor(kCyan);

  TPave *pave = CanvasHelper::getDefaultPaveStats(pad2);
  CanvasHelper::setPaveAlignment(pave, kPaveAlignRight | kPaveAlignTop);

  Double_t escapeEnergy = 0; // getWeightedIntegral(htemp2);
  Double_t energy;
  tree->SetBranchAddress("energy", &energy);
  for (Int_t i = 0; i < maxEntries; i++) {
    tree->GetEntry(i);
    escapeEnergy += energy;
  }

  EnergyValueUnit escapeEnergyEVU = StringUtils::formatEnergy(escapeEnergy);
  TString escapeEnergyString = TString::Format("Total E = %.2f %s", escapeEnergyEVU.value, escapeEnergyEVU.unit.Data());
  CanvasHelper::addTextToStats(escapeEnergyString.Data(), pad2);

  // PAD 3: Draw PDG
  /*TVirtualPad* pad3 =*/canvas->cd(3);
  Int_t pdgMax = std::numeric_limits<int>::min();
  Int_t pdgMin = std::numeric_limits<int>::max();
  Int_t pdg;
  tree->SetBranchAddress("pdg", &pdg);
  for (Int_t i = 0; i < maxEntries; i++) {
    tree->GetEntry(i);
    if (pdg > pdgMax)
      pdgMax = pdg;
    if (pdg < pdgMin)
      pdgMin = pdg;
  }
  TH1 *pdgHist = new TH1D("pdgHist", "Particle Type (PDG)", pdgMax - pdgMin + 1, pdgMin - 0.5, pdgMax + 0.5);
  for (Int_t i = 0; i < maxEntries; i++) {
    tree->GetEntry(i);
    pdgHist->Fill(pdg);
  }
  // Make pie chart
  TMap *pdgNameMap = (TMap*) file->Get("pdgNameMap");
  pdgNameMap->Print();
  std::vector<const char*> particleNames = { };
  std::vector<double> binValues = { };
  for (Int_t i = 1; i <= pdgHist->GetXaxis()->GetNbins(); i++) {
    if (pdgHist->GetBinContent(i) != 0) {
      TString *label = new TString();
      label->Form("%d", (int) pdgHist->GetBinCenter(i));
      TObjString *objString = (TObjString*) pdgNameMap->GetValue(label->Data());
      particleNames.push_back(objString->GetName());
      std::cout << label->Data() << std::endl;
      binValues.push_back(pdgHist->GetBinContent(i));
    }
  }
  Int_t colors[] = { kGray, kOrange + 1, kSpring - 5, kAzure + 8, kPink + 1, kViolet - 4, kRed - 7, kViolet + 6 };
  TPie *pie4 = new TPie("pie4", "Particle IDs", (int) binValues.size(), &binValues[0], colors, &particleNames[0]);
  pie4->SetRadius(.2);
  // pie4->SetLabelFormat("%txt: %val (%perc)");
  pie4->SetLabelFormat("%txt");
  pie4->SetTextSize(0.04);

  pie4->Draw("3dnol");
  TLegend *legend = new TLegend(0.5, 0.5, 0.9, 0.9, "");
  for (Int_t i = 0; i < pie4->GetEntries(); ++i) {
    Double_t pieValue = pie4->GetSlice(i)->GetValue();
    TString text = TString::Format("%s (%d particles, %.1f %%)", pie4->GetSlice(i)->GetTitle(), (int) pieValue,
                                   pieValue / tree->GetEntries() * 100);
    legend->AddEntry(pie4->GetSlice(i), text.Data(), "f");
  }
  legend->SetX1(0.1);
  Double_t legendBottom = 0.9 - 0.06 * legend->GetNRows();
  legend->SetY1(legendBottom);
  legend->SetX2(.9);
  legend->SetY2(.9);
  legend->Draw();
  pie4->SetY((1 - legendBottom) * 3 / 4);
  pie4->SetTitle("Particle Types and Ratios");

  // Add title and subtitle to canvas
  std::stringstream buffer;
  buffer << "Particles escaped the simulation for " << (Int_t) crystalsNumber->X() << "x" << (Int_t) crystalsNumber->Y()
         << ", " << crystalSize->X() << "x" << crystalSize->Y() << "x" << crystalSize->Z() << " mm " << crystalMaterial
         << " crystal assembly per event, " << particleEnergy / 1E3 << " GeV " << particleName << ".";
  TString canvasTitle = buffer.str().c_str();
  canvas->SetTitle(canvasTitle.Data());

  EnergyValueUnit evuTotal = StringUtils::formatEnergy(totalGPSEnergy);
  EnergyValueUnit evuEscape = StringUtils::formatEnergy(escapeEnergy);
  TString subtitle = TString::Format("Injected Kinetic Energy %.2f %s. Escaped Kinetic Energy %.2f %s (%.2f %%)",
                                     evuTotal.value, evuTotal.unit.Data(), evuEscape.value, evuEscape.unit.Data(),
                                     escapeEnergy / totalGPSEnergy * 100);
  CanvasHelper::addMultiCanvasTitle(canvas, canvas->GetTitle(), subtitle.Data());
  // CanvasHelper::getInstance()->addCanvas(canvas);

  // Save canvas
  TString fileNameOnly = StringUtils::stripExtension(fileName);
  fileNameOnly += "-eesc";
  canvas->SetName(fileNameOnly);
  CanvasHelper::saveCanvas(canvas, kFormatPng);
}

int escapeParticles() {
  // Get list of file names
  TString text = "Select output files to plot escape particles.";
  UiUtils::showMessageBox(text);
  TList *filePaths = UiUtils::getFilePaths(kTRUE);

  // Process every file in the list of user input files
  for (TObject *obj : *filePaths) {
    if (!obj->InheritsFrom(TObjString::Class()))
      continue;
    TObjString *objString = (TObjString*) obj;
    const char *fileName = objString->GetString().Data();
    plotEscapeParticles(fileName);
  }
  return 0;
}

#ifndef __CINT__

int main(int argc, char **argv) {
  // Instantiate TApplication
  TApplication *app = new TApplication("escapeParticles", &argc, argv);

  escapeParticles();

  // Enter the event loop
  app->Run();

  // Return success
  return 0;
}

#endif
