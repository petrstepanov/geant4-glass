//
// ********************************************************************
// * License and Disclaimer                                           *
// *                                                                  *
// * The  Geant4 software  is  copyright of the Copyright Holders  of *
// * the Geant4 Collaboration.  It is provided  under  the terms  and *
// * conditions of the Geant4 Software License,  included in the file *
// * LICENSE and available at  http://cern.ch/geant4/license .  These *
// * include a list of copyright holders.                             *
// *                                                                  *
// * Neither the authors of this software system, nor their employing *
// * institutes,nor the agencies providing financial support for this *
// * work  make  any representation or  warranty, express or implied, *
// * regarding  this  software system or assume any liability for its *
// * use.  Please see the license in the file  LICENSE  and URL above *
// * for the full disclaimer and the limitation of liability.         *
// *                                                                  *
// * This  code  implementation is the result of  the  scientific and *
// * technical work of the GEANT4 collaboration.                      *
// * By using,  copying,  modifying or  distributing the software (or *
// * any work based  on the software)  you  agree  to acknowledge its *
// * use  in  resulting  scientific  publications,  and indicate your *
// * acceptance of all terms of the Geant4 Software license.          *
// ********************************************************************
//
//
// $Id: HistoManager.hh 92322 2015-08-27 14:54:05Z gcosmo $
// GEANT4 tag $Name: geant4-09-04 $
//
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#ifndef HistoManager_h
#define HistoManager_h 1

#include "globals.hh"
#include "G4ThreeVector.hh"
#include "G4Track.hh"
#include <vector>

// Ignoring warning shadow messages (doiPETAnalysis.hh)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wshadow"
#include "TFile.h"
#include "TTree.h"
#include "TH1F.h"
#include "TMap.h"
#pragma GCC diagnostic pop

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

// Forward Declaration of Messenger
class HistoManagerMessenger;

//class TFile;
//class TTree;
//class TH1D;

const G4int MaxHisto = 4;


//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

class HistoManager {
public:
  static HistoManager* getInstance();
  ~HistoManager();

  void Book();
  void FillNtupleEnergyDep(G4double* energy);
  void FillNtupleOptical(G4int* photonsProduced, G4int* scintPhotonsProduced, G4int* cherePhotonsProduced,
                         G4int* photonsDetected, G4int* scintPhotonsDetected, G4int* cherePhotonsDetected,
                         G4double* photoElectrons, G4double* scintPhotoElectrons, G4double* cherePhotoElectrons);
  void FillNtupleOutOfWorld(G4double energy, G4double x, G4double y, G4double z, G4int pdg, const char* particleName);
  void FillNtupleUnitVolumeEnergies(std::vector<G4double> energies);

  void Save();

  void PrintStatistic();

  // HistoManagerMessenger methods
  void setFileNamePattern(G4String fileNamePattern);
  G4String getFileNamePattern();

  // Calls from optical Stepping and Stacking actions
  void SaveProducedPhoton(const G4Track* track, G4int copyNumber);
  void SaveDetectedPhoton(const G4Track* track, G4int copyNumber);

  G4double getTotalWorldOutEnergy();

  void fillCreatorHist(const char* processName);

private:
  HistoManager();
  static HistoManager* instance;

  Bool_t producedHistIsFull();

  G4String fFileNamePattern;

  // Output ROOT file
  TFile*   fRootFile;

  // Energy deposition in each crystal tree
  TTree* fNtupleCrystals;
  G4double* fEdep;
  G4double fEdepTotal;

  // Energy deposition in crystals' unit volumes tree
  TTree*   fNtupleUnitVolumes;
  G4double* fUnitVolumeEnergies;

  // Escaped the world tree
  TTree* fNtupleOutOfWorld;
  G4double fTotalOutWorldEnergy;
  G4double fOutWorldEnergy;
  G4double fOutWorldX;
  G4double fOutWorldY;
  G4double fOutWorldZ;
  G4int fPdg;
  TMap* pdgNameMap;

  // Optical information per each event saved in the "optical" tree
  TTree* fNtupleOptical;

  G4int* fPhotonsProduced;                  // Numbers per event (no wavelength data)
  G4int* fScintPhotonsProduced;             // Numbers per event (no wavelength data)
  G4int* fCherePhotonsProduced;             // Numbers per event (no wavelength data)

  G4int* fPhotonsDetected;                  // Numbers per event (no wavelength data)
  G4int* fScintPhotonsDetected;             // Numbers per event (no wavelength data)
  G4int* fCherePhotonsDetected;             // Numbers per event (no wavelength data)

  G4int* fPhotoElectrons;                   // Numbers per event (no wavelength data)
  G4int* fScintPhotoElectrons;              // Numbers per event (no wavelength data)
  G4int* fCherePhotoElectrons;              // Numbers per event (no wavelength data)

  G4int fAllPhotoElectrons;                   // Numbers per event (no wavelength data) from all crystals
  G4int fAllScintPhotoElectrons;              // Numbers per event (no wavelength data) from all crystals
  G4int fAllCherePhotoElectrons;              // Numbers per event (no wavelength data) from all crystals

  // For building the optical response from emission from unit volume centers
  // These need to be of a float type because we are shooting a single op per event
  // If shooting 10e6 per event Farm runs out of 512 Mb memory - not good.
  G4double fPhotoElectronsDouble;                   // Numbers per event (no wavelength data)

  // Event-independent cumulative histograms with optical data
  // Photon spectra (with wavelength) produced in each crystal
  TH1I** fPhotonsProducedHists;
  TH1I** fScintPhotonsProducedHists;
  TH1I** fCherePhotonsProducedHists;

  // Photon spectra (with wavelength) produced in all crystals together
  TH1I* fPhotonsProducedHist;
  TH1I* fScintPhotonsProducedHist;
  TH1I* fCherePhotonsProducedHist;

  // Photon spectra (with wavelength) detected in each crystal
  TH1I** fPhotonsDetectedHists;
  TH1I** fScintPhotonsDetectedHists;
  TH1I** fCherePhotonsDetectedHists;

  // Photon spectra (with wavelength) detected in all crystals together
  TH1I* fPhotonsDetectedHist;
  TH1I* fScintPhotonsDetectedHist;
  TH1I* fCherePhotonsDetectedHist;

  // Histogram for processes that produced OP that reached detector
  TH1I* fOPCreatorProcess;
};

#endif
