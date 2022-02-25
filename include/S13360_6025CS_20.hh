/*
 * S13360_6025CS_20.hh
 *
 *  Created on: Feb 22, 2021
 *      Author: petrstepanov
 */

#ifndef INCLUDE_S13360_6025CS_20_HH_
#define INCLUDE_S13360_6025CS_20_HH_

#include <AbsMPPC.hh>

class S13360_6025CS_20 : public AbsMPPC {
public:
	S13360_6025CS_20();
	virtual ~S13360_6025CS_20();

	G4String getName();
	G4double getSize();
	G4double getLength();
	G4double getWindowThickness();
	G4double getCathodeThickness();
	G4Material* getWindowMaterial();
	G4Material* getCathodeMaterial();
};

#endif /* INCLUDE_S13360_6025CS_20_HH_ */
