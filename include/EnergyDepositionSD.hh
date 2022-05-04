/*
utr - Geant4 simulation of the UTR at HIGS
Copyright (C) 2017 the developing team (see README.md)

This file is part of utr.

utr is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

utr is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with utr.  If not, see <http://www.gnu.org/licenses/>.
*/
#pragma once

#include "G4VSensitiveDetector.hh"

#include "TargetHit.hh"

#include <vector>

class G4Step;
class G4HCofThisEvent;

class EnergyDepositionSD : public G4VSensitiveDetector {
  public:
  EnergyDepositionSD(const G4String &name,
                     const G4String &hitsCollectionName);
  virtual ~EnergyDepositionSD();

  // methods from base class
  virtual void Initialize(G4HCofThisEvent *hitCollection);
  virtual G4bool ProcessHits(G4Step *step, G4TouchableHistory *history);
  virtual void EndOfEvent(G4HCofThisEvent *hitCollection);
  unsigned int GetDetectorID() { return detectorID; };
  void SetDetectorID(unsigned int detID) { detectorID = detID; };
  static std::vector<bool> anyDetectorHitInEvent; // Needed for EVENT_EVENTWISE mode, signals whether an entry (row) needs to be written to the root file for the current event (or whether the row would be zeroes only)

  private:
  TargetHitsCollection *hitsCollection;
  G4int detectorID;
  G4int eventID;
};
