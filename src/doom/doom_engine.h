/**********************************************************************************************************************************************\
	Copyright(C) 1993-1996 Id Software, Inc.
	Copyright(C) 2005-2014 Simon Howard

	This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License
	as published by the Free Software Foundation; either version 2 of the License, or (at your option) any later version.

	This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

	DESCRIPTION:
		DOOM main program (D_DoomMain) and game loop (D_DoomLoop),plus functions to determine game mode (shareware, registered),
		parse command line parameters, configure game parameters (turbo), and call the startup functions.
\**********************************************************************************************************************************************/
#pragma once

#include "doom_includes.h"
#include "doom.h"

namespace cudadoom
{

void D_ConnectNetGame();
void D_CheckNetGame();

static class DoomEngine
{
	//int p;
	char file[256];
	char demolumpname[9];
	int numiwadlumps;

	void CheckParams()
	{
		// Start a dedicated server, routing packets but not participating in the game itself.
		if (M_CheckParm("-dedicated") > 0)
		{
			printf("Dedicated server mode.\n");
			NET_DedicatedServer();
			// Never returns
		}

		// Query the Internet master server for a global list of active servers.
		if (M_CheckParm("-search"))
		{
			NET_MasterQuery();
			exit(0);
		}

		// Query the status of the server running on the given IP address.
		

		if (auto p = M_CheckParmWithArgs("-query", 1); p)
		{
			NET_QueryAddress(myargv[p + 1]);
			exit(0);
		}

		// Search the local LAN for running servers.
		if (M_CheckParm("-localsearch"))
		{
			NET_LANQuery();
			exit(0);
		}

		// Disable monsters.
		nomonsters = M_CheckParm("-nomonsters");

		// Monsters respawn after being killed.

		respawnparm = M_CheckParm("-respawn");

		// Monsters move faster.
		fastparm = M_CheckParm("-fast");

		// Developer mode. F1 saves a screenshot in the current working directory.
		devparm = M_CheckParm("-devparm");

		I_DisplayFPSDots(devparm);

		// Start a deathmatch game.
		if (M_CheckParm("-deathmatch")){
			deathmatch = 1;}

		// Start a deathmatch 2.0 game. Weapons do not stay in place and all items respawn after 30 seconds.
		if (M_CheckParm("-altdeath")){
			deathmatch = 2;}

		// Start a deathmatch 3.0 game. Weapons stay in place and all items respawn after 30 seconds.
		if (M_CheckParm("-dm3")){
			deathmatch = 3;}

		if (devparm){
			DEH_printf(D_DEVSTR);}

		// find which dir to use for config files
	#ifdef _WIN32
		// Save configuration data and savegames in c:\doomdata, allowing play from CD.
		if (M_ParmExists("-cdrom"))
		{
			printf(D_CDROM);

			M_SetConfigDir("c:\\doomdata\\");
		}
		else
	#endif
		{
			// Auto-detect the configuration dir.
			M_SetConfigDir(NULL);
		}

		// Turbo mode. The player's speed is multiplied by x%. If unspecified, x defaults to 200. Values are rounded up to 10 and down to 400.
		if (auto p = M_CheckParm("-turbo"); p)
		{
			int scale = 200;
			int forwardmove[2];
			int sidemove[2];

			if (p < myargc - 1)
			{
				scale = atoi(myargv[p + 1]);
			}
			if (scale < 10)
			{
				scale = 10;
			}
			if (scale > 400)
			{
				scale = 400;
			}

			DEH_printf("turbo scale: %i%%\n", scale);
			forwardmove[0] = forwardmove[0] * scale / 100;
			forwardmove[1] = forwardmove[1] * scale / 100;
			sidemove[0] = sidemove[0] * scale / 100;
			sidemove[1] = sidemove[1] * scale / 100;
		}

		// Disable automatic loading of Dehacked patches for certain IWAD files.
		if (!M_ParmExists("-nodeh"))
		{
			// Some IWADs have dehacked patches that need to be loaded for them to be played properly.
			LoadIwadDeh();
		}

		// Automatic pistol start when advancing from one level to the next. At the
		// beginning of each level, the player's health is reset to 100, their
		// armor to 0 and their inventory is reduced to the following: pistol,
		// fists and 50 bullets. This option is not allowed when recording a demo,
		// playing back a demo or when starting a network game.
		crispy->pistolstart = M_ParmExists("-pistolstart");

		// Disable auto-loading of .wad and .deh files.
		if (!M_ParmExists("-noautoload") && gamemode != GameMode::shareware)
		{
			::std::string autoload_dir;

			// common auto-loaded files for all Doom flavors
			if (gamemission < GameMission::pack_chex)
			{
				autoload_dir = M_GetAutoloadDir("doom-all");
				DEH_AutoLoadPatches(autoload_dir);
				W_AutoLoadWADs(autoload_dir);
				free(autoload_dir);
			}

			// auto-loaded files per IWAD
			autoload_dir = M_GetAutoloadDir(D_SaveGameIWADName(gamemission, gamevariant));
			DEH_AutoLoadPatches(autoload_dir);
			W_AutoLoadWADs(autoload_dir);
			free(autoload_dir);
		}

		// Load Dehacked patches specified on the command line with -deh.
		// Note that there's a very careful and deliberate ordering to how. Dehacked patches are loaded. The order we use is:
		// 1. IWAD dehacked patches.
		// 2. Command line dehacked patches specified with -deh.
		// 3. PWAD dehacked patches in DEHACKED lumps.
		DEH_ParseCommandLine();

		// Load PWAD files.
		modifiedgame = W_ParseCommandLine();

		// experimental feature: in conjunction with -merge <files> merges PWADs into the main IWAD and writes the merged data into <file>
		if (auto p = M_CheckParm("-mergedump"); p)
		{
			p = M_CheckParmWithArgs("-mergedump", 1);

			if (p)
			{
				int merged;

				if (M_StringEndsWith(myargv[p + 1], ".wad"))
				{
					M_StringCopy(file, myargv[p + 1], sizeof(file));
				}
				else
				{
					DEH_snprintf(file, sizeof(file), "%s.wad", myargv[p + 1]);
				}

				merged = W_MergeDump(file);
				I_Error("W_MergeDump: Merged %d lumps into file '%s'.", merged, file);
			}
			else
			{
				I_Error("W_MergeDump: The '-mergedump' parameter requires an argument.");
			}
		}

		// experimental feature: dump lump data into a new LMP file <file>
		if (auto p = M_CheckParm("-lumpdump"); p)
		{
			p = M_CheckParmWithArgs("-lumpdump", 1);

			if (p)
			{
				int dumped;

				M_StringCopy(file, myargv[p + 1], sizeof(file));

				dumped = W_LumpDump(file);

				if (dumped < 0)
				{
					I_Error("W_LumpDump: Failed to write lump '%s'.", file);
				}
				else
				{
					I_Error("W_LumpDump: Dumped lump into file '%s.lmp'.", file);
				}
			}
			else
			{
				I_Error("W_LumpDump: The '-lumpdump' parameter requires an argument.");
			}
		}

		{
			// Play back the demo named demo.lmp.
			auto p = M_CheckParmWithArgs("-playdemo", 1);

			if (!p)
			{
				// Play back the demo named demo.lmp, determining the framerate of the screen.
				p = M_CheckParmWithArgs("-timedemo", 1);

			}

			if (p)
			{
				::std::string uc_filename = strdup(myargv[p + 1]);
				M_ForceUppercase(uc_filename);

				// With Vanilla you have to specify the file without extension, but make that optional.
				if (M_StringEndsWith(uc_filename, ".LMP"))
				{
					M_StringCopy(file, myargv[p + 1], sizeof(file));
				}
				else
				{
					DEH_snprintf(file, sizeof(file), "%s.lmp", myargv[p + 1]);
				}

				free(uc_filename);

				if (D_AddFile(file))
				{
					int i;
					// check if the demo file name gets truncated to a lump name that is already present
					if ((i = W_CheckNumForNameFromTo(lumpinfo[numlumps - 1]->name, numlumps - 2, 0)) != -1)
					{
						printf("Demo lump name collision detected with lump \'%.8s\' from %s.\n", lumpinfo[i]->name, W_WadNameForLump(lumpinfo[i]));
						// the DEMO1 lump is almost certainly always a demo lump
						M_StringCopy(lumpinfo[numlumps - 1]->name, "DEMO1", 6);
					}

					M_StringCopy(demolumpname, lumpinfo[numlumps - 1]->name, sizeof(demolumpname));
				}
				else
				{
					// If file failed to load, still continue trying to play the demo in the same way as Vanilla Doom.
					// This makes tricks like "-playdemo demo1" possible.
					M_StringCopy(demolumpname, myargv[p + 1], sizeof(demolumpname));
				}

				printf("Playing demo %s.\n", file);
			}
		}

		// allow overriding of special-casing
		if (!M_ParmExists("-noautoload") && gamemode != GameMode::shareware)
		{
			if (gamemode == GameMode::retail && gameversion == GameVersion::exe_ultimate && gamevariant != GameVariant::freedoom)
			{
				D_LoadSigilWad();
			}

			if (gamemission == GameMission::doom2)
			{
				D_LoadNerveWad();
				D_LoadMasterlevelsWad();
			}
		}

		// Load DEHACKED lumps from WAD files - but only if we give the right command line parameter.

		// Load Dehacked patches from DEHACKED lumps contained in one of the loaded PWAD files.

		// load DEHACKED lumps by default, but allow overriding
		if (!M_ParmExists("-nodehlump") && !M_ParmExists("-nodeh"))
		{
			int i;
			int loaded = 0;

			for (i = numiwadlumps; i < numlumps; ++i)
			{
				if (!strncmp(lumpinfo[i]->name, "DEHACKED", 8))
				{
					DEH_LoadLump(i, true, true); // allow long, allow error
					++loaded;
				}
			}

			printf(" loaded %i DEHACKED lumps from PWAD files.\n", loaded);
		}

		// Set the game skill, 1-5 (1: easiest, 5: hardest). A skill of 0 disables all monsters.
		if (auto p = M_CheckParmWithArgs("-skill", 1); p)
		{
			startskill = myargv[p + 1][0] - '1';
			autostart = true;
		}

		// Start playing on episode n (1-4)
		if (auto p = M_CheckParmWithArgs("-episode", 1); p)
		{
			startepisode = myargv[p + 1][0] - '0';
			startmap = 1;
			autostart = true;
		}

		timelimit = 0;

		// For multiplayer games: exit each level after n minutes.
		if (auto p = M_CheckParmWithArgs("-timer", 1); p)
		{
			timelimit = atoi(myargv[p + 1]);
		}

		// Austin Virtual Gaming: end levels after 20 minutes.

		if (auto p = M_CheckParm("-avg"); p)
		{
			timelimit = 20;
		}

		// Start a game immediately, warping to ExMy (Doom 1) or MAPxy (Doom 2)
		if (auto p = M_CheckParmWithArgs("-warp", 1); p)
		{
			if (gamemode == GameMode::commercial){
				startmap = atoi(myargv[p + 1]);}
			else
			{
				startepisode = myargv[p + 1][0] - '0';

				// only if second argument is not another option
				if (p + 2 < myargc && myargv[p + 2][0] != '-')
				{
					startmap = myargv[p + 2][0] - '0';
				}
				else
				{
					// allow second digit without space in between for Doom 1
					startmap = myargv[p + 1][1] - '0';
				}
			}
			autostart = true;
			// if used with -playdemo, fast-forward demo up to the desired map
			crispy->demowarp = startmap;
		}

		// Undocumented: Invoked by setup to test the controls.
		if (auto p = M_CheckParm("-testcontrols"); p > 0)
		{
			startepisode = 1;
			startmap = 1;
			autostart = true;
			testcontrols = true;
		}

		// port level flipping feature over from Strawberry Doom
	#ifdef ENABLE_APRIL_1ST_JOKE
		{
			time_t curtime = time(NULL);
			struct tm* curtm = localtime(&curtime);

			if (curtm && curtm->tm_mon == 3 && curtm->tm_mday == 1)
				crispy->fliplevels = true;
		}
	#endif

		if (auto p = M_CheckParm("-fliplevels"); p > 0)
		{
			crispy->fliplevels = !crispy->fliplevels;
			crispy->flipweapons = !crispy->flipweapons;
		}

		if (auto p = M_CheckParm("-flipweapons"); p > 0)
		{
			crispy->flipweapons = !crispy->flipweapons;
		}

		// Check for load game parameter
		// We do this here and save the slot number, so that the network code can override it or send the load slot to other players.

		// Load the game in slot s.
		if (auto p = M_CheckParmWithArgs("-loadgame", 1); p)
		{
			startloadgame = atoi(myargv[p + 1]);
		}
		else
		{
			// Not loading a game
			startloadgame = -1;
		}

		if (M_CheckParmWithArgs("-statdump", 1))
		{
			I_AtExit(StatDump, true);
			DEH_printf("External statistics registered.\n");
		}

		// Record a demo named x.lmp.
		if (auto p = M_CheckParmWithArgs("-record", 1); p)
		{
			G_RecordDemo(myargv[p + 1]);
			autostart = true;
		}

		if (auto p = M_CheckParmWithArgs("-playdemo", 1); p)
		{
			singledemo = true; // quit after one demo
			G_DeferedPlayDemo(demolumpname);
			D_DoomLoop(); // never returns
		}
		crispy->demowarp = 0; // we don't play a demo, so don't skip maps

		if (auto p = M_CheckParmWithArgs("-timedemo", 1); p)
		{
			G_TimeDemo(demolumpname);
			D_DoomLoop(); // never returns
		}
	}

	DoomEngine()
	{
		// unconditionally initialize DEH tables
		DEH_Init();

		I_AtExit(D_Endoom, false);

		// print banner
		I_PrintBanner(PACKAGE_STRING);

		DEH_printf("Z_Init: Init zone memory allocation daemon. \n");
		Z_Init();

		CheckParams();

		// init subsystems
		DEH_printf("V_Init: allocate screens.\n");
		V_Init();

		// Load configuration files before initialising other subsystems.
		DEH_printf("M_LoadDefaults: Load system defaults.\n");
		M_SetConfigFilenames("default.cfg", PROGRAM_PREFIX "doom.cfg");
		D_BindVariables();
		M_LoadDefaults();

		// Save configuration at exit.
		I_AtExit(M_SaveDefaults, true); // always save configuration at exit

		// Find main IWAD file and load it.
		iwadfile = D_FindIWAD(IWAD_MASK_DOOM, &gamemission);

		// None found?
		if (iwadfile.empty())
		{
			I_Error("Game mode indeterminate. No IWAD file was found. Try\nspecifying one with the '-iwad' command line parameter.\n");
		}

		modifiedgame = false;

		DEH_printf("W_Init: Init WADfiles.\n");
		D_AddFile(iwadfile);
		numiwadlumps = numlumps;

		W_CheckCorrectIWAD(GameMission::doom);

		// Now that we've loaded the IWAD, we can figure out what gamemission we're playing and which version of Vanilla Doom we need to emulate.
		D_IdentifyVersion();
		InitGameVersion();

		// Check which IWAD variant we are using.
		if (W_CheckNumForName("FREEDOOM") >= 0)
		{
			if (W_CheckNumForName("FREEDM") >= 0)
			{
				gamevariant = GameVariant::freedm;
			}
			else
			{
				gamevariant = GameVariant::freedoom;
			}
		}
		else if (W_CheckNumForName("DMENUPIC") >= 0)
		{
			gamevariant = GameVariant::bfgedition;
		}

		// Doom 3: BFG Edition includes modified versions of the classic
		// IWADs which can be identified by an additional DMENUPIC lump.
		// Furthermore, the M_GDHIGH lumps have been modified in a way that
		// makes them incompatible to Vanilla Doom and the modified version
		// of doom2.wad is missing the TITLEPIC lump.
		// We specifically check for DMENUPIC here, before PWADs have been loaded which could probably include a lump of that name.
		if (gamevariant == GameVariant::bfgedition)
		{
			printf("BFG Edition: Using workarounds as needed.\n");

			// BFG Edition changes the names of the secret levels to
			// censor the Wolfenstein references. It also has an extra
			// secret level (MAP33). In Vanilla Doom (meaning the DOS
			// version), MAP33 overflows into the Plutonia level names
			// array, so HUSTR_33 is actually PHUSTR_1.
			DEH_AddStringReplacement(HUSTR_31, "level 31: idkfa");
			DEH_AddStringReplacement(HUSTR_32, "level 32: keen");
			DEH_AddStringReplacement(PHUSTR_1, "level 33: betray");

			// The BFG edition doesn't have the "low detail" menu option (fair
			// enough). But bizarrely, it reuses the M_GDHIGH patch as a label
			// for the options menu (says "Fullscreen:"). Why the perpetrators
			// couldn't just add a new graphic lump and had to reuse this one,
			// I don't know.

			// The end result is that M_GDHIGH is too wide and causes the game
			// to crash. As a workaround to get a minimum level of support for
			// the BFG edition IWADs, use the "ON"/"OFF" graphics instead.
			DEH_AddStringReplacement("M_GDHIGH", "M_MSGON");
			DEH_AddStringReplacement("M_GDLOW", "M_MSGOFF");

			// The BFG edition's "Screen Size:" graphic has also been changed
			// to say "Gamepad:". Fortunately, it (along with the original
			// Doom IWADs) has an unused graphic that says "Display". So we
			// can swap this in instead, and it kind of makes sense.
			DEH_AddStringReplacement("M_SCRNSZ", "M_DISP");
		}

		// Debug:
		//W_PrintDirectory();

		I_AtExit(G_CheckDemoStatusAtExit, true);

		// Generate the WAD hash table. Speed things up a bit.
		W_GenerateHashTable();

		// Set the gamedescription string. This is only possible now that we've finished loading Dehacked patches.
		D_SetGameDescription();

		savegamedir = M_GetSaveGameDir(D_SaveGameIWADName(gamemission, gamevariant));

		// Check for -file in shareware
		if (modifiedgame && (gamevariant != GameVariant::freedoom))
		{
			// These are the lumps that will be checked in IWAD, if any one is not present, execution will be aborted.
			char name[23][8] =
			{
				"e2m1", "e2m2", "e2m3", "e2m4", "e2m5", "e2m6", "e2m7", "e2m8", "e2m9",
				"e3m1", "e3m3", "e3m3", "e3m4", "e3m5", "e3m6", "e3m7", "e3m8", "e3m9",
				"dphoof", "bfgga0", "heada1", "cybra1", "spida1d1"
			};

			if (gamemode == GameMode::shareware)
			{
				I_Error(DEH_String("\nYou cannot -file with the shareware version. Register!"));
			}

			// Check for fake IWAD with right name, but w/o all the lumps of the registered version.
			if (gamemode == GameMode::registered)
			{
				for (size_t i = 0; i < 23; ++i)
				{
					if (W_CheckNumForName(name[i]) < 0)
					{
						I_Error(DEH_String("\nThis is not the registered version."));
					}
				}
			}
		}

		// disable meaningless warning, we always use "-merge" anyway
	#if 0
		if (W_CheckNumForName("SS_START") >= 0 || W_CheckNumForName("FF_END") >= 0)
		{
			I_PrintDivider();
			printf(" WARNING: The loaded WAD file contains modified sprites or\n"
					" floor textures. You may want to use the '-merge' command\n line option instead of '-file'.\n");
		}
	#endif

		I_PrintStartupBanner(gamedescription);
		PrintDehackedBanners();

		DEH_printf("I_Init: Setting up machine state.\n");
		I_CheckIsScreensaver();
		I_InitTimer();
		I_InitJoystick();
		I_InitSound(true);
		I_InitMusic();

		// check for SSG resources
		// wielding/firing sprite sequence
		// firing sound
		// opening sound
		// reloading sound
		// closing sound
		crispy->havessg = (
						gamemode == GameMode::commercial || (
						W_CheckNumForName("sht2a0") != -1
						&& I_GetSfxLumpNum(&S_sfx[::std::size_t(sfxenum_t::sfx_dshtgn)]) != -1
						&& I_GetSfxLumpNum(&S_sfx[::std::size_t(sfxenum_t::sfx_dbopn)]) != -1
						&& I_GetSfxLumpNum(&S_sfx[::std::size_t(sfxenum_t::sfx_dbload)]) != -1
						&& I_GetSfxLumpNum(&S_sfx[::std::size_t(sfxenum_t::sfx_dbcls)]) != -1)
				);

		// check for presence of a 5th episode
		crispy->haved1e5 = (gameversion == GameVersion::exe_ultimate)
			&& (W_CheckNumForName("m_epi5") != -1) && (W_CheckNumForName("e5m1") != -1) && (W_CheckNumForName("wilv40") != -1);

		// check for presence of E1M10
		crispy->havee1m10 = (gamemode == GameMode::retail) && (W_CheckNumForName("e1m10") != -1) && (W_CheckNumForName("sewers") != -1);

		// check for presence of MAP33
		crispy->havemap33 = (gamemode == GameMode::commercial) && (W_CheckNumForName("map33") != -1) && (W_CheckNumForName("cwilv32") != -1);

		// change level name for MAP33 if not already changed
		if (crispy->havemap33 && !DEH_HasStringReplacement(PHUSTR_1))
		{
			DEH_AddStringReplacement(PHUSTR_1, "level 33: betray");
		}

		printf("NET_Init: Init network subsystem.\n");
		NET_Init();

		// Initial netgame startup. Connect to server etc.
		D_ConnectNetGame();

		// get skill / episode / map from parms
		startskill = SkillType::sk_medium;
		startepisode = 1;
		startmap = 1;
		autostart = false;

		DEH_printf("M_Init: Init miscellaneous info.\n");
		M_Init();

		DEH_printf("R_Init: Init DOOM refresh daemon - ");
		R_Init();

		DEH_printf("\nP_Init: Init Playloop state.\n");
		P_Init();

		DEH_printf("S_Init: Setting up sound.\n");
		S_Init(sfxVolume * 8, musicVolume * 8);

		DEH_printf("D_CheckNetGame: Checking network game status.\n");
		D_CheckNetGame();

		PrintGameVersion();

		DEH_printf("HU_Init: Setting up heads up display.\n");
		HU_Init();

		DEH_printf("ST_Init: Init status bar.\n");
		ST_Init();

		// If Doom II without a MAP01 lump, this is a store demo. Moved this here so that MAP01 isn't constantly looked up in the main loop.
		if (gamemode == GameMode::commercial && W_CheckNumForName("map01") < 0)
		{
			storedemo = true;
		}

		if (startloadgame >= 0)
		{
			M_StringCopy(file, P_SaveGameFile(startloadgame), sizeof(file));
			G_LoadGame(file);
		}

		if (gameaction != GameAction_t::ga_loadgame)
		{
			if (autostart || netgame)
			{
				G_InitNew(startskill, startepisode, startmap);
			}
			else
			{
				D_StartTitle(); // start up intro loop
			}
		}

		D_DoomLoop(); // never returns
	}

	void D_DoomLoop()
	{
		if (gamevariant == GameVariant::bfgedition && (demorecording || (gameaction == GameAction_t::ga_playdemo) || netgame))
		{
			printf(" WARNING: You are playing using one of the Doom Classic\n IWAD files shipped with the Doom 3: BFG Edition. These are\n"
				" known to be incompatible with the regular IWAD files and\n may cause demos and network games to get out of sync.\n");
		}

		// no need to write a demo header in demo continue mode
		if (demorecording && gameaction != GameAction_t::ga_playdemo)
		{
			G_BeginRecording();
		}

		main_loop_started = true;

		I_SetWindowTitle(gamedescription);
		I_GraphicsCheckCommandLine();
		I_SetGrabMouseCallback(D_GrabMouseCallback);
		I_InitGraphics();
		EnableLoadingDisk();

		TryRunTics();

		V_RestoreBuffer();
		R_ExecuteSetViewSize();

		D_StartGameLoop();

		if (testcontrols)
		{
			wipegamestate = gamestate;
		}

		while (1)
		{
			D_RunFrame();
		}
	}

};

}
