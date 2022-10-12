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
/// \file hadronic/Hadr03/include/DetectorMessenger.hh
/// \brief Definition of the DetectorMessenger class
//
// $Id: DetectorMessenger.hh 67103 2013-01-31 18:18:03Z maire $
//
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#ifndef DetectorMessenger_h
#define DetectorMessenger_h 1

#include <globals.hh>
#include <G4UImessenger.hh>
#include <G4UIcmdWith3VectorAndUnit.hh>

class DetectorConstruction;
class G4UIdirectory;
class G4UIcmdWithAString;
class G4UIcmdWithADoubleAndUnit;
class G4UIcmdWithAnInteger;
class G4UIcmdWithABool;

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

class DetectorMessenger: public G4UImessenger
{
  public:
  
    DetectorMessenger(DetectorConstruction* );
   ~DetectorMessenger();
    
    virtual void SetNewValue(G4UIcommand*, G4String);

  private:
    // Detector instance for

    DetectorConstruction*      fDetectorConstruction;

    G4UIdirectory* fDirectory;
    G4UIcmdWithADoubleAndUnit* fSetGapCmd;
    G4UIcmdWith3VectorAndUnit* fSetCrystalSizeCmd;
    G4UIcmdWithAString* fSetCrystalMaterialCmd;
    G4UIcmdWithAString* fSetOuterCrystalMaterialCmd;
    G4UIcmdWithAString* fSetWrapMaterialCmd;
    G4UIcmdWithAString* fSetDetectorTypeCmd;
    G4UIcmdWithAnInteger* fSetCrystalNumberXCmd;
    G4UIcmdWithAnInteger* fSetCrystalNumberYCmd;
    G4UIcmdWithADoubleAndUnit* fSetGreaseThicknessCmd;
    G4UIcmdWithADoubleAndUnit* fSetWrapThicknessCmd;
    G4UIcmdWithAString* fSetGreaseTypeCmd;
    G4UIcmdWithABool* fSetReflectiveShieldCmd;

    G4UIcmdWithABool* fUsePrimitiveScorerCmd;
    G4UIcmdWithABool* fUseScoringMeshesCmd;
    G4UIcmdWithABool* fSaveEnergyWorldEscapeCmd;
    G4UIcmdWithABool* fSaveTimeOfFlightCmd;
    G4UIcmdWithABool* fUseUnitVolumeScoringMeshesCmd;

  public:
    static const G4bool usePrimitiveScorerCmdDefaultValue = true;
    static const G4bool useScoringMeshesCmdDefaultValue = true;
    static const G4bool saveEnergyWorldEscapeCmdDefaultValue = true;
    static const G4bool useUnitVolumeScoringMeshesCmdDefaultValue = true;
    static const G4bool saveTimeOfFlightCmdDefaultValue = false;
};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#endif

