/**********************************************************************************************************************************************\
	Copyright(C) 2005-2014 Simon Howard

	This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License
	as published by the Free Software Foundation; either version 2 of the License, or (at your option) any later version.

	This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
\**********************************************************************************************************************************************/
#pragma once

#include "../../build/config.h"

#include "../../derma/common.h"

#include "../m_argv.h"
#include "../m_config.h"
#include "../m_controls.h"
#include "../m_misc.h"
#include "../z_zone.h"

#include "setup_icon.cc"

#include "compatibility.h"
#include "display.h"
#include "joystick.h"
#include "keyboard.h"
#include "mouse.h"
#include "multiplayer.h"
#include "sound.h"

#include "mode.h"
#include "execute.h"

#define WINDOW_HELP_URL "https://www.chocolate-doom.org/setup"

static const int cheat_sequence[] =
{
	KEY_UPARROW, KEY_UPARROW, KEY_DOWNARROW, KEY_DOWNARROW,
	KEY_LEFTARROW, KEY_RIGHTARROW, KEY_LEFTARROW, KEY_RIGHTARROW,
	'b', 'a', KEY_ENTER, 0
};

static unsigned cheat_sequence_index = 0;

// I think these are good "sensible" defaults:

static void SensibleDefaults()
{
	key_up = 'w';
	key_down = 's';
	key_strafeleft = 'a';
	key_straferight = 'd';
	key_jump = '/';
	key_lookup = KEY_PGUP;
	key_lookdown = KEY_PGDN;
	key_lookcenter = KEY_HOME;
	key_flyup = KEY_INS;
	key_flydown = KEY_DEL;
	key_flycenter = KEY_END;
	key_prevweapon = ',';
	key_nextweapon = '.';
	key_invleft = '[';
	key_invright = ']';
	key_message_refresh = '\'';
	key_mission = 'i';				// Strife keys
	key_invpop = 'o';
	key_invkey = 'p';
	key_multi_msgplayer[0] = 'g';
	key_multi_msgplayer[1] = 'h';
	key_multi_msgplayer[2] = 'j';
	key_multi_msgplayer[3] = 'k';
	key_multi_msgplayer[4] = 'v';
	key_multi_msgplayer[5] = 'b';
	key_multi_msgplayer[6] = 'n';
	key_multi_msgplayer[7] = 'm';
	mousebprevweapon = 4;			// Scroll wheel = weapon cycle
	mousebnextweapon = 3;
	snd_musicdevice = 3;
	joybspeed = 29;					// Always run
	vanilla_savegame_limit = 0;
	vanilla_keyboard_mapping = 0;
	vanilla_demo_limit = 0;
	graphical_startup = 0;
	show_endoom = 0;
	dclick_use = 0;
	novert = 1;
	snd_dmxoption = "-opl3 -reverse";
	png_screenshots = 1;
}

static int MainMenuKeyPress(txt::Window* window, int key, void* user_data)
{
	if (key == cheat_sequence[cheat_sequence_index])
	{
		++cheat_sequence_index;

		if (cheat_sequence[cheat_sequence_index] == 0)
		{
			SensibleDefaults();
			cheat_sequence_index = 0;

			window = txt::MessageBox(NULL, "	\x01	");

			return 1;
		}
	}
	else
	{
		cheat_sequence_index = 0;
	}

	return 0;
}

static void DoQuit(void* widget, void* dosave)
{
	if (dosave != NULL)
	{
		M_SaveDefaults();
	}

	txt::Shutdown();

	exit(0);
}

static void QuitConfirm(void* unused1, void* unused2)
{
	txt::Window* window;
	txt::txt_label_t* label;
	txt::Button* yes_button;
	txt::Button* no_button;

	window = txt::NewWindow(NULL);

	txt::AddWidgets(window,
		label = txt::NewLabel("Exiting setup.\nSave settings?"),
		txt::NewStrut(24, 0),
		yes_button = txt::NewButton2(" Yes ", DoQuit, DoQuit),
		no_button = txt::NewButton2(" No	", DoQuit, NULL),
		NULL);

	txt::SetWidgetAlign(label, txt::HORIZ_CENTER);
	txt::SetWidgetAlign(yes_button, txt::HORIZ_CENTER);
	txt::SetWidgetAlign(no_button, txt::HORIZ_CENTER);

	// Only an "abort" button in the middle.
	txt::SetWindowAction(window, txt::HORIZ_LEFT, NULL);
	txt::SetWindowAction(window, txt::HORIZ_CENTER,
		txt::NewWindowAbortAction(window));
	txt::SetWindowAction(window, txt::HORIZ_RIGHT, NULL);
}

static void LaunchDoom(void* unused1, void* unused2)
{
	ExecuteContext* exec;

	// Save configuration first

	M_SaveDefaults();

	// Shut down textscreen GUI

	txt::Shutdown();

	// Launch Doom

	exec = NewExecuteContext();
	PassThroughArguments(exec);
	ExecuteDoom(exec);

	exit(0);
}

static txt::Button* GetLaunchButton()
{
	::std::string label;

	switch (gamemission)
	{
	case doom:
		label = "Save parameters and launch DOOM";
		break;
	case heretic:
		label = "Save parameters and launch Heretic";
		break;
	case hexen:
		label = "Save parameters and launch Hexen";
		break;
	case strife:
		label = "Save parameters and launch STRIFE!";
		break;
	default:
		label = "Save parameters and launch game";
		break;
	}

	return txt::NewButton2(label, LaunchDoom, NULL);
}

void MainMenu()
{
	txt::Window* window;
	txt::WindowAction* quit_action;
	txt::WindowAction* warp_action;

	window = txt::NewWindow("Main Menu");

	txt::SetWindowHelpURL(window, WINDOW_HELP_URL);

	txt::AddWidgets(window,
		txt::NewButton2("Configure Display", (txt::WidgetSignalFunc)ConfigDisplay, NULL),
		txt::NewButton2("Configure Sound", (txt::WidgetSignalFunc)ConfigSound, NULL),
		txt::NewButton2("Configure Keyboard", (txt::WidgetSignalFunc)ConfigKeyboard, NULL),
		txt::NewButton2("Configure Mouse", (txt::WidgetSignalFunc)ConfigMouse, NULL),
		txt::NewButton2("Configure Gamepad/Joystick", (txt::WidgetSignalFunc)ConfigJoystick, NULL),
		txt::NewButton2(gamemission == GameMission::doom ? "Crispness" : "Compatibility", (txt::WidgetSignalFunc)CompatibilitySettings, NULL),
		GetLaunchButton(), txt::NewStrut(0, 1),
		txt::NewButton2("Start a Network Game", (txt::WidgetSignalFunc)StartMultiGame, NULL),
		txt::NewButton2("Join a Network Game", (txt::WidgetSignalFunc)JoinMultiGame, NULL),
		txt::NewButton2("Multiplayer Configuration", (txt::WidgetSignalFunc)MultiplayerConfig, NULL),
		NULL);

	quit_action = txt::NewWindowAction(KEY_ESCAPE, "Quit");
	warp_action = txt::NewWindowAction(KEY_F2, "Warp");
	txt::SignalConnect(quit_action, "pressed", QuitConfirm, NULL);
	txt::SignalConnect(warp_action, "pressed", (txt::WidgetSignalFunc)WarpMenu, NULL);
	txt::SetWindowAction(window, txt:AlignHorizontal::txt::HORIZ_LEFT, quit_action);
	txt::SetWindowAction(window, txt:AlignHorizontal::txt::HORIZ_CENTER, warp_action);

	txt::SetKeyListener(window, MainMenuKeyPress, NULL);
}

//
// Initialize all configuration variables, load config file, etc
//

static void InitConfig()
{
	M_SetConfigDir(NULL);
	InitBindings();

	SetChatMacroDefaults();
	SetPlayerNameDefault();

	M_LoadDefaults();

	// Create and configure the music pack directory if it does not
	// already exist.
	M_SetMusicPackDir();
}

//
// Application icon
//

static void SetIcon()
{
	extern SDL_Window* txt::SDLWindow;
	SDL_Surface* surface;

	surface = SDL_CreateRGBSurfaceFrom((void*)setup_icon_data, setup_icon_w,
		setup_icon_h, 32, setup_icon_w * 4,
		0xff << 24, 0xff << 16,
		0xff << 8, 0xff << 0);

	SDL_SetWindowIcon(txt::SDLWindow, surface);
	SDL_FreeSurface(surface);
}

static void SetWindowTitle()
{
	::std::string title;

	title = M_StringReplace(PACKAGE_NAME " Setup ver " PACKAGE_VERSION,
		"Doom",
		GetGameTitle());

	txt::SetDesktopTitle(title);
}

// Initialize the textscreen library.

static void InitTextscreen()
{
	SetDisplayDriver();

	if (!txt::Init())
	{
		fprintf(stderr, "Failed to initialize GUI\n");
		exit(-1);
	}

	// Set Romero's "funky blue" color:
	// <https://doomwiki.org/wiki/Romero_Blue>
	txt::SetColor(txt::ColorType::blue, 0x04, 0x14, 0x40);

	// Crispy colors for Crispy Setup
	txt::SetColor(txt::ColorType::bright_green, 249, 227, 0); // 0xF9, 0xE3, 0x00
	txt::SetColor(txt::ColorType::cyan, 220, 153, 0);			// 0xDC, 0x99, 0x00
	txt::SetColor(txt::ColorType::bright_cyan, 76, 160, 223); // 0x4C, 0xA0, 0xDF

	SetIcon();
	SetWindowTitle();
}

// Restart the textscreen library. Used when the video_driver variable
// is changed.

void RestartTextscreen()
{
	txt::Shutdown();
	InitTextscreen();
}

//
// Initialize and run the textscreen GUI.
//

static void RunGUI()
{
	InitTextscreen();

	txt::GUIMainLoop();
}

static void MissionSet()
{
	SetWindowTitle();
	InitConfig();
	MainMenu();
}

void D_DoomMain()
{
	SetupMission(MissionSet);

	RunGUI();
}
