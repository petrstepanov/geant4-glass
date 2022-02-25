/*
 * R1450PMT.cpp
 *
 *  Created on: Feb 20, 2021
 *      Author: petrstepanov
 */

#include <R1450PMT.hh>
#include "G4SystemOfUnits.hh"
#include "Materials.hh"

// Info here: https://www.hamamatsu.com/resources/pdf/etd/R1450_TPMH1215E.pdf

R1450PMT::R1450PMT() : AbsPMT() {
	// Digitized from above PDF
	efficiencyPoints = {273.8, 8.1, 281.1, 10.2, 289.4, 13.0, 299.8, 16.4, 313.3, 20.1, 335.2, 22.6, 360.1, 23.0, 384.1, 24.5, 408.0, 24.8, 432.9, 24.3, 455.8, 22.1, 477.6, 19.5, 497.4, 17.0, 516.1, 14.5, 536.9, 12.5, 553.6, 10.4, 564.0, 8.4, 573.3, 6.6, 583.7, 5.3, 594.1, 4.2, 605.5, 3.4, 613.9, 2.7, 623.2, 2.1, 631.5, 1.7, 638.8, 1.3, 645.1, 1.0, 652.3, 0.8, 658.6, 0.6, 663.8, 0.5, 670.0, 0.4, 676.3, 0.3, 681.5, 0.2, 684.6, 0.2, 690.8, 0.1, 696.0, 0.1, 699.1, 0.1, 704.3, 0.1, 708.5, 0.1, 714.7, 0.0, 718.9, 0.0, 724.1, 0.0, 728.2, 0.0, 733.4, 0.0, 737.6, 0.0};
}

R1450PMT::~R1450PMT() {
}

G4String R1450PMT::getName(){
	return "R1450";
}

G4double R1450PMT::getDiameter(){
	return 18.6*mm;
}

G4double R1450PMT::getCathodeDiameter(){
	return 15*mm; // TODO: change to 7*mm
}

G4double R1450PMT::getLength(){
	return 88*mm;
}

//PhotoCathodeType R1450PMT::getPhotocathodeType(){
//	return PhotoCathodeType::Bialkali;
//}

G4Material* R1450PMT::getWindowMaterial(){
	return Materials::getInstance()->getMaterial("borosilicate");
}

G4Material* R1450PMT::getCathodeMaterial(){
	return Materials::getInstance()->getMaterial("bialkali");
}
