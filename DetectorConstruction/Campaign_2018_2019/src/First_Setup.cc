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

#include "Units.hh"
#include "First_Setup.hh"

First_Setup::First_Setup(G4LogicalVolume *World_Log):
World_Logical(World_Log),
First_Setup_Length(41.5*inch),
First_Setup_X(44.*inch),
First_Setup_Y(50.*inch) // Arbitrary
{}

void First_Setup::Construct(G4ThreeVector global_coordinates){

	G4Colour grey(0.5, 0.5, 0.5);
	G4Colour green(0., 0.5, 0.);
	G4Colour white(1.0, 1.0, 1.0);

	G4NistManager *nist = G4NistManager::Instance();
	G4Material *Al = nist->FindOrBuildMaterial("G4_Al");
	G4Material *Pb = nist->FindOrBuildMaterial("G4_Pb");
	G4Material *concrete = nist->FindOrBuildMaterial("G4_CONCRETE");
	G4Material *one_third_density_Al = nist->FindOrBuildMaterial("one_third_density_Al");

	// Lead and concrete wall
	
	G4double Lead_Tunnel_X = 16.*inch;
	G4double Lead_Tunnel_Y = 4.*inch;
	G4double Lead_Tunnel_Z = 8.*inch;
	G4double Beam_Pipe_Outer_Radius = 1.*inch;

	G4Box *Lead_Tunnel_Solid_Solid = new G4Box("Lead_Tunnel_Solid", Lead_Tunnel_X*0.5, Lead_Tunnel_Y*0.5, Lead_Tunnel_Z*0.5);

	G4Tubs *Lead_Tunnel_Hole_Solid = new G4Tubs("Lead_Tunnel_Hole_Solid", 0., Beam_Pipe_Outer_Radius, Lead_Tunnel_Z, 0., twopi);

	G4SubtractionSolid* Lead_Tunnel_Solid = new G4SubtractionSolid("Lead_Tunnel_Solid", Lead_Tunnel_Solid_Solid, Lead_Tunnel_Hole_Solid);

	G4LogicalVolume *Lead_Tunnel_Logical = new G4LogicalVolume(Lead_Tunnel_Solid, Pb, "Lead_Tunnel_Logical");
	Lead_Tunnel_Logical->SetVisAttributes(green);

	new G4PVPlacement(0, global_coordinates + G4ThreeVector(0., 0., -First_Setup_Length*0.5 + Lead_Tunnel_Z*0.5 + 4.*inch), Lead_Tunnel_Logical, "Lead_Tunnel", World_Logical, false, 0, false);

	G4double Lead_Base_X = 16.*inch;
	G4double Lead_Base_Y = 4.*inch;
	G4double Lead_Base_Z = 16.*inch;
	G4Box *Lead_Base_Solid = new G4Box("Lead_Base_Solid", Lead_Base_X*0.5, Lead_Base_Y*0.5, Lead_Base_Z*0.5);

	G4LogicalVolume *Lead_Base_Logical = new G4LogicalVolume(Lead_Base_Solid, Pb, "Lead_Base_Logical");
	Lead_Base_Logical->SetVisAttributes(green);

	new G4PVPlacement(0, global_coordinates +  G4ThreeVector(0., -Lead_Tunnel_Y*0.5 - Lead_Base_Y*0.5, -First_Setup_Length*0.5 + Lead_Base_Z*0.5), Lead_Base_Logical, "Lead_Base", World_Logical, false, 0, false);

	G4double Concrete_Block_X = 14.*inch;
	G4double Concrete_Block_Y = 3.75*inch;
	G4double Concrete_Block_Z = 15.5*inch;

	G4Box *Concrete_Block_Solid = new G4Box("Concrete_Block_Solid", Concrete_Block_X*0.5, Concrete_Block_Y*0.5, Concrete_Block_Z*0.5);

	G4LogicalVolume *Concrete_Block_Logical = new G4LogicalVolume(Concrete_Block_Solid, concrete, "Concrete_Block_Logical");
	Concrete_Block_Logical->SetVisAttributes(white);

	new G4PVPlacement(0, global_coordinates +  G4ThreeVector(0., -Lead_Tunnel_Y*0.5 - Lead_Base_Y - Concrete_Block_Y*0.5, -First_Setup_Length*0.5 + Concrete_Block_Z*0.5), Concrete_Block_Logical, "Concrete_Block", World_Logical, false, 0, false);

	G4double Lead_Top_X = 16.*inch;
	G4double Lead_Top_Y = 4.*inch;
	G4double Lead_Top_Z = 8.*inch;
	G4Box *Lead_Top_Solid = new G4Box("Lead_Top_Solid", Lead_Top_X*0.5, Lead_Top_Y*0.5, Lead_Top_Z*0.5);

	G4LogicalVolume *Lead_Top_Logical = new G4LogicalVolume(Lead_Top_Solid, Pb, "Lead_Top_Logical");
	Lead_Top_Logical->SetVisAttributes(green);
	
	new G4PVPlacement(0, global_coordinates +  G4ThreeVector(0., Lead_Tunnel_Y*0.5 + Lead_Top_Y*0.5, -First_Setup_Length*0.5 + 4.*inch + Lead_Top_Z*0.5), Lead_Top_Logical, "Lead_Top", World_Logical, false, 0, false);

	G4double Lead_Top_Right_X = 4.*inch;
	G4double Lead_Top_Right_Y = 8.*inch;
	G4double Lead_Top_Right_Z = 4.*inch;
	G4Box *Lead_Top_Right_Solid = new G4Box("Lead_Top_Right_Solid", Lead_Top_Right_X*0.5, Lead_Top_Right_Y*0.5, Lead_Top_Right_Z*0.5);

	G4LogicalVolume *Lead_Top_Right_Logical = new G4LogicalVolume(Lead_Top_Right_Solid, Pb, "Lead_Top_Right_Logical");
	Lead_Top_Right_Logical->SetVisAttributes(green);

	new G4PVPlacement(0, global_coordinates +  G4ThreeVector(-6.*inch, -Lead_Tunnel_Y *0.5 + Lead_Top_Right_Y*0.5, -First_Setup_Length*0.5 + Lead_Top_Right_Z*0.5), Lead_Top_Right_Logical, "Lead_Top_Right", World_Logical, false, 0, false);

	G4double Lead_Portal_Top_X = 8.*inch;
	G4double Lead_Portal_Top_Y = 2.*inch;
	G4double Lead_Portal_Top_Z = 4.*inch;
	G4Box *Lead_Portal_Top_Solid = new G4Box("Lead_Portal_Top_Solid", Lead_Portal_Top_X*0.5, Lead_Portal_Top_Y*0.5, Lead_Portal_Top_Z*0.5);

	G4LogicalVolume *Lead_Portal_Top_Logical = new G4LogicalVolume(Lead_Portal_Top_Solid, Pb, "Lead_Portal_Top_Logical");
	Lead_Portal_Top_Logical->SetVisAttributes(green);

	new G4PVPlacement(0, global_coordinates +  G4ThreeVector(0., Lead_Tunnel_Y*0.5 + Lead_Portal_Top_Y*0.5, -First_Setup_Length*0.5 + 4.*inch + Lead_Tunnel_Z + Lead_Portal_Top_Z*0.5), Lead_Portal_Top_Logical, "Lead_Portal_Top", World_Logical, false, 0, false);

	G4double Lead_Portal_Right_X = 4.*inch;
	G4double Lead_Portal_Right_Y = 4.*inch;
	G4double Lead_Portal_Right_Z = 4.*inch;
	G4Box *Lead_Portal_Right_Solid = new G4Box("Lead_Portal_Right_Solid", Lead_Portal_Right_X*0.5, Lead_Portal_Right_Y*0.5, Lead_Portal_Right_Z*0.5);

	G4LogicalVolume *Lead_Portal_Right_Logical = new G4LogicalVolume(Lead_Portal_Right_Solid, Pb, "Lead_Portal_Right_Logical");
	Lead_Portal_Right_Logical->SetVisAttributes(green);

	new G4PVPlacement(0, global_coordinates +  G4ThreeVector(-3.*inch, -Lead_Tunnel_Y*0.5 + Lead_Portal_Right_Y*0.5, -First_Setup_Length*0.5 + 4.*inch + Lead_Tunnel_Z + Lead_Portal_Right_Z*0.5), Lead_Portal_Right_Logical, "Lead_Portal_Right", World_Logical, false, 0, false);

	G4double Lead_Portal_Left_X = 4.*inch;
	G4double Lead_Portal_Left_Y = 4.*inch;
	G4double Lead_Portal_Left_Z = 4.*inch;
	G4Box *Lead_Portal_Left_Solid = new G4Box("Lead_Portal_Left_Solid", Lead_Portal_Left_X*0.5, Lead_Portal_Left_Y*0.5, Lead_Portal_Left_Z*0.5);

	G4LogicalVolume *Lead_Portal_Left_Logical = new G4LogicalVolume(Lead_Portal_Left_Solid, Pb, "Lead_Portal_Left_Logical");
	Lead_Portal_Left_Logical->SetVisAttributes(green);
	
	new G4PVPlacement(0, global_coordinates +  G4ThreeVector(3.*inch, -Lead_Tunnel_Y*0.5 + Lead_Portal_Left_Y*0.5, -First_Setup_Length*0.5 + 4.*inch + Lead_Tunnel_Z + Lead_Portal_Left_Z*0.5), Lead_Portal_Left_Logical, "Lead_Portal_Left", World_Logical, false, 0, false);

	// Beam pipe holder

	G4double Beam_Pipe_Holder_X = 4.*inch;
	G4double Beam_Pipe_Holder_Y = Concrete_Block_Y + Lead_Base_Y + Lead_Tunnel_Y*0.5;	
	G4double Beam_Pipe_Holder_Z = 0.5*inch;;	

	G4Box *Beam_Pipe_Holder_Solid_Solid = new G4Box("Beam_Pipe_Holder_Solid_Solid", Beam_Pipe_Holder_X*0.5, Beam_Pipe_Holder_Y*0.5, Beam_Pipe_Holder_Z*0.5);

	G4Box *Beam_Pipe_Holder_Hole_Solid = new G4Box("Beam_Pipe_Holder_Hole_Solid", Beam_Pipe_Outer_Radius, Beam_Pipe_Outer_Radius, Beam_Pipe_Holder_Z);

	G4SubtractionSolid *Beam_Pipe_Holder_Solid = new G4SubtractionSolid("Beam_Pipe_Holder_Solid", Beam_Pipe_Holder_Solid_Solid, Beam_Pipe_Holder_Hole_Solid, 0, G4ThreeVector(0., Beam_Pipe_Holder_Y*0.5, 0.));

	G4LogicalVolume *Beam_Pipe_Holder_Logical = new G4LogicalVolume(Beam_Pipe_Holder_Solid, Al, "Beam_Pipe_Holder_Logical");
	Beam_Pipe_Holder_Logical->SetVisAttributes(grey);

	new G4PVPlacement(0, global_coordinates +  G4ThreeVector(0., -Beam_Pipe_Holder_Y*0.5, First_Setup_Length*0.5 -Beam_Pipe_Holder_Z*0.5), Beam_Pipe_Holder_Logical, "Beam_Pipe_Holder", World_Logical, false, 0, false);

	// Lead wrap around beam pipe
	
	G4double Lead_Wrap1_Thickness =0.04*inch;
	G4double Lead_Wrap1_Length = 11.5*inch ;

	G4Tubs* Lead_Wrap1_Solid = new G4Tubs("Lead_Wrap1_Solid", Beam_Pipe_Outer_Radius, Beam_Pipe_Outer_Radius + Lead_Wrap1_Thickness, Lead_Wrap1_Length*0.5, 0., twopi);

	G4LogicalVolume *Lead_Wrap1_Logical = new G4LogicalVolume(Lead_Wrap1_Solid, Pb, "Lead_Wrap1_Logical");
	Lead_Wrap1_Logical->SetVisAttributes(green);

	new G4PVPlacement(0, global_coordinates +  G4ThreeVector(0., 0., -First_Setup_Length*0.5 + 4.*inch + Lead_Tunnel_Z + Lead_Portal_Right_Z + Lead_Wrap1_Length*0.5), Lead_Wrap1_Logical, "Lead_Wrap1", World_Logical, false, 0, false);

	G4double Lead_Wrap2_Thickness =0.04*inch;
	G4double Lead_Wrap2_Length = 10.*inch;

	G4Tubs* Lead_Wrap2_Solid = new G4Tubs("Lead_Wrap2_Solid", Beam_Pipe_Outer_Radius, Beam_Pipe_Outer_Radius + Lead_Wrap2_Thickness, Lead_Wrap2_Length*0.5, 0., twopi);

	G4LogicalVolume *Lead_Wrap2_Logical = new G4LogicalVolume(Lead_Wrap2_Solid, Pb, "Lead_Wrap2_Logical");
	Lead_Wrap2_Logical->SetVisAttributes(green);

	new G4PVPlacement(0, global_coordinates +  G4ThreeVector(0., 0., First_Setup_Length*0.5 - Beam_Pipe_Holder_Z - Lead_Wrap2_Length*0.5), Lead_Wrap2_Logical, "Lead_Wrap2", World_Logical, false, 0, false);
	
	// Construct table plate
	G4double First_Setup_Box_Y = 12.*inch;
	G4double First_Setup_Table_Thickness = 0.5*inch; 

	G4Box *Table_Solid_Solid = new G4Box("Table_Solid_Solid", First_Setup_X*0.5, First_Setup_Table_Thickness*0.5, First_Setup_Length*0.5);

	G4double Table_Hole_X = 20.*inch;
	G4double Table_Hole_Thickness = 2.*First_Setup_Table_Thickness;
	G4double Table_Hole_Length = 24.5*inch;
		
	G4Box *Table_Hole_Solid = new G4Box("Table_Hole_Solid", Table_Hole_X*0.5, Table_Hole_Thickness*0.5, Table_Hole_Length*0.5);
	
	G4SubtractionSolid* Table_Solid = new G4SubtractionSolid("Table_Solid", Table_Solid_Solid, Table_Hole_Solid, 0, G4ThreeVector(0., 0., 6.*inch));

	G4LogicalVolume* Table_Logical = new G4LogicalVolume(Table_Solid, Al, "Table_Solid");
	Table_Logical->SetVisAttributes(grey);

	new G4PVPlacement(0, global_coordinates +  G4ThreeVector(0., -Lead_Tunnel_Y*0.5 - Lead_Base_Y - Concrete_Block_Y -First_Setup_Table_Thickness*0.5, 0.), Table_Logical, "First_Setup_Table", World_Logical, false, 0, false);
	
	// Construct columns on table
	
	G4double Table_Column_Base_Length = 1.5*inch;
	G4double Table_Column_Height = 18.5*inch;
	
	G4Box *Table_Column_Solid = new G4Box("Table_Column_Solid", Table_Column_Base_Length*0.5, Table_Column_Height*0.5, Table_Column_Base_Length*0.5);

	G4LogicalVolume *Table_Column_Logical = new G4LogicalVolume(Table_Column_Solid, Al, "Table_Column_Logical");
	Table_Column_Logical->SetVisAttributes(grey);

	new G4PVPlacement(0, global_coordinates +  G4ThreeVector(Table_Hole_X*0.5 + Table_Column_Base_Length*0.5, -Lead_Tunnel_Y*0.5 - Lead_Base_Y - Concrete_Block_Y + Table_Column_Height*0.5, -First_Setup_Length*0.5 + 3.5*inch), Table_Column_Logical, "Table_Column_0", World_Logical, false, 0, false);
	new G4PVPlacement(0, global_coordinates +  G4ThreeVector(-Table_Hole_X*0.5 - Table_Column_Base_Length*0.5, -Lead_Tunnel_Y*0.5 - Lead_Base_Y - Concrete_Block_Y +Table_Column_Height*0.5, -First_Setup_Length*0.5 + 3.5*inch), Table_Column_Logical, "Table_Column_1", World_Logical, false, 0, false);

	new G4PVPlacement(0, global_coordinates +  G4ThreeVector(Table_Hole_X*0.5 + Table_Column_Base_Length*0.5, -Lead_Tunnel_Y*0.5 - Lead_Base_Y - Concrete_Block_Y +Table_Column_Height*0.5, First_Setup_Length*0.5 - 3.5*inch), Table_Column_Logical, "Table_Column_2", World_Logical, false, 0, false);
	new G4PVPlacement(0, global_coordinates +  G4ThreeVector(-Table_Hole_X*0.5 - Table_Column_Base_Length*0.5, -Lead_Tunnel_Y*0.5 - Lead_Base_Y - Concrete_Block_Y + Table_Column_Height*0.5, First_Setup_Length*0.5 - 3.5*inch), Table_Column_Logical, "Table_Column_3", World_Logical, false, 0, false);
	
	// Construct box on top of table
	
	G4double Box_Wall_Thickness = 2.*inch;

	G4Box *Box_Wall_Solid = new G4Box("Box_Wall_Solid", Box_Wall_Thickness*0.5, First_Setup_Box_Y*0.5, First_Setup_Length*0.5);

	G4LogicalVolume *Box_Wall_Logical = new G4LogicalVolume(Box_Wall_Solid, one_third_density_Al, "Box_Wall_Logical");
	Box_Wall_Logical->SetVisAttributes(grey);

	new G4PVPlacement(0, global_coordinates +  G4ThreeVector(-Table_Hole_X*0.5 - Table_Column_Base_Length + Box_Wall_Thickness*0.5, -Lead_Tunnel_Y*0.5 - Lead_Base_Y - Concrete_Block_Y + Table_Column_Height + First_Setup_Box_Y*0.5, 0.), Box_Wall_Logical, "Box_Wall_0", World_Logical, false, 0, false);
	new G4PVPlacement(0, global_coordinates +  G4ThreeVector(Table_Hole_X*0.5 + Table_Column_Base_Length - Box_Wall_Thickness*0.5, -Lead_Tunnel_Y*0.5 - Lead_Base_Y - Concrete_Block_Y + Table_Column_Height + First_Setup_Box_Y*0.5, 0.), Box_Wall_Logical, "Box_Wall_1", World_Logical, false, 0, false);

	// Construct beams on box
	
	G4double Beam_Base_Length = 1.5*inch;
	G4Box *Box_Beam_Solid = new G4Box("Box_Beam_Solid", (Table_Hole_X - 1.*inch)*0.5, Beam_Base_Length*0.5, Beam_Base_Length*0.5);

	G4LogicalVolume *Box_Beam_Logical = new G4LogicalVolume(Box_Beam_Solid, one_third_density_Al, "Box_Beam_Logical");
	Box_Beam_Logical->SetVisAttributes(grey);

	new G4PVPlacement(0, global_coordinates +  G4ThreeVector(0., -Lead_Tunnel_Y*0.5 - Lead_Base_Y - Concrete_Block_Y + Table_Column_Height + First_Setup_Box_Y - Beam_Base_Length*0.5, -First_Setup_Length*0.5 + Beam_Base_Length*0.5), Box_Beam_Logical, "Box_Beam_0", World_Logical, false, 0, false);
	new G4PVPlacement(0, global_coordinates +  G4ThreeVector(0., -Lead_Tunnel_Y*0.5 - Lead_Base_Y - Concrete_Block_Y + Table_Column_Height + First_Setup_Box_Y - Beam_Base_Length*0.5, First_Setup_Length*0.5 -Beam_Base_Length*0.5), Box_Beam_Logical, "Box_Beam_1", World_Logical, false, 0, false);

	new G4PVPlacement(0, global_coordinates +  G4ThreeVector(0., -Lead_Tunnel_Y*0.5 - Lead_Base_Y - Concrete_Block_Y + Table_Column_Height + Beam_Base_Length*0.5, -First_Setup_Length*0.5 + Beam_Base_Length*0.5), Box_Beam_Logical, "Box_Beam_0", World_Logical, false, 0, false);
	new G4PVPlacement(0, global_coordinates +  G4ThreeVector(0., -Lead_Tunnel_Y*0.5 - Lead_Base_Y - Concrete_Block_Y + Table_Column_Height + Beam_Base_Length*0.5, First_Setup_Length*0.5 -Beam_Base_Length*0.5), Box_Beam_Logical, "Box_Beam_1", World_Logical, false, 0, false);

}
