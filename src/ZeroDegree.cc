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
//	120%  ZeroDegree Detector @ HIGS Serial No. 33-P40383A
//**************************************************************//

#include "ZeroDegree.hh"
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

#include "NamedColors.hh"

ZeroDegree::ZeroDegree(G4String Detector_Name) {

  G4NistManager *nist = G4NistManager::Instance();

  G4Material *Cu = nist->FindOrBuildMaterial("G4_Cu");
  G4Material *Al = nist->FindOrBuildMaterial("G4_Al");
  G4Material *Ge = nist->FindOrBuildMaterial("G4_Ge");
  G4Material *vacuum = nist->FindOrBuildMaterial("G4_Galactic");
  G4Material *Be = nist->FindOrBuildMaterial("G4_Be");

  // Detector dimensions and materials as given in the ORTEC data sheet.

  G4double Detector_Radius = 82. * mm / 2.;
  G4double Detector_Length = 108.7 * mm;
  G4double Detector_End_Radius = 8. * mm; // (J)
  G4double Hole_Radius = 10.9 * mm / 2.;
  G4double Hole_Depth = 96.1 * mm;
  G4double Hole_Bottom_Radius = 10.9 * mm / 2;

  G4double MountCup_Length = 130. * mm; // (A)
  G4double End_Cap_To_Crystal_Gap = 4. * mm; // (B)
  G4double MountCup_Base = 3.2 * mm; // (C)
  G4double EndCap_Window = 1.5 * mm; // (D)
  G4double MountCup_Wall = 0.76 * mm; // (H)
  G4double EndCap_Wall = 1.5 * mm; // (I)

  G4double ColdFinger_Radius =
      4. * mm / 2; // Suggestion by B. Fallin, not in the ORTEC data sheet.
  G4double ColdFinger_Length =
      MountCup_Base +
      (MountCup_Length - MountCup_Wall - MountCup_Base - Detector_Length) +
      Hole_Depth - 5. * mm;

  G4Material *Mother_Material = vacuum;
  G4Material *MountCup_Material = Al;
  G4Material *EndCap_Material = Al;
  G4Material *EndCap_Window_Material = Be;
  G4Material *ColdFinger_Material = Cu;
  G4Material *Crystal_Material = Ge;

  // Mother Volume

  G4double Mother_Radius =
      Detector_Radius + MountCup_Wall + End_Cap_To_Crystal_Gap + EndCap_Wall;
  G4double Mother_Length =
      EndCap_Window + End_Cap_To_Crystal_Gap + MountCup_Length;

  Length = Mother_Length;
  Radius = Mother_Radius;

  G4Tubs *Mother_Solid = new G4Tubs("Mother_Solid", 0., Mother_Radius,
                                    Mother_Length / 2, 0. * deg, 360. * deg);
  ZeroDegree_Logical = new G4LogicalVolume(Mother_Solid, Mother_Material,
                                           "Mother_Logical", 0, 0, 0);

  ZeroDegree_Logical->SetVisAttributes(new G4VisAttributes(red));

  // End Cap

  G4double EndCap_innerRadius =
      Detector_Radius + MountCup_Wall + End_Cap_To_Crystal_Gap;
  G4double EndCap_outerRadius =
      Detector_Radius + MountCup_Wall + End_Cap_To_Crystal_Gap + EndCap_Wall;

  G4double EndCap_Length = MountCup_Length + End_Cap_To_Crystal_Gap;

  G4Tubs *EndCap_Solid =
      new G4Tubs("EndCap_Solid", EndCap_innerRadius, EndCap_outerRadius,
                 EndCap_Length / 2, 0. * deg, 360. * deg);
  G4LogicalVolume *EndCap_Logical = new G4LogicalVolume(
      EndCap_Solid, EndCap_Material, "EndCap_Logical", 0, 0, 0);

  EndCap_Logical->SetVisAttributes(new G4VisAttributes(gray));

  new G4PVPlacement(0, G4ThreeVector(0., 0., Length / 2 - EndCap_Length / 2 - EndCap_Window),
                    EndCap_Logical, "EndCap", ZeroDegree_Logical, false, 0);

  // End Cap Window

  G4double EndCap_Window_Radius = EndCap_outerRadius;

  G4Tubs *EndCap_Window_Solid =
      new G4Tubs("EndCap_Window_Solid", 0., EndCap_Window_Radius,
                 EndCap_Window / 2, 0. * deg, 360. * deg);
  G4LogicalVolume *EndCap_Window_Logical =
      new G4LogicalVolume(EndCap_Window_Solid, EndCap_Window_Material,
                          "EndCap_Window_Logical", 0, 0, 0);

  EndCap_Window_Logical->SetVisAttributes(new G4VisAttributes(gray));

  new G4PVPlacement(0, G4ThreeVector(0., 0., Length / 2 - EndCap_Window / 2),
                    EndCap_Window_Logical, "EndCap_Window",
                    ZeroDegree_Logical, false, 0);

  // Mount Cup Wall

  G4double MountCup_innerRadius = Detector_Radius;
  G4double MountCup_outerRadius = Detector_Radius + MountCup_Wall;
  G4double MountCup_Wall_Length =
      MountCup_Length - MountCup_Wall - MountCup_Base;

  G4Tubs *MountCup_Solid =
      new G4Tubs("MountCup_Solid", MountCup_innerRadius, MountCup_outerRadius,
                 MountCup_Wall_Length / 2, 0. * deg, 360. * deg);
  G4LogicalVolume *MountCup_Logical = new G4LogicalVolume(
      MountCup_Solid, MountCup_Material, "MountCup_Logical", 0, 0, 0);

  MountCup_Logical->SetVisAttributes(new G4VisAttributes(cyan));

  new G4PVPlacement(
      0, G4ThreeVector(0., 0., Length / 2 - EndCap_Window - End_Cap_To_Crystal_Gap - MountCup_Wall - MountCup_Wall_Length / 2),
      MountCup_Logical, "MountCup_Wall", ZeroDegree_Logical, false, 0);

  // Mount Cup Base

  G4double MountCup_Base_outerRadius = Detector_Radius + MountCup_Wall;
  G4double MountCup_Base_innerRadius = Hole_Bottom_Radius;

  G4Tubs *MountCup_Base_Solid = new G4Tubs(
      "MountCup_Base_Solid", MountCup_Base_innerRadius,
      MountCup_Base_outerRadius, MountCup_Base / 2, 0. * deg, 360. * deg);
  G4LogicalVolume *MountCup_Base_Logical =
      new G4LogicalVolume(MountCup_Base_Solid, MountCup_Material,
                          "MountCup_Base_Logical", 0, 0, 0);

  MountCup_Base_Logical->SetVisAttributes(new G4VisAttributes(cyan));

  new G4PVPlacement(0, G4ThreeVector(0., 0., -Length / 2 + MountCup_Base / 2),
                    MountCup_Base_Logical, "MountCupBase", ZeroDegree_Logical,
                    false, 0);

  // Mount Cup Face

  G4double MountCup_Face_Radius = Detector_Radius + MountCup_Wall;

  G4Tubs *MountCup_Face_Solid =
      new G4Tubs("MountCup_Face_Solid", 0., MountCup_Face_Radius,
                 MountCup_Wall / 2, 0. * deg, 360. * deg);
  G4LogicalVolume *MountCup_Face_Logical =
      new G4LogicalVolume(MountCup_Face_Solid, MountCup_Material,
                          "MountCup_Face_Logical", 0, 0, 0);

  MountCup_Face_Logical->SetVisAttributes(new G4VisAttributes(cyan));

  new G4PVPlacement(0, G4ThreeVector(0., 0., Length / 2 - EndCap_Window - End_Cap_To_Crystal_Gap - MountCup_Wall / 2),
                    MountCup_Face_Logical, "MountCup_Face",
                    ZeroDegree_Logical, false, 0);

  // Cold Finger

  const G4int nsteps = 100;

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

  new G4PVPlacement(0, G4ThreeVector(0., 0., -Length * 0.5),
                    ColdFinger_Logical, "ColdFinger", ZeroDegree_Logical,
                    false, 0);

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
                    Crystal_Logical, "Crystal", ZeroDegree_Logical, false, 0);
}
