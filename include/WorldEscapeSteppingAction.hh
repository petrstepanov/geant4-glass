/*
 * SteppingAction.h
 *
 *  Created on: Nov 24, 2020
 *      Author: petrstepanov
 */

#ifndef SRC_SteppingAction_H_
#define SRC_SteppingAction_H_

#include <G4UserSteppingAction.hh>
#include <G4Step.hh>

class WorldEscapeSteppingAction: public G4UserSteppingAction {
public:
  WorldEscapeSteppingAction();
  virtual ~WorldEscapeSteppingAction();

  void UserSteppingAction(const G4Step* aStep);
};

#endif /* SRC_SteppingAction_H_ */
