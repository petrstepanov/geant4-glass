/*
 * R1450PMT.h
 *
 *  Created on: Feb 20, 2021
 *      Author: petrstepanov
 */

#ifndef INCLUDE_PMT_R2257PMT_H_
#define INCLUDE_PMT_R2257PMT_H_

#include <AbsPMT.hh>

class R2257PMT : public AbsPMT {
public:
	R2257PMT();
	virtual ~R2257PMT();

	G4String getName();
	G4double getDiameter();
	G4double getCathodeDiameter();
	G4double getLength();
	// PhotoCathodeType getPhotocathodeType();
	G4Material* getWindowMaterial();
	G4Material* getCathodeMaterial();
};

#endif /* INCLUDE_PMT_R2257PMT_H_ */
