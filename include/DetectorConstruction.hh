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
/// \file radioactivedecay/rdecay02/include/DetectorConstruction.hh
/// \brief Definition of the DetectorConstruction class
//
// $Id: DetectorConstruction.hh 66586 2012-12-21 10:48:39Z ihrivnac $
//

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#ifndef DetectorConstruction_h
#define DetectorConstruction_h 1

#include <globals.hh>
#include <G4String.hh>
#include <G4VUserDetectorConstruction.hh>
#include <G4ThreeVector.hh>
#include <G4SystemOfUnits.hh>
#include <G4Material.hh>
#include <G4VPrimitiveScorer.hh>
#include <G4TwoVector.hh>

#include "DetectorMessenger.hh"
#include "AbsDetector.hh"

#include <vector>
#include <string>

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

enum class GreaseType {
  PDMS, BC630
};

// G4VScoringMesh contains G4MultiFunctionalDetector object with the same name.
// So essentially we can think of a Scoring Mesh as an advanced MultiFunctionalDetector

// Namespace with constants
// https://stackoverflow.com/questions/2465546/where-do-i-put-constant-strings-in-c-static-class-members-or-anonymous-namesp#comment2457908_2465546
// https://stackoverflow.com/questions/2879548/is-it-appropriate-to-set-a-value-to-a-const-char-in-the-header-file/2879851#2879851

namespace MultiFunctionalDetectorNames {
  // Constant pointer to a constant char (so nothing about it can be changed)
  // https://stackoverflow.com/questions/890535/what-is-the-difference-between-char-const-and-const-char
  const char* const CRYSTAL_VOLUME = "crystalVolume";           // Name for Primitive Scorer (attached to the crystal logical volume)
  const char* const CRYSTALS_MESH = "crystalsMesh";    // Name for Scoring Mesh that wraps around all crystals
  const char* const PMTS_MESH = "pmtsMesh";            // Name for Scoring Mesh that wraps around all PMTs

  const char* const UNIT_VOLUME_MESH_BASE_NAME = "crystalMesh"; // Base name for Scoring Meshes that wraps individual crystal

  inline const char* getUnitVolumeMeshName(G4int i) {                  // Get name for Scoring Mesh that wraps individual crystal (number suffix to base name)
    G4String* s = new G4String(UNIT_VOLUME_MESH_BASE_NAME);
    *s += std::to_string(i+1);
    return s->c_str();
  }
}

// It may be not effective, but I always used unique pair MultiFunctionalDetectorName/HitsCollectionName
// to register certain value. If using meshes, mesh name must be unique I guess because every mesh is fixed
// in position. But maybe hit collection names can be same for different meshes? Possibly, but its not that important.

// Basically these are PrimitiveScorer names. When MFD registers primitive scorer, HitsMap with
// the same name as PrimitiveScorer is created
namespace HitsCollectionNames {
  const char* const ENE_DEP_CRYSTAL_VOLUME = "eneDepCrystalVolume";  // For scorer that is attached to the crystal logical volume
  const char* const ENE_DEP_CRYSTALS_MESH = "eDepCrystals";          // For scorer that is attached to the global mesh around crystals
  const char* const ENE_DEP_PMTS_MESH = "eDepPmts";                  // For scorer that is attached to the global mesh arouind PMTs
  // It is important that "unit volume" hits collection names are unique
  // Due to the way we clear and populate unit volume meshes in EndOfEventAction
  // Technically, other hits collection names can be same - just "eDep"
  const char* const ENE_DEP_UNIT_VOLUME_MESH_BASE = "eDepCrystal";   // For scorers that are attached to every individual crystal volume

  inline const char* getUnitVolumeHCName(G4int i) {
     G4String* s = new G4String(ENE_DEP_UNIT_VOLUME_MESH_BASE);
     *s += std::to_string(i+1);
     return s->c_str();
  }
}

class DetectorConstruction: public G4VUserDetectorConstruction {
  public:
    DetectorConstruction();
    ~DetectorConstruction();

  public:

    // Mandatory virtual functions
    virtual G4VPhysicalVolume* Construct();
    virtual void ConstructSDandField();

    // Messenger setters and getters
    void SetDetectorGap(G4double value);
    G4double GetDetectorGap();

    void SetCrystalSize(G4ThreeVector vector);
    G4ThreeVector* GetCrystalSize();

    void SetCrystalMaterial(G4String material);
    G4String GetCrystalMaterial();

    void SetOuterCrystalMaterial(G4String material);
    G4String GetOuterCrystalMaterial();

    void SetCrystalNumberX(G4int num);
    G4int GetCrystalNumberX();

    void SetCrystalNumberY(G4int num);
    G4int GetCrystalNumberY();

    G4double GetDetectorLength();

    void SetDetectorType(G4String detectorType);
    G4String GetDetectorType();

    void SetGreaseThickness(G4double thickness);
    G4double GetGreaseThickness();

    void SetGreaseType(G4String greaseType);
    G4String GetGreaseType();

    void SetShieldReflective(G4bool value);
    G4bool GetShieldReflective();

    void SetUsePrimitiveScorer(G4bool);
    G4bool GetUsePrimitiveScorer();

    void SetSaveWorldEscapeEnergy(G4bool);
    G4bool GetSaveWorldEscapeEnergy();

    void SetSaveTimeOfFlight(G4bool);
    G4bool GetSaveTimeOfFlight();

    void SetUseGlobalScoringMesh(G4bool);
    G4bool GetUseGlobalScoringMesh();

    void SetUseUnitVolumeScoringMesh(G4bool);
    G4bool GetUseUnitVolumeScoringMesh();

    // Energy deposit getter
    G4VPrimitiveScorer* GetEdepScorer();

    AbsDetector* getDetector();

    G4int getNumberOfUnitVolumes();

    void SetWrapMaterial(G4String wrapMaterial);
    G4String GetWrapMaterial();

    void SetWrapThickness(G4double wrapThickness);
    G4double GetWrapThickness();

    std::vector<G4VPhysicalVolume*> GetSinglePhysicalVolumes();

    G4bool isHybrid();

  private:
    AbsDetector *detector;

    // Materials
    G4Material *fWorldMater;
    G4Material *fDetectorMater;
    G4Material *fOuterDetectorMater;
    G4Material *fGreaseMater;
    G4Material *fPMTBodyMater;
//  G4Material*          fPMTCathodeMater;
    G4Material *fPMTVacuumMater;
    G4Material *fWrapMater;
    G4Material *fPMTShieldMater;
    G4Material *fPMTreflectorMater;

//  G4Material*          fMPPCMater;
    G4Material *fMPPCShieldMater;

    // Physical Volumes
    G4VPhysicalVolume *fPhysiWorld;
//  G4VPhysicalVolume*   fMotherPhysicalVolume;

    // Logical Volumes
    G4LogicalVolume *fLogicDetector;
    G4LogicalVolume *fLogicCrystal;    // in order to call from ConstructSDandField
    G4LogicalVolume *fLogicGrease;
    G4LogicalVolume *fLogicPMTCase;
    G4LogicalVolume *fLogicPMTWindow;
    G4LogicalVolume *fLogicPMTVacuum;
    G4LogicalVolume *fLogicPMTCathode;
    G4LogicalVolume *fLogicWrap;
    G4LogicalVolume *fLogicPMTshield;
    G4LogicalVolume *fLogicPMTReflector;

    G4LogicalVolume *fLogicMPPC;
    G4LogicalVolume *fLogicMPPCWindow;
    G4LogicalVolume *fLogicMPPCShield;

    G4double fGap;

    G4double fCrystal_X;
    G4double fCrystal_Y;
    G4double fCrystal_Z;

    G4double fMom_X;
    G4double fMom_Y;

    G4int fCrystalNumX;
    G4int fCrystalNumY;

    G4double fWrapThickness;

    G4double fGreaseThickness;
    // G4double             fPMT_reflector_thickness;

    DetectorMessenger *fDetectorMessenger;
    G4VPrimitiveScorer *fEDepScorerCrystal;

    std::vector<G4VPhysicalVolume*> fSingleVolumes;

    void DefineMaterials();
    void ConstructVolumes();

    void InitGlobalScoringMesh();
    void InitUnitVolumeScoringMesh();

    G4bool fUsePrimitiveScorer;
    G4bool fUseWorldEscapeAction;
    G4bool fUseTimeOfFlight;
    G4bool fUseGlobalScoringMesh;
    G4bool fUseUnitVolumeScoringMesh;

    G4int fNumberOfUnitVolumes;

    // Names
    G4String fCrystalType;
    G4String fOuterCrystalType;
    G4String fWrapMaterName;

    GreaseType fGreaseType;

    G4bool isShieldReflective;

    void addReflectiveSkin(G4LogicalVolume *volumeLog);
};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#endif
