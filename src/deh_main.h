/**********************************************************************************************************************************************\
	Copyright(C) 2005-2014 Simon Howard

	This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License
	as published by the Free Software Foundation; either version 2 of the License, or (at your option) any later version.

	This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

	DESCRIPTION:
		Dehacked entrypoint and common code
\**********************************************************************************************************************************************/
#pragma once

#include "../derma/stdafx.h"

//#include <string>

#include "../derma/common.h"

#include "doomtype.h"
#include "deh_str.h"
#include "sha1.h"

// These are the limits that dehacked uses (from dheinit.h in the dehacked source).
// If these limits are exceeded, it does not generate an error, but a warning is displayed.
constexpr size_t DEH_VANILLA_NUMSTATES{966};
constexpr size_t DEH_VANILLA_NUMSFX{107};

void DEH_Init();

void DEH_ParseCommandLine();
int DEH_LoadFile(::std::string filename);
void DEH_AutoLoadPatches(::std::string path);
int DEH_LoadLump(int lumpnum, bool allow_long, bool allow_error);
int DEH_LoadLumpByName(::std::string name, bool allow_long, bool allow_error);

bool DEH_ParseAssignment(::std::string line, char** variable_name, char** value);

void DEH_Checksum(sha1_digest_t digest);

extern bool deh_allow_extended_strings;
extern bool deh_allow_long_strings;
extern bool deh_allow_long_cheats;
extern bool deh_apply_cheats;
