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
/// \file OpticalEventAction.cc
/// \brief Implementation of the OpticalEventAction class

#include <OpticalEventAction.hh>
#include "G4RunManager.hh"
#include "G4Event.hh"
#include "G4SDManager.hh"
#include "G4HCofThisEvent.hh"
#include "G4UnitsTable.hh"
#include "DetectorConstruction.hh"
#include "G4VPrimitiveScorer.hh"

#include "G4Utils.hh"
#include "HistoManager.hh"
#include "Randomize.hh"
#include <iomanip>

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

OpticalEventAction::OpticalEventAction() : G4UserEventAction() {
  // Initialize  emitted photon numbers (for each crystral)
  fPhotonsProduced = new G4int[G4Utils::getNCrystals()];
  fScintPhotonsProduced = new G4int[G4Utils::getNCrystals()];
  fCherePhotonsProduced = new G4int[G4Utils::getNCrystals()];

  // Produced numbers of scintillation and Cherenkov photons per event
  fPhotonsDetected = new G4int[G4Utils::getNCrystals()];
  fScintPhotonsDetected = new G4int[G4Utils::getNCrystals()];
  fCherePhotonsDetected = new G4int[G4Utils::getNCrystals()];

  // Initialize output photo-electron numbers (for each crystral)
  fPhotoElectrons = new G4double[G4Utils::getNCrystals()];
  fScintPhotoElectrons = new G4double[G4Utils::getNCrystals()];
  fCherePhotoElectrons = new G4double[G4Utils::getNCrystals()];
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

OpticalEventAction::~OpticalEventAction(){}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

// Suppress unused warnings:
// https://stackoverflow.com/questions/3599160/how-to-suppress-unused-parameter-warnings-in-c
void OpticalEventAction::BeginOfEventAction(const G4Event* event){
  // Tip: EventAction clases do not created for every new event. One instance for all run?
  if (G4RunManager::GetRunManager()->GetVerboseLevel() != 0){
    std::cout << "OpticalEventAction::BeginOfEventAction " << event->GetEventID() << std::endl;
  }
  if (event->GetEventID() % 1000 == 0){
    std::cout << "OpticalEventAction::BeginOfEventAction " << event->GetEventID() << "+"<< std::endl;
  }

  // In the beginning of the action set arrays to zero
  for (int i = 0; i < G4Utils::getNCrystals(); i++){
    fPhotonsProduced[i] = 0;
    fScintPhotonsProduced[i] = 0;
    fCherePhotonsProduced[i] = 0;

    fPhotonsDetected[i] = 0;
    fScintPhotonsDetected[i] = 0;
    fCherePhotonsDetected[i] = 0;

    fPhotoElectrons[i] = 0;
    fScintPhotoElectrons[i] = 0;
    fCherePhotoElectrons[i] = 0;
  }

  // Create separate ntuple for TOF in current event in Analysis Manager
  if (G4Utils::getDetectorConstruction()->GetSaveTimeOfFlight()) {
    if (event->GetEventID() < G4Utils::maxTofEvents){
      G4Utils::getInstance()->prepareTOFNtuple(event->GetEventID());
    }
  }
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void OpticalEventAction::EndOfEventAction(const G4Event* event){
  if (G4RunManager::GetRunManager()->GetVerboseLevel() >= 1){
    std::cout << "OpticalEventAction::EndOfEventAction(). Event " << event->GetEventID() << std::endl;
    for (int i = 0; i < G4Utils::getNCrystals(); i++){
      std::cout << "Crystal " << i << ": " << fPhotonsProduced[i] << " OP produced (" << fScintPhotonsProduced[i] << " scintillation, " << fCherePhotonsProduced[i] << " Cherenkov)." << std::endl;
      std::cout << "        " << i << "  " << fPhotonsDetected[i] << " OP detected (" << fScintPhotonsDetected[i] << " scintillation, " << fCherePhotonsDetected[i] << " Cherenkov)." << std::endl;
      std::cout << "        " << i << "  " << fPhotoElectrons[i]  << " PE output   (" << fScintPhotoElectrons[i]  << " scintillation, " << fCherePhotoElectrons[i]  << " Cherenkov)." << std::endl;
    }
  }

  // In the end of the action pass total accumulated number of the PE in each crystal to HistoManager
  HistoManager* histoManager = HistoManager::getInstance();
  histoManager->FillNtupleOptical(fPhotonsProduced, fScintPhotonsProduced, fCherePhotonsProduced,
                                   fPhotonsDetected, fScintPhotonsDetected, fCherePhotonsDetected,
                                   fPhotoElectrons, fScintPhotoElectrons, fCherePhotoElectrons);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void OpticalEventAction::saveProducedPhoton(G4String creatorProcess, G4int copyNumber){
  fPhotonsProduced[copyNumber]++;
  if (creatorProcess == "Scintillation"){
    fScintPhotonsProduced[copyNumber]++;
  }
  else if (creatorProcess == "Cerenkov"){
    fCherePhotonsProduced[copyNumber]++;
  }
}

void OpticalEventAction::saveDetectedPhoton(G4String creatorProcess, G4int copyNumber){
  fPhotonsDetected[copyNumber]++;
  if (creatorProcess == "Scintillation"){
    fScintPhotonsDetected[copyNumber]++;
  }
  else if (creatorProcess == "Cerenkov"){
    fCherePhotonsDetected[copyNumber]++;
  }
}

void OpticalEventAction::savePhotoElectron(G4double efficiency, G4String creatorProcess, G4int copyNumber){
  fPhotoElectrons[copyNumber]+=efficiency;
  if (creatorProcess == "Scintillation"){
    fScintPhotoElectrons[copyNumber]+=efficiency;
  }
  else if (creatorProcess == "Cerenkov"){
    fCherePhotoElectrons[copyNumber]+=efficiency;
  }
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
