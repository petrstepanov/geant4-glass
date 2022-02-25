//
// ************************************************************************
// * License and Disclaimer                                               *
// *                                                                      *
// * The	Geant4 software	is	copyright of the Copyright Holders	of      *
// * the Geant4 Collaboration.	It is provided	under	the terms	and       *
// * conditions of the Geant4 Software License,	included in the file      *
// * LICENSE and available at	http://cern.ch/geant4/license .	These       *
// * include a list of copyright holders.                                 *
// *                                                                      *
// * Neither the authors of this software system, nor their employing     *
// * institutes,nor the agencies providing financial support for this     *
// * work	make	any representation or	warranty, express or implied,       *
// * regarding	this	software system or assume any liability for its     *
// * use.	Please see the license in the file	LICENSE	and URL above       *
// * for the full disclaimer and the limitation of liability.             *
// *                                                                      *
// * This	code	implementation is the result of	the	scientific and        *
// * technical work of the GEANT4 collaboration.                          *
// * By using,	copying,	modifying or	distributing the software (or     *
// * any work based	on the software)	you	agree	to acknowledge its        *
// * use	in	resulting	scientific	publications,	and indicate your       *
// * acceptance of all terms of the Geant4 Software license.              *
// ************************************************************************
//

#include <CLHEP/Random/Random.h>
#include <CLHEP/Random/RanecuEngine.h>
#include <DetectorConstruction.hh>
#include <G4String.hh>
#include <G4Timer.hh>
#include <G4UIExecutive.hh>
#include <G4UImanager.hh>
#include <G4Utils.hh>
#include <G4VisExecutive.hh>
#include <G4VisManager.hh>
#include <MyVisCommands.hh>
#include <PhysicsListMessenger.hh>
#include <Randomize.hh>
#include <MyRunManager.hh>
#include <iostream>
#include <string>
#include <G4TrajectoryDrawByAttribute.hh>
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

int main(int argc, char **argv) {
  // Detect interactive mode (if no arguments) and define UI session
  G4UIExecutive *ui = nullptr;
  if (argc == 1) {
    ui = new G4UIExecutive(argc, argv);
  }

  // Choose the Random engine
  G4Random::setTheEngine(new CLHEP::RanecuEngine);

  // Create an instance of the G4RunManager class
  // It controls the flow of the program and manages the event loop(s) within a run

  // Start Timer
  // G4Timer *timer = new G4Timer();
  // Define scoring meshes in same parallel world for Multi-Threaded Run Manager is very hard to achieve

//#ifdef G4MULTITHREADED
//  std::cout << "Is Multithreaded" << std::endl;
//  std::cout << "Instantiating multi-treaded Run Manager" << std::endl;
//  G4int threads = 2;
//  printf("Enter number of threads to use: ");
//  scanf("%d", &threads);
//  if (threads <= 0) threads = 2;
//  G4RunManager *runManager = new MTRunManager();
//runManager->SetNumberOfThreads(8);
//#else
//  std::cout << "Instantiating single-treaded Run Manager" << std::endl;
  G4RunManager *runManager = new MyRunManager();
// #endif

// Contruct the detector
// TODO: use TextGeometry to define the detector geometry instead?
// PS: try using the detector construction from AnaEx02
  DetectorConstruction *detector = new DetectorConstruction();

// Register parallel world (turned out we do not need this)
// See example RE01.cc
// See https://geant4-userdoc.web.cern.ch/UsersGuides/ForApplicationDeveloper/html/Detector/parallelWorld.html
// detector->RegisterParallelWorld(new MyParallelWorld(MyConstants::singleParallelWorldName));

  runManager->SetUserInitialization(detector);

// PS: Instantiate local physics list and define physics
// http://geant4-userdoc.web.cern.ch/geant4-userdoc/UsersGuides/ForApplicationDeveloper/html/GettingStarted/physicsDef.html?highlight=g4vuserphysicslists#how-to-specify-physics-processes
// PhysicsList *physicsList = new PhysicsList();

// PS: 5. Replace Physics
// G4VModularPhysicsList* physicsList = new FTFP_BERT();
// runManager->SetUserInitialization(physicsList);

// PS: Try adding optical physics (works)
// http://geant4-userdoc.web.cern.ch/geant4-userdoc/UsersGuides/ForApplicationDeveloper/html/TrackingAndPhysics/physicsProcess.html?highlight=ftfp_bert#g4opticalphysics-constructor
// G4VModularPhysicsList* physicsList = new QGSP_BERT();

// Instantiate default physics list
// G4VModularPhysicsList* physicsList = new FTFP_BERT();
// G4OpticalPhysics* opticalPhysics = new G4OpticalPhysics();
// physicsList->RegisterPhysics(opticalPhysics);
// runManager->SetUserInitialization(physicsList);

// Instantiate physics list messenger (allows changing physics list, adding optical physics)
  new PhysicsListMessenger(runManager);

// PS: Try set up physics using factory - like in Gears App
// G4PhysListFactory factory = new G4PhysListFactory();
// G4VModularPhysicsList* physlist = factory->GetReferencePhysList("FTFP_BERT_EMV");
// physlist.SetVerboseLevel(verbose);
// runManager->SetUserInitialization(physlist);

// PS: Object that outputs the ROOT file

// Instantiate General Particle Source (gps)
// Save primary particle information in Histogram Manager

// ActionInitialization* actionInitialization = new ActionInitialization(detector);
// runManager->SetUserInitialization(actionInitialization);

// PS: why we not initialize the Run Manager here?
// runManager->Initialize();

// Initialize visualization
  G4VisManager *visManager = new G4VisExecutive();
  visManager->Initialize();

  // Colorize trajectories
  // G4TrajectoryDrawByParticleID* model = new G4TrajectoryDrawByParticleID;
  // model->SetDefault("cyan");
  // model->Set("gamma", "green");
  // model->Set("e+", "magenta");
  // model->Set("e-", G4Colour(0.3, 0.3, 0.3));

  // visManager->RegisterModel(model);
//
//  G4TrajectoryDrawByAttribute* model2 = new G4TrajectoryDrawByAttribute("test");
//  model2->Set("CPN");
//
//  G4VAttValueFilter* cFilter = new G4VAttValueFilter("Cerenkov");
//  model2->AddValueContext("Cerenkov", )
//  visManager->RegisterModel(model2);
//
//  visManager->SelectTrajectoryModel(model2->Name());

// Get the pointer to the User Interface manager (created by runManager)
// In order for the user to issue commands to the program
  G4UImanager *UImanager = G4UImanager::GetUIpointer();

  G4Utils::getInstance()->startTimer();

  if (!ui) {
    // batch mode
    G4String command = "/control/execute ";
    G4String fileName = argv[1];
    UImanager->ApplyCommand(command + fileName);
  } else {
    // interactive mode
    // Instantiate Misc Messenger that saves image
    new MyVisCommands();

    // UImanager->ApplyCommand("/control/execute init_vis.mac");
    ui->SessionStart();

    delete ui;
  }

// Print elapsed time
  G4Utils::getInstance()->printTimer();

  delete visManager;
  delete runManager;

  return 0;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
