#include <TFile.h>
#include <TApplication.h>
#include <iomanip>
#include <RooConstVar.h>
#include <TList.h>
#include <TVector3.h>
#include <TSystem.h>
#include <TROOT.h>
#include <TObjString.h>
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

void npeVsSourcePos(){
  // Show file picker

  TString text = "Select series of spectra with detected number of PE for different source positions.";
  TList* fileNames = getFileNamesList(text);
  fileNames->Sort(kSortAscending);

  if (fileNames->GetSize() <= 0) return;

  // Set current ROOT working directory to first file path
//  TString firstFileName = ((TObjString*)fileNames->At(0))->String();
//  TFile* firstFile = openFile(firstFileName); // For future use
//  TString directory = firstFile->GetPath();
//  gSystem->cd(directory);
//
//  // Open material file
//  const char* materialName = ((TObjString*)firstFile->Get("crystalMaterial"))->GetString().Data();
//  TString materialFileName = TString::Format("materials/%s.root", materialName);
//  TFile* materialFile = new TFile(materialFileName.Data());
//
//  // Open detector file
//  const char* detectorName = ((TObjString*)firstFile->Get("detectorName"))->GetString().Data();
//  TString detectorFileName = TString::Format("detectors/%s.root", detectorName);
//  TFile* detectorFile = new TFile(detectorFileName.Data());
//
//  firstFile->Close();

  // Display fitting parameters
  gStyle->SetOptFit(1);

  // Iterate through files and plot
  for (int i=0; i < fileNames->GetSize(); i++){
    TObject* obj = fileNames->At(i);
    if (!obj->InheritsFrom(TObjString::Class())) continue;
    TObjString *objString = (TObjString*)obj;
    TString filePath = objString->GetString().Data();
    TFile* file = openFile(filePath.Data());
    TTree* tree = getTree(file, "tree_optical");

    TString fileName = getFileNameFromPath(filePath);
    TString canvasName = TString::Format("canvas%d", i);
    /*TCanvas* canvas =*/ new TCanvas(canvasName.Data(), fileName.Data());
    tree->Draw("pe");

    TH1 *peHist = (TH1*)gPad->GetPrimitive("htemp");
    peHist->SetTitle(fileName.Data());
    TF1* func = getCrystalBallFunction(peHist, kTRUE);
    func->Draw("SAME");

    // Obtain pe histogram from tree branch
    // Int_t maxPE = getBranchMaximum(tree, "pe");
    // TString histName=TString::Format("hist%d", i);
    // TH1* peHist = new TH1I(histName.Data(), fileName.Data(), 200, 0, maxPE);
    // TString drawPar = TString::Format("pe_all>>%s", histName.Data());
    // tree->Draw(drawPar.Data(), "", "goff");

    // Fit pe histogram with crystall ball function
    // TF1* func = fitHistWithCrystalBall(peHist, kTRUE);

    // Draw histogram and fitting function
	// peHist->Draw();
	// func->Draw("SAME");
    //  m = edepHist->GetFunction("fBall")->GetParameter(2); // mean
    //  Dm = edepHist->GetFunction("fBall")->GetParError(2); // mean error
    //  s = edepHist->GetFunction("fBall")->GetParameter(3); // sigma
    //  Ds = edepHist->GetFunction("fBall")->GetParError(3); // sigma error
  }
}

#ifndef __CINT__

int main(int argc, char **argv) {
  // Instantiate TApplication
  TApplication* app = new TApplication("lumiUnitVolumesOutput", &argc, argv);

  npeVsSourcePos();

  // Enter the event loop
  app->Run();
  // Return success
  return 0;
}

#endif
