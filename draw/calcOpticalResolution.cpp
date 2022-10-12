#include <FileUtils.h>
#include <HistUtils.h>
#include <Rtypes.h>
#include <RtypesCore.h>
#include <StringUtils.h>
#include <TApplication.h>
#include <TAttLine.h>
#include <TAxis.h>
#include <TCanvas.h>
#include <TCollection.h>
#include <TColor.h>
#include <TFile.h>
#include <TH1.h>
#include <TH2.h>
#include <TList.h>
#include <TMathBase.h>
#include <TNamed.h>
#include <TObjString.h>
#include <TRandom3.h>
#include <TString.h>
#include <TStyle.h>
#include <TSystemFile.h>
#include <TTree.h>
#include <TVector2.h>
#include <TVector3.h>
#include <TVectorD.h>
#include <cstdlib>
#include <iostream>

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
  Int_t i = 0;
  while (TObjString *el = dynamic_cast<TObjString*>(next())) {
    TFile *file = FileUtils::openFile(el->GetString().Data());

    // Create histogram with optical responses for each file
    TString histName = TString::Format("opResHist%d", i++);
    TH1 *hist = new TH1D(histName.Data(), histName.Data(), 100, 0, 0.27);

    TTree *tree = FileUtils::getTree(file, "tree_optical");

    TString varexp = TString::Format("pe_double>>%s", hist->GetName());
    tree->Draw(varexp.Data(), "", "goff");

    peFromSinglePhotonHistList->Add(hist);
  }
  return peFromSinglePhotonHistList;
}

void reconstructOpticalResolution(const char *fileName, TList *peResHists) {
  // Open ROOT file with energy deposition in unit volumes
  TFile *file = FileUtils::openFile(fileName);

  // Check Tree exists and not empty
  TTree *tree = FileUtils::getTree(file, "tree_unit_volumes");

  // Obtain number of crystals from file
  TVector2 *crystalsNumber = (TVector2*) file->Get("crystalsNumber");
  const Int_t nCrystals = crystalsNumber->X() * crystalsNumber->Y();

  // Obtain scintillation yield (number of OP/MeV) for given material
  const char *crystalMaterial = ((TObjString*) file->Get("crystalMaterial"))->GetName();
  PathComponents pt = FileUtils::parseFilePath(fileName);
  const char *uvEDepFilePath = pt.path.Data();
  TString materialFileName = TString::Format("%s/materials/%s.root", uvEDepFilePath, crystalMaterial);
  TFile *materialFile = FileUtils::openFile(materialFileName.Data());
  TVectorD *scintillationYieldVector = (TVectorD*) (materialFile->Get("SCINTILLATIONYIELD"));
  Double_t scintillationYield = (*scintillationYieldVector)[0];
  materialFile->Close();

  // TODO: parameterize in TFile
  Int_t nUnitVolumes = 10;

  // Read array of unit volume energies from the "tree_unit_volumes" tree
  Double_t *unitVolumeEnergies = new Double_t[nCrystals * nUnitVolumes];
  tree->SetBranchAddress("unitVolumeEnergies", unitVolumeEnergies);

  // Check Tree exists and not empty
  TTree *treePE = new TTree("tree_pe", "Tree with optical resolution - number of registered PE");

  // Tree array to save PE in each crystal
  // https://root.cern.ch/root/htmldoc/guides/users-guide/Trees.html#cb22
  Int_t *crystals = new Int_t[nCrystals];
  TString option = TString::Format("crystals[%d]/I", nCrystals);
  treePE->Branch("crystals", crystals, option.Data());

  // Tree variable to save PE output in all crystals
  Int_t all_crystals;
  treePE->Branch("all_crystals", &all_crystals, "all_crystals/I");

  // TODO: try different TRandom objects, compare speed.
  // https://root-forum.cern.ch/t/th1-getrandom-same-events-all-the-time/8469/4
  gRandom = new TRandom3(0);

  const int absoluteMaxEvents = 10000;

  Int_t maxEvents = TMath::Min(absoluteMaxEvents, (Int_t) tree->GetEntries());
  for (Int_t i = 0; i < maxEvents; i++) {
    StringUtils::writeProgress("Processing events", maxEvents);
    tree->GetEntry(i);

    Double_t peFromAllCrystals = 0; // Total optical output for fitting
    for (Int_t j = 0; j < nCrystals; j++) {
      // Iterate nuit volumes and sum all pe that reached detector
      Double_t eInCrystal = 0;
      Double_t opInCrystal = 0;
      Double_t peInCrystal = 0;
      for (Int_t unitVolumeIndex = 0; unitVolumeIndex < nUnitVolumes; unitVolumeIndex++) {
        Double_t energyInUnit = unitVolumeEnergies[j * (nUnitVolumes) + unitVolumeIndex];
        Int_t opInUnit = scintillationYield * energyInUnit; // number of emitted op in unit volume
        eInCrystal += energyInUnit;
        opInCrystal += opInUnit;
        TH1D *unitVolumeHistPE = static_cast<TH1D*>(peResHists->At(unitVolumeIndex));
        for (Int_t op = 0; op < opInUnit; op++) {
          peInCrystal += unitVolumeHistPE->GetRandom();
        }
      }
      crystals[j] = peInCrystal;
      peFromAllCrystals += peInCrystal;
    }
    all_crystals = peFromAllCrystals;
    treePE->Fill();
  }
  // Close energy deposition file (was opened in "READ" mode)
  file->Close();

  // Open file in write mode
  file = FileUtils::openFile(fileName, "UPDATE");

  // Check if "tree_pe" already exists and wipe all its instances
  // https://root-forum.cern.ch/t/how-to-remove-a-ttree-from-a-root-file/14477
  TObject *existingTree = file->Get("tree_pe");
  if (existingTree){
    file->Delete("tree_pe;*");
  }

  // Write a new PE tree
  treePE->Write();
  file->Close();
}

void drawLightOutputFromUnitVolumes(TList* listWithHistograms, const char* firstFileName){
  // Obtain crystal length
  TFile* file = FileUtils::openFile(firstFileName);
  Double_t crystalLength = ((TVector3*)(file->Get("crystalSize")))->Z();
  file->Close();

  TH2* histStack = HistUtils::makeHistStack(listWithHistograms, 0, crystalLength);

  TCanvas* canvas = new TCanvas();
  // gStyle->SetPalette(kGreyScale);
  TColor::InvertPalette();
  canvas->SetGrayscale();
  histStack->Draw();

  // Set histogram maximum - find local maximum of the last hist excluding first bin
  TH1* lastHist = (TH1*)(listWithHistograms->At(listWithHistograms->GetSize() - 1));
  Double_t max = -1;
  for (int bin = 2; bin < lastHist->GetNbinsX(); bin++){
    if (lastHist->GetBinContent(bin) > max) max = lastHist->GetBinContent(bin);
  }
  histStack->SetMaximum(max);
  canvas->SetTheta(31);
  canvas->SetPhi(-40);
  canvas->SetLeftMargin(0.15);

  histStack->SetTitle("Light Output from Unit Volume Centers");
  histStack->GetXaxis()->SetTitle("Photo-Electron Output");
  histStack->GetYaxis()->SetTitle("Emission position, mm");
  histStack->GetZaxis()->SetTitle("Distribution of Registered PE");

  histStack->GetXaxis()->SetTitleOffset(2);
  histStack->GetYaxis()->SetTitleOffset(2);
  histStack->GetZaxis()->SetTitleOffset(1.5);

  histStack->SetStats(0);
  histStack->SetLineColor(kBlack);
  TColor::InvertPalette();

  // histStack->GetXaxis()->SetRangeUser(minWavelengthBin, maxWavelengthBin);
  histStack->GetXaxis()->SetTitleOffset(2);
  histStack->GetYaxis()->SetTitleOffset(2);
  histStack->GetZaxis()->SetTitleOffset(1.5);

  histStack->Draw("LEGO2");
}

int calcOpticalResolution() {
  // FIRST PARAMETER: ROOT file or directory path with ROOT files containing energy deposition in unit volumes
  // Check if first parameter is file or directory
  TSystemFile *file = new TSystemFile();
  TList *uvEnergyDepositionFiles = new TList();
  if (file->IsDirectory(gApplication->Argv()[1])) {
    // First parameter is directory path
    uvEnergyDepositionFiles = FileUtils::findFilesInDirectory(gApplication->Argv()[1], "root");
  } else {
    // First parameter is file path
    TObjString *s = new TObjString(gApplication->Argv()[1]);
    uvEnergyDepositionFiles->Add(s);
  }

  // SECOND PARAMETER: always a path to the directory containing a set of ROOT files with detected PE output from unit volumes
  TList *uvDetectedEmissionFiles = FileUtils::findFilesInDirectory(gApplication->Argv()[2], "root");

    // Create histograms with detected PE from each unit volume
  const char* firstTileName = ((TObjString*)uvEnergyDepositionFiles->At(0))->GetString().Data();
  TList *peResHists = buildOpticalResolutionHists(uvDetectedEmissionFiles);
  drawLightOutputFromUnitVolumes(peResHists, firstTileName);

  // For each provided energy deposition file reconstruct ROOT file with optical resolution
  for (TObject *obj : *uvEnergyDepositionFiles) {
    const char *fileName = ((TObjString*) obj)->GetString().Data();

    // Merge unit volume energy deposition data and detected PE data from unit volumes
    reconstructOpticalResolution(fileName, peResHists);
  }
  return 0;
}

#ifndef __CINT__

int main(int argc, char **argv) {
  // Instantiate TApplication
  TApplication *app = new TApplication("calcOpticalResolution", &argc, argv);

  calcOpticalResolution();

  // Enter the event loop
  app->Run();

  // Return success
  return 0;
}

#endif
