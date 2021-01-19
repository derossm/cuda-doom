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

#include "../../derma/common.h"
#include "doom_includes.h"

namespace cudadoom
{

::std::string gamedescription;

// Location where savegames are stored
::std::string savegamedir;

// location of IWAD and WAD files
::std::string iwadfile;

bool devparm;		// started game with -devparm
bool nomonsters;	// checkparm of -nomonsters
bool respawnparm;	// checkparm of -respawn
bool fastparm;		// checkparm of -fast

//extern int soundVolume;
//extern int sfxVolume;
//extern int musicVolume;

extern bool inhelpscreens;

SkillType startskill;
int startepisode;
int startmap;
bool autostart;
int startloadgame;

bool advancedemo;

// Store demo, do not accept any inputs
bool storedemo;

// If true, the main game loop has started.
bool main_loop_started = false;

char wadfile[1024];			// primary wad file
char mapdir[1024];			// directory of development maps

int show_endoom = 0;		// disable
int show_diskicon = 1;

// draw current display, possibly wiping it from the previous wipegamestate can be set to -1 to force a wipe on the next draw
GameState_t wipegamestate = GameState_t::GS_DEMOSCREEN;
extern bool setsizeneeded;
extern int showMessages;

// Add configuration file variable bindings.
static ::std::string const chat_macro_defaults[10] =
{
	HUSTR_CHATMACRO0,
	HUSTR_CHATMACRO1,
	HUSTR_CHATMACRO2,
	HUSTR_CHATMACRO3,
	HUSTR_CHATMACRO4,
	HUSTR_CHATMACRO5,
	HUSTR_CHATMACRO6,
	HUSTR_CHATMACRO7,
	HUSTR_CHATMACRO8,
	HUSTR_CHATMACRO9
};

void D_BindVariables()
{
	M_ApplyPlatformDefaults();

	I_BindInputVariables();
	I_BindVideoVariables();
	I_BindJoystickVariables();
	I_BindSoundVariables();

	M_BindBaseControls();
	M_BindWeaponControls();
	M_BindMapControls();
	M_BindMenuControls();
	M_BindChatControls(MAX_PLAYERS);

	key_multi_msgplayer[0] = HUSTR_KEYGREEN;
	key_multi_msgplayer[1] = HUSTR_KEYINDIGO;
	key_multi_msgplayer[2] = HUSTR_KEYBROWN;
	key_multi_msgplayer[3] = HUSTR_KEYRED;

	NET_BindVariables();

	M_BindVariable("mouse_sensitivity", &mouseSensitivity);
	M_BindVariable("mouse_sensitivity_x2", &mouseSensitivity_x2);
	M_BindVariable("mouse_sensitivity_y", &mouseSensitivity_y);
	M_BindVariable("sfx_volume", &sfxVolume);
	M_BindVariable("music_volume", &musicVolume);
	M_BindVariable("show_messages", &showMessages);
	M_BindVariable("screenblocks", &screenblocks);
	M_BindVariable("detaillevel", &detailLevel);
	M_BindVariable("snd_channels", &snd_channels);
	// unconditionally disable savegame and demo limits
	//M_BindVariable("vanilla_savegame_limit", &vanilla_savegame_limit);
	//M_BindVariable("vanilla_demo_limit", &vanilla_demo_limit);
	M_BindVariable("show_endoom", &show_endoom);
	M_BindVariable("show_diskicon", &show_diskicon);

	// Multiplayer chat macros
	for (auto i{0}; i < 10; ++i)
	{
		chat_macros[i] = chat_macro_defaults[i];
		auto buf = M_snprintf("chatmacro", i);
		M_BindVariable(buf, &chat_macros[i]);
	}

	// bind "crispness" config variables
	M_BindVariable("crispy_automapoverlay", &crispy->automapoverlay);
	M_BindVariable("crispy_automaprotate", &crispy->automaprotate);
	M_BindVariable("crispy_automapstats", &crispy->automapstats);
	M_BindVariable("crispy_bobfactor", &crispy->bobfactor);
	M_BindVariable("crispy_btusetimer", &crispy->btusetimer);
	M_BindVariable("crispy_brightmaps", &crispy->brightmaps);
	M_BindVariable("crispy_centerweapon", &crispy->centerweapon);
	M_BindVariable("crispy_coloredblood", &crispy->coloredblood);
	M_BindVariable("crispy_coloredhud", &crispy->coloredhud);
	M_BindVariable("crispy_crosshair", &crispy->crosshair);
	M_BindVariable("crispy_crosshairhealth", &crispy->crosshairhealth);
	M_BindVariable("crispy_crosshairtarget", &crispy->crosshairtarget);
	M_BindVariable("crispy_crosshairtype", &crispy->crosshairtype);
	M_BindVariable("crispy_demobar", &crispy->demobar);
	M_BindVariable("crispy_demotimer", &crispy->demotimer);
	M_BindVariable("crispy_demotimerdir", &crispy->demotimerdir);
	M_BindVariable("crispy_extautomap", &crispy->extautomap);
	M_BindVariable("crispy_extsaveg", &crispy->extsaveg);
	M_BindVariable("crispy_flipcorpses", &crispy->flipcorpses);
	M_BindVariable("crispy_freeaim", &crispy->freeaim);
	M_BindVariable("crispy_freelook", &crispy->freelook);
	M_BindVariable("crispy_hires", &crispy->hires);
	M_BindVariable("crispy_jump", &crispy->jump);
	M_BindVariable("crispy_leveltime", &crispy->leveltime);
	M_BindVariable("crispy_mouselook", &crispy->mouselook);
	M_BindVariable("crispy_neghealth", &crispy->neghealth);
	M_BindVariable("crispy_overunder", &crispy->overunder);
	M_BindVariable("crispy_pitch", &crispy->pitch);
	M_BindVariable("crispy_playercoords", &crispy->playercoords);
	M_BindVariable("crispy_recoil", &crispy->recoil);
	M_BindVariable("crispy_secretmessage", &crispy->secretmessage);
	M_BindVariable("crispy_smoothlight", &crispy->smoothlight);
	M_BindVariable("crispy_smoothmap", &crispy->smoothmap);
	M_BindVariable("crispy_smoothscaling", &crispy->smoothscaling);
	M_BindVariable("crispy_soundfix", &crispy->soundfix);
	M_BindVariable("crispy_soundfull", &crispy->soundfull);
	M_BindVariable("crispy_soundmono", &crispy->soundmono);
	M_BindVariable("crispy_translucency", &crispy->translucency);
#ifdef CRISPY_TRUECOLOR
	M_BindVariable("crispy_truecolor", &crispy->truecolor);
#endif
	M_BindVariable("crispy_uncapped", &crispy->uncapped);
	M_BindVariable("crispy_vsync", &crispy->vsync);
	M_BindVariable("crispy_weaponsquat", &crispy->weaponsquat);
	M_BindVariable("crispy_widescreen", &crispy->widescreen);
}

// DEMO LOOP
int demosequence;
TimeType pagetic;
::std::string pagename;

// Strings for dehacked replacements of the startup banner.
// These are from the original source: some of them are perhaps not used in any dehacked patches
static ::std::string banners[] =
{
	// doom2.wad
	"							"
	"DOOM 2: Hell on Earth v%i.%i"
	"							",
	// doom2.wad v1.666
	"							"
	"DOOM 2: Hell on Earth v%i.%i66"
	"							",
	// doom1.wad
	"							"
	"DOOM Shareware Startup v%i.%i"
	"							",
	// doom.wad
	"							"
	"DOOM Registered Startup v%i.%i"
	"							",
	// Registered DOOM uses this
	"							"
	"DOOM System Startup v%i.%i"
	"							",
	// Doom v1.666
	"							"
	"DOOM System Startup v%i.%i66"
	"							"
	// doom.wad (Ultimate DOOM)
	"							"
	"The Ultimate DOOM Startup v%i.%i"
	"						",
	// tnt.wad
	"						"
	"DOOM 2: TNT - Evilution v%i.%i"
	"							",
	// plutonia.wad
	"					"
	"DOOM 2: Plutonia Experiment v%i.%i"
	"							",
};

// print title for every printed line
char title[128];

// Copyright message banners. Some dehacked mods replace these. These are only displayed if they are replaced by dehacked.
static ::std::string copyright_banners[] =
{
	"===========================================================================\n"
	"ATTENTION: This version of DOOM has been modified. If you would like to\n"
	"get a copy of the original game, call 1-800-IDGAMES or see the readme file.\n"
	"		You will not receive technical support for modified games.\n"
	"						press enter to continue\n"
	"===========================================================================\n",

	"===========================================================================\n"
	"					Commercial product - do not distribute!\n"
	"			Please report software piracy to the SPA: 1-800-388-PIR8\n"
	"===========================================================================\n",

	"===========================================================================\n"
	"								Shareware!\n"
	"===========================================================================\n"
};

static struct gversion_t
{
	::std::string description;
	::std::string cmdline;
	GameVersion version;
};
static const std::string str{"hello"};
constexpr size_t k = sizeof(str);

static const std::array<const gversion_t, 10> gameversions{
	gversion_t{ "Doom 1.2",				"1.2",				GameVersion::exe_doom_1_2 },
	gversion_t{ "Doom 1.666",			"1.666",			GameVersion::exe_doom_1_666 },
	gversion_t{ "Doom 1.7/1.7a",		"1.7",				GameVersion::exe_doom_1_7 },
	gversion_t{ "Doom 1.8",				"1.8",				GameVersion::exe_doom_1_8 },
	gversion_t{ "Doom 1.9",				"1.9",				GameVersion::exe_doom_1_9 },
	gversion_t{ "Hacx",					"hacx",				GameVersion::exe_hacx },
	gversion_t{ "Ultimate Doom",		"ultimate",			GameVersion::exe_ultimate },
	gversion_t{ "Final Doom",			"final",			GameVersion::exe_final },
	gversion_t{ "Final Doom(a)",		"final2",			GameVersion::exe_final2 }, // if strings below 15 length, SSO enabled for all std impl
	gversion_t{ "Chex Quest",			"chex",				GameVersion::exe_chex }
};

} // end namespace cudadoom
