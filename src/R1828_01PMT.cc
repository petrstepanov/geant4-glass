/*
 * R1828_01PMT.cpp
 *
 *  Created on: Feb 20, 2021
 *      Author: petrstepanov
 */

#include <R1828_01PMT.hh>
#include "G4SystemOfUnits.hh"
#include "Materials.hh"

// Info here: https://www.hamamatsu.com/resources/pdf/etd/R1828-01_R2059_TPMH1259E.pdf

R1828_01PMT::R1828_01PMT() : AbsPMT() {
	// Digitized from above PDF
	efficiencyPoints = {288.8, 1.1, 290.9, 3.8, 297.1, 8.0, 300.2, 10.2, 305.3, 13.1, 311.5, 16.7, 323.9, 21.0, 341.5, 24.9, 364.2, 27.6, 388.0, 28.5, 411.7, 27.3, 434.4, 24.9, 457.1, 21.9, 475.7, 18.7, 494.3, 15.8, 511.9, 13.4, 527.4, 11.0, 541.8, 9.0, 555.2, 7.4, 569.7, 5.9, 582.1, 4.8, 592.4, 3.8, 600.7, 3.0, 606.9, 2.4, 614.1, 1.9, 620.3, 1.5, 625.5, 1.1, 631.7, 0.9, 637.9, 0.7, 643.0, 0.6, 648.2, 0.4, 652.3, 0.3, 657.5, 0.3, 662.7, 0.2, 667.8, 0.2, 671.9, 0.1, 676.1, 0.1, 681.2, 0.1, 685.4, 0.1, 690.5, 0.0, 695.7, 0.0, 699.8, 0.0, 704.0, 0.0, 708.1, 0.0};
}

R1828_01PMT::~R1828_01PMT() {
}

G4String R1828_01PMT::getName(){
	return "R1828-01";
}

G4double R1828_01PMT::getDiameter(){
	return 53*mm;
}

G4double R1828_01PMT::getCathodeDiameter(){
	return 46*mm;
}

G4double R1828_01PMT::getLength(){
	return 170*mm;
}

//PhotoCathodeType R1828_01PMT::getPhotocathodeType(){
//	return PhotoCathodeType::Bialkali;
//}

G4Material* R1828_01PMT::getWindowMaterial(){
	return Materials::getInstance()->getMaterial("borosilicate");
}

G4Material* R1828_01PMT::getCathodeMaterial(){
	return Materials::getInstance()->getMaterial("bialkali");
}
