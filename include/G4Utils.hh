/*
 * G4Utils.hh
 *
 *  Created on: Nov 29, 2020
 *      Author: petrstepanov
 */

#ifndef SRC_G4UTILS_HH_
#define SRC_G4UTILS_HH_

#include <G4String.hh>
#include <G4ThreeVector.hh>
#include <G4Types.hh>
#include <G4Step.hh>
#include <G4Track.hh>
#include <string>
#include <vector>

class AbsDetector;
class DetectorConstruction;
class G4Event;
class G4Timer;
class G4VScoringMesh;

struct EnergyValueUnit {
  G4double value;
  G4String unit;
};

class G4Utils {
private:
  G4Utils();
  static G4Utils* instance;

  std::chrono::high_resolution_clock::time_point tStart;

//  G4int firstTOFNtupleId;
//  G4int* TOFNtupleIds;

  //       event_number  ntuple_number
  std::map<int,          int> eventNumberToNTupleNumber;

public:
  virtual ~G4Utils();

  static const G4int maxTofEvents;

  void startTimer();
  void printTimer();

  // G4Utils();
  static G4Utils* getInstance();

  static G4double getGPSMonoEnergy();
  static G4String getGPSParticleName();
  static G4double getGPSZPos();
  static G4int getNumberOfEvents();
  static G4String getCrystalMaterial();
  static AbsDetector* getDetector();

  static DetectorConstruction* getDetectorConstruction();

  static G4ThreeVector* getCrystalSize();
  static G4double getCrystalX();
  static G4double getCrystalY();
  static G4double getCrystalZ();

  static G4int getNCrystalsX();
  static G4int getNCrystalsY();
  static G4int getNCrystals();

  static G4bool usingOpticalPhysics();

  static G4double getTotalQuantityFromMesh(const char *meshName, const char *psName);
  static std::vector<G4double> getUnitVolumesQuantityFromMesh(const char *meshName, const char *psName);

  static G4double getMaximumQuantityFromMesh(const char *meshName, const char *psName);
  static G4double getProjectionZMaximumQuantityFromMesh(const char *meshName, const char *psName);
  static void setMaximumMeshQuantity(const char *meshName, const char *psName, G4double val);

  static EnergyValueUnit formatEnergy(G4double);
  // static PhotoCathodeType getPhotoCathodeType();

  static bool replaceSubstring(std::string& str, const std::string& from, const std::string& to);
  static G4String replaceIndex(const char* str, G4int i);
  static std::string replaceAll(std::string str, const std::string& from, const std::string& to);

  static void updateScoring(const G4Event* event, G4String hitsCollectionName);
  static void resetMesh(const char* meshName);
//  static G4ThreeVector* getAbsolutePosition(G4VPhysicalVolume* volume);
  static void checkCurrentMeshIsClosed();
  static void checkUniqueMeshName(G4String meshName);
  static void checkUniqueScorerName(G4VScoringMesh* mesh, G4String psName);

  static G4bool isOutsideCrystal(G4int x, G4int y, G4int crystalsX, G4int crystalsY);
  static G4bool isHybridDetector();

  static G4int getCrystalCopyNumber(const G4Step* step);
  static G4int getDetectorCopyNumber(const G4Step* step);

  static G4double getWavelength(const G4Track* track);

  static G4String getOutputFileName(G4String suffix = "");

  G4String getTOFNtupleName(G4int eventNumber = -1);

  G4int getNTupleNumber(G4int eventNumber = -1);

  void prepareTOFNtuple(G4int eventNumber = -1);

  static G4String getCreatorProcessSafe(const G4Track* track);
};

#endif /* SRC_G4UTILS_HH_ */
