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
#include "../../derma/stdafx.h"

#include "doom_includes.h"
#include "doom.h"
#include "doom_engine.h"

namespace cudadoom
{

void R_ExecuteSetViewSize();

// Send all the events of the given timestamp down the responder chain
void D_ProcessEvents()
{
	EventType* ev;

	// IF STORE DEMO, DO NOT ACCEPT INPUT
	if (storedemo)
	{
		return;
	}

	while ((ev = D_PopEvent()) != NULL)
	{
		if (M_Responder(ev))
		{
			continue; // menu ate the event
		}

		G_Responder(ev);
	}
}

bool D_Display()
{
	static bool viewactivestate = false;
	static bool menuactivestate = false;
	static bool inhelpscreensstate = false;
	static bool fullscreen = false;
	static GameState_t oldgamestate = GameState_t(-1);
	static int borderdrawcount;
	int y;
	bool wipe;
	bool redrawsbar;

	redrawsbar = false;

	// change the view size if needed
	if (setsizeneeded)
	{
		R_ExecuteSetViewSize();
		oldgamestate = GameState_t(-1); // force background redraw
		borderdrawcount = 3;
	}

	// save the current screen if about to wipe
	if (gamestate != wipegamestate)
	{
		wipe = true;
		wipe_StartScreen(0, 0, SCREENWIDTH, SCREENHEIGHT);
	}
	else
	{
		wipe = false;
	}

	if (gamestate == GameState_t::GS_LEVEL && gametic)
	{
		HU_Erase();
	}

	// do buffered drawing
	switch (gamestate)
	{
	case GameState_t::GS_LEVEL:
		if (!gametic)
		{
			break;
		}
		if (automapactive && !crispy->automapoverlay)
		{
			// update automap while playing
			R_RenderPlayerView(&players[displayplayer]);
			AM_Drawer();
		}
		if (wipe || (viewheight != SCREENHEIGHT && fullscreen))
		{
			redrawsbar = true;
		}
		if (inhelpscreensstate && !inhelpscreens)
		{
			redrawsbar = true; // just put away the help screen
		}
		ST_Drawer(viewheight == SCREENHEIGHT, redrawsbar);
		fullscreen = viewheight == SCREENHEIGHT;
		break;

	case GameState_t::GS_INTERMISSION:
		WI_Drawer();
		break;

	case GameState_t::GS_FINALE:
		F_Drawer();
		break;

	case GameState_t::GS_DEMOSCREEN:
		D_PageDrawer();
		break;
	}

	// draw buffered stuff to screen
	I_UpdateNoBlit();

	// draw the view directly
	if (gamestate == GameState_t::GS_LEVEL && (!automapactive || crispy->automapoverlay) && gametic)
	{
		R_RenderPlayerView(&players[displayplayer]);

		// Crispy HUD
		if (screenblocks >= CRISPY_HUD)
		{
			ST_Drawer(false, true);
		}
	}

	// in automap overlay mode, the HUD is drawn on top of everything else
	if (gamestate == GameState_t::GS_LEVEL && gametic && !(automapactive && crispy->automapoverlay))
	{
		HU_Drawer();
	}

	// clean up border stuff
	if (gamestate != oldgamestate && gamestate != GameState_t::GS_LEVEL)
	{
#ifndef CRISPY_TRUECOLOR
		I_SetPalette(W_CacheLumpName<byte>(DEH_String("PLAYPAL"), pu_tags_t::PU_CACHE));
#else
		I_SetPalette(0);
#endif
	}

	// see if the border needs to be initially drawn
	if (gamestate == GameState_t::GS_LEVEL && oldgamestate != GameState_t::GS_LEVEL)
	{
		viewactivestate = false;		// view was not active
		R_FillBackScreen();				// draw the pattern into the back screen
	}

	// see if the border needs to be updated to the screen
	if (gamestate == GameState_t::GS_LEVEL && (!automapactive || crispy->automapoverlay) && scaledviewwidth != SCREENWIDTH)
	{
		if (menuactive || menuactivestate || !viewactivestate)
		{
			borderdrawcount = 3;
		}

		if (borderdrawcount)
		{
			R_DrawViewBorder();	// erase old menu stuff
			--borderdrawcount;
		}
	}

	if (testcontrols)
	{
		// Box showing current mouse speed
		V_DrawMouseSpeedBox(testcontrols_mousespeed);
	}

	menuactivestate = menuactive;
	viewactivestate = viewactive;
	inhelpscreensstate = inhelpscreens;
	oldgamestate = wipegamestate = gamestate;

	// in automap overlay mode, draw the automap and HUD on top of everything else
	if (automapactive && crispy->automapoverlay)
	{
		AM_Drawer();
		HU_Drawer();

		// force redraw of status bar and border
		viewactivestate = false;
		inhelpscreensstate = true;
	}

	// draw neither pause pic nor menu when taking a clean screenshot
	if (crispy->cleanscreenshot)
	{
		return false;
	}

	// draw pause pic
	if (paused)
	{
		if (automapactive && !crispy->automapoverlay)
		{
			y = 4;
		}
		else
		{
			y = (viewwindowy >> crispy->hires) + 4;
		}
		V_DrawPatchDirect((viewwindowx >> crispy->hires) + ((scaledviewwidth >> crispy->hires) - 68) / 2 - WIDESCREENDELTA, y,
			W_CacheLumpName<patch_t>(DEH_String("M_PAUSE"), pu_tags_t::PU_CACHE));
	}

	// menus go directly to the screen
	M_Drawer();			// menu is drawn even on top of everything
	NetUpdate();		// send out any new accumulation

	return wipe;
}

void EnableLoadingDisk() // un-static
{
	::std::string disk_lump_name;

	if (show_diskicon)
	{
		if (M_CheckParm("-cdrom") > 0)
		{
			disk_lump_name = DEH_String("STCDROM");
		}
		else
		{
			disk_lump_name = DEH_String("STDISK");
		}

		V_EnableLoadingDisk(disk_lump_name, SCREENWIDTH - LOADING_DISK_W, SCREENHEIGHT - LOADING_DISK_H);
	}
}

// Called to determine whether to grab the mouse pointer
bool D_GrabMouseCallback()
{
	// Drone players don't need mouse focus
	if (drone)
	{
		return false;
	}

	// when menu is active or game is paused, release the mouse
	if (menuactive || paused)
	{
		return false;
	}

	// only grab mouse when playing levels (but not demos)
	return (gamestate == GameState_t::GS_LEVEL) && !demoplayback && !advancedemo;
}

void D_RunFrame()
{
	TimeType nowtime;
	TimeType tics;
	static int wipestart;
	static bool wipe;

	if (wipe)
	{
		do
		{
			nowtime = I_GetTime();
			tics = nowtime - wipestart;
			I_Sleep(1);
		} while (tics <= 0);

		wipestart = nowtime;
		wipe = !wipe_ScreenWipe(wipe_Melt, 0, 0, SCREENWIDTH, SCREENHEIGHT, tics);
		I_UpdateNoBlit();
		M_Drawer();								// menu is drawn even on top of wipes
		I_FinishUpdate();						// page flip or blit buffer
		return;
	}

	// frame syncronous IO operations
	I_StartFrame();

	TryRunTics(); // will run at least one tic

	S_UpdateSounds(players[consoleplayer]);// move positional sounds

	// Update display, next frame, with current state if no profiling is on
	if (screenvisible && !nodrawers)
	{
		if ((wipe = D_Display()))
		{
			// start wipe on this frame
			wipe_EndScreen(0, 0, SCREENWIDTH, SCREENHEIGHT);

			wipestart = I_GetTime() - 1;
		}
		else
		{
			// normal update
			I_FinishUpdate(); // page flip or blit buffer
		}
	}

	// post-rendering function pointer to apply config changes that affect rendering and that are better
	// applied after the current frame has finished rendering
	if (crispy->post_rendering_hook && !wipe)
	{
		crispy->post_rendering_hook();
		crispy->post_rendering_hook = NULL;
	}
}

// Handles timing for warped projection
void D_PageTicker()
{
	if (--pagetic < 0)
		D_AdvanceDemo();
}

void D_PageDrawer()
{
	V_DrawPatchFullScreen(W_CacheLumpName<patch_t>(pagename, pu_tags_t::PU_CACHE), crispy->fliplevels);
}

// Called after each demo or intro demosequence finishes
void D_AdvanceDemo()
{
	advancedemo = true;
}

// This cycles through the demo sequences.
// FIXME - version dependend demo numbers?
void D_DoAdvanceDemo()
{
	players[consoleplayer].playerstate = PlayerState::live; // not reborn
	advancedemo = false;
	usergame = false;				// no save / end game here
	paused = false;
	gameaction = GameAction_t::ga_nothing;
	// update the "singleplayer" variable
	CheckCrispySingleplayer(!demorecording && !demoplayback && !netgame);

	// The Ultimate Doom executable changed the demo sequence to add a DEMO4 demo. Final Doom was based on Ultimate, so also includes this change;
	// however, the Final Doom IWADs do not include a DEMO4 lump, so the game bombs out with an error when it reaches this point in the demo sequence.

	// However! There is an alternate version of Final Doom that includes a fixed executable.

	// get rid of this demo sequence breaking bug
	//if (gameversion == GameVersion::exe_ultimate || gameversion == GameVersion::exe_final)
	if (W_CheckNumForName(DEH_String("demo4")) >= 0)
	{
		demosequence = (demosequence + 1) % 7;
	}
	else
	{
		demosequence = (demosequence + 1) % 6;
	}

	switch (demosequence)
	{
	case 0:
		if (gamemode == GameMode::commercial)
		{
			pagetic = TICRATE * 11;
		}
		else
		{
			pagetic = 170;
		}
		gamestate = GameState_t::GS_DEMOSCREEN;
		pagename = DEH_String("TITLEPIC");
		if (gamemode == GameMode::commercial)
		{
			S_StartMusic(musicenum_t::mus_dm2ttl);
		}
		else
		{
			S_StartMusic(musicenum_t::mus_intro);
		}
		break;
	case 1:
		G_DeferedPlayDemo(DEH_String("demo1"));
		break;
	case 2:
		pagetic = 200;
		gamestate = GameState_t::GS_DEMOSCREEN;
		pagename = DEH_String("CREDIT");
		break;
	case 3:
		G_DeferedPlayDemo(DEH_String("demo2"));
		break;
	case 4:
		gamestate = GameState_t::GS_DEMOSCREEN;
		if (gamemode == GameMode::commercial)
		{
			pagetic = TICRATE * 11;
			pagename = DEH_String("TITLEPIC");
			S_StartMusic(musicenum_t::mus_dm2ttl);
		}
		else
		{
			pagetic = 200;

			if (gameversion >= GameVersion::exe_ultimate)
			{
				pagename = DEH_String("CREDIT");
			}
			else
			{
				pagename = DEH_String("HELP2");
			}
		}
		break;
	case 5:
		G_DeferedPlayDemo(DEH_String("demo3"));
		break;
		// THE DEFINITIVE DOOM Special Edition demo
	case 6:
		G_DeferedPlayDemo(DEH_String("demo4"));
		break;
	}

	// The Doom 3: BFG Edition version of doom2.wad does not have a TITLETPIC lump. Use INTERPIC instead as a workaround.
	if (gamevariant == GameVariant::bfgedition && !iequals(pagename, "TITLEPIC") && W_CheckNumForName("titlepic") < 0)
	{
		// use DMENUPIC instead of TITLEPIC, it's awesome
		pagename = DEH_String("DMENUPIC");
	}
}

void D_StartTitle()
{
	gameaction = GameAction_t::ga_nothing;
	demosequence = -1;
	D_AdvanceDemo();
}

// Get game name: if the startup banner has been replaced, use that. Otherwise, use the name given
::std::string GetGameName(::std::string gamename)
{
	size_t i;

	for (i = 0; i < arrlen(banners); ++i)
	{
		::std::string deh_sub;

		// Has the banner been replaced?
		deh_sub = DEH_String(banners[i]);

		if (deh_sub != banners[i])
		{
			size_t gamename_size;
			int version;
			::std::string deh_gamename;

			// Has been replaced. We need to expand via printf to include the Doom version number.
			// We also need to cut off spaces to get the basic name
			gamename_size = strlen(deh_sub) + 10;
			deh_gamename = static_cast<decltype(deh_gamename)>(malloc(gamename_size));
			if (deh_gamename == NULL)
			{
				I_Error("GetGameName: Failed to allocate new string");
			}
			version = G_VanillaVersionCode();
			DEH_snprintf(deh_gamename, gamename_size, banners[i], version / 100, version % 100);

			while (deh_gamename[0] != '\0' && isspace(deh_gamename[0]))
			{
				memmove(deh_gamename, deh_gamename + 1, gamename_size - 1);
			}

			while (deh_gamename[0] != '\0' && isspace(deh_gamename[strlen(deh_gamename) - 1]))
			{
				deh_gamename[strlen(deh_gamename) - 1] = '\0';
			}

			return deh_gamename;
		}
	}

	return ::std::string(gamename);
}

static void SetMissionForPackName(::std::string pack_name)
{
	int i;
	static const struct
	{
		::std::string name;
		int mission;
	} packs[] = {
		{ "doom2",		GameMission::doom2 },
		{ "tnt",		GameMission::pack_tnt },
		{ "plutonia",	GameMission::pack_plut },
	};

	for (i = 0; i < arrlen(packs); ++i)
	{
		if (!iequals(pack_name, packs[i].name))
		{
			gamemission = packs[i].mission;
			return;
		}
	}

	printf("Valid mission packs are:\n");

	for (i = 0; i < arrlen(packs); ++i)
	{
		printf("\t%s\n", packs[i].name);
	}

	I_Error("Unknown mission pack name: %s", pack_name);
}

// Find out what version of Doom is playing.
void D_IdentifyVersion()
{
	// gamemission is set up by the D_FindIWAD function. But if we specify '-iwad', we have to identify using IdentifyIWADByName.
	// However, if the iwad does not match any known IWAD name, we may have a dilemma. Try to identify by its contents.
	if (gamemission == GameMission::none)
	{
		unsigned i;

		for (i = 0; i < numlumps; ++i)
		{
			if (!strncasecmp(lumpinfo[i]->name, "MAP01", 8))
			{
				gamemission = GameMission::doom2;
				break;
			}
			else if (!strncasecmp(lumpinfo[i]->name, "E1M1", 8))
			{
				gamemission = GameMission::doom;
				break;
			}
		}

		if (gamemission == GameMission::none)
		{
			// Still no idea. I don't think this is going to work.
			I_Error("Unknown or invalid IWAD file.");
		}
	}

	// Make sure gamemode is set up correctly
	if (logical_gamemission == GameMission::doom)
	{
		// Doom 1. But which version?
		if (W_CheckNumForName("E4M1") > 0)
		{
			// Ultimate Doom
			gamemode = GameMode::retail;
		}
		else if (W_CheckNumForName("E3M1") > 0)
		{
			gamemode = GameMode::registered;
		}
		else
		{
			gamemode = GameMode::shareware;
		}
	}
	else
	{
		int p;

		// Doom 2 of some kind.
		gamemode = GameMode::commercial;

		// We can manually override the gamemission that we got from the IWAD detection code.
		// This allows us to eg. play Plutonia 2 with Freedoom and get the right level names.

		// Explicitly specify a Doom II "mission pack" to run as, instead of detecting it based on the filename.
		// Valid values are: "doom2", "tnt" and "plutonia".
		p = M_CheckParmWithArgs("-pack", 1);
		if (p > 0)
		{
			SetMissionForPackName(myargv[p + 1]);
		}
	}
}

// Set the gamedescription string
static void D_SetGameDescription()
{
	if (logical_gamemission == GameMission::doom)
	{
		// Doom 1. But which version?
		if (gamevariant == GameVariant::freedoom)
		{
			gamedescription = GetGameName("Freedoom: Phase 1");
		}
		else if (gamemode == GameMode::retail)
		{
			// Ultimate Doom
			gamedescription = GetGameName("The Ultimate DOOM");
		}
		else if (gamemode == GameMode::registered)
		{
			gamedescription = GetGameName("DOOM Registered");
		}
		else if (gamemode == GameMode::shareware)
		{
			gamedescription = GetGameName("DOOM Shareware");
		}
	}
	else
	{
		// Doom 2 of some kind. But which mission?
		if (gamevariant == GameVariant::freedm)
		{
			gamedescription = GetGameName("FreeDM");
		}
		else if (gamevariant == GameVariant::freedoom)
		{
			gamedescription = GetGameName("Freedoom: Phase 2");
		}
		else if (logical_gamemission == GameMission::doom2)
		{
			gamedescription = GetGameName("DOOM 2: Hell on Earth");
		}
		else if (logical_gamemission == GameMission::pack_plut)
		{
			gamedescription = GetGameName("DOOM 2: Plutonia Experiment");
		}
		else if (logical_gamemission == GameMission::pack_tnt)
		{
			gamedescription = GetGameName("DOOM 2: TNT - Evilution");
		}
		else if (logical_gamemission == GameMission::pack_nerve)
		{
			gamedescription = GetGameName("DOOM 2: No Rest For The Living");
		}
		else if (logical_gamemission == GameMission::pack_master)
		{
			gamedescription = GetGameName("Master Levels for DOOM 2");
		}
	}

	if (gamedescription == NULL)
	{
		gamedescription = ::std::string("Unknown");
	}
}

static bool D_AddFile(::std::string filename)
{
	wad_file_t* handle;

	printf(" adding %s\n", filename);
	handle = W_AddFile(filename);

	return handle != NULL;
}

// Prints a message only if it has been modified by dehacked.
void PrintDehackedBanners()
{
	size_t i;

	for (i = 0; i < arrlen(copyright_banners); ++i)
	{
		::std::string deh_s;

		deh_s = DEH_String(copyright_banners[i]);

		if (deh_s != copyright_banners[i])
		{
			printf("%s", deh_s);

			// Make sure the modified banner always ends in a newline character. If it doesn't, add a newline. This fixes av.wad.
			if (deh_s[strlen(deh_s) - 1] != '\n')
			{
				printf("\n");
			}
		}
	}
}

// Initialize the game version
static void InitGameVersion()
{
	byte* demolump;
	char demolumpname[6];
	int demoversion;
	int p;
	int i;
	bool status;

	// Emulate a specific version of Doom. Valid values are "1.2", "1.666", "1.7", "1.8", "1.9", "ultimate", "final", "final2", "hacx" and "chex".
	p = M_CheckParmWithArgs("-gameversion", 1);

	if (p)
	{
		for (i = 0; gameversions[i].description != NULL; ++i)
		{
			if (!strcmp(myargv[p + 1], gameversions[i].cmdline))
			{
				gameversion = gameversions[i].version;
				break;
			}
		}

		if (gameversions[i].description == NULL)
		{
			printf("Supported game versions:\n");

			for (i = 0; gameversions[i].description != NULL; ++i)
			{
				printf("\t%s (%s)\n", gameversions[i].cmdline,
					gameversions[i].description);
			}

			I_Error("Unknown game version '%s'", myargv[p + 1]);
		}
	}
	else
	{
		// Determine automatically
		if (gamemission == GameMission::pack_chex)
		{
			// chex.exe - identified by iwad filename
			gameversion = GameVersion::exe_chex;
		}
		else if (gamemission == GameMission::pack_hacx)
		{
			// hacx.exe: identified by iwad filename
			gameversion = GameVersion::exe_hacx;
		}
		else if (gamemode == GameMode::shareware || gamemode == GameMode::registered
			|| (gamemode == GameMode::commercial && gamemission == GameMission::doom2))
		{
			// original
			gameversion = GameVersion::exe_doom_1_9;

			// Detect version from demo lump
			for (i = 1; i <= 3; ++i)
			{
				M_snprintf(demolumpname, 6, "demo%i", i);
				if (W_CheckNumForName(demolumpname) > 0)
				{
					demolump = W_CacheLumpName<byte>(demolumpname, pu_tags_t::PU_STATIC);
					demoversion = demolump[0];
					W_ReleaseLumpName(demolumpname);
					status = true;
					switch (demoversion)
					{
					case 0:
					case 1:
					case 2:
					case 3:
					case 4:
						gameversion = GameVersion::exe_doom_1_2;
						break;
					case 106:
						gameversion = GameVersion::exe_doom_1_666;
						break;
					case 107:
						gameversion = GameVersion::exe_doom_1_7;
						break;
					case 108:
						gameversion = GameVersion::exe_doom_1_8;
						break;
					case 109:
						gameversion = GameVersion::exe_doom_1_9;
						break;
					default:
						status = false;
						break;
					}
					if (status)
					{
						break;
					}
				}
			}
		}
		else if (gamemode == GameMode::retail)
		{
			gameversion = GameVersion::exe_ultimate;
		}
		else if (gamemode == GameMode::commercial)
		{
			// Final Doom: tnt or plutonia. Defaults to emulating the first Final Doom executable, which has the crash in the demo loop;
			// however, having this as the default should mean that it plays back most demos correctly.

			gameversion = GameVersion::exe_final;
		}
	}

	// Deathmatch 2.0 did not exist until Doom v1.4
	if (gameversion <= GameVersion::exe_doom_1_2 && deathmatch == 2)
	{
		deathmatch = 1;
	}

	// The original exe does not support retail - 4th episode not supported
	if (gameversion < GameVersion::exe_ultimate && gamemode == GameMode::retail)
	{
		gamemode = GameMode::registered;
	}

	// EXEs prior to the Final Doom exes do not support Final Doom.
	if (gameversion < GameVersion::exe_final && gamemode == GameMode::commercial
		&& (gamemission == GameMission::pack_tnt || gamemission == GameMission::pack_plut))
	{
		gamemission = GameMission::doom2;
	}
}

void PrintGameVersion()
{
	int i;

	for (i = 0; gameversions[i].description != NULL; ++i)
	{
		if (gameversions[i].version == gameversion)
		{
			printf("Emulating the behavior of the "
				"'%s' executable.\n", gameversions[i].description);
			break;
		}
	}
}

// Function called at exit to display the ENDOOM screen
static void D_Endoom()
{
	byte* endoom;

	// Don't show ENDOOM if we have it disabled, or we're running in screensaver or control test mode.
	// Only show it once the game has actually started.
	if (!show_endoom || !main_loop_started
		|| screensaver_mode || M_CheckParm("-testcontrols") > 0)
	{
		return;
	}

	endoom = W_CacheLumpName<byte>(DEH_String("ENDOOM"), pu_tags_t::PU_STATIC);

	I_Endoom(endoom);
}

// Load dehacked patches needed for certain IWADs.
static void LoadIwadDeh()
{
	// The Freedoom IWADs have DEHACKED lumps that must be loaded.
	if (gamevariant == GameVariant::freedoom || gamevariant == GameVariant::freedm)
	{
		// Old versions of Freedoom (before 2014-09) did not have technically valid DEHACKED lumps,
		// so ignore errors and just continue if this is an old IWAD.
		DEH_LoadLumpByName("DEHACKED", false, true);
	}

	// If this is the HACX IWAD, we need to load the DEHACKED lump.
	if (gameversion == GameVersion::exe_hacx)
	{
		if (!DEH_LoadLumpByName("DEHACKED", true, false))
		{
			I_Error("DEHACKED lump not found. Please check that this is the Hacx v1.2 IWAD.");
		}
	}

	// Chex Quest needs a separate Dehacked patch which must be downloaded and installed next to the IWAD.
	if (gameversion == GameVersion::exe_chex)
	{
		::std::string chex_deh = NULL;
		::std::string dirname;

		// Look for chex.deh in the same directory as the IWAD file.
		dirname = M_DirName(iwadfile);
		chex_deh = ::std::string(dirname + DIR_SEPARATOR_S + "chex.deh");

		// If the dehacked patch isn't found, try searching the WAD search path instead. We might find it...
		if (!M_FileExists(chex_deh))
		{
			chex_deh = D_FindWADByName("chex.deh");
		}

		// Still not found?
		if (chex_deh.empty())
		{
			I_Error("Unable to find Chex Quest dehacked file (chex.deh).\nThe dehacked file is required in order to emulate\n"
				"chex.exe correctly. It can be found in your nearest\n/idgames repository mirror at:\n\n utils/exe_edit/patches/chexdeh.zip");
		}

		if (!DEH_LoadFile(chex_deh))
		{
			I_Error("Failed to load chex.deh needed for emulating chex.exe.");
		}
	}
}

static void G_CheckDemoStatusAtExit()
{
	G_CheckDemoStatus();
}

void D_DoomMain()
{

}

} // end namespace cudadoom
