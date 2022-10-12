#include <CanvasHelper.h>

#include <TApplication.h>
#include <TGClient.h>
#include <TGInputDialog.h>
#include <TTreeInput.h>
#include <TFile.h>
#include <TStyle.h>
#include <THStack.h>

#include "include/utilities.h"

Int_t nPadsX = 1;
Int_t nPadsY = 2;

void plotTOF(TTree* tree, Double_t maxTime, Double_t integrationWindowWidth, TVirtualPad* canvas, Int_t fillStyle, Bool_t grayscale = false){
	static Int_t histNumber = 0;
	const Int_t nBins = maxTime/integrationWindowWidth;

	// Construct scintillation histogram
	TString histScintName = TString::Format("histScint%d", histNumber);
	TH1F* histScint = new TH1F(histScintName, "Scintillation Photons", nBins, 0, maxTime);
	TString varExpScint = TString::Format("nPE:globalTime >> %s", histScintName.Data());
	// tree->Draw(varExpScint,"Process == \"Scintillation\"", "B");

	// Construct Cherenkov histogram
	TString histChereName = TString::Format("histChere%d", histNumber);
	TH1F* histChere = new TH1F(histChereName, "Cherenkov Photons", nBins, 0, maxTime);
	TString varExpChere = TString::Format("nPE:globalTime >> %s", histChereName.Data());
	// tree->Draw(varExpChere,"Process == \"Cerenkov\"", "B");
	histNumber++;

	// std::string process;
	char process[16] = "";
	Double_t nPE;
	Double_t time;
	tree->SetBranchAddress("Process",process);
	tree->SetBranchAddress("nPE",&nPE);
	tree->SetBranchAddress("globalTime",&time);

	Int_t nentries = (Int_t)tree->GetEntries();
	for (Int_t i=0; i<nentries; i++) {
	  tree->GetEntry(i);
	  // TH1* hist = (process == "Scintillation") ? histScint : histChere;
	  TH1* hist = (strcmp(process, "Scintillation") == 0	) ? histScint : histChere;
	  Int_t bin = hist->GetXaxis()->FindBin(time);
	  Double_t binContent = hist->GetBinContent(bin);
	  hist->SetBinContent(bin, binContent+nPE);
	}

	THStack* hs = new THStack("hs", "");

  histChere->SetLineColor(grayscale ? EColor::kGray+1 : EColor::kBlue);
  histChere->SetFillColor(grayscale ? EColor::kGray+1 : EColor::kBlue);
  if (!grayscale) histChere->SetFillStyle(fillStyle);
  hs->Add(histChere);

  histScint->SetLineColor(grayscale ? EColor::kBlack : EColor::kMagenta);
	histScint->SetFillColor(grayscale ? EColor::kBlack : EColor::kMagenta);
	if (!grayscale) histScint->SetFillStyle(fillStyle);
	hs->Add(histScint);

	// TCanvas* canvas = new TCanvas();
	canvas->SetGrid();
	// canvas->SetLogy();

	hs->Draw("nostack");
  hs->GetXaxis()->SetTitle("Time, ns");
  hs->GetYaxis()->SetTitle("Photo-Electron Count");

	TLegend* legend = new TLegend();
	legend->AddEntry(histScint);
	legend->AddEntry(histChere);
	legend->Draw();

	alignLegend(canvas, 0.2);

	// TText* text = new TText();
  // TString string = TString::Format("Integration Window %.2f ns", integrationWindowWidth);
  // text->DrawTextNDC(0.12, 0.78, string);
}

void tof(){
	// Open TOF file
	const char* fileName = getFileName("Open Time-Of-Flight Output File");
	TFile* file = openFile(fileName);

	// Instantiate CanvasHelper that will tweak canvas design
	CanvasHelper::getInstance();

	// Enter number of the events to analyze
	// Int_t nKeys = file->GetNkeys();

	// Temp variable to read values from the ROOT InputDialog
  char returnString[256] = {0};

  // Input number of events to process
  // {
  //   TString text = TString::Format("Input number of events to plot. 0 - all events. 1-%d", nKeys - 1);
  //   TString defaultValue = TString::Format("%d", nKeys - 1);
  //   new TGInputDialog(gClient->GetRoot(), NULL, text, defaultValue, returnString);
  // }
	// Int_t maxEvent = atoi(returnString);
	// if (maxEvent < 0 || maxEvent > nKeys - 1) return;

  // Input integration window width
//  {
//  TString text = TString::Format("Input integration window width, ns");
//    TString defaultValue = TString::Format("%.1f", 0.1);
//    new TGInputDialog(gClient->GetRoot(), NULL, text, defaultValue, returnString);
//  }
//  Double_t integationWindowWidth = atof(returnString);

  // Input event number
  {
  TString text = TString::Format("Input event number. Leave empty for all events.");
    TString defaultValue = TString::Format("%d", 1);
    new TGInputDialog(gClient->GetRoot(), NULL, text, defaultValue, returnString);
  }

	// Plot TOF for all events saved in the Tree
  TString treeName = TString::Format("tof%s", returnString);
	TTree* treeAllEvents = getTree(file, treeName);

	// Plot TOF for 400 ps window
  {
    TCanvas* canvas = new TCanvas();
    canvas->SetWindowSize(1440, 800);
    canvas->Divide(1, 3);

    Double_t samplingWindow = 0.4; // [ns]
//    Double_t maxTime = getBranchMaximum(treeAllEvents, "globalTime");
//    plotTOF(treeAllEvents, maxTime,  integationWindowWidth, canvas->cd(1), 1001);
    plotTOF(treeAllEvents, 400,  samplingWindow, canvas->cd(1), 1001);
    plotTOF(treeAllEvents, 10,  samplingWindow, canvas->cd(2), EFillStyle::kFDotted2);
    plotTOF(treeAllEvents, 10, 0.01, canvas->cd(3), EFillStyle::kFSolid, kTRUE);

    TString title = TString::Format("3 GeV mu-. Sampling window %d ps.", (int)(samplingWindow*1000));
    addMultiCanvasTitle(canvas, "Photon Time of Flight - Oscilloscope Output", title.Data());

    // Tweak canvas look (remove relative sizing)
    CanvasHelper::getInstance()->addCanvas(canvas);

    // Save canvas
    TString outFileName = fileName;
    TString suffix = TString::Format("%dps", (int)(samplingWindow*1000));
    outFileName.ReplaceAll(".root", suffix.Data());
    saveCanvasToDisk(canvas, outFileName);
  }


	// Queue save canvas

  // for (Int_t event = 0; event < maxEvent; event++){
	//	TString treeName = TString::Format("tof%d", event);
	//	TTree* tree = getTree(file, treeName);
	//	plotTOF(tree, maxTime, integationWindowWidth);
	//}


  //CanvasHelper::GetInstance()->exportCanvas(canvas, fileName);
}

#ifndef __CINT__

int main(int argc, char **argv) {
  // Instantiate TApplication
  TApplication* app = new TApplication("tof", &argc, argv);

  tof();

  // Enter the event loop
  app->Run();
  // Return success
  return 0;
}

#endif
