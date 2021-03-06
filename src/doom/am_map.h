/**********************************************************************************************************************************************\
	Copyright(C) 1993-1996 Id Software, Inc.
	Copyright(C) 2005-2014 Simon Howard

	This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License
	as published by the Free Software Foundation; either version 2 of the License, or (at your option) any later version.

	This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

	DESCRIPTION:
		AutoMap module.
\**********************************************************************************************************************************************/
#pragma once

#include "../../derma/common.h"

#include "../d_event.h"
#include "../m_cheat.h"
#include "../m_fixed.h"

// Used by ST StatusBar stuff.
constexpr size_t AM_MSGHEADER{(('a' << 24) + ('m' << 16))};
constexpr size_t AM_MSGENTERED{(AM_MSGHEADER | ('e' << 8))};
constexpr size_t AM_MSGEXITED{(AM_MSGHEADER | ('x' << 8))};

// Called by main loop.
bool AM_Responder(EventType* ev);

// Called by main loop.
void AM_Ticker();

// Called by main loop,
// called instead of view drawer if automap active.
void AM_Drawer();

// Called to force the automap to quit
// if the level is completed while it is up.
void AM_Stop();

cheatseq_t cheat_amap = CHEAT("iddt", 0);

struct fpoint_t
{
	int x;
	int y;
};

struct fline_t
{
	fpoint_t a;
	fpoint_t b;
};

struct mpoint_t
{
	int64_t x;
	int64_t y;
};

struct mline_t
{
	mpoint_t a;
	mpoint_t b;
};

struct islope_t
{
	fixed_t slp;
	fixed_t islp;
};

enum class keycolor_t
{
	no_key,
	red_key,
	yellow_key,
	blue_key
};
