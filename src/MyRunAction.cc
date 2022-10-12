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
/// \file analysis/shared/src/MyRunAction.cc
/// \brief Implementation of the MyRunAction class
//
//
// $Id: MyRunAction.cc 92322 2015-08-27 14:54:05Z gcosmo $
//
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#include "MyRunAction.hh"
#include "G4Utils.hh"
#include "DetectorConstruction.hh"
#include "HistoManager.hh"

#include <CLHEP/Random/Random.h>
#include <G4DefaultLinearColorMap.hh>
#include <G4ios.hh>
#include <G4ScoringManager.hh>
#include <G4String.hh>
#include <G4Types.hh>
#include <G4VScoreColorMap.hh>
#include <Randomize.hh>
#include <RtypesCore.h>
#include <G4RunManager.hh>
#include <G4AnalysisManager.hh>

using CLHEP::Hep3Vector;
// For analysis manager
// #include <g4analysis.hh>
#include <algorithm>
#include <ctime>
#include <iostream>

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

MyRunAction::MyRunAction()
 : G4UserRunAction()
{
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

MyRunAction::~MyRunAction(){}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void MyRunAction::BeginOfRunAction(const G4Run* aRun)
{ 

  // PS: this is invoked in Sequential mode
  HistoManager* histoManager = HistoManager::getInstance();

  // Create the Analysis Manager
  // https://geant4-userdoc.web.cern.ch/UsersGuides/ForApplicationDeveloper/BackupVersions/V10.7/html/Analysis/managers.html
  if (G4Utils::getDetectorConstruction()->GetSaveTimeOfFlight()) {
    G4AnalysisManager* analysisManager = G4AnalysisManager::Instance();
    analysisManager->SetVerboseLevel(1);
    // Open an output file
    G4String tofFileName = G4Utils::getOutputFileName("tof");
    analysisManager->OpenFile(tofFileName);

    // Create global ntuple for storing first 100 TOF events in Analysis Manager
    G4Utils::getInstance()->prepareTOFNtuple(-1);
  }

  // PS:
  // This method is invoked before entering the event loop (BeamOn?).
  // A typical use of this method would be to initialize and/or book histograms for a particular run.

  // PS: added automatic time-based random seeds for each run
  long seeds[2];
  time_t systime = time(NULL);
  seeds[0] = (long) systime;
  seeds[1] = (long) (systime*G4UniformRand());
  G4Random::setTheSeeds(seeds);
  G4Random::showEngineStatus();

  G4cout << "### Run " << aRun->GetRunID() << " start." << G4endl;

  // Instantiate histograms
  histoManager->Book();
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void MyRunAction::EndOfRunAction(const G4Run* aRun)
{
  // PS:
  // This method is invoked at the very end of the run processing.
  // It is typically used for a simple analysis of the processed run.

  G4int NbOfEvents = aRun->GetNumberOfEvent();
  if (NbOfEvents == 0) return;

  // Register color map for Glogal scoring mesh
  if (G4Utils::getDetectorConstruction()->GetUseGlobalScoringMesh()){
    // Equalize scoring mesh color maps for Crystals and PMTs (just for visualization)
    // Obtain maximum value of the crystals mesh
    Double_t maxCrystalMeshValue = G4Utils::getProjectionZMaximumQuantityFromMesh(MultiFunctionalDetectorNames::CRYSTALS_MESH, HitsCollectionNames::ENE_DEP_CRYSTALS_MESH);
    Double_t maxPMTMeshValue = G4Utils::getProjectionZMaximumQuantityFromMesh(MultiFunctionalDetectorNames::PMTS_MESH, HitsCollectionNames::ENE_DEP_PMTS_MESH);

    // Assign this value to the 1st bin of the PMT mesh
    G4VScoreColorMap* globalColorMap = new G4DefaultLinearColorMap("globalColorMap");
    globalColorMap->SetFloatingMinMax(false);
    globalColorMap->SetMinMax(0, std::max(maxCrystalMeshValue, maxPMTMeshValue));

    G4ScoringManager* scoringManager = G4ScoringManager::GetScoringManager();
    scoringManager->RegisterScoreColorMap(globalColorMap);
  }

  // Register color map for individual crystals scoring meshes
  if (G4Utils::getDetectorConstruction()->GetUseUnitVolumeScoringMesh()){
    Int_t crystalCenterNumber = G4Utils::getNCrystals()/2 + 0.5;
    G4String centerMeshName = MultiFunctionalDetectorNames::getUnitVolumeMeshName(crystalCenterNumber);

    G4String centerPrScoName = HitsCollectionNames::getUnitVolumeHCName(crystalCenterNumber);
    // G4String centerPrScoName = HitsCollectionNames::ENE_DEP_CRYSTAL_MESH;
    Double_t maxCenterCrystalZMeshValue = G4Utils::getMaximumQuantityFromMesh(centerMeshName, centerPrScoName);

    G4VScoreColorMap* centerCrystalColorMap = new G4DefaultLinearColorMap("centerCrystalColorMap");
    centerCrystalColorMap->SetFloatingMinMax(false);
    centerCrystalColorMap->SetMinMax(0, maxCenterCrystalZMeshValue);

    G4ScoringManager* scoringManager = G4ScoringManager::GetScoringManager();
    scoringManager->RegisterScoreColorMap(centerCrystalColorMap);
  }

  // List color maps
  G4ScoringManager* scoringManager = G4ScoringManager::GetScoringManager();
  scoringManager->ListScoreColorMaps();

  // Save histograms
  HistoManager* histoManager = HistoManager::getInstance();
  histoManager->Save();

  // Save Analysis Manager
  auto analysisManager = G4AnalysisManager::Instance();
  analysisManager->Write();
  analysisManager->CloseFile();

 //  if (maxCrystalMeshValue > maxPMTMeshValue){
//    G4Utils::setMaximumMeshQuantity("pmtsMesh", "eneEdepPMT", maxCrystalMeshValue);
//  } else {
//    G4Utils::setMaximumMeshQuantity("crystalsMesh", "eneDepCrystal", maxPMTMeshValue);
//  }
//
//  G4ScoringManager* scoringManager = G4ScoringManager::GetScoringManager();
//  scoringManager->DrawMesh(meshName, psName, colorMapName, axflg)
}
