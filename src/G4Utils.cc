/*
 * G4Utils.cc
 *
 *  Created on: Nov 29, 2020
 *      Author: petrstepanov
 */

#include <CLHEP/Vector/ThreeVector.h>
#include <DetectorConstruction.hh>
#include <G4Event.hh>
#include <G4Exception.hh>
#include <G4ExceptionSeverity.hh>
#include <G4GeneralParticleSource.hh>
#include <G4HCofThisEvent.hh>
#include <G4ios.hh>
#include <G4ParticleDefinition.hh>
#include <G4Run.hh>
#include <G4RunManager.hh>
#include <G4ScoringManager.hh>
#include <G4SingleParticleSource.hh>
#include <G4SPSEneDistribution.hh>
#include <G4SPSPosDistribution.hh>
#include <G4StatDouble.hh>
#include <G4THitsMap.hh>
#include <G4Timer.hh>
#include <G4Utils.hh>
#include <G4VHitsCollection.hh>
#include <G4VModularPhysicsList.hh>
#include <G4VScoringMesh.hh>
#include <PrimaryGeneratorAction.hh>
#include <RtypesCore.h>
#include <stddef.h>
#include <algorithm>
#include <chrono>
#include <cstdlib>
#include <ctime>
#include <iomanip>
#include <iostream>
#include <G4VProcess.hh>
#include <map>
#include "g4analysis.hh"

using std::chrono::_V2::system_clock;

G4Utils::G4Utils() /* : firstTOFNtupleId(0)*/{
}

const G4int G4Utils::maxTofEvents = 100;

// G4int* G4Utils::TOFNtupleIds = new G4int[maxTofEvents];

G4Utils::~G4Utils() {
  // TODO Auto-generated destructor stub
}

void G4Utils::startTimer(){
  tStart = std::chrono::high_resolution_clock::now();
}

using namespace std::chrono_literals;

void G4Utils::printTimer(){
  // Print time (wall clock)
  // https://en.cppreference.com/w/cpp/chrono/c/clock
  auto tEnd = std::chrono::high_resolution_clock::now();
  std::chrono::duration<float> interval = tEnd - tStart;

  // https://stackoverflow.com/questions/14391327/how-to-get-duration-as-int-millis-and-float-seconds-from-chrono
  std::cout << "Duration is: " << std::chrono::duration_cast<std::chrono::hours>(interval).count() << " hours or "
                               << std::chrono::duration_cast<std::chrono::minutes>(interval).count() << " minutes or "
                               << std::chrono::duration_cast<std::chrono::seconds>(interval).count() << " seconds." << std::endl;
}

G4Utils* G4Utils::instance = NULL;

G4Utils* G4Utils::getInstance(){
    if (!instance){
        instance = new G4Utils();
    }
    return instance;
}

G4double G4Utils::getGPSMonoEnergy() {
  PrimaryGeneratorAction *primaryGeneratorAction =
      (PrimaryGeneratorAction*) G4RunManager::GetRunManager()->GetUserPrimaryGeneratorAction();
  if (primaryGeneratorAction == NULL)
    return 0;

  // PS: we're setting energy via "/gps/ene/mono". Therefore energy is saved
  // in G4SingleParticleSource (check in G4GeneralParticcleSourcceMessenger) which is a member ofG4GeneralParticleSource
  G4GeneralParticleSource *particleGun = primaryGeneratorAction->GetParticleGun();
  G4SingleParticleSource *particleSource = particleGun->GetCurrentSource();
  return particleSource->GetEneDist()->GetMonoEnergy();
}

G4double G4Utils::getGPSZPos() {
  PrimaryGeneratorAction *primaryGeneratorAction =
      (PrimaryGeneratorAction*) G4RunManager::GetRunManager()->GetUserPrimaryGeneratorAction();
  if (primaryGeneratorAction == NULL)
    return 0;

  // PS: we're setting energy via "/gps/ene/mono". Therefore energy is saved
  // in G4SingleParticleSource (check in G4GeneralParticcleSourcceMessenger) which is a member ofG4GeneralParticleSource
  G4GeneralParticleSource *particleGun = primaryGeneratorAction->GetParticleGun();
  G4SingleParticleSource *particleSource = particleGun->GetCurrentSource();
  G4ThreeVector particlePosition = particleSource->GetPosDist()->GetCentreCoords();
  return particlePosition.getZ();
}

G4String G4Utils::getGPSParticleName() {
  PrimaryGeneratorAction *primaryGeneratorAction =
      (PrimaryGeneratorAction*) G4RunManager::GetRunManager()->GetUserPrimaryGeneratorAction();
  if (primaryGeneratorAction == NULL)
    return "";

  // PS: we're setting energy via "/gps/ene/mono". Therefore energy is saved
  // in G4SingleParticleSource (check in G4GeneralParticcleSourcceMessenger) which is a member ofG4GeneralParticleSource
  G4GeneralParticleSource *particleGun = primaryGeneratorAction->GetParticleGun();
  G4SingleParticleSource *particleSource = particleGun->GetCurrentSource();
  return particleSource->GetParticleDefinition()->GetParticleName();
}

G4int G4Utils::getNumberOfEvents() {
  G4RunManager* runManager = G4RunManager::GetRunManager();
  G4int nEvents = runManager->GetNumberOfEventsToBeProcessed();
  return nEvents;
}

DetectorConstruction* G4Utils::getDetectorConstruction(){
  DetectorConstruction *detectorConstruction =
      (DetectorConstruction*) G4RunManager::GetRunManager()->GetUserDetectorConstruction();
  return detectorConstruction;
}

G4String G4Utils::getCrystalMaterial() {
  DetectorConstruction *detectorConstruction =
      (DetectorConstruction*) G4RunManager::GetRunManager()->GetUserDetectorConstruction();
  if (detectorConstruction == NULL)
    return "";
  return detectorConstruction->GetCrystalMaterial();
}

G4ThreeVector* G4Utils::getCrystalSize() {
  DetectorConstruction *detectorConstruction =
      (DetectorConstruction*) G4RunManager::GetRunManager()->GetUserDetectorConstruction();
  if (detectorConstruction == NULL)
    return NULL;
  return detectorConstruction->GetCrystalSize();
}

G4double G4Utils::getCrystalX() {
  G4ThreeVector *vector = getCrystalSize();
  return vector != NULL ? vector->getX() : 0;
}

G4double G4Utils::getCrystalY() {
  G4ThreeVector *vector = getCrystalSize();
  return vector != NULL ? vector->getY() : 0;
}

G4double G4Utils::getCrystalZ() {
  G4ThreeVector *vector = getCrystalSize();
  return vector != NULL ? vector->getZ() : 0;
}

G4int G4Utils::getNCrystalsX() {
  DetectorConstruction *detectorConstruction =
      (DetectorConstruction*) G4RunManager::GetRunManager()->GetUserDetectorConstruction();
  if (detectorConstruction == NULL)
    return 0;
  return detectorConstruction->GetCrystalNumberX();
}

G4int G4Utils::getNCrystalsY() {
  DetectorConstruction *detectorConstruction =
      (DetectorConstruction*) G4RunManager::GetRunManager()->GetUserDetectorConstruction();
  if (detectorConstruction == NULL)
    return 0;
  return detectorConstruction->GetCrystalNumberY();
}

G4int G4Utils::getNCrystals() {
  return getNCrystalsX() * getNCrystalsY();
}

AbsDetector* G4Utils::getDetector() {
  DetectorConstruction *detectorConstruction =
      (DetectorConstruction*) G4RunManager::GetRunManager()->GetUserDetectorConstruction();
  if (detectorConstruction == NULL)
    return 0;
  return detectorConstruction->getDetector();
}

EnergyValueUnit G4Utils::formatEnergy(G4double value){
  if (value >= 1E6){
    EnergyValueUnit evu;
    evu.unit = "TeV";
    evu.value = value/1E6;
    return evu;
  }
  if (value >= 1E3){
    EnergyValueUnit evu;
    evu.unit = "GeV";
    evu.value = value/1E3;
    return evu;
  }
  if (value >= 1){
    EnergyValueUnit evu;
    evu.unit = "MeV";
    evu.value = value;
    return evu;
  }
  EnergyValueUnit evu;
  evu.unit = "keV";
  evu.value = value*1000;
  return evu;
}

G4bool G4Utils::usingOpticalPhysics(){
  G4VUserPhysicsList* userPhysicsList =
    (G4VUserPhysicsList*) G4RunManager::GetRunManager()->GetUserPhysicsList();
  if (userPhysicsList == NULL) return false;

  G4VModularPhysicsList* modularPhysicsList = dynamic_cast<G4VModularPhysicsList*>(userPhysicsList);
  if (modularPhysicsList == NULL) return false;

  return modularPhysicsList->GetPhysics("Optical") != NULL;
}

// Petr Stepanov: Obtain total energy deposited in the Mesh
//                code copied from G4VScoreWriter.cc
G4double G4Utils::getTotalQuantityFromMesh(const char *meshName, const char *psName) {
  G4ScoringManager *scoringManager = G4ScoringManager::GetScoringManager();
  G4VScoringMesh *fScoringMesh = scoringManager->FindMesh(meshName);

  using MeshScoreMap = G4VScoringMesh::MeshScoreMap;

  // retrieve the map
  MeshScoreMap fSMap = fScoringMesh->GetScoreMap();

  MeshScoreMap::const_iterator msMapItr = fSMap.find(psName);
  if (msMapItr == fSMap.end()) {
    G4cerr << "ERROR : DumpToFile : Unknown quantity, \"" << psName << "\"." << G4endl;
    return 0;
  }

  std::map<G4int, G4StatDouble*> *score = msMapItr->second->GetMap();

  G4double unitValue = fScoringMesh->GetPSUnitValue(psName);
  G4String unit = fScoringMesh->GetPSUnit(psName);
  G4String divisionAxisNames[3];
  fScoringMesh->GetDivisionAxisNames(divisionAxisNames);

  // Get number of the mesh segments
  G4int fNMeshSegments[3] = { 0 }; // number of segments of the mesh
  fScoringMesh->GetNumberOfSegments(fNMeshSegments);

  G4double fact = 1;
  G4double total = 0;
  G4double grandTotal = 0;
  // G4double totalVal2 = 0;
  // G4double entry = 0;

  for (int x = 0; x < fNMeshSegments[0]; x++) {
    for (int y = 0; y < fNMeshSegments[1]; y++) {
      for (int z = 0; z < fNMeshSegments[2]; z++) {
        G4int idx = x*fNMeshSegments[1]*fNMeshSegments[2] +y*fNMeshSegments[2]+z;
        std::map<G4int, G4StatDouble*>::iterator value = score->find(idx);

        if (value == score->end()) {
          total = 0;
          // totalVal2 = 0;
          // entry = 0;
        } else {
          total = (value->second->sum_wx())/unitValue*fact;
          // totalVal2 = (value->second->sum_wx2()) / unitValue / unitValue * fact * fact;
          // entry = value->second->n();
        }

        grandTotal += total;
      }
    }
  }
  return grandTotal;
}

std::vector<G4double> G4Utils::getUnitVolumesQuantityFromMesh(const char* meshName, const char* psName) {
  G4ScoringManager *scoringManager = G4ScoringManager::GetScoringManager();
  G4VScoringMesh *fScoringMesh = scoringManager->FindMesh(meshName);

//  std::cout << "# mesh name: " << fScoringMesh->GetWorldName() << std::endl;

  using MeshScoreMap = G4VScoringMesh::MeshScoreMap;
  // retrieve the map
  MeshScoreMap fSMap = fScoringMesh->GetScoreMap();


  MeshScoreMap::const_iterator msMapItr = fSMap.find(psName);
  if(msMapItr == fSMap.end()) {
    G4cerr << "ERROR : DumpToFile : Unknown quantity, \""
     << psName << "\"." << G4endl;
    std::vector<G4double> v = {};
    return v;
  }

  std::map<G4int, G4StatDouble*> * score = msMapItr->second->GetMap();
//  std::cout << "# primitive scorer name: " << msMapItr->first << std::endl;

  double fact = 1;

  G4double unitValue = fScoringMesh->GetPSUnitValue(psName);
  G4String unit = fScoringMesh->GetPSUnit(psName);
  G4String divisionAxisNames[3];
  fScoringMesh->GetDivisionAxisNames(divisionAxisNames);
  // index order
//  std::cout << "# i" << divisionAxisNames[0]
//  << ", i" << divisionAxisNames[1]
//  << ", i" << divisionAxisNames[2];
//  // unit of scored value
//  std::cout << ", total(value) ";
//  if(unit.size() > 0) std::cout << "[" << unit << "]";
//  std::cout << ", total(val^2), entry" << std::endl;


  // write quantity
  std::cout << std::setprecision(16); // for double value with 8 bytes

  G4int fNMeshSegments[3];
  fScoringMesh->GetNumberOfSegments(fNMeshSegments);
  std::vector<G4double> v = {};
  for(int x = 0; x < fNMeshSegments[0]; x++) {
    for(int y = 0; y < fNMeshSegments[1]; y++) {
      for(int z = 0; z < fNMeshSegments[2]; z++) {
        G4int idx = x*fNMeshSegments[1]*fNMeshSegments[2] +y*fNMeshSegments[2]+z;

//    std::cout << x << "," << y << "," << z << ",";

  std::map<G4int, G4StatDouble*>::iterator value = score->find(idx);
  if(value == score->end()) {
//    std::cout << 0. << "," << 0. << "," << 0;
    v.push_back(0);
  } else {
    v.push_back((value->second->sum_wx())/unitValue*fact);
//    std::cout << (value->second->sum_wx())/unitValue*fact << ","
                //<< (value->second->sum_wx2())/unitValue/unitValue*fact*fact << ","
                // << value->second->n();
  }

//    std::cout << std::endl;

      } // z
    } // y
  } // x
  return v;
}

// Petr Stepanov: Obtain total energy deposited in the Mesh
//                code copied from G4VScoreWriter.cc
G4double G4Utils::getProjectionZMaximumQuantityFromMesh(const char *meshName, const char *psName) {
  G4ScoringManager *scoringManager = G4ScoringManager::GetScoringManager();
  G4VScoringMesh *fScoringMesh = scoringManager->FindMesh(meshName);

  using MeshScoreMap = G4VScoringMesh::MeshScoreMap;

  // retrieve the map
  MeshScoreMap fSMap = fScoringMesh->GetScoreMap();

  MeshScoreMap::const_iterator msMapItr = fSMap.find(psName);
  if (msMapItr == fSMap.end()) {
    G4cerr << "ERROR : DumpToFile : Unknown quantity, \"" << psName << "\"." << G4endl;
    return 0;
  }

  std::map<G4int, G4StatDouble*> *score = msMapItr->second->GetMap();

  G4double unitValue = fScoringMesh->GetPSUnitValue(psName);
  G4String unit = fScoringMesh->GetPSUnit(psName);
  G4String divisionAxisNames[3];
  fScoringMesh->GetDivisionAxisNames(divisionAxisNames);

  // Get number of the mesh segments
  G4int fNMeshSegments[3] = { 0 }; // number of segments of the mesh
  fScoringMesh->GetNumberOfSegments(fNMeshSegments);

  G4double fact = 1;
  G4double total = 0;
  // G4double grandMax = 0;
  // G4double totalVal2 = 0;
  // G4double entry = 0;

  size_t segmentsX = fNMeshSegments[0];
  size_t segmentsY = fNMeshSegments[1];
  size_t segmentsZ = fNMeshSegments[2];

  // XY projection Maximum
  std::vector<std::vector<G4double>> xy (segmentsX, std::vector<G4double> (segmentsY, 0));
  for (int x = 0; x < fNMeshSegments[0]; x++) {
    for (int y = 0; y < fNMeshSegments[1]; y++) {
      xy[x][y] = 0;
      for (int z = 0; z < fNMeshSegments[2]; z++) {
        G4int idx = x*fNMeshSegments[1]*fNMeshSegments[2] +y*fNMeshSegments[2]+z;
        std::map<G4int, G4StatDouble*>::iterator value = score->find(idx);

        if (value == score->end()) {
          total = 0;
        } else {
          total = (value->second->sum_wx())/unitValue*fact;
        }

        xy[x][y] += total;
      }
    }
  }
  Double_t xyMax = 0;
  for (int x = 0; x < fNMeshSegments[0]; x++) {
    for (int y = 0; y < fNMeshSegments[1]; y++) {
      if (xy[x][y] > xyMax) xyMax = xy[x][y];
    }
  }
  std::cout << "xyMax = " << xyMax << std::endl;

  // YZ projection Maximum
  std::vector<std::vector<G4double>> yz (segmentsY, std::vector<G4double> (segmentsZ, 0));
  for (int y = 0; y < fNMeshSegments[1]; y++) {
    for (int z = 0; z < fNMeshSegments[2]; z++) {
      yz[y][z] = 0;
      for (int x = 0; x < fNMeshSegments[0]; x++) {
        G4int idx = x*fNMeshSegments[1]*fNMeshSegments[2] +y*fNMeshSegments[2]+z;
        std::map<G4int, G4StatDouble*>::iterator value = score->find(idx);

        if (value == score->end()) {
          total = 0;
        } else {
          total = (value->second->sum_wx())/unitValue*fact;
        }

        yz[y][z] += total;
      }
    }
  }
  Double_t yzMax = 0;
  for (int y = 0; y < fNMeshSegments[1]; y++) {
    for (int z = 0; z < fNMeshSegments[2]; z++) {
      if (yz[y][z] > yzMax) yzMax = yz[y][z];
    }
  }
  std::cout << "yzMax = " << yzMax << std::endl;

  // XZ projection Maximum
  std::vector<std::vector<G4double>> xz (segmentsX, std::vector<G4double> (segmentsZ, 0));
  for (int x = 0; x < fNMeshSegments[0]; x++) {
    for (int z = 0; z < fNMeshSegments[2]; z++) {
      xz[x][z] = 0;
      for (int y = 0; y < fNMeshSegments[1]; y++) {
        G4int idx = x*fNMeshSegments[1]*fNMeshSegments[2] +y*fNMeshSegments[2]+z;
        std::map<G4int, G4StatDouble*>::iterator value = score->find(idx);

        if (value == score->end()) {
          total = 0;
        } else {
          total = (value->second->sum_wx())/unitValue*fact;
        }

        xz[x][z] += total;
      }
    }
  }
  Double_t xzMax = 0;
  for (int x = 0; x < fNMeshSegments[0]; x++) {
    for (int z = 0; z < fNMeshSegments[2]; z++) {
      if (xz[x][z] > xzMax) xzMax = xz[x][z];
    }
  }
  std::cout << "zzMax = " << xzMax << std::endl;

  return std::max(xzMax, yzMax);
}

G4double G4Utils::getMaximumQuantityFromMesh(const char *meshName, const char *psName) {
  G4ScoringManager *scoringManager = G4ScoringManager::GetScoringManager();
  G4VScoringMesh *fScoringMesh = scoringManager->FindMesh(meshName);

  using MeshScoreMap = G4VScoringMesh::MeshScoreMap;

  // retrieve the map
  MeshScoreMap fSMap = fScoringMesh->GetScoreMap();

  MeshScoreMap::const_iterator msMapItr = fSMap.find(psName);
  if (msMapItr == fSMap.end()) {
    G4cerr << "ERROR : DumpToFile : Unknown quantity, \"" << psName << "\"." << G4endl;
    return 0;
  }

  G4double unitValue = fScoringMesh->GetPSUnitValue(psName);
  Double_t fact = 1;
  std::map<G4int, G4StatDouble*> *score = msMapItr->second->GetMap();


  G4double max = 0;
  std::map<G4int, G4StatDouble*>::iterator it;

  for (it = score->begin(); it != score->end(); it++){
    Double_t total = (it->second->sum_wx())/unitValue*fact;
    if (total > max) max = total;
  }

  return max;
}

bool G4Utils::replaceSubstring(std::string& str, const std::string& from, const std::string& to) {
    size_t start_pos = str.find(from);
    if(start_pos == std::string::npos)
        return false;
    str.replace(start_pos, from.length(), to);
    return true;
}

G4String G4Utils::replaceIndex(const char* str, G4int index) {
  G4String string = str;
  G4String replaceWhat = "#";
  G4String replaceWith = std::to_string(index);
  return replaceAll(string, replaceWhat, replaceWith);
}

// https://stackoverflow.com/questions/2896600/how-to-replace-all-occurrences-of-a-character-in-string
std::string G4Utils::replaceAll(std::string str, const std::string& from, const std::string& to) {
    size_t start_pos = 0;
    while((start_pos = str.find(from, start_pos)) != std::string::npos) {
        str.replace(start_pos, from.length(), to);
        start_pos += to.length(); // Handles case where 'to' is a substring of 'from'
    }
    return str;
}

void G4Utils::updateScoring(const G4Event* event, G4String hitsCollectionName){
  // Code copied from G4RunActino::UpdateScoring();
  G4ScoringManager* ScM = G4ScoringManager::GetScoringManagerIfExist();
  if(!ScM)
    return;
  G4int nPar = ScM->GetNumberOfMesh();
  if(nPar < 1)
    return;

  G4HCofThisEvent* HCE = event->GetHCofThisEvent();
  if(!HCE)
    return;
  G4int nColl = HCE->GetCapacity();
  for(G4int i = 0; i < nColl; i++)
  {
    G4VHitsCollection* HC = HCE->GetHC(i);
    // Petr Stepanov - update hits collection if only it
    if(HC){
    	G4String currentHCName = HC->GetName();
    	if (currentHCName == hitsCollectionName){
        ScM->Accumulate(HC);
    	}
    }
  }
}

void G4Utils::resetMesh(const char* meshName) {
  G4ScoringManager *scoringManager = G4ScoringManager::GetScoringManager();
  G4VScoringMesh *fScoringMesh = scoringManager->FindMesh(meshName);
  fScoringMesh->ResetScore();
}
//G4ThreeVector* G4Utils::getAbsolutePosition(G4VPhysicalVolume* volume){
//  G4ThreeVector* vector = new G4ThreeVector();
//  for (G4VPhysicalVolume* currentVolume = volume; currentVolume!=NULL; currentVolume = currentVolume->GetMotherLogical()->Get;GetMotherLogical()Logical()->Get)
//}

//PhotoCathodeType G4Utils::getPhotoCathodeType(){
//  DetectorConstruction *detectorConstruction =
//      (DetectorConstruction*) G4RunManager::GetRunManager()->GetUserDetectorConstruction();
//  if (detectorConstruction == NULL)
//    return PhotoCathodeType::Bialkali;
//  return detectorConstruction->getPMT()->getPhotocathodeType();
//}

void G4Utils::checkCurrentMeshIsClosed(){
	G4ScoringManager* scoringManager = G4ScoringManager::GetScoringManager();
	G4VScoringMesh* currentMesh = scoringManager->GetCurrentMesh();
	if (currentMesh){
    G4ExceptionDescription ed;
    ed << "ERROR[" << "DetectorConstruction::InitUnitVolumeScoringMesh()"
       << "] : Mesh <" << currentMesh->GetWorldName()
       << "> is still open. Close it first. Command ignored.";
  }
}

void G4Utils::checkUniqueMeshName(G4String meshName){
	G4ScoringManager* scoringManager = G4ScoringManager::GetScoringManager();
  G4VScoringMesh* mesh = scoringManager->FindMesh(meshName);
  if ( mesh ){
  	G4Exception("G4Utils::checkUniqueMeshName",
  			        "Run Aborted",
								G4ExceptionSeverity::RunMustBeAborted,
  	            "Scoring mesh already exists!");
  }
}

void G4Utils::checkUniqueScorerName(G4VScoringMesh* mesh, G4String psName){
  if(mesh->FindPrimitiveScorer(psName)){
  	G4Exception("G4Utils::checkUniqueScorerName",
  			        "Run Aborted",
								G4ExceptionSeverity::RunMustBeAborted,
  	            "Primitive scorer already exists!");
  }
}

G4bool G4Utils::isOutsideCrystal(G4int x, G4int y, G4int crystalsX, G4int crystalsY){
	// Check if crystal is in the bottom row
	if (x == 0 || x == crystalsX-1) return true;
  if (y == 0 || y == crystalsY-1) return true;

	return false;

}

G4bool G4Utils::isHybridDetector(){
	  DetectorConstruction *detectorConstruction =
	      (DetectorConstruction*) G4RunManager::GetRunManager()->GetUserDetectorConstruction();
	  if (detectorConstruction == NULL){
	  	std::cout << "Cannot access DetectorConstruction" << std::endl;
	  	exit(1);
	  }
	  return detectorConstruction->isHybrid();
}

G4int G4Utils::getCrystalCopyNumber(const G4Step* step){
  G4StepPoint* point = step->GetPreStepPoint();
  G4TouchableHandle touch = point->GetTouchableHandle();
  G4VPhysicalVolume* volume = touch->GetVolume();

  // Check if volume exist
  if (!volume) return -1;

  // Ensure particle is in crystal volume
  if (volume->GetName() != "crystal_physical") return -1;

  // Crystal copy number
  G4int depth = 0;
  G4int copyNumber = touch->GetCopyNumber(depth);
  return copyNumber;
}

G4int G4Utils::getDetectorCopyNumber(const G4Step* step){
  // TODO: check Pre- or Post- point! (was Pre, now Post)
  // Solution: Post step[ point belongs to the next volume!
  // https://indico.cern.ch/event/294651/sessions/55918/attachments/552022/760640/UserActions.pdf
  G4StepPoint* point = step->GetPreStepPoint();
  G4TouchableHandle touch = point->GetTouchableHandle();
  G4VPhysicalVolume* volume = touch->GetVolume();

  // Check if volume exist
  if (!volume) return -1;

  // Crystal copy number
  if (volume->GetName() == "pmtCathode_physical") {
    G4int depth = 2;
    G4int copyNumber = touch->GetCopyNumber(depth);
    return copyNumber;
  }
  else if (volume->GetName() == "mppc_physical"){
    G4int depth = 1;
    G4int copyNumber = touch->GetCopyNumber(depth);
    return copyNumber;
  }
  return -1;
}

G4double G4Utils::getWavelength(const G4Track* track){
  G4double energy = track->GetDynamicParticle()->GetKineticEnergy()/eV; // in eV
  G4double hc = 1239.84193;
  G4double waveLength = hc/(energy); // in nm
  return waveLength;
}

G4String G4Utils::getTOFNtupleName(G4int eventNumber){
	G4String ntupleName = "tof";
	if (eventNumber == -1){
		return ntupleName;
	}
	ntupleName += std::to_string(eventNumber);
	return ntupleName;
}

// Depending if the event number is passed to the function we create
// - a "global" ntuple containing TOF for the first 100 events
// - "local" ntuple containing TOF information for one particular event

void G4Utils::prepareTOFNtuple(G4int eventNumber){
	// Construct ntuple name
	G4String ntupleName = getTOFNtupleName(eventNumber);

	// Construct ntuple title
	G4String ntupleTitle = "Time of Flight information";
	if (eventNumber == -1){
		ntupleTitle += " for first ";
		ntupleTitle += maxTofEvents;
		ntupleTitle += " events";
	}
	else {
		ntupleTitle += " for Event";
		ntupleTitle += eventNumber;
	}

	G4AnalysisManager* analysisManager = G4AnalysisManager::Instance();
	G4int ntupleId = analysisManager->CreateNtuple(ntupleName, ntupleTitle);
//	if (eventNumber == -1){
//		firstTOFNtupleId = ntupleId;
//	}
	analysisManager->CreateNtupleSColumn(ntupleId, "Process");
	analysisManager->CreateNtupleDColumn(ntupleId, "nPE");
	analysisManager->CreateNtupleDColumn(ntupleId, "globalTime");
	analysisManager->CreateNtupleDColumn(ntupleId, "localTime");
  analysisManager->CreateNtupleDColumn(ntupleId, "creationTime");
	analysisManager->FinishNtuple();
}

//G4int G4Utils::getFirstTOFNtupleId(){
//	return firstTOFNtupleId;
//}


G4String G4Utils::getCreatorProcessSafe(const G4Track* track){
  // If shooting op from GPS, CreatorProcess is NULL
  G4String creatorProcessString = (track->GetCreatorProcess() == nullptr) ?
                                  "gps" :
                                  std::string(track->GetCreatorProcess()->GetProcessName().c_str());
  return creatorProcessString;
}
