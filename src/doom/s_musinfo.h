/**********************************************************************************************************************************************\
	Copyright(C) 1993-1996 Id Software, Inc.
	Copyright(C) 1993-2008 Raven Software
	Copyright(C) 2005-2014 Simon Howard
	Copyright(C) 2005-2006 Florian Schulze, Colin Phipps, Neil Stevens, Andrey Budko
	Copyright(C) 2017 Fabian Greffrath

	This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License
	as published by the Free Software Foundation; either version 2 of the License, or (at your option) any later version.

	This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

	DESCRIPTION:
		Support MUSINFO lump (dynamic music changing)
\**********************************************************************************************************************************************/
#pragma once

#include "../../derma/stdafx.h"

//#include <string>

#include "../../derma/common.h"

#include "p_mobj.h"

constexpr size_t MAX_MUS_ENTRIES{65};// 0 to 64 inclusive

struct musinfo_t
{
	MapObject* mapthing;
	MapObject* lastmapthing;
	TimeType tics;
	int current_item;
	int items[MAX_MUS_ENTRIES];
	bool from_savegame;
};

extern musinfo_t musinfo;

extern void S_ParseMusInfo(::std::string mapid);
extern void T_MusInfo();
