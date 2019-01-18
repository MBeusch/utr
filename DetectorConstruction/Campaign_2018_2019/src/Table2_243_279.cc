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

#include "G4Box.hh"
#include "G4NistManager.hh"
#include "G4PVPlacement.hh"
#include "G4VisAttributes.hh"
#include "G4SubtractionSolid.hh"
#include "G4Tubs.hh"
#include "G4PhysicalConstants.hh"
#include "G4SystemOfUnits.hh"
#include "G4TwoVector.hh"
#include "G4ExtrudedSolid.hh"

#include "Units.hh"
#include "Table2_243_279.hh"

Table2_243_279::Table2_243_279(G4LogicalVolume *World_Log):
World_Logical(World_Log),
Table2_Length(38.*inch),
Z_Axis_Offset_Z(-2.25*inch) // This is how far the beam tube holder extends into the G3 table 
{}

void Table2_243_279::Construct(G4ThreeVector global_coordinates){
	
	G4Colour green(0., 0.5, 0.);
	G4Colour grey(0.5, 0.5, 0.5);
	G4Colour white(1.0, 1.0, 1.0);
	G4Colour light_orange(1.0, 0.82, 0.36);

	G4NistManager *nist = G4NistManager::Instance();

	G4Material *Pb = nist->FindOrBuildMaterial("G4_Pb");
	G4Material *Al = nist->FindOrBuildMaterial("G4_Al");
	G4Material *Fe = nist->FindOrBuildMaterial("G4_Fe");
	G4Material *Plexiglass = nist->FindOrBuildMaterial("G4_PLEXIGLASS");
	G4Material *one_third_density_brass = nist->FindOrBuildMaterial("one_third_density_brass");

	G4double Table_Plate_Radius = 17.*inch;
	G4double Table_Plate_Hole_Radius = 9.*inch;

	// Upstream beam pipe holder
	
	G4double Holder_Base_To_Table = 3.*inch; // Estimated
	
	G4double Upstream_Holder_Ring_Outer_Radius = 3.*inch;
	G4double Upstream_Holder_Ring_Inner_Radius = Upstream_Holder_Ring_Outer_Radius - 0.375*inch;
	G4double Upstream_Holder_Ring_Length = 6.5*inch;
	
	G4double Upstream_Holder_Base_X = 6.*inch;
	G4double Upstream_Holder_Base_Y = 1.5*inch;
	G4double Upstream_Holder_Base_Z = 4.5*inch;
	G4double Upstream_Holder_Hole_Depth = 0.5*inch;

	G4Box *Upstream_Holder_Base_Solid_Solid = new G4Box("Upstream_Holder_Base_Solid_Solid", Upstream_Holder_Base_X*0.5, Upstream_Holder_Base_Y*0.5, Upstream_Holder_Base_Z*0.5);
	G4Tubs *Upstream_Holder_Base_Hole_Solid = new G4Tubs("Upstream_Hold_Base_Hole_Solid", 0., Upstream_Holder_Ring_Outer_Radius, Upstream_Holder_Base_Z*0.5, 0., twopi);
	G4SubtractionSolid *Upstream_Holder_Base_Solid = new G4SubtractionSolid("Upstream_Holder_Base_Solid", Upstream_Holder_Base_Solid_Solid, Upstream_Holder_Base_Hole_Solid, 0, G4ThreeVector(0., Upstream_Holder_Base_Y*0.5 + Upstream_Holder_Ring_Outer_Radius - Upstream_Holder_Hole_Depth));

	G4LogicalVolume *Upstream_Holder_Base_Logical = new G4LogicalVolume(Upstream_Holder_Base_Solid, Al, "Upstream_Holder_Base_Logical");
	Upstream_Holder_Base_Logical->SetVisAttributes(grey);

	new G4PVPlacement(0, global_coordinates + G4ThreeVector(0., -Upstream_Holder_Ring_Outer_Radius +Upstream_Holder_Hole_Depth - Upstream_Holder_Base_Y*0.5, -Table_Plate_Radius + Upstream_Holder_Base_Z*0.5), Upstream_Holder_Base_Logical, "Upstream_Holder_Base", World_Logical, false, 0, false);

	G4Tubs *Upstream_Holder_Ring_Solid = new G4Tubs("Upstream_Holder_Ring_Solid", Upstream_Holder_Ring_Inner_Radius, Upstream_Holder_Ring_Outer_Radius, Upstream_Holder_Ring_Length*0.5, 0., twopi);

	G4LogicalVolume *Upstream_Holder_Ring_Logical = new G4LogicalVolume(Upstream_Holder_Ring_Solid, Al, "Upstream_Holder_Ring_Logical");
	Upstream_Holder_Ring_Logical->SetVisAttributes(grey);

	new G4PVPlacement(0, global_coordinates + G4ThreeVector(0., 0., -Table2_Length*0.5 + Upstream_Holder_Ring_Length*0.5), Upstream_Holder_Ring_Logical, "Upstream_Holder_Ring", World_Logical, false, 0, false);

	// Vaccum valve inside upstream beam pipe holder
	
	G4double Valve_Outer_Radius = 2.5*inch; // Estimated
	G4double Valve_Thickness = 1.5*inch; // Estimated
	G4double Valve_Window_Thickness = 2.*mm; // Estimated

	G4Tubs* Valve_Solid = new G4Tubs("Valve_Solid", 1.*inch, Valve_Outer_Radius, Valve_Thickness*0.5, 0., twopi);

	G4LogicalVolume *Valve_Logical = new G4LogicalVolume(Valve_Solid, Plexiglass, "Valve_Logical");
	Valve_Logical->SetVisAttributes(white);

	new G4PVPlacement(0, global_coordinates + G4ThreeVector(0., 0., -Table2_Length*0.5 + Upstream_Holder_Ring_Length*0.5), Valve_Logical, "Valve", World_Logical, false, 0, false); // Position estimated

	G4Tubs *Valve_Window_Solid = new G4Tubs("Valve_Window_Solid", 0., 1.*inch, Valve_Window_Thickness*0.5, 0., twopi);

	G4LogicalVolume *Valve_Window_Logical = new G4LogicalVolume(Valve_Window_Solid, Plexiglass, "Valve_Window_Logical");
	Valve_Window_Logical->SetVisAttributes(white);

	new G4PVPlacement(0, global_coordinates + G4ThreeVector(0., 0., -Table2_Length*0.5 + Upstream_Holder_Ring_Length*0.5 - Valve_Thickness*0.5 + Valve_Window_Thickness*0.5), Valve_Window_Logical, "Valve_Window", World_Logical, false, 0, false); // Position estimated
	
	// Lead shielding inside of upstream beam pipe holder
	
	G4double Lead_Brick_X = 4.*inch;
	G4double Lead_Brick_Y = 4.*inch;
	G4double Lead_Brick_Z = 2.*inch;
	G4double Lead_Brick_Hole_Radius = 1.*inch;

	G4Box *Lead_Brick_Solid_Solid = new G4Box("Lead_Brick_Solid_Solid", Lead_Brick_X*0.5, Lead_Brick_Y*0.5, Lead_Brick_Z*0.5);
	G4Tubs *Lead_Brick_Hole_Solid = new G4Tubs("Lead_Brick_Hole_Solid", 0., Lead_Brick_Hole_Radius, Lead_Brick_Z, 0., twopi);
	
	G4SubtractionSolid *Lead_Brick_Solid = new G4SubtractionSolid("Lead_Brick_Solid", Lead_Brick_Solid_Solid, Lead_Brick_Hole_Solid);

	G4LogicalVolume *Lead_Brick_Logical = new G4LogicalVolume(Lead_Brick_Solid, Pb, "Lead_Brick_Logical");
	Lead_Brick_Logical->SetVisAttributes(green);

	new G4PVPlacement(0, global_coordinates + G4ThreeVector(0., 0., -Table2_Length*0.5 + Lead_Brick_Z*0.5), Lead_Brick_Logical, "Lead_Brick_1", World_Logical, false, 0, false);
	new G4PVPlacement(0, global_coordinates + G4ThreeVector(0., 0., -Table2_Length*0.5 + Upstream_Holder_Ring_Length - Lead_Brick_Z*0.5), Lead_Brick_Logical, "Lead_Brick_2", World_Logical, false, 0, false);

	// Downstream beam pipe holder
	
	G4double Downstream_Holder_Ring_Outer_Radius = 3.*inch;
	G4double Downstream_Holder_Ring_Inner_Radius = Downstream_Holder_Ring_Outer_Radius - 0.375*inch;
	G4double Downstream_Holder_Ring_Length = 9.5*inch;
	
	G4double Downstream_Holder_Base_X = 6.*inch;
	G4double Downstream_Holder_Base_Y = 1.5*inch;
	G4double Downstream_Holder_Base_Z = 5.5*inch;
	G4double Downstream_Holder_Hole_Depth = 0.5*inch;

	G4Box *Downstream_Holder_Base_Solid_Solid = new G4Box("Downstream_Holder_Base_Solid_Solid", Downstream_Holder_Base_X*0.5, Downstream_Holder_Base_Y*0.5, Downstream_Holder_Base_Z*0.5);
	G4Tubs *Downstream_Holder_Base_Hole_Solid = new G4Tubs("Downstream_Hold_Base_Hole_Solid", 0., Downstream_Holder_Ring_Outer_Radius, Downstream_Holder_Base_Z*0.5, 0., twopi);
	G4SubtractionSolid *Downstream_Holder_Base_Solid = new G4SubtractionSolid("Downstream_Holder_Base_Solid", Downstream_Holder_Base_Solid_Solid, Downstream_Holder_Base_Hole_Solid, 0, G4ThreeVector(0., Downstream_Holder_Base_Y*0.5 + Downstream_Holder_Ring_Outer_Radius - Downstream_Holder_Hole_Depth));

	G4LogicalVolume *Downstream_Holder_Base_Logical = new G4LogicalVolume(Downstream_Holder_Base_Solid, Al, "Downstream_Holder_Base_Logical");
	Downstream_Holder_Base_Logical->SetVisAttributes(grey);

	new G4PVPlacement(0, global_coordinates + G4ThreeVector(0., -Downstream_Holder_Ring_Outer_Radius + Downstream_Holder_Hole_Depth - Downstream_Holder_Base_Y*0.5, Table_Plate_Hole_Radius + Downstream_Holder_Base_Z*0.5), Downstream_Holder_Base_Logical, "Downstream_Holder_Base", World_Logical, false, 0, false);

	G4Tubs *Downstream_Holder_Ring_Solid = new G4Tubs("Downstream_Holder_Ring_Solid", Downstream_Holder_Ring_Inner_Radius, Downstream_Holder_Ring_Outer_Radius, Downstream_Holder_Ring_Length*0.5, 0., twopi);

	G4LogicalVolume *Downstream_Holder_Ring_Logical = new G4LogicalVolume(Downstream_Holder_Ring_Solid, Al, "Downstream_Holder_Ring_Logical");
	Downstream_Holder_Ring_Logical->SetVisAttributes(grey);

	new G4PVPlacement(0, global_coordinates + G4ThreeVector(0., 0., Table_Plate_Hole_Radius + Downstream_Holder_Ring_Length*0.5), Downstream_Holder_Ring_Logical, "Downstream_Holder_Ring", World_Logical, false, 0, false);

	G4double Downstream_Plexi_Ring_Radius = 2.5*inch; // Estimated
	G4double Downstream_Plexi_Ring_Thickness = 4.*mm; // Estimated

	G4Tubs *Downstream_Plexi_Ring_Solid = new G4Tubs("Downstream_Plexi_Ring_Solid", 1.*inch, Downstream_Plexi_Ring_Radius, Downstream_Plexi_Ring_Thickness*0.5, 0., twopi);

	G4LogicalVolume *Downstream_Plexi_Ring_Logical = new G4LogicalVolume(Downstream_Plexi_Ring_Solid, Plexiglass, "Downstream_Plexi_Ring_Logical");
	Downstream_Plexi_Ring_Logical->SetVisAttributes(white);

	new G4PVPlacement(0, global_coordinates + G4ThreeVector(0., 0., Table_Plate_Hole_Radius + Downstream_Holder_Ring_Length*0.5), Downstream_Plexi_Ring_Logical, "Downstream_Holder_Ring", World_Logical, false, 0, false);

    // Downstream holder fixture
    // A peculiar, custom-made shape that fixes the beam pipe holder ring to the second table. It was accurately modeled by OP as an ExtrudedSolid. 
    // However, the positioning in the world was done by visual judgment, because it is unclear to us where the zero-coordinate of the ExtrudedSolid is.
    // It is definitely not the zero-point which all the TwoVectors have in common.

    std::vector<G4TwoVector> Downstream_Holder_Fixture_Shape(42);
    Downstream_Holder_Fixture_Shape[0] = G4TwoVector(34.623*mm, 181.469*mm);
    Downstream_Holder_Fixture_Shape[1] = G4TwoVector(13.592*mm, 179.986*mm);
    Downstream_Holder_Fixture_Shape[2] = G4TwoVector(-6.736*mm, 174.895*mm);
    Downstream_Holder_Fixture_Shape[3] = G4TwoVector(-25.935*mm, 166.529*mm);
    Downstream_Holder_Fixture_Shape[4] = G4TwoVector(-43.581*mm, 155.215*mm);
    Downstream_Holder_Fixture_Shape[5] = G4TwoVector(-59.249*mm, 141.285*mm);
    Downstream_Holder_Fixture_Shape[6] = G4TwoVector(-72.515*mm, 125.067*mm);
    Downstream_Holder_Fixture_Shape[7] = G4TwoVector(-82.955*mm, 106.892*mm);
    Downstream_Holder_Fixture_Shape[8] = G4TwoVector(-90.143*mm, 87.090*mm);
    Downstream_Holder_Fixture_Shape[9] = G4TwoVector(-94.255*mm, 63.751*mm);
    Downstream_Holder_Fixture_Shape[10] = G4TwoVector(-95.090*mm, 40.093*mm);
    Downstream_Holder_Fixture_Shape[11] = G4TwoVector(-93.670*mm, -7.401*mm);
    Downstream_Holder_Fixture_Shape[12] = G4TwoVector(-90.432*mm, -24.633*mm);
    Downstream_Holder_Fixture_Shape[13] = G4TwoVector(-83.944*mm, -40.850*mm);
    Downstream_Holder_Fixture_Shape[14] = G4TwoVector(-74.628*mm, -55.727*mm);
    Downstream_Holder_Fixture_Shape[15] = G4TwoVector(-62.908*mm, -68.943*mm);
    Downstream_Holder_Fixture_Shape[16] = G4TwoVector(-33.954*mm, -89.098*mm);
    Downstream_Holder_Fixture_Shape[17] = G4TwoVector(-17.567*mm, -95.390*mm);
    Downstream_Holder_Fixture_Shape[18] = G4TwoVector(-0.472*mm, -98.727*mm);
    Downstream_Holder_Fixture_Shape[19] = G4TwoVector(34.911*mm, -105.920*mm);
    Downstream_Holder_Fixture_Shape[20] = G4TwoVector(67.087*mm, -119.718*mm);
    Downstream_Holder_Fixture_Shape[21] = G4TwoVector(94.708*mm, -140.327*mm);
    Downstream_Holder_Fixture_Shape[22] = G4TwoVector(116.429*mm, -167.948*mm);
    Downstream_Holder_Fixture_Shape[23] = G4TwoVector(126.792*mm, -171.065*mm);
    Downstream_Holder_Fixture_Shape[24] = G4TwoVector(134.164*mm, -176.535*mm);
    Downstream_Holder_Fixture_Shape[25] = G4TwoVector(140.233*mm, -189.220*mm);
    Downstream_Holder_Fixture_Shape[26] = G4TwoVector(140.233*mm, -699.220*mm);
    Downstream_Holder_Fixture_Shape[27] = G4TwoVector(241.833*mm, -699.220*mm);
    Downstream_Holder_Fixture_Shape[28] = G4TwoVector(241.833*mm, -189.220*mm);
    Downstream_Holder_Fixture_Shape[29] = G4TwoVector(241.861*mm, -127.860*mm);
    Downstream_Holder_Fixture_Shape[30] = G4TwoVector(240.894*mm, -82.838*mm);
    Downstream_Holder_Fixture_Shape[31] = G4TwoVector(238.383*mm, -56.414*mm);
    Downstream_Holder_Fixture_Shape[32] = G4TwoVector(226.089*mm, -44.854*mm);
    Downstream_Holder_Fixture_Shape[33] = G4TwoVector(222.797*mm, -36.936*mm);
    Downstream_Holder_Fixture_Shape[34] = G4TwoVector(222.050*mm, -27.851*mm);
    Downstream_Holder_Fixture_Shape[35] = G4TwoVector(222.472*mm, 10.355*mm);
    Downstream_Holder_Fixture_Shape[36] = G4TwoVector(209.684*mm, 40.062*mm);
    Downstream_Holder_Fixture_Shape[37] = G4TwoVector(195.141*mm, 66.242*mm);
    Downstream_Holder_Fixture_Shape[38] = G4TwoVector(159.940*mm, 113.108*mm);
    Downstream_Holder_Fixture_Shape[39] = G4TwoVector(130.121*mm, 141.502*mm);
    Downstream_Holder_Fixture_Shape[40] = G4TwoVector(96.590*mm, 165.407*mm);
    Downstream_Holder_Fixture_Shape[41] = G4TwoVector(66.622*mm, 177.358*mm);
    G4double Downstream_Holder_Fixture_Thickness = 13.15*mm;
    G4double Downstream_Holder_Fixture_Hole_Radius = 78.*mm;

    G4ExtrudedSolid *Downstream_Holder_Fixture_Solid_Solid = new G4ExtrudedSolid(
            "Downstream_Holder_Fixture_Solid_Solid",
            Downstream_Holder_Fixture_Shape,
            Downstream_Holder_Fixture_Thickness/2,
            G4TwoVector(0, 0), 1.0, G4TwoVector(0, 0), 1.0);
    
	G4Tubs *Downstream_Holder_Fixture_Hole_Solid = new G4Tubs(
            "Downstream_Holder_Fixture_Hole_Solid", 0.,
            Downstream_Holder_Fixture_Hole_Radius,
            Downstream_Holder_Fixture_Thickness, 0., twopi);
	
    G4SubtractionSolid *Downstream_Holder_Fixture_Solid = new G4SubtractionSolid(
            "Downstream_Holder_Fixture_Solid",
            Downstream_Holder_Fixture_Solid_Solid,
            Downstream_Holder_Fixture_Hole_Solid,
            0, G4ThreeVector(30., 40., 0.));
	
    G4LogicalVolume *Downstream_Holder_Fixture_Logical = new G4LogicalVolume(Downstream_Holder_Fixture_Solid, Al, "Downstream_Holder_Fixture_Logical");
	Downstream_Holder_Fixture_Logical->SetVisAttributes(grey);

    new G4PVPlacement(0, global_coordinates + G4ThreeVector(-30.*mm, -40.*mm, 430.*mm), Downstream_Holder_Fixture_Logical, "Downstream_Holder_Fixture", World_Logical, false, 0, false);

	// Holding structure for vertical detectors
	
	G4double Brass_Column_Height = 24.*inch;
	G4double Brass_Column_Base = 1.5*inch;

	G4double Al_Bar_Thickness = 0.5*inch;
	G4double Al_Bar_X = 27.5*inch;
	G4double Al_Bar_Y = 4.*inch;

	G4Box *Brass_Column_Solid = new G4Box("Brass_Column_Solid", Brass_Column_Base*0.5, Brass_Column_Height*0.5, Brass_Column_Base*0.5);

	G4LogicalVolume* Brass_Column_Logical = new G4LogicalVolume(Brass_Column_Solid, one_third_density_brass, "Brass_Column_Solid");
	Brass_Column_Logical->SetVisAttributes(light_orange);
	
	// Table plate
	// The table plate is divided into two parts with different inner radii and thicknesses
	G4double Table_Plate_Thickness = 0.5*inch; // Estimated
	G4double Table_Thin_Plate_Thickness = 0.25*inch; // Estimated
	
	G4double Table_Thin_Plate_Hole_Radius = 12.5*inch;

	G4Tubs *Table_Plate_Solid_Solid = new G4Tubs("Table_Plate_Solid_Solid", Table_Plate_Hole_Radius, Table_Plate_Radius, Table_Plate_Thickness*0.5, (90. + 46.05)*deg, (360. - 2.*46.05)*deg); // Angles estimated
	G4Box *Table_Plate_Hole = new G4Box("Table_Plate_Hole", Table_Plate_Hole_Radius, 8.6747*inch*0.5, Table_Plate_Thickness);

	G4SubtractionSolid *Table_Plate_Solid = new G4SubtractionSolid("Table_Plate_Solid", Table_Plate_Solid_Solid, Table_Plate_Hole, 0, G4ThreeVector(0., 8.6747*inch*0.5, 0.)); // Cut out a rectangular piece to fit both plates into each other

	G4LogicalVolume *Table_Plate_Logical = new G4LogicalVolume(Table_Plate_Solid, Al, "Table_Plate_Logical");
	Table_Plate_Logical->SetVisAttributes(grey);

	G4RotationMatrix *rot_Table = new G4RotationMatrix();
	rot_Table->rotateX(90.*deg);
	new G4PVPlacement(rot_Table, global_coordinates + G4ThreeVector(0., -Upstream_Holder_Ring_Outer_Radius + Upstream_Holder_Hole_Depth - Upstream_Holder_Base_Y - Holder_Base_To_Table - Table_Plate_Thickness*0.5, 0.), Table_Plate_Logical, "Table_Plate", World_Logical, false, 0, false);

	G4Tubs *Table_Thin_Plate_Solid = new G4Tubs("Table_Thin_Plate_Solid", Table_Thin_Plate_Hole_Radius, Table_Plate_Radius, Table_Thin_Plate_Thickness*0.5, (90.-46.05)*deg, 2.*46.05*deg); // Angles estimated

	G4LogicalVolume *Table_Thin_Plate_Logical = new G4LogicalVolume(Table_Thin_Plate_Solid, Al, "Table_Plate_Logical");
	Table_Thin_Plate_Logical->SetVisAttributes(grey);

	new G4PVPlacement(rot_Table, global_coordinates + G4ThreeVector(0., -Upstream_Holder_Ring_Outer_Radius + Upstream_Holder_Hole_Depth - Upstream_Holder_Base_Y - Holder_Base_To_Table -Table_Plate_Thickness*0.5 + Table_Thin_Plate_Thickness*0.5, 0.), Table_Thin_Plate_Logical, "Table_Plate", World_Logical, false, 0, false);

	// Construct holding structure on top of the table

	new G4PVPlacement(0, global_coordinates + G4ThreeVector(Table_Plate_Hole_Radius + 2.5*inch + Brass_Column_Base*0.5, -Upstream_Holder_Ring_Outer_Radius + Upstream_Holder_Hole_Depth - Upstream_Holder_Base_Y - Holder_Base_To_Table + Brass_Column_Height*0.5, -3.75*inch - Brass_Column_Base*0.5), Brass_Column_Logical, "Brass_Column_1", World_Logical, false, 0, false);
	new G4PVPlacement(0, global_coordinates + G4ThreeVector(-Table_Plate_Hole_Radius - 2.5*inch - Brass_Column_Base*0.5,-Upstream_Holder_Ring_Outer_Radius + Upstream_Holder_Hole_Depth - Upstream_Holder_Base_Y - Holder_Base_To_Table + Brass_Column_Height*0.5 , -3.75*inch - Brass_Column_Base*0.5), Brass_Column_Logical, "Brass_Column_2", World_Logical, false, 0, false);

	G4Box *Al_Bar_Solid = new G4Box("Al_Bar_Solid", Al_Bar_X*0.5, Al_Bar_Y*0.5, Al_Bar_Thickness*0.5);

	G4LogicalVolume *Al_Bar_Logical = new G4LogicalVolume(Al_Bar_Solid, Al, "Al_Bar_Logical");
	Al_Bar_Logical->SetVisAttributes(grey);

	new G4PVPlacement(0, global_coordinates + G4ThreeVector(0.,-Upstream_Holder_Ring_Outer_Radius + Upstream_Holder_Hole_Depth - Upstream_Holder_Base_Y - Holder_Base_To_Table + 18.*inch,  -3.75*inch - Brass_Column_Base - Al_Bar_Thickness*0.5), Al_Bar_Logical, "Al_Bar_1", World_Logical, false, 0, false); // Estimated vertical position
	
	// Construct holding structure below table

	new G4PVPlacement(0, global_coordinates + G4ThreeVector(Table_Plate_Hole_Radius + 2.5*inch + Brass_Column_Base*0.5,-Upstream_Holder_Ring_Outer_Radius + Upstream_Holder_Hole_Depth - Upstream_Holder_Base_Y - Holder_Base_To_Table -Table_Plate_Thickness - Brass_Column_Height*0.5, -3.75*inch - Brass_Column_Base*0.5), Brass_Column_Logical, "Brass_Column_1", World_Logical, false, 0, false);
	new G4PVPlacement(0, global_coordinates + G4ThreeVector(-Table_Plate_Hole_Radius - 2.5*inch - Brass_Column_Base*0.5,-Upstream_Holder_Ring_Outer_Radius + Upstream_Holder_Hole_Depth - Upstream_Holder_Base_Y - Holder_Base_To_Table -Table_Plate_Thickness - Brass_Column_Height*0.5, -3.75*inch - Brass_Column_Base*0.5), Brass_Column_Logical, "Brass_Column_2", World_Logical, false, 0, false); // Estimated length. Actually measured only the length of the holding structure on top

	new G4PVPlacement(0, global_coordinates + G4ThreeVector(0., -Upstream_Holder_Ring_Outer_Radius + Upstream_Holder_Hole_Depth - Upstream_Holder_Base_Y - Holder_Base_To_Table -Table_Plate_Thickness - 18.*inch,  -3.75*inch - Brass_Column_Base - Al_Bar_Thickness*0.5), Al_Bar_Logical, "Al_Bar_2", World_Logical, false, 0, false); // Estimated vertical position

	// Lead shielding after first beam pipe holder
	
	G4Box *Lead_On_Table_1_Solid_Solid = new G4Box("Lead_On_Table_1_Solid_Solid", 12.*inch*0.5, 8.*inch*0.5, 2.*inch*0.5);
	G4Tubs *Lead_On_Table_1_Hole_Solid = new G4Tubs("Lead_On_Table_1_Hole_Solid", 0., 1.*inch, 2.*inch, 0., twopi);

	G4SubtractionSolid *Lead_On_Table_1_Solid = new G4SubtractionSolid("Lead_On_Table_1_Solid", Lead_On_Table_1_Solid_Solid, Lead_On_Table_1_Hole_Solid, 0, G4ThreeVector(0., 2.*inch, 0.));

	G4LogicalVolume* Lead_On_Table_1_Logical = new G4LogicalVolume(Lead_On_Table_1_Solid, Pb, "Lead_On_Table_1_Logical");
	Lead_On_Table_1_Logical->SetVisAttributes(green);

	new G4PVPlacement(0, global_coordinates + G4ThreeVector(0., -2.*inch, -Table2_Length*0.5 + Upstream_Holder_Ring_Length + 1.*inch), Lead_On_Table_1_Logical, "Lead_On_Table_1", World_Logical, false, 0, false);

	G4Box *Lead_On_Table_2_Solid = new G4Box("Lead_On_Table_1_Solid_Solid", 8.*inch*0.5, 4.*inch*0.5, 2.*inch*0.5);

	G4LogicalVolume* Lead_On_Table_2_Logical = new G4LogicalVolume(Lead_On_Table_2_Solid, Pb, "Lead_On_Table_2_Logical");
	Lead_On_Table_2_Logical->SetVisAttributes(green);

	new G4PVPlacement(0, global_coordinates + G4ThreeVector(0., 4.*inch, -Table2_Length*0.5 + Upstream_Holder_Ring_Length + 1.*inch), Lead_On_Table_2_Logical, "Lead_On_Table_2", World_Logical, false, 0, false);

	G4Box *Al_Bar_On_Table_Solid = new G4Box("Al_Bar_On_Table_Solid", 10.*inch*0.5, 1.*inch*0.5, 2.*inch*0.5);

	G4LogicalVolume* Al_Bar_On_Table_Logical = new G4LogicalVolume(Al_Bar_On_Table_Solid, Al, "Al_Bar_On_Table_Logical");

	new G4PVPlacement(0, global_coordinates + G4ThreeVector(0., -6.5*inch, -Table2_Length*0.5 + Upstream_Holder_Ring_Length + 1.*inch), Al_Bar_On_Table_Logical, "Al_Bar_On_Table", World_Logical, false, 0, false);

	

	// Additional lead shielding on table, inside a U-shaped iron bar
	
	G4double Iron_Bar_X = 22.*inch; // Estimated (I think I measured 56 cm)
	G4double Iron_Bar_Y = 2.*inch; // Estimated
	G4double Iron_Bar_Z = 4.*inch; // Estimated
	G4double Iron_Bar_Wall_Thickness = 0.2*inch; // Estimated

	G4Box *Iron_Bar_Solid_Solid = new G4Box("Iron_Bar_Solid_Solid", Iron_Bar_X*0.5, Iron_Bar_Y*0.5, Iron_Bar_Z*0.5);
	G4Box *Iron_Bar_Hole_Solid = new G4Box("Iron_Bar_Hole_Solid", Iron_Bar_X, Iron_Bar_Y*0.5, (Iron_Bar_Z - 2.*Iron_Bar_Wall_Thickness)*0.5);

	G4SubtractionSolid *Iron_Bar_Solid = new G4SubtractionSolid("Iron_Bar_Solid", Iron_Bar_Solid_Solid, Iron_Bar_Hole_Solid, 0, G4ThreeVector(0., Iron_Bar_Wall_Thickness, 0.));

	G4LogicalVolume *Iron_Bar_Logical = new G4LogicalVolume(Iron_Bar_Solid, Fe, "Iron_Bar_Logical");

	new G4PVPlacement(0, global_coordinates + G4ThreeVector(0., -Upstream_Holder_Ring_Outer_Radius + Upstream_Holder_Hole_Depth - Upstream_Holder_Base_Y - Holder_Base_To_Table + Iron_Bar_Y*0.5, -4.5*inch - Iron_Bar_Z*0.5), Iron_Bar_Logical, "Iron_Bar", World_Logical, false, 0, false);

	// Lead shielding inside iron bar
	G4double Table_Plate_Offset_Y = -Upstream_Holder_Ring_Outer_Radius + Upstream_Holder_Hole_Depth - Upstream_Holder_Base_Y - Holder_Base_To_Table;

	G4Box *Lead_On_Bar_Solid = new G4Box("Lead_On_Bar_Solid", 16.*inch*0.5, 5.*inch*0.5, 3.*inch*0.5);

	G4LogicalVolume *Lead_On_Bar_Logical = new G4LogicalVolume(Lead_On_Bar_Solid, Pb, "Lead_On_Bar_Logical");
	Lead_On_Bar_Logical->SetVisAttributes(green);

	new G4PVPlacement(0, global_coordinates + G4ThreeVector(0., Table_Plate_Offset_Y + Iron_Bar_Wall_Thickness + 5.*inch*0.5, -4.5*inch - Iron_Bar_Z*0.5), Lead_On_Bar_Logical, "Lead_On_Bar", World_Logical, false, 0, false);

	G4Box *Lead_On_Bar_2_Solid_Solid = new G4Box("Lead_On_Bar_2_Solid_Solid", 10.*inch*0.5, fabs(Table_Plate_Offset_Y) - Iron_Bar_Wall_Thickness - 5.*inch, 2.*inch*0.5);
	G4Tubs *Lead_On_Bar_2_Hole_Solid = new G4Tubs("Lead_On_Bar_2_Hole_Solid", 0., 1.*inch, 2.*inch, 0., twopi);

	G4SubtractionSolid *Lead_On_Bar_2_Solid = new G4SubtractionSolid("Lead_On_Bar_2_Solid", Lead_On_Bar_2_Solid_Solid, Lead_On_Bar_2_Hole_Solid, 0, G4ThreeVector(-2.*inch, 0., 0.));

	G4LogicalVolume* Lead_On_Bar_2_Logical = new G4LogicalVolume(Lead_On_Bar_2_Solid, Pb, "Lead_On_Bar_2_Logical");
	Lead_On_Bar_2_Logical->SetVisAttributes(green);

	new G4PVPlacement(0, global_coordinates + G4ThreeVector(2.*inch, 0., -4.5*inch - Iron_Bar_Z*0.5), Lead_On_Bar_2_Logical, "Lead_On_Bar_2", World_Logical, false, 0, false);

	G4Box *Lead_On_Bar_3_Solid = new G4Box("Lead_On_Bar_3_Solid", 4.*inch*0.5, 4.*inch*0.5, 2.*inch*0.5);

	G4LogicalVolume *Lead_On_Bar_3_Logical = new G4LogicalVolume(Lead_On_Bar_3_Solid, Pb, "Lead_On_Bar_3_Logical");
	Lead_On_Bar_3_Logical->SetVisAttributes(green);

	new G4PVPlacement(0, global_coordinates + G4ThreeVector(0.,fabs(Table_Plate_Offset_Y) - Iron_Bar_Wall_Thickness - 5.*inch + 4.*inch*0.5, -4.5*inch - Iron_Bar_Z*0.5), Lead_On_Bar_3_Logical, "Lead_On_Bar_3", World_Logical, false, 0, false);
	
	// Lead wrap around beam pipe
	
	G4Tubs *Lead_Wrap_Solid = new G4Tubs("Lead_Wrap_Solid", 1.*inch, 1.*inch + 3.*mm, 6.*inch*0.5, 0., twopi);

	G4LogicalVolume *Lead_Wrap_Logical = new G4LogicalVolume(Lead_Wrap_Solid, Pb, "Lead_Wrap_Logical");
	Lead_Wrap_Logical->SetVisAttributes(green);

	new G4PVPlacement(0, global_coordinates + G4ThreeVector(0., 0., Table_Plate_Hole_Radius - 6.*inch*0.5), Lead_Wrap_Logical, "Lead_Wrap", World_Logical, false, 0, false);
}
