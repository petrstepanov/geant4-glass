/*
 * AbsPmt.cpp
 *
 *  Created on: Feb 20, 2021
 *      Author: petrstepanov
 */

#include <AbsPMT.hh>
#include "G4SystemOfUnits.hh"

AbsPMT::AbsPMT() : AbsDetector() {
}

AbsPMT::~AbsPMT() {
}

G4double AbsPMT::getWindowThickness(){
	return 1.5*mm;
}

G4double AbsPMT::getCathodeThickness(){
	return 10*mm;
}
