/*
 * R1450PMT.h
 *
 *  Created on: Feb 20, 2021
 *      Author: petrstepanov
 */

#ifndef INCLUDE_PMT_R1828_01PMT_H_
#define INCLUDE_PMT_R1828_01PMT_H_

#include <AbsPMT.hh>

class R1828_01PMT : public AbsPMT {
public:
	R1828_01PMT();
	virtual ~R1828_01PMT();

	G4String getName();
	G4double getDiameter();
	G4double getCathodeDiameter();
	G4double getLength();
	// PhotoCathodeType getPhotocathodeType();
	G4Material* getWindowMaterial();
	G4Material* getCathodeMaterial();
};

#endif /* INCLUDE_PMT_R1828_01PMT_H_ */
