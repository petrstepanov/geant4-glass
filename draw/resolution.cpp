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

// ----------------------------------
// Plot energy deposition in crystals
// ----------------------------------

int plotCrystalEdep(const char *fileName) {
  TFile *file = FileUtils::openFile(fileName);

  // Obtain number of crystals from file
  TVector2 *nCrystals = (TVector2*) file->Get("crystalsNumber");
  const Int_t nHists = nCrystals->X() * nCrystals->Y();
  std::cout << "Crystal assembly size: " << nCrystals->X() << "x" << nCrystals->Y() << std::endl;

  // Obtain incident particle energy
  Double_t particleEnergy = ((RooConstVar*) file->Get("gpsParticleEnergy"))->getVal();

  // Instantiate histograms to be saved in ROOT file
  TH1D **edepHist = new TH1D*[nHists];
  for (Int_t i = 0; i < nHists; i++) {
    TString edepHistName = TString::Format("edepHist_%d", i);
    TString edepHistTitle = TString::Format("Energy Deposition in Crystal %d", i + 1);
    edepHist[i] = new TH1D(edepHistName.Data(), edepHistTitle.Data(), 100, 0, particleEnergy);
    edepHist[i]->GetXaxis()->SetTitle("Deposited energy, MeV");
    edepHist[i]->GetYaxis()->SetTitle("Counts");
  }

  TTree *tree = FileUtils::getTree(file, "tree_crystals");
  if (tree == NULL)
    return 0;

  // Do not process files smaller that 100 events
  if ((Int_t) tree->GetEntries() < 100) {
    if (gROOT->IsBatch()) {
      Error("plotCrystalEdep", "Not enough events to plot energy resolution graph");
    } else {
      UiUtils::showMessageBox("Not enough events to plot energy resolution graph", "plotCrystalEdep",
                              EMsgBoxIcon::kMBIconStop);
    }
    return 0;
  }

  // Try opening branches
  // TBranch* branch = tree->GetBranch("edep");
  // if (!branch){
  //   std::cout << "Branch \"edep\" could not be initialized. Skipping." << std::endl;
  //   return 0;
  // }

  // Every branch contains an array with values for each PMT event
  Double_t *edep = new Double_t[nHists];
  Double_t *totalEdep = new Double_t[nHists];
  for (Int_t j = 0; j < nHists; j++) {
    totalEdep[j] = 0;
  }

  tree->SetBranchAddress("edep", edep);
  for (Int_t i = 0; i < (Int_t) tree->GetEntries(); i++) {
    tree->GetEntry(i);
    for (Int_t j = 0; j < nHists; j++) {
      edepHist[j]->Fill(edep[j]);
      totalEdep[j] += edep[j];
    }
  }

  // Calculate total deposited energy
  // TODO: check if correct
  Double_t totalDepositedEnergy = 0;
  for (Int_t i = 0; i < nHists; i++) {
    // std::cout << "Crystal " << i << ": " << totalEdep[i] << " MeV" << std::endl;
    totalDepositedEnergy += totalEdep[i];
  }

  // Get number of events, GPS particle energy and particle name
  Int_t numberOfEvents = ((RooConstVar*) file->Get("numberOfEvents"))->getVal();
  const char *particleName = ((TObjString*) file->Get("gpsParticleName"))->GetName();
  const char *crystalMaterial = ((TObjString*) file->Get("crystalMaterial"))->GetName();
  TVector2 *crystalsNumber = (TVector2*) file->Get("crystalsNumber");
  TVector3 *crystalSize = (TVector3*) file->Get("crystalSize");
  Double_t totalGPSEnergy = numberOfEvents * particleEnergy;

  // Instantiate the Canvas
  TCanvas *canvas = new TCanvas();
  canvas->SetGrid();
  canvas->SetWindowSize(650, 650);

  // Divide the canvas unless we have a 1x1 assembly
  if (!(nCrystals->X() == 1 && nCrystals->Y() == 1)) {
    canvas->SetWindowSize(250 * nCrystals->X(), 250 * nCrystals->Y() + 50);
    canvas->Divide(nCrystals->X(), nCrystals->Y(), 1E-10, 1E-10);
  }

  gStyle->SetPalette(1);

  for (Int_t i = 0; i < nHists; i++) {
    TVirtualPad *pad = canvas->cd(i + 1);
    edepHist[i]->SetTitle("");
    edepHist[i]->Draw();
    edepHist[i]->SetFillColor(kCyan);
    // alignStats(pad);
    // alignStats(pad);
    // TString text1 = TString::Format("%.1f MeV deposited", totaledep[i]);
    Double_t percentDepositedEnergy = totalEdep[i] / totalDepositedEnergy * 100.;
    EnergyValueUnit evu = StringUtils::formatEnergy(totalEdep[i]);

    if (!(nCrystals->X() == 1 && nCrystals->Y() == 1)) {
      std::stringstream buffer;
      // buffer << "E Dep = " << evu.value << " " << evu.unit << ", " << percentDepositedEnergy << "%";
      buffer << "Collected energy = " << std::fixed << std::setprecision(1) << percentDepositedEnergy << "%";
      std::string line = buffer.str();
      buffer.str(std::string());
      buffer.clear();

      CanvasHelper::addTextToStats(line.c_str(), pad);
    }
    Bool_t centerCrystal = (nHists == 1 && i == 0) || (nHists == 9 && i == 4) || (nHists == 25 && i == 12);
    TPaveStats *paveStats = CanvasHelper::getDefaultPaveStats(pad);
    // paveStats->Print();
    if (centerCrystal) {
      CanvasHelper::setPaveAlignment(paveStats, kPaveAlignLeft | kPaveAlignTop);
    } else {
      CanvasHelper::setPaveAlignment(paveStats, kPaveAlignRight | kPaveAlignTop);
    }
  }

  // Set Canvas title and subtitle
  std::stringstream canvasTitle;
  canvasTitle << "Energy Deposition in " << crystalMaterial << " Crystal Assembly";

  EnergyValueUnit evu = StringUtils::formatEnergy(particleEnergy);
  EnergyValueUnit evuTotal = StringUtils::formatEnergy(totalGPSEnergy);
  EnergyValueUnit evuCrystals = StringUtils::formatEnergy(totalDepositedEnergy);
  std::stringstream canvasSubTitle;
  canvasSubTitle << (Int_t) crystalsNumber->X() << " #times " << (Int_t) crystalsNumber->Y() << " matrix  #upoint  "
                 << crystalSize->X() << " #times " << crystalSize->Y() << " #times " << crystalSize->Z()
                 << " mm^{3} crystal  #upoint  " << evu.value << " " << evu.unit << " " << particleName << "  #upoint  "
                 << std::fixed << std::setprecision(1) << totalDepositedEnergy / totalGPSEnergy * 100
                 << "% energy captured";

  CanvasHelper::addMultiCanvasTitle(canvas, canvasTitle.str().c_str(), canvasSubTitle.str().c_str());

  CanvasHelper::getInstance()->addCanvas(canvas);

  // Save canvas to file
  TString fileNameOnly = StringUtils::stripExtension(fileName);
  canvas->SaveAs((fileNameOnly + "-edep.png").Data());

  return 0;
}

// ----------------------------------
// Plot energy deposition in crystals
// ----------------------------------

TList* buildOpticalResolutionHists(TList *files) {
  TList *peFromSinglePhotonHistList = new TList();

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
    while (TObjString *el = dynamic_cast<TObjString*>(next())) {
      TFile *file = FileUtils::openFile(el->GetString().Data());

      // Create histogram with optical responces for each file
      TString histName = TString::Format("opResHist%d", i++);
      TH1 *hist = new TH1D(histName.Data(), histName.Data(), 100, 0, 0.27);

      TTree *tree = FileUtils::getTree(file, "tree_optical");

      TString varexp = TString::Format("pe_double>>%s", hist->GetName());
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
  TCanvas *c = new TCanvas("unitVolumesOutput", "Light Output from Unit Volumes");
  c->SetLogy();

  Int_t nHists = peFromSinglePhotonHistList->GetSize();
  for (int i = 0; i < nHists; i++) {
    TH1 *hist = static_cast<TH1D*>(peFromSinglePhotonHistList->At(i));

    // Set histogram color
    // Double_t alpha = (i+1.)/(nHists+1);
    // hist->SetLineColorkBlack, alpha);

    if (i == 0) {
      hist->Draw();
    } else {
      hist->Draw("SAME");
    }
  }
  TPaveStats *pave = CanvasHelper::getDefaultPaveStats(c);
  CanvasHelper::setPaveAlignment(pave, kPaveAlignRight | kPaveAlignTop);

  CanvasHelper::getInstance()->addCanvas(c);

  return peFromSinglePhotonHistList;
}

// ---------------------------------
// Fitting of the resolutino points
// ---------------------------------

class EresFunctionObject {
  public:
    EresFunctionObject() {
      // if option "width" is specified, the integral is the sum of
      // the bin contents multiplied by the bin width in x.
      // this->normalization = hist->Integral("width");
    }

    Double_t operator()(double *_x, double *par) {
      Double_t x = _x[0];
      Double_t a = par[0];
      Double_t b = par[1];

      return a + b / sqrt(x);
    }

  private:
//    Double_t normalization;
};

TVector3 getEnergyResolution(const char *fileName) {

  // Plot energy deposition in each crystal in the matrix
  plotCrystalEdep(fileName);

  TFile *file = FileUtils::openFile(fileName);
  TTree *tree = FileUtils::getTree(file, "tree_crystals");

  // Get number of events, GPS particle energy and particle name
  Double_t particleEnergy = ((RooConstVar*) file->Get("gpsParticleEnergy"))->getVal();
  Int_t numberOfEvents = ((RooConstVar*) file->Get("numberOfEvents"))->getVal();
  const char *particleName = ((TObjString*) file->Get("gpsParticleName"))->GetName();
  const char *crystalMaterial = ((TObjString*) file->Get("crystalMaterial"))->GetName();
  TVector3 *crystalSize = (TVector3*) file->Get("crystalSize");
  TVector2 *crystalsNumber = (TVector2*) file->Get("crystalsNumber");
  Double_t totalGPSEnergy = numberOfEvents * particleEnergy;

  // Estimate energy histogram lower and upper ranges
  // Obtain ranges of energy deposited per one event
  const Int_t nCrystals = crystalsNumber->X() * crystalsNumber->Y();
  // Double_t edep[nCrystals];
  Double_t *edep = new Double_t[nCrystals];
  tree->SetBranchAddress("edep", edep);
  Double_t minEdep = particleEnergy;
  Double_t maxEdep = 0;
  for (Int_t i = 0; i < (Int_t) tree->GetEntries(); i++) {
    // Get tree entry
    tree->GetEntry(i);
    // Each event we calculate the total energy deposited in all crystals
    Double_t totalEdep = 0;
    for (Int_t j = 0; j < nCrystals; j++) {
      totalEdep += edep[j];
    }
    if (totalEdep > maxEdep)
      maxEdep = totalEdep;
    if (totalEdep < minEdep)
      minEdep = totalEdep;
  }

  // Instantiate histogram for total energy deposition
  TH1D *edepHist = new TH1D("totalEdepHist", "", 250, minEdep, maxEdep * 1.25);
  edepHist->GetYaxis()->SetTitle("Counts");
  edepHist->GetXaxis()->SetTitle("Total deposited energy, MeV");

  // Fill histogram from the tree
  for (Int_t i = 0; i < (Int_t) tree->GetEntries(); i++) {
    // Get tree entry
    tree->GetEntry(i);
    // Each event we sum total energy deposited in all crystals
    Double_t totalEdep = 0;
    for (Int_t j = 0; j < nCrystals; j++) {
      totalEdep += edep[j];
    }
    edepHist->Fill(totalEdep);
  }
  edepHist->SetBinErrorOption(TH1::EBinErrorOpt::kNormal);

  // Should delete this heap after use
  // https://stackoverflow.com/questions/11379433/c-forbids-variable-size-array/11379442
  delete[] edep;

  TCanvas *canvas = new TCanvas();
  canvas->SetGrid();

  gStyle->SetOptFit(0);

  TF1 *cball = FitUtils::getCrystalBallFunction(edepHist);
  edepHist->Fit(cball, "M");

  // Plot histogram and fit
  gStyle->SetOptStat(10);
  edepHist->Draw();
  edepHist->SetFillColor(kCyan);

  // Set canvas title and subtitle
  // Tip: histogram Draw() calls TPad::Clear() therefore we add primitives (subtitle) after histogram
  {
    std::stringstream title;
    title << "Energy Resolution of " << crystalMaterial << " Crystal Assembly";
    edepHist->SetTitle(title.str().c_str());

    std::stringstream subtitle;
    subtitle << (Int_t) crystalsNumber->X() << " #times " << (Int_t) crystalsNumber->Y() << " matrix  #upoint  "
             << crystalSize->X() << " #times " << crystalSize->Y() << " #times " << crystalSize->Z()
             << " mm^{3} crystal  #upoint  " << numberOfEvents << " events  #upoint  " << particleEnergy / 1E3
             << " GeV " << particleName;
    CanvasHelper::addSubtitle(canvas, subtitle.str().c_str());
  }

  // Get fit parameters
  TVector2 mean = FitUtils::getCrystalBallMean(cball);
  TVector2 dispersion = FitUtils::getCrystalBallDispersion(cball);
  TVector2 resolution = FitUtils::getCrystalBallResolution(cball);

  // Add values to statistics box
  TString str;
  str = TString::Format("Mean, MeV = %f #pm %f", mean.X(), mean.Y());
  CanvasHelper::addTextToStats(str.Data(), canvas);
  str = TString::Format("Dispersion, MeV = %f #pm %f", dispersion.X(), dispersion.Y());
  CanvasHelper::addTextToStats(str.Data(), canvas);
  str = TString::Format("Resolution, %% = %f #pm %f", resolution.X(), resolution.Y());
  CanvasHelper::addTextToStats(str.Data(), canvas);


  Double_t depositedEnergy = 0;
  for (Int_t i = 1; i <= edepHist->GetNbinsX(); i++) {
    depositedEnergy += edepHist->GetBinCenter(i) * edepHist->GetBinContent(i);
  }

  Double_t percentDepositedEnergy = depositedEnergy / totalGPSEnergy * 100;
  TString st = TString::Format("Energy deposited, %% = %.1f", percentDepositedEnergy);
  CanvasHelper::addTextToStats(st.Data(), canvas);

  TPaveStats *stats = CanvasHelper::getDefaultPaveStats(canvas);
  CanvasHelper::setPaveAlignment(stats, kPaveAlignLeft | kPaveAlignTop);

  // Save canvas with energy resolution fitting
  CanvasHelper::getInstance()->addCanvas(canvas);
  TString canvasName = TString::Format("%s-eres", StringUtils::stripExtension(fileName).Data());
  canvas->SetName(canvasName.Data());
  CanvasHelper::saveCanvas(canvas, kFormatPng);

  // Save single energy resolution point to ASCII
  std::vector<std::string> colNames = { "Incident Energy, GeV", "Energy Resolution, %", "Error, %" };
  std::vector<double> values = { particleEnergy / 1000., resolution.X(), resolution.Y()};
  FileUtils::exportValuesToGnuplot("gnuplot-eres.dat", colNames, values);

  TVector3 point(particleEnergy / 1000., resolution.X(), resolution.Y());
  return point;
}

TVector3 getOpticalResolution(const char *fileName) {
  TFile *file = FileUtils::openFile(fileName);
  TTree *tree = FileUtils::getTree(file, "tree_pe");

  // Get number of events, GPS particle energy and particle name
  Double_t particleEnergy = ((RooConstVar*) file->Get("gpsParticleEnergy"))->getVal();
  Int_t numberOfEvents = ((RooConstVar*) file->Get("numberOfEvents"))->getVal();
  const char *particleName = ((TObjString*) file->Get("gpsParticleName"))->GetName();
  const char *crystalMaterial = ((TObjString*) file->Get("crystalMaterial"))->GetName();
  TVector3 *crystalSize = (TVector3*) file->Get("crystalSize");
  TVector2 *crystalsNumber = (TVector2*) file->Get("crystalsNumber");

  // Estimate energy histogram lower and upper ranges
  Double_t branchMin = FileUtils::getBranchMinimum(tree, "all_crystals");
  Double_t branchMax = FileUtils::getBranchMaximum(tree, "all_crystals");

  // Instantiate histogram for total energy deposition
  static int index = 1;
  TString histName = TString::Format("peHist%d", index++);
  TH1D *hist = new TH1D(histName.Data(), "Photo-Electron Distribution in Sensitive Detector", 250, branchMin,
                        branchMax);
  hist->GetYaxis()->SetTitle("Events");
  hist->GetXaxis()->SetTitle("Number of registered Photo-Electrons");

  // Fill histogram from "all_crystals" branch
  TString varexp = TString::Format("all_crystals>>%s", hist->GetName());
  tree->Draw(varexp.Data(), "", "goff");

  // Create canvas for plotting
  TCanvas *canvas = new TCanvas();
  canvas->SetGrid();

  gStyle->SetOptFit(0);
  TF1 *cball = FitUtils::getCrystalBallFunction(hist, kTRUE);
  cball->SetParLimits(1, 1, 100); // Tweak n sits at the limit

  hist->Fit(cball, "M");

  // Plot histogram
  gStyle->SetOptStat(10);
  hist->Draw();
  hist->SetFillColor(kCyan);

  // Set canvas title and subtitle
  // Tip: histogram Draw() calls TPad::Clear() therefore we add primitives (subtitle) after histogram
  {
    std::stringstream title;
    title << "Optical Resolution of " << crystalMaterial << " Crystal Assembly";
    hist->SetTitle(title.str().c_str());

    std::stringstream subtitle;
    subtitle << (Int_t) crystalsNumber->X() << " #times " << (Int_t) crystalsNumber->Y() << " matrix  #upoint  "
             << crystalSize->X() << " #times " << crystalSize->Y() << " #times " << crystalSize->Z()
             << " mm^{3} crystal  #upoint  " << numberOfEvents << " events  #upoint  " << particleEnergy / 1E3
             << " GeV " << particleName;
    CanvasHelper::addSubtitle(canvas, subtitle.str().c_str());
  }

  // Get fit parameters
  TVector2 mean = FitUtils::getCrystalBallMean(cball);
  TVector2 dispersion = FitUtils::getCrystalBallDispersion(cball);
  TVector2 resolution = FitUtils::getCrystalBallResolution(cball);

  // Add values to statistics box
  TString str;
  str = TString::Format("Mean, PE = %f #pm %f", mean.X(), mean.Y());
  CanvasHelper::addTextToStats(str.Data(), canvas);
  str = TString::Format("Dispersion, PE = %f #pm %f", dispersion.X(), dispersion.Y());
  CanvasHelper::addTextToStats(str.Data(), canvas);
  str = TString::Format("Resolution, %% = %f #pm %f", resolution.X(), resolution.Y());
  CanvasHelper::addTextToStats(str.Data(), canvas);

  // Align Stats Box
  TPaveStats *stats = CanvasHelper::getDefaultPaveStats(canvas);
  CanvasHelper::setPaveAlignment(stats, kPaveAlignRight | kPaveAlignTop);

  // Process and save canvas
  CanvasHelper::getInstance()->addCanvas(canvas);
  TString canvasName = TString::Format("%s-optres", StringUtils::stripExtension(fileName).Data());
  canvas->SetName(canvasName.Data());
  CanvasHelper::saveCanvas(canvas, kFormatPng);

  // Save single resolution point to ASCII file (for Gnuplot)
  std::vector<std::string> colNames = { "Incident Energy, GeV", "Optical Resolution, %", "Error, %" };
  std::vector<double> values = { particleEnergy / 1000., resolution.X(), resolution.Y() };
  FileUtils::exportValuesToGnuplot("gnuplot-optres.dat", colNames, values);

  TVector3 point(particleEnergy / 1000., resolution.X(), resolution.Y());
  return point;
}

void plotResolutionSeries(TList *fileNames, Bool_t isOptical) {
  // Create vectors to instantiate TGraph later from them
  std::vector<Double_t> energy = { };
  std::vector<Double_t> resolution = { };
  std::vector<Double_t> resolutionErr = { };

  // Get minimum and mmaximum energy range for subtitle wording
  Double_t eMin = std::numeric_limits<int>::max();
  Double_t eMax = std::numeric_limits<int>::min();

  // Process every file in the list of user input files
  for (TObject *obj : *fileNames) {
    if (!obj->InheritsFrom(TObjString::Class()))
      continue;
    TObjString *objString = (TObjString*) obj;
    const char *fileName = objString->GetString().Data();

    TVector3 resolutionPoint = isOptical ? getOpticalResolution(fileName) : getEnergyResolution(fileName);

    // Print obtained resolution to screen
    std::cout << (isOptical == kFALSE ? "Energy resolution point: " : "Optical resolution point: ") << std::endl;
    resolutionPoint.Print("V");

    energy.push_back(resolutionPoint.X());
    resolution.push_back(resolutionPoint.Y());
    resolutionErr.push_back(resolutionPoint.Z());

    TObjString *os = (TObjString*) obj;
    if (!os)
      continue;
    TFile *file = FileUtils::openFile(os->GetString().Data());
    Double_t particleEnergy = ((RooConstVar*) file->Get("gpsParticleEnergy"))->getVal();
    if (particleEnergy > eMax)
      eMax = particleEnergy;
    if (particleEnergy < eMin)
      eMin = particleEnergy;
  }

// Plot graph with energy resolution
  const char *firstFilename = ((TObjString*) fileNames->At(0))->GetString().Data();
  TFile *file = FileUtils::openFile(firstFilename);
// Get number of events, GPS particle energy and particle name
  const char *crystalMaterial = ((TObjString*) file->Get("crystalMaterial"))->GetName();
  TVector3 *crystalSize = (TVector3*) file->Get("crystalSize");
  TVector2 *crystalsNumber = (TVector2*) file->Get("crystalsNumber");

  Int_t numberOfEvents = ((RooConstVar*) file->Get("numberOfEvents"))->getVal();
  const char *particleName = ((TObjString*) file->Get("gpsParticleName"))->GetName();

  // Create TGraph with errors from above data
  TGraphErrors *gr = new TGraphErrors((int) energy.size(), &energy[0], &resolution[0], 0, &resolutionErr[0]); // convert vector to array

  TCanvas *canvas = new TCanvas();
  canvas->SetGrid();

  // Assign unique markers for every series
  static int markerStyle = EMarkerStyle::kFullCircle;
  gr->SetMarkerStyle(markerStyle);
  markerStyle++;

  // Sort points along X axis and draw the graph
  gr->Sort(&TGraph::CompareX);

  // Set title and subtitle after drawing the TGraph because it clears the Canvvas
  TString title = TString::Format("%s Resolution Series for %s  Crystal Assembly;Incident particle energy, GeV;%s resolution, %%;", isOptical ? "Optical" : "Energy", crystalMaterial, isOptical ? "Optical" : "Energy");
  gr->SetTitle(title.Data());
  gr->Draw("AP");

  EnergyValueUnit minEnergy = StringUtils::formatEnergy(eMin);
  EnergyValueUnit maxEnergy = StringUtils::formatEnergy(eMax);

  std::stringstream subtitle;
  subtitle << (Int_t) crystalsNumber->X() << " #times " << (Int_t) crystalsNumber->Y() << " matrix  #upoint  "
           << crystalSize->X() << " #times " << crystalSize->Y() << " #times " << crystalSize->Z()
           << " mm^{3} crystal  #upoint  " << numberOfEvents << " events  #upoint  " << minEnergy.value << "-"
           << maxEnergy.value << " " << maxEnergy.unit << " " << particleName;
  CanvasHelper::addSubtitle(canvas, subtitle.str().c_str());

// Fit series if it contains more than three points
  if (gr->GetN() >= 3) {
    // Define fitting function
    EresFunctionObject *eResFO = new EresFunctionObject();
    static int fitFunctionNumber = 1;
    TString fitFunctionName = "eRes";
    fitFunctionName += fitFunctionNumber++;
    TF1 *eRes = new TF1(fitFunctionName, eResFO, gr->GetXaxis()->GetXmin(), gr->GetXaxis()->GetXmax(), 2); // create TF1 class.
    eRes->SetParName(0, "a");
    eRes->SetParName(1, "b");

    // Output chi2 and fit the graph
    gStyle->SetOptFit(111);
    gr->Fit(eRes);

    // Align legend - not works
    canvas->GetListOfPrimitives()->Print();
    TPaveStats *paveStats = CanvasHelper::getDefaultPaveStats(canvas);
    CanvasHelper::setPaveAlignment(paveStats, kPaveAlignTop | kPaveAlignRight);
  }

  CanvasHelper::getInstance()->addCanvas(canvas);

  // Save Canvas as image
  std::stringstream buffer;
  buffer << crystalMaterial << "-" << (Int_t) crystalsNumber->X() << "x" << (Int_t) crystalsNumber->Y() << "-"
         << crystalSize->X() << "x" << crystalSize->Y() << "x" << crystalSize->Z() << "mm-" << particleName;
  TString baseFileName = buffer.str().c_str();
  buffer.str(std::string());
  buffer.clear();

  TString canvasName = TString::Format("%s-%s-canvas", baseFileName.Data(), isOptical ? "optres":"eres");
  canvas->SetName(canvasName);
  CanvasHelper::saveCanvas(canvas, kFormatPng);

}

int resolution() {
// List of file names
  TList *filePathNames = NULL;
//  TList *unitVolumesFilesList = NULL;
// TList* unitVolumesOuterFilesList = NULL;

// Read input files for energy deposition and optical output from unit volumes (optional)
  Bool_t isBatch = gApplication->Argc() > 1;
  if (isBatch) {
    // Batch mode is for running on the farm (no GUI)
    // Parameter 1: path to directory containing energy deposition spectra
    if (gApplication->Argc() < 2) {
      std::cout << "Energy deposition file(s) not provided. Aborting." << std::endl;
      return 1;
    }
    std::cout << "Reading energy deposition output files from:" << std::endl;
    std::cout << gApplication->Argv()[1] << std::endl;
    filePathNames = FileUtils::findFilesInDirectory(gApplication->Argv()[1], "root");

    // Fallback: if passed directly ROOT filename that exists
    if (filePathNames == NULL && !gSystem->AccessPathName(gApplication->Argv()[1])) {
      std::cout << "Trying to open a single file: " << gApplication->Argv()[1] << std::endl;
      filePathNames = new TList();
      TObjString *objFileName = new TObjString(gApplication->Argv()[1]);
      filePathNames->Add(objFileName);
    }
  } else {
    // GUI mode: files are picked with ROOT UI file picker
    // Energy depositions in crystals for various incident particle energies
    TString text =
        "Select series of energy deposition spectra with same geometry but different incident particle energies.\n";
    text += "Optical resolution requires spectra to be processed with \"calcOpticalResolution\" script first.";
    UiUtils::showMessageBox(text);
    filePathNames = UiUtils::getFilePaths(kTRUE);
  }

  // Plot and save series of points
  plotResolutionSeries(filePathNames, kFALSE);

  // Check if first file contains optical information
  TString firstFileName = ((TObjString*) filePathNames->At(0))->GetString();
  TFile *file = FileUtils::openFile(firstFileName.Data());
  TTree *treePE = FileUtils::getTree(file, "tree_pe");
  if (treePE != nullptr) {
    plotResolutionSeries(filePathNames, kTRUE /*isOptical*/);
  }

  return 0;
}

#ifndef __CINT__

int main(int argc, char **argv) {
  // Instantiate TApplication
  TApplication *app = new TApplication("energyResolution", &argc, argv);

  resolution();

  // Enter the event loop
  app->Run();

  // Return success
  return 0;
}

#endif
