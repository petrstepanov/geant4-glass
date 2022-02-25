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
/// \file EventAction.cc
/// \brief Implementation of the EventAction class

#include <DetectorConstruction.hh>
#include <EventAction.hh>
#include <G4Event.hh>
#include <G4HCofThisEvent.hh>
#include <G4HCtable.hh>
#include <G4Run.hh>
#include <G4RunManager.hh>
#include <G4SDManager.hh>
#include <G4String.hh>
#include <G4Types.hh>
#include <G4Utils.hh>
#include <G4VPrimitiveScorer.hh>
#include <iostream>
#include <map>
#include <string>
#include <vector>
#include "HistoManager.hh"
#include <g4analysis.hh>


//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

EventAction::EventAction() : G4UserEventAction(), fCrystalEdepHCID(-1){
//  std::cout << "EventAction::EventAction()" << std::endl;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

EventAction::~EventAction(){
//  std::cout << "EventAction::~EventAction()" << std::endl;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

//G4double EventAction::GetSum(G4THitsMap<G4double>* hitsMap) const {
//  G4double sumValue = 0.;
//  for ( auto it : *hitsMap->GetMap() ) {
//    sumValue += *(it.second);
//  }
//  return sumValue;
//}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void EventAction::BeginOfEventAction(const G4Event* event){
  if (G4RunManager::GetRunManager()->GetVerboseLevel() != 0){
    std::cout << "EventAction::BeginOfEventAction " << event->GetEventID()+1 << std::endl;
  }
  else if (event->GetEventID() % 1000 == 0){
    std::cout << "EventAction::BeginOfEventAction " << event->GetEventID()+1 << "+"<< std::endl;
  }

  // Create separate ntuple for TOF in current event in Analysis Manager
  if (event->GetEventID() < G4Utils::maxTofEvents){
	  G4Utils::getInstance()->prepareTOFNtuple(event->GetEventID());
  }
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void EventAction::EndOfEventAction(const G4Event* event){
  if (G4RunManager::GetRunManager()->GetVerboseLevel() != 0){
    std::cout << "EventAction::EndOfEventAction " << event->GetEventID()+1 << std::endl;
  }
  DetectorConstruction* detectorConstruction = (DetectorConstruction*) G4RunManager::GetRunManager()->GetUserDetectorConstruction();
  if (detectorConstruction == NULL) return;

  // Debug: print all collection IDs
  G4RunManager* runManager = G4RunManager::GetRunManager();
  std::cout << "Run number " << runManager->GetCurrentRun()->GetRunID()+1 << ". Event number " << runManager->GetCurrentEvent()->GetEventID()+1 << "." << std::endl;
  G4Utils::getInstance()->printTimer();
  // Debug: print all collection IDs
  if (G4RunManager::GetRunManager()->GetVerboseLevel() != 0){
    std::cout << "Hits Collection Table (HIT_COLLECTION_NAME/SEN_DET_NAME):" << std::endl;
    G4HCtable* hitsCollectionTable = G4SDManager::GetSDMpointer()->GetHCtable();
    for (int i=0; i < hitsCollectionTable->entries(); i++){
      G4String hcName = hitsCollectionTable->GetHCname(i);
      G4String sdName = hitsCollectionTable->GetSDname(i);
      std::cout << "Index: " << i << ". Hit collection name: " << hcName << ". Sensitive detector name: " << sdName << std::endl;
    }
  }

  // Save energies registered by the primitive scorer
  if (detectorConstruction->GetSaveEnergyDeposition()){
    saveEnergyDepositionFromScorer(event);
  }

  // Save unit energies registered by meshes
  if (detectorConstruction->GetUseUnitVolumeScoringMesh()){
    saveEnergyDepositionFromScoringMeshUnitVolumes(event);
  }

//  // Save TOF per event
//  if (event->GetEventID() < G4Utils::maxTofEvents){
//	  ...
//  }

}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void EventAction::saveEnergyDepositionFromScorer(const G4Event* event){
  DetectorConstruction* detectorConstruction = (DetectorConstruction*) G4RunManager::GetRunManager()->GetUserDetectorConstruction();
  if (detectorConstruction == NULL) return;

  G4VPrimitiveScorer* eDepScorer = detectorConstruction->GetEdepScorer(); // G4PSEnergyDeposit
  // eDepScorer->PrintAll();

  // Get hit collection ID
  if (fCrystalEdepHCID == -1){
    // TODO: check collection name...
    G4String collectionID = MultiFunctionalDetectorNames::CRYSTAL_VOLUME;
    collectionID += "/";
    collectionID += HitsCollectionNames::ENE_DEP_CRYSTAL_VOLUME;
    fCrystalEdepHCID = G4SDManager::GetSDMpointer()->GetCollectionID(collectionID);
  }

  // Get hits collections of current event
  G4HCofThisEvent* HCE = event->GetHCofThisEvent();
  if (!HCE) return;

  G4THitsMap<G4double>* evtMap = static_cast<G4THitsMap<G4double>*>(HCE->GetHC(fCrystalEdepHCID));

  // Create array with energy deposition values, fill with zeros
  G4double* eDep = new G4double[G4Utils::getNCrystals()];
  for (G4int i = 0; i < G4Utils::getNCrystals(); i++){
    eDep[i] = 0;
  }

  // Iterate the Hits Map. Map index corresponds to the copy number of the Single Volume,
  // therefore we take it as a crystal index
  std::map<G4int,G4double*>::iterator itr;
  for (itr = evtMap->GetMap()->begin(); itr != evtMap->GetMap()->end(); itr++) {
    G4int copyNumber = itr->first;
    G4double energy = *(itr->second)/eDepScorer->GetUnitValue();
    if (G4RunManager::GetRunManager()->GetVerboseLevel() != 0){
      std::cout << "  Crystal Scorer, copy N " << copyNumber << ": total E " << energy << " " << eDepScorer->GetUnit() << std::endl;
    }
    // Remember energy deposition in crystal with corresponding index
    eDep[copyNumber] = energy;
  }

  // Save to Histogram Manager
  HistoManager* histoManager = HistoManager::getInstance();
  histoManager->FillNtupleEnergyDep(eDep);
}

void EventAction::saveEnergyDepositionFromScoringMeshUnitVolumes(const G4Event* event){
  // Clean Mesh from values
  for (int i = 0; i < G4Utils::getNCrystals(); i++){
    G4String mfdName = MultiFunctionalDetectorNames::getUnitVolumeMeshName(i);
    G4Utils::resetMesh(mfdName.c_str());
  }

  // Fill Mesh with values from current event
  // Problem: Mesh contents is empty when it in EventAction::EndOfEventAction
  // Figured G4VScoringMesh::Accumulate() fills Mesh from HitsCollection
  // Let's try to do so manually! Looked up in VSCode that Accumulate is called from RunManager::UpdateScoring()
  // G4RunManager::UpdateScoring() is protected - lets copy its contents here
  for (int i = 0; i < G4Utils::getNCrystals(); i++){
    G4String hcName = HitsCollectionNames::getUnitVolumeHCName(i);
    G4Utils::updateScoring(event, hcName);
  }

  // Read array of energies deposited in unit volumes of every crystal from meshes
  // Array size is nCrystals*nUnitVolumes = 9*10 = 90
  std::vector<G4double> allUnitVolumeEnergies = {};
  DetectorConstruction* detectorConstruction = (DetectorConstruction*) G4RunManager::GetRunManager()->GetUserDetectorConstruction();
  if (detectorConstruction == NULL) return;

  for (int i = 0; i < G4Utils::getNCrystals(); i++){
    // Mesh name is basically MultiFunctionalDetector name (Mesh contains MFD with same name)
    // TODO: maybe this increases number of active Navigators
    G4String mfdName = MultiFunctionalDetectorNames::getUnitVolumeMeshName(i);
    // Hits Collection Name is basically PrimitiveScorer name
    G4String hcName = HitsCollectionNames::getUnitVolumeHCName(i);
    //G4String hcName = HitsCollectionNames::ENE_DEP_CRYSTAL_MESH;

    // Vector of unit volume energies deposited in one crystal
    std::vector<G4double> v = G4Utils::getUnitVolumesQuantityFromMesh(mfdName.c_str(), hcName.c_str());

    // Test output
    if (G4RunManager::GetRunManager()->GetVerboseLevel() != 0){
      G4double total = 0;
      for (int j = 0; j < (int)v.size(); j++){
        total += v[j];
      }
      std::cout << "  Crystal Mesh " << i << ": total E from unit volumes: " << total << " MeV" << std::endl;
    }

    // Add to all unit volume energies
    allUnitVolumeEnergies.insert(allUnitVolumeEnergies.end(), v.begin(), v.end());

    // Clear Mesh (we're filling and clearing these meshes every event, not every run)
  }

  // Check the deposited energy equals injected
  if (G4RunManager::GetRunManager()->GetVerboseLevel() != 0){
    G4double total = 0;
    for (int j = 0; j < (int)allUnitVolumeEnergies.size(); j++){
      total += allUnitVolumeEnergies[j];
    }
    std::cout << "  Total in all unit volumes of all unit volumes: " << total << " MeV." << std::endl;
  }

  // Save unit energies in HistoManager
  HistoManager* histoManager = HistoManager::getInstance();
  histoManager->FillNtupleUnitVolumeEnergies(allUnitVolumeEnergies);
}
