/*
 * RunManager.hh
 *
 *  Created on: Apr 13, 2021
 *      Author: petrstepanov
 */

#ifndef GLASS_SRC_MYRUNMANAGER_HH_
#define GLASS_SRC_MYRUNMANAGER_HH_

#include <G4RunManager.hh>
#include "ActionInitialization.hh"

class MyRunManager : public G4RunManager {
public:
  MyRunManager();
  virtual ~MyRunManager();

//  void Initialize() override;

  void ConstructScoringWorlds() override;


private:
  // G4String mySingleParallelWorldName;
  // ActionInitialization* fActionInitialization;

};

#endif /* GLASS_SRC_RUNMANAGER_HH_ */
