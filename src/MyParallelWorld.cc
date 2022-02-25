/*
 * MyParallelWorld.cpp
 *
 *  Created on: Jun 29, 2021
 *      Author: petrstepanov
 */

#include "MyParallelWorld.hh"
#include "G4LogicalVolume.hh"
#include "G4VPhysicalVolume.hh"
#include "G4Box.hh"
#include "G4PVPlacement.hh"
#include "G4SystemOfUnits.hh"
#include "G4VScoringMesh.hh"
#include "G4ScoringManager.hh"
#include "DetectorConstruction.hh"
#include "G4Utils.hh"
#include "G4ScoringBox.hh"
#include "G4PSEnergyDeposit.hh"
#include "G4PSEnergyDeposit3D.hh"

MyParallelWorld::MyParallelWorld(G4String worldName):G4VUserParallelWorld(worldName)
{;}

MyParallelWorld::~MyParallelWorld()
{;}

void MyParallelWorld::Construct()
{
  // See example: https://geant4-userdoc.web.cern.ch/UsersGuides/ForApplicationDeveloper/html/Detector/parallelWorld.html

  // Makoto Asai: This is Master thread. Define meshes here.
  DetectorConstruction* d = G4Utils::getDetectorConstruction();
  if (d->GetUseUnitVolumeScoringMesh()){
    InitUnitVolumeScoringMesh();
  }
}

void MyParallelWorld::ConstructSD()
{
  // Makoto Asai: This is worker thread. Not define meshes here.
}

void MyParallelWorld::InitUnitVolumeScoringMesh(){
  // Retrieve a copy of the mass geometry WorldVolume
  // G4VPhysicalVolume* ghostWorldPhysical = GetWorld();                               // mySinglePWNameGhost
  // G4LogicalVolume* ghostWorldLogical = ghostWorldPhysical->GetLogicalVolume();      // mySinglePWName

	DetectorConstruction* d = G4Utils::getDetectorConstruction();
    // Instantiate Scoring Manager for visualization
  G4ScoringManager* scoringManager = G4ScoringManager::GetScoringManager();

  // Define the box Mesh with unique name: /score/create/boxMesh meshName
  for (int i = 0; i < d->GetCrystalNumberX()*d->GetCrystalNumberY(); i++){
    G4String meshName = MultiFunctionalDetectorNames::getUnitVolumeMeshName(i);
    // G4String meshName = ghostWorldPhysical->GetName();
    // Check current mesh is closed
    // Petr Stepanov: copied from G4ScoringMessenger.cc
    G4VScoringMesh* currentmesh = scoringManager->GetCurrentMesh();
    if ( currentmesh ){
      G4ExceptionDescription ed;
      ed << "ERROR[" << "DetectorConstruction::InitUnitVolumeScoringMesh()"
         << "] : Mesh <" << currentmesh->GetWorldName()
         << "> is still open. Close it first. Command ignored.";
    }

    // Check mesh with the same name already exists
    G4VScoringMesh* mesh = scoringManager->FindMesh(meshName);
    if ( mesh ){
      G4ExceptionDescription ed;
      ed << "ERROR[" << "DetectorConstruction::InitUnitVolumeScoringMesh()"
         << "] : Scoring mesh <" << meshName
         << "> already exists. Creation ignored.";
    }

    // Create new mesh
    mesh = new G4ScoringBox(meshName);
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
    size[0] = d->GetCrystalSize()->x()/2;
    size[1] = d->GetCrystalSize()->y()/2;
    size[2] = d->GetCrystalSize()->z()/2;
    mesh->SetSize(size);

    // Set number of bins in the Mesh: /score/mesh/nBin # # #
    G4int segments[3];
    segments[0] = 1;
    segments[1] = 1;
    segments[2] = d->getNumberOfUnitVolumes();
    mesh->SetNumberOfSegments(segments);

    // Set Mesh position: /score/mesh/translate/xyz 0. 0. 2100. mm
    std::vector<G4VPhysicalVolume*> singlePhysicalVolumes = d->GetSinglePhysicalVolumes();
    G4VPhysicalVolume* singlePhysicalVolume = singlePhysicalVolumes[i];
    G4double centerPosition[3];
    centerPosition[0] = singlePhysicalVolume->GetTranslation().x();
    centerPosition[1] = singlePhysicalVolume->GetTranslation().y();
    centerPosition[2] = singlePhysicalVolume->GetTranslation().z();
    centerPosition[2] = centerPosition[2] + d->GetCrystalSize()->z()/2 + d->GetWrapThickness();
    mesh->SetCenterPosition(centerPosition);

    // Set Mesh primitive scorer: /score/quantity/energyDeposit eneDep
    // TODO: see if multiple meshes can have primitive scorer with same name?
    //       this could fix the error!
    G4String psName = HitsCollectionNames::getUnitVolumeHCName(i);
    // G4String psName = HitsCollectionNames::ENE_DEP_CRYSTAL_MESH;

    // Check mesh has primitive scorer
    if(mesh->FindPrimitiveScorer(psName)){
      G4ExceptionDescription ed;
      ed << "WARNING[" << "ERROR[" << "DetectorConstruction::InitUnitVolumeScoringMesh()"
         << "] : Quantity name, \"" << psName << "\", is already existing.";
      mesh->SetNullToCurrentPrimitiveScorer();
    }
    else {
      G4PSEnergyDeposit* edepScorer = new G4PSEnergyDeposit3D(psName);
      mesh->SetPrimitiveScorer(edepScorer);
    }

    // Close the Mesh: /score/close
    scoringManager->CloseCurrentMesh();
  }
}
