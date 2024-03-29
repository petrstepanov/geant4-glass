/*
 * MyVisMessenger.cpp
 *
 *  Created on: Dec 17, 2020
 *      Author: petrstepanov
 */

#include <AbsDetector.hh>
#include <CLHEP/Geometry/Point3D.h>
#include <CLHEP/Vector/ThreeVector.h>
#include <DetectorConstruction.hh>
#include <G4Colour.hh>
#include <G4OpenGLViewer.hh>
#include <G4Point3D.hh>
#include <G4Scene.hh>
#include <G4Square.hh>
#include <G4String.hh>
#include <G4Text.hh>
#include <G4ThreeVector.hh>
#include <G4Types.hh>
#include <G4Utils.hh>
#include <G4ViewParameters.hh>
#include <G4VisAttributes.hh>
#include <G4VisManager.hh>
#include <G4VMarker.hh>
#include <G4VSceneHandler.hh>
#include <G4VViewer.hh>
#include <HistoManager.hh>
#include <MyVisCommands.hh>
#include <stddef.h>
#include <sstream>
#include <string>

using CLHEP::Hep3Vector;

// /myvis/centerviewer

MyVisCommands::MyVisCommands() : G4VVisCommand() {
  // Instantiate command for setting the gap between crystals
  fCenterViewCommand = new G4UIcommand("/myvis/centerViewer", this);
  fCenterViewCommand->SetGuidance("Center the camera on the crystals.");

  // Instantiate command for drawing 2D text
  fDrawText2DCommand = new G4UIcmdWithAString("/myvis/drawText2D", this);
  fDrawText2DCommand->SetGuidance("Draw 2D text over the visualization.");

  // Instantiate command for drawing 2D text
  fDrawStatsCommand = new G4UIcommand("/myvis/drawStats", this);
  fDrawStatsCommand->SetGuidance("Draw energy deposition statistics.");

  // Instantiate Print EPS command
  fPrintEPScommand = new G4UIcommand("/myvis/ogl/printEPS", this);
  fPrintEPScommand->SetGuidance("Print EPS file.");

  // Instantiate Print EPS command
  fPrintPNGcommand = new G4UIcommand("/myvis/ogl/printPNG", this);
  fPrintPNGcommand->SetGuidance("Print PNG file.");
}

MyVisCommands::~MyVisCommands() {
  delete fCenterViewCommand;
}

void MyVisCommands::SetNewValue(G4UIcommand* cmd, G4String string) {
  if (cmd==fCenterViewCommand){
    G4VViewer* currentViewer = fpVisManager->GetCurrentViewer();
    G4ViewParameters vp = currentViewer->GetViewParameters();

    if (!currentViewer) return;
    // G4OpenGLViewer* pOGLViewer = dynamic_cast<G4OpenGLViewer*>(currentViewer);
    // if (!pOGLViewer) return;

    // Set pivot point
    // Command: /vis/viewer/set/targetPoint 0 0 145 mm
    G4double detectorLength = G4Utils::getDetector()->getLength();
    G4ThreeVector targetPoint(0, 0, (G4Utils::getCrystalZ() + detectorLength)/2.);
    const G4Point3D& standardTargetPoint = currentViewer->GetSceneHandler()->GetScene()->GetStandardTargetPoint();
    vp.SetCurrentTargetPoint(targetPoint - standardTargetPoint);

    // Set viewing angle
    // Command: /vis/viewer/set/viewpointVector -1 1 -1
    G4ThreeVector viewVector(-1, 1, -1);
    Hep3Vector viewpointVector = viewVector.unit();
    vp.SetViewAndLights(viewpointVector);

    SetViewParameters(currentViewer,vp);
  } else if (cmd==fDrawText2DCommand){
    drawText2D(string);
  } else if (cmd==fDrawStatsCommand){
    std::stringstream buffer;

    lineNumber = 0;

    // Write detector type
    buffer << "Detector:               " << G4Utils::getDetector()->getName();
    drawText2D(buffer.str());
    buffer.str( std::string() ); buffer.clear();

    // Write material
    buffer << "Crystal material:       " << G4Utils::getCrystalMaterial();
    drawText2D(buffer.str());
    buffer.str( std::string() ); buffer.clear();

    // Write crystal size
    buffer << "Crystal size:           " << G4Utils::getCrystalX() << "x"
                                         << G4Utils::getCrystalY() << "x"
                                         << G4Utils::getCrystalZ() << " mm";
    drawText2D(buffer.str());
    buffer.str( std::string() ); buffer.clear();

    // Write crystal assembly size (1x1, 3x3, 5x5)
    buffer << "Crystal assembly:       " << G4Utils::getNCrystalsX() << "x"
                                         << G4Utils::getNCrystalsY();
    drawText2D(buffer.str());
    buffer.str( std::string() ); buffer.clear();

    // Write incident particle details
    EnergyValueUnit gpsMonoEnergyEVU = G4Utils::formatEnergy(G4Utils::getGPSMonoEnergy());
    buffer << "Incident particle:      " << G4Utils::getGPSParticleName() << ", " << gpsMonoEnergyEVU.value << " " << gpsMonoEnergyEVU.unit << ", " << G4Utils::getNumberOfEvents() << " events";
    drawText2D(buffer.str());
    buffer.str( std::string() ); buffer.clear();

    // Write total energy from GPS
    G4double energyTotalGPS = G4Utils::getGPSMonoEnergy()*G4Utils::getNumberOfEvents();
    EnergyValueUnit energyTotalGPSEVU = G4Utils::formatEnergy(energyTotalGPS);
    // buffer << "Total incident energy:  " << energyTotalGPSEVU.value << " " << energyTotalGPSEVU.unit;
    // drawText2D(buffer.str());
    // buffer.str( std::string() ); buffer.clear();

    // Write values registered by mesh
    if (G4Utils::getDetectorConstruction()->GetUsePrimitiveScorer()){
      // Write total energy in crystals
      G4double energyTotalCrystalsMesh = G4Utils::getTotalQuantityFromMesh(MultiFunctionalDetectorNames::CRYSTALS_MESH, HitsCollectionNames::ENE_DEP_CRYSTALS_MESH);
      EnergyValueUnit evuC = G4Utils::formatEnergy(energyTotalCrystalsMesh);
      // buffer << "Deposited in crystals:  " << evuC.value << " " << evuC.unit << " (" << energyTotalCrystalsMesh/energyTotalGPS*100 << " %)";
      buffer << "Deposited in crystals:  " << std::setprecision(2) << energyTotalCrystalsMesh/energyTotalGPS*100 << " %";
      drawText2D(buffer.str());
      buffer.str( std::string() ); buffer.clear();
    }

    // Write values registered by mesh
    if (G4Utils::getDetectorConstruction()->GetUseGlobalScoringMesh()){
//      // Write total energy in crystals
//      G4double energyTotalCrystalsMesh = G4Utils::getTotalQuantityFromMesh(MultiFunctionalDetectorNames::CRYSTALS_MESH, HitsCollectionNames::ENE_DEP_CRYSTALS_MESH);
//      EnergyValueUnit evuC = G4Utils::formatEnergy(energyTotalCrystalsMesh);
//      buffer << "Deposited in crystals:  " << evuC.value << " " << evuC.unit << " (" << energyTotalCrystalsMesh/energyTotalGPS*100 << " %)";
//      drawText2D(buffer.str());
//      buffer.str( std::string() ); buffer.clear();

      // Write total energy in detectors
      G4double energyTotalDetMesh = G4Utils::getTotalQuantityFromMesh(MultiFunctionalDetectorNames::PMTS_MESH, HitsCollectionNames::ENE_DEP_PMTS_MESH);
      EnergyValueUnit evuD = G4Utils::formatEnergy(energyTotalDetMesh);
      // buffer << "Deposited in detectors: " << evuD.value << " " << evuD.unit << " (" << energyTotalDetMesh/energyTotalGPS*100 << " %)";
      buffer << "Deposited in detectors: " << std::setprecision(2) << energyTotalDetMesh/energyTotalGPS*100 << " %";
      drawText2D(buffer.str());
      buffer.str( std::string() ); buffer.clear();
    }

    // Write total energy escaped the world
    if (G4Utils::getDetectorConstruction()->GetSaveWorldEscapeEnergy()){
      G4double energyEscapedWorld = HistoManager::getInstance()->getTotalWorldOutEnergy();
      EnergyValueUnit evu = G4Utils::formatEnergy(energyEscapedWorld);
      // buffer <<  "Escaped the world:      " << evu.value << " " << evu.unit << " (" << energyEscapedWorld/energyTotalGPS*100 << " %)";
      buffer <<  "Escaped the world:      " << std::setprecision(2) << energyEscapedWorld/energyTotalGPS*100 << " %";
      drawText2D(buffer.str());
      buffer.str( std::string() ); buffer.clear();
    }
  }
  else if (cmd==fPrintEPScommand){
    G4String fileName = G4Utils::getOutputFileName();
    G4VViewer* currentViewer = fpVisManager->GetCurrentViewer();
    G4OpenGLViewer* pOGLViewer = dynamic_cast<G4OpenGLViewer*>(currentViewer);
    G4Utils::replaceSubstring(fileName, ".root", ".eps");
    pOGLViewer->setExportImageFormat("eps",true);
    pOGLViewer->exportImage(fileName);
  }
  else if (cmd==fPrintPNGcommand){
    G4String fileName = G4Utils::getOutputFileName();
    G4VViewer* currentViewer = fpVisManager->GetCurrentViewer();
    G4OpenGLViewer* pOGLViewer = dynamic_cast<G4OpenGLViewer*>(currentViewer);
    G4Utils::replaceSubstring(fileName, ".root", ".png");
    pOGLViewer->setExportImageFormat("png",true);
    pOGLViewer->exportImage(fileName);
  }
}

void MyVisCommands::drawText2D(const char* string){
  G4String str(string);
  drawText2D(str);
}

void MyVisCommands::drawText2D(G4String text){
//  G4VViewer* currentViewer = fpVisManager->GetCurrentViewer();
//  G4ViewParameters vp = currentViewer->GetViewParameters();

//  G4VVisManager* fVisManager = G4VVisManager::GetConcreteInstance();
//  if(!fVisManager) {
//    G4cerr << "G4VScoringMesh::DrawColorChart(): no visualization system" << G4endl;
//    return;
//  }
//

  // Draw text shadow (black)
  // G4double shadowOffset = 0.0035;

  // G4VViewer* currentViewer = fpVisManager->GetCurrentViewer();

  const G4double textSize = 7; // text height actually, in px
  const G4double charWidth = textSize * 0.63;

  const G4double lineHeight = textSize*1.5;

  const G4double paddingLeft = textSize*2.5;
  const G4double paddingTop = textSize*2.5;

  const G4double viewerHeight = fpVisManager->GetCurrentViewer()->GetViewParameters().GetWindowSizeHintY();
  const G4double viewerWidth = fpVisManager->GetCurrentViewer()->GetViewParameters().GetWindowSizeHintX();

  // fpVisManager->BeginDraw2D();

  // Draw text background


  // Here coordinate system is weird, (0,0) is screen center, (-1,-1) is top left corner
  // Pixel scale is twice smaller than actual, need to multiply everything by 2

  G4double yPx = paddingTop + lineNumber*lineHeight;
  for(size_t i = 0; i <= text.length(); i++){
    G4double shadowXPx = paddingLeft + charWidth*i;
    G4Square* shadow = new G4Square(G4Point3D(-1. + shadowXPx/viewerWidth*4., 1. - yPx/viewerHeight*4., 0));  // Due to coordinate system need to multiply everything by 4?? why dont understand - may be retina issue??
    shadow->SetScreenSize(lineHeight*2.);
    shadow->SetFillStyle(G4VMarker::FillStyle::filled);
    G4Colour bgColor = fpVisManager->GetCurrentViewer()->GetViewParameters().GetBackgroundColour();
    // G4Colour bgColor = G4Colour::Red();
    G4VisAttributes att(bgColor);
    shadow->SetVisAttributes(&att);
    fpVisManager->Draw2D(*shadow);
  }

  // Draw text
  // G4Text t(text, G4Point3D(paddingLeft/viewerWidth, yPx/viewerHeight, 0));
  G4Text t(text, G4Point3D(-1. + paddingLeft/viewerWidth*4., 1. - (yPx + (lineHeight-textSize)/2.)/viewerHeight*4., 0));
  t.SetScreenSize(textSize*2.); // Due to coordinate system need to multiply everything to 2
  G4Colour textColor = fpVisManager->GetCurrentViewer()->GetViewParameters().GetBackgroundColour()==G4Colour::Black() ? G4Colour::White() : G4Colour::Black();
  // G4Colour textColor = G4Colour::Green();
  G4VisAttributes att(textColor);
  t.SetVisAttributes(&att);
  fpVisManager->Draw2D(t);

  // fpVisManager->EndDraw2D();

  lineNumber++;
}
