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
/// \file OpNovice/src/OpticalStackingAction.cc
/// \brief Implementation of the OpticalStackingAction class
//
//
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#include "OpticalStackingAction.hh"

#include "G4ios.hh"
#include "G4ParticleDefinition.hh"
#include "G4ParticleTypes.hh"
#include "G4RunManager.hh"
#include "G4LogicalVolume.hh"
#include "G4PVPlacement.hh"
#include "G4Track.hh"
#include "G4VProcess.hh"
#include "G4Utils.hh"
#include "HistoManager.hh"
#include "OpticalUserTrackInformation.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

OpticalStackingAction::OpticalStackingAction(OpticalEventAction* opticalEventAction)
  : G4UserStackingAction(),
	fOpticalEventAction(opticalEventAction)
{}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

OpticalStackingAction::~OpticalStackingAction() {}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4ClassificationOfNewTrack OpticalStackingAction::ClassifyNewTrack(const G4Track* track) {
  if(track->GetDefinition() == G4OpticalPhoton::OpticalPhotonDefinition()){
  	// particle is optical photon

    if(track->GetParentID() > 0) {
    	// particle is secondary

      // track->GetStep() is NULL, therefore we are getting the
      // copy number from track->GetOriginTouchable()->GetCopyNumber()
      // This seems to work. However, sometimes simulation crashes.
      // Fix: let's do a NULL check.
      const G4VTouchable* touchable = track->GetOriginTouchable();
      if (!touchable) return fUrgent;

      // Did not help... analyzed the Geant4 code and figured this may help...
      if (touchable->GetHistoryDepth() == 0) return fUrgent;

      G4int copyNumber = touchable->GetCopyNumber(1);

      // Ensure that the particle track belongs to the crystal
      // and obtain crystal's parent single volume copy number
      if (copyNumber != -1){
        // OpticalEventAction accumulates numbers of produced photons per event
        G4String creatorProcess = G4Utils::getCreatorProcessSafe(track);
        fOpticalEventAction->saveProducedPhoton(creatorProcess, copyNumber);

        // Event independent information (e.g. photon wavelength spectrum reached detector)
        // is directly passed to the Histogram Manager
        HistoManager::getInstance()->SaveProducedPhoton(track, copyNumber);
      }
    }

    // Add timing information when optical photon was born:
    G4double localTime = track->GetLocalTime();
    G4double globalTime = track->GetGlobalTime();
    G4double properTime = track->GetProperTime();

    G4double wavelength = G4Utils::getWavelength(track);

    OpticalUserTrackInformation* info = new OpticalUserTrackInformation(localTime , globalTime, properTime, wavelength);
    track->SetUserInformation(info);
  }
  return fUrgent;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void OpticalStackingAction::NewStage()
{
  // run->AddScintillation((G4double) fScintillationCounter);
  // run->AddCerenkov((G4double) fCerenkovCounter);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void OpticalStackingAction::PrepareNewEvent()
{
//  fOpticalEventAction->fTotalPhotons = 0;
//  fOpticalEventAction->fScintillationPhotons = 0;
//  fOpticalEventAction->fCherenkovPhotons = 0;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
