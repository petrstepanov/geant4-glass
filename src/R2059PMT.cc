/*
 * R2059PMT.cpp
 *
 *  Created on: Feb 20, 2021
 *      Author: petrstepanov
 */

#include <R2059PMT.hh>
#include "G4SystemOfUnits.hh"
#include "Materials.hh"

// Info here: https://www.hamamatsu.com/resources/pdf/etd/R1828-01_R2059_TPMH1259E.pdf

R2059PMT::R2059PMT() : AbsPMT() {
	// Digitized from above PDF
	efficiencyPoints = {181.6, 15.9, 205.7, 15.9, 231.0, 16.4, 252.9, 18.3, 273.6, 21.0, 296.6, 23.3, 320.7, 25.6, 343.7, 27.1, 367.8, 28.1, 387.4, 28.1, 411.5, 27.1, 434.5, 25.0, 455.2, 21.7, 474.7, 18.9, 493.1, 15.9, 510.3, 13.3, 526.4, 11.1, 541.4, 9.1, 555.2, 7.4, 569.0, 5.9, 581.6, 4.7, 592.0, 3.8, 600.0, 3.0, 608.0, 2.3, 613.8, 1.9, 619.5, 1.4, 626.4, 1.1, 631.0, 0.9, 636.8, 0.7, 642.5, 0.6, 647.1, 0.4, 651.7, 0.3, 657.5, 0.3, 662.1, 0.2, 666.7, 0.2, 672.4, 0.1, 675.9, 0.1, 681.6, 0.1, 685.1, 0.1, 689.7, 0.0, 694.3, 0.0, 698.9, 0.0, 703.4, 0.0, 708.0, 0.0};
}

R2059PMT::~R2059PMT() {
}

G4String R2059PMT::getName(){
	return "R2059";
}

G4double R2059PMT::getDiameter(){
	return 53*mm;
}

G4double R2059PMT::getCathodeDiameter(){
	return 46*mm;
}

G4double R2059PMT::getLength(){
	return 170*mm;
}

//PhotoCathodeType R2059PMT::getPhotocathodeType(){
//	return PhotoCathodeType::Bialkali;
//}

G4Material* R2059PMT::getWindowMaterial(){
	return Materials::getInstance()->getMaterial("fused-silica");
}

G4Material* R2059PMT::getCathodeMaterial(){
	return Materials::getInstance()->getMaterial("bialkali");
}
