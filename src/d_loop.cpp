/**********************************************************************************************************************************************\
	Copyright(C) 1993-1996 Id Software, Inc.
	Copyright(C) 2005-2014 Simon Howard

	This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License
	as published by the Free Software Foundation; either version 2 of the License, or (at your option) any later version.

	This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

	DESCRIPTION:
		Main loop code.
\**********************************************************************************************************************************************/

#include "d_event.h"
#include "d_loop.h"
#include "d_ticcmd.h"

#include "i_system.h"
#include "i_timer.h"
#include "i_video.h"

#include "m_argv.h"
#include "m_fixed.h"

#include "net_client.h"
#include "net_gui.h"
#include "net_io.h"
#include "net_query.h"
#include "net_server.h"
#include "net_sdl.h"
#include "net_loop.h"

#include "crispy.h"

// TODO: Move nonvanilla demo functions into a dedicated file.
#include "m_misc.h"
#include "w_wad.h"

// The complete set of data for a particular tic.
struct ticcmd_set_t
{
	ticcmd_t cmds[NET_MAXPLAYERS];
	bool ingame[NET_MAXPLAYERS];
};

// Maximum time that we wait in TryRunTics() for netgame data to be received before we bail out and render a frame anyway.
// Vanilla Doom used 20 for this value, but we use a smaller value instead for better responsiveness of the menu when we're stuck.
constexpr size_t MAX_NETGAME_STALL_TICS{5};

// gametic is the tic about to (or currently being) run maketic is the tic that hasn't had control made for it yet recvtic is the latest tic
// received from the server. a gametic cannot be run until ticcmds are received for it from all players.
static ticcmd_set_t ticdata[BACKUPTICS];

// The index of the next tic to be made (with a call to BuildTiccmd).
static TimeType maketic;

// The number of complete tics received from the server so far.
static TimeType recvtic;

// The number of tics that have been run (using RunTic) so far.
TimeType gametic;
TimeType oldleveltime; // [crispy] check if leveltime keeps tickin'

// When set to true, a single tic is run each time TryRunTics() is called. This is used for -timedemo mode.
bool singletics{false};

// Index of the local player.
static int localplayer;

// Used for original sync code.
static TimeType skiptics{0};

// Reduce the bandwidth needed by sampling game input less and transmitting less. If ticdup is 2, sample half normal, 3 = one third normal, etc.
TimeType ticdup;

// Amount to offset the timer for game sync.
fixed_t offsetms;

// Use new client syncronisation code
static bool new_sync{true};

// Callback functions for loop code.
static loop_interface_t* loop_interface{nullptr};

// Current players in the multiplayer game.
// This is distinct from playeringame[] used by the game code, which may modify playeringame[] when playing back multiplayer demos.
static bool local_playeringame[NET_MAXPLAYERS];

// Requested player class "sent" to the server on connect.
// If we are only doing a single player game then this needs to be remembered and saved in the game settings.
static int player_class;

// 35 fps clock adjusted by offsetms milliseconds
static int GetAdjustedTime()
{
	auto time_ms{I_GetTimeMS()};

	if (new_sync)
	{
		// Use the adjustments from net_client.c only if we are using the new sync mode.
		time_ms += (offsetms / FRACUNIT);
	}

	return (time_ms * TICRATE) / 1000;
}

static bool BuildNewTic()
{
	auto gameticdiv{gametic/ticdup};

	I_StartTic();
	loop_interface->ProcessEvents();

	// Always run the menu
	loop_interface->RunMenu();

	if (drone)
	{
		// In drone mode, do not generate any ticcmds.
		{
			return false;
		}
	}

	if (new_sync)
	{
		// If playing single player, do not allow tics to buffer up very far
		if (!net_client_connected && maketic - gameticdiv > 2)
		{
			return false;
		}

		// Never go more than ~200ms ahead
		if (maketic - gameticdiv > 8)
		{
			return false;
		}
	}
	else
	{
		if (maketic - gameticdiv >= 5)
		{
			return false;
		}
	}

	ticcmd_t cmd;

	//printf ("mk:%i ",maketic);
	memset(&cmd, 0, sizeof(ticcmd_t));
	loop_interface->BuildTiccmd(&cmd, maketic);

	if (net_client_connected)
	{
		NET_CL_SendTiccmd(&cmd, maketic);
	}

	ticdata[maketic % BACKUPTICS].cmds[localplayer] = cmd;
	ticdata[maketic % BACKUPTICS].ingame[localplayer] = true;

	++maketic;

	return true;
}

// Builds ticcmds for console player, sends out a packet
TimeType lasttime;

void NetUpdate()
{
	// If we are running with singletics (timing a demo), this is all done separately.
	if (singletics)
	{
		return;
	}

	// Run network subsystems
	NET_CL_Run();
	NET_SV_Run();

	// check time
	auto nowtime{GetAdjustedTime() / ticdup};
	auto newtics{nowtime - lasttime};

	lasttime = nowtime;

	if (skiptics <= newtics)
	{
		newtics -= skiptics;
		skiptics = 0;
	}
	else
	{
		skiptics -= newtics;
		newtics = 0;
	}

	// build new ticcmds for console player

	for (size_t i{0}; i < newtics; ++i)
	{
		if (!BuildNewTic())
		{
			break;
		}
	}
}

static void D_Disconnected()
{
	// In drone mode, the game cannot continue once disconnected.
	if (drone)
	{
		I_Error("Disconnected from server in drone mode.");
	}

	// disconnected from server
	printf("Disconnected from server.\n");
}

// Invoked by the network engine when a complete set of ticcmds is available.
void D_ReceiveTic(ticcmd_t* ticcmds, bool* players_mask)
{
	// Disconnected from server?
	if (!ticcmds && !players_mask)
	{
		D_Disconnected();
		return;
	}

	for (size_t i{0}; i < NET_MAXPLAYERS; ++i)
	{
		if (!drone && i == localplayer)
		{
			// This is us. Don't overwrite it.
		}
		else
		{
			ticdata[recvtic % BACKUPTICS].cmds[i] = ticcmds[i];
			ticdata[recvtic % BACKUPTICS].ingame[i] = players_mask[i];
		}
	}

	++recvtic;
}

// Called after the screen is set but before the game starts running.
void D_StartGameLoop()
{
	lasttime = GetAdjustedTime() / ticdup;
}

// Block until the game start message is received from the server.
static void BlockUntilStart(net_gamesettings* settings, netgame_startup_callback_t callback)
{
	while (!NET_CL_GetSettings(settings))
	{
		NET_CL_Run();
		NET_SV_Run();

		if (!net_client_connected)
		{
			I_Error("Lost connection to server");
		}

		if (callback && !callback(net_client_wait_data.ready_players, net_client_wait_data.num_players))
		{
			I_Error("Netgame startup aborted.");
		}

		I_Sleep(100);
	}
}

void D_StartNetGame(net_gamesettings* settings, netgame_startup_callback_t callback)
{
	offsetms = 0;
	recvtic = 0;

	settings->consoleplayer = 0;
	settings->num_players = 1;
	settings->player_classes[0] = player_class;

	// Use original network client sync code rather than the improved sync code.
	settings->new_sync = !M_ParmExists("-oldsync");

	// Send n extra tics in every packet as insurance against dropped packets.
	if (auto i{M_CheckParmWithArgs("-extratics", 1)}; i > 0)
	{
		settings->extratics = atoi(myargv[i+1]);
	}
	else
	{
		settings->extratics = 1;
	}

	// Reduce the resolution of the game by a factor of n, reducing the amount of network bandwidth needed.
	if (auto i{M_CheckParmWithArgs("-dup", 1)}; i > 0)
	{
		settings->ticdup = atoi(myargv[i+1]);
	}
	else
	{
		settings->ticdup = 1;
	}

	if (net_client_connected)
	{
		// Send our game settings and block until game start is received from the server.
		NET_CL_StartGame(settings);
		BlockUntilStart(settings, callback);

		// Read the game settings that were received.
		NET_CL_GetSettings(settings);
	}

	if (drone)
	{
		settings->consoleplayer = 0;
	}

	// Set the local player and playeringame[] values.
	localplayer = settings->consoleplayer;

	for (size_t i{0}; i < NET_MAXPLAYERS; ++i)
	{
		local_playeringame[i] = i < settings->num_players;
	}

	// Copy settings to global variables.
	ticdup = settings->ticdup;
	new_sync = settings->new_sync;

	if (ticdup < 1)
	{
		I_Error("D_StartNetGame: invalid ticdup value (%d)", ticdup);
	}

	// TODO: Message disabled until we fix new_sync.
	//if (!new_sync)
	//{
	//	printf("Syncing netgames like Vanilla Doom.\n");
	//}
}

bool D_InitNetGame(net_connect_data* connect_data)
{
	bool result{false};
	net_addr_t* addr = nullptr;

	// Call D_QuitNetGame on exit:
	I_AtExit(D_QuitNetGame, true);

	player_class = connect_data->player_class;

	// Start a multiplayer server, listening for connections.
	if (M_CheckParm("-server") > 0 || M_CheckParm("-privateserver") > 0)
	{
		NET_SV_Init();
		NET_SV_AddModule(&net_loop_server_module);
		NET_SV_AddModule(&net_sdl_module);
		NET_SV_RegisterWithMaster();

		net_loop_client_module.InitClient();
		addr = net_loop_client_module.ResolveAddress(nullptr);
		NET_ReferenceAddress(addr);
	}
	else
	{
		// Automatically search the local LAN for a multiplayer server and join it.
		if (M_CheckParm("-autojoin") > 0)
		{
			addr = NET_FindLANServer();

			if (!addr)
			{
				I_Error("No server found on local LAN");
			}
		}

		// Connect to a multiplayer server running on the given address.
		if (auto i{M_CheckParmWithArgs("-connect", 1)}; i > 0)
		{
			net_sdl_module.InitClient();
			addr = net_sdl_module.ResolveAddress(myargv[i+1]);
			NET_ReferenceAddress(addr);

			if (!addr)
			{
				I_Error("Unable to resolve '%s'\n", myargv[i+1]);
			}
		}
	}

	if (addr)
	{
		if (M_CheckParm("-drone") > 0)
		{
			connect_data->drone = true;
		}

		if (!NET_CL_Connect(addr, connect_data))
		{
			I_Error("D_InitNetGame: Failed to connect to %s:\n%s\n", NET_AddrToString(addr), net_client_reject_reason);
		}

		printf("D_InitNetGame: Connected to %s\n", NET_AddrToString(addr));
		NET_ReleaseAddress(addr);

		// Wait for launch message received from server.
		NET_WaitForLaunch();

		result = true;
	}

	return result;
}

// Called before quitting to leave a net game without hanging the other players
void D_QuitNetGame()
{
	NET_SV_Shutdown();
	NET_CL_Disconnect();
}

static int GetLowTic()
{
	auto lowtic{maketic};

	if (net_client_connected)
	{
		if (drone || recvtic < lowtic)
		{
			lowtic = recvtic;
		}
	}

	return lowtic;
}

static int frameon;
static int frameskip[4];
static TimeType oldnettics;

static void OldNetSync()
{
	++frameon;

	int keyplayer{-1};

	// ideally maketic should be 1 - 3 tics above lowtic if we are consistantly slower, speed up time
	for (size_t i{0}; i < NET_MAXPLAYERS; ++i)
	{
		if (local_playeringame[i])
		{
			keyplayer = i;
			break;
		}
	}

	if (keyplayer < 0)
	{
		// If there are no players, we can never advance anyway
		return;
	}

	if (localplayer == keyplayer)
	{
		// the key player does not adapt
	}
	else
	{
		if (maketic <= recvtic)
		{
			--lasttime;
			//printf ("-");
		}

		frameskip[frameon & 3] = oldnettics > recvtic;
		oldnettics = maketic;

		if (frameskip[0] && frameskip[1] && frameskip[2] && frameskip[3])
		{
			skiptics = 1;
			//printf ("+");
		}
	}
}

// Returns true if there are players in the game:
static bool PlayersInGame()
{
	bool result{false};

	// If we are connected to a server, check if there are any players in the game.
	if (net_client_connected)
	{
		for (size_t i{0}; i < NET_MAXPLAYERS; ++i)
		{
			result = result || local_playeringame[i];
		}
	}

	// Whether single or multi-player, unless we are running as a drone, we are in the game.
	if (!drone)
	{
		result = true;
	}

	return result;
}

// When using ticdup, certain values must be cleared out when running the duplicate ticcmds.
static void TicdupSquash(ticcmd_set_t* set)
{
	for (size_t i{0}; i < NET_MAXPLAYERS; ++i)
	{
		auto& cmd{set->cmds[i]};
		cmd.chatchar = 0;
		if (_underlying_value(cmd.buttons) & _underlying_value(buttoncode::BT_SPECIAL))
		{
			cmd.buttons = buttoncode::BT_NONE;
		}
	}
}

// When running in single player mode, clear all the ingame[] array except the local player.
static void SinglePlayerClear(ticcmd_set_t* set)
{
	for (size_t i{0}; i < NET_MAXPLAYERS; ++i)
	{
		if (i != localplayer)
		{
			set->ingame[i] = false;
		}
	}
}

void TryRunTics()
{
	static TimeType oldentertics;

	// If we've uncapped the framerate and there are no tics to run, return early instead of waiting around.
	// FIXME WHAT THE FUCK why would anyone macro this?
	extern TimeType leveltime;
	#define return_early (crispy->uncapped && counts == 0 && leveltime > oldleveltime && screenvisible)

	// get real tics
	auto entertic{I_GetTime() / ticdup};
	auto realtics{entertic - oldentertics};
	oldentertics = entertic;

	// in singletics mode, run a single tic every time this function is called.
	if (singletics)
	{
		BuildNewTic();
	}
	else
	{
		NetUpdate();
	}

	auto lowtic{GetLowTic()};
	auto availabletics{lowtic - gametic/ticdup};

	int counts;
	// decide how many tics to run
	if (new_sync)
	{
		counts = availabletics;

		// If we've uncapped the framerate and there are no tics to run, return early instead of waiting around.
		if (return_early)
		{
			return;
		}
	}
	else
	{
		// decide how many tics to run
		if (realtics < availabletics-1)
		{
			counts = realtics+1;
		}
		else if (realtics < availabletics)
		{
			counts = realtics;
		}
		else
		{
			counts = availabletics;
		}

		// If we've uncapped the framerate and there are no tics to run, return early instead of waiting around.
		if (return_early)
		{
			return;
		}

		if (counts < 1)
		{
			counts = 1;
		}

		if (net_client_connected)
		{
			OldNetSync();
		}
	}

	if (counts < 1)
	{
		counts = 1;
	}

	// wait for new tics if needed
	while (!PlayersInGame() || lowtic < gametic/ticdup + counts)
	{
		NetUpdate();

		lowtic = GetLowTic();

		if (lowtic < gametic/ticdup)
		{
			I_Error("TryRunTics: lowtic < gametic");
		}

		// Still no tics to run? Sleep until some are available.
		if (lowtic < gametic/ticdup + counts)
		{
			// If we're in a netgame, we might spin forever waiting for new network data to be received.
			// So don't stay in here forever - give the menu a chance to work.
			if (I_GetTime() / ticdup - entertic >= MAX_NETGAME_STALL_TICS)
			{
				return;
			}

			I_Sleep(1);
		}
	}

	// FIXME check scope for postfix decr
	// run the count * ticdup dics
	while (counts--)
	{
		if (!PlayersInGame())
		{
			return;
		}

		auto set{&ticdata[(gametic / ticdup) % BACKUPTICS]};

		if (!net_client_connected)
		{
			SinglePlayerClear(set);
		}

		for (size_t i{0}; i < ticdup; ++i)
		{
			if (gametic/ticdup > lowtic)
			{
				I_Error("gametic>lowtic");
			}

			memcpy(local_playeringame, set->ingame, sizeof(local_playeringame));

			loop_interface->RunTic(set->cmds, set->ingame);
			++gametic;

			// modify command for duplicated tics
			TicdupSquash(set);
		}

		NetUpdate();	// check for new console commands
	}
}

void D_RegisterLoopCallbacks(loop_interface_t* i)
{
	loop_interface = i;
}

static bool StrictDemos()
{
	// When recording or playing back demos, disable any extensions of the vanilla demo format - record demos as vanilla would do,
	// and play back demos as vanilla would do.
	return M_ParmExists("-strictdemos");
}

// If the provided conditional value is true, we're trying to record a demo file that will include a non-vanilla extension. The function
// will return true if the conditional is true and it's allowed to use this extension (no extensions are allowed if -strictdemos is given
// on the command line). A warning is shown on the console using the provided string describing the non-vanilla expansion.
bool D_NonVanillaRecord(bool conditional, std::string feature)
{
	if (!conditional || StrictDemos())
	{
		return false;
	}

	printf("Warning: Recording a demo file with a non-vanilla extension (%s). Use -strictdemos to disable this extension.\n", feature);

	return true;
}

// Returns true if the given lump number corresponds to data from a .lmp file, as opposed to a WAD.
static bool IsDemoFile(int lumpnum)
{
	auto lower{std::string(lumpinfo[lumpnum]->wad_file->path)};
	// FIXME
	//M_ForceLowercase(lower);

	return M_StringEndsWith(*lower, ".lmp");
}

// If the provided conditional value is true, we're trying to play back a demo that includes a non-vanilla extension.
// We return true if the conditional is true and it's allowed to use this extension, checking that:
// - The -strictdemos command line argument is not provided.
// - The given lumpnum identifying the demo to play back identifies a demo that comes from a .lmp file, not a .wad file.
// - Before proceeding, a warning is shown to the user on the console.
bool D_NonVanillaPlayback(bool conditional, int lumpnum, std::string feature)
{
	if (!conditional || StrictDemos())
	{
		return false;
	}

	if (!IsDemoFile(lumpnum))
	{
		printf("Warning: WAD contains demo with a non-vanilla extension (%s)\n", feature);
		return false;
	}

	printf("Warning: Playing back a demo file with a non-vanilla extension (%s). Use -strictdemos to disable this extension.\n", feature);

	return true;
}
