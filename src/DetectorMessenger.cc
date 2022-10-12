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
/// \file hadronic/Hadr03/src/DetectorMessenger.cc
/// \brief Implementation of the DetectorMessenger class
//
// $Id: DetectorMessenger.cc 70755 2013-06-05 12:17:48Z ihrivnac $
//
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#include "DetectorMessenger.hh"

#include "DetectorConstruction.hh"
#include "G4UIdirectory.hh"
#include "G4UIcmdWithAString.hh"
#include "G4UIcmdWithADoubleAndUnit.hh"
#include "G4UIcmdWithAnInteger.hh"
#include "G4UIcmdWithABool.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

DetectorMessenger::DetectorMessenger(DetectorConstruction *Det) : G4UImessenger(), fDetectorConstruction(Det) {
  // Instantiate the Command directory
  fDirectory = new G4UIdirectory("/detector/");
  fDirectory->SetGuidance("Commands for Detector construction");

  // Instantiate command for setting the gap between crystals
  fSetGapCmd = new G4UIcmdWithADoubleAndUnit("/detector/setGap", this);
  fSetGapCmd->SetGuidance("Set the gap between the crystals.");
  fSetGapCmd->SetUnitCategory("Length");
  fSetGapCmd->SetParameterName("choice", false);
  fSetGapCmd->AvailableForStates(G4ApplicationState::G4State_PreInit);

  // Instantiate command for setting crystal size
  fSetCrystalSizeCmd = new G4UIcmdWith3VectorAndUnit("/detector/setCrystalSize", this);
  fSetCrystalSizeCmd->SetGuidance("Set crystal size");
  fSetCrystalSizeCmd->SetDefaultUnit("mm");
  fSetCrystalSizeCmd->AvailableForStates(G4ApplicationState::G4State_PreInit);

  // Instantiate command for setting crystal Material
  fSetCrystalMaterialCmd = new G4UIcmdWithAString("/detector/setCrystalMaterial", this);
  fSetCrystalMaterialCmd->SetGuidance("Set crystal material type, \"BaSi2O5\" (default) or \"PbWO4\"");
  fSetCrystalMaterialCmd->AvailableForStates(G4ApplicationState::G4State_PreInit);

  // Instantiate command for setting outer crystal Material layer
  fSetOuterCrystalMaterialCmd = new G4UIcmdWithAString("/detector/setOuterCrystalMaterial", this);
  fSetOuterCrystalMaterialCmd->SetGuidance("Set outer crystals material type, \"BaSi2O5\" (default) or \"PbWO4\"");
  fSetOuterCrystalMaterialCmd->AvailableForStates(G4ApplicationState::G4State_PreInit);

  // Instantiate command for setting outer crystal Material layer
  fSetWrapMaterialCmd = new G4UIcmdWithAString("/detector/setWrapMaterial", this);
  fSetWrapMaterialCmd->SetGuidance("Set outer crystals material type, \"BaSi2O5\" (default) or \"PbWO4\"");
  fSetWrapMaterialCmd->AvailableForStates(G4ApplicationState::G4State_PreInit);

  // Instantiate command for setting PMT diameter
  fSetDetectorTypeCmd = new G4UIcmdWithAString("/detector/setDetectorType", this);
  fSetDetectorTypeCmd->SetGuidance(
      "Set PMT type, \"R1425\" (or \"R1450\" - default ~ 18 mm) or \"R2059\" (~ 53 mm) or \"S13360-6025CS\" for MPPC");
  fSetDetectorTypeCmd->AvailableForStates(G4ApplicationState::G4State_PreInit);

  // Instantiate command for crystal wrap thickness
  fSetWrapThicknessCmd = new G4UIcmdWithADoubleAndUnit("/detector/setWrapThickness", this);
  fSetWrapThicknessCmd->SetGuidance("Set crystal wrap thickness");
  fSetWrapThicknessCmd->SetDefaultUnit("um");
  fSetWrapThicknessCmd->AvailableForStates(G4ApplicationState::G4State_PreInit);

  // Instantiate command for PMT grease thickness
  fSetGreaseThicknessCmd = new G4UIcmdWithADoubleAndUnit("/detector/setGreaseThickness", this);
  fSetGreaseThicknessCmd->SetGuidance("Set PMT grease thickness");
  fSetGreaseThicknessCmd->SetDefaultUnit("mm");
  fSetGreaseThicknessCmd->AvailableForStates(G4ApplicationState::G4State_PreInit);

  // Command for grease type
  fSetGreaseTypeCmd = new G4UIcmdWithAString("/detector/setGreaseType", this);
  fSetGreaseTypeCmd->SetGuidance("Set PMT grease type - BC630 or PDMS");
  fSetGreaseTypeCmd->AvailableForStates(G4ApplicationState::G4State_PreInit);

  // Instantiate commands for setting number of crystals
  fSetCrystalNumberXCmd = new G4UIcmdWithAnInteger("/detector/setCrystalNumberX", this);
  fSetCrystalNumberXCmd->SetGuidance("Number of crystals along X axis");
  fSetCrystalNumberXCmd->AvailableForStates(G4ApplicationState::G4State_PreInit);
  fSetCrystalNumberYCmd = new G4UIcmdWithAnInteger("/detector/setCrystalNumberY", this);
  fSetCrystalNumberYCmd->SetGuidance("Number of crystals along Y axis");
  fSetCrystalNumberYCmd->AvailableForStates(G4ApplicationState::G4State_PreInit);

  fSetReflectiveShieldCmd = new G4UIcmdWithABool("/detector/setReflectiveShield", this);
  fSetReflectiveShieldCmd->SetGuidance("Whether the detector shield is reflective");
  fSetReflectiveShieldCmd->AvailableForStates(G4ApplicationState::G4State_PreInit);
  fSetReflectiveShieldCmd->SetDefaultValue(false);

  fUsePrimitiveScorerCmd = new G4UIcmdWithABool("/detector/usePrimitiveScorer", this);
  fUsePrimitiveScorerCmd->SetGuidance("Register energy deposition in crystals via PrimitiveScorer on/off");
  fUsePrimitiveScorerCmd->SetDefaultValue(usePrimitiveScorerCmdDefaultValue);

  fUseScoringMeshesCmd = new G4UIcmdWithABool("/detector/useGlobalScoringMeshes", this);
  fUseScoringMeshesCmd->SetGuidance("Visualize and save energy deposition in crystals and global mesh on/off");
  fUseScoringMeshesCmd->SetDefaultValue(useScoringMeshesCmdDefaultValue);

  fSaveTimeOfFlightCmd = new G4UIcmdWithABool("/detector/saveTimeOfFlight", this);
  fSaveTimeOfFlightCmd->SetGuidance("Save time-of-flight information of the detected photons on/off");
  fSaveTimeOfFlightCmd->SetDefaultValue(saveTimeOfFlightCmdDefaultValue);

  fSaveEnergyWorldEscapeCmd = new G4UIcmdWithABool("/detector/saveEnergyWorldEscape", this);
  fSaveEnergyWorldEscapeCmd->SetGuidance("Control output of the particles escaping the World.");
  fSaveEnergyWorldEscapeCmd->SetDefaultValue(saveEnergyWorldEscapeCmdDefaultValue);

  fUseUnitVolumeScoringMeshesCmd = new G4UIcmdWithABool("/detector/useUnitVolumeScoringMeshes", this);
  fUseUnitVolumeScoringMeshesCmd->SetGuidance("Visualize and save energy deposition in unit volumes on/off");
  fUseUnitVolumeScoringMeshesCmd->SetDefaultValue(useUnitVolumeScoringMeshesCmdDefaultValue);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

DetectorMessenger::~DetectorMessenger() {
  delete fSetGapCmd;
  delete fSetCrystalSizeCmd;
  delete fSetCrystalMaterialCmd;
//  delete fDirectory;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void DetectorMessenger::SetNewValue(G4UIcommand *command, G4String newValue) {
  if (command == fSetGapCmd) {
    fDetectorConstruction->SetDetectorGap(fSetGapCmd->GetNewDoubleValue(newValue));
  } else if (command == fSetCrystalSizeCmd) {
    fDetectorConstruction->SetCrystalSize(fSetCrystalSizeCmd->GetNew3VectorValue(newValue));
  } else if (command == fSetCrystalMaterialCmd) {
    fDetectorConstruction->SetCrystalMaterial(newValue);
  } else if (command == fSetOuterCrystalMaterialCmd) {
    fDetectorConstruction->SetOuterCrystalMaterial(newValue);
  } else if (command == fSetWrapMaterialCmd) {
    fDetectorConstruction->SetWrapMaterial(newValue);
  } else if (command == fSetCrystalNumberXCmd) {
    fDetectorConstruction->SetCrystalNumberX(fSetCrystalNumberXCmd->GetNewIntValue(newValue));
  } else if (command == fSetCrystalNumberYCmd) {
    fDetectorConstruction->SetCrystalNumberY(fSetCrystalNumberYCmd->GetNewIntValue(newValue));
  } else if (command == fSetDetectorTypeCmd) {
    fDetectorConstruction->SetDetectorType(newValue);
  } else if (command == fSetGreaseThicknessCmd) {
    fDetectorConstruction->SetGreaseThickness(fSetGreaseThicknessCmd->GetNewDoubleValue(newValue));
  } else if (command == fSetWrapThicknessCmd) {
    fDetectorConstruction->SetWrapThickness(fSetGreaseThicknessCmd->GetNewDoubleValue(newValue));
  } else if (command == fSetGreaseTypeCmd) {
    fDetectorConstruction->SetGreaseType(newValue);
  } else if (command == fSetReflectiveShieldCmd) {
    fDetectorConstruction->SetShieldReflective(fSetReflectiveShieldCmd->GetNewBoolValue(newValue));
  } else if (command == fUsePrimitiveScorerCmd) {
    fDetectorConstruction->SetUsePrimitiveScorer(fUsePrimitiveScorerCmd->GetNewBoolValue(newValue));
  } else if (command == fUseScoringMeshesCmd) {
    fDetectorConstruction->SetUseGlobalScoringMesh(fUseScoringMeshesCmd->GetNewBoolValue(newValue));
  } else if (command == fSaveEnergyWorldEscapeCmd) {
    fDetectorConstruction->SetSaveWorldEscapeEnergy(fSaveEnergyWorldEscapeCmd->GetNewBoolValue(newValue));
  } else if (command == fSaveTimeOfFlightCmd) {
    fDetectorConstruction->SetSaveTimeOfFlight(fSaveTimeOfFlightCmd->GetNewBoolValue(newValue));
  } else if (command == fUseUnitVolumeScoringMeshesCmd) {
    fDetectorConstruction->SetUseUnitVolumeScoringMesh(fUseUnitVolumeScoringMeshesCmd->GetNewBoolValue(newValue));
  }
}

//G4String DetectorMessenger::GetCurrentValue(G4UIcommand* command){
//  if (command == fSetGapCmd){
//    return fSetGapCmd->ConvertToString(fDetectorConstruction->GetDetectorGap());
//  } else if (command == fSetCrystalSizeCmd){
//    fSetCrystalSizeCmd->ConvertToString(*(fDetectorConstruction->GetCrystalSize()));
//  } else if (command==fSetCrystalMaterialCmd){
//    return fDetectorConstruction->GetCrystalMaterial();
//  }
//  return "";
//}
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
