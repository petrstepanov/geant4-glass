/*
 * R2257PMT.cpp
 *
 *  Created on: Feb 20, 2021
 *      Author: petrstepanov
 */

#include <R2257PMT.hh>
#include "G4SystemOfUnits.hh"
#include "Materials.hh"

// Info here: https://www.hamamatsu.com/resources/pdf/etd/R2257_TPMH1141E.pdf

R2257PMT::R2257PMT() : AbsPMT() {
	// Digitized from above PDF
	efficiencyPoints = {252.38, 0.24, 257.82, 0.40, 265.99, 0.65, 274.15, 1.09, 283.67, 1.76, 291.84, 2.27, 302.72, 2.85, 314.97, 2.92, 327.21, 2.78, 340.82, 2.65, 354.42, 2.65, 370.75, 2.68, 381.63, 2.81, 391.16, 3.06, 399.32, 3.41, 423.81, 5.38, 438.78, 6.60, 456.46, 8.01, 480.95, 9.25, 505.44, 10.18, 534.01, 10.81, 561.22, 10.94, 589.80, 10.81, 617.01, 10.18, 642.86, 9.25, 668.71, 8.10, 689.12, 7.01, 710.88, 5.93, 731.29, 5.01, 750.34, 4.13, 768.03, 3.41, 800.68, 2.27, 829.25, 1.45, 846.94, 0.92, 859.18, 0.56, 874.15, 0.27, 880.95, 0.17, 889.12, 0.10, 895.92, 0.06, 901.36, 0.04, 909.52, 0.02};
}

R2257PMT::~R2257PMT() {
}

G4String R2257PMT::getName(){
	return "R2257";
}

G4double R2257PMT::getDiameter(){
	return 52*mm;
}

G4double R2257PMT::getCathodeDiameter(){
	return 46*mm;
}

G4double R2257PMT::getLength(){
	return 126*mm;
}

//PhotoCathodeType R2257PMT::getPhotocathodeType(){
//	return PhotoCathodeType::Bialkali;
//}

G4Material* R2257PMT::getWindowMaterial(){
	return Materials::getInstance()->getMaterial("borosilicate");
}

G4Material* R2257PMT::getCathodeMaterial(){
	// ERMA cathode SbNa2KCs, p.4
	// see: https://psec.uchicago.edu/library/photomultipliers/Photonis_PMT_basics.pdf
	return Materials::getInstance()->getMaterial("multialkali");
}
