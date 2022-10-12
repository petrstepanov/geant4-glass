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
// $Id: HistoManager.cc 92374 2015-08-31 08:52:09Z gcosmo $
// GEANT4 tag $Name: geant4-09-04 $
// 
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#include "HistoManager.hh"
#include <TH1.h>
//#include <TFile.h>
//#include <TTree.h>
//#include <CLHEP/Units/SystemOfUnits.h>

#include "G4UnitsTable.hh"
#include "TString.h"
#include "TObjString.h"

#include "G4SystemOfUnits.hh"
#include "G4PhysicalConstants.hh"
#include "G4RunManager.hh"
#include "DetectorConstruction.hh"
#include "PrimaryGeneratorAction.hh"
#include "G4ParticleDefinition.hh"
#include "G4ParticleTable.hh"
#include "G4GeneralParticleSource.hh"
#include "G4SingleParticleSource.hh"
// #include <g4root.hh>
#include "G4Utils.hh"
#include "G4Run.hh"

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wshadow"
#pragma GCC diagnostic ignored "-Wpedantic"
#include "RooConstVar.h"
#include "TVector2.h"
#include "TVector3.h"
#include "TSystem.h"
#pragma GCC diagnostic pop

#include <iostream>
#include <sstream>
#include <limits>

#include <TArrayD.h>

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

HistoManager::HistoManager() : fFileNamePattern("file_x_%dmm_y_%dmm_z_%dmm_%.1fGeV.root"), fRootFile(0),
// Energy deposition tree
fNtupleCrystals(0), fEdep { 0 },                  // PS: use the initializer list for array members (C++11)
fEdepTotal(0),
// Energy depositino in unit volumes tree
fNtupleUnitVolumes(0), fUnitVolumeEnergies(0),
// World escape tree (1 million events max)
fNtupleOutOfWorld(0), fTotalOutWorldEnergy(0), fOutWorldEnergy(0), fOutWorldX(0), fOutWorldY(0), fOutWorldZ(0), fPdg(0), pdgNameMap(
    0),
// Optical data tree
fNtupleOptical(0),

fPhotonsProduced(0), fScintPhotonsProduced(0), fCherePhotonsProduced(0), fPhotonsDetected(0), fScintPhotonsDetected(0), fCherePhotonsDetected(
    0),

fPhotoElectrons(0), fScintPhotoElectrons(0), fCherePhotoElectrons(0), fAllPhotoElectrons(0), fAllScintPhotoElectrons(0), fAllCherePhotoElectrons(
    0),

// For unit volumes light distribution data
fPhotoElectronsDouble(0),
// Event-independent cumulative histograms with optical data

fPhotonsProducedHists(0), fScintPhotonsProducedHists(0), fCherePhotonsProducedHists(0),

fPhotonsProducedHist(0), fScintPhotonsProducedHist(0), fCherePhotonsProducedHist(0),

fPhotonsDetectedHists(0), fScintPhotonsDetectedHists(0), fCherePhotonsDetectedHists(0),

fPhotonsDetectedHist(0), fScintPhotonsDetectedHist(0), fCherePhotonsDetectedHist(0),

fOPCreatorProcess(0) {
}

HistoManager *HistoManager::instance = NULL;

HistoManager* HistoManager::getInstance() {
  if (!instance) {
    instance = new HistoManager;
  }
  return instance;
}

HistoManager::~HistoManager() {
  G4cout << "HistoManager::~HistoManager()" << G4endl;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void HistoManager::Book() {
  // Clear total energy escaped the world and total deposited energy from mesh.
  fEdepTotal = 0;
  fTotalOutWorldEnergy = 0;

  // Create output directory
  gSystem->MakeDirectory("./output");

  // Construct filename
  TString fileName = G4Utils::getOutputFileName();

  fRootFile = new TFile(fileName.Data(), "RECREATE");
  if (!fRootFile) {
    G4cout << "HistoManager::Book(). Cannot create output file \"" << fileName.Data() << "\"" << G4endl;
    exit(1);
  }
  G4cout << "HistoManager::Book(). Created output file \"" << fileName.Data() << "\"" << G4endl;

  // █▀▀ █▄░█ █▀▀ █▀█ █▀▀ █▄█   █▀▄ █▀▀ █▀█ █▀█ █▀ █ ▀█▀ █ █▀█ █▄░█   ▀█▀ █▀█ █▀▀ █▀▀
  // ██▄ █░▀█ ██▄ █▀▄ █▄█ ░█░   █▄▀ ██▄ █▀▀ █▄█ ▄█ █ ░█░ █ █▄█ █░▀█   ░█░ █▀▄ ██▄ ██▄

  fNtupleCrystals = new TTree("tree_crystals", "Energy deposition in crystals");
  // Create variable length array for energy deposition
  fEdep = new G4double[G4Utils::getNCrystals()];

  // Writing arrays to tree:
  // https://root.cern.ch/root/htmldoc/guides/users-guide/Trees.html#cb22
  TString eDepLeafList = TString::Format("fEdep[%d]/D", G4Utils::getNCrystals());
  fNtupleCrystals->Branch("edep", fEdep, eDepLeafList.Data());

  // █▀█ █▀█ ▀█▀ █ █▀▀ ▄▀█ █░░   ▀█▀ █▀█ █▀▀ █▀▀
  // █▄█ █▀▀ ░█░ █ █▄▄ █▀█ █▄▄   ░█░ █▀▄ ██▄ ██▄

  // Optical Tree
  fNtupleOptical = new TTree("tree_optical", "Tree for optical photons data");

  // Writing arrays to tree:
  // https://root.cern.ch/root/htmldoc/guides/users-guide/Trees.html#cb22
  // Arrays for produced and detected OP and PE

  // Photo-Electron output per event
  fPhotoElectrons = new G4int[G4Utils::getNCrystals()];
  TString leafListPhotoElectrons = TString::Format("fPhotoElectrons[%d]/I", G4Utils::getNCrystals());
  fNtupleOptical->Branch("pe", fPhotoElectrons, leafListPhotoElectrons.Data());

  fScintPhotoElectrons = new G4int[G4Utils::getNCrystals()];
  TString leafListScintPhotoElectrons = TString::Format("fScintPhotoElectrons[%d]/I", G4Utils::getNCrystals());
  fNtupleOptical->Branch("pe_scint", fScintPhotoElectrons, leafListScintPhotoElectrons.Data());

  fCherePhotoElectrons = new G4int[G4Utils::getNCrystals()];
  TString leafListCherePhotoElectrons = TString::Format("fCherePhotoElectrons[%d]/I", G4Utils::getNCrystals());
  fNtupleOptical->Branch("pe_chere", fCherePhotoElectrons, leafListCherePhotoElectrons.Data());

  // Total photo-electron output in all crystals
  fNtupleOptical->Branch("pe_all", &fAllPhotoElectrons, "fAllPhotoElectrons/I");
  fNtupleOptical->Branch("pe_scint_all", &fAllScintPhotoElectrons, "fAllScintPhotoElectrons/I");
  fNtupleOptical->Branch("pe_chere_all", &fAllCherePhotoElectrons, "fAllCherePhotoElectrons/I");

  // Photo-Electron output per event for emission from unit volumes - need to be float because we shoot 1 OP at a time
  fNtupleOptical->Branch("pe_double", &fPhotoElectronsDouble, "fPhotoElectronsDouble/D");

  // Photons produced per event
  fPhotonsProduced = new G4int[G4Utils::getNCrystals()];
  TString leafListPhotonsProduced = TString::Format("fPhotonsProduced[%d]/I", G4Utils::getNCrystals());
  fNtupleOptical->Branch("prod_op", fPhotonsProduced, leafListPhotonsProduced.Data());

  fScintPhotonsProduced = new G4int[G4Utils::getNCrystals()];
  TString leafListScintPhotonsProduced = TString::Format("fScintPhotonsProduced[%d]/I", G4Utils::getNCrystals());
  fNtupleOptical->Branch("prod_op_scint", fScintPhotonsProduced, leafListScintPhotonsProduced.Data());

  fCherePhotonsProduced = new G4int[G4Utils::getNCrystals()];
  TString leafListCherePhotonsProduced = TString::Format("fCherePhotonsProduced[%d]/I", G4Utils::getNCrystals());
  fNtupleOptical->Branch("prod_op_chere", fCherePhotonsProduced, leafListCherePhotonsProduced.Data());

  // Photons detected per event
  fPhotonsDetected = new G4int[G4Utils::getNCrystals()];
  TString leafListPhotonsDetected = TString::Format("fPhotonsDetected[%d]/I", G4Utils::getNCrystals());
  fNtupleOptical->Branch("det_op", fPhotonsDetected, leafListPhotonsDetected.Data());

  fScintPhotonsDetected = new G4int[G4Utils::getNCrystals()];
  TString leafListScintPhotonsDetected = TString::Format("fScintPhotonsDetected[%d]/I", G4Utils::getNCrystals());
  fNtupleOptical->Branch("det_op_scint", fScintPhotonsDetected, leafListScintPhotonsDetected.Data());

  fCherePhotonsDetected = new G4int[G4Utils::getNCrystals()];
  TString leafListCherePhotonsDetected = TString::Format("fCherePhotonsDetected[%d]/I", G4Utils::getNCrystals());
  fNtupleOptical->Branch("det_op_chere", fCherePhotonsDetected, leafListCherePhotonsDetected.Data());

  // █░█ █▄░█ █ ▀█▀   █░█ █▀█ █░░ █░█ █▀▄▀█ █▀▀ █▀   ▀█▀ █▀█ █▀▀ █▀▀
  // █▄█ █░▀█ █ ░█░   ▀▄▀ █▄█ █▄▄ █▄█ █░▀░█ ██▄ ▄█   ░█░ █▀▄ ██▄ ██▄

  // Unit Volumes energy deposition
  fNtupleUnitVolumes = new TTree("tree_unit_volumes", "Energy deposition in crystals unit volumes");

  G4int nUnitVolumes = G4Utils::getDetectorConstruction()->getNumberOfUnitVolumes();
  fUnitVolumeEnergies = new G4double[G4Utils::getNCrystals() * nUnitVolumes]; // row count
  TString leafListName = TString::Format("fUnitVolumeEnergies[%d]/D", G4Utils::getNCrystals() * nUnitVolumes);
  fNtupleUnitVolumes->Branch("unitVolumeEnergies", fUnitVolumeEnergies, leafListName.Data());

  // █▀▀ █▀ █▀▀ ▄▀█ █▀█ █▀▀   █░█░█ █▀█ █▀█ █░░ █▀▄   ▀█▀ █▀█ █▀▀ █▀▀
  // ██▄ ▄█ █▄▄ █▀█ █▀▀ ██▄   ▀▄▀▄▀ █▄█ █▀▄ █▄▄ █▄▀   ░█░ █▀▄ ██▄ ██▄

  // Detect energy of the particles that leave the world
  fNtupleOutOfWorld = new TTree("tree_escape", "Energy of particles that escape the world");
  fNtupleOutOfWorld->Branch("energy", &fOutWorldEnergy, "fOutWorldEnergy/D");
  fNtupleOutOfWorld->Branch("x", &fOutWorldX, "fOutWorldX/D");
  fNtupleOutOfWorld->Branch("y", &fOutWorldY, "fOutWorldY/D");
  fNtupleOutOfWorld->Branch("z", &fOutWorldZ, "fOutWorldZ/D");
  fNtupleOutOfWorld->Branch("pdg", &fPdg, "Particle ID /I");
  pdgNameMap = new TMap();

  // █░█░█ ▄▀█ █░█ █▀▀ █░░ █▀▀ █▄░█ █▀▀ ▀█▀ █░█   █░█ █ █▀ ▀█▀ █▀█ █▀▀ █▀█ ▄▀█ █▀▄▀█ █▀
  // ▀▄▀▄▀ █▀█ ▀▄▀ ██▄ █▄▄ ██▄ █░▀█ █▄█ ░█░ █▀█   █▀█ █ ▄█ ░█░ █▄█ █▄█ █▀▄ █▀█ █░▀░█ ▄█

  const G4int wlMin = 0;     // nm
  const G4int wlMax = 1000;  // nm

  // Photon spectra (with wavelength) produced in all crystals together
  fPhotonsProducedHist = new TH1I("prod_op", "OP produced in all crystals", wlMax - wlMin, wlMin, wlMax);
  fScintPhotonsProducedHist = new TH1I("prod_op_scint", "Scintillation OP produced in all crystals", wlMax - wlMin,
                                       wlMin, wlMax);
  fCherePhotonsProducedHist = new TH1I("prod_op_chere", "Cherenkov OP produced in all crystals", wlMax - wlMin, wlMin,
                                       wlMax);

  // Photon spectra (with wavelength) detected in all crystals together
  fPhotonsDetectedHist = new TH1I("det_op", "OP detected in all photo-cathodes", wlMax - wlMin, wlMin, wlMax);
  fScintPhotonsDetectedHist = new TH1I("det_op_scint", "Scintillation OP detected in all photo-cathodes", wlMax - wlMin,
                                       wlMin, wlMax);
  fCherePhotonsDetectedHist = new TH1I("det_op_chere", "Cherenkov OP detected in all photo-cathodes", wlMax - wlMin,
                                       wlMin, wlMax);

  // Photon spectra (with wavelength) produced in each crystal
  fPhotonsProducedHists = new TH1I*[G4Utils::getNCrystals()];
  fScintPhotonsProducedHists = new TH1I*[G4Utils::getNCrystals()];
  fCherePhotonsProducedHists = new TH1I*[G4Utils::getNCrystals()];

  // Photon spectra (with wavelength) detected in each crystal
  fPhotonsDetectedHists = new TH1I*[G4Utils::getNCrystals()];
  fScintPhotonsDetectedHists = new TH1I*[G4Utils::getNCrystals()];
  fCherePhotonsDetectedHists = new TH1I*[G4Utils::getNCrystals()];

  for (G4int i = 0; i < G4Utils::getNCrystals(); i++) {
    // Photon spectra (with wavelength) produced in each crystal
    fPhotonsProducedHists[i] = new TH1I(TString::Format("prod_op_%d", i),
                                        TString::Format("OP produced in crystal #%d", i), wlMax - wlMin, wlMin, wlMax);
    fScintPhotonsProducedHists[i] = new TH1I(TString::Format("prod_op_scint_%d", i),
                                             TString::Format("Scintillation OP produced in crystal #%d", i),
                                             wlMax - wlMin, wlMin, wlMax);
    fCherePhotonsProducedHists[i] = new TH1I(TString::Format("prod_op_chere_%d", i),
                                             TString::Format("Cherenkov OP produced in crystal #%d", i), wlMax - wlMin,
                                             wlMin, wlMax);

    // Photon spectra (with wavelength) detected in each crystal
    fPhotonsDetectedHists[i] = new TH1I(TString::Format("det_op_%d", i),
                                        TString::Format("OP detected from crystal #%d", i), wlMax - wlMin, wlMin,
                                        wlMax);
    fScintPhotonsDetectedHists[i] = new TH1I(TString::Format("det_op_scint_%d", i),
                                             TString::Format("Scintillation OP detected from crystal #%d", i),
                                             wlMax - wlMin, wlMin, wlMax);
    fCherePhotonsDetectedHists[i] = new TH1I(TString::Format("det_op_chere_%d", i),
                                             TString::Format("Cherenkov OP detected from crystal #%d", i),
                                             wlMax - wlMin, wlMin, wlMax);
  }

  fOPCreatorProcess = new TH1I("op_process", "Creator Processes for OP at the Photo-Cathode", 100, 0, 100);
  // fOPCreatorProcessNames(0) = new TList();
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

// Only secondary produced photons passed here from StackingAction

void HistoManager::fillCreatorHist(const char *processName) {
  // Check if the bin label exist
  static int maxProcessBin = 1;

  // Find bin with given process name
  Int_t bin = 0;
  for (int i = 1; i <= maxProcessBin; i++) {
    if (strcmp(processName, fOPCreatorProcess->GetXaxis()->GetBinLabel(i)) == 0) {
      bin = i;
      break;
    }
  }

  // If bin not found assign new histogram axis label
  if (bin == 0) {
    fOPCreatorProcess->GetXaxis()->SetBinLabel(maxProcessBin, processName);
    bin = maxProcessBin;
    fOPCreatorProcess->GetXaxis()->SetRange(1, maxProcessBin);
    maxProcessBin++;
  }

  // Fill the hist
  fOPCreatorProcess->Fill(processName, 1);
}

Bool_t HistoManager::producedHistIsFull() {
  if (fPhotonsProducedHist->GetBinContent(fPhotonsProducedHist->GetMaximumBin()) < INT_MAX - 10)
    return false;
  return true;
}

void HistoManager::SaveProducedPhoton(const G4Track *track, G4int copyNumber) {
  // Enure the produced OP histogram did not reach its INT_MAX maximum
  if (producedHistIsFull())
    return;

  G4double wavelength = G4Utils::getWavelength(track);

  G4String creatorProcess = G4Utils::getCreatorProcessSafe(track);
  // Populate photon wavelength histograms
  fPhotonsProducedHist->Fill(wavelength);
  fPhotonsProducedHists[copyNumber]->Fill(wavelength);
  if (creatorProcess == "Scintillation") {
    fScintPhotonsProducedHist->Fill(wavelength);
    fScintPhotonsProducedHists[copyNumber]->Fill(wavelength);
  } else if (creatorProcess == "Cerenkov") {
    fCherePhotonsProducedHist->Fill(wavelength);
    fCherePhotonsProducedHists[copyNumber]->Fill(wavelength);
  }
}

// All photons reached detector passed here from SteppingAction
void HistoManager::SaveDetectedPhoton(const G4Track *track, G4int copyNumber) {
  // Enure the produced OP histogram did not reach its INT_MAX maximum
  if (producedHistIsFull())
    return;

  G4double wavelength = G4Utils::getWavelength(track);

  G4String creatorProcess = G4Utils::getCreatorProcessSafe(track);
  // Populate photon wavelength histograms
  fPhotonsDetectedHist->Fill(wavelength);
  fPhotonsDetectedHists[copyNumber]->Fill(wavelength);
  if (creatorProcess == "Scintillation") {
    fScintPhotonsDetectedHist->Fill(wavelength);
    fScintPhotonsDetectedHists[copyNumber]->Fill(wavelength);
  } else if (creatorProcess == "Cerenkov") {
    fCherePhotonsDetectedHist->Fill(wavelength);
    fCherePhotonsDetectedHists[copyNumber]->Fill(wavelength);
  }
}

void HistoManager::FillNtupleOutOfWorld(G4double energy, G4double x, G4double y, G4double z, G4int pdg,
  const char *particleName) {
  fTotalOutWorldEnergy += energy;
  // Book maximum 1 million events
  // Book maximum 1 million events

  if (fNtupleOutOfWorld->GetEntries() < 1E6) {
    fOutWorldEnergy = energy;
    fOutWorldX = x;
    fOutWorldY = y;
    fOutWorldZ = z;
    fPdg = pdg;
    // fPdgVector.push_back(pdg); // Save pdg to vector

    fNtupleOutOfWorld->Fill();

    // Save pair of particle pdg and name. Will write this to file later.
    TString pdgString = TString::Format("%d", pdg);
    TObjString *key = new TObjString(pdgString.Data());
    TObjString *value = new TObjString(particleName);
    if (pdgNameMap->GetValue(pdgString.Data()) == NULL) {
      pdgNameMap->Add(key, value);
    }
  }
}

void HistoManager::FillNtupleEnergyDep(G4double *energyDeposition) {
  // Copy energy deposition values
  for (G4int i = 0; i < G4Utils::getNCrystals(); i++) {
    fEdep[i] = energyDeposition[i];
    fEdepTotal += energyDeposition[i];
  }
  // Save pair of particle pdg and name. Will write this to file later.
  fNtupleCrystals->Fill();
}

void HistoManager::FillNtupleUnitVolumeEnergies(std::vector<G4double> unitVolumeEnergies) {
  // Assure size of vector is correct
  if (G4Utils::getNCrystals() * G4Utils::getDetectorConstruction()->getNumberOfUnitVolumes() != (int) unitVolumeEnergies.size()) {
    std::cout << "size of vector not equals size of array" << std::endl;
    exit(1);
  }
  for (int i = 0; i < (int) unitVolumeEnergies.size(); i++) {
    fUnitVolumeEnergies[i] = unitVolumeEnergies[i];
  }
  fNtupleUnitVolumes->Fill();
}

void HistoManager::FillNtupleOptical(G4int *photonsProduced, G4int *scintPhotonsProduced, G4int *cherePhotonsProduced,
  G4int *photonsDetected, G4int *scintPhotonsDetected, G4int *cherePhotonsDetected, G4double *photoElectrons,
  G4double *scintPhotoElectrons, G4double *cherePhotoElectrons) {
  fAllPhotoElectrons = 0;
  fAllScintPhotoElectrons = 0;
  fAllCherePhotoElectrons = 0;
  fPhotoElectronsDouble = 0;

  // Copy optical photon and photoelectron numbers
  for (G4int i = 0; i < G4Utils::getNCrystals(); i++) {
    fPhotonsProduced[i] = photonsProduced[i];
    fScintPhotonsProduced[i] = scintPhotonsProduced[i];
    fCherePhotonsProduced[i] = cherePhotonsProduced[i];

    fPhotonsDetected[i] = photonsDetected[i];
    fScintPhotonsDetected[i] = scintPhotonsDetected[i];
    fCherePhotonsDetected[i] = cherePhotonsDetected[i];

    // We are doing rounding here. in EventAction Pe are counted as doubles. This saves time compared to generating a random number at every PE event.
    fPhotoElectrons[i] = (G4int) (photoElectrons[i] + 0.5);
    fAllPhotoElectrons += (G4int) (photoElectrons[i] + 0.5);
    fScintPhotoElectrons[i] = (G4int) (scintPhotoElectrons[i] + 0.5);
    fAllScintPhotoElectrons += (G4int) (scintPhotoElectrons[i] + 0.5);
    fCherePhotoElectrons[i] = (G4int) (cherePhotoElectrons[i] + 0.5);
    fAllCherePhotoElectrons += (G4int) (cherePhotoElectrons[i] + 0.5);

    // For light output from unit volumes (technically there is only one block in this simulation buut this will work)
    fPhotoElectronsDouble += photoElectrons[i];
  }

  // Write to tree
  fNtupleOptical->Fill();
}

void HistoManager::Save() {
  if (!fRootFile)
    return;

  // Save Trees ?
  // fNtupleOptical->Write();

  // Write particle PDG names
  // Save collection as a single object, otherwise each key will be saved independently
  // pdgNameMap->Print();
  pdgNameMap->Write("pdgNameMap", TObject::kSingleKey);

  // Write detector type
  std::cout << "Detector: " << G4Utils::getDetector()->getName() << std::endl;
  TObjString *detectorName = new TObjString(G4Utils::getDetector()->getName().c_str());
  detectorName->Write("detectorName");

  // Write material
  std::cout << "Crystal material: " << G4Utils::getCrystalMaterial() << std::endl;
  TObjString *crystalMaterial = new TObjString(G4Utils::getCrystalMaterial().c_str());
  crystalMaterial->Write("crystalMaterial");

  // Write crystal size
  std::cout << "Crystal size: " << G4Utils::getCrystalX() << "x" << G4Utils::getCrystalY() << "x"
            << G4Utils::getCrystalZ() << std::endl;
  TVector3 *crystalSize = new TVector3(G4Utils::getCrystalX(), G4Utils::getCrystalY(), G4Utils::getCrystalZ());
  crystalSize->Write("crystalSize");

  // Write crystal assembly size (1x1, 3x3, 5x5)
  std::cout << "Crystal assembly size: " << G4Utils::getNCrystalsX() << "x" << G4Utils::getNCrystalsY() << std::endl;
  TVector2 *nCrystals = new TVector2(G4Utils::getNCrystalsX(), G4Utils::getNCrystalsY());
  nCrystals->Write("crystalsNumber");

  // Write incident particle name
  std::cout << "Incident particle name: " << G4Utils::getGPSParticleName() << std::endl;
  TObjString *gpsParticleName = new TObjString(G4Utils::getGPSParticleName().c_str());
  gpsParticleName->Write("gpsParticleName");

  // Write incident particle energy
  EnergyValueUnit energyIncidentParticleEVU = G4Utils::formatEnergy(G4Utils::getGPSMonoEnergy());
  std::cout << "Incident particle energy: " << energyIncidentParticleEVU.value << " " << energyIncidentParticleEVU.unit
            << std::endl;
  RooConstVar *gpsEnergy = new RooConstVar("gpsParticleEnergy", "Incident particle energy, MeV",
                                           G4Utils::getGPSMonoEnergy());
  gpsEnergy->Write();

  // Write incident particle position
  std::cout << "Incident particle position: " << G4Utils::getGPSZPos() << " mm" << std::endl;
  RooConstVar *gpsPosition = new RooConstVar("gpsPosition", "Incident particle position, mm", G4Utils::getGPSZPos());
  gpsPosition = new RooConstVar("gpsPosition", "Incident particle position, mm", G4Utils::getGPSZPos());
  gpsPosition->Write();

  // Write number of events
  std::cout << "Number of events: " << G4Utils::getNumberOfEvents() << std::endl;
  RooConstVar *numberOfEvents = new RooConstVar("numberOfEvents", "Number of events", G4Utils::getNumberOfEvents());
  numberOfEvents->Write();

  // Write total energy from GPS
  G4double energyTotalGPS = G4Utils::getGPSMonoEnergy() * G4Utils::getNumberOfEvents();
  EnergyValueUnit energyTotalGPSEVU = G4Utils::formatEnergy(energyTotalGPS);
  std::cout << "Total injected energy: " << energyTotalGPSEVU.value << " " << energyTotalGPSEVU.unit << std::endl;
  RooConstVar *energyTotalGPSVar = new RooConstVar("energyGPSTotal", "Total energy from GPS, MeV", energyTotalGPS);
  energyTotalGPSVar->Write();

  // Write total energy in crystals from scorer
  EnergyValueUnit energyTotalCrystalsScorerEVU = G4Utils::formatEnergy(fEdepTotal);
  std::cout << "Deposited in crystals - from scorer: " << energyTotalCrystalsScorerEVU.value << " "
            << energyTotalCrystalsScorerEVU.unit << " (" << fEdepTotal / energyTotalGPS * 100 << " %)" << std::endl;
  RooConstVar *energyTotalCrystalsScorer = new RooConstVar("energyTotalCrystalsScorer",
                                                           "Total energy in crystals from scorer, MeV", fEdepTotal);
  energyTotalCrystalsScorer->Write();

  if (G4Utils::getDetectorConstruction()->GetUseGlobalScoringMesh()) {
    // Write total energy in crystals from mesh
    G4double energyTotalCrystalsMesh = G4Utils::getTotalQuantityFromMesh(MultiFunctionalDetectorNames::CRYSTALS_MESH,
                                                                         HitsCollectionNames::ENE_DEP_CRYSTALS_MESH);
    EnergyValueUnit energyTotalCrystalsMeshEVU = G4Utils::formatEnergy(energyTotalCrystalsMesh);
    std::cout << "Deposited in crystals - from global mesh: " << energyTotalCrystalsMeshEVU.value << " "
              << energyTotalCrystalsMeshEVU.unit << " (" << energyTotalCrystalsMesh / energyTotalGPS * 100 << " %)"
              << std::endl;
    RooConstVar *energyTotalCrystalsMeshVar = new RooConstVar("energyTotalCrystalsMesh",
                                                              "Total energy in crystals from mesh, MeV",
                                                              energyTotalCrystalsMesh);
    energyTotalCrystalsMeshVar->Write();

    // Write total energy in PMTs from mesh
    G4double energyTotalPMTsMesh = G4Utils::getTotalQuantityFromMesh(MultiFunctionalDetectorNames::PMTS_MESH,
                                                                     HitsCollectionNames::ENE_DEP_PMTS_MESH);
    EnergyValueUnit energyTotalPMTsMeshEVU = G4Utils::formatEnergy(energyTotalPMTsMesh);
    std::cout << "Deposited in detectors - from global mesh: " << energyTotalPMTsMeshEVU.value << " "
              << energyTotalPMTsMeshEVU.unit << " (" << energyTotalPMTsMesh / energyTotalGPS * 100 << " %)"
              << std::endl;
    RooConstVar *energyTotalPMTsMeshVar = new RooConstVar("energyTotalPMTsMesh", "Total energy in PMTs from mesh, MeV",
                                                          energyTotalCrystalsMesh);
    energyTotalPMTsMeshVar->Write();
  }

  // Save energy deposition from every unit volume of the crystal
//  if (G4Utils::getDetectorConstruction()->GetUseUnitVolumeScoringMesh()){
//    for (int i=0; i < G4Utils::getNCrystals(); i++){
//      // Construct names for mesh and primitive scorer
//      G4String meshName = G4Utils::addIndex(MultiFunctionalDetectorNames::CRYSTAL_MESH_, i+1);
//      G4String scorerName = G4Utils::addIndex(HitsCollectionNames::ENE_DEP_CRYSTAL_MESH_, i+1);
//
//      // Obtain std::vector with list of energies deposited in "unit volumes"
//      std::vector<G4double> vector = G4Utils::getUnitVolumesQuantityFromMesh(meshName.c_str(), scorerName.c_str());
//
//      // Convert vector to array
//      G4double* array = &vector[0];
//
//      // Create ROOT array from regular array
//      TArrayD* unitVolumeEnergies = new TArrayD(vector.size(), array);
//
//      // Write ROOT array to ROOT file
//      G4String unitVolumeEnergiesName = "unitVolumeEnergies" + std::to_string(i);
//      fRootFile->WriteObject(unitVolumeEnergies, unitVolumeEnergiesName.c_str());
//    }
//  }

  // Write total energy escaped the world
  EnergyValueUnit energyEscapedWorldEVU = G4Utils::formatEnergy(fTotalOutWorldEnergy);
  std::cout << "Escaped the world: " << energyEscapedWorldEVU.value << " " << energyEscapedWorldEVU.unit << " ("
            << fTotalOutWorldEnergy / energyTotalGPS * 100 << " %)" << std::endl;
  RooConstVar *energyTotalOutWorldVar = new RooConstVar("energyTotalOutWorld", "Total energy escaped the world, MeV",
                                                        fTotalOutWorldEnergy);
  energyTotalOutWorldVar->Write();

  // Write histograms with emitted and detected OP (wavelength spectra)
  // PS: no need because ROOT saves histograms by default in the file
  /*
   fPhotonsProducedHist->Write();
   fScintPhotonsProducedHist->Write();
   fCherePhotonsProducedHist->Write();

   fPhotonsDetectedHist->Write();
   fScintPhotonsDetectedHist->Write();
   fCherePhotonsDetectedHist->Write();

   for (G4int i = 0; i < G4Utils::getNCrystals(); i++){
   // Write histograms with emitted photon spectra
   fPhotonsProducedHists[i]->Write();
   fScintPhotonsProducedHists[i]->Write();
   fCherePhotonsProducedHists[i]->Write();
   fPhotonsDetectedHists[i]->Write();
   fScintPhotonsDetectedHists[i]->Write();
   fCherePhotonsDetectedHists[i]->Write();
   }
   */

  // Set creator process histofgram range
//  int maxCreatorBin = 1;
//  for (int i = 1; i <= fOPCreatorProcess->GetNbinsX(); i++){
//    if (fOPCreatorProcess->GetBinContent(i) == 0){
//      maxCreatorBin = i;
//    }
//  }
//  fOPCreatorProcess->GetXaxis()->SetRange(1, maxCreatorBin-1);
  // Close file
  fRootFile->Write();       // Writing the histograms to the file
  fRootFile->Close();       // and closing the tree (and the file)

  // Output
  G4cout << "HistoManager::Save(). Histograms and ntuples saved." << G4endl;

//  auto manager = G4AnalysisManager::Instance();
//  manager->OpenFile("test.root");
//  manager->CreateNtuple("t", "Geant4 step points");
//  manager->CreateNtupleIColumn("pdg", fPdgVector);
//  manager->FinishNtuple();
//  manager->Write();
//  manager->CloseFile();

  // Save histogram with particles exiting the world to file

//  TH1* hist = new TH1();
//  hist->Draw();
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
//void HistoManager::SetPrimaryParticle(G4double time, G4int PID, G4ThreeVector pos, G4ThreeVector mom, G4double energy){
//  fPrimaryTime = time;
//  fPrimaryPID = PID;
//  fPrimaryPos[0] = pos.x();
//  fPrimaryPos[1] = pos.y();
//  fPrimaryPos[2] = pos.z();
//  fPrimaryMom[0] = mom.x();
//  fPrimaryMom[1] = mom.y();
//  fPrimaryMom[2] = mom.z();
//  fPrimaryEnergy = energy;
// }

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

//void HistoManager::SetEnergy(G4int id, G4double edep, G4int sc, G4int ce, G4int opc, G4int opfc, G4int oppc){
//  fEdep[id] = edep;
//  fOP_sc[id] = sc;
//  fOP_ce[id] = ce;
//  fOP_cover[id] = opc;
//  fOP_frontcover[id] = opfc;
//  fOP_pmtcover[id] = oppc;
//}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

// void HistoManager::SetFluxEnergy(G4int evtNb, G4int id, G4double edep, G4ThreeVector position) {
//  if (fWriteStepPoints){
//    fEvtNb = evtNb;
//
//    for (int i = 0; i < MaxNtuple; i++){
//      fFluxEne[i] = 0.; fFluxPos_X[i] = 0.; fFluxPos_Y[i] = 0.; fFluxPos_Z[i] = 0.;
//    }
//    fFluxEne[id] = edep;
//    fFluxPos_X[id] = position.x();
//    fFluxPos_Y[id] = position.y();
//    fFluxPos_Z[id] = position.z();
// }
// }

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void HistoManager::setFileNamePattern(G4String fileNamePattern) {
  fFileNamePattern = fileNamePattern;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4String HistoManager::getFileNamePattern() {
  return fFileNamePattern;
}

////....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
//
//void HistoManager::setWriteStepPoints(G4bool value){
//  fWriteStepPoints = value;
//}
//
////....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
//
// G4bool HistoManager::getWriteStepPoints(){
//  return fWriteStepPoints;
// }

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4double HistoManager::getTotalWorldOutEnergy() {
  return fTotalOutWorldEnergy;
}
