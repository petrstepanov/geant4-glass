/*
 * RunManager.cc
 *
 *  Created on: Apr 13, 2021
 *      Author: petrstepanov
 */

#include "MyRunManager.hh"
#include "MyConstants.hh"
#include <G4VScoringMesh.hh>
#include <G4ScoringManager.hh>
#include <G4TransportationManager.hh>
#include <G4ParallelWorldProcess.hh>
#include <G4Version.hh>

#include <string>
#include <regex>

#define XSTR(x) STR(x)
#define STR(x) #x

// Custom Run Manager class needed to ensure correct sequence of initialization of user actions

MyRunManager::MyRunManager() : G4RunManager() {
  // mySingleParallelWorldName = "mySingleParallelWorldName";
}

MyRunManager::~MyRunManager() {
// TODO Auto-generated destructor stub
}

//void MyRunManager::Initialize() {
//// When '/run/initialize' is called, assign custom user actions
//  fActionInitialization = new ActionInitialization();
//  SetUserInitialization(fActionInitialization);
//
//// Run base class method
//  G4RunManager::Initialize();
//}

// Copied from G4RunManager.cc. But modified to use single parallel world (one specific world name)
// to ensure only single parallel world is created


// █▀█ █▀█ █ █▀▀ █ █▄░█ ▄▀█ █░░
// █▄█ █▀▄ █ █▄█ █ █░▀█ █▀█ █▄▄

//void G4RunManager::ConstructScoringWorlds() {
//  using MeshShape = G4VScoringMesh::MeshShape;
//
//  G4ScoringManager *ScM = G4ScoringManager::GetScoringManagerIfExist();
//  if (!ScM)
//    return;
//
//  G4int nPar = ScM->GetNumberOfMesh();
//  if (nPar < 1)
//    return;
//
//  G4ParticleTable::G4PTblDicIterator *theParticleIterator = G4ParticleTable::GetParticleTable()->GetIterator();
//  for (G4int iw = 0; iw < nPar; iw++) {
//    G4VScoringMesh *mesh = ScM->GetMesh(iw);
//    if (fGeometryHasBeenDestroyed)
//      mesh->GeometryHasBeenDestroyed();
//    G4VPhysicalVolume *pWorld = nullptr;
//    if (mesh->GetShape() != MeshShape::realWorldLogVol) {
//      pWorld = G4TransportationManager::GetTransportationManager()->IsWorldExisting(ScM->GetWorldName(iw));
//      if (!pWorld) {
//        pWorld = G4TransportationManager::GetTransportationManager()->GetParallelWorld(ScM->GetWorldName(iw));
//        pWorld->SetName(ScM->GetWorldName(iw));
//
//        G4ParallelWorldProcess *theParallelWorldProcess = mesh->GetParallelWorldProcess();
//        if (theParallelWorldProcess) {
//          theParallelWorldProcess->SetParallelWorld(ScM->GetWorldName(iw));
//        } else {
//          theParallelWorldProcess = new G4ParallelWorldProcess(ScM->GetWorldName(iw));
//          mesh->SetParallelWorldProcess(theParallelWorldProcess);
//          theParallelWorldProcess->SetParallelWorld(ScM->GetWorldName(iw));
//
//          theParticleIterator->reset();
//          while ((*theParticleIterator)()) {
//            G4ParticleDefinition *particle = theParticleIterator->value();
//            G4ProcessManager *pmanager = particle->GetProcessManager();
//            if (pmanager) {
//              pmanager->AddProcess(theParallelWorldProcess);
//              if (theParallelWorldProcess->IsAtRestRequired(particle)) {
//                pmanager->SetProcessOrdering(theParallelWorldProcess, idxAtRest, 9900);
//              }
//              pmanager->SetProcessOrderingToSecond(theParallelWorldProcess, idxAlongStep);
//              pmanager->SetProcessOrdering(theParallelWorldProcess, idxPostStep, 9900);
//            }
//          }
//        }
//        theParallelWorldProcess->SetLayeredMaterialFlag(mesh->LayeredMassFlg());
//      }
//    }
//    mesh->Construct(pWorld);
//  }
//
//  GeometryHasBeenModified();
//}

// █▀▄▀█ █▀█ █▀▄ █▀▄ █▀▀ █▀▄
// █░▀░█ █▄█ █▄▀ █▄▀ ██▄ █▄▀

void MyRunManager::ConstructScoringWorlds() {
  using MeshShape = G4VScoringMesh::MeshShape;

  G4ScoringManager *ScM = G4ScoringManager::GetScoringManagerIfExist();
  if (!ScM)
    return;

  G4int nPar = ScM->GetNumberOfMesh();
  if (nPar < 1)
    return;

  G4ParticleTable::G4PTblDicIterator *theParticleIterator = G4ParticleTable::GetParticleTable()->GetIterator();
  for (G4int iw = 0; iw < nPar; iw++) {
    G4VScoringMesh *mesh = ScM->GetMesh(iw);
    if (fGeometryHasBeenDestroyed) mesh->GeometryHasBeenDestroyed();
    G4VPhysicalVolume *pWorld = nullptr;
    if (mesh->GetShape() != MeshShape::realWorldLogVol) {
      // Petr Stepanov: define all Unit Volume scoring meshes in one world, other meshes in separate worlds
      //                we can place a group of meshes into one single parallel world having them not overlap each other
      G4String worldName = (mesh->GetWorldName().find(MultiFunctionalDetectorNames::UNIT_VOLUME_MESH_BASE_NAME) != std::string::npos) ?
                           MyConstants::singleParallelWorldName : ScM->GetWorldName(iw);
      pWorld = G4TransportationManager::GetTransportationManager()->IsWorldExisting(worldName);

      if (!pWorld) {
        pWorld = G4TransportationManager::GetTransportationManager()->GetParallelWorld(worldName);
        pWorld->SetName(worldName);

        G4ParallelWorldProcess *theParallelWorldProcess = mesh->GetParallelWorldProcess();
        if (theParallelWorldProcess) {
          theParallelWorldProcess->SetParallelWorld(worldName);
        } else {
          theParallelWorldProcess = new G4ParallelWorldProcess(worldName);
          mesh->SetParallelWorldProcess(theParallelWorldProcess);
          theParallelWorldProcess->SetParallelWorld(worldName);

          theParticleIterator->reset();
          while ((*theParticleIterator)()) {
            G4ParticleDefinition *particle = theParticleIterator->value();
            G4ProcessManager *pmanager = particle->GetProcessManager();
            if (pmanager) {
              pmanager->AddProcess(theParallelWorldProcess);
              if (theParallelWorldProcess->IsAtRestRequired(particle)) {
                pmanager->SetProcessOrdering(theParallelWorldProcess, idxAtRest, 9900);
              }
              pmanager->SetProcessOrderingToSecond(theParallelWorldProcess, idxAlongStep);
              pmanager->SetProcessOrdering(theParallelWorldProcess, idxPostStep, 9900);
            }
          }
        }

        // Petr Stepanov: determine the Geant version from the computer environment
        // PROBLEM:  On the Farm environment 2.4 we have older version of Geant4, code won't compile.
        //           Only starting Geant 4.07 LayeredMassFlg() is implemented. Therefore code below causes error on older Farm's Geant4 v10.06
        // SOLUTION: Geant4 minor and major versions G4_V1 and G4_V2 are determined at the compile time and added as compile_definitions
        #if(G4VERSION_NUMBER <= 1060)
          std::cout << "Geant4 version <= 10.6" << std::endl;
        #else
          std::cout << "Geant4 version > 10.6" << std::endl;
          theParallelWorldProcess->SetLayeredMaterialFlag(mesh->LayeredMassFlg());
        #endif
      }
    }
    mesh->Construct(pWorld);
  }

  GeometryHasBeenModified();
}
