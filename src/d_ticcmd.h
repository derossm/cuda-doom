/**********************************************************************************************************************************************\
	Copyright(C) 1993-1996 Id Software, Inc.
	Copyright(C) 1993-2008 Raven Software
	Copyright(C) 2005-2014 Simon Howard

	This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License
	as published by the Free Software Foundation; either version 2 of the License, or (at your option) any later version.

	This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

	DESCRIPTION:
		System specific interface stuff.
\**********************************************************************************************************************************************/
#pragma once

#include "../derma/common.h"

#include "doomtype.h"

#include "d_event.h"

struct ArtiType_t
{
};

// The data sampled per tick (single player) and transmitted to other peers (multiplayer).
// Mainly movements/button commands per game tick, plus a checksum for internal state consistency.
struct ticcmd_t
{
	char forwardmove;	// *2048 for move
	char sidemove;		// *2048 for move
	short angleturn;			// <<16 for angle delta
	byte chatchar;
	buttoncode buttons;

	// villsa [STRIFE] according to the asm, consistancy is a short, not a byte
	byte consistancy;			// checks for net game

	// villsa - Strife specific:
	buttoncode2_t buttons2;
	int inventory;

	// Heretic/Hexen specific:
	byte lookfly;				// look/fly up/down/centering
	ArtiType_t arti;			// ArtiType_t to use

	int lookdir;
};
