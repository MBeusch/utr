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

#include "Pu242_outerBox.hh"

using std::stringstream;
using std::vector;

Pu242_outerBox::Pu242_outerBox(G4LogicalVolume *World_Log) : World_Logical(World_Log) {}

void Pu242_outerBox::Construct(G4ThreeVector global_coordinates) {

  // Global offset
  G4double offsetZ = 1.5*mm; // offset_targetCenter_containerBottom

  // Steel pipe dimensios
  G4double steelpipe_ir = 0.5 * 66 * mm;
  G4double steelpipe_or = 0.5 * 70 * mm;
  G4double steelpipe_length = 30 * cm;

  // Rain pipe dimensios
  G4double rainpipe_ir = 0.5 * 54 * mm;
  G4double rainpipe_or = 0.5 * 63 * mm;
  G4double rainpipe_length = 10 * cm;

  // Al target dimensions
  G4double al_radius = 0.5 * cm;
  G4double al_mass1 = 0.1176 * g;
  G4double al_mass5 = 0.4735 * g;
  G4double al_density = 2.699 * g/cm3;
  G4double al_thickness1 = al_mass1 / (pi * al_radius * al_radius * al_density);
  G4double al_thickness5 = al_mass5 / (pi * al_radius * al_radius * al_density);
  
  // Inner container dimensions (PuO2 container)
  G4double inner_ir = 0.5 * 12 * mm;
  G4double inner_or = 0.5 * 16 * mm;
  G4double inner_h = 13 * mm;
  G4double inner_bottom_th = 1 * mm; // 3 * mm;
  G4double inner_lid_th = 6 * mm;
  G4double puo2_th = 6 * mm;      // GUESSTIMATED!!!

  // Outer container dimensions
  G4double outer_ir = 0.5 * 43.5 * mm; // inner radius
  G4double outer_or = 0.5 * 63.5 * mm; // outer radius
  G4double outer_h = 16 * mm;
  G4double outer_bottom_th = 2 * mm;
  G4double outer_lid_th = 4 * mm;
  G4double ring_ir = 0.5 * 16.5 * mm; //Attached to bottom to imitate hole 
  G4double ring_or = outer_or;
  G4double ring_th = 2 * mm;


  /* MATERIALS ******************************************************************************************/

  G4NistManager *nist = G4NistManager::Instance();

  // Define elements
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

  // Rain pipe
  // Define UPVC material
  G4double density = 1.40 * g/cm3; // Average for uPVC
  G4Material* upvc = new G4Material("uPVC", density, 3);
  upvc->AddElement(elC, 2);
  upvc->AddElement(elH, 3);
  upvc->AddElement(elCl, 1);

  /* POSITIONS ******************************************************************************************/

  // PuO2 target inside inner container
  G4ThreeVector puo2_pos(0, 0,  0.5 * puo2_th - offsetZ);

  // Inner container bottom
  G4ThreeVector inner_bottom_pos(0, 0, puo2_pos.z() - 0.5*puo2_th - 0.5*inner_bottom_th);

  // Inner container lid
  G4ThreeVector inner_lid_pos(0, 0, puo2_pos.z() + 0.5*puo2_th + 0.5*inner_lid_th);

  // Inner container wall
  G4ThreeVector inner_wall_pos(0, 0, inner_bottom_pos.z() + 0.5*inner_bottom_th + 0.5*inner_h);


  /* GEOMETRY ******************************************************************************************/

  // Ring attached to bottom 
  G4Tubs* ring_solid = new G4Tubs("RingBottom", ring_ir, ring_or, 0.5*ring_th, 0, twopi);
  G4LogicalVolume* ring_log = new G4LogicalVolume(ring_solid, peek, "RingBottomLog");
  ring_log->SetVisAttributes(G4Color::White());
  G4ThreeVector ring_pos(0, 0, inner_bottom_pos.z() - 0.5*inner_bottom_th - 0.5*ring_th);
  new G4PVPlacement(nullptr, global_coordinates + ring_pos, ring_log, "RingBottom", World_Logical, false, 0);
  
  // Outer container bottom
  G4Tubs* outer_bottom_solid = new G4Tubs("OuterBottom", 0, outer_or, 0.5*outer_bottom_th, 0, twopi);
  G4LogicalVolume* outer_bottom_log = new G4LogicalVolume(outer_bottom_solid, peek, "OuterBottomLog");
  outer_bottom_log->SetVisAttributes(G4Color::White());
  G4ThreeVector outer_bottom_pos(0, 0, ring_pos.z() - 0.5*ring_th - 0.5*outer_bottom_th);
  new G4PVPlacement(nullptr, global_coordinates + outer_bottom_pos, outer_bottom_log, "OuterBottom", World_Logical, false, 0);

  // Outer container wall
  G4Tubs* outer_wall_solid = new G4Tubs("OuterWall", outer_ir, outer_or, 0.5*outer_h, 0, twopi);
  G4LogicalVolume* outer_wall_log = new G4LogicalVolume(outer_wall_solid, peek, "OuterWallLog");
  outer_wall_log->SetVisAttributes(G4Color::White());
  G4ThreeVector outer_wall_pos(0, 0, outer_bottom_pos.z() + 0.5*outer_bottom_th + 0.5*outer_h);
  new G4PVPlacement(nullptr, global_coordinates + outer_wall_pos, outer_wall_log, "OuterWall", World_Logical, false, 0);

  // Outer Container lid
  G4Tubs *outer_lid_solid = new G4Tubs("OuterLid", 0, outer_or, 0.5 * outer_lid_th, 0, twopi);
  G4LogicalVolume *outer_lid_log = new G4LogicalVolume(outer_lid_solid, peek, "OuterLidLog");
  outer_lid_log->SetVisAttributes(G4Color::White());
  G4ThreeVector outer_lid_pos(0, 0, outer_wall_pos.z() + 0.5 * outer_h + 0.5 * outer_lid_th);
  new G4PVPlacement(nullptr, global_coordinates + outer_lid_pos, outer_lid_log, "OuterLid", World_Logical, false, 0);

  // Steel pipe wall
  G4Tubs* steelpipe_wall_solid = new G4Tubs("PipeWall", steelpipe_ir, steelpipe_or, 0.5*steelpipe_length, 0, twopi);
  G4LogicalVolume* steelpipe_wall_log = new G4LogicalVolume(steelpipe_wall_solid, stainless316L, "PipeWallLog");
  steelpipe_wall_log->SetVisAttributes(G4Color::Gray());
  G4ThreeVector steelpipe_wall_pos(0, 0, 0);
  new G4PVPlacement(nullptr, global_coordinates + steelpipe_wall_pos, steelpipe_wall_log, "PipeWall", World_Logical, false, 0);

  // Rain pipe wall
  G4Tubs* rainpipe_wall_solid = new G4Tubs("RainpipeWall", rainpipe_ir, rainpipe_or, 0.5*rainpipe_length, 0, twopi);
  G4LogicalVolume* rainpipe_wall_log = new G4LogicalVolume(rainpipe_wall_solid, upvc, "RainpipeWallLog");
  rainpipe_wall_log->SetVisAttributes(G4Color::Yellow());
  G4ThreeVector rainpipe_wall_pos(0, 0, outer_bottom_pos.z() - 0.5* outer_bottom_th - 0.5*rainpipe_length);
  new G4PVPlacement(nullptr, global_coordinates + rainpipe_wall_pos, rainpipe_wall_log, "RainpipeWall", World_Logical, false, 0);


}