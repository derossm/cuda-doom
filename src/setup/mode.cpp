/**********************************************************************************************************************************************\
	Copyright(C) 2005-2014 Simon Howard

	This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License
	as published by the Free Software Foundation; either version 2 of the License, or (at your option) any later version.

	This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
\**********************************************************************************************************************************************/

#include <string>

#include "mode.h"

#include "config.h"
#include "../../textscreen/textscreen.h"
#include "../../textscreen/txt_widget.h"
#include "../../textscreen/txt_window.h"

#include "doomtype.h"
#include "d_mode.h"
#include "d_iwad.h"
#include "i_system.h"
#include "m_argv.h"
#include "m_config.h"
#include "m_controls.h"
#include "m_misc.h"

#include "compatibility.h"
#include "display.h"
#include "joystick.h"
#include "keyboard.h"
#include "mouse.h"
#include "multiplayer.h"
#include "sound.h"

GameMission gamemission;
static const iwad_t** mode_iwads;

struct mission_config_t
{
	::std::string label;
	GameMission mission;
	int mask;
	::std::string name;
	::std::string config_file;
	::std::string extra_config_file;
	::std::string executable;
};

// Default mission to fall back on, if no IWADs are found at all:

#define DEFAULT_MISSION (&mission_configs[0])

static mission_config_t mission_configs[] =
{
	{
		"Doom",
		doom,
		IWAD_MASK_DOOM,
		"doom",
		"default.cfg",
		PROGRAM_PREFIX "doom.cfg",
		PROGRAM_PREFIX "doom"
	},
	{
		"Heretic",
		heretic,
		IWAD_MASK_HERETIC,
		"heretic",
		"heretic.cfg",
		PROGRAM_PREFIX "heretic.cfg",
		PROGRAM_PREFIX "heretic"
	},
	{
		"Hexen",
		hexen,
		IWAD_MASK_HEXEN,
		"hexen",
		"hexen.cfg",
		PROGRAM_PREFIX "hexen.cfg",
		PROGRAM_PREFIX "hexen"
	},
	{
		"Strife",
		strife,
		IWAD_MASK_STRIFE,
		"strife",
		"strife.cfg",
		PROGRAM_PREFIX "strife.cfg",
		PROGRAM_PREFIX "strife"
	}
};

static GameSelectCallback game_selected_callback;

// Miscellaneous variables that aren't used in setup.

static int showMessages = 1;
static int screenblocks = 10;
static int detailLevel = 0;
::std::string savedir;
::std::string executable;
::std::string game_title = "Doom";
::std::string back_flat = "F_PAVE01";
static int comport = 0;
::std::string nickname;

static void BindMiscVariables()
{
	if (gamemission == GameMission::doom)
	{
		M_BindIntVariable("detaillevel",	&detailLevel);
		M_BindIntVariable("show_messages", &showMessages);
	}

	if (gamemission == GameMission::hexen)
	{
		M_BindStringVariable("savedir", &savedir);
		M_BindIntVariable("messageson", &showMessages);

		// Hexen has a variable to control the savegame directory
		// that is used.

		savedir = M_GetSaveGameDir("hexen.wad");

		// On Windows, hexndata\ is the default.

		if (!savedir.compare(""))
		{
			savedir = "hexndata" DIR_SEPARATOR_S;
		}
	}

	if (gamemission == GameMission::strife)
	{
		// Strife has a different default value than the other games
		screenblocks = 10;

		M_BindStringVariable("back_flat",	&back_flat);
		M_BindStringVariable("nickname",	&nickname);

		M_BindIntVariable("screensize",		&screenblocks);
		M_BindIntVariable("comport",		&comport);
	}
	else
	{
		M_BindIntVariable("screenblocks",	&screenblocks);
	}

}

// Initialise all configuration file bindings.
void InitBindings()
{
	M_ApplyPlatformDefaults();

	// Keyboard, mouse, joystick controls
	M_BindBaseControls();
	M_BindWeaponControls();
	M_BindMapControls();
	M_BindMenuControls();

	if (gamemission == GameMission::heretic || gamemission == GameMission::hexen)
	{
		M_BindHereticControls();
	}

	if (gamemission == GameMission::hexen)
	{
		M_BindHexenControls();
	}

	if (gamemission == GameMission::strife)
	{
		M_BindStrifeControls();
	}

	// All other variables
	BindCompatibilityVariables();
	BindDisplayVariables();
	BindJoystickVariables();
	BindKeyboardVariables();
	BindMouseVariables();
	BindSoundVariables();
	BindMiscVariables();
	BindMultiplayerVariables();
}

// Set the name of the executable program to run the game:
static void SetExecutable(mission_config_t* config)
{
	::std::string extension;

#ifdef _WIN32
	extension = ".exe";
#else
	extension = "";
#endif

	executable = ::std::string(config->executable + extension);
}

static void SetMission(mission_config_t* config)
{
	mode_iwads = D_FindAllIWADs(config->mask);
	gamemission = config->mission;
	SetExecutable(config);
	game_title = config->label;
	M_SetConfigFilenames(config->config_file, config->extra_config_file);
}

static mission_config_t* GetMissionForName(::std::string name)
{
	for (size_t i{0}; i < arrlen(mission_configs); ++i)
	{
		if (!name.compare(mission_configs[i].name)) //!strcmp(mission_configs[i].name, name))
		{
			return &mission_configs[i];
		}
	}

	return nullptr;
}

// Check the name of the executable. If it contains one of the game names (eg. chocolate-hexen-setup.exe) then use that game.
static bool CheckExecutableName(GameSelectCallback callback)
{
	mission_config_t* config;
	::std::string exe_name;

	exe_name = M_GetExecutableName();

	for (size_t i{0}; i < arrlen(mission_configs); ++i)
	{
		config = &mission_configs[i];

		if (strstr(exe_name, config->name) != NULL)
		{
			SetMission(config);
			callback();
			return true;
		}
	}

	return false;
}

static void GameSelected(cudadoom::txt::Widget* widget, mission_config_t* config)
{
	SetMission(config);
	game_selected_callback();
}

static void OpenGameSelectDialog(GameSelectCallback callback)
{
	mission_config_t* mission = NULL;
	cudadoom::txt::Window window("Select game");
	const iwad_t** mode_iwads;
	int num_games;
	int i;

	window.AddWidget(cudadoom::txt::Label("Select a game to configure:\n"));
	num_games = 0;

	// Add a button for each game.
	for (i=0; i<arrlen(mission_configs); ++i)
	{
		// Do we have any IWADs for this game installed? If so, add a button.
		mode_iwads = D_FindAllIWADs(mission_configs[i].mask & (IWAD_MASK_DOOM|IWAD_MASK_HERETIC)); // restrict game choice to Doom and Heretic

		if (mode_iwads[0] != NULL)
		{
			mission = &mission_configs[i];
			window.AddWidget(cudadoom::txt::Button(mission_configs[i].label, GameSelected, &mission_configs[i]));
			++num_games;
		}

		free(mode_iwads);
	}

	window.AddWidget(cudadoom::txt::Strut(0, 1));

	// No IWADs found at all? Fall back to doom, then.
	if (num_games == 0)
	{
		window.CloseWindow();
		SetMission(DEFAULT_MISSION);
		callback();
		return;
	}

	// Only one game? Use that game, and don't bother with a dialog.
	if (num_games == 1)
	{
		window.CloseWindow();
		SetMission(mission);
		callback();
		return;
	}

	game_selected_callback = callback;

	// FIXME this function just ends?
}

void SetupMission(GameSelectCallback callback)
{
	mission_config_t* config;
	::std::string mission_name;
	int p;

	// Specify the game to configure the settings for. Valid values are 'doom', 'heretic', 'hexen' and 'strife'.
	p = M_CheckParm("-game");

	if (p > 0)
	{
		mission_name = myargv[p + 1];

		config = GetMissionForName(mission_name);

		if (config == NULL)
		{
			I_Error("Invalid parameter - '%s'", mission_name);
		}

		SetMission(config);
		callback();
	}
	else if (!CheckExecutableName(callback))
	{
		OpenGameSelectDialog(callback);
	}
}

::std::string GetExecutableName()
{
	return executable;
}

::std::string GetGameTitle()
{
	return game_title;
}

const iwad_t**GetIwads()
{
	return mode_iwads;
}
