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

#include <sstream>
#include <vector>

#include "G4NistManager.hh"

#include "G4PVPlacement.hh"
#include "G4PhysicalConstants.hh"
#include "G4SystemOfUnits.hh"
#include "G4Tubs.hh"
#include "G4VisAttributes.hh"

#include "Al27_Reference.hh"

using std::stringstream;
using std::vector;

Al27_Reference::Al27_Reference(G4LogicalVolume *World_Log) : World_Logical(World_Log) {}

void Al27_Reference::Construct(G4ThreeVector global_coordinates) {

  // === Container dimensions and densities ===
  G4double container_inner_radius = 0.5 * 12. * mm;
  G4double container_outer_radius = 0.5 * 16. * mm;
  G4double container_bottom_thickness = 1.5 * mm;
  G4double container_lid_thickness = 6. * mm;
  G4double container_inner_length = 16 * mm;
  G4double offset_targetCenter_containerBottom = 3 * mm;
  G4double puo2_thickness = 6 * mm;  // ESTIMATED
  G4double al_radius = 0.5 * cm;
  G4double outer_container_lid_thickness = 4 *mm;
  G4double outer_container_bottom_thickness = 2 *mm;
  G4double outer_container_inner_radius = 0.5 * 43.5 *mm;
  G4double outer_container_outer_radius = 0.5 * 63.5 *mm;
  G4double outer_container_inner_length = 16 * mm;
  

  // === G4 materials ===
  G4NistManager *nist = G4NistManager::Instance();

  G4Element* elO  = nist->FindOrBuildElement("O");
  G4Element* elC = nist->FindOrBuildElement("C");
  G4Element* elH = nist->FindOrBuildElement("H");

  // === Create PEEK material ===
  G4double density = 1.32 * g/cm3;
  G4int ncomponents;
  G4Material* peek = new G4Material("PEEK", density, ncomponents = 3);

  peek->AddElement(elC, 19);
  peek->AddElement(elH, 12);
  peek->AddElement(elO, 3);

  // === Define Al reference material and dimensions ===
  G4double al_mass1 = 0.1176 * g;
  G4double al_mass5 = 0.4735 * g;
  G4double al_density = 2.699 * g/cm3;  // nat. density from Wikipedia (22.05.25): 2.699 g/cm3
  G4double al_thickness1 = al_mass1/(pi * al_radius * al_radius * al_density);
  G4double al_thickness5 = al_mass5/(pi * al_radius * al_radius * al_density);

  G4Material* matAl = nist->FindOrBuildMaterial("G4_Al");

  // Define inner container positions
  G4ThreeVector wall_position = G4ThreeVector(0., 0., - 0.5 * container_inner_length + offset_targetCenter_containerBottom);
  G4ThreeVector bottom_position = G4ThreeVector(0., 0., 0.5 * container_bottom_thickness + offset_targetCenter_containerBottom);  
  G4ThreeVector lid_position = G4ThreeVector(0., 0., - puo2_thickness - 0.5 * container_lid_thickness + offset_targetCenter_containerBottom);  
  
  // === Construct target container housing ===

  // Outer Container wall
  G4Tubs *container_wall_solid = new G4Tubs("container_wall_solid", outer_container_inner_radius, outer_container_outer_radius, outer_container_inner_length * 0.5, 0., twopi);
  G4LogicalVolume *container_wall_logical = new G4LogicalVolume(container_wall_solid, peek, "container_wall_logical");
  container_wall_logical->SetVisAttributes(G4Color::White());
  // Positioning wall
  G4ThreeVector outer_wall_position = G4ThreeVector(0., 0., - 0.5 * outer_container_inner_length + offset_targetCenter_containerBottom + container_bottom_thickness);  
  new G4PVPlacement(0, global_coordinates + outer_wall_position, container_wall_logical, "container_wall", World_Logical, false, 0);

  // Outer Container bottom
  G4Tubs *container_bottom_solid = new G4Tubs("container_bottom_solid", 0., outer_container_outer_radius, outer_container_bottom_thickness * 0.5, 0., twopi);
  G4LogicalVolume *container_bottom_logical = new G4LogicalVolume(container_bottom_solid, peek, "container_bottom_logical");
  container_bottom_logical->SetVisAttributes(G4Color::Grey());
  // Positioning bottom
  G4ThreeVector outer_bottom_position = G4ThreeVector(0., 0., container_bottom_thickness + offset_targetCenter_containerBottom + 0.5 * outer_container_bottom_thickness);  
  new G4PVPlacement(0, global_coordinates + outer_bottom_position, container_bottom_logical, "container_bottom", World_Logical, false, 0);

  // Outer Container lid
  G4Tubs *container_lid_solid = new G4Tubs("container_lid_solid", 0., outer_container_outer_radius, outer_container_lid_thickness * 0.5, 0., twopi);
  G4LogicalVolume *container_lid_logical = new G4LogicalVolume(container_lid_solid, peek, "container_lid_logical");
  container_lid_logical->SetVisAttributes(G4Color::Grey());
  // Positioning lid
  G4ThreeVector outer_lid_position = G4ThreeVector(0., 0., outer_wall_position.z() - 0.5 * outer_container_inner_length - 0.5 * outer_container_lid_thickness);  
  new G4PVPlacement(0, global_coordinates + outer_lid_position, container_lid_logical, "container_lid", World_Logical, false, 0);
  

  // === Construct Al27 reference target ===
  //G4Tubs* al_solid = new G4Tubs("Al_solid", 0., al_radius, al_thickness1 * 0.5, 0., twopi);  // 1 Al reference disk
  G4Tubs* al_solid = new G4Tubs("Al_solid", 0., al_radius, al_thickness5 * 0.5, 0., twopi);   // 5 Al reference disks
  G4LogicalVolume* al_logical = new G4LogicalVolume(al_solid, matAl, "Al_logical");
  al_logical->SetVisAttributes(G4Color::Cyan());
  // Place the target in the container
  //G4ThreeVector al_position = G4ThreeVector(0., 0., wall_position.z() - 0.5 * container_inner_length - outer_container_lid_thickness - 0.5 * al_thickness1);  // 1 Al reference disk
  G4ThreeVector al_position = G4ThreeVector(0., 0., outer_wall_position.z() - 0.5 * outer_container_inner_length - outer_container_lid_thickness - 0.5 * al_thickness5);  // 5 Al reference disks
  new G4PVPlacement(nullptr, global_coordinates + al_position, al_logical, "Al", World_Logical, false, 0);

}
