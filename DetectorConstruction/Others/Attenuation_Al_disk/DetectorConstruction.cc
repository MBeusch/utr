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

/*
 * Setup of the Pu242 NRF experiment from the 2022 DHIPS campaign
 * The purpose of this experiment was to measure the low-energy dipole response.
 */

#include "DetectorConstruction.hh"

// Materials
#include "G4Material.hh"
#include "G4NistManager.hh"
#include "Materials.hh"
Materials *materials = Materials::Instance();

// Geometry
#include "G4Box.hh"
#include "G4LogicalVolume.hh"
#include "G4PVPlacement.hh"
#include "G4VisAttributes.hh"
#include "globals.hh"

// Sensitive Detectors
#include "EnergyDepositionSD.hh"
#include "G4SDManager.hh"
#include "ParticleSD.hh"
#include "SecondarySD.hh"

// Detectors

// Geometry
#include "G4Box.hh"
#include "G4GenericTrap.hh"
#include "G4LogicalVolume.hh"
#include "G4PVPlacement.hh"
#include "G4SubtractionSolid.hh"
#include "G4ThreeVector.hh"
#include "G4Tubs.hh"
#include "G4TwoVector.hh"
#include "G4UnionSolid.hh"
#include "G4VisAttributes.hh"
#include "globals.hh"

// Units
#include "G4PhysicalConstants.hh"
#include "G4SystemOfUnits.hh"

#include <iostream>


DetectorConstruction::DetectorConstruction() {}

DetectorConstruction::~DetectorConstruction() {}

G4VPhysicalVolume *DetectorConstruction::Construct() {

  /***************** Target Properties *****************/


  // Al target dimensions
  G4double al_mass1 = 0.1176 * g;
  G4double al_mass5 = 0.4735 * g;

  // Choose Al thickness via mass:
  G4double al_mass = al_mass1;    // <--- CHANGE here: al_mass5 <-> al_mass1
  
  G4double al_radius = 0.5 * cm;
  G4double al_density = 2.699 * g/cm3;
  G4double al_thickness = al_mass / (pi * al_radius * al_radius * al_density);

  // Detector properties
  const double detector_length = 1. * mm;
  const double detector_radius = 10. * mm;
  const double distance = 10. * cm;

  /***************** Materials *****************/

  G4NistManager *nist = G4NistManager::Instance();
  G4Material *vacuum = nist->FindOrBuildMaterial("G4_Galactic");
  //G4Material *target_material = nist->FindOrBuildMaterial(target_material_name);
  G4Material* matAl = nist->FindOrBuildMaterial("G4_Al");

  /***************** World Volume *****************/

  const double world_x = 50 *cm;
  const double world_y = 50 *cm;
  const double world_z = 50 *cm;

  G4Box *world_solid = new G4Box("world_solid", world_x, world_y, world_z);
  G4LogicalVolume *world_logical = new G4LogicalVolume(world_solid, vacuum, "world_logical");
  G4VPhysicalVolume *world_physical = new G4PVPlacement(0, G4ThreeVector(), world_logical, "world", 0, false, 0);

  /***************** Target *****************/

  // Al-27 disk 
  G4Tubs* al_solid = new G4Tubs("AlSolid", 0, al_radius, al_thickness*0.5, 0, twopi);  
  G4LogicalVolume* al_log = new G4LogicalVolume(al_solid, matAl, "AlLog");
  al_log->SetVisAttributes(G4Color::Blue());
  G4ThreeVector al_pos(0, 0, 0);
  new G4PVPlacement(nullptr, al_pos, al_log, "Al", world_logical, false, 0);

  
  /******************** Detector ******************/

  // Rotation: tilt by 130° around X-axis
  G4RotationMatrix* rotation = new G4RotationMatrix();
  rotation->rotateX(130. * deg);

  // Define offset vector (local position before rotation)
  G4ThreeVector local_offset(0, 0, al_thickness / 2. + distance + detector_length / 2.);

  // Apply rotation to get global position
  G4ThreeVector det_pos = rotation->inverse() * local_offset;


  // Define detector solid and logical volume
  G4Tubs *Detector_solid = new G4Tubs("Detector_solid", 0, detector_radius, detector_length * 0.5, 0, twopi);
  G4LogicalVolume *Detector_logical = new G4LogicalVolume(Detector_solid, vacuum, "Detector_logical");
  Detector_logical->SetVisAttributes(new G4VisAttributes(G4Color::Blue()));

  // Place detector at rotated position, with same rotation
  new G4PVPlacement(rotation, det_pos, Detector_logical, "Detector", world_logical, false, 0);
  

  return world_physical;
}

// Definiere das Detektorvolumen als Detektor/sensitives Volumen in Geant4
void DetectorConstruction::ConstructSDandField() {

	// Use ParticleSD instead of EnergyDepositionSD, as ParticleSD records the hits of each particle within a event individually regardless whether the particle actually deposited energy in the detector or not.
	// An EnergyDepositionSD however only records a single particle per event and only if it actually left some energy in the detector
	ParticleSD *DetectorSD = new ParticleSD("Detector_logical", "Detector_logical");
	G4SDManager::GetSDMpointer()->AddNewDetector(DetectorSD);
	DetectorSD->SetDetectorID(0);
	SetSensitiveDetector("Detector_logical", DetectorSD, true);
}
