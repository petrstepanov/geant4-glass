/*
 * AbsDetector.cc
 *
 *  Created on: Feb 22, 2021
 *      Author: petrstepanov
 */

#include <AbsDetector.hh>
#include <G4ios.hh>

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wshadow"
#include <TSystem.h>
#include <TFile.h>
#include <TObjString.h>
#include <TGraph.h>
#pragma GCC diagnostic pop

AbsDetector::AbsDetector() : efficiencyPoints{} {
}

AbsDetector::~AbsDetector() {
}

G4double AbsDetector::getEfficiency(G4double wavelength){
  if (wavelength < efficiencyPoints[0]) return 0;
  if (wavelength >= efficiencyPoints[efficiencyPoints.size()-2]) return 0;

  // Return average value between most closest left and right points in the array
  for (G4int i = 0; i <= (G4int)efficiencyPoints.size()-4; i+=2){
    if (wavelength >= efficiencyPoints[i] && wavelength < efficiencyPoints[i+2]){
      G4double dLeft = wavelength - efficiencyPoints[i];
      G4double dRight = efficiencyPoints[i+2] - wavelength;
      return efficiencyPoints[i+1] + (efficiencyPoints[i+3] - efficiencyPoints[i+1]) * dLeft / (dLeft+dRight);
    }
  }

  return 0;
}

void AbsDetector::saveToFile(G4String outputPath){
  // Create output directory
  gSystem->MakeDirectory(outputPath.c_str());

  // Open file for output
  G4String fileName = outputPath + getName() + ".root";
	TFile* file = new TFile(fileName, "RECREATE");
  if (!file) {
    G4cout << "AbsDetector::saveToFile: cannot create output file \"" << fileName.c_str() << "\"" << G4endl;
    return;
  }
  G4cout << "AbsDetector::saveToFile: created output file \"" << fileName.c_str() << "\"" << G4endl;

  // Write detector name
	TObjString* detectorName = new TObjString(getName().c_str());
	detectorName->Write("name");

  // Write efficiency graph
	TGraph* graph = new TGraph();
	for (int i=0; i < (int)efficiencyPoints.size()-1; i+=2){
		graph->SetPoint(i/2., efficiencyPoints[i], efficiencyPoints[i+1]);
	}

  G4String graphTitle = getName() + " Quantum Efficiency" + ";Wavelength, nm; Efficiency, %";
  graph->SetTitle(graphTitle);
  graph->SetMarkerStyle(kStar);
  graph->Write("efficiency");

  // Close file
  file->Write();
  file->Close();
}
