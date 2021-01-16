/**********************************************************************************************************************************************\
	Copyright(C) 1993-1996 Id Software, Inc.
	Copyright(C) 1993-2008 Raven Software
	Copyright(C) 2005-2014 Simon Howard

	This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License
	as published by the Free Software Foundation; either version 2 of the License, or (at your option) any later version.

	This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

	DESCRIPTION:
		Lookup tables.
		Do not try to look them up :-).
		In the order of appearance:

		int finetangent[4096]	- Tangens LUT.
			Should work with BAM fairly well (12 of 16bit,
			effectively, by shifting).

		int finesine[10240]		- Sine lookup.
			Guess what, serves as cosine, too.
			Remarkable thing is, how to use BAMs with this?

		int tantoangle[2049]	- ArcTan LUT,
			maps tan(angle) to angle fast. Gotta search.
\**********************************************************************************************************************************************/
#pragma once

#include "../derma/stdafx.h"

#include "../derma/common.h"

#include "doomtype.h"

#include "m_fixed.h"

constexpr size_t FINEANGLES{8192};
constexpr size_t FINEMASK{(FINEANGLES - 1)};

// 0x100000000 to 0x2000
constexpr size_t ANGLETOFINESHIFT{19};

// Effective size is 10240.
extern const fixed_t finesine[5 * FINEANGLES / 4];

// Re-use data, is just PI/2 pahse shift.
extern const fixed_t* finecosine;

// Effective size is 4096.
extern const fixed_t finetangent[FINEANGLES / 2];

// Gamma correction tables.
extern const byte gammatable[5][256];

// Binary Angle Measument, BAM.

constexpr size_t ANG45{0x20000000};
constexpr size_t ANG90{0x40000000};
constexpr size_t ANG180{0x80000000};
constexpr size_t ANG270{0xc0000000};
constexpr size_t ANG_MAX{0xffffffff};

constexpr size_t ANG1{(ANG45 / 45)};
constexpr size_t ANG60{(ANG180 / 3)};

// Heretic code uses this definition as though it represents one degree, but it is not! This is actually ~1.40 degrees.
constexpr size_t ANG1_X{0x01000000};

constexpr size_t SLOPERANGE{2048};
constexpr size_t SLOPEBITS{11};
constexpr size_t DBITS{(FRACBITS - SLOPEBITS)};

using angle_t = unsigned;

// Effective size is 2049;
// The +1 size is to handle the case when x==y without additional checking.
extern const angle_t tantoangle[SLOPERANGE + 1];

// Utility function, called by R_PointToAngle.
int SlopeDiv(unsigned num, unsigned den);
int SlopeDivCrispy(unsigned num, unsigned den);
