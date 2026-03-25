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

const size_t n_target_layers = 1; // Determines the number of layers of the target.

DetectorConstruction::DetectorConstruction() {}

DetectorConstruction::~DetectorConstruction() {}

G4VPhysicalVolume *DetectorConstruction::Construct() {

  /***************** Target Properties *****************/

  const double detector_length = 1. * mm;
  const double distance = 5. * mm;
  const double target_thickness_peek = 5. * mm;
  const double target_radius_peek = 5. * mm;
  const double target_thickness_steel = 2.5 * mm;
  const double target_radius_steel = 5. * mm;
  //const G4String target_material_name = "G4_Pb";

  /***************** Materials *****************/

  G4NistManager *nist = G4NistManager::Instance();

  // Materials
  //G4Material *target_material = nist->FindOrBuildMaterial(target_material_name);
  //auto *target_material = new G4Material("target_material", new_density, nist->FindOrBuildMaterial(target_material_name));

  // Define elements
  G4Material *vacuum = nist->FindOrBuildMaterial("G4_Galactic");
  G4Element* elO  = nist->FindOrBuildElement("O");
  G4Element* elC  = nist->FindOrBuildElement("C");
  G4Element* elH  = nist->FindOrBuildElement("H");
  G4Element* elFe = nist->FindOrBuildElement("Fe");
  G4Element* elCr = nist->FindOrBuildElement("Cr");
  G4Element* elNi = nist->FindOrBuildElement("Ni");
  G4Element* elMo = nist->FindOrBuildElement("Mo");
  G4Element* elMn = nist->FindOrBuildElement("Mn");
  G4Element* elSi = nist->FindOrBuildElement("Si");
  G4Element* elCl = nist->FindOrBuildElement("Cl");

  // PEEK container
  G4double peek_density = 1.32 * g/cm3;
  G4int ncomponents;
  G4Material* peek = new G4Material("PEEK", peek_density, ncomponents = 3);
  peek->AddElement(elC, 19);
  peek->AddElement(elH, 12);
  peek->AddElement(elO, 3);
  
  // Steel pipe
  G4double steel_density = 8.0 * g/cm3;  
  G4Material* stainless316L = new G4Material("StainlessSteel_1_4404", steel_density, 7);
  // Composition based on typical 316L (AISI 1.4404) weight percentages
  stainless316L->AddElement(elFe, 0.685);  // ~68.5% Iron
  stainless316L->AddElement(elCr, 0.17);   // ~17% Chromium
  stainless316L->AddElement(elNi, 0.12);   // ~12% Nickel
  stainless316L->AddElement(elMo, 0.025);  // ~2.5% Molybdenum
  stainless316L->AddElement(elMn, 0.02);   // ~2% Manganese
  stainless316L->AddElement(elSi, 0.01);   // ~1% Silicon
  stainless316L->AddElement(elC, 0.0003);  // ~0.03% Carbon (low-carbon 316L)


  /***************** World Volume *****************/

  const double world_x = 4. * target_radius_peek;
  const double world_y = 4. * target_radius_peek;
  const double world_z = 4. * target_thickness_peek + distance + detector_length;

  G4Box *world_solid = new G4Box("world_solid", world_x, world_y, world_z);
  G4LogicalVolume *world_logical = new G4LogicalVolume(world_solid, vacuum, "world_logical");
  G4VPhysicalVolume *world_physical = new G4PVPlacement(0, G4ThreeVector(), world_logical, "world", 0, false, 0);

  /***************** Target 1 *****************/

  auto *target_solid_steel = new G4Tubs("target_solid_steel", 0., target_radius_steel, 0.5 * target_thickness_steel, 0., twopi);
  auto *target_logical_steel = new G4LogicalVolume(target_solid_steel, stainless316L, "target_logical_steel");

  target_logical_steel->SetVisAttributes(new G4VisAttributes(G4Color::Cyan()));
  new G4PVPlacement(0, G4ThreeVector(0., 0., target_thickness_steel/2), target_logical_steel,  "Target2", world_logical, false, 0);
  

  /***************** Target 2 **************

  auto *target_solid_peek = new G4Tubs("target_solid_peek", 0., target_radius_peek, 0.5 * target_thickness_peek, 0., twopi);
  auto *target_logical_peek = new G4LogicalVolume(target_solid_peek, peek, "target_logical_peek");

  target_logical_peek->SetVisAttributes(new G4VisAttributes(G4Color::Yellow()));
  new G4PVPlacement(0, G4ThreeVector(0., 0., target_thickness_steel + distance + target_thickness_peek/2), target_logical_peek,  "Target1", world_logical, false, 0);
  
  ***/

 
  /******************** Detector ******************/

	G4Tubs *Detector_solid = new G4Tubs("Detector_solid", 0, target_radius_peek, detector_length * 0.5, 0, twopi);
	G4LogicalVolume *Detector_logical = new G4LogicalVolume(Detector_solid, vacuum, "Detector_logical", 0, 0, 0);

	//Visualisierung (Farbe)
	Detector_logical->SetVisAttributes(new G4VisAttributes(G4Color::Blue()));
	new G4PVPlacement(0, G4ThreeVector(0, 0, target_thickness_peek + distance + target_thickness_steel + distance + detector_length/2), Detector_logical, "Detector", world_logical, false, 0);
	

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
