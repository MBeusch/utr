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

// Units
#include "G4PhysicalConstants.hh"
#include "G4SystemOfUnits.hh"


// #include "G4UnitsTable.hh"
#include "utrConfig.h"
#include <iostream>
#include <array>


/*
 * Simple geometry to study the evolution of the photon flux when a particle beam traverses a target.
 * To get a snapshot of the kinetic energy distribution of the photon beam at a given penetration depth,
 * the target is segmented along the z-axis into n layers.
 *
 * The simulation uses the ParticleSD detector type, which records particle information at any time when a
 * particle propagates into a new target segment for the first time (or when it is created there).
 *
 * It is recommended to record the following information in the output file, to be able to filter out
 * backward-propagating particles or particles which come from outside the target:
 *
 * EVENT_EKIN
 * EVENT_MOMX
 * EVENT_MOMY
 * EVENT_MOMZ
 * EVENT_POSX
 * EVENT_POSY
 * EVENT_POSZ
 * EVENT_PARTICLE
 * EVENT_VOLUME
 */

DetectorConstruction::DetectorConstruction() {}

DetectorConstruction::~DetectorConstruction() {}

G4VPhysicalVolume *DetectorConstruction::Construct() {


  /***************** Target Properties *****************/

  const G4String detector_material_name = "G4_Si";   // <------ adjust material
  const double detector_thickness = 0.2 * mm;
  const double detector_edgelength = 10. * mm;

  /***************** Materials *****************/

  G4NistManager *nist = G4NistManager::Instance();
  G4Material *detector_material = nist->FindOrBuildMaterial(detector_material_name);  // <------ material with natural density
  //auto *detector_material = new G4Material("detector_material", new_density, nist->FindOrBuildMaterial(detector_material_name));  // <------  material with arbitrary density ()
  G4Material *vacuum = nist->FindOrBuildMaterial("G4_Galactic");

  /***************** World Volume *****************/

  const double world_x = 2. * detector_edgelength;
  const double world_y = 2. * detector_edgelength;
  const double world_z = 10. * mm;

  G4Box *world_solid = new G4Box("world_solid", world_x, world_y, world_z);
  G4LogicalVolume *world_logical = new G4LogicalVolume(world_solid, vacuum, "world_logical");
  G4VPhysicalVolume *world_physical = new G4PVPlacement(0, G4ThreeVector(), world_logical, "world", 0, false, 0);

  
  /******************** Detector ******************/

	G4Box *Detector_solid = new G4Box("Detector_solid", detector_edgelength, detector_edgelength, detector_thickness);
	G4LogicalVolume *Detector_logical = new G4LogicalVolume(Detector_solid, detector_material, "Detector_logical", 0, 0, 0);

	//Visualisierung (Farbe)
	Detector_logical->SetVisAttributes(new G4VisAttributes(G4Color::Cyan()));
	new G4PVPlacement(0, G4ThreeVector(0, 0, 0), Detector_logical, "Detector", world_logical, false, 0);
	

  return world_physical;
}

// Definiere das Detektorvolumen als Detektor/sensitives Volumen in Geant4
void DetectorConstruction::ConstructSDandField() {

	// Use ParticleSD instead of EnergyDepositionSD, as ParticleSD records the hits of each particle within a event individually regardless whether the particle actually deposited energy in the detector or not.
	// An EnergyDepositionSD however only records a single particle per event and only if it actually left some energy in the detector
	EnergyDepositionSD *DetectorSD = new EnergyDepositionSD("Detector_logical", "Detector_logical");
	G4SDManager::GetSDMpointer()->AddNewDetector(DetectorSD);
	DetectorSD->SetDetectorID(0);
	SetSensitiveDetector("Detector_logical", DetectorSD, true);
}
