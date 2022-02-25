/*
 * S13360_6025CS_Custom.cc
 *
 *  Created on: Feb 22, 2021
 *      Author: petrstepanov
 */

#include <S13360_6025CS_Custom.hh>
#include "G4SystemOfUnits.hh"
#include "Materials.hh"

// More info here:
// https://www.hamamatsu.com/resources/pdf/ssd/s13360_series_kapd1052e.pdf

S13360_6025CS_Custom::S13360_6025CS_Custom() : AbsMPPC() {
	efficiencyPoints = {302.3, 6.5, 322.0, 7.4, 340.9, 7.8, 359.8, 8.2, 373.7, 9.2, 392.6, 10.5, 413.1, 12.1, 430.4, 12.9, 446.8, 14.6, 459.9, 16.5, 473.1, 18.2, 490.3, 20.9, 501.8, 22.8, 517.4, 25.5, 524.8, 26.4, 551.1, 27.8, 570.0, 28.8, 596.2, 29.5, 611.0, 29.0, 636.5, 27.2, 661.1, 25.8, 680.8, 24.8, 704.6, 22.4, 730.0, 19.6, 754.7, 16.7, 787.5, 13.8, 819.5, 11.4, 846.6, 9.4, 876.2, 7.1, 911.5, 4.7, 955.8, 2.7, 999.3, 1.4};
}

S13360_6025CS_Custom::~S13360_6025CS_Custom() {
	// TODO Auto-generated destructor stub
}

G4String S13360_6025CS_Custom::getName(){
	return "S13360-6025CS-Custom";
}

G4double S13360_6025CS_Custom::getSize(){
	return 12*mm;
}

G4double S13360_6025CS_Custom::getLength(){
	return 20*mm; // original 2*mm, increased to see photon tracks terminated
}

G4double S13360_6025CS_Custom::getWindowThickness(){
	return 0.45*mm;
}

G4double S13360_6025CS_Custom::getCathodeThickness(){
	return getLength()-getWindowThickness();
}

G4Material* S13360_6025CS_Custom::getWindowMaterial(){
	return Materials::getInstance()->getMaterial("silicone-resin");
}

G4Material* S13360_6025CS_Custom::getCathodeMaterial(){
	return Materials::getInstance()->getMaterial("silicon-crystal");
}
