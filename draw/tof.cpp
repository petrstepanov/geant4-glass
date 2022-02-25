#include <TApplication.h>
#include <TGClient.h>
#include <TGInputDialog.h>
#include <TTreeInput.h>
#include <TFile.h>
#include <TStyle.h>
#include <THStack.h>

#include "include/CanvasHelper.h"
#include "include/utilities.h"

Int_t nPadsX = 1;
Int_t nPadsY = 2;

void plotTOF(TTree* tree, Double_t maxTime, Double_t integrationWindowWidth, TVirtualPad* canvas, Int_t fillStyle){
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
  histScint->SetLineColor(EColor::kMagenta);
	histScint->SetFillColor(EColor::kMagenta);
	histScint->SetFillStyle(fillStyle);
	hs->Add(histScint);

	histChere->SetLineColor(EColor::kBlue);
	histChere->SetFillColor(EColor::kBlue);
	histChere->SetFillStyle(fillStyle);
	hs->Add(histChere);

	// TCanvas* canvas = new TCanvas();
	canvas->SetGrid();
	canvas->SetLogy();

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
	CanvasHelper::GetInstance();

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
  {
  TString text = TString::Format("Input integration window width, ns");
    TString defaultValue = TString::Format("%.1f", 0.1);
    new TGInputDialog(gClient->GetRoot(), NULL, text, defaultValue, returnString);
  }
  Double_t integationWindowWidth = atof(returnString);

	// Plot TOF for all events saved in the Tree
	TTree* treeAllEvents = getTree(file, "tof");
	Double_t maxTime = getBranchMaximum(treeAllEvents, "globalTime");

	// Plot TOF for 100ps window
	{
    TCanvas* canvas = new TCanvas();
    canvas->SetWindowSize(1440, 800);
    canvas->Divide(1, 2);

    plotTOF(treeAllEvents, maxTime,  integationWindowWidth, canvas->cd(1), 1001);
    plotTOF(treeAllEvents, 10,  integationWindowWidth, canvas->cd(2), EFillStyle::kFDotted2);

    addCanvasTitle(canvas, "Photon Time of Flight", "3 GeV mu-. 50 events. Cube10. Integration window 100 ps.");

    // Tweak canvas look (remove relative sizing)
    CanvasHelper::GetInstance()->tweakCanvas(canvas);

    // Save canvas
    TString outFileName = fileName;
    outFileName.ReplaceAll(".root", "-100ps");
    saveCanvasToDisk(canvas, outFileName);
	}

	// Plot TOF for 200 ns window
  {
    TCanvas* canvas = new TCanvas();
    canvas->SetWindowSize(1440, 800);
    canvas->Divide(1, 2);

    plotTOF(treeAllEvents, maxTime,  200, canvas->cd(1), 1001);
    plotTOF(treeAllEvents, 10,  200,      canvas->cd(2), EFillStyle::kFDotted2);

    addCanvasTitle(canvas, "Photon Time of Flight", "3 GeV mu-. 50 events. Cube10. Integration window 200 ns.");

    // Tweak canvas look (remove relative sizing)
    CanvasHelper::GetInstance()->tweakCanvas(canvas);

    // Save canvas
    TString outFileName = fileName;
    outFileName.ReplaceAll(".root", "-200ns");
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
