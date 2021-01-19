/**********************************************************************************************************************************************\
	Copyright(C) 1993-1996 Id Software, Inc.
	Copyright(C) 2005-2014 Simon Howard

	This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License
	as published by the Free Software Foundation; either version 2 of the License, or (at your option) any later version.

	This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
\**********************************************************************************************************************************************/
#pragma once

#include "../../derma/common.h"

#include "../d_event.h"
#include "../d_ticcmd.h"

#include "doomdef.h"

void G_DeathMatchSpawnPlayer(int playernum);

void G_InitNew(SkillType skill, int episode, int map);

// Can be called by the startup code or M_Responder.
// A normal game starts at map 1, but a warp test can start elsewhere
void G_DeferedInitNew(SkillType skill, int episode, int map);

void G_DeferedPlayDemo(::std::string demo);

// Can be called by the startup code or M_Responder, calls P_SetupLevel or W_EnterWorld.
void G_LoadGame(::std::string name);

void G_DoLoadGame();

// Called by M_Responder.
void G_SaveGame(int slot, ::std::string description);

// Only called by startup code.
void G_RecordDemo(::std::string name);

void G_BeginRecording();

void G_PlayDemo(::std::string name);
void G_TimeDemo(::std::string name);
bool G_CheckDemoStatus();

void G_ExitLevel();
void G_SecretExitLevel();

void G_WorldDone();

// Read current data from inputs and build a player movement command.
void G_BuildTiccmd(ticcmd_t* cmd, int maketic);

void G_Ticker();
bool G_Responder(EventType* ev);

void G_ScreenShot();

void G_DrawMouseSpeedBox();
int G_VanillaVersionCode();

extern int vanilla_savegame_limit;
extern int vanilla_demo_limit;
