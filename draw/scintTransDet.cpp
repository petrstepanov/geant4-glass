#include <TFile.h>
#include <TGraph.h>
#include <TMultiGraph.h>
#include <TCanvas.h>
#include <TLegend.h>
#include <TBox.h>
#include <TStyle.h>
#include <TObjString.h>
#include <TVector3.h>
#include <TPad.h>
#include <TAxis.h>
#include <TFrame.h>
#include <TPaveStats.h>
#include <TText.h>
#include <TH1.h>
#include <TH1F.h>
#include <TSystem.h>
#include <TTree.h>
#include <TVirtualPad.h>
#include <TDirectory.h>
#include <TGFileDialog.h>
#include <TGInputDialog.h>
#include <cstdio>
#include <cstdlib>
#include <TApplication.h>
#include "./include/utilities.h"

Int_t uuid = 0;

Int_t wlMin = 150;
Int_t wlMax = 900;

void plotOP(TFile* simulationFile){
	// Open material file
	const char* crystalMaterial = ((TObjString*)simulationFile->Get("crystalMaterial"))->GetString().Data();
	TString crystalFileName = TString::Format("./../output/materials/%s.root", crystalMaterial);
	TFile* crystalFile = new TFile(crystalFileName.Data());

	// Open detector file
	const char* detectorName = ((TObjString*)simulationFile->Get("detectorName"))->GetString().Data();
	TString detectorFileName = TString::Format("./../output/detectors/%s.root", detectorName);
	TFile* detectorFile = new TFile(detectorFileName.Data());

	// Prepare canvas
	TString name = TString::Format("canvas%d", uuid++);
	TCanvas* canvas = new TCanvas(name.Data(), "Emission, Absorption Length and Quantum Efficincy", 640, 900);
	canvas->Divide(1, 3);

	// Add 4th pad to canvas overlays 3rd - refer to TPad::Divide()
	/* TPad* pad = */ addOverlayPad(canvas, 3, 3);

	Float_t scaleFactor = 2;

	// Plot emission
	TVirtualPad* pad1 = canvas->cd(1);
	addGrid(pad1);
  const Double_t leftMargin = 0.15;
  const Double_t rightMargin = 0.05;
  pad1->SetLeftMargin(leftMargin);
  pad1->SetRightMargin(rightMargin);
  pad1->SetBottomMargin(0.2);
	TGraph *emission = (TGraph*)crystalFile->Get("FASTCOMPONENT");
	increaseGraphFontSize(emission, scaleFactor);
	emission->GetXaxis()->SetLimits(wlMin, wlMax);
	emission->Draw();

	// Plot absorption length
	TVirtualPad* pad2 = canvas->cd(2);
	addGrid(pad2);
  pad2->SetLeftMargin(leftMargin);
  pad2->SetRightMargin(rightMargin);
  pad2->SetBottomMargin(0.2);
	TGraph *absLength = (TGraph*)crystalFile->Get("ABSLENGTH");
	increaseGraphFontSize(absLength, scaleFactor);
	absLength->GetXaxis()->SetLimits(wlMin, wlMax);
	absLength->Draw();
	// Draw region fading the crystal length
	TVector3* crystalSize = (TVector3*)simulationFile->Get("crystalSize");
	TBox* box = new TBox(wlMin, 0, wlMax, crystalSize->Z());
	box->SetFillColorAlpha(kBlack, 0.15);
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
	efficiency->Draw();

	// Draw photons arriving at the photocathode
	TVirtualPad* pad4 = canvas->cd(4);
  pad4->SetLeftMargin(leftMargin);
  pad4->SetRightMargin(rightMargin);
  pad4->SetBottomMargin(0.2);
	TH1* wlHist = (TH1*)simulationFile->Get("photonWlReachedDetector");

	// Calculate avg OP per event
	TTree* treeOptical = (TTree*)simulationFile->Get("tree_optical");
	LongDouble_t opTotal = 0;
	Int_t op;
	treeOptical->SetBranchAddress("op", &op);
	for (Int_t i = 0; i < (Int_t)treeOptical->GetEntries(); i++){
		treeOptical->GetEntry(i);
		opTotal += op;
  }
	Double_t opMean = opTotal/treeOptical->GetEntries();
	TString histTitle = TString::Format("OP on cathode, %.1f/event", opMean);

	wlHist->SetTitle(histTitle.Data());
	wlHist->GetXaxis()->SetRangeUser(wlMin, wlMax);
	wlHist->GetXaxis()->SetLabelSize(0);
	wlHist->GetXaxis()->SetTickLength(0);
	wlHist->GetYaxis()->SetLabelSize(0);
	wlHist->GetYaxis()->SetTickLength(0);
	wlHist->GetYaxis()->SetTitle("OP at Cathode, a.u.");
	increaseAxisFontSize(wlHist->GetYaxis(), scaleFactor);
	wlHist->SetStats(0);
	wlHist->SetLineColor(EColor::kBlack);
	wlHist->SetFillStyle(EFillStyle::kFDotted2);
	wlHist->SetFillColor(EColor::kBlack);
	wlHist->Draw("Y+");

	pad4->BuildLegend();
	wlHist->SetTitle("");
	alignLegend(pad4);
}

void plotPE(TFile* simulationFile){
	// Create Canvas
	TString name = TString::Format("canvas%d", uuid++);
	TCanvas* canvas = new TCanvas(name.Data(), "Optical Output", 800, 400);
	addGrid(canvas);

	// Plot PE output
	TTree* treeOptical = (TTree*)simulationFile->Get("tree_optical");
	gStyle->SetOptStat("me"); // mean, entries
	treeOptical->Draw("pe");
	TH1F* hist = (TH1F*)canvas->GetPrimitive("htemp");
	// TH1F* hist = (TH1F*)gDirectory->Get("peHist");
	hist->GetXaxis()->SetTitle("Number of photo-electrons");
	hist->GetYaxis()->SetTitle("Events");
	hist->SetTitle("Optical output");
	hist->SetLineColor(EColor::kBlack);
	hist->SetFillStyle(EFillStyle::kFDotted2);
	hist->SetFillColor(EColor::kBlack);

	// Draw info
	const char* crystalMaterial = ((TObjString*)simulationFile->Get("crystalMaterial"))->GetString().Data();
	TText* materialText = new TText(0.15, 0.8, crystalMaterial);
	materialText->SetNDC();
	materialText->Draw();

	addPaveStatsEntry(canvas, crystalMaterial);

	const char* detectorName = ((TObjString*)simulationFile->Get("detectorName"))->GetString().Data();
	TText* detectorText = new TText(0.15, 0.75, detectorName);
	detectorText->SetNDC();
	detectorText->Draw();

	addPaveStatsEntry(canvas, detectorName);


	// Fit the histogram
	canvas->Update();
	char val[64] = "";
	/*TGInputDialog* inputDialog =*/ new TGInputDialog(gClient->GetRoot(), NULL, "Enter fit minimum. Click \"Cancel\" to abort fit.", val, val);
	if (val[0] != 0){
		Double_t fitMin = atof(val);
		Double_t fitMax = hist->GetXaxis()->GetXmax();
		gStyle->SetOptFit(1011);
		hist->Fit("gaus", "", "", fitMin, fitMax);
	}

  // Align the legend
	alignLegend(canvas);
}

void plotGraphs(const char* fileName){
  TFile* file = TFile::Open(fileName, "READ");
	plotOP(file);
	plotPE(file);
}

void scintTransDet(){
  TString text = "Open one or multiple simulation outputs with optical effects turned on";
  TList* fileNames = getFileNamesList(text);

  for (int i=0; i < fileNames->GetSize(); i++){
    TObject* obj = fileNames->At(i);
    if (!obj->InheritsFrom(TObjString::Class())) continue;
    TObjString *objString = (TObjString*)obj;

    plotGraphs(objString->GetString().Data());
  }
}

#ifndef __CINT__

int main(int argc, char **argv) {
  // Instantiate TApplication
  TApplication* app = new TApplication("energyResolution", &argc, argv);

  scintTransDet();

  // Enter the event loop
  app->Run();
  // Return success
  return 0;
}

#endif
