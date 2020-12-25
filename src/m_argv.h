/**********************************************************************************************************************************************\
	Copyright(C) 1993-1996 Id Software, Inc.
	Copyright(C) 2005-2014 Simon Howard

	This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License
	as published by the Free Software Foundation; either version 2 of the License, or (at your option) any later version.

	This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
\**********************************************************************************************************************************************/
#pragma once

#include "../derma/common.h"

#ifndef __M_ARGV__
#define __M_ARGV__

#include "doomtype.h"

// MISC
extern int myargc;
extern char** myargv;

// Returns the position of the given parameter
// in the arg list (0 if not found).
int M_CheckParm (const char* check);

// Same as M_CheckParm, but checks that num_args arguments are available
// following the specified argument.
int M_CheckParmWithArgs(const char* check, int num_args);

void M_FindResponseFile();
void M_AddLooseFiles();

// Parameter has been specified?
bool M_ParmExists(const char* check);

// Get name of executable used to run this program:
const char *M_GetExecutableName();

#endif
