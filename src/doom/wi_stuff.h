/**********************************************************************************************************************************************\
	Copyright(C) 1993-1996 Id Software, Inc.
	Copyright(C) 2005-2014 Simon Howard

	This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License
	as published by the Free Software Foundation; either version 2 of the License, or (at your option) any later version.

	This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
\**********************************************************************************************************************************************/
#pragma once

#include "../../derma/stdafx.h"

#include "../../derma/common.h"

#include "doomdef.h"

// States for the intermission
enum class stateenum_t
{
	NoState = -1,
	StatCount,
	ShowNextLoc,
};

// Called by main loop, animate the intermission.
void WI_Ticker();

// Called by main loop,
// draws the intermission directly into the screen buffer.
void WI_Drawer();

// Setup for an intermission screen.
void WI_Start(wbstartstruct_t* wbstartstruct);

// Shut down the intermission screen
void WI_End();
