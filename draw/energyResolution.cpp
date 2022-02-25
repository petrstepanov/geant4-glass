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
#include <sstream>
#include <fstream>
#include <functional>

#include "include/utilities.h"

enum class FitFunction {
  CBALL,
  INV_CBALL,
  GAUSS,
  MYGAUSS
};

const char* fitFunctionToString(FitFunction fitFunction){
  if (fitFunction == FitFunction::CBALL) return "cball";
  if (fitFunction == FitFunction::INV_CBALL) return "inv_cball";
  if (fitFunction == FitFunction::GAUSS) return "gauss";
  return "mygauss";
}

void addTextToStats(TVirtualPad* pad, const char* text){
  // Retrieve the stat box
  TPaveStats *stats = getPadStats(pad);
  if (!stats) return;

  TList *listOfLines = stats->GetListOfLines();

  // Note that "=" is a control character
  TText *newLine = new TLatex(0, 0, text);
  newLine->SetTextAlign(0);
  newLine->SetTextFont(0);
  newLine ->SetTextSize(0);
  newLine->SetTextColor(0);
  listOfLines->Add(newLine);

  pad->Modified();
}

Double_t getWeightedIntegral(TH1* hist){
  Double_t weightedIntegral = 0;
  for (Int_t i = 1; i <= hist->GetXaxis()->GetNbins(); i++){
    weightedIntegral += hist->GetBinCenter(i)*hist->GetBinContent(i);
  }
  return weightedIntegral;
}

// ----------------------------------
// Plot energy deposition in crystals
// ----------------------------------

int plotCrystalEdep(const char *fileName){
  TFile *file = openFile(fileName);

  // Obtain number of crystals from file
  TVector2* nCrystals = (TVector2*) file->Get("crystalsNumber");
  const Int_t nHists = nCrystals->X() * nCrystals->Y();
  std::cout << "Crystal assembly size: " <<  nCrystals->X() << "x" << nCrystals->Y() << std::endl;

  // Obtain incident particle energy
  Double_t particleEnergy = ((RooConstVar*)file->Get("gpsParticleEnergy"))->getVal();

  // Instantiate histograms to be saved in ROOT file
  TH1D **edepHist = new TH1D*[nHists];
  for (Int_t i = 0; i < nHists; i++){
    TString edepHistName = TString::Format("edepHist_%d", i);
    TString edepHistTitle = TString::Format("Energy Deposition in Crystal %d", i+1);
    edepHist[i] = new TH1D(edepHistName.Data(), edepHistTitle.Data(), 100, 0, particleEnergy);
    edepHist[i]->GetXaxis()->SetTitle("Deposited energy, MeV");
    edepHist[i]->GetYaxis()->SetTitle("Counts");
  }

  TTree* tree = getTree(file, "tree_crystals");
  if (tree == NULL) return 0;

  // Do not process files smaller that 100 events
  if ((Int_t)tree->GetEntries() < 100) return 0;

  // Try opening branches
  // TBranch* branch = tree->GetBranch("edep");
  // if (!branch){
  //   std::cout << "Branch \"edep\" could not be initialized. Skipping." << std::endl;
  //   return 0;
  // }

  // Every branch contains an array with values for each PMT event
  Double_t* edep = new Double_t[nHists];
  Double_t* totalEdep = new Double_t[nHists];
  for(Int_t j = 0 ; j < nHists ; j++){
    totalEdep[j] = 0;
  }

  tree->SetBranchAddress("edep", edep);
  for (Int_t i = 0; i < (Int_t)tree->GetEntries(); i++){
    tree->GetEntry(i);
    for(Int_t j = 0 ; j < nHists ; j++){
      edepHist[j]->Fill(edep[j]);
      totalEdep[j] += edep[j];
    }
  }

  // Calculate total deposited energy
  // TODO: check if correct
  Double_t totalDepositedEnergy = 0;
  for (Int_t i = 0; i < nHists; i++){
    // std::cout << "Crystal " << i << ": " << totalEdep[i] << " MeV" << std::endl;
    totalDepositedEnergy += totalEdep[i];
  }

  // Get number of events, GPS particle energy and particle name
  Int_t numberOfEvents = ((RooConstVar*)file->Get("numberOfEvents"))->getVal();
  const char* particleName = ((TObjString*)file->Get("gpsParticleName"))->GetName();
  const char* crystalMaterial = ((TObjString*)file->Get("crystalMaterial"))->GetName();
  TVector2* crystalsNumber = (TVector2*)file->Get("crystalsNumber");
  TVector3* crystalSize = (TVector3*)file->Get("crystalSize");
  Double_t totalGPSEnergy = numberOfEvents*particleEnergy;

  // Instantiate the Canvas
  std::stringstream buffer;
  buffer << "Energy deposition in " << (Int_t)crystalsNumber->X() << "x" << (Int_t)crystalsNumber->Y() <<
      ", " << crystalSize->X() << "x" << crystalSize->Y() << "x" << crystalSize->Z() << " mm " <<  crystalMaterial <<
      " crystal assembly";
  std::string canvasTitle = buffer.str();
  buffer.str( std::string() ); buffer.clear();

  EnergyValueUnit evu = formatEnergy(particleEnergy);
  EnergyValueUnit evuTotal = formatEnergy(totalGPSEnergy);
  EnergyValueUnit evuCrystals = formatEnergy(totalDepositedEnergy);
  buffer << "Injected " << numberOfEvents << " events of " << evu.value << " " << evu.unit << " " << particleName << ". "
         << "Total kinetic energy " << evuTotal.value << " " << evuTotal.unit << ". "
         << "Deposited in Crystals " << evuCrystals.value << " " << evuCrystals.unit << " "
         << "(" << totalDepositedEnergy/totalGPSEnergy*100 << "%).";
  std::string canvasSubTitle = buffer.str();
  buffer.str( std::string() ); buffer.clear();

  TCanvas* canvas = new TCanvas(); //"crystalEdepCanvas", "", 1024, 800
  canvas->SetWindowSize(1024, 800);
  canvas->Divide(nCrystals->X(),nCrystals->Y());

  gStyle->SetPalette(1);

  for (Int_t i = 0; i < nHists; i++){
    TVirtualPad* pad = canvas->cd(i+1);
    edepHist[i]->Draw();
    edepHist[i]->SetFillColor(kCyan);
    alignStats(pad);
    // TString text1 = TString::Format("%.1f MeV deposited", totaledep[i]);
    Double_t percentDepositedEnergy = totalEdep[i]/totalDepositedEnergy*100.;
    /*EnergyValueUnit*/ evu = formatEnergy(totalEdep[i]);

    // std::stringstream buffer2;
    buffer << "E Dep = " << evu.value << " " << evu.unit << ", " << percentDepositedEnergy << "%";
    std::string line = buffer.str();
    buffer.str( std::string() ); buffer.clear();

    addTextToStats(pad, line.c_str());

    Bool_t centerCrystal = (nHists == 1 && i == 0) || (nHists == 9 && i == 4) || (nHists == 25 && i == 12);
    alignStats(pad, centerCrystal ? Alignment::TOP_LEFT : Alignment::TOP_RIGHT, Decoration::DEFAULT, 0.07, 0.5);
  }

  // Write global title for first Canvas
  std::cout << "Test" << std::endl;
  std::cout << canvasTitle << std::endl;
  std::cout << canvasSubTitle << std::endl;
  addCanvasTitle(canvas, canvasTitle.c_str(), canvasSubTitle.c_str());

  // Save canvas to file
  TString fileNameOnly = removeFileExtension(fileName);
  canvas->SaveAs((fileNameOnly+"-edep.png").Data());

  return 0;
}

// ----------------------------------
// Plot energy deposition in crystals
// ----------------------------------

TList* buildOpticalResolutionHists(TList* files){
  TList* peFromSinglePhotonHistList = new TList();

  // Make sure number of unit Volumes match number of opened files
  // TODO: write number of segments to ROOT file
  Int_t nUnitVolumes = 10;
  if (files->GetSize() != nUnitVolumes) {
    std::cout << "Error: number of files not equal to number of unit volumes" << std::endl;
    exit(1);
  }
  files->Sort();
  files->Print();

  TIter next(files);
  {
    Int_t i = 0;
    while (TObjString* el = dynamic_cast<TObjString *>(next())) {
      TFile* file = new TFile(el->GetString().Data());
      TString s = TString::Format("opResHist%d", i++);
      TH1* hist = new TH1D(s.Data(), s.Data(), 100, 0, 0.27);

      TTree* tree = (TTree*)file->Get("tree_optical");

      TString varexp = TString::Format("pe>>%s", hist->GetName());
      tree->Draw(varexp.Data(), "", "goff");

      // TODO - figure why below reading from histogram does not work!
  //    Double_t pe = 0;
  //    tree->SetBranchAddress("pe", &pe);
  //
  //    Int_t nEntries = (Int_t)tree->GetEntries();
  //    std::cout << "Tree has " << nEntries << " entries" << std::endl;
  //
  //    for (Int_t n = 0; n < nEntries; n++) {
  //       tree->GetEntry(n);
  //       hist->Fill(pe);
  //    }

      peFromSinglePhotonHistList->Add(hist);
    }
  }
  // Plot histograms with optical output
  TCanvas* c = new TCanvas("opticalResolution", "Optical Output from Unit Volumes");
  c->SetLogy();

  Int_t nHists = peFromSinglePhotonHistList->GetSize();
  for (int i = 0; i < nHists; i++){
    TH1* hist = static_cast<TH1D*>(peFromSinglePhotonHistList->At(i));

    // Set histogram color
    // Double_t alpha = (i+1.)/(nHists+1);
    // hist->SetLineColorkBlack, alpha);

    if (i == 0){
      hist->Draw();
    }
    else {
      hist->Draw("SAME");
    }
  }
  alignLegend(c);

  return peFromSinglePhotonHistList;
}

TVector3* calcOpticalResolution(const char *fileName, TList* unitVolumesFilesList, FitFunction function = FitFunction::GAUSS){
  TFile *file = openFile(fileName);

  // Check Tree exists and not empty
  TTree* tree = getTree(file, "tree_unit_volumes");
  if (tree == NULL){
    std::cout << "  \"tree_unit_volumes\" not fount in TFile." << std::endl;
    return NULL;
  }
  if (tree->GetEntries() == 0){
    std::cout << "  \"tree_unit_volumes\" is empty in TFile." << std::endl;
    return NULL;
  }

  // Obtain number of crystals from file
  TVector2* crystalsNumber = (TVector2*) file->Get("crystalsNumber");
  const Int_t nCrystals = crystalsNumber->X() * crystalsNumber->Y();
  std::cout << "Crystal assembly size: " <<  crystalsNumber->X() << "x" << crystalsNumber->Y() << std::endl;

  // Obtain number of OP/event for given material
  const char* crystalMaterial = ((TObjString*)file->Get("crystalMaterial"))->GetName();
  TString materialFileName = TString::Format("./materials/%s.root", crystalMaterial);
  TFile* materialFile = new TFile(materialFileName.Data());
  TVectorD* scintillationYield = (TVectorD*)(materialFile->Get("SCINTILLATIONYIELD"));
  scintillationYield->Print();
  Double_t sY = (*scintillationYield)[0];
  std::cout << "Scintillation yield " << crystalMaterial << ": " << sY << " OP/MeV." << std::endl;

  // Obtain maximum PE efficiency
//  const char* detectorName = ((TObjString*)file->Get("detectorName"))->GetName();
//  TString detectorFileName = TString::Format("./detectors/%s.root", detectorName);
//  TFile* detectorFile = new TFile(detectorFileName.Data());
//  TGraph* efficiencyGraph = (TGraph*)(materialFile->Get("efficiency"));
//  // efficiency->Print();
//  Double_t maxEfficiency = getGraphMax(efficiencyGraph);
//  std::cout << "Max efficiency " << detectorName << ": " << maxEfficiency << " %." << std::endl;


  // Optical resolution histograms with optical output from each unit volume
  TList* peResHists = buildOpticalResolutionHists(unitVolumesFilesList);
  // Plot histograms with optical output
  // Maximum PE output from 1 photon is ~ 25% (0.25). Therefore from ~ sY it will be ~ 0.25*sY

  // Instantiate histograms to be saved in ROOT file
  Double_t particleEnergy = ((RooConstVar*)file->Get("gpsParticleEnergy"))->getVal();

  // TODO: parameterize in TFile
  Int_t nUnitVolumes = 10;
  // Array of unit volume energies to be read from Tree
  Double_t* unitVolumeEnergies = new Double_t[nCrystals*nUnitVolumes];

  // Processing the energy file and optical resolutions files
  tree->SetBranchAddress("unitVolumeEnergies", unitVolumeEnergies);

  // Determine average pe Value to set proper histogram limits
  Int_t testNEvents = 100;
  Double_t avgPeFromAllCrystals = 0;
  for (Int_t i = 0; i < testNEvents; i++){
    tree->GetEntry(i);

    Double_t peFromAllCrystals = 0; // Total optical output for fitting
    for (Int_t j = 0 ; j < nCrystals ; j++){
      // Iterate nuit volumes and sum all pe that reached detector
      Double_t eInCrystal = 0;
      Double_t opInCrystal = 0;
      Double_t peInCrystal = 0;
      for (Int_t unitVolumeIndex = 0; unitVolumeIndex < nUnitVolumes; unitVolumeIndex++){
        Double_t energyInUnit = unitVolumeEnergies[j*(nUnitVolumes) + unitVolumeIndex];
        Int_t opInUnit = sY*energyInUnit; // number of emitted op in unit volume
        eInCrystal += energyInUnit;
        opInCrystal += opInUnit;
        TH1D* unitVolumeHistPE = static_cast<TH1D*>(peResHists->At(unitVolumeIndex));
        for (Int_t op = 0; op < opInUnit; op++){
          peInCrystal += unitVolumeHistPE->GetRandom();
        }
      }
      // std::cout << "  Crystal " << j << ": " << eInCrystal << "MeV, "<< opInCrystal << " OP, " << peInCrystal << " PE." << std::endl;

      peFromAllCrystals += peInCrystal;
    }
    avgPeFromAllCrystals+=peFromAllCrystals;
  }
  avgPeFromAllCrystals = avgPeFromAllCrystals/(Double_t)testNEvents;

  // Populate NxN histiograms and PE hist for fitting
  Double_t nBins = 2000;
  Double_t peHistMax = avgPeFromAllCrystals*1.25; // Roughly the average

  TH1D **peHist = new TH1D*[nCrystals];
  for (Int_t i = 0; i < nCrystals; i++){
    TString edepHistName = TString::Format("peHist_%d", i);
    TString edepHistTitle = TString::Format("Optical Output in Crystal %d", i+1);
    peHist[i] = new TH1D(edepHistName.Data(), edepHistTitle.Data(), nBins, 0, peHistMax);
    peHist[i]->GetXaxis()->SetTitle("Number of photo-electrons");
    peHist[i]->GetYaxis()->SetTitle("Counts");
  }

  // Instantiate histogram for optical resolution
  TH1D* peHistAll = new TH1D("peHistAll", "Total Optical from all Crystals per Event", nBins, peHistMax/2., peHistMax);
  peHistAll->GetXaxis()->SetTitle("Number of photo-electrons");
  peHistAll->GetYaxis()->SetTitle("Counts");

  // Do not process files smaller that 100 events
//  if ((Int_t)tree->GetEntries() < 100){
//    std::cout << "Tree has less than 100 entries" << std::endl;
//    return 0;
//  }

  // TODO: try different TRandom objects, compare speed.
  // https://root-forum.cern.ch/t/th1-getrandom-same-events-all-the-time/8469/4
  gRandom = new TRandom3(0);

  Int_t absMaxEvents = 20000;
  Int_t maxEvents = TMath::Min(absMaxEvents, (Int_t)tree->GetEntries());

  for (Int_t i = 0; i < maxEvents; i++){
    if (i%1000 == 0){
      std::cout << "Processing event " << i << "+..." << std::endl;
    }
    tree->GetEntry(i);

    Double_t peFromAllCrystals = 0; // Total optical output for fitting
    for (Int_t j = 0 ; j < nCrystals ; j++){
      // Iterate nuit volumes and sum all pe that reached detector
      Double_t eInCrystal = 0;
      Double_t opInCrystal = 0;
      Double_t peInCrystal = 0;
      for (Int_t unitVolumeIndex = 0; unitVolumeIndex < nUnitVolumes; unitVolumeIndex++){
        Double_t energyInUnit = unitVolumeEnergies[j*(nUnitVolumes) + unitVolumeIndex];
        Int_t opInUnit = sY*energyInUnit; // number of emitted op in unit volume
        eInCrystal += energyInUnit;
        opInCrystal += opInUnit;
        TH1D* unitVolumeHistPE = static_cast<TH1D*>(peResHists->At(unitVolumeIndex));
        for (Int_t op = 0; op < opInUnit; op++){
          peInCrystal += unitVolumeHistPE->GetRandom();
        }
      }
      // std::cout << "  Crystal " << j << ": " << eInCrystal << "MeV, "<< opInCrystal << " OP, " << peInCrystal << " PE." << std::endl;

      peHist[j]->Fill(peInCrystal);
      peFromAllCrystals += peInCrystal;
    }
    peHistAll->Fill(peFromAllCrystals);
  }

  // Get number of events, GPS particle energy and particle name
  Int_t numberOfEvents = ((RooConstVar*)file->Get("numberOfEvents"))->getVal();
  const char* particleName = ((TObjString*)file->Get("gpsParticleName"))->GetName();
  TVector3* crystalSize = (TVector3*)file->Get("crystalSize");

  // Instantiate the Canvas
  std::stringstream buffer;
  buffer << "Photo-electron output from "
         << crystalsNumber->X() << "x" << crystalsNumber->Y() << ", "
         << crystalSize->X() << "x" << crystalSize->Y() << "x" << crystalSize->Z()
         << crystalMaterial << " crystal assembly";
  std::string canvasTitle = buffer.str();

  buffer.str( std::string() ); buffer.clear();

  EnergyValueUnit evu = formatEnergy(particleEnergy);
  buffer << "Injected " << numberOfEvents << " events of " << evu.value << " " << evu.unit << " " << particleName;
  std::string canvasSubTitle = buffer.str();

  TCanvas* canvas = new TCanvas();
  canvas->SetWindowSize(1024, 800);
  canvas->Divide(crystalsNumber->X(),crystalsNumber->Y());

  gStyle->SetPalette(1);

  for (Int_t i = 0; i < nCrystals; i++){
    TVirtualPad* pad = canvas->cd(i+1);
    peHist[i]->Draw();
    peHist[i]->SetFillColor(kCyan);
    alignLegend(pad);
  }

  // Write global title for first Canvas
  addCanvasTitle(canvas, canvasTitle.c_str(), canvasSubTitle.c_str());

  // Save canvas to file
  TString fileNameOnly = removeFileExtension(fileName);
  canvas->SaveAs((fileNameOnly+"-pe.png").Data());

  // Fit and plot optical resolution
  TCanvas* opResCanvas = new TCanvas();
  opResCanvas->SetGrid();

  Double_t m, Dm, s, Ds;

  // Fit with ROOT gauss
  // TODO: use crystal Ball function unstead!
  peHistAll->Fit("gaus"); //, "", "", 5000, edepHist->GetXaxis()->GetXmax()); // "0" - Fit with gaus, do not plot the result of the fit
  m = peHistAll->GetFunction("gaus")->GetParameter(1); // mean
  Dm = peHistAll->GetFunction("gaus")->GetParError(1); // mean error
  s = peHistAll->GetFunction("gaus")->GetParameter(2); // sigma
  Ds = peHistAll->GetFunction("gaus")->GetParError(2); // sigma error

  // Calculate resolution value and error
  Double_t r = s/m*100;  // Resolution
  Double_t Dr = 100*TMath::Sqrt(TMath::Power(1/m*Ds,2)+TMath::Power(s/m/m*Dm,2)); // Indirect error

  std::cout << "Resolution: " << r << " " << Dr << std::endl;

  // Plot histogram and fit
  gStyle->SetOptStat(10);
  peHistAll->Draw();
  peHistAll->SetFillColor(kCyan);
  TString meanString = TString::Format("Mean, MeV = %.1f #pm %.1f", m, Dm);
  addTextToStats(opResCanvas, meanString.Data());
  TString sigmaString = TString::Format("Sigma, MeV = %.1f #pm %.1f", s, Ds);
  addTextToStats(opResCanvas, sigmaString.Data());
  TString resolutionString = TString::Format("Resolution, %% = %.1f #pm %.1f", r, Dr);
  addTextToStats(opResCanvas, resolutionString.Data());
  alignLegend(opResCanvas, Alignment::TOP_RIGHT, Decoration::DEFAULT, 0.09);

  // Save canvas with energy resolution fitting
  // TODO: when move from gauss change next line
  TString canvasFilename = TString::Format("%s-optres-%s", removeFileExtension(fileName).Data(), "gauss");
  saveCanvasToDisk(canvas, canvasFilename.Data());

  // Save single energy resolution point to ASCII
  TString asciiFilename = TString::Format("%s-optres.dat", removeFileExtension(fileName).Data());
  std::vector<std::string> colNames = {"Incident Energy, GeV", "Optical Resolution, %", "Error, %"};
  std::vector<double> values = {particleEnergy/1000., r, Dr};
  exportPointToGnuplot(asciiFilename, colNames, values);


  return new TVector3(particleEnergy/1000., r, Dr);
}


// ---------------------------------
// Plot particles escaping the world
// ---------------------------------

void plotEscapeParticles(const char* fileName){
  TFile* file = openFile(fileName);
  TTree *tree = getTree(file, "tree_escape");
  if (tree == NULL) return;
  if (tree->GetEntries() == 0) return;

  // Get number of events, GPS particle energy and particle name
  Double_t particleEnergy = ((RooConstVar*)file->Get("gpsParticleEnergy"))->getVal();
  Int_t numberOfEvents = ((RooConstVar*)file->Get("numberOfEvents"))->getVal();
  const char* particleName = ((TObjString*)file->Get("gpsParticleName"))->GetName();
  const char* crystalMaterial = ((TObjString*)file->Get("crystalMaterial"))->GetName();
  TVector2* crystalsNumber = (TVector2*)file->Get("crystalsNumber");
  TVector3* crystalSize = (TVector3*)file->Get("crystalSize");
  Double_t totalGPSEnergy = numberOfEvents*particleEnergy;

  // Set Marker Style
  TCanvas* canvas = new TCanvas("escapeCanvas", "", 1280, 520);
  canvas->Divide(3, 1);

  // PAD 1: Draw world escape locations
  tree->SetMarkerStyle(6);
  TVirtualPad* pad1 = canvas->cd(1);
  // Int_t maxEntries = TMath::Min((Double_t)tree->GetEntries(), 1.E4);
  // tree->Draw("x:y:z:energy", "", "COLZ", maxEntries);
  pad1->SetMargin(0.15, 0.2, 0.15, 0.15);
  gStyle->SetPalette(kTemperatureMap);
  tree->Draw("y:x:z:energy", "", "COLZ");
  TH1* htemp1 = (TH1*) pad1->GetListOfPrimitives()->FindObject("htemp");
  // ROOT bug:
  htemp1->SetTitle("Escape Locations;Z, mm; X, mm; Energy, MeV");
  pad1->Update();
  TPaletteAxis *palette = (TPaletteAxis*)htemp1->GetListOfFunctions()->FindObject("palette");
  palette->SetX1NDC(0.85);
  palette->SetX2NDC(0.9);
  palette->SetTitleOffset(-1.5);
  htemp1->GetXaxis()->SetTitleOffset(2);
  htemp1->GetYaxis()->SetTitleOffset(2);
  htemp1->GetZaxis()->SetTitleOffset(2);
  htemp1->GetZaxis()->SetTitle("Y, mm");

  // PAD 2: Draw Energy distribution
  TVirtualPad* pad2 = canvas->cd(2);
  pad2->SetLogy();
  gStyle->SetOptStat(10); // https://root.cern.ch/doc/master/classTPaveStats.html
  tree->Draw("energy");
  TH1* htemp2 = (TH1*) pad2->GetListOfPrimitives()->FindObject("htemp");
  htemp2->SetTitle("Kinetic Energy Distribution;Kinetic energy, MeV;Events");
  htemp2->SetFillColor(kCyan);
  alignLegend(pad2, Alignment::TOP_RIGHT, Decoration::DEFAULT, 0.09, 0.5);

  Double_t escapeEnergy = 0; // getWeightedIntegral(htemp2);
  Double_t energy;
  tree->SetBranchAddress("energy",&energy);
  for (Int_t i = 0; i<(Int_t)tree->GetEntries(); i++) {
    tree->GetEntry(i);
    escapeEnergy+= energy;
  }
  std::stringstream buffer;
  buffer << "Particles escaped the simulation for " << (Int_t)crystalsNumber->X() << "x" << (Int_t)crystalsNumber->Y() <<
      ", " << crystalSize->X() << "x" << crystalSize->Y() << "x" << crystalSize->Z() << " mm " <<  crystalMaterial <<
      " crystal assembly per event, " << particleEnergy/1E3 << " GeV " << particleName << ".";

  // buffer.str( std::string() ); buffer.clear();
  // buffer << numberOfEvents << " events";

  EnergyValueUnit escapeEnergyEVU = formatEnergy(escapeEnergy);
  TString escapeEnergyString = TString::Format("Total E = %.2f %s", escapeEnergyEVU.value, escapeEnergyEVU.unit.c_str());
  addTextToStats(pad2, escapeEnergyString.Data());
  TString canvasTitle = buffer.str().c_str();
  canvas->SetTitle(canvasTitle.Data());

  // PAD 3: Draw PDG
  /*TVirtualPad* pad3 =*/ canvas->cd(3);
  Int_t pdgMax =  std::numeric_limits<int>::min();
  Int_t pdgMin = std::numeric_limits<int>::max();
  Int_t pdg;
  tree->SetBranchAddress("pdg",&pdg);
  for (Int_t i = 0; i < (Int_t)tree->GetEntries(); i++) {
    tree->GetEntry(i);
    if (pdg > pdgMax) pdgMax = pdg;
    if (pdg < pdgMin) pdgMin = pdg;
  }
  TH1* pdgHist = new TH1D("pdgHist", "Particle Type (PDG)", pdgMax - pdgMin + 1, pdgMin - 0.5, pdgMax + 0.5);
  for (Int_t i = 0; i < (Int_t)tree->GetEntries(); i++) {
    tree->GetEntry(i);
    pdgHist->Fill(pdg);
  }
  // Make pie chart
  TMap* pdgNameMap = (TMap*)file->Get("pdgNameMap");
  pdgNameMap->Print();
  std::vector<const char*> particleNames = {};
  std::vector<double> binValues = {};
  for (Int_t i = 1; i <= pdgHist->GetXaxis()->GetNbins(); i++){
    if (pdgHist->GetBinContent(i)!=0){
      TString* label = new TString();
      label->Form("%d", (int)pdgHist->GetBinCenter(i));
      TObjString* objString = (TObjString*)pdgNameMap->GetValue(label->Data());
      particleNames.push_back(objString->GetName());
      std::cout << label->Data() << std::endl;
      binValues.push_back(pdgHist->GetBinContent(i));
    }
  }
  Int_t colors[] = {kGray, kOrange + 1, kSpring - 5, kAzure + 8, kPink + 1, kViolet - 4, kRed - 7, kViolet + 6};
  TPie *pie4 = new TPie("pie4", "Particle IDs", (int)binValues.size(), &binValues[0], colors, &particleNames[0]);
  pie4->SetRadius(.2);
  // pie4->SetLabelFormat("%txt: %val (%perc)");
  pie4->SetLabelFormat("%txt");
  pie4->SetTextSize(0.04);

  pie4->Draw("3dnol");
  TLegend* legend = new TLegend(0.5,0.5,0.9,0.9,"");
  for (Int_t i = 0; i < pie4->GetEntries(); ++i) {
    Double_t pieValue = pie4->GetSlice(i)->GetValue();
    TString text = TString::Format("%s (%d particles, %.1f %%)", pie4->GetSlice(i)->GetTitle(), (int)pieValue, pieValue/tree->GetEntries()*100);
    legend->AddEntry(pie4->GetSlice(i),text.Data(),"f");
  }
  legend->SetX1(0.1);
  Double_t legendBottom = 0.9-0.06*legend->GetNRows();
  legend->SetY1(legendBottom);
  legend->SetX2(.9);
  legend->SetY2(.9);
  legend->Draw();
  pie4->SetY((1-legendBottom)*3/4);
  pie4->SetTitle("Particle Types and Ratios");

  // Add title to canvas2
  EnergyValueUnit evuTotal = formatEnergy(totalGPSEnergy);
  EnergyValueUnit evuEscape = formatEnergy(escapeEnergy);
  TString subtitle = TString::Format("Injected Kinetic Energy %.2f %s. Escaped Kinetic Energy %.2f %s (%.2f %%)", evuTotal.value, evuTotal.unit.c_str(), evuEscape.value, evuEscape.unit.c_str(), escapeEnergy/totalGPSEnergy*100);
  addCanvasTitle(canvas, canvas->GetTitle(), subtitle.Data());

  // Save canvas
  TString fileNameOnly = removeFileExtension(fileName);
  canvas->SaveAs((fileNameOnly+"-eesc.png").Data());
}


// ---------------------------------
// Plot energy resolution
// ---------------------------------


class EresFunctionObject {
  public:
		EresFunctionObject(){
      // if option "width" is specified, the integral is the sum of
      // the bin contents multiplied by the bin width in x.
      // this->normalization = hist->Integral("width");
    }

    Double_t operator() (double *_x, double *par) {
      Double_t x = _x[0];
      Double_t a = par[0];
      Double_t b = par[1];

      return a + b/sqrt(x);
    }

  private:
//    Double_t normalization;
};

TVector3* calcEnergyResolution(const char* fileName, FitFunction fitFunction){
  TFile* file = openFile(fileName);
  TTree *tree = getTree(file, "tree_crystals");
  if (tree == NULL) return 0;

  // Do not process files smaller that 100 events
  if ((Int_t)tree->GetEntries() < 100) return 0;

  // Get number of events, GPS particle energy and particle name
  Double_t particleEnergy = ((RooConstVar*)file->Get("gpsParticleEnergy"))->getVal();
  /* Int_t numberOfEvents = ((RooConstVar*)file->Get("numberOfEvents"))->getVal(); */
  const char* particleName = ((TObjString*)file->Get("gpsParticleName"))->GetName();
  const char* crystalMaterial = ((TObjString*)file->Get("crystalMaterial"))->GetName();
  TVector3* crystalSize = (TVector3*)file->Get("crystalSize");
  TVector2* crystalsNumber =  (TVector2*)file->Get("crystalsNumber");
  /* Double_t totalGPSEnergy = numberOfEvents*particleEnergy; */

  // Estimate energy histogram lower and upper ranges
  // Obtain ranges of energy deposited per one event
  const Int_t nCrystals = crystalsNumber->X()*crystalsNumber->Y();
  // Double_t edep[nCrystals];
  Double_t* edep = new Double_t[nCrystals];
  tree->SetBranchAddress("edep", edep);
  Double_t minEdep = particleEnergy;
  Double_t maxEdep = 0;
  for (Int_t i = 0; i < (Int_t)tree->GetEntries(); i++){
    // Get tree entry
    tree->GetEntry(i);
    // Each event we calculate the total energy deposited in all crystals
    Double_t totalEdep = 0;
    for(Int_t j = 0 ; j < nCrystals ; j++){
      totalEdep += edep[j];
    }
    if (totalEdep > maxEdep) maxEdep = totalEdep;
    if (totalEdep < minEdep) minEdep = totalEdep;
  }

  // Instantiate histogram for total energy deposition
  std::stringstream buffer;
  buffer << "Total energy deposition in " << (Int_t)crystalsNumber->X() << "x" << (Int_t)crystalsNumber->Y() <<
      ", " << crystalSize->X() << "x" << crystalSize->Y() << "x" << crystalSize->Z() << " mm " <<  crystalMaterial <<
      " crystal assembly per event, " << particleEnergy/1E3 << " GeV " << particleName << ".";

  // TString title = TString::Format("Total energy deposition in %dx%d, %dx%dx%d %s crystal assembly per event, %.0f GeV %s.", (Int_t)crystalsNumber->X(), (Int_t)crystalsNumber->Y(), (Int_t)crystalSize->X(), (Int_t)crystalSize->Y(), (Int_t)crystalSize->Z(), crystalMaterial, particleEnergy/1E3, particleName);
  TH1D* edepHist = new TH1D("totalEdepHist", buffer.str().c_str(), 250, minEdep, maxEdep*1.25);
  edepHist->GetYaxis()->SetTitle("Counts");
  edepHist->GetXaxis()->SetTitle("Total deposited energy, MeV");

  // Fill histogram from the tree
  for (Int_t i = 0; i < (Int_t)tree->GetEntries(); i++){
    // Get tree entry
    tree->GetEntry(i);
    // Each event we sum total energy deposited in all crystals
    Double_t totalEdep = 0;
    for(Int_t j = 0 ; j < nCrystals ; j++){
      totalEdep += edep[j];
    }
    edepHist->Fill(totalEdep);
  }
  edepHist->SetBinErrorOption(TH1::EBinErrorOpt::kNormal);

  // Should delete this heap after use
  // https://stackoverflow.com/questions/11379433/c-forbids-variable-size-array/11379442
  delete[] edep;

  TCanvas* canvas = new TCanvas();
  canvas->SetTitle(buffer.str().c_str());
  canvas->SetGrid();

  Double_t m, Dm, s, Ds;

  gStyle->SetOptFit(0);
  if (fitFunction == FitFunction::GAUSS){
    // Fit with ROOT gaus
    std::cout << "Fitting with GAUSS" << std::endl;
    edepHist->Fit("gaus"); //, "", "", 5000, edepHist->GetXaxis()->GetXmax()); // "0" - Fit with gaus, do not plot the result of the fit
    m = edepHist->GetFunction("gaus")->GetParameter(1); // mean
    Dm = edepHist->GetFunction("gaus")->GetParError(1); // mean error
    s = edepHist->GetFunction("gaus")->GetParameter(2); // sigma
    Ds = edepHist->GetFunction("gaus")->GetParError(2); // sigma error
  }
  else if (fitFunction == FitFunction::MYGAUSS){
    // Fit with my gauss
    std::cout << "Fitting with MYGAUSS" << std::endl;
    GaussFunctionObject* fGaussFunctionObject = new GaussFunctionObject(edepHist);
    TF1* fGauss = new TF1("fGauss", fGaussFunctionObject, edepHist->GetXaxis()->GetXmin(), edepHist->GetXaxis()->GetXmax(), 2);    // create TF1 class.

    fGauss->SetParName(0, "mean");
    fGauss->SetParameter(0, edepHist->GetBinCenter(edepHist->GetMaximumBin()));
    fGauss->SetParLimits(0, edepHist->GetXaxis()->GetXmin(), edepHist->GetXaxis()->GetXmax());

    fGauss->SetParName(1, "sigma");
    fGauss->SetParameter(1, 10);
    fGauss->SetParLimits(1, 0, edepHist->GetXaxis()->GetXmax()-edepHist->GetXaxis()->GetXmin());

    // printParameters(fGauss);
    edepHist->Fit(fGauss, "RW"); // ignore bin uncertanties?
    m = edepHist->GetFunction("fGauss")->GetParameter(0); // mean
    Dm = edepHist->GetFunction("fGauss")->GetParError(0); // mean error
    s = edepHist->GetFunction("fGauss")->GetParameter(1); // sigma
    Ds = edepHist->GetFunction("fGauss")->GetParError(1); // sigma error
  }
  else /* if (fitFunction == FitFunction::CBALL || fitFunction == FitFunction::INV_CBALL ) */{
    // Fit with crystal ball
    TF1* fBall = new TF1();
    if (fitFunction == FitFunction::CBALL){
      std::cout << "Fitting with CBALL" << std::endl;
      CrystalBallFunctionObject* fFunctionObject = new CrystalBallFunctionObject();
      fBall = new TF1("fBall", *fFunctionObject, edepHist->GetXaxis()->GetXmin(), edepHist->GetXaxis()->GetXmax(), 5);    // create TF1 class.
    }
    if (fitFunction == FitFunction::INV_CBALL){
      std::cout << "Fitting with INV_CBALL" << std::endl;
      InvCrystalBallFunctionObject* fFunctionObject = new InvCrystalBallFunctionObject();
      fBall = new TF1("invFBall", *fFunctionObject, edepHist->GetXaxis()->GetXmin(), edepHist->GetXaxis()->GetXmax(), 5);    // create TF1 class.
    }
    fBall->SetNpx(500);

    fBall->SetParName(0, "a");
    fBall->SetParameter(0, 1);
    fBall->SetParLimits(0, 0, 1E2);

    fBall->SetParName(1, "n");
    fBall->SetParameter(1, 10);
    fBall->SetParLimits(1, 1+1E-3, 500);

    fBall->SetParName(2, "mean");
    fBall->SetParameter(2, edepHist->GetBinCenter(edepHist->GetMaximumBin()));
    fBall->SetParLimits(2, edepHist->GetXaxis()->GetXmin(), edepHist->GetXaxis()->GetXmax());

    fBall->SetParName(3, "sigma");
    fBall->SetParameter(3, 500);
    fBall->SetParLimits(3, 0, 10000);

    fBall->SetParName(4, "norm");
    fBall->SetParameter(4, edepHist->Integral("width"));
    fBall->SetParLimits(4, edepHist->Integral("width")/2, edepHist->Integral("width")*2);

    // Abnormal termination of minimization.
    // Solution: increase the fitting range. Specify function range (-10000, 10000) and pass "R" parameter
    //           also use Long Double vs Double in the fitting function evaluate()

    edepHist->Fit(fBall, "W");

    m = edepHist->GetFunction("fBall")->GetParameter(2); // mean
    Dm = edepHist->GetFunction("fBall")->GetParError(2); // mean error
    s = edepHist->GetFunction("fBall")->GetParameter(3); // sigma
    Ds = edepHist->GetFunction("fBall")->GetParError(3); // sigma error
  }

  // Calculate resolution value and error
  Double_t r = s/m*100;  // Resolution
  Double_t Dr = 100*TMath::Sqrt(TMath::Power(1/m*Ds,2)+TMath::Power(s/m/m*Dm,2)); // Indirect error

  std::cout << "Resolution: " << r << " " << Dr << std::endl;

  // Plot histogram and fit
  gStyle->SetOptStat(10);
  edepHist->Draw();
  edepHist->SetFillColor(kCyan);

	buffer.str( std::string() ); buffer.clear();
	buffer << "Mean, MeV = " << m << " #pm " << Dm;
  addTextToStats(canvas, buffer.str().c_str());

	buffer.str( std::string() ); buffer.clear();
	buffer << "Sigma, MeV = " << s << " #pm " << Ds;
  addTextToStats(canvas, buffer.str().c_str());

	buffer.str( std::string() ); buffer.clear();
	buffer << "Resolution, %% = " << r << " #pm " << Dr;

	alignLegend(canvas, Alignment::TOP_LEFT, Decoration::DEFAULT, 0.07, 0.4);

  // Save canvas with energy resolution fitting
  TString canvasFilename = TString::Format("%s-eres-%s", removeFileExtension(fileName).Data(), fitFunctionToString(fitFunction));
  saveCanvasToDisk(canvas, canvasFilename);

  // Save single energy resolution point to ASCII
  TString asciiFilename = TString::Format("%s-eres.dat", removeFileExtension(fileName).Data());
  std::vector<std::string> colNames = {"Incident Energy, GeV", "Energy Resolution, %", "Error, %"};
  std::vector<double> values = {particleEnergy/1000., r, Dr};
  exportPointToGnuplot(asciiFilename, colNames, values);

  return new TVector3(particleEnergy/1000., r, Dr);
}

TGraph* getResolutionSeriesGraph(FitFunction fitFunction, TList* fileNames, TList* uvFileNames = NULL){
	// Create vectors to instantiate TGraph later from them
	std::vector<Double_t> energy = {};
	std::vector<Double_t> resolution = {};
	std::vector<Double_t> resolutionErr = {};

	// Process every file in the list of user input files
	for (TObject* obj : *fileNames){
  	if (!obj->InheritsFrom(TObjString::Class())) continue;
  	TObjString *objString = (TObjString*)obj;
  	const char* fileName = objString->GetString().Data();

    // Plot energy deposition in crystals
    plotCrystalEdep(fileName);

    // Plot escape particles list
    plotEscapeParticles(fileName);

    TVector3* resolutionPoint;
    if (uvFileNames == NULL){
      // Calculate and energy resolution
      resolutionPoint = calcEnergyResolution(fileName, fitFunction);
    }
    else {
      // Plot energy deposition in crystals
      resolutionPoint = calcOpticalResolution(fileName, uvFileNames, fitFunction);
    }
    if (resolutionPoint == NULL) continue;

    // Print obtained resolution to screen
    std::cout << (uvFileNames == NULL ? "Energy resolution point: " : "Optical resolution point: ") << std::endl;
    resolutionPoint->Print("V");

    energy.push_back(resolutionPoint->X());
    resolution.push_back(resolutionPoint->Y());
    resolutionErr.push_back(resolutionPoint->Z());
	}

	// Plot graph with energy resolution
	const char* firstFilename = ((TObjString *)fileNames->At(0))->GetString().Data();
	TFile* file = openFile(firstFilename);
	// Get number of events, GPS particle energy and particle name
	const char* crystalMaterial = ((TObjString*)file->Get("crystalMaterial"))->GetName();
	TVector3* crystalSize = (TVector3*)file->Get("crystalSize");
	TVector2* crystalsNumber =  (TVector2*)file->Get("crystalsNumber");

  TGraphErrors* gr = new TGraphErrors((int)energy.size(), &energy[0], &resolution[0], 0, &resolutionErr[0]); // convert vector to array

  // Set graph title and axis titles from info from the first file in the list
  std::stringstream buffer;
  buffer << (Int_t)crystalsNumber->X() << "x" << (Int_t)crystalsNumber->Y() << " " << crystalMaterial << " (nominal "
         << crystalSize->X()/10 << "x" << crystalSize->Y()/10 << "x" << crystalSize->Z()/10 << " cm^{3})" << ";";
  buffer << "Incident particle energy, GeV;";
  buffer << "Energy resolution, %;";
  gr->SetTitle(buffer.str().c_str());

  // Assign unique markers for every series
  static int markerStyle = EMarkerStyle::kFullCircle;
  gr->SetMarkerStyle(markerStyle);
  markerStyle++;

  // Sort points along X axis and draw the graph
  gr->Sort(&TGraph::CompareX);

  // Create TGraph with errors from above data
  TCanvas* canvas = new TCanvas();
  canvas->SetGrid();
  gr->Draw("AP");

  // Fit series if it contains more than three points
	if (gr->GetN() >= 3){
		// Define fitting function
		EresFunctionObject* eResFO = new EresFunctionObject();
		static int fitFunctionNumber = 1;
		TString fitFunctionName = "eRes";
		fitFunctionName += fitFunctionNumber++;
		TF1* eRes = new TF1(fitFunctionName, eResFO, gr->GetXaxis()->GetXmin(), gr->GetXaxis()->GetXmax(), 2);    // create TF1 class.
		eRes->SetParName(0, "a");
		eRes->SetParName(1, "b");

		// Output chi2 and fit the graph
		gStyle->SetOptFit(111);
		gr->Fit(eRes);

	  // Align legend - not works
		canvas->GetListOfPrimitives()->Print();
		alignStats(canvas, Alignment::TOP_RIGHT, Decoration::DEFAULT, 0.07, 0.3);
	}

	return gr;
}

int energyResolution(){
  // List of file names
  TList* eDepFilesList = NULL;
  TList* unitVolumesFilesList = NULL;
  // TList* unitVolumesOuterFilesList = NULL;

  // Read input files for energy deposition and optical output from unit volumes (optional)
  Bool_t isBatch = gApplication->Argc() > 1;
  if (isBatch){
    // Batch mode is for running on the farm (no GUI)
    // Parameter 1: path to directory containing energy deposition spectra
    if (gApplication->Argc() < 2) {
      std::cout << "Energy deposition file(s) not provided. Aborting." << std::endl;
      return 1;
    }
    std::cout << "Reading energy deposition output files from:" << std::endl;
    std::cout << gApplication->Argv()[1] << std::endl;
    eDepFilesList = getFilesListInDrectory(gApplication->Argv()[1], "root");

    // Fallback: if passed directly ROOT filename that exists
    if (eDepFilesList == NULL && !gSystem->AccessPathName(gApplication->Argv()[1])){
        std::cout << "Trying to open a single file: " << gApplication->Argv()[1] << std::endl;
        eDepFilesList = new TList();
        TObjString* objFileName = new TObjString(gApplication->Argv()[1]);
        eDepFilesList->Add(objFileName);
    }

    // Parameter 2: (optional) path to directory containing optical output from unit volumes
    if (gApplication->Argc() >= 3){
      std::cout << "Reading unit volume output files from:" << std::endl;
      std::cout << gApplication->Argv()[2] << std::endl;
      unitVolumesFilesList = getFilesListInDrectory(gApplication->Argv()[2], "root");
    }
    else {
      std::cout << "Unit volume optical output files not provided." << std::endl;
    }
  }
  else {
    // GUI mode: files are picked with ROOT UI file picker
    // Energy depositions in crystals for various incident particle energies
    TString text = "Select series of energy deposition spectra with same geometry but different incident particle energies.\n";
    text += "Optical resolution requires spectra to have energy deposition in unit volumes.";
    eDepFilesList = getFileNamesList(text);


    if (msgBoxQuestion("Plot optical resolution?", "Optical resolution requires optical spectra from unit volumes.") == EMsgBoxButton::kMBYes){
      unitVolumesFilesList = getFileNamesList("Select files with optical output from unit volumes");
    }
  }

  // Plot other graphs
  FitFunction fitFunction = FitFunction::CBALL;

  // Construct and plot energy resolution graph

  // Prepare output filename for energy resolution this way:
  // PWO-SICCAS-3x3-20x20x200mm-e--eres
  const char* firstFilename = ((TObjString *)eDepFilesList->At(0))->GetString().Data();
  TFile* file = openFile(firstFilename);
  // Get number of events, GPS particle energy and particle name
  const char* crystalMaterial = ((TObjString*)file->Get("crystalMaterial"))->GetName();
  TVector3* crystalSize = (TVector3*)file->Get("crystalSize");
  TVector2* crystalsNumber = (TVector2*)file->Get("crystalsNumber");
  const char* particleName = ((TObjString*)file->Get("gpsParticleName"))->GetName();

  std::stringstream buffer;
  buffer << crystalMaterial << "-" << (Int_t)crystalsNumber->X() << "x" << (Int_t)crystalsNumber->Y() << "-"
         << crystalSize->X() << "x" << crystalSize->Y() << "x" << crystalSize->Z() << "mm-" << particleName;
  TString eResBaseFileName = buffer.str().c_str();
  buffer.str( std::string() ); buffer.clear();

  {
    TGraph* energyResGraph = getResolutionSeriesGraph(fitFunction, eDepFilesList);
    TCanvas* canvas = new TCanvas();
    energyResGraph->Draw();

    TString canvasFilename = eResBaseFileName + "-eres-canvas";
    saveCanvasToDisk(canvas, canvasFilename);

    TString graphFilename = eResBaseFileName + "-eres-graph";
    saveGraphToDisk(energyResGraph, graphFilename);
  }

  if (unitVolumesFilesList != NULL){
    // Prepare output filename for energy resolution this way:
    // PWO-SICCAS-3x3-20x20x200mm-e--40mm-R1450-opres

    const char* detectorName = ((TObjString*)file->Get("detectorName"))->GetName();
    buffer << crystalMaterial << "-" << (Int_t)crystalsNumber->X() << "x" << (Int_t)crystalsNumber->Y() << "-"
           << crystalSize->X() << "x" << crystalSize->Y() << "x" << crystalSize->Z() << "mm-" << particleName << "-" << detectorName;
    TString opticalResBaseFileName = buffer.str().c_str();
    buffer.str( std::string() ); buffer.clear();

    TGraph* opticalResGraph = getResolutionSeriesGraph(fitFunction, eDepFilesList, unitVolumesFilesList);
    TCanvas* canvas = new TCanvas();
    opticalResGraph->Draw();

    TString canvasFilename = opticalResBaseFileName + "-optres-canvas";
    saveCanvasToDisk(canvas, canvasFilename);

    TString graphFilename = opticalResBaseFileName + "-optres-graph";
    saveGraphToDisk(opticalResGraph, graphFilename);
  }

  return 0;
}

#ifndef __CINT__

int main(int argc, char **argv) {
  // Instantiate TApplication
  TApplication* app = new TApplication("energyResolution", &argc, argv);

  energyResolution();

  // Enter the event loop
  app->Run();
  // Return success
  return 0;
}

#endif
