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
/// \file ActionInitialization.cc
/// \brief Implementation of the ActionInitialization class

#include "ActionInitialization.hh"

#include "WorldEscapeSteppingAction.hh"
#include "G4MultiEventAction.hh"
#include "G4MultiSteppingAction.hh"

#include "EventAction.hh"
#include "MyRunAction.hh"

#include "G4UserEventAction.hh"
#include "OpticalEventAction.hh"

#include "G4UserSteppingAction.hh"
#include "OpticalSteppingAction.hh"

#include "OpticalStackingAction.hh"
#include "PrimaryGeneratorAction.hh"
#include "HistoManager.hh"
#include "G4Utils.hh"
#include <vector>
#include <memory>
// For analysis manager
// https://geant4-userdoc.web.cern.ch/UsersGuides/ForApplicationDeveloper/BackupVersions/V10.7/html/Analysis/managers.html
// #include "g4analysis.hh"

class DetectorConstruction;

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

ActionInitialization::ActionInitialization()
 : G4VUserActionInitialization()
{
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

ActionInitialization::~ActionInitialization()
{
  std::cout << "Destroying ActionInitialization" << std::endl;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void ActionInitialization::BuildForMaster() const
{
  // PS: this is invoked in Multi-threading mode
  // HistoManager* histoManager = HistoManager::getInstance();
  MyRunAction* runAction = new MyRunAction();
  SetUserAction(runAction);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void ActionInitialization::Build() const
{
	// Moved initialization of histogram and ntuples classes to the Run Action
	// PS: this is invoked in Sequential mode
//  HistoManager* histoManager = HistoManager::getInstance();
//
//  // Creating the Analysis Manager
//  G4AnalysisManager* analysisManager = G4Analysis::ManagerInstance("root");
//  analysisManager->SetVerboseLevel(1);
//  // Open an output file
//  analysisManager->OpenFile("./output/tof");

  // Instantiate General Particle Source (GPS)
  PrimaryGeneratorAction* primaryGeneratorAction = new PrimaryGeneratorAction();
  SetUserAction(primaryGeneratorAction);

  // Initialize Histogram Manager before runs and save ROOT file after run is complete
  MyRunAction* runAction = new MyRunAction();
  SetUserAction(runAction);

  // Multiple user actions:
  // https://geant4-userdoc.web.cern.ch/UsersGuides/ForApplicationDeveloper/html/UserActions/multipleAction.html
  auto multiEventAction = new G4MultiEventAction{};
  auto multiSteppingAction = new G4MultiSteppingAction{};

  if (G4Utils::getDetectorConstruction()->GetUsePrimitiveScorer()){
    // Extract deposited energy in every crystal and save in Histogram Manager
    // https://gitlab.cern.ch/Gaussino/Gaussino/blob/e26413adf0060bbedbfe5b94604a933bf5f49a22/Sim/GiGaMT/src/Lib/GiGaActionInitializer.cpp
    // https://stackoverflow.com/questions/17473900/unique-ptr-to-a-derived-class-as-an-argument-to-a-function-that-takes-a-unique-p
    EventAction* eventAction = new EventAction();
    multiEventAction->push_back(std::unique_ptr<G4UserEventAction>(eventAction));
  }

  if (G4Utils::usingOpticalPhysics()){
    // Optical Event Action accumulates number of the charge carriers in each crystal during the event
    OpticalEventAction* opticalEventAction = new OpticalEventAction();
    multiEventAction->push_back(std::unique_ptr<G4UserEventAction>(opticalEventAction));

    // Stepping action counts optical photons that reach photocathode
    OpticalSteppingAction* opticalSteppingAction = new OpticalSteppingAction(opticalEventAction);
    multiSteppingAction->push_back(std::unique_ptr<G4UserSteppingAction>(opticalSteppingAction));

    // Optical Stacking Action detects number of optical photons (cherenkov vs scintillation)
    OpticalStackingAction* opticalStackingAction = new OpticalStackingAction(opticalEventAction);
    SetUserAction(opticalStackingAction);
  }

  // Stepping actions
  if (G4Utils::getDetectorConstruction()->GetSaveWorldEscapeEnergy()){
    // Stepping action detects and records particles that are leaving the World volume
    WorldEscapeSteppingAction* worldEscapeSteppingAction = new WorldEscapeSteppingAction();
    multiSteppingAction->push_back(std::unique_ptr<G4UserSteppingAction>(worldEscapeSteppingAction));
  }

  SetUserAction(multiEventAction);
  SetUserAction(multiSteppingAction);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
