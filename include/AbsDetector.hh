/*
 * AbsDetector.hh
 *
 *  Created on: Feb 22, 2021
 *      Author: petrstepanov
 */

#ifndef INCLUDE_ABSDETECTOR_HH_
#define INCLUDE_ABSDETECTOR_HH_

#include <G4Types.hh>
#include <G4String.hh>
#include <G4Material.hh>
#include <vector>

class AbsDetector {
public:
	AbsDetector();
	virtual ~AbsDetector();

	virtual G4String getName()=0;
	virtual G4double getLength()=0;
	virtual G4Material* getWindowMaterial()=0;
	virtual G4Material* getCathodeMaterial()=0;
	virtual G4double getWindowThickness()=0;

	virtual G4double getCathodeThickness()=0;

	void saveToFile(G4String outputPath);

	G4double getEfficiency(G4double wavelength);


protected:
	std::vector<G4double> efficiencyPoints;
};

#endif /* INCLUDE_ABSDETECTOR_HH_ */
