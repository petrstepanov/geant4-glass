/*
 * MyVisMessenger.h
 *
 *  Created on: Dec 17, 2020
 *      Author: petrstepanov
 */

#ifndef SRC_MyVisCommandsSet_H_
#define SRC_MyVisCommandsSet_H_

#include "G4VVisCommand.hh"
#include "G4UIcommand.hh"
#include "G4UIcmdWithAString.hh"

// Class inheritance and commands copied from source G4VisCommandsViewerSet.cc

class MyVisCommands : public G4VVisCommand {
public:
  MyVisCommands();
  virtual ~MyVisCommands();

  virtual void SetNewValue(G4UIcommand*, G4String);

private:
  G4UIcommand* fCenterViewCommand;
  G4UIcmdWithAString* fDrawText2DCommand;
  G4UIcommand* fDrawStatsCommand;
  G4UIcommand* fPrintEPScommand;
  G4UIcommand* fPrintPNGcommand;

  G4int lineNumber = 0;

  void drawText2D(const char*);
  void drawText2D(G4String);
};

#endif /* SRC_MyVisCommandsSet_H_ */
