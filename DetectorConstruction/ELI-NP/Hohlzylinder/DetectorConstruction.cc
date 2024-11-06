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
#include "DetectorConstruction.hh"

// Materials
#include "G4Material.hh"
#include "G4NistManager.hh"

// Geometry
#include "G4Box.hh"
#include "G4LogicalVolume.hh"
#include "G4PVPlacement.hh"
#include "G4ThreeVector.hh"
#include "G4Tubs.hh"
#include "G4VisAttributes.hh"
#include "globals.hh"
#include "G4SubtractionSolid.hh"

// Sensitive Detectors
#include "G4SDManager.hh"
#include "EnergyDepositionSD.hh"
#include "ParticleSD.hh"

#include "HPGe_Coaxial.hh"               // <- ?
#include "HPGe_Coaxial_Properties.hh"    // <- ?
#include "Units.hh"                      // <- ?


// Hohlzylinder
#include "Hohlzylinder.hh"

// Units
#include "G4PhysicalConstants.hh"
#include "G4SystemOfUnits.hh"


// #include "G4UnitsTable.hh"
#include "utrConfig.h"
#include <iostream>
#include <array>

#include "G4GDMLParser.hh"
#include "G4TessellatedSolid.hh"


/*
 * Simple geometry to determine the efficiency of a detector implemented in this DetectorConstruction.
 * 
 * The simulation uses the ParticleSD detector type, which records particle information at any time when a particle propagates into a new target segment for the first time (or when it is created there).
 *
 * The detector is a p-type coaxial HPGe detector with a relative efficiency of 24 % at 1332 keV from ORTEC.
 */

G4VPhysicalVolume *DetectorConstruction::Construct() {

  /***************** Setup Properties *****************/
  // There is no filter

  // In this case distance_source_detector describes the distance between end cap and zero point
  const double distance_source_detector = 0.; // 100. * mm;    // <------ adjust distance from source to detector
//  const G4String filter_material_name = "G4_Pb";     // <------ adjust filter material
//  const double filter_thickness = 0.5 * mm;           // <------ adjust filter thickness

  /***************** Materials *****************/

  G4NistManager *nist = G4NistManager::Instance();
  G4Material *air = nist->FindOrBuildMaterial("G4_AIR");
  //G4Material *vacuum = nist->FindOrBuildMaterial("G4_Galactic");
  G4Material *Al = nist->FindOrBuildMaterial("G4_Al");
 // G4Material *PLA = nist->FindOrBuildMaterial("G4_PLA");
  //G4Material *polyethylene = nist->FindOrBuildMaterial("G4_POLYETHYLENE");
  G4Material *Ge = nist->FindOrBuildMaterial("G4_Ge");
  G4Material *Mylar = nist->FindOrBuildMaterial("G4_Mylar");

  //Epoxy from https://www.phenix.bnl.gov/~suhanov/ncc/geant/rad-source/src/ExN03DetectorConstruction.cc
  //G4double density = 1.2*g/cm3;
  //G4Material* Epoxy = new G4Material("Epoxy" , density, 4);
  //Epoxy->AddElement(nist->FindOrBuildElement("H"), 32);
  //Epoxy->AddElement(nist->FindOrBuildElement("C"), 15);
  //Epoxy->AddElement(nist->FindOrBuildElement("N"), 2);
  //Epoxy->AddElement(nist->FindOrBuildElement("O"), 4);
  
  
  /***************** World Volume *****************/

  const double world_x =  1. *m; //HPGe_GEM20P470_props.dewar_outer_radius;
  const double world_y =  1. *m; //HPGe_GEM20P470_props.dewar_outer_radius;
  const double world_z =  1. *m; //2 * HPGe_GEM20P470_props.dewar_length + distance_source_detector;

  G4Box *world_solid = new G4Box("world_solid", world_x, world_y, world_z);
  G4LogicalVolume *world_logical = new G4LogicalVolume(world_solid, air, "world_logical");
  G4VPhysicalVolume *world_physical = new G4PVPlacement(0, G4ThreeVector(), world_logical, "world", 0, false, 0);

  /***************** Hohlzylinder *********************************/

  Hohlzylinder *hohlzylinder = new Hohlzylinder(world_logical);
  hohlzylinder->Put(0., 0., 128.5, 180. *deg, 0., 0.);


  return world_physical;

}
