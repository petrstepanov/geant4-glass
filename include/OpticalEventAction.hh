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
/// \file OpticalEventAction.hh
/// \brief Definition of the OpticalEventAction class

#ifndef OpticalEventAction_h
#define OpticalEventAction_h 1

#include "G4UserEventAction.hh"

#include "G4THitsMap.hh"
#include "G4Track.hh"
#include "globals.hh"
/// Event action class
///
/// In EndOfOpticalEventAction(), it prints the accumulated quantities of the energy
/// deposit and track lengths of charged particles in Absober and Gap layers
/// stored in the hits collections.

class OpticalEventAction : public G4UserEventAction
{
public:
  OpticalEventAction();
  virtual ~OpticalEventAction();

  void BeginOfEventAction(const G4Event* event) override;
  void EndOfEventAction(const G4Event* event) override;

  // Methods to increase count of photons/photoelectrons detected in the specific crystal
  void saveProducedPhoton(G4String creatorProcess, G4int copyNumber);
  void saveDetectedPhoton(G4String creatorProcess, G4int copyNumber);
  void savePhotoElectron(G4double efficiency, G4String creatorProcess, G4int copyNumber);

private:
//  HistoManager* fHistoManager;

  // Produced numbers of scintillation and Cherenkov photons per event
  G4int* fPhotonsProduced;
  G4int* fScintPhotonsProduced;
  G4int* fCherePhotonsProduced;

  // Produced numbers of scintillation and Cherenkov photons per event
  G4int* fPhotonsDetected;
  G4int* fScintPhotonsDetected;
  G4int* fCherePhotonsDetected;

  // Number of photo-electrons output in each crystal per event
  G4double* fPhotoElectrons;
  G4double* fScintPhotoElectrons;
  G4double* fCherePhotoElectrons;

};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#endif
