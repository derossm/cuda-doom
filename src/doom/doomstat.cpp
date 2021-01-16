/**********************************************************************************************************************************************\
	Copyright(C) 1993-1996 Id Software, Inc.
	Copyright(C) 2005-2014 Simon Howard

	This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License
	as published by the Free Software Foundation; either version 2 of the License, or (at your option) any later version.

	This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

	DESCRIPTION:
		Put all global tate variables here.
\**********************************************************************************************************************************************/
#include "../derma/stdafx.h"

#include "doomstat.h"

// Game Mode - identify IWAD as shareware, retail etc.
GameMode gamemode = GameMode::indetermined;
GameMission gamemission = GameMission::doom;
GameVersion gameversion = GameVersion::exe_final2;
GameVariant gamevariant = GameVariant::vanilla;

// Set if homebrew PWAD stuff has been added.
bool modifiedgame;
