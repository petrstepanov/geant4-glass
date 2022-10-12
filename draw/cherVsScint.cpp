#include <TGFileDialog.h>
#include <TTree.h>
#include <TH2.h>
#include <RooConstVar.h>
#include <TVector2.h>
#include <TVector3.h>
#include <TPaletteAxis.h>
#include <TStyle.h>
#include <TBranch.h>
#include <TF1.h>
#include <TRootCanvas.h>
#include <TApplication.h>
#include <cstring>
#include <float.h>
#include <THStack.h>
#include <TMultiGraph.h>
#include <TLegendEntry.h>
#include <TGaxis.h>

#include "include/utilities.h"

TString makeTitle(TFile* file){
  // Generate canvas title
  TString title = getMaterialName(file);
  title += " ";
  title += getCrystalSize(file);
  title += " mm. ";

  EnergyValueUnit evu = formatEnergy(getIncidentParticleEnergy(file));
  title += evu.value;
  title += " ";
  title += evu.unit;
  title += " ";
  title += getIncidentParticleType(file);
  title += ". ";
  title += getNumberOfEvents(file);
  title += " events.";

  std::cout << title << std::endl;
  return title;
}

Double_t slopeVariance(TGraph* graph, Double_t meanSlope){
  Double_t variance = 0;
  for (int i=0; i < graph->GetN(); i++){
    Double_t slope = graph->GetPointY(i)/graph->GetPointX(i);
    variance += (slope-meanSlope)*(slope-meanSlope);
  }
  variance /= (graph->GetN()-1);
  return variance;
}


void plotEmittedVsDetected(const char* filename){
  TFile* file = openFile(filename);

  // Set curent directory to this file - to locate relative Materials and Detectors files
  TString directory = file->GetPath();
  gSystem->cd(directory);

  // Get procuced (emitted) and detected OP spectra from file
  TH1* prodOPChereHist = (TH1*)file->Get("prod_op_chere");
  TH1* prodOPScintHist = (TH1*)file->Get("prod_op_scint");
  TH1* detOPChereHist = (TH1*)file->Get("det_op_chere");
  TH1* detOPScintHist = (TH1*)file->Get("det_op_scint");

  // Define colors
  prodOPChereHist->SetLineColor(kBlack);
  prodOPChereHist->SetLineStyle(kDashed);
  prodOPChereHist->SetFillColor(19);
  prodOPScintHist->SetLineColor(kBlack);
  prodOPScintHist->SetFillColor(19);

  detOPChereHist->SetLineColor(kBlack);
  detOPChereHist->SetLineStyle(kDashed);
  detOPChereHist->SetFillColor(kGray);
  detOPScintHist->SetLineColor(kBlack);
  detOPScintHist->SetFillColor(kGray);

  // Combine Cherenkov and scintillation spectra (histograms) in stacks
  THStack *emittedHS = new THStack("emitted_hs","Emitted vs Detected Scintillation and Cherenkov OP Spectra;Wavelength, nm;Counts");
  emittedHS->Add(prodOPChereHist);
  emittedHS->Add(prodOPScintHist);

  THStack *detectedHS = new THStack("detected_hs","");
  detectedHS->Add(detOPChereHist);
  detectedHS->Add(detOPScintHist);

  // Prepare Canvas
  TCanvas* canvas = new TCanvas();
  ((TRootCanvas*)canvas->GetCanvasImp())->ShowMenuBar(kFALSE);
  canvas->Divide(1,2);
  canvas->SetWindowSize(canvas->GetWw()*1.5, canvas->GetWw()*1.5);  // Make square canvasw

  // Add overlay pad
  addOverlayPad(canvas, 2, 2);

  // Plot emitted and detected OP spectra
  TVirtualPad* pad1 = canvas->cd(1);
  pad1->SetGrid();
  emittedHS->Draw();
  detectedHS->Draw("SAME");

  const int wlMin = 300, wlMax = 700;
  emittedHS->GetXaxis()->SetRangeUser(wlMin, wlMax);
  emittedHS->GetYaxis()->SetTitleOffset(1);
  pad1->BuildLegend();
  alignLegend(pad1, Alignment::TOP_RIGHT, Decoration::DEFAULT, 0.07, 0.399713);

  // Plot material transmittance
  TVirtualPad* pad2 = canvas->cd(2);
  pad2->SetGrid();

  const char* crystalMaterial = ((TObjString*)file->Get("crystalMaterial"))->GetString().Data();
  TString crystalFileName = TString::Format("./materials/%s.root", crystalMaterial);
  TFile* crystalFile = new TFile(crystalFileName.Data());

  TGraph* absLengthGraph = (TGraph*)crystalFile->Get("ABSLENGTH");
  absLengthGraph->SetTitle("Crystal Absorption Length and Detector Efficiency");
  absLengthGraph->SetMarkerStyle(kStar);
  absLengthGraph->GetXaxis()->SetLimits(wlMin, wlMax);
  absLengthGraph->GetXaxis()->SetRangeUser(wlMin, wlMax);
  absLengthGraph->GetXaxis()->SetTitle("Wavelength, nm");
  absLengthGraph->GetYaxis()->SetTitle("Absorption length, mm");
  absLengthGraph->GetYaxis()->SetTitleOffset(1);

  absLengthGraph->Draw("apl");
  // pad2->BuildLegend();
  // alignLegend(pad2, Alignment::TOP_LEFT);

  // Plot detector efficiency
  TVirtualPad* pad3 = canvas->cd(3);
  const char* detectorName = ((TObjString*)file->Get("detectorName"))->GetString().Data();
  TString detectorFileName = TString::Format("./detectors/%s.root", detectorName);
  TFile* detectorFile = new TFile(detectorFileName.Data());

  TGraph* efficiencyGraph = (TGraph*)detectorFile->Get("efficiency");
  efficiencyGraph->SetTitle(""); // Empty title because it overlays graph below
  efficiencyGraph->SetMarkerStyle(kFullDotLarge);
  efficiencyGraph->SetMarkerSize(0.75);
  efficiencyGraph->GetXaxis()->SetLimits(wlMin, wlMax);              // Not sure why for X axis SetLimits()
  efficiencyGraph->GetXaxis()->SetRangeUser(wlMin, wlMax);              // Not sure why for X axis SetLimits()
  efficiencyGraph->GetYaxis()->SetLimits(0, 100);                 // But for Y axis SetRangeUser()
  efficiencyGraph->GetYaxis()->SetRangeUser(0, 100);                 // But for Y axis SetRangeUser()
  efficiencyGraph->GetYaxis()->SetTitle("Quantum efficiency, %");
  efficiencyGraph->GetYaxis()->SetAxisColor(kRed);
  efficiencyGraph->GetYaxis()->SetTitleColor(kRed);
  efficiencyGraph->GetYaxis()->SetLabelColor(kRed);
  efficiencyGraph->GetYaxis()->SetAxisColor(kRed);
  efficiencyGraph->GetYaxis()->SetTitleOffset(1);
  efficiencyGraph->SetLineColor(kRed);
  efficiencyGraph->SetMarkerColor(kRed);
  efficiencyGraph->Draw("aplY+"); // Y+ - axis on right
  // efficiencyGraph->GetXaxis()->SetLabelOffset(999);  // Hack hide axis
  // efficiencyGraph->GetXaxis()->SetLabelSize(0);      // https://root-forum.cern.ch/t/removing-only-x-axis-from-a-histogram/4934/2

  // Build legend
  TLegend* legend = new TLegend();
  TString absLengthTitle = TString::Format("%s absorption length", crystalMaterial);
  legend->AddEntry(absLengthGraph, absLengthTitle, "LP");
  TString efficiencyTitle = TString::Format("%s quantum efficiency", detectorName);
  TLegendEntry* efficiencyLegendEntry = legend->AddEntry(efficiencyGraph, efficiencyTitle, "LP");
  efficiencyLegendEntry->SetTextColor(kRed);
  legend->Draw();
  alignLegend(pad3, Alignment::TOP_RIGHT);

  // Add joint title to the canvas
  TString title = makeTitle(file);
  addMultiCanvasTitle(canvas, title);

  // Save canvas to disk
  TString outputFilename = removeFileExtension(filename);
  outputFilename += "emit-detect";
  saveCanvasToDisk(canvas, outputFilename.Data(), kTRUE, kTRUE, kFALSE);
}

void plotPEHists(const char* filename){
  TFile* file = openFile(filename);
  TTree* tree = getTree(file, "tree_optical");

  Int_t maxPE = getBranchMaximum(tree, "pe")*0.3;

  // Prepare histograms for filling
  Int_t nbins = maxPE+1;
  TH1* peHist = new TH1I("photo_electrons", "All PE", nbins, 0, maxPE);
  TH1* peScintHist = new TH1I("photo_electrons_scint", "Scintillation PE", nbins, 0, maxPE);
  TH1* peChereHist = new TH1I("photo_electrons_chere", "Cherenkov PE", nbins, 0, maxPE);

  // Populate histograms. Should pass "y:x >> hist
  tree->Draw("pe_all >> photo_electrons", "", "goff");
  tree->Draw("pe_scint_all >> photo_electrons_scint", "", "goff");
  tree->Draw("pe_chere_all >> photo_electrons_chere", "", "goff");

  THStack* hs = new THStack("hs","Photo-Electron (PE) Output from the Photo-Cathode");
  peScintHist->SetLineColor(EColor::kBlack);
  peScintHist->SetFillColor(EColor::kBlack);
  peScintHist->SetFillStyle(EFillStyle::kFDotted2);
  hs->Add(peScintHist);

  peChereHist->SetLineColor(EColor::kBlack);
  peChereHist->SetFillColor(EColor::kBlack);
  peChereHist->SetFillStyle(EFillStyle::kFDotted3);
  hs->Add(peChereHist);

  peHist->SetLineColor(EColor::kBlack);
  peHist->SetFillColor(EColor::kBlack);
  peHist->SetFillStyle(EFillStyle::kFDotted1);
  hs->Add(peHist);
  hs->SetMaximum(hs->GetMaximum()*1.1); // Rescale Y axis to see the label

  TCanvas* canvas = new TCanvas();
  canvas->SetWindowSize(canvas->GetWw()*1.5, canvas->GetWw()*1.5*0.5);
  canvas->SetGrid();

  hs->Draw("nostack");
  TText* t = makeLabelAtHistogramPeak(peHist);
  t->Draw();
  TText* ts = makeLabelAtHistogramPeak(peScintHist);
  ts->Draw();
  TText* tc = makeLabelAtHistogramPeak(peChereHist);
  tc->Draw();

  // Fit Histograms
  // TF1* peScintFunc = fitHistWithCrystalBall(peScintHist, kTRUE);
  //peScintFunc->Draw();

  // Add joint title to the canvas
  TString title = makeTitle(file);
  addMultiCanvasTitle(canvas, title);

  // Weird but it requires to be Updated
  canvas->Modified();
  canvas->Update();

  // Save canvas to disk
  TString outputFilename = removeFileExtension(filename);
  outputFilename += "-s-to-c-2";
  saveCanvasToDisk(canvas, outputFilename, kTRUE, kTRUE, kFALSE);
}

std::vector<Double_t> plotCherVsScint(const char* fileName){
  TFile* file = openFile(fileName);
  TTree* tree = getTree(file, "tree_optical");

  Int_t maxProdChere = getBranchMaximum(tree, "prod_op_chere");
  Int_t maxProdScint = getBranchMaximum(tree, "prod_op_scint");

  // Create and fillemitted ad detected histograms from tree
  Int_t nbins = 100;
  TH2I* prodHist = new TH2I("op_emitted", "OP Emitted per Event;Cherenkov photon count;Scintillation photon count", nbins, 0, maxProdChere, nbins, 0, maxProdScint);
  TH2I* detHist =  new TH2I("op_detected", "OP Detected per Event;Cherenkov photon count;Scintillation photon count", nbins, 0, maxProdChere, nbins, 0, maxProdScint);

  // Populate histograms. Should pass "y:x >> hist
  tree->Draw("prod_op_scint:prod_op_chere >> op_emitted", "", "goff");
  tree->Draw("det_op_scint:det_op_chere >> op_detected", "", "goff");

  TCanvas* canvas = new TCanvas();
  ((TRootCanvas*)canvas->GetCanvasImp())->ShowMenuBar(kFALSE);
  canvas->SetWindowSize(canvas->GetWw()*1.25, canvas->GetWw()*1.25);  // Make square canvas with size (width*2 x width*2)
  canvas->Divide(2,2);

  // Canvas 1. Plot Produced S:C ratio
  {
    TVirtualPad* pad = canvas->cd(1);
    pad->SetGrid();
    pad->SetLeftMargin(0.15);
    pad->SetRightMargin(0.15);
    prodHist->Draw("COLZ");
    movePalleteAxis(pad, prodHist);
    alignStats(pad);
  }

  // Canvas 2. Plot Detected S:C ratio
  {
    TVirtualPad* pad = canvas->cd(2);
    pad->SetGrid();
    pad->SetLeftMargin(0.15);
    pad->SetRightMargin(0.15);
    detHist->Draw("COLZ");
    movePalleteAxis(pad, detHist);
    alignStats(pad);
  }

  // Set fit information
  gStyle->SetOptFit(1);

  // Canvas 3. Fit Produced S:C ratio
  Double_t sToCEmitted, sToCEmittedSTD;
  {
    TVirtualPad* pad = canvas->cd(3);
    pad->SetGrid();
    pad->SetLeftMargin(0.15);
    pad->SetRightMargin(0.15);
    Int_t n = tree->Draw("prod_op_chere:prod_op_scint","","goff");
    TGraph *g = new TGraph(n,tree->GetV1(),tree->GetV2());
    g->SetTitle(";Cherenkov photon count;Scintillation photon count");
    g->SetMarkerStyle(EMarkerStyle::kFullDotMedium);

    // Fit TGraph with line
    TF1* func = new TF1("linFunc", "[0]*x", 2e6);
    func->SetParName(0, "S #divide C");
    g->Fit("linFunc");
    g->Draw("AP");
    // For TGraph we need to do call SetLimits then SetRangeUser()
    g->GetXaxis()->SetLimits(0, maxProdChere);
    g->GetXaxis()->SetRangeUser(0, maxProdChere);
    g->GetYaxis()->SetLimits(0, maxProdScint);
    g->GetYaxis()->SetRangeUser(0, maxProdScint);

    // https://www.webassign.net/question_assets/unccolphysmechl1/measurements/manual.html
    sToCEmitted = func->GetParameter(0);
    Double_t sToCEmittedVariance = slopeVariance(g, sToCEmitted);
    sToCEmittedSTD = TMath::Sqrt(sToCEmittedVariance);
  }

  // Canvas 4. Fit Detected S:C ratio
  Double_t sToCDetected, sToCDetectedSTD;
  {
    TVirtualPad* pad = canvas->cd(4);
    pad->SetGrid();
    pad->SetLeftMargin(0.15);
    pad->SetRightMargin(0.15);
    Int_t n = tree->Draw("det_op_chere:det_op_scint","","goff");
    TGraph *g = new TGraph(n,tree->GetV1(),tree->GetV2());
    g->SetTitle(";Cherenkov photon count;Scintillation photon count");
    g->SetMarkerStyle(EMarkerStyle::kFullDotMedium);

    // Fit TGraph with line
    TF1* func = new TF1("linFunc", "[0]*x", 2e6);
    func->SetParName(0, "S #divide C");
    g->Fit("linFunc");
    g->Draw("AP");
    // For TGraph we need to do call SetLimits then SetRangeUser()
    g->GetXaxis()->SetLimits(0, maxProdChere);
    g->GetXaxis()->SetRangeUser(0, maxProdChere);
    g->GetYaxis()->SetLimits(0, maxProdScint);
    g->GetYaxis()->SetRangeUser(0, maxProdScint);

    // https://www.webassign.net/question_assets/unccolphysmechl1/measurements/manual.html
    sToCDetected = func->GetParameter(0);
    Int_t sToCDetectedVariance = slopeVariance(g, sToCDetected);
    sToCDetectedSTD = TMath::Sqrt(sToCDetectedVariance);
  }

  // Add joint title to the canvas
  TString title = makeTitle(file);
  addMultiCanvasTitle(canvas, title);

  // Save canvas to disk
  TString outputFilename = removeFileExtension(fileName);
  outputFilename += "-s-to-c";
  saveCanvasToDisk(canvas, outputFilename, kTRUE, kTRUE, kFALSE);

  // Return graph point
  std::vector<Double_t> vector = {sToCEmitted, sToCEmittedSTD, sToCDetected , sToCDetectedSTD};
  return vector;
}

int cherVsScint(){
  TString text = "Open simulation outputs with optical effects turned on";
  TList* fileNames = getFileNamesList(text);
  // fileNames->Sort()
  // Tree for saving parameters to ASCII file for gnuplot
  Double_t energy;
  Double_t events;

  // Ratios of emitted OP
  Double_t sToCEmitted;
  Double_t sEmittedRatio;
  Double_t sEmittedRatioError;
  Double_t cEmittedRatio;
  Double_t sEmittedMean;
  Double_t sEmittedRMS;
  Double_t cEmittedMean;
  Double_t cEmittedRMS;

  // Ratios of detceted OP
  Double_t sToCDetected;
  Double_t sDetectedRatio;
  Double_t sDetectedRatioError;
  Double_t cDetectedRatio;
  Double_t sDetectedMean;
  Double_t sDetectedRMS;
  Double_t cDetectedMean;
  Double_t cDetectedRMS;

  // Construct tree to output ASCII file
  TTree *tree = new TTree("tree","Scintillation to Cherenkov data");

  // Assign general columns
  (tree->Branch("energy",&energy,"energy/D"))->SetTitle("Energy, GeV");
  (tree->Branch("events",&events,"events/D"))->SetTitle("Events");

  // Assign emission ratios as tree columns
  (tree->Branch("sToCEmitted",&sToCEmitted,"sToCEmitted/D"))->SetTitle("Emitted Scint to Chere ratio, a.u.");
  (tree->Branch("sEmittedRatio",&sEmittedRatio,"sEmittedRatio/D"))->SetTitle("Emitted Scint ratio, a.u.");
  (tree->Branch("sEmittedRatioError",&sEmittedRatioError,"sEmittedRatioError/D"))->SetTitle("ESR, a.u.");
  (tree->Branch("cEmittedRatio",&cEmittedRatio,"cEmittedRatio/D"))->SetTitle("ECR, a.u.");
  (tree->Branch("sEmittedMean",&sEmittedMean,"sEmittedMean/D"))->SetTitle("Mean emitted scint photons");
  (tree->Branch("sEmittedRMS",&sEmittedRMS,"sEmittedRMS/D"))->SetTitle("MESP RMS");
  (tree->Branch("cEmittedMean",&cEmittedMean,"cEmittedMean/D"))->SetTitle("Mean emitted Chere photons");
  (tree->Branch("cEmittedRMS",&cEmittedRMS,"cEmittedRMS/D"))->SetTitle("MECP RMS");

  // Assign detected ratios as tree columns
  (tree->Branch("sToCDetected",&sToCDetected,"sToCDetected/D"))->SetTitle("Detected Scint to Chere ratio, a.u.");
  (tree->Branch("sDetectedRatio",&sDetectedRatio,"sDetectedRatio/D"))->SetTitle("Detected Scint ratio, a.u.");
  (tree->Branch("sDetectedRatioError",&sDetectedRatioError,"sDetectedRatioError/D"))->SetTitle("DSR, a.u.");
  (tree->Branch("cDetectedRatio",&cDetectedRatio,"cDetectedRatio/D"))->SetTitle("DCR, a.u.");
  (tree->Branch("sDetectedMean",&sDetectedMean,"sDetectedMean/D"))->SetTitle("Mean detected scint photons");
  (tree->Branch("sDetectedRMS",&sDetectedRMS,"sDetectedRMS/D"))->SetTitle("MDSP RMS");
  (tree->Branch("cDetectedMean",&cDetectedMean,"cDetectedMean/D"))->SetTitle("Mean detected Chere photons");
  (tree->Branch("cDetectedRMS",&cDetectedRMS,"cDetectedRMS/D"))->SetTitle("MDCP RMS");

  for (int i=0; i < fileNames->GetSize(); i++){
    // Get filename from list of selected files
    TObject* obj = fileNames->At(i);
    if (!obj->InheritsFrom(TObjString::Class())) continue;
    TObjString *objString = (TObjString*)obj;
    const char* filename = objString->GetString().Data();

    // Plot fractions of the emitted and detected OP on the same graph
    plotEmittedVsDetected(filename);
    plotPEHists(filename);

    // Plot and obtain ratios for emitted and detected
    std::vector<Double_t> vector = plotCherVsScint(filename);
    // std::vector<Double_t> vector = {1,2,3};

    // Calculate scintillation and Cherenkov photon ratios, write tree
    energy = getIncidentParticleEnergy(filename)/1000; // GeV
    events = getNumberOfEvents(filename);
    sToCEmitted = vector[0];
    sEmittedRatio = sToCEmitted/(sToCEmitted+1);
    cEmittedRatio = 1-sEmittedRatio;
    Double_t sToCError = vector[1];
    sEmittedRatioError = 1/(sToCEmitted+1)/(sToCEmitted+1)*sToCError; // Partial derivatives error

    // Get Absolute values of Scintillation to Cherenkov photons per MeV
    std::pair<Double_t, Double_t> sEmitDist = getBranchDistributionPerMeV(filename, "prod_op_scint");
    sEmittedMean = sEmitDist.first;
    sEmittedRMS = sEmitDist.second;

    std::pair<Double_t, Double_t> cEmitDist = getBranchDistributionPerMeV(filename, "prod_op_chere");
    cEmittedMean = cEmitDist.first;
    cEmittedRMS = cEmitDist.second;

    tree->Fill();
  }

  // Save tree to disk for Gnuplot
  // const char* firstFileName = ((TObjString*)fileNames->At(0))->String().Data();
  // TString filename = getMaterialName(firstFileName); filename += "-";
  // filename += getAssemblySize(firstFileName); filename += "-";
  // filename += getIncidentParticleType(firstFileName); filename += "-";
  // filename += getIncidentParticlePosition(firstFileName); filename += "-";
  // filename += "-scint-cher.dat";

  TTree* treeSorted = sortTree(tree, "energy");
  exportTreeToGnuplot(treeSorted, "scint-cher.dat");

  return 1;
}

#ifndef __CINT__

int main(int argc, char **argv) {
  // Instantiate TApplication
  TApplication* app = new TApplication("energyResolution", &argc, argv);

  cherVsScint();

  // Enter the event loop
  app->Run();
  // Return success
  return 0;
}

#endif
