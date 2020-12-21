/**********************************************************************************************************************************************\
	Copyright(C) 1993-1996 Id Software, Inc.
	Copyright(C) 1993-2008 Raven Software
	Copyright(C) 2005-2014 Simon Howard

	This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License
	as published by the Free Software Foundation; either version 2 of the License, or (at your option) any later version.

	This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
\**********************************************************************************************************************************************/
#pragma once


#ifndef STSTART_H
#define STSTART_H

#include "doomtype.h"

// HEADER FILES ------------------------------------------------------------

// MACROS ------------------------------------------------------------------

// TYPES -------------------------------------------------------------------

// PUBLIC FUNCTION PROTOTYPES ----------------------------------------------
extern void ST_Init();
extern void ST_Done();
extern void ST_Message(const char *message, ...) PRINTF_ATTR(1, 2);
extern void ST_RealMessage(const char *message, ...) PRINTF_ATTR(1, 2);
extern void ST_Progress();
extern void ST_NetProgress();
extern void ST_NetDone();

// PUBLIC DATA DECLARATIONS ------------------------------------------------

extern int graphical_startup;

#endif
