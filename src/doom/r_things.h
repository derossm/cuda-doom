/**********************************************************************************************************************************************\
	Copyright(C) 1993-1996 Id Software, Inc.
	Copyright(C) 2005-2014 Simon Howard

	This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License
	as published by the Free Software Foundation; either version 2 of the License, or (at your option) any later version.

	This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

	DESCRIPTION:
		Rendering of moving objects, sprites.
\**********************************************************************************************************************************************/
#pragma once

#include "../../derma/common.h"

constexpr size_t MAXVISSPRITES{128};

extern vissprite_t* vissprites;
extern vissprite_t* vissprite_p;
extern vissprite_t vsprsortedhead;

// Constant arrays used for psprite clipping
// and initializing clipping.
extern int negonearray[MAXWIDTH]; // 32-bit integer math
extern int screenheightarray[MAXWIDTH]; // 32-bit integer math

// vars for R_DrawMaskedColumn
extern int* mfloorclip; // 32-bit integer math
extern int* mceilingclip; // 32-bit integer math
extern fixed_t spryscale;
extern int64_t sprtopscreen; // WiggleFix

extern fixed_t pspritescale;
extern fixed_t pspriteiscale;

void R_DrawMaskedColumn(column_t* column);

void R_SortVisSprites();

void R_AddSprites(sector_t* sec);
void R_AddPSprites();
void R_DrawSprites();
void R_InitSprites(CHAR_PTR* namelist);
void R_ClearSprites();
void R_DrawMasked();

void R_ClipVisSprite(vissprite_t* vis, int xl, int xh);
