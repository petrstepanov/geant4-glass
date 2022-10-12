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
/// \file hadronic/Hadr01/src/PhysicsListMessenger.cc
/// \brief Implementation of the PhysicsListMessenger class
//
//
// $Id: PhysicsListMessenger.cc 99019 2016-08-31 08:27:06Z gcosmo $
//
//
/////////////////////////////////////////////////////////////////////////
//
// PhysicsListMessenger
//
// Created: 31.01.2006 V.Ivanchenko
//
// Modified:
// 04.06.2006 Adoptation of Hadr01 (V.Ivanchenko)
//
////////////////////////////////////////////////////////////////////////
//
// 

#include "PhysicsListMessenger.hh"
#include "ActionInitialization.hh"
#include "MyConstants.hh"

#include <G4PhysListFactory.hh>
#include <G4OpticalParameters.hh>
#include <G4OpticalPhysics.hh>
#include <G4RunManager.hh>
#include <G4ParallelWorldPhysics.hh>
#include <G4Tokenizer.hh>

#include <iostream>

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

const G4String PhysicsListMessenger::physicsListCmdDefaultValue = "FTFP_BERT";

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

PhysicsListMessenger::PhysicsListMessenger(G4RunManager *runManager) : G4UImessenger(), fPhysicsList(0) {
  // Save pointer to run manager
  fRunManager = runManager;

  // Instantiate commands
  fSelectListCmd = new G4UIcommand("/physics/selectList", this);
  fSelectListCmd->SetGuidance("Add modular physics list.");
  fSelectListCmd->AvailableForStates(G4State_PreInit);
  // fSelectListCmd->SetDefaultValue(physicsListCmdDefaultValue);

  fSelectListCmd->SetGuidance("Define modular physics list");
  fSelectListCmd->SetGuidance("   [usage] /physics/selectList listName withOptical");
  fSelectListCmd->SetGuidance("      listName (string) : physics list name FTFP_BERT, QGSP_BERT, QGSC_BERT_EMZ, ...");
  fSelectListCmd->SetGuidance("      withOptical  (string) : add optical physics");

  G4UIparameter *param = nullptr;
  param = new G4UIparameter("listName", 's', false);
  fSelectListCmd->SetParameter(param);
  param = new G4UIparameter("withOptical", 's', true /* omittable */);
  param->SetParameterCandidates("withOptical");
  fSelectListCmd->SetParameter(param);

//  fAddOpticalCmd = new G4UIcmdWithoutParameter("/physics/addOptical", this);
//  fAddOpticalCmd->SetGuidance("Available Physics Lists");
//  fAddOpticalCmd->AvailableForStates(G4State_PreInit);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

PhysicsListMessenger::~PhysicsListMessenger() {
  delete fSelectListCmd;
//  delete fAddOpticalCmd;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void PhysicsListMessenger::SetNewValue(G4UIcommand *command, G4String value) {
  if (command == fSelectListCmd) {
    G4Tokenizer next(value);
    G4String physicsListName = next();
    G4String withOptical = next();

    // Initialize physics list
    G4PhysListFactory *physListFactory = new G4PhysListFactory();
    if (!physListFactory->IsReferencePhysList(physicsListName)) {
      std::cout << "Physics list \"" << physicsListName << "\" not found." << std::endl;
      value = physicsListCmdDefaultValue;
    }
    fPhysicsList = physListFactory->GetReferencePhysList(value);

    // Not forget to set the "layered mass geometry"
    // https://geant4-userdoc.web.cern.ch/UsersGuides/ForApplicationDeveloper/html/Detector/parallelWorld.html?highlight=registerparallelworld#layered-mass-geometry
    // fPhysicsList->RegisterPhysics(new G4ParallelWorldPhysics(MyConstants::singleParallelWorldName));

    // Initialize optical
    if (withOptical == "withOptical") {
      // Add optical physics to physics
      G4OpticalPhysics *opticalPhysics = new G4OpticalPhysics();

      // Setting the decay time profile of the WLS process to "exponential" or
      // "delta" - like (default)
      // http://publications.rwth-aachen.de/record/667646/files/667646.pdf
      // opticalPhysics->SetWLSTimeProfile("exponential");

      // deactivating ( false ) or activating ( true , default ) the postponed
      // processing of mother particles if optical photons are created by
      // http://publications.rwth-aachen.de/record/667646/files/667646.pdf
      auto opticalParams = G4OpticalParameters::Instance(); // Geant4 v.11
      opticalParams->SetCerenkovTrackSecondariesFirst(false);
      opticalParams->SetScintTrackSecondariesFirst(false);

      // option to allow for G4ScintillationTrackInformation
      // to be attached to a scintillation photon's track
      // opticalPhysics->SetScintillationTrackInfo(true);

      // setting the rise time of the scintillation process and the WLS process finite values ( true ) or zero ( false , default )
      opticalParams->SetScintFiniteRiseTime(true);

      // TODO?: specify Bricks Constant, p.187
      // http://publications.rwth-aachen.de/record/667646/files/667646.pdf

      // Register Physics List
      fPhysicsList->RegisterPhysics(opticalPhysics);
    }

    fRunManager->SetUserInitialization(fPhysicsList);

    // Deferred instantiate ActionInitialization
    fRunManager->SetUserInitialization(new ActionInitialization());
  }
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
