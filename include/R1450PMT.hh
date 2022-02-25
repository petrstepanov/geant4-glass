/*
 * R1450PMT.h
 *
 *  Created on: Feb 20, 2021
 *      Author: petrstepanov
 */

#ifndef INCLUDE_PMT_R1450PMT_H_
#define INCLUDE_PMT_R1450PMT_H_

#include <AbsPMT.hh>

class R1450PMT : public AbsPMT {
public:
	R1450PMT();
	virtual ~R1450PMT();

	G4String getName();
	G4double getDiameter();
	G4double getCathodeDiameter();
	G4double getLength();
	// PhotoCathodeType getPhotocathodeType();
	G4Material* getWindowMaterial();
	G4Material* getCathodeMaterial();
};

#endif /* INCLUDE_PMT_R1450PMT_H_ */
