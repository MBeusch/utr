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

//**************************************************************//
//	Nominal 100% coaxial HPGe,
//	Ortec serial number: 73954
//	Dimensions from g4horus Geant4 implementation of the
//	HORUS setup in Cologne (https://gitlab.ikp.uni-koeln.de/jmayer/g4horus/)
//	Published under the MIT license (see header file)
//**************************************************************//

#include "HPGe_Cologne.hh"
#include "G4LogicalVolume.hh"
#include "G4Material.hh"
#include "G4MaterialTable.hh"
#include "G4PVPlacement.hh"
#include "G4Polycone.hh"
#include "G4RotationMatrix.hh"
#include "G4ThreeVector.hh"
#include "G4Tubs.hh"
#include "G4VisAttributes.hh"

#include "G4NistManager.hh"
#include "G4RunManager.hh"

#include "G4PhysicalConstants.hh"
#include "G4SystemOfUnits.hh"

#include "OptimizePolycone.hh"
#include "Units.hh"

#include "NamedColors.hh"

HPGe_Cologne::HPGe_Cologne(G4String Detector_Name) {

  G4NistManager *nist = G4NistManager::Instance();

  G4Material *Cu = nist->FindOrBuildMaterial("G4_Cu");
  G4Material *Al = nist->FindOrBuildMaterial("G4_Al");
  G4Material *Ge = nist->FindOrBuildMaterial("G4_Ge");
  G4Material *vacuum = nist->FindOrBuildMaterial("G4_Galactic");

  // Detector dimensions as given in g4horus. The corresponding g4horus variables are
  // given in the comments

  G4double Detector_Radius = 74. * mm * 0.5; //     specs["73954"].crystal.diameter = 74. * mm
  G4double Detector_Length = 70. * mm; //     specs["73954"].crystal.length = (72. - 2.) * mm;
  G4double Detector_End_Radius = 1. * mm; // Estimated
  G4double Hole_Radius = 5.8 * mm; //     specs["73954"].crystal.hole_diameter = 5.8 * mm * 2.; // guess
  G4double Hole_Depth = 52. * mm; //     specs["73954"].crystal.hole_length = 52. * mm; //guess
  G4double Hole_Bottom_Radius = 5.8 * mm; // From hole radius

  // G4double MountCup_Length = 160. * mm; //     specs["73954"].hull.length = 16. * cm;
  G4double MountCup_Length = 10.5 * inch; //     Measured at the detector. The difference to above might be because the data sheet shows only the case of the crystal, not the preamp electronics
  G4double End_Cap_To_Crystal_Gap = 3. * mm; //     specs["73954"].hull.padding = 3. * mm;
  G4double MountCup_Base = 5. * mm; // Estimated
  G4double EndCap_Window = 1. * mm; //     specs["73954"].hull.endcap_thickness = 1. * mm;
  G4double MountCup_Wall = 1. * mm; //    specs["73954"].hull.thickness = 1. * mm;
  G4double EndCap_Wall = 1. * mm; //    specs["73954"].hull.thickness = 1. * mm;

  G4double ColdFinger_Radius =
      4. * mm; // Estimated
  G4double ColdFinger_Length =
      MountCup_Base +
      (MountCup_Length - MountCup_Wall - MountCup_Base - Detector_Length) +
      Hole_Depth - 10. * mm; // Estimated

  G4Material *Mother_Material = vacuum;
  G4Material *MountCup_Material = Al;
  G4Material *EndCap_Material = Al;
  G4Material *EndCap_Window_Material = Al;
  G4Material *ColdFinger_Material = Cu;
  G4Material *Crystal_Material = Ge;
  G4Material *Dewar_Material = Al;
  G4Material *Connection_Material = Al;

  // Connection between dewar and mount cup
  G4double Connection_Length = 3.5 * inch;
  G4double Connection_Radius = 0.75 * inch;

  // Dewar dimensions

  G4double Dewar_Length = 10.5 * inch;
  G4double Dewar_Outer_Radius = 5. * inch;
  G4double Dewar_Wall_Thickness = 5. * mm; // Estimated

  // Mother Volume

  G4double Mother_Radius = Dewar_Outer_Radius;
  G4double Mother_Length =
      EndCap_Window + End_Cap_To_Crystal_Gap + MountCup_Length + Connection_Length + Dewar_Length;

  Length = Mother_Length;
  Radius = Mother_Radius;
  Front_Radius = Detector_Radius + MountCup_Wall + End_Cap_To_Crystal_Gap + EndCap_Wall;

  G4Tubs *Mother_Solid = new G4Tubs("Mother_Solid", 0., Mother_Radius,
                                    Mother_Length * 0.5, 0. * deg, 360. * deg);
  HPGe_Cologne_Logical = new G4LogicalVolume(Mother_Solid, Mother_Material,
                                             "Mother_Logical", 0, 0, 0);

  HPGe_Cologne_Logical->SetVisAttributes(G4VisAttributes::GetInvisible());

  // End Cap

  G4double EndCap_innerRadius =
      Detector_Radius + MountCup_Wall + End_Cap_To_Crystal_Gap;
  G4double EndCap_outerRadius =
      Detector_Radius + MountCup_Wall + End_Cap_To_Crystal_Gap + EndCap_Wall;

  G4double EndCap_Length = MountCup_Length + End_Cap_To_Crystal_Gap;

  G4Tubs *EndCap_Solid =
      new G4Tubs("EndCap_Solid", EndCap_innerRadius, EndCap_outerRadius,
                 EndCap_Length * 0.5, 0. * deg, 360. * deg);
  G4LogicalVolume *EndCap_Logical = new G4LogicalVolume(
      EndCap_Solid, EndCap_Material, "EndCap_Logical", 0, 0, 0);

  EndCap_Logical->SetVisAttributes(new G4VisAttributes(gray));

  new G4PVPlacement(0, G4ThreeVector(0., 0., Length * 0.5 - EndCap_Length * 0.5 - EndCap_Window),
                    EndCap_Logical, "EndCap", HPGe_Cologne_Logical, false, 0);

  // End Cap Window

  G4double EndCap_Window_Radius = EndCap_outerRadius;

  G4Tubs *EndCap_Window_Solid =
      new G4Tubs("EndCap_Window_Solid", 0., EndCap_Window_Radius,
                 EndCap_Window * 0.5, 0. * deg, 360. * deg);
  G4LogicalVolume *EndCap_Window_Logical =
      new G4LogicalVolume(EndCap_Window_Solid, EndCap_Window_Material,
                          "EndCap_Window_Logical", 0, 0, 0);

  EndCap_Window_Logical->SetVisAttributes(new G4VisAttributes(gray));

  new G4PVPlacement(0, G4ThreeVector(0., 0., Length * 0.5 - EndCap_Window * 0.5),
                    EndCap_Window_Logical, "EndCap_Window", HPGe_Cologne_Logical,
                    false, 0);

  // Mount Cup Wall

  G4double MountCup_innerRadius = Detector_Radius;
  G4double MountCup_outerRadius = Detector_Radius + MountCup_Wall;
  G4double MountCup_Wall_Length =
      MountCup_Length - MountCup_Wall - MountCup_Base;

  G4Tubs *MountCup_Solid =
      new G4Tubs("MountCup_Solid", MountCup_innerRadius, MountCup_outerRadius,
                 MountCup_Wall_Length * 0.5, 0. * deg, 360. * deg);
  G4LogicalVolume *MountCup_Logical = new G4LogicalVolume(
      MountCup_Solid, MountCup_Material, "MountCup_Logical", 0, 0, 0);

  MountCup_Logical->SetVisAttributes(new G4VisAttributes(cyan));

  new G4PVPlacement(
      0, G4ThreeVector(0., 0., Length * 0.5 - EndCap_Window - End_Cap_To_Crystal_Gap - MountCup_Wall - MountCup_Wall_Length * 0.5),
      MountCup_Logical, "MountCup_Wall", HPGe_Cologne_Logical, false, 0);

  // Mount Cup Base

  G4double MountCup_Base_outerRadius = Detector_Radius + MountCup_Wall;
  G4double MountCup_Base_innerRadius = Hole_Bottom_Radius;

  G4Tubs *MountCup_Base_Solid = new G4Tubs(
      "MountCup_Base_Solid", MountCup_Base_innerRadius,
      MountCup_Base_outerRadius, MountCup_Base * 0.5, 0. * deg, 360. * deg);
  G4LogicalVolume *MountCup_Base_Logical =
      new G4LogicalVolume(MountCup_Base_Solid, MountCup_Material,
                          "MountCup_Base_Logical", 0, 0, 0);

  MountCup_Base_Logical->SetVisAttributes(new G4VisAttributes(cyan));

  new G4PVPlacement(0, G4ThreeVector(0., 0., Length * 0.5 - EndCap_Window - End_Cap_To_Crystal_Gap - MountCup_Wall + MountCup_Base * 0.5),
                    MountCup_Base_Logical, "MountCupBase", HPGe_Cologne_Logical,
                    false, 0);

  // Mount Cup Face

  G4double MountCup_Face_Radius = Detector_Radius + MountCup_Wall;

  G4Tubs *MountCup_Face_Solid =
      new G4Tubs("MountCup_Face_Solid", 0., MountCup_Face_Radius,
                 MountCup_Wall * 0.5, 0. * deg, 360. * deg);
  G4LogicalVolume *MountCup_Face_Logical =
      new G4LogicalVolume(MountCup_Face_Solid, MountCup_Material,
                          "MountCup_Face_Logical", 0, 0, 0);

  MountCup_Face_Logical->SetVisAttributes(new G4VisAttributes(cyan));

  new G4PVPlacement(0, G4ThreeVector(0., 0., Length * 0.5 - EndCap_Window - End_Cap_To_Crystal_Gap - MountCup_Wall * 0.5),
                    MountCup_Face_Logical, "MountCup_Face", HPGe_Cologne_Logical,
                    false, 0);

  // Cold Finger

  const G4int nsteps = 500;

  G4double zPlaneTemp[nsteps];
  G4double rInnerTemp[nsteps];
  G4double rOuterTemp[nsteps];

  G4double z;

  for (int i = 0; i < nsteps; i++) {
    z = (double)i / (nsteps - 1) * ColdFinger_Length;

    zPlaneTemp[i] = z;

    rInnerTemp[i] = 0. * mm;

    if (z <= ColdFinger_Length - ColdFinger_Radius) {
      rOuterTemp[i] = ColdFinger_Radius;
    } else if (z <= ColdFinger_Length) {
      rOuterTemp[i] =
          ColdFinger_Radius *
          sqrt(1 - pow((z - (ColdFinger_Length - ColdFinger_Radius)) /
                           ColdFinger_Radius,
                       2));
    } else {
      rOuterTemp[i] = 0. * mm;
    }
  }

  G4double zPlane[nsteps];
  G4double rInner[nsteps];
  G4double rOuter[nsteps];

  OptimizePolycone *opt = new OptimizePolycone();
  G4int nsteps_optimized =
      opt->Optimize(zPlaneTemp, rInnerTemp, rOuterTemp, zPlane, rInner,
                    rOuter, nsteps, "ColdFinger_Solid");

  G4Polycone *ColdFinger_Solid =
      new G4Polycone("ColdFinger_Solid", 0. * deg, 360. * deg,
                     nsteps_optimized, zPlane, rInner, rOuter);

  G4LogicalVolume *ColdFinger_Logical = new G4LogicalVolume(
      ColdFinger_Solid, ColdFinger_Material, "ColdFinger_Logical", 0, 0, 0);

  ColdFinger_Logical->SetVisAttributes(new G4VisAttributes(orange));

  new G4PVPlacement(0, G4ThreeVector(0., 0., Length * 0.5 - EndCap_Window - End_Cap_To_Crystal_Gap - MountCup_Length),
                    ColdFinger_Logical, "ColdFinger", HPGe_Cologne_Logical, false,
                    0, false);

  // Germanium Detector Crystal

  for (int i = 0; i < nsteps; i++) {
    z = (double)i / (nsteps - 1) * Detector_Length;

    zPlaneTemp[i] = z;

    if (z <= Hole_Depth) {
      if (z <= Hole_Depth - Hole_Bottom_Radius) {
        rInnerTemp[i] = Hole_Radius;
      } else {
        rInnerTemp[i] =
            Hole_Radius *
            sqrt(1 - pow((z - (Hole_Depth - Hole_Bottom_Radius)) /
                             Hole_Bottom_Radius,
                         2));
      }
    } else {
      rInnerTemp[i] = 0. * mm;
    }

    if (z <= Detector_Length - Detector_End_Radius) {
      rOuterTemp[i] = Detector_Radius;
    } else if (z <= Detector_Length) {
      rOuterTemp[i] =
          Detector_End_Radius *
              sqrt(1 - pow((z - (Detector_Length - Detector_End_Radius)) /
                               Detector_End_Radius,
                           2)) +
          (Detector_Radius - Detector_End_Radius);
    } else {
      rOuterTemp[i] = 0. * mm;
    }
  }

  nsteps_optimized = opt->Optimize(zPlaneTemp, rInnerTemp, rOuterTemp, zPlane,
                                   rInner, rOuter, nsteps, "Crystal_Solid");

  G4Polycone *Crystal_Solid =
      new G4Polycone("Crystal_Solid", 0. * deg, 360. * deg, nsteps_optimized,
                     zPlane, rInner, rOuter);

  G4LogicalVolume *Crystal_Logical = new G4LogicalVolume(
      Crystal_Solid, Crystal_Material, Detector_Name, 0, 0, 0);

  Crystal_Logical->SetVisAttributes(new G4VisAttributes(green));

  new G4PVPlacement(0, G4ThreeVector(0., 0., Length * 0.5 - EndCap_Window - End_Cap_To_Crystal_Gap - MountCup_Wall - Detector_Length),
                    Crystal_Logical, "Crystal", HPGe_Cologne_Logical, false, 0);

  // Connection

  G4Tubs *Connection_Solid = new G4Tubs("Connection_Solid", 0., Connection_Radius, Connection_Length * 0.5, 0., twopi);
  G4LogicalVolume *Connection_Logical = new G4LogicalVolume(Connection_Solid, Connection_Material, "Connection_Logical");

  Connection_Logical->SetVisAttributes(grey);

  new G4PVPlacement(0, G4ThreeVector(0., 0., Length * 0.5 - EndCap_Window - End_Cap_To_Crystal_Gap - MountCup_Length - Connection_Length * 0.5), Connection_Logical, "Connection", HPGe_Cologne_Logical, false, 0, false);

  // Dewar

  G4Tubs *Dewar_Lid_Solid = new G4Tubs("Dewar_Lid_Solid", 0, Dewar_Outer_Radius, Dewar_Wall_Thickness * 0.5, 0., twopi);
  G4LogicalVolume *Dewar_Lid_Logical = new G4LogicalVolume(Dewar_Lid_Solid, Dewar_Material, "Dewar_Lid_Logical");
  Dewar_Lid_Logical->SetVisAttributes(cyan);

  new G4PVPlacement(0, G4ThreeVector(0., 0., Length * 0.5 - EndCap_Window - End_Cap_To_Crystal_Gap - MountCup_Length - Connection_Length - Dewar_Wall_Thickness * 0.5), Dewar_Lid_Logical, "Dewar_Lid_1", HPGe_Cologne_Logical, false, 0, false);

  G4Tubs *Dewar_Wall_Solid = new G4Tubs("Dewar_Wall_Solid", Dewar_Outer_Radius - Dewar_Wall_Thickness, Dewar_Outer_Radius, (Dewar_Length - 2. * Dewar_Wall_Thickness) * 0.5, 0., twopi);
  G4LogicalVolume *Dewar_Wall_Logical = new G4LogicalVolume(Dewar_Wall_Solid, Dewar_Material, "Dewar_Wall_Logical");
  Dewar_Wall_Logical->SetVisAttributes(cyan);

  new G4PVPlacement(0, G4ThreeVector(0., 0., Length * 0.5 - EndCap_Window - End_Cap_To_Crystal_Gap - MountCup_Length - Connection_Length - Dewar_Length * 0.5), Dewar_Wall_Logical, "Dewar_Wall", HPGe_Cologne_Logical, false, 0, false);

  new G4PVPlacement(0, G4ThreeVector(0., 0., Length * 0.5 - EndCap_Window - End_Cap_To_Crystal_Gap - MountCup_Length - Connection_Length - Dewar_Length + Dewar_Wall_Thickness * 0.5), Dewar_Lid_Logical, "Dewar_Lid_2", HPGe_Cologne_Logical, false, 0, false);
}
