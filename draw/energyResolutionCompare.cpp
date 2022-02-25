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
#include <TMath.h>
#include <TVirtualPad.h>
#include <TPaveStats.h>
#include <TLegend.h>
#include <TStyle.h>
#include <TPie.h>
#include <TPieSlice.h>
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
#include <TGMsgBox.h>
#include <TApplication.h>
#include <cstring>
#include <string>
#include <iostream>
#include <sstream>
#include <fstream>

#include "include/utilities.h"

TH1* getHistogram(const char* fileName){
  TFile* file = openFile(fileName);
  TTree *tree = getTree(file, "tree_crystals");
  if (tree == NULL) return 0;

  // Do not process files smaller that 100 events
  if ((Int_t)tree->GetEntries() < 100) return 0;

  // Get number of events, GPS particle energy and particle name
  Double_t particleEnergy = ((RooConstVar*)file->Get("gpsParticleEnergy"))->getVal();
  // Int_t numberOfEvents = ((RooConstVar*)file->Get("numberOfEvents"))->getVal();
  const char* particleName = ((TObjString*)file->Get("gpsParticleName"))->GetName();
  const char* crystalMaterial = ((TObjString*)file->Get("crystalMaterial"))->GetName();
  TVector3* crystalSize = (TVector3*)file->Get("crystalSize");
  TVector2* crystalsNumber =  (TVector2*)file->Get("crystalsNumber");
  // Double_t totalGPSEnergy = numberOfEvents*particleEnergy;

  // Estimate energy histogram lower and upper ranges
  // Obtain ranges of energy deposited per one event
  Int_t nCrystals = crystalsNumber->X()*crystalsNumber->Y();
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
  buffer << (Int_t)crystalsNumber->X() << "x" << (Int_t)crystalsNumber->Y() <<
      ", " << crystalSize->X()/10 << "x" << crystalSize->Y()/10 << "x" << crystalSize->Z()/10 << " cm " <<  crystalMaterial <<
      ", " << particleEnergy/1E3 << " GeV " << particleName << ".";

  // TString title = TString::Format("Total energy deposition in %dx%d, %dx%dx%d %s crystal assembly per event, %.0f GeV %s.", (Int_t)crystalsNumber->X(), (Int_t)crystalsNumber->Y(), (Int_t)crystalSize->X(), (Int_t)crystalSize->Y(), (Int_t)crystalSize->Z(), crystalMaterial, particleEnergy/1E3, particleName);
  static int histNumber = 0;
  TString histName = TString::Format("totalEdepHist%d", ++histNumber);
  TH1D* edepHist = new TH1D(histName.Data(), buffer.str().c_str(), 250, minEdep, maxEdep*1.25);
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
  return edepHist;
}

TList* getHistogramsList(TList* fileNames){
	TList* histograms = new TList;
  for (TObject* obj : *fileNames){
  	if (!obj->InheritsFrom(TObjString::Class())) continue;
  	TObjString *objString = (TObjString*)obj;
  	TH1* histogram = getHistogram(objString->GetString().Data());
  	histograms->Add(histogram);
  }
  return histograms;
}

Double_t getNonZeroRightWing(TH1* hist){
	int binNumber = hist->GetNbinsX()-1;
	do {
		binNumber--;
	} while(hist->GetBinContent(binNumber) == 0);
	return hist->GetXaxis()->GetBinUpEdge(binNumber);
}

void drawHistograms(TList* histograms){
	TCanvas* canvas = new TCanvas();
	gStyle->SetOptStat(0);

  for (int i=0; i<histograms->GetSize(); i++){
  	TObject* obj = histograms->At(i);
  	if (!obj->InheritsFrom(TH1::Class())) continue;
  	TH1* histogram = (TH1*)obj;

  	histogram->SetLineColor(1);
  	Color_t fillColor = 12;
  	if (i%3 == 1) fillColor = 15;
  	if (i%3 == 2) fillColor = 18;
  	histogram->SetFillColor(fillColor);
  	// histogram->SetFillStyle(3000 + (i+1)%3);

  	const char* drawOption = (i==0)?"":"SAME";
  	histogram->Draw(drawOption);
  }

	// Get maximum non-zero right wing value
  TH1* firstHist = (TH1*)histograms->At(0);
  Double_t xMax = getNonZeroRightWing(firstHist);
  firstHist->GetXaxis()->SetRangeUser(0, xMax);

  canvas->BuildLegend();
  alignLegend(canvas, Alignment::TOP_LEFT, Decoration::DEFAULT, 0.07, 0.5);
  ((TH1*)histograms->At(0))->SetTitle("Effect of assembly size on energy deposition");
}

void energyResolutionCompare(){
	TList* fileNames = getFileNamesList();
	fileNames->Sort(kSortDescending);
	fileNames->Print();

	TList* histograms = getHistogramsList(fileNames);
	histograms->Print();

  drawHistograms(histograms);

//	for(TObject* obj:*histograms){
//		TH1* hist = static_cast<TH1*>(obj);
//		TCanvas* c = new TCanvas();
//		hist->Draw();
//	}
}

#ifndef __CINT__

int main(int argc, char **argv) {
  // Instantiate TApplication
  TApplication* app = new TApplication("energyResolution", &argc, argv);

  energyResolutionCompare();

  // Enter the event loop
  app->Run();
  // Return success
  return 0;
}

#endif
