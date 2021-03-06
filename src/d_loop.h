/**********************************************************************************************************************************************\
	Copyright(C) 1993-1996 Id Software, Inc.
	Copyright(C) 2005-2014 Simon Howard

	This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License
	as published by the Free Software Foundation; either version 2 of the License, or (at your option) any later version.

	This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

	DESCRIPTION:
		Main loop stuff.
\**********************************************************************************************************************************************/
#pragma once

#include "../derma/common.h"

#include "network/net_defs.h"

// Callback function invoked while waiting for the netgame to start.
// The callback is invoked when new players are ready. The callback should return true, or return false to abort startup.
typedef bool (*netgame_startup_callback_t)(int ready_players, int num_players);

struct loop_interface_t
{
	// Read events from the event queue, and process them.
	void (*ProcessEvents)();

	// Given the current input state, fill in the fields of the specified ticcmd_t structure with data for a new tic.
	void (*BuildTiccmd)(ticcmd_t* cmd, TimeType maketic);

	// Advance the game forward one tic, using the specified player input.
	void (*RunTic)(ticcmd_t* cmds, ::std::span<bool> ingame);

	// Run the menu (runs independently of the game).
	void (*RunMenu)();
};

// Register callback functions for the main loop code to use.
void D_RegisterLoopCallbacks(loop_interface_t* i);

// Create any new ticcmds and broadcast to other players.
void NetUpdate();

// Broadcasts special packets to other players
// to notify of game exit
void D_QuitNetGame();

// how many ticks to run?
void TryRunTics();

// Called at start of game loop to initialize timers
void D_StartGameLoop();

// Initialize networking code and connect to server.
bool D_InitNetGame(net_connect_data* connect_data);

// Start game with specified settings. The structure will be updated with the actual settings for the game.
void D_StartNetGame(net_gamesettings* settings, netgame_startup_callback_t callback);

extern bool singletics;
extern TimeType gametic;
extern TimeType ticdup;
extern TimeType oldleveltime;	// check if leveltime keeps tickin'

// Check if it is permitted to record a demo with a non-vanilla feature.
bool D_NonVanillaRecord(bool conditional, ::std::string feature);

// Check if it is permitted to play back a demo with a non-vanilla feature.
bool D_NonVanillaPlayback(bool conditional, int lumpnum, ::std::string feature);
