#include <TFile.h>
#include <TGraph.h>
#include <TMultiGraph.h>
#include <TCanvas.h>
#include <TLegend.h>
#include <TObjString.h>
#include <TApplication.h>
#include <iostream>
#include "include/utilities.h"

void detectors(){
	TFile* r1450 = new TFile("../output/detectors/R1450.root");
	TFile* r1828 = new TFile("../output/detectors/R1828-01.root");
	TFile* r2059 = new TFile("../output/detectors/R2059.root");
	TFile* r2257 = new TFile("../output/detectors/R2257.root");
	TFile* s13360 = new TFile("../output/detectors/S13360-6025CS.root");
  TFile* s13360C = new TFile("../output/detectors/S13360-6025CS-Custom.root");

	// Detector efficiencies
	TMultiGraph *mg = new TMultiGraph();
	mg->SetTitle("Detector Quantum Efficiencies;Wavelength, nm;Efficiency, %");
	addGraph(mg, r1450, kStar);
	addGraph(mg, r1828, kFullDotLarge);
	addGraph(mg, r2059, kFullDotLarge, kViolet);
	addGraph(mg, r2257, kFullDotLarge, kRed);
	addGraph(mg, s13360, kOpenSquare);
  addGraph(mg, s13360C, kFullSquare);

	TCanvas* canvas = new TCanvas();
	// canvas->SetLeftMargin(0.15);
	canvas->SetRightMargin(0.05);

	mg->Draw("apl");
	canvas->BuildLegend();
	alignLegend(canvas, Alignment::TOP_RIGHT);
	addGrid(canvas);
}

#ifndef __CINT__

int main(int argc, char **argv) {
  // Instantiate TApplication
  TApplication* app = new TApplication("energyResolution", &argc, argv);

  detectors();

  // Enter the event loop
  app->Run();
  // Return success
  return 0;
}

#endif

