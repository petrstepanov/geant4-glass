/*
 * MyParallelWorld.h
 *
 *  Created on: Jun 29, 2021
 *      Author: petrstepanov
 */

#ifndef GLASS_SRC_MYPARALLELWORLD_H_
#define GLASS_SRC_MYPARALLELWORLD_H_

#include "globals.hh"
#include "G4VUserParallelWorld.hh"

class MyParallelWorld : public G4VUserParallelWorld
{
  public:
    MyParallelWorld(G4String worldName);
    virtual ~MyParallelWorld();

  public:
    virtual void Construct();
    virtual void ConstructSD();
    void InitUnitVolumeScoringMesh();
//    void InitGlobalScoringMesh();
};

#endif /* GLASS_SRC_MYPARALLELWORLD_H_ */
