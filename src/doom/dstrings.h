/**********************************************************************************************************************************************\
	Copyright(C) 1993-1996 Id Software, Inc.
	Copyright(C) 2005-2014 Simon Howard

	This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License
	as published by the Free Software Foundation; either version 2 of the License, or (at your option) any later version.

	This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

	DESCRIPTION:
		DOOM strings, by language.
\**********************************************************************************************************************************************/
#pragma once

#include "../../derma/stdafx.h".h"

//#include <string>

#include "../../derma/common.h"

// All important printed strings.
#include "d_englsh.h"

// Misc. other strings.
#define SAVEGAMENAME "doomsav"

// QuitDOOM messages
// 8 per each game type
constexpr size_t NUM_QUITMESSAGES{8};

extern ::std::string doom1_endmsg[];
extern ::std::string doom2_endmsg[];
