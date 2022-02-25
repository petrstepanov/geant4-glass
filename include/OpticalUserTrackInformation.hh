#ifndef OpticalUserTrackInformation_h
#define OpticalUserTrackInformation_h 1

#include <G4VUserTrackInformation.hh>

class OpticalUserTrackInformation: public G4VUserTrackInformation {
public:
  OpticalUserTrackInformation(G4double localTime, G4double globalTime, G4double properTime, G4double wavelength);
  ~OpticalUserTrackInformation();

  G4double localTime;
  G4double globalTime;
  G4double properTime;

  G4double wavelength;
};

#endif
