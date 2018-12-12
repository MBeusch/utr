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

#ifndef RunAction_h
#define RunAction_h 1

#include "G4UserRunAction.hh"
#include "globals.hh"

#include <string>

using std::string;

enum output_flags: short {
	EDEP = 0,	
	EKIN = 1,	
	PARTICLE = 2,	
	VOLUME = 3,	
	POSX = 4,	
	POSY = 5,	
	POSZ = 6,	
	MOMX = 7,	
	MOMY = 8,	
	MOMZ = 9,
	NFLAGS = 10
};

class RunAction : public G4UserRunAction {
  public:
	RunAction();
	virtual ~RunAction();

	virtual void BeginOfRunAction(const G4Run *);
	virtual void EndOfRunAction(const G4Run *);

	G4String GetOutputFlagName(unsigned int n);

	static const int n_output_flags = 10;
	void setOutputDir(string output) { outputdir = output; };

  private:
	unsigned int output_flags[n_output_flags];
	string outputdir;
};

#endif
