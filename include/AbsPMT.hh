/*
 * Pmt.h
 *
 *  Created on: Feb 20, 2021
 *      Author: petrstepanov
 */

#ifndef SRC_ABSPMT_HH_
#define SRC_ABSPMT_HH_

#include "AbsDetector.hh"
#include <G4Types.hh>
#include <G4String.hh>

class AbsPMT : public AbsDetector {
public:
	AbsPMT();
	virtual ~AbsPMT();

	virtual G4double getDiameter()=0;
	virtual G4double getCathodeDiameter()=0;

	G4double getCathodeThickness();

	virtual G4double getWindowThickness();
};

#endif /* SRC_ABSPMT_HH_ */
