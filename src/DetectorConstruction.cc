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
/// \file hadronic/Hadr03/src/DetectorConstruction.cc
/// \brief Implementation of the DetectorConstruction class
//
// $Id: DetectorConstruction.cc 70755 2013-06-05 12:17:48Z ihrivnac $
//

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#include "DetectorConstruction.hh"
#include "DetectorMessenger.hh"

#include "G4Box.hh"
#include "G4Material.hh"
#include "G4NistManager.hh"

#include "G4SDManager.hh"
#include "G4AutoDelete.hh"

#include "G4Tubs.hh"
#include "G4LogicalVolume.hh"
#include "G4PVPlacement.hh"

#include "G4GeometryManager.hh"
#include "G4PhysicalVolumeStore.hh"
#include "G4LogicalVolumeStore.hh"
#include "G4SolidStore.hh"
#include "G4RunManager.hh"

#include "G4SystemOfUnits.hh"
#include "G4PhysicalConstants.hh"
#include "G4UnitsTable.hh"

#include "G4Box.hh"
#include "G4Sphere.hh"
#include "G4PVReplica.hh"
#include "G4VisAttributes.hh"//coloring

#include "G4SubtractionSolid.hh"
#include "G4Utils.hh"

#include "G4LogicalBorderSurface.hh"
#include "G4OpticalSurface.hh"

#include "G4MultiFunctionalDetector.hh"
#include "G4PSEnergyDeposit.hh"
#include "G4PSEnergyDeposit3D.hh"

#include "Materials.hh"

#include "G4ScoringManager.hh"
#include "G4VScoringMesh.hh"
#include "G4ScoringBox.hh"

#include "G4LogicalSkinSurface.hh"

#include "R1450PMT.hh"
#include "R2059PMT.hh"
#include "R2257PMT.hh"
#include "R1828_01PMT.hh"
#include "S13360_6025CS.hh"
#include "S13360_6025CS_2x2.hh"
#include "S13360_6025CS_20.hh"
#include "S13360_6025CS_Custom.hh"

#include "MyConstants.hh"
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

DetectorConstruction::DetectorConstruction() : G4VUserDetectorConstruction(), detector(0),

// Materials
fWorldMater(0), fDetectorMater(0), fOuterDetectorMater(0), fGreaseMater(0), fPMTBodyMater(0),
//	 fPMTCathodeMater(0),
fPMTVacuumMater(0), fWrapMater(0), fPMTShieldMater(0), fPMTreflectorMater(0),

//   fMPPCMater(0),
fMPPCShieldMater(0),

// Physical Volumes
fPhysiWorld(0),
//   fMotherPhysicalVolume(0),

// Logical Volumes
fLogicDetector(0), fLogicCrystal(0), fLogicGrease(0), fLogicPMTCase(0), fLogicPMTWindow(0), fLogicPMTVacuum(0), fLogicPMTCathode(
    0), fLogicWrap(0), fLogicPMTshield(0), fLogicPMTReflector(0),

fLogicMPPC(0), fLogicMPPCWindow(0), fLogicMPPCShield(0),

fMom_X(0), fMom_Y(0),

fEDepScorerCrystal(0),

fSingleVolumes { },

fCrystalType("PWO-CRYTUR"), fOuterCrystalType(""), fWrapMaterName("") {

  // Default size of crystals (can be overrided in DetectorMessenger)
  fCrystal_X = 20 * mm;
  fCrystal_Y = 20 * mm;
  fCrystal_Z = 200 * mm;

  // Default number of crystals in the assembly
  fCrystalNumX = 3;
  fCrystalNumY = 3;

  // Thickness of the wrap materials (default)
  fWrapThickness = 65 * micrometer; // 10*mm;

  // Gap between crystals
  fGap = 150 * micrometer;

  // Optical grease
  fGreaseThickness = 0.2 * mm; //0.2*mm;

  // Default grease type
  fGreaseType = GreaseType::BC630;

  // Reflectiveness of the shield
  isShieldReflective = false;

  // Custom UImessenger for Detector geometry modification
  fDetectorMessenger = new DetectorMessenger(this);

  // Default values for using Primitive Scorer
  fUsePrimitiveScorer = fDetectorMessenger->usePrimitiveScorerCmdDefaultValue;

  // Write world escape energy and locations
  fUseWorldEscapeAction = fDetectorMessenger->saveEnergyWorldEscapeCmdDefaultValue;

  // Write time of flight information
  fUseTimeOfFlight = fDetectorMessenger->saveTimeOfFlightCmdDefaultValue;

  // Default value for using Scoring Mesh
  fUseGlobalScoringMesh = fDetectorMessenger->useScoringMeshesCmdDefaultValue;
  fUseUnitVolumeScoringMesh = fDetectorMessenger->useUnitVolumeScoringMeshesCmdDefaultValue;
  fNumberOfUnitVolumes = 10;

  // Instantiate the materials databse
  Materials::getInstance();

  // Save quantum efficiency curves
  G4String outputPath = "./output/detectors/";
  (new R1450PMT())->saveToFile(outputPath);
  (new R2059PMT())->saveToFile(outputPath);
  (new R2257PMT())->saveToFile(outputPath);
  (new R1828_01PMT())->saveToFile(outputPath);
  (new S13360_6025CS())->saveToFile(outputPath);
  (new S13360_6025CS_2x2())->saveToFile(outputPath);
  (new S13360_6025CS_20())->saveToFile(outputPath);
  (new S13360_6025CS_Custom())->saveToFile(outputPath);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

DetectorConstruction::~DetectorConstruction() {
  delete fDetectorMessenger;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4VPhysicalVolume* DetectorConstruction::Construct() {
  ConstructVolumes();
  return fPhysiWorld;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void DetectorConstruction::DefineMaterials() {
  // NIST material database:
  // https://geant4-userdoc.web.cern.ch/UsersGuides/ForApplicationDeveloper/html/Appendix/materialNames.html
  fWorldMater = Materials::getInstance()->getMaterial("air");

  // Define crystal material
  fDetectorMater = Materials::getInstance()->getMaterial(fCrystalType.c_str());
  fOuterDetectorMater =
      fOuterCrystalType.length() == 0 ?
      NULL :
                                        Materials::getInstance()->getMaterial(fOuterCrystalType.c_str());

  // Default wrap material is VM2000 reflective wrap
  fWrapMater = Materials::getInstance()->getMaterial(fWrapMaterName);
  if (!fWrapMater) {
    fWrapThickness = 0;
  }

  // PMT Shield
  fPMTShieldMater = Materials::getInstance()->getMaterial("MuMetal");

  // PMT body and window
  fPMTBodyMater = Materials::getInstance()->getMaterial("borosilicate");

  // PMT vacuum
  fPMTVacuumMater = Materials::getInstance()->getMaterial("vacuum"); // G4NistManager::Instance()->FindOrBuildMaterial("G4_Galactic");

  // PMT Reflective mirror from UltraDetectorConstruction.cc example
  fPMTreflectorMater = G4NistManager::Instance()->FindOrBuildMaterial("G4_Al");

  // Optical grease
  fGreaseMater =
      (fGreaseType == GreaseType::PDMS) ?
          Materials::getInstance()->getMaterial("PDMS") : Materials::getInstance()->getMaterial("BC630");

  // MPPC material
//  fMPPCMater = Materials::getInstance()->getMaterial("borosilicate");
  fMPPCShieldMater = Materials::getInstance()->getMaterial("MuMetal");
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void DetectorConstruction::ConstructVolumes() {

  G4bool checkOverlaps = true;
  // Cleanup old geometry
  //G4GeometryManager::GetInstance()->OpenGeometry();
  //G4PhysicalVolumeStore::GetInstance()->Clean();
  //G4LogicalVolumeStore::GetInstance()->Clean();
  //G4SolidStore::GetInstance()->Clean();

  // Obain materials for the below geometry
  DefineMaterials();

  // START DEFINING THE GEOMETRY
  if (!detector) {
    std::cout << "Detector type not defined. Aborting!" << std::endl;
    return;
  }

  // █░█░█ █▀█ █▀█ █░░ █▀▄
  // ▀▄▀▄▀ █▄█ █▀▄ █▄▄ █▄▀
  // https://fsymbols.com/generators/carty/

  //  G4double fWorld_X = fCrystal_X*fCrystalNumX + 20*cm;
  //  G4double fWorld_Y = fCrystal_Y*fCrystalNumY + 20*cm;
  //  G4double fWorld_Z = (fCrystal_Z + detector->getLength() + 20*cm)*2;

  G4double fWorld_X = (fCrystal_X * fCrystalNumX) * 3;
  G4double fWorld_Y = (fCrystal_Y * fCrystalNumY) * 3;
  G4double fWorld_Z = (fCrystal_Z /* + detector->getLength() + 20 * cm*/) * 3;

  G4double fWorld_max = std::max(fWorld_X, std::max(fWorld_Y, fWorld_Z));
  G4Box *sWorld = new G4Box("World", 0.5 * fWorld_max, 0.5 * fWorld_max, 0.5 * fWorld_max); //its size
  G4LogicalVolume *lWorld = new G4LogicalVolume(sWorld /* solid */, fWorldMater /* material */, "World" /* name */);
  fPhysiWorld = new G4PVPlacement(0 /* no rotation */, G4ThreeVector() /* position */, lWorld /* logical volume */,
                                  "World" /* name */, 0 /* parent logical */, false /* boolean operation */,
                                  0 /* copy number */, checkOverlaps);

  // █▀▄▀█ █▀█ ▀█▀ █░█ █▀▀ █▀█
  // █░▀░█ █▄█ ░█░ █▀█ ██▄ █▀▄
  // Contains NxN matrix of Single volumes (crystal + detector)

  // Detector height is either PMT diameter of MPPC width (height)
  G4double detectorHeight = 0;
  if (AbsPMT *pmt = dynamic_cast<AbsPMT*>(detector)) {
    detectorHeight = pmt->getDiameter();
  } else if (AbsMPPC *mppc = dynamic_cast<AbsMPPC*>(detector)) {
    detectorHeight = mppc->getSize();
  }

  // TODO: make two single volumes. For outer and for inner

  // Single volume wraps crystal and corresponding detector
  G4double single_X = std::max(fCrystal_X + 2 * fWrapThickness + fGap, detectorHeight); // max(crystalWidth, detectorWidth)
  G4double single_Y = std::max(fCrystal_Y + 2 * fWrapThickness + fGap, detectorHeight); // max(crystalHeight, detectorHeight)
  G4double single_Z = fWrapThickness + fCrystal_Z + fGreaseThickness + detector->getLength();

  fMom_X = fCrystalNumX * single_X;
  fMom_Y = fCrystalNumY * single_Y;
  G4double mom_Z = single_Z;

  G4double mom_pos_Z = 0.5 * single_Z;

  G4Box *sMother = new G4Box("mother_solid", 0.5 * fMom_X, 0.5 * fMom_Y, 0.5 * mom_Z);
  G4LogicalVolume *lMother = new G4LogicalVolume(sMother  /* solid */, fWorldMater /* material */, "mother_logical" /* name */);
  new G4PVPlacement(0 /* no rotation */, G4ThreeVector(0, 0, mom_pos_Z) /* position */, lMother /* logical volume */,
                    "mother_physical" /* name */, lWorld /* parent logical */, false /* boolean operation */,
                    0 /* copy number */,  checkOverlaps);

  // █▀ █ █▄░█ █▀▀ █░░ █▀▀
  // ▄█ █ █░▀█ █▄█ █▄▄ ██▄
  // Contains a single assembly of a crystal and sensitive detector. Multiplied NxN times.

  G4Box *singleS = new G4Box("single_solid", 0.5 * single_X, 0.5 * single_Y, 0.5 * single_Z);
  G4LogicalVolume *singleLV = new G4LogicalVolume(singleS,           // solid
      fWorldMater,       // material
      "single_logical"); // name

  // We only need this in hybrid design. However avoiding compiler warnings.
  G4LogicalVolume *singleOutLV = new G4LogicalVolume(singleS,           // solid
      fWorldMater,              // material
      "singleOutside_logical"); // name

  G4double totalLengthX = fCrystalNumX * single_X;
  G4double minX = -totalLengthX / 2 + single_X / 2;
  G4double totalLengthY = fCrystalNumY * single_Y;
  G4double minY = -totalLengthY / 2 + single_Y / 2;
  G4int replicaNumber = 0;
  std::cout << "Single physical volume replicas:" << std::endl;
  fSingleVolumes.clear();
  for (G4int ly = 0; ly < fCrystalNumY; ly++) {
    for (G4int lx = 0; lx < fCrystalNumX; lx++) {
      // Physical volumes of same type can share a logical volume
      G4double x = minX + single_X * lx;
      G4double y = minY + single_Y * ly;

      // G4String singlePhysName = "Single" + std::to_string(replicaNumber);
      G4VPhysicalVolume *pv;
      if (!isHybrid()) {
        // Non-hybrid assembly - one type of material
        pv = new G4PVPlacement(0,                         // rotation
            G4ThreeVector(x, y, 0),    // translation position
            singleLV,                  // logical volume
            "single_physical",         // name
            lMother,               // place the each Singles (which will be containing the cyrstal) inside Mother Volume
            false, replicaNumber, checkOverlaps);
      } else {
        // Hybrid assembly - one material inside, another outside
        G4bool isOutside = G4Utils::isOutsideCrystal(lx, ly, fCrystalNumX, fCrystalNumY);
        pv = new G4PVPlacement(0,                         // rotation
            G4ThreeVector(x, y, 0),    // translation position
            isOutside ? singleOutLV : singleLV, // logical volume
            isOutside ? "singleOutside_physical" : "single_physical",         // name
            lMother,               // place the each Singles (which will be containing the cyrstal) inside Mother Volume
            false, replicaNumber, checkOverlaps);

      }

      // Remember single volume for later use in scoring mesh - mainly for
      fSingleVolumes.push_back(pv);
      replicaNumber++;
      std::cout << "  Y: " << ly + 1 << ", X " << lx + 1 << ": " << pv << std::endl;
      std::cout << "  copy number: " << replicaNumber << std::endl;
    }
  }

  // GEOMETRY: World volume -> Mother volume -> Temperature control box -> Single volume -> Crystal Volume
  G4Box *crystalSolid = new G4Box("crystal_solid", 0.5 * fCrystal_X, 0.5 * fCrystal_Y, 0.5 * fCrystal_Z);
  fLogicCrystal = new G4LogicalVolume(crystalSolid,   // its solid
      fDetectorMater, // its material
      "crystal_logical"); // its name

  // Float crystals to the face of the Single container
  // One logical volume can be placed more than one time
  // so we place it inside both solid and outside solid
  G4double crystalPosZ = -0.5 * single_Z + 0.5 * fCrystal_Z + fWrapThickness;
  G4VPhysicalVolume *crystalPV = new G4PVPlacement(0,   //no rotation
      G4ThreeVector(0, 0, crystalPosZ), // translation
      fLogicCrystal,           // its logical volume
      "crystal_physical",      // its name
      singleLV,                // its parent volume
      false,                   // no boolean operation
      0,                       // copy number
      checkOverlaps);         // checking overlaps

  // We only need this in hybrid design. However avoiding compiler warnings.
  G4VPhysicalVolume *crystalOutPV;
  crystalOutPV = new G4PVPlacement(0,   //no rotation
      G4ThreeVector(0, 0, crystalPosZ), // translation
      fLogicCrystal,                    // its logical volume
      "crystalOutside_physical",        // its name
      singleOutLV,                      // its parent volume
      false,                            // no boolean operation
      0,                                // copy number
      checkOverlaps);                  // checking overlaps

  // GEOMETRY: World volume -> Mother volume -> Temperature control box -> Single volume -> Crystal wrap
  // https://geant4-userdoc.web.cern.ch/UsersGuides/ForApplicationDeveloper/html/Detector/Geometry/geomSolids.html?highlight=g4subtractionsolid#solids-made-by-boolean-operations
  if (fWrapMater) {
    G4Box *wrapASolid = new G4Box("wrapA_solid", 0.5 * (fCrystal_X + 2 * fWrapThickness),
                                  0.5 * (fCrystal_Y + 2 * fWrapThickness), 0.5 * (fCrystal_Z + fWrapThickness));
    G4Box *wrapBSolid = new G4Box("wrapB_solid", 0.5 * fCrystal_X, 0.5 * fCrystal_Y,
                                  0.5 * (fCrystal_Z + fWrapThickness));
    G4RotationMatrix *wrapRot = new G4RotationMatrix();
    G4ThreeVector wrapTrans(0, 0, fWrapThickness);
    // sWrap_end_sol = sWrap - sWrap_end
    G4SubtractionSolid *wrapSolid = new G4SubtractionSolid("wrap_solid", wrapASolid, wrapBSolid, wrapRot, wrapTrans);
    fLogicWrap = new G4LogicalVolume(wrapSolid, fWrapMater, "wrap_logical");
    G4double wrap_pos_Z = -(0.5 * single_Z - 0.5 * (fCrystal_Z + fWrapThickness));
    G4VPhysicalVolume *wrapPV = new G4PVPlacement(0, G4ThreeVector(0, 0, wrap_pos_Z), fLogicWrap, "wrap_physical",
                                                  singleLV, false, 0, checkOverlaps);

    G4VisAttributes* wrapVisAttributes = new G4VisAttributes();
    wrapVisAttributes->SetForceCloud();
    wrapVisAttributes->SetForceNumberOfCloudPoints(1E3);
    fLogicWrap->SetVisAttributes(wrapVisAttributes);

    // Optical Surface for crystal wrap
    // Define surface properties depending on the material
    G4OpticalSurface *opWrapperSurface = new G4OpticalSurface("WrapperOpticalSurface");
    if (fWrapMater == Materials::getInstance()->getMaterial("C10H8O4")) {    // If vm2000
      opWrapperSurface->SetType(dielectric_LUT);
      opWrapperSurface->SetModel(LUT);
      opWrapperSurface->SetFinish(polishedvm2000air);
    } else if (fWrapMater == Materials::getInstance()->getMaterial("G4_TEFLON")) {  // If teflon
      opWrapperSurface->SetType(dielectric_LUT);
      opWrapperSurface->SetModel(LUT);
      opWrapperSurface->SetFinish(polishedteflonair);
    }
    // Instantiate optical surface for regular or hybrid geometry
    new G4LogicalBorderSurface("WrapperSurface", crystalPV, wrapPV, opWrapperSurface);
    if (isHybrid()) {
      G4VPhysicalVolume *wrapOutPV = new G4PVPlacement(0, G4ThreeVector(0, 0, wrap_pos_Z), fLogicWrap,
                                                       "wrapOutside_physical", singleOutLV, false, 0, checkOverlaps);
      new G4LogicalBorderSurface("WrapperOutSurface", crystalOutPV, wrapOutPV, opWrapperSurface);
    }
  }
  // We only need this in hybrid design. However avoiding compiler warnings.
//  G4VPhysicalVolume* wrapOutPV;
//  if (isHybrid()){
//     wrapOutPV = new G4PVPlacement(0,
//                                   G4ThreeVector(0, 0, wrap_pos_Z),
//                                   fLogicWrap,
//                                   "wrapOutside_physical",
//                                   singleOutLV,
//                                   false,
//                                   0,
//                                   checkOverlaps);
//  }

  // █▀█ █▀▄▀█ ▀█▀
  // █▀▀ █░▀░█ ░█░
  // If PMT type detector

  if (AbsPMT *pmt = dynamic_cast<AbsPMT*>(detector)) {

    // GEOMETRY: World volume -> Mother volume -> Temperature control box -> Single volume -> Optical grease
    if (fGreaseThickness > 0) {
      G4Tubs *greaseSolid = new G4Tubs("grease_solid", 0, pmt->getDiameter() / 2, fGreaseThickness / 2, 0, twopi);

      fLogicGrease = new G4LogicalVolume(greaseSolid, fGreaseMater, "grease_logical");
      G4double greasePosZ = 0.5 * single_Z - 0.5 * fGreaseThickness - detector->getLength();
      new G4PVPlacement(0, G4ThreeVector(0, 0, greasePosZ), fLogicGrease, "grease_physical", singleLV, false, 0,
                        checkOverlaps);
      if (isHybrid()) {
        new G4PVPlacement(0, G4ThreeVector(0, 0, greasePosZ), fLogicGrease, "greaseOutside_physical", singleOutLV,
                          false, 0, checkOverlaps);
      }
    }

    // GEOMETRY: World volume -> Mother volume -> Temperature control box -> Single volume -> PMT
    G4Tubs *sPMTCase = new G4Tubs("pmt_solid", 0, pmt->getDiameter() / 2, pmt->getLength() / 2, 0, twopi);

    fLogicPMTCase = new G4LogicalVolume(sPMTCase, fPMTBodyMater, "pmt_logical");
    G4double PMT_pos_Z = single_Z / 2 - pmt->getLength() / 2; // Float PMTs to far end
    new G4PVPlacement(0, G4ThreeVector(0, 0, PMT_pos_Z), fLogicPMTCase, "pmt_physical", singleLV, false, 0,
                      checkOverlaps);
    if (isHybrid()) {
      new G4PVPlacement(0, G4ThreeVector(0, 0, PMT_pos_Z), fLogicPMTCase, "pmtOutside_physical", singleOutLV, false, 0,
                        checkOverlaps);
    }

    // GEOMETRY: World volume -> Mother volume -> Temperature control box -> Single volume -> PMT -> Window
    G4Tubs *pmtWindowSolid = new G4Tubs("pmtWindow_solid", 0, pmt->getCathodeDiameter() / 2,
                                        pmt->getWindowThickness() / 2, 0, twopi);
    fLogicPMTWindow = new G4LogicalVolume(pmtWindowSolid, pmt->getWindowMaterial(), "pmtWindow_logical");
    G4double pmtWindowPosZ = -pmt->getLength() / 2 + pmt->getWindowThickness() / 2;
    new G4PVPlacement(0, G4ThreeVector(0, 0, pmtWindowPosZ), fLogicPMTWindow, "pmtWindow_physical", fLogicPMTCase,
                      false, 0, checkOverlaps);

    // GEOMETRY: World volume -> Mother volume -> Temperature control box -> Single volume -> PMT -> Photocathode
    // Make cathode material same as window maerial to prevent reflections
    // G4Material* pmtCathodeMaterial = pmt->getWindowMaterial();
    G4Tubs *pmtCathodeSolid = new G4Tubs("pmtCathode_solid", 0, pmt->getCathodeDiameter() / 2,
                                         pmt->getCathodeThickness() / 2, 0, twopi);
    fLogicPMTCathode = new G4LogicalVolume(pmtCathodeSolid, pmt->getCathodeMaterial(), "pmtCathode_logical");
    G4double pmtCathodePosZ = -pmt->getLength() / 2 + pmt->getWindowThickness() + pmt->getCathodeThickness() / 2;
    new G4PVPlacement(0, G4ThreeVector(0, 0, pmtCathodePosZ), fLogicPMTCathode, "pmtCathode_physical", fLogicPMTCase,
                      false, 0, checkOverlaps);

    // GEOMETRY: World volume -> Mother volume -> Temperature control box -> Single volume -> PMT -> Vacuum
    G4double pmtWallThickness = (pmt->getDiameter() - pmt->getCathodeDiameter()) / 2;
    G4double pmtVacuumLength = pmt->getLength() - pmt->getWindowThickness() - pmt->getCathodeThickness()
                               - pmtWallThickness;
    G4Tubs *pmtVacuumSolid = new G4Tubs("pmtVacuum_solid", 0, pmt->getCathodeDiameter() / 2, pmtVacuumLength / 2, 0,
                                        twopi);
    fLogicPMTVacuum = new G4LogicalVolume(pmtVacuumSolid, fPMTVacuumMater, "pmtVacuum_logical");
    G4VisAttributes *logicPMTVacuumVisAttr = new G4VisAttributes();
    logicPMTVacuumVisAttr->SetForceWireframe();
    // logicPMTshieldVisAttr->SetForceAuxEdgeVisible();
    fLogicPMTVacuum->SetVisAttributes(logicPMTVacuumVisAttr);

    G4double pmtVacuumPosZ = -pmt->getLength() / 2 + pmtVacuumLength / 2 + pmt->getWindowThickness()
                             + pmt->getCathodeThickness();
    new G4PVPlacement(0, G4ThreeVector(0, 0, pmtVacuumPosZ), fLogicPMTVacuum, "pmtVacuum_physical", fLogicPMTCase,
                      false, 0, checkOverlaps);

    // GEOMETRY: World volume -> Mother volume -> Temperature control box -> Single volume -> PMT -> Vacuum -> Reflector
    /*
     G4double pmtReflectorLength = fPMT_cathode_distance + fPMT_cathode_thickness;
     G4Tubs* pmtReflectorSolid = new G4Tubs("pmtReflectorSolid", fPMT_window_radius - fPMT_reflector_thickness, fPMT_window_radius, pmtReflectorLength/2, 0, twopi);
     G4double pmtReflectorPosZ = - pmtVacuumLength/2 + pmtReflectorLength/2;

     fLogicPMTReflector = new G4LogicalVolume(pmtReflectorSolid, fPMTreflectorMater, "pmtReflectorLog");
     new G4PVPlacement(0,
     G4ThreeVector(0, 0, pmtReflectorPosZ),
     fLogicPMTReflector,
     "pmtReflectorPhys",
     fLogicPMTVacuum,
     false,
     0,
     checkOverlaps);

     // Reflector surface - copied from UltraDetectotConstruction.cc
     G4OpticalSurface* pmtReflectorOpticalSurface = new G4OpticalSurface("ReflectorOpticalSurface");
     pmtReflectorOpticalSurface->SetType(dielectric_metal);
     pmtReflectorOpticalSurface->SetFinish(polished);
     pmtReflectorOpticalSurface->SetModel(unified);

     // const G4int NUM = 2;
     // G4double lambda_min = 200*nm ;
     // G4double lambda_max = 700*nm ;
     // G4double XX[NUM] = {h_Planck*c_light/lambda_max, h_Planck*c_light/lambda_min} ;
     // G4double ICEREFLECTIVITY[NUM] = { 0.95, 0.95 };

     // G4MaterialPropertiesTable *VacuumMirrorMPT = new G4MaterialPropertiesTable();
     // VacuumMirrorMPT->AddProperty("REFLECTIVITY", XX, ICEREFLECTIVITY,NUM);
     // pmtReflectorOpticalSurface->SetMaterialPropertiesTable(VacuumMirrorMPT);

     new G4LogicalSkinSurface("pmtReflectorSkinSurface", fLogicPMTReflector, pmtReflectorOpticalSurface);
     */

    // GEOMETRY: World volume -> Mother volume -> Temperature control box -> Single volume -> PMT Magnetic Shield
    G4Box *sPMTcoverOuter = new G4Box("pmtCoverOut_solid", 0.5 * (single_X), 0.5 * (single_Y), 0.5 * pmt->getLength());
    G4Tubs *sPMTcoverInner = new G4Tubs("pmtCoverIn_solid", 0, pmt->getDiameter() / 2, pmt->getLength() / 2 + 1, 0,
                                        twopi); // + 1 just in case for better subtraction

    G4RotationMatrix *coverRot = new G4RotationMatrix();
    G4ThreeVector coverTrans(0, 0, 0);
    G4SubtractionSolid *sPMTcover = new G4SubtractionSolid("pmtCover_solid", sPMTcoverOuter, sPMTcoverInner, coverRot,
                                                           coverTrans);

    fLogicPMTshield = new G4LogicalVolume(sPMTcover, fPMTShieldMater, "pmtCover_logical");
    G4VisAttributes *logicPMTshieldVisAttr = new G4VisAttributes();
    logicPMTshieldVisAttr->SetForceWireframe();
    // logicPMTshieldVisAttr->SetForceAuxEdgeVisible();
    fLogicPMTshield->SetVisAttributes(logicPMTshieldVisAttr);

    new G4PVPlacement(0, G4ThreeVector(0, 0, PMT_pos_Z), fLogicPMTshield, "pmtCover_physical", singleLV, false, 0,
                      checkOverlaps);
    if (isHybrid()) {
      new G4PVPlacement(0, G4ThreeVector(0, 0, PMT_pos_Z), fLogicPMTshield, "pmtShieldOutside_physical", singleOutLV,
                        false, 0, checkOverlaps);
    }

    // Make shield surface reflective
    if (isShieldReflective) {
      addReflectiveSkin(fLogicPMTshield);
    }
  }

  // █▀▄▀█ █▀█ █▀█ █▀▀
  // █░▀░█ █▀▀ █▀▀ █▄▄
  // If MPPC type detector

  else if (AbsMPPC *mppc = dynamic_cast<AbsMPPC*>(detector)) {

    // GEOMETRY: World volume -> Mother volume -> Temperature control box -> Single volume -> Optical grease
    if (fGreaseThickness > 0) {
      G4Box *greaseSolid = new G4Box("grease_solid", mppc->getSize() / 2, mppc->getSize() / 2, fGreaseThickness / 2);

      fLogicGrease = new G4LogicalVolume(greaseSolid, fGreaseMater, "grease_logical");
      G4double greasePosZ = 0.5 * single_Z - 0.5 * fGreaseThickness - detector->getLength();
      new G4PVPlacement(0, G4ThreeVector(0, 0, greasePosZ), fLogicGrease, "grease_physical", singleLV, false, 0,
                        checkOverlaps);
      if (isHybrid()) {
        new G4PVPlacement(0, G4ThreeVector(0, 0, greasePosZ), fLogicGrease, "greaseOutside_physical", singleOutLV,
                          false, 0, checkOverlaps);
      }
    }

    // GEOMETRY: World volume -> Mother volume -> Temperature control box -> Single volume -> MPPC Reflector
    G4Box *mppcReflectorOuter = new G4Box("mppcReflectorOuterSolid", 0.5 * (single_X), 0.5 * (single_Y),
                                          0.5 * mppc->getLength());
    G4Box *mppcReflectorInner = new G4Box("mppcReflectorInnerSolid", 0.5 * mppc->getSize(), 0.5 * mppc->getSize(),
                                          0.5 * mppc->getLength());
    G4RotationMatrix *reflectorRot = new G4RotationMatrix();
    G4ThreeVector reflectorTrans(0, 0, 0);
    G4SubtractionSolid *mppcReflectorSolid = new G4SubtractionSolid("mppcReflector_solid", mppcReflectorOuter,
                                                                    mppcReflectorInner, reflectorRot, reflectorTrans);

    fLogicMPPCShield = new G4LogicalVolume(mppcReflectorSolid, fMPPCShieldMater, "mppcCase_logical");
    G4double mppcShieldPosZ = 0.5 * single_Z - fGreaseThickness - detector->getLength() / 2;
    new G4PVPlacement(0, G4ThreeVector(0, 0, mppcShieldPosZ), fLogicMPPCShield, "mppcCase_physical", singleLV, false,
                      0, checkOverlaps);
    if (isHybrid()) {
      new G4PVPlacement(0, G4ThreeVector(0, 0, mppcShieldPosZ), fLogicMPPCShield, "mppcCaseOutside_physical",
                        singleOutLV, false, 0, checkOverlaps);
    }
    // Make shield surface reflective
    if (isShieldReflective) {
      addReflectiveSkin(fLogicMPPCShield);
    }

    G4VisAttributes *logicMPPCshieldVisAttr = new G4VisAttributes();
    logicMPPCshieldVisAttr->SetForceWireframe();
    // logicPMTshieldVisAttr->SetForceAuxEdgeVisible();
    fLogicMPPCShield->SetVisAttributes(logicMPPCshieldVisAttr);

    // GEOMETRY: World volume -> Mother volume -> Temperature control box -> Single volume -> MPPC Window
    G4Box *mppcWindowSolid = new G4Box("MPPC_Win_sol", 0.5 * mppc->getSize(), 0.5 * mppc->getSize(),
                                       0.5 * mppc->getWindowThickness());
    fLogicMPPCWindow = new G4LogicalVolume(mppcWindowSolid, mppc->getWindowMaterial(), "mppcWindow_logical");
    G4double mppcWindowPosZ = 0.5 * single_Z - detector->getLength() + detector->getWindowThickness() / 2;
    new G4PVPlacement(0, G4ThreeVector(0, 0, mppcWindowPosZ), fLogicMPPCWindow, "mppcWindow_physical", singleLV, false,
                      0, checkOverlaps);
    if (isHybrid()) {
      new G4PVPlacement(0, G4ThreeVector(0, 0, mppcWindowPosZ), fLogicMPPCWindow, "mppcWindowOut_physical", singleLV,
                        false, 0, checkOverlaps);
    }

    // GEOMETRY: World volume -> Mother volume -> Temperature control box -> Single volume -> MPPC Cathode
    G4Box *mppcSolid = new G4Box("mppc_solid", 0.5 * mppc->getSize(), 0.5 * mppc->getSize(),
                                 0.5 * mppc->getCathodeThickness());
    fLogicMPPC = new G4LogicalVolume(mppcSolid, mppc->getCathodeMaterial(), "mppc_logical");
    G4double mppcPosZ = 0.5 * single_Z - 0.5 * detector->getCathodeThickness();
    new G4PVPlacement(0, G4ThreeVector(0, 0, mppcPosZ), fLogicMPPC, "mppc_physical", singleLV, false, 0,
                      checkOverlaps);
    if (isHybrid()) {
      new G4PVPlacement(0, G4ThreeVector(0, 0, mppcPosZ), fLogicMPPC, "mppcOutside_physical", singleOutLV, false, 0,
                        checkOverlaps);
    }
  }
  /*
   G4Box* sFrame_outer = new G4Box("Frame_outer_sol", 0.5*single_X, 0.5*single_Y, 0.5*fFrame_length);
   G4Box* sFrame_inner = new G4Box("Frame_inner_sol", 0.5*(single_X - fGap), 0.5*(single_Y - fGap), 0.5*fFrame_length);
   G4SubtractionSolid* sFrame = new G4SubtractionSolid("Frame_sol", sFrame_outer, sFrame_inner);

   G4LogicalVolume* lFrame = new G4LogicalVolume(sFrame,
   fFrameMater,
   "Frame_log");

   new G4PVPlacement(0,
   G4ThreeVector(0., 0., -(0.5*single_Z - (0.5*fFrame_length + fWrapThickness))),
   lFrame,
   "Frame_pos1",
   lSingle,
   false,
   0,
   checkOverlaps);

   new G4PVPlacement(0,
   G4ThreeVector(0., 0., 0.5*single_Z - (0.5*fFrame_length + fPMTcoverThickness + fPMT_length)),
   lFrame,
   "Frame_pos2",
   lSingle,
   false,
   0,
   checkOverlaps);
   */

  // Crystal skin surface
//  G4OpticalSurface* crystalSurface = new G4OpticalSurface("CrystalOpticalSurface");
//  crystalSurface->SetType(dielectric_dielectric);
//  crystalSurface->SetModel(unified);
//  crystalSurface->SetFinish(ground);
//  crystalSurface->SetSigmaAlpha(0.1);
//  new G4LogicalSkinSurface("CrystalSurface", fLogicCrystal, crystalSurface);
  // G4MaterialPropertiesTable* opWS = new G4MaterialPropertiesTable();
  // opWrapperSurface->SetMaterialPropertiesTable(opWS);
  // Optical Surface for Crystal and Grease
//  G4OpticalSurface* opWrapperFrontSurface = new G4OpticalSurface("WrapperFrontSurface");
//  opWrapperFrontSurface->SetType(dielectric_LUT);
//  opWrapperFrontSurface->SetFinish(polishedvm2000air);
//  opWrapperFrontSurface->SetModel(LUT);
//  new G4LogicalBorderSurface("WrapperFrontSurface", crystalPos, fWrapFrontPos, opWrapperFrontSurface);
//  G4MaterialPropertiesTable* opWFS = new G4MaterialPropertiesTable();
//  opWFS->DumpTable();
//  opWrapperFrontSurface->SetMaterialPropertiesTable(opWFS);
//  G4OpticalSurface* opPMTSurface = new G4OpticalSurface("PMTSurface");
//  opPMTSurface->SetType(dielectric_dielectric);
//  opPMTSurface->SetFinish(polished);
//  opPMTSurface->SetModel(unified);
//  new G4LogicalBorderSurface("PMTSurface", crystalPos,fPMTpos,opPMTSurface);
//  G4MaterialPropertiesTable* opPS = new G4MaterialPropertiesTable();
//  opPS->DumpTable();
//  opPMTSurface->SetMaterialPropertiesTable(opPS);
//  G4OpticalSurface* opPMTcoverSurface = new G4OpticalSurface("PMTcoverSurface");
//  opPMTcoverSurface->SetType(dielectric_metal);
//  opPMTcoverSurface->SetFinish(polished);
//  opPMTcoverSurface->SetModel(unified);
//  const  G4int nEntriesPMTcover = 2;
//  G4double PhotonEnergyPMTcover[nEntriesPMTcover] = { 2.30*eV, 3.26*eV};
//  G4double reflectivityPMTcover[nEntriesPMTcover] = {0., 0.};
//  G4double efficiencyPMTcover[nEntriesPMTcover] = {1., 1.};
//  new G4LogicalBorderSurface("PMTcoverSurface", fPMTpos,fPMTcoverPos,opPMTcoverSurface);
//  G4MaterialPropertiesTable* opPcS = new G4MaterialPropertiesTable();
//  opPcS -> AddProperty("REFLECTIVITY",PhotonEnergyPMTcover,reflectivityPMTcover,nEntriesPMTcover);
//  opPcS -> AddProperty("EFFICIENCY",PhotonEnergyPMTcover,efficiencyPMTcover,nEntriesPMTcover);
//  opPcS->DumpTable();
//  opPMTcoverSurface->SetMaterialPropertiesTable(opPcS);

  // COLORS
  // https://geant4-userdoc.web.cern.ch/UsersGuides/ForApplicationDeveloper/BackupVersions/V10.7/html/Visualization/attributes.html#visibility
//  lWorld->SetVisAttributes(G4VisAttributes::GetInvisible());
//  lMother->SetVisAttributes(G4VisAttributes::GetInvisible());

//  singleLV->SetVisAttributes(new G4VisAttributes(G4Colour::Gray()));
//  if (singleOutLV!=NULL) singleOutLV->SetVisAttributes(new G4VisAttributes(G4Colour::Red()));

//  singleLV->SetVisAttributes(new G4VisAttributes(G4VisAttributes::GetInvisible()));
//  if (singleOutLV != NULL)
//    singleOutLV->SetVisAttributes(G4VisAttributes::GetInvisible());

//  if (fLogicPMTCase)
//    fLogicPMTCase->SetVisAttributes(new G4VisAttributes(G4Colour::Brown()));
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
void DetectorConstruction::ConstructSDandField() {
  // Primitive Scorer registers energy deposition in every crystal
  if (fUsePrimitiveScorer) {
    G4MultiFunctionalDetector *detectorCrystal = new G4MultiFunctionalDetector(
        MultiFunctionalDetectorNames::CRYSTAL_VOLUME);

    // Second parameter - depth, needed for determining the copy number of the logical volume
    fEDepScorerCrystal = new G4PSEnergyDeposit(HitsCollectionNames::ENE_DEP_CRYSTAL_VOLUME, 1);
    detectorCrystal->RegisterPrimitive(fEDepScorerCrystal);

    // Attach Sensitive Detector to crystal's Logical Volume (one logical, but many physical volumes)
    fLogicCrystal->SetSensitiveDetector(detectorCrystal);

    G4SDManager::GetSDMpointer()->AddNewDetector(detectorCrystal);
  }
  // TODO: is this deleted or not after the run?
//  else {
//    G4VSensitiveDetector* det = G4SDManager::GetSDMpointer()->FindSensitiveDetector("crystal");
//    if (det){
//      std::cout << "detector found";
//    }
//  }

  // Scoring mesh for energy deposition visualization

  // Petr Stepanov: now we are doing this in a parallel world
  if (fUseGlobalScoringMesh) {
    InitGlobalScoringMesh();
  }

  if (fUseUnitVolumeScoringMesh) {
    InitUnitVolumeScoringMesh();
  }

  // TODO: check if scoring manager is deleted is this deleted or not after the run?
  //       If not Override Scoring Manager and add mesh deletion
  //       delete singleton like shown here: https://stackoverflow.com/questions/8699434/how-to-delete-singleton-pointer/8699599
//	else {

//	  DestroyScoringManager();
//	}

  G4ScoringManager *scoringManager = G4ScoringManager::GetScoringManager();
  scoringManager->List();
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void DetectorConstruction::SetDetectorGap(G4double value) {
  fGap = value * mm;
  G4RunManager::GetRunManager()->ReinitializeGeometry();
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4double DetectorConstruction::GetDetectorGap() {
  return fGap;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void DetectorConstruction::SetCrystalSize(G4ThreeVector vector) {
  fCrystal_X = vector.getX();
  fCrystal_Y = vector.getY();
  if (vector.z() != fCrystal_Z) {
    fCrystal_Z = vector.getZ();
    // Materials::getInstance()->setCrystalLength(fCrystal_Z);
    // DefineMaterials();
  }
  std::cout << "DetectorConstruction::SetCrystalSize()" << std::endl;
  std::cout << "- setting crystal size to " << vector.getX() << "x" << vector.getY() << "x" << vector.getZ()
            << std::endl;

  // TODO: check if Materials were re-initialized with correct crystal length!
  G4RunManager::GetRunManager()->ReinitializeGeometry(true);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4ThreeVector* DetectorConstruction::GetCrystalSize() {
  return new G4ThreeVector(fCrystal_X, fCrystal_Y, fCrystal_Z);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void DetectorConstruction::SetCrystalMaterial(G4String material) {
  fCrystalType = material;
  // DefineMaterials();
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4String DetectorConstruction::GetCrystalMaterial() {
  return fCrystalType;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void DetectorConstruction::SetOuterCrystalMaterial(G4String material) {
  fOuterCrystalType = material;
  // DefineMaterials();
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4String DetectorConstruction::GetOuterCrystalMaterial() {
  return fOuterCrystalType;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void DetectorConstruction::SetCrystalNumberX(G4int num) {
  fCrystalNumX = num;
  G4cout << "DetectorConstruction::SetCrystalNumberX()" << G4endl;
  G4cout << "- setting crystal number along X axis to " << num << G4endl;

  // Notify Run Manager that geometry was modified
  G4RunManager::GetRunManager()->GeometryHasBeenModified();
}

G4int DetectorConstruction::GetCrystalNumberX() {
  return fCrystalNumX;
}

void DetectorConstruction::SetCrystalNumberY(G4int num) {
  fCrystalNumY = num;
  G4cout << "DetectorConstruction::SetCrystalNumberY()" << G4endl;
  G4cout << "- setting crystal number along Y axis to " << num << G4endl;
  // G4RunManager::GetRunManager()->ReinitializeGeometry(true);
}

G4int DetectorConstruction::GetCrystalNumberY() {
  return fCrystalNumY;
}

G4VPrimitiveScorer* DetectorConstruction::GetEdepScorer() {
  return fEDepScorerCrystal;
}

//G4TwoVector* DetectorConstruction::GetMomXY(){
//  return new G4TwoVector(fMom_X, fMom_Y);
//}

// Petr Stepanov: initialize scoring manager for Interactive visualization
//                of the energy deposition in the crystal and PMT assembly

void DetectorConstruction::InitGlobalScoringMesh() {
  // Petr Stepanov: copied from G4ScoringMessenger.cc
  // Instantiate Scoring Manager for visualization
  G4ScoringManager *scoringManager = G4ScoringManager::GetScoringManager();

  // CRYSTALS MESH
  G4Utils::checkCurrentMeshIsClosed();

  // Define the box Mesh with name "crystalsMesh": /score/create/boxMesh crystalsMesh
  G4Utils::checkUniqueMeshName(MultiFunctionalDetectorNames::CRYSTALS_MESH);
  G4VScoringMesh *mesh = new G4ScoringBox(MultiFunctionalDetectorNames::CRYSTALS_MESH);
  scoringManager->RegisterScoringMesh(mesh);

  // Set mesh size to wrap around all crystal volumes (and wrap) in XY plane and have length of a crystal in Z
  // Example in macro file: /score/mesh/boxSize 31. 31. 100. mm
  G4double size[3];
  size[0] = (fCrystalNumX * (fCrystal_X + 2 * fWrapThickness) + (fCrystalNumX - 1) * fGap) / 2;
  size[1] = (fCrystalNumY * (fCrystal_Y + 2 * fWrapThickness) + (fCrystalNumY - 1) * fGap) / 2;
  size[2] = (fCrystal_Z + fWrapThickness) / 2;
  mesh->SetSize(size);

  // Set number of bins in the Mesh: /score/mesh/nBin 30 30 100
  // We will use 5 segments per crystal width
  G4int transverseSegmentsPerCrystal = 5;
  G4int segments[3];
  segments[0] = fCrystalNumX * transverseSegmentsPerCrystal;
  segments[1] = fCrystalNumY * fCrystal_Y / fCrystal_X * transverseSegmentsPerCrystal;
  segments[2] = fCrystal_Z / fCrystal_X * transverseSegmentsPerCrystal;
  mesh->SetNumberOfSegments(segments);

  // Set Mesh position: /score/mesh/translate/xyz 0. 0. 2100. mm
  G4double centerPosition[3];
  centerPosition[0] = 0;
  centerPosition[1] = 0;
  centerPosition[2] = (fWrapThickness + fCrystal_Z) / 2;
  mesh->SetCenterPosition(centerPosition);

  // Set Mesh primitive scorer: /score/quantity/energyDeposit eneDep
  G4Utils::checkUniqueScorerName(mesh, HitsCollectionNames::ENE_DEP_CRYSTALS_MESH);
  G4PSEnergyDeposit *primitiveScorer = new G4PSEnergyDeposit3D(HitsCollectionNames::ENE_DEP_CRYSTALS_MESH);
  // Important: when assigning primitive scorer to the mesh, then basically
  // Multi-functional detector inside mesh registers primitive scorer
  // G4MultiFunctinoalDetetor::RegisterPrimitive(scorer);
  mesh->SetPrimitiveScorer(primitiveScorer);

  // Close the Mesh: /score/close
  scoringManager->CloseCurrentMesh();

  // PMT MESH
  G4Utils::checkCurrentMeshIsClosed();

  // Define the box Mesh with name "pmtMesh"
  G4Utils::checkUniqueMeshName(MultiFunctionalDetectorNames::PMTS_MESH);
  mesh = new G4ScoringBox(MultiFunctionalDetectorNames::PMTS_MESH);
  scoringManager->RegisterScoringMesh(mesh);

  // Set mesh size to wrap around all single volumes in XY plane and have length of a PMT in Z
  G4double size2[3];
  size2[0] = fMom_X / 2;
  size2[1] = fMom_Y / 2;
  size2[2] = detector->getLength() / 2;
  mesh->SetSize(size2);

  // Set number of bins in the Mesh: /score/mesh/nBin 30 30 100
  G4int segments2[3];
  segments2[0] = segments[0];
  segments2[1] = segments[1];
  segments2[2] = detector->getLength() / fCrystal_X * transverseSegmentsPerCrystal;
  mesh->SetNumberOfSegments(segments2);

  // Set Mesh position: /score/mesh/translate/xyz 0. 0. 2100. mm
  G4double centerPosition2[3];
  centerPosition2[0] = 0;
  centerPosition2[1] = 0;
  centerPosition2[2] = fWrapThickness + fCrystal_Z + detector->getLength() / 2;
  mesh->SetCenterPosition(centerPosition2);

  // Set Mesh primitive scorer: /score/quantity/energyDeposit eneDep
  G4Utils::checkUniqueScorerName(mesh, HitsCollectionNames::ENE_DEP_PMTS_MESH);
  G4PSEnergyDeposit *pmtEdepScorer = new G4PSEnergyDeposit3D(HitsCollectionNames::ENE_DEP_PMTS_MESH);
  mesh->SetPrimitiveScorer(pmtEdepScorer);

  // Close the Mesh: /score/close
  scoringManager->CloseCurrentMesh();
}

void DetectorConstruction::InitUnitVolumeScoringMesh() {
  // Petr Stepanov: copied from G4ScoringMessenger.cc
  // Instantiate Scoring Manager for visualization
  G4ScoringManager *scoringManager = G4ScoringManager::GetScoringManager();

  // Define the box Mesh with unique name: /score/create/boxMesh meshName
  for (int i = 0; i < fCrystalNumX * fCrystalNumY; i++) {
    // Check current mesh is closed
    G4Utils::checkCurrentMeshIsClosed();

    // Check mesh with the same name already exists
    G4String meshName = MultiFunctionalDetectorNames::getUnitVolumeMeshName(i);
    G4Utils::checkUniqueMeshName(meshName);

    G4VScoringMesh *mesh = new G4ScoringBox(meshName);
    scoringManager->RegisterScoringMesh(mesh);

    // Set mesh size (from single volume) /score/mesh/boxSize # # # #
//    G4LogicalVolume* singleLogicalVolume = singlePhysicalVolume->GetLogicalVolume();
//    G4VSolid* singleSolid = singleLogicalVolume->GetSolid();
//    G4Box* box = dynamic_cast<G4Box*>(singleSolid);
//    if (box == NULL){
//      std::cout << "Cannot determine mesh size from single volume. Aborting." << std::endl;
//      exit(0);
//    }
    G4double size[3];
    size[0] = fCrystal_X / 2;
    size[1] = fCrystal_Y / 2;
    size[2] = fCrystal_Z / 2;
    mesh->SetSize(size);

    // Set number of bins in the Mesh: /score/mesh/nBin # # #
    G4int segments[3];
    segments[0] = 1;
    segments[1] = 1;
    segments[2] = fNumberOfUnitVolumes;
    mesh->SetNumberOfSegments(segments);

    // Set Mesh position: /score/mesh/translate/xyz 0. 0. 2100. mm
    G4VPhysicalVolume *singlePhysicalVolume = fSingleVolumes[i];
    G4double centerPosition[3];
    centerPosition[0] = singlePhysicalVolume->GetTranslation().x();
    centerPosition[1] = singlePhysicalVolume->GetTranslation().y();
    centerPosition[2] = singlePhysicalVolume->GetTranslation().z();
    centerPosition[2] = centerPosition[2] + fCrystal_Z / 2 + fWrapThickness;
    mesh->SetCenterPosition(centerPosition);

    // Set Mesh primitive scorer: /score/quantity/energyDeposit eneDep
    // TODO: see if multiple meshes can have primitive scorer with same name?
    //       this could fix the error!
    G4String psName = HitsCollectionNames::getUnitVolumeHCName(i);
    G4Utils::checkUniqueScorerName(mesh, psName);

    // Check mesh has primitive scorer
    G4PSEnergyDeposit *edepScorer = new G4PSEnergyDeposit3D(psName);
    mesh->SetPrimitiveScorer(edepScorer);

    // Close the Mesh: /score/close
    scoringManager->CloseCurrentMesh();
  }
}

//void DetectorConstruction::DestroyScoringManager(){
//  // Instantiate Scoring Manager for visualization
//  G4ScoringManager* scoringManager = G4ScoringManager::GetScoringManager();
//
//  // CRYSTALS MESH
//  G4VScoringMesh* mesh = scoringManager->FindMesh("crystalsMesh");
//  if (mesh){
//    std::cout << "mesh found";
//  }
//  if (mesh != NULL){
//    mesh->scoringManager->RegisterScoringMesh(mesh);
//  }
//}

G4double DetectorConstruction::GetDetectorLength() {
  return detector->getLength();
}

void DetectorConstruction::SetDetectorType(G4String pmtType) {
  if (pmtType == "R4125" || pmtType == "R1450") {
    detector = new R1450PMT();
  } else if (pmtType == "R2059" || pmtType == "H3177") {
    detector = new R2059PMT();
  } else if (pmtType == "R1828-01" || pmtType == "H1949-51") {
    detector = new R1828_01PMT();
  } else if (pmtType == "R2257") {
    detector = new R2257PMT();
  } else if (pmtType == "S13360-6025CS") {
    detector = new S13360_6025CS();
  } else if (pmtType == "S13360-6025CS-2x2") {
    detector = new S13360_6025CS_2x2();
  } else if (pmtType == "S13360-6025CS-20") {
    detector = new S13360_6025CS_20();
  } else if (pmtType == "S13360-6025CS-Custom") {
    detector = new S13360_6025CS_Custom();
  }
}

G4String DetectorConstruction::GetDetectorType() {
  return detector->getName();
}

void DetectorConstruction::SetGreaseThickness(G4double thickness) {
  fGreaseThickness = thickness;
}

G4double DetectorConstruction::GetGreaseThickness() {
  return fGreaseThickness;
}

void DetectorConstruction::SetGreaseType(G4String greaseType) {
  if (greaseType == "PDMS") {
    fGreaseType = GreaseType::PDMS;
  } else {
    fGreaseType = GreaseType::BC630;
  }
  // DefineMaterials();
}

G4String DetectorConstruction::GetGreaseType() {
  if (fGreaseType == GreaseType::PDMS) {
    return "PDMS";
  } else if (fGreaseType == GreaseType::BC630) {
    return "BC630";
  }
  return "";
}

AbsDetector* DetectorConstruction::getDetector() {
  return detector;
}

void DetectorConstruction::SetShieldReflective(G4bool value) {
  isShieldReflective = value;
}
G4bool DetectorConstruction::GetShieldReflective() {
  return isShieldReflective;
}

void DetectorConstruction::SetUsePrimitiveScorer(G4bool value) {
  fUsePrimitiveScorer = value;

  // Notify Run Manager that geometry was modified
  G4RunManager::GetRunManager()->GeometryHasBeenModified();
}

G4bool DetectorConstruction::GetUsePrimitiveScorer() {
  return fUsePrimitiveScorer;
}

void DetectorConstruction::SetSaveWorldEscapeEnergy(G4bool value) {
  fUseWorldEscapeAction = value;
  G4RunManager::GetRunManager()->GeometryHasBeenModified();
}

G4bool DetectorConstruction::GetSaveWorldEscapeEnergy() {
  return fUseWorldEscapeAction;
}

void DetectorConstruction::SetSaveTimeOfFlight(G4bool value) {
  fUseTimeOfFlight = value;
  G4RunManager::GetRunManager()->GeometryHasBeenModified();
}

G4bool DetectorConstruction::GetSaveTimeOfFlight() {
  return fUseTimeOfFlight;
}

void DetectorConstruction::SetUseGlobalScoringMesh(G4bool value) {
  fUseGlobalScoringMesh = value;
  G4RunManager::GetRunManager()->GeometryHasBeenModified();
}

G4bool DetectorConstruction::GetUseGlobalScoringMesh() {
  return fUseGlobalScoringMesh;
}

void DetectorConstruction::SetUseUnitVolumeScoringMesh(G4bool value) {
  fUseUnitVolumeScoringMesh = value;

  // Notify Run Manager that geometry was modified
  G4RunManager::GetRunManager()->GeometryHasBeenModified();
}

G4bool DetectorConstruction::GetUseUnitVolumeScoringMesh() {
  return fUseUnitVolumeScoringMesh;
}

void DetectorConstruction::addReflectiveSkin(G4LogicalVolume *volumeLog) {
  G4OpticalSurface *opMPPCReflectorSurface = new G4OpticalSurface("opMPPCReflectorSurface");
  opMPPCReflectorSurface->SetType(dielectric_LUT);
  opMPPCReflectorSurface->SetModel(LUT);
  opMPPCReflectorSurface->SetFinish(polishedvm2000air);
  new G4LogicalSkinSurface("opMPPCReflectorSkinSurface", volumeLog, opMPPCReflectorSurface);
}

G4int DetectorConstruction::getNumberOfUnitVolumes() {
  return fNumberOfUnitVolumes;
}

void DetectorConstruction::SetWrapMaterial(G4String wrapMaterial) {
  fWrapMaterName = wrapMaterial;
}

G4String DetectorConstruction::GetWrapMaterial() {
  return fWrapMater->GetName();
}

void DetectorConstruction::SetWrapThickness(G4double wrapThickness) {
  fWrapThickness = wrapThickness;
}

G4double DetectorConstruction::GetWrapThickness() {
  return fWrapThickness;
}

std::vector<G4VPhysicalVolume*> DetectorConstruction::GetSinglePhysicalVolumes() {
  return fSingleVolumes;
}

G4bool DetectorConstruction::isHybrid() {
  return fOuterCrystalType.length() > 0;
}
