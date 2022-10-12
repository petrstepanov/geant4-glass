/*
 * OpticalSteppingAction.cpp
 *
 *  Created on: Nov 24, 2020
 *      Author: petrstepanov
 */

#include <AbsPMT.hh>
#include "OpticalSteppingAction.hh"
#include "OpticalStackingAction.hh"
#include "OpticalUserTrackInformation.hh"
#include "DetectorConstruction.hh"
#include "OpticalEventAction.hh"
#include "G4VPhysicalVolume.hh"
#include <G4RunManager.hh>
#include <G4SystemOfUnits.hh>
#include <G4OpticalPhoton.hh>
#include <G4AnalysisManager.hh>
// #include <g4analysis.hh>
#include <G4VProcess.hh>
#include "HistoManager.hh"
#include "G4Utils.hh"

#include "AbsDetector.hh"
// For analysis manager
// https://geant4-userdoc.web.cern.ch/UsersGuides/ForApplicationDeveloper/BackupVersions/V10.7/html/Analysis/managers.html
// #include "g4analysis.hh"

OpticalSteppingAction::OpticalSteppingAction(OpticalEventAction *opticalEventAction) : G4UserSteppingAction(), fOpticalEventAction(
    opticalEventAction) {
}

OpticalSteppingAction::~OpticalSteppingAction() {
}

void OpticalSteppingAction::UserSteppingAction(const G4Step *step) {
  // Refer to OpNoviceSteppingAction.cc

  // Ensure particle type is optical photon
  if (step->GetTrack()->GetDefinition() != G4OpticalPhoton::Definition())
    return;

  // Ensure photon reached corresponding detector's cathode (PMT or MPPC).
  // And obtain detector's parent single volume copy number
  G4int copyNumber = G4Utils::getDetectorCopyNumber(step);
  if (copyNumber == -1)
    return;

  // Determine photon energy and wavelength
  G4double energy = step->GetTrack()->GetDynamicParticle()->GetKineticEnergy() / eV; // in eV
  G4double waveLength = G4Utils::getWavelength(step->GetTrack()); // in nm

  // Quantum efficiency
  AbsDetector *detector = G4Utils::getDetector();
  G4double efficiency = detector->getEfficiency(waveLength); //  in percent
  G4double peNumber = efficiency / 100.;

  // Populate Analysis Manager with TOF information (for sum of first 100 events and individual events too)
  G4Track *track = step->GetTrack();
  G4String creatorProcess = G4Utils::getCreatorProcessSafe(track);

  // Output
  if (G4RunManager::GetRunManager()->GetVerboseLevel() >= 2) {
    printf("OP track ID %d. ", step->GetTrack()->GetTrackID());
    printf("Volume \"%s\". ", step->GetPreStepPoint()->GetTouchableHandle()->GetVolume()->GetName().c_str());
    printf("Detector #%d. ", copyNumber);
    printf("E=%.2f eV, ", energy);

    G4VUserTrackInformation *userInfo = track->GetUserInformation();
    OpticalUserTrackInformation *info = static_cast<OpticalUserTrackInformation*>(userInfo);
    if (info) {
      G4double originalWavelength = info->wavelength;
      printf("wl_0=%.1f nm, ", originalWavelength);
    }

    printf("wl=%.1f nm, ", waveLength);
    printf("track=%d mm, ", (G4int) step->GetTrack()->GetTrackLength());
    printf("eff=%.1f %%, ", efficiency);
    printf("PE=%.2f, ", peNumber);
    printf("Proc=%s\n", creatorProcess.c_str());
  }

  // G4double trackGlobalTime = step->GetTrack()->GetGlobalTime();
  // G4double trackLocalTime = step->GetTrack()->GetLocalTime();
  // G4double trackProperTime = step->GetTrack()->GetProperTime();

  // G4double preGlobalTime = step->GetPreStepPoint()->GetGlobalTime();
  // G4double preLocalTime = step->GetPreStepPoint()->GetLocalTime();
  // G4double preProperTime = step->GetPreStepPoint()->GetProperTime();

  // G4double postGlobalTime = step->GetPostStepPoint()->GetGlobalTime();
  // G4double postLocalTime = step->GetPostStepPoint()->GetLocalTime();
  // G4double postProperTime = step->GetPostStepPoint()->GetProperTime();

  // If saving time of flight "/detector/saveTimeOfFlight"
  if (G4Utils::getDetectorConstruction()->GetSaveTimeOfFlight()) {

    G4VUserTrackInformation *userInfo = track->GetUserInformation();
    OpticalUserTrackInformation *info = static_cast<OpticalUserTrackInformation*>(userInfo);

    G4double trackBeginningGlobalTime = 0;
    // G4double trackBeginningLocalTime = 0;
    // G4double trackBeginningProperTime = 0;

    if (info) {
      trackBeginningGlobalTime = info->globalTime;
      // trackBeginningLocalTime = info->localTime;
      // trackBeginningProperTime = info->properTime;
    }

    G4AnalysisManager *analysisManager = G4AnalysisManager::Instance();
    G4double globalTime = step->GetTrack()->GetGlobalTime();
    // Since beginning of the track
    G4double localTime = step->GetTrack()->GetLocalTime();

    G4int eventNumber = G4RunManager::GetRunManager()->GetCurrentEvent()->GetEventID();
    if (eventNumber < G4Utils::maxTofEvents) {
      // Fill current event ntuple
      G4int currentNtupleId = G4Utils::getInstance()->getNTupleNumber(eventNumber);
      analysisManager->FillNtupleSColumn(currentNtupleId, 0, creatorProcess);
      analysisManager->FillNtupleDColumn(currentNtupleId, 1, peNumber);
      analysisManager->FillNtupleDColumn(currentNtupleId, 2, globalTime);
      analysisManager->FillNtupleDColumn(currentNtupleId, 3, localTime);
      analysisManager->FillNtupleDColumn(currentNtupleId, 4, trackBeginningGlobalTime);
      analysisManager->AddNtupleRow(currentNtupleId);

      // Fill all events ntuple
      G4int firstTOFNtupleId = G4Utils::getInstance()->getNTupleNumber(-1);
      analysisManager->FillNtupleSColumn(firstTOFNtupleId, 0, creatorProcess);
      analysisManager->FillNtupleDColumn(firstTOFNtupleId, 1, peNumber);
      analysisManager->FillNtupleDColumn(firstTOFNtupleId, 2, globalTime);
      analysisManager->FillNtupleDColumn(firstTOFNtupleId, 3, localTime);
      analysisManager->FillNtupleDColumn(firstTOFNtupleId, 4, trackBeginningGlobalTime);
      analysisManager->AddNtupleRow(firstTOFNtupleId);
    }
  }
  // Fill creator process histogram
  HistoManager::getInstance()->fillCreatorHist(creatorProcess.c_str());

  // Collected data per event is passed to the OpticalEventAction
  // Increase count of optical photons in correspondent photo-cathode
  // TODO: separate into registered Cherenkov and Scintillation photons
  fOpticalEventAction->saveDetectedPhoton(creatorProcess, copyNumber);

  // Save number of photo-electrons to OpticalEventAction
  fOpticalEventAction->savePhotoElectron(peNumber, creatorProcess, copyNumber);

  // Event independent information (e.g. photon wavelength spectrum reached detector)
  // is directly passed to the Histogram Manager
  HistoManager::getInstance()->SaveDetectedPhoton(track, copyNumber);

  // Terminate track
  step->GetTrack()->SetTrackStatus(fStopAndKill);
}
