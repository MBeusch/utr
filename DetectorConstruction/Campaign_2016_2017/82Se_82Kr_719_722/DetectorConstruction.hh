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

#ifndef DETECTORCONSTRUCTION_HH
#define DETECTORCONSTRUCTION_HH

#include "G4SystemOfUnits.hh"
#include "G4UnitsTable.hh"
#include "G4VUserDetectorConstruction.hh"

class DetectorConstruction : public G4VUserDetectorConstruction {
  public:
  DetectorConstruction();
  ~DetectorConstruction();

  virtual G4VPhysicalVolume *Construct();
  virtual void ConstructSDandField();

  void print_info(G4double Target2_To_Target, G4double Collimator_To_Target) const;

  private:
  G4double world_x;
  G4double world_y;
  G4double world_z;
  G4double Collimator_Length;
};

#endif
