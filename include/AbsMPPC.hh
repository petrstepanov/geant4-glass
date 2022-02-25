/*
 * Pmt.h
 *
 *  Created on: Feb 20, 2021
 *      Author: petrstepanov
 */

#ifndef SRC_ABSMPPC_HH_
#define SRC_ABSMPPC_HH_

#include "AbsDetector.hh"
#include <G4Types.hh>
#include <G4String.hh>
#include <G4Material.hh>

class AbsMPPC : public AbsDetector {
public:
	AbsMPPC();
	virtual ~AbsMPPC();

	virtual G4double getSize()=0;
};

#endif /* SRC_ABSMPPC_HH_ */
