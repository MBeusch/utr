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

#include "Pu242_Target_full.hh"

using std::stringstream;
using std::vector;

Pu242_Target_full::Pu242_Target_full(G4LogicalVolume *World_Log) : World_Logical(World_Log) {}

void Pu242_Target_full::Construct(G4ThreeVector global_coordinates) {

  G4NistManager *nist = G4NistManager::Instance();

  G4Element* elPu = nist->FindOrBuildElement("Pu");
  G4Element* elO  = nist->FindOrBuildElement("O");
  G4Element* elC  = nist->FindOrBuildElement("C");
  G4Element* elH  = nist->FindOrBuildElement("H");
  G4Material* matAl = nist->FindOrBuildMaterial("G4_Al");

  // Materials
  G4Material* PuO2 = new G4Material("PuO2", 1.11575 * g / (pi * sqr(6.*mm) * 6.*mm), 2);
  PuO2->AddElement(elPu,1);
  PuO2->AddElement(elO,2);
  G4Material* peek = new G4Material("PEEK", 1.32 * g/cm3, 3);
  peek->AddElement(elC,19); peek->AddElement(elH,12); peek->AddElement(elO,3);

  // Al target dimensions
  G4double al_radius = 0.5 * cm;
  G4double al_mass1 = 0.1176 * g;
  G4double al_mass5 = 0.4735 * g;
  G4double al_density = 2.699 * g/cm3;
  G4double al_thickness1 = al_mass1/(pi * al_radius * al_radius * al_density);
  G4double al_thickness5 = al_mass5 / (pi * al_radius * al_radius * al_density);

  // Global offset
  G4double offsetZ = 3*mm; // offset_targetCenter_containerBottom

  // Outer container dimensions
  G4double outer_ir = 0.5 * 43.5 * mm; // inner radius
  G4double outer_or = 0.5 * 63.5 * mm; // outer radius
  G4double outer_h = 16 * mm;
  G4double outer_bottom_th = 2 * mm;
  G4double outer_lid_th = 4 * mm;
  
  // Inner container dimensions (PuO2 container)
  G4double inner_ir = 0.5 * 12 * mm;
  G4double inner_or = 0.5 * 16 * mm;
  G4double inner_h = 13 * mm;
  G4double inner_bottom_th = 1.5 * mm; // 3 * mm;
  G4double inner_lid_th = 6 * mm;
  G4double puo2_th = 6 * mm;

  /*******************************************************************************************/

  // Outer container wall
  G4Tubs* outer_wall_solid = new G4Tubs("OuterWall", outer_ir, outer_or, 0.5*outer_h, 0, twopi);
  G4LogicalVolume* outer_wall_log = new G4LogicalVolume(outer_wall_solid, peek, "OuterWallLog");
  outer_wall_log->SetVisAttributes(G4Color::White());
  G4ThreeVector outer_wall_pos(0, 0, -0.5 * outer_h + offsetZ + inner_bottom_th);
  new G4PVPlacement(nullptr, global_coordinates + outer_wall_pos, outer_wall_log, "OuterWall", World_Logical, false, 0);

  // Outer container bottom
  G4Tubs* outer_bottom_solid = new G4Tubs("OuterBottom", 0, outer_or, 0.5*outer_bottom_th, 0, twopi);
  G4LogicalVolume* outer_bottom_log = new G4LogicalVolume(outer_bottom_solid, peek, "OuterBottomLog");
  outer_bottom_log->SetVisAttributes(G4Color::Blue());
  G4ThreeVector outer_bottom_pos(0, 0, 0.5 * outer_bottom_th + offsetZ + inner_bottom_th);
  new G4PVPlacement(nullptr, global_coordinates + outer_bottom_pos, outer_bottom_log, "OuterBottom", World_Logical, false, 0);

  // Outer Container lid
  G4Tubs *outer_lid_solid = new G4Tubs("OuterLid", 0, outer_or, 0.5 * outer_lid_th, 0, twopi);
  G4LogicalVolume *outer_lid_log = new G4LogicalVolume(outer_lid_solid, peek, "OuterLidLog");
  outer_lid_log->SetVisAttributes(G4Color::Blue());
  G4ThreeVector outer_lid_pos(0, 0, outer_wall_pos.z() - 0.5 * outer_h - 0.5 * outer_lid_th);
  new G4PVPlacement(nullptr, global_coordinates + outer_lid_pos, outer_lid_log, "OuterLid", World_Logical, false, 0);

  /*******************************************************************************************/

  // Inner container wall
  G4Tubs* inner_wall_solid = new G4Tubs("InnerWall", inner_ir, inner_or, 0.5*inner_h, 0, twopi);
  G4LogicalVolume* inner_wall_log = new G4LogicalVolume(inner_wall_solid, peek, "InnerWallLog");
  inner_wall_log->SetVisAttributes(G4Color::Green());
  G4ThreeVector inner_wall_pos(0, 0, -0.5 * inner_h + offsetZ);
  new G4PVPlacement(nullptr, global_coordinates + inner_wall_pos, inner_wall_log, "InnerWall", World_Logical, false, 0);

  // Inner container bottom
  G4Tubs* inner_bottom_solid = new G4Tubs("InnerBottom", 0, inner_or, 0.5*inner_bottom_th, 0, twopi);
  G4LogicalVolume* inner_bottom_log = new G4LogicalVolume(inner_bottom_solid, peek, "InnerBottomLog");
  inner_bottom_log->SetVisAttributes(G4Color::Gray());
  G4ThreeVector inner_bottom_pos(0, 0, 0.5 * inner_bottom_th + offsetZ);
  new G4PVPlacement(nullptr, global_coordinates + inner_bottom_pos, inner_bottom_log, "InnerBottom", World_Logical, false, 0);

  // Inner container lid
  G4Tubs* inner_lid_solid = new G4Tubs("InnerLid", 0, inner_ir, 0.5*inner_lid_th, 0, twopi);
  G4LogicalVolume* inner_lid_log = new G4LogicalVolume(inner_lid_solid, peek, "InnerLidLog");
  inner_lid_log->SetVisAttributes(G4Color::Gray());
  G4ThreeVector inner_lid_pos(0, 0, -puo2_th - 0.5 * inner_lid_th + offsetZ);
  new G4PVPlacement(nullptr, global_coordinates + inner_lid_pos, inner_lid_log, "InnerLid", World_Logical, false, 0);

  /*******************************************************************************************/

  // PuO2 target inside inner container
  G4Tubs* puo2_solid = new G4Tubs("PuO2Solid", 0, inner_ir, puo2_th*0.5, 0, twopi);
  G4LogicalVolume* puo2_log = new G4LogicalVolume(puo2_solid, PuO2, "PuO2Log");
  puo2_log->SetVisAttributes(G4Color::Magenta());
  G4ThreeVector puo2_pos(0, 0, -0.5 * puo2_th + offsetZ);
  new G4PVPlacement(nullptr, global_coordinates + puo2_pos, puo2_log, "PuO2", World_Logical, false, 0);


  // Al27 disk near bottom of outer container
  G4Tubs* al_solid = new G4Tubs("AlSolid", 0, al_radius, al_thickness5*0.5, 0, twopi);
  G4LogicalVolume* al_log = new G4LogicalVolume(al_solid, matAl, "AlLog");
  al_log->SetVisAttributes(G4Color::Cyan());
  G4ThreeVector al_pos(0, 0, outer_wall_pos.z() - 0.5 * outer_h - outer_lid_th - 0.5 * al_thickness5);
  new G4PVPlacement(nullptr, global_coordinates + al_pos, al_log, "Al", World_Logical, false, 0);


}