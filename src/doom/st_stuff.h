/**********************************************************************************************************************************************\
	Copyright(C) 1993-1996 Id Software, Inc.
	Copyright(C) 2005-2014 Simon Howard

	This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License
	as published by the Free Software Foundation; either version 2 of the License, or (at your option) any later version.

	This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

	DESCRIPTION:
		Status bar code. Does the face/direction indicator animatin. Does palette indicators as well (red pain/berserk, bright pickup)
\**********************************************************************************************************************************************/
#pragma once

#include "../../derma/common.h"

#include "doomtype.h"
#include "d_event.h"
//#include "m_cheat.h"

#include "../i_video.h"

// Size of statusbar.
// Now sensitive for scaling.
constexpr size_t ST_HEIGHT{32};
constexpr size_t ST_WIDTH{ORIGWIDTH};
constexpr size_t ST_Y{(ORIGHEIGHT - ST_HEIGHT)};

constexpr size_t CRISPY_HUD{12};

// Demo Timer widget
extern void ST_DrawDemoTimer(const TimeType time);
extern TimeType defdemotics;
extern TimeType deftotaldemotics;

//
// STATUS BAR
//

// Called by main loop.
bool ST_Responder(EventType* ev);

// Called by main loop.
void ST_Ticker();

// Called by main loop.
void ST_Drawer(bool fullscreen, bool refresh);

// Called when the console player is spawned on each level.
void ST_Start();

// Called by startup code.
void ST_Init();

// forcefully initialize the status bar backing screen
extern void ST_refreshBackground(bool force);

// States for status bar code.
enum class st_stateenum_t
{
	AutomapState,
	FirstPersonState
};

// States for the chat code.
enum class st_chatstateenum_t
{
	StartChatState,
	WaitDestState,
	GetChatState
};

enum class hudcolor_t
{
	hudcolor_ammo,
	hudcolor_health,
	hudcolor_frags,
	hudcolor_armor
};

struct cheatseq_t;

extern pixel_t* st_backing_screen;
extern cheatseq_t cheat_mus;
extern cheatseq_t cheat_god;
extern cheatseq_t cheat_ammo;
extern cheatseq_t cheat_ammonokey;
extern cheatseq_t cheat_noclip;
extern cheatseq_t cheat_commercial_noclip;
extern cheatseq_t cheat_powerup[8]; // idbehold0
extern cheatseq_t cheat_choppers;
extern cheatseq_t cheat_clev;
extern cheatseq_t cheat_mypos;
