#include "OpticalUserTrackInformation.hh"

OpticalUserTrackInformation::OpticalUserTrackInformation(G4double _localTime, G4double _globalTime, G4double _properTime, G4double _wavelength) :
    localTime(_localTime), globalTime(_globalTime), properTime(_properTime), wavelength(_wavelength){
}

OpticalUserTrackInformation::~OpticalUserTrackInformation(){}
