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
 * Setup of the Sn112 NRF experiment from the 2015 DHIPS campaign
 * The purpose of this experiment was to measure the excitation strength
 * of the 2^+_1 state of 112Sn in NRF relative to the known strengths
 * of excited states in 27Al and 59Co. This was supposed to clarify the
 * experimental situation of the systematics of B(E2; 0^+_1 -> 2^+_1)
 * strengths in the tin isotopic chain.
 *
 * An alternative would have been to determine two B(E2) strengths
 * of the tin isotopes 112Sn and 116Sn simultaneously. A sandwich
 * target which would be necessary for this alternative experiment
 * has also been implemented.
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
#include "BeamPipe_Downstream.hh"
#include "BeamPipe_Upstream.hh"
#include "Detectors.hh"
#include "LeadCastle.hh"
#include "RadiatorTarget.hh"
#include "Sn112_Target_Co_powder.hh"

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

DetectorConstruction::DetectorConstruction() {
}

DetectorConstruction::~DetectorConstruction() {}

G4VPhysicalVolume *DetectorConstruction::Construct() {
  G4NistManager *nist = G4NistManager::Instance();
  G4Material *air = nist->FindOrBuildMaterial("G4_AIR");

  /************************* World volume *****************/

  /***************** WORLD *****************/

  World_x = 6000. * mm;
  World_y = 6000. * mm;
  World_z = 6000. * mm;

  G4Box *World_dim =
      new G4Box("World_Solid", World_x * 0.5, World_y * 0.5, World_z * 0.5);

  G4LogicalVolume *World_Logical =
      new G4LogicalVolume(World_dim, air, "World_Logical", 0, 0, 0);

  G4VisAttributes *world_vis = new G4VisAttributes(true, G4Color::Red());
  world_vis->SetForceWireframe(true);

  World_Logical->SetVisAttributes(world_vis);

  G4VPhysicalVolume *World_Physical =
      new G4PVPlacement(0, G4ThreeVector(), World_Logical, "World", 0, false, 0);

  /***************************************************/
  /***************** INITIALIZATIONS *****************/
  /***************************************************/

  BeamPipe_Upstream BeamPipe_Upstream(World_Logical);
  RadiatorTarget RadiatorTarget(World_Logical);
  LeadCastle LeadCastle(World_Logical);
  Detectors Detectors(World_Logical);

  /***************************************************/
  /*****************  CONSTRUCTION  *****************/
  /***************************************************/

  BeamPipe_Upstream.Construct(G4ThreeVector(0, 0, -1500 * mm), 0.1); // Position guesstimated
  RadiatorTarget.Construct(G4ThreeVector(0, 0, -1190 * mm), "Radiator1", "Au", 3 * mm, "Al", 0.); // Position guesstimated
  RadiatorTarget.Construct(G4ThreeVector(0, 0, -1160 * mm), "Radiator2", "Au", 3 * mm, "Al", 0.); // Position guesstimated

  auto BGO1 = BGO(World_Logical, "BGO1");
  auto BGO2 = BGO(World_Logical, "BGO2");
  auto BGOPol = BGO(World_Logical, "BGOPol");
  // auto HPGe1 = DetectorInfo::get(DetectorDHIPS::HPGe1, PositionDHIPS::Right90);
  auto HPGe2 = DetectorInfo::get(DetectorDHIPS::HPGe2, PositionDHIPS::Left130, false);
  auto HPGePol = DetectorInfo::get(DetectorDHIPS::HPGePol, PositionDHIPS::Left90);

  // Detectors.ConstructDetectorBGO(G4ThreeVector(), HPGe1, BGO1);
  Detectors.ConstructDetectorBGO(G4ThreeVector(), HPGe2, BGO2);
  Detectors.ConstructDetectorBGO(G4ThreeVector(), HPGePol, BGOPol);
  LeadCastle.Construct(G4ThreeVector(), &BGO1, &BGO2, &BGOPol);
  Detectors.ConstructDetectorFilter(G4ThreeVector(), HPGe1, 10. * mm, 10. * mm);
  Detectors.ConstructDetectorFilter(G4ThreeVector(), HPGe2, 10. * mm, 5. * mm);
  Detectors.ConstructDetectorFilter(G4ThreeVector(), HPGePol, 10. * mm, 10. * mm);

#ifdef USE_TARGETS
  Sn112_Target_Co_powder Sn112_Target_Co_powder(World_Logical);
  Sn112_Target_Co_powder.Construct(G4ThreeVector(0., 0., 0.));
#endif

  print_info();
  return World_Physical;
}

void DetectorConstruction::ConstructSDandField() {

  // EnergyDepositionSD *HPGe1SD = new EnergyDepositionSD("HPGe1", "HPGe1");
  // G4SDManager::GetSDMpointer()->AddNewDetector(HPGe1SD);
  // HPGe1SD->SetDetectorID(1);
  // SetSensitiveDetector("HPGe1", HPGe1SD, true);

  EnergyDepositionSD *HPGe2SD = new EnergyDepositionSD("HPGe2", "HPGe2");
  G4SDManager::GetSDMpointer()->AddNewDetector(HPGe2SD);
  HPGe2SD->SetDetectorID(2);
  SetSensitiveDetector("HPGe2", HPGe2SD, true);

  EnergyDepositionSD *HPGePolSD = new EnergyDepositionSD("HPGePol", "HPGePol");
  G4SDManager::GetSDMpointer()->AddNewDetector(HPGePolSD);
  HPGePolSD->SetDetectorID(3);
  SetSensitiveDetector("HPGePol", HPGePolSD, true);
}

void DetectorConstruction::print_info() const {
  printf("==============================================================\n");
  printf("  DetectorConstruction: Info (all dimensions in mm)\n");
  G4cout << G4endl;
  printf("> World dimensions             : ( %5.2f, %5.2f, %5.2f )\n", World_x, World_y, World_z);
  printf("==============================================================\n");
}
