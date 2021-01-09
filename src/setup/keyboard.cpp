/**********************************************************************************************************************************************\
	Copyright(C) 2005-2014 Simon Howard

	This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License
	as published by the Free Software Foundation; either version 2 of the License, or (at your option) any later version.

	This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
\**********************************************************************************************************************************************/

#include <string>

#include "keyboard.h"

#include "../../textscreen/textscreen.h"

#include "doomtype.h"
#include "m_config.h"
#include "m_controls.h"
#include "m_misc.h"

#include "execute.h"
#include "mode.h"
#include "joystick.h"

#include "txt_keyinput.h"

#define WINDOW_HELP_URL "https://www.chocolate-doom.org/setup-keyboard"

int vanilla_keyboard_mapping = 1;

static int always_run = 0;

// Keys within these groups cannot have the same value.

static int* controls[] = {&key_left, &key_right, &key_up, &key_down,
							&key_alt_up, &key_alt_down,
							&key_reverse, &key_toggleautorun, &key_togglenovert,
							&key_strafeleft, &key_straferight, &key_fire,
							&key_alt_strafeleft, &key_alt_straferight,
							&key_use, &key_strafe, &key_speed, &key_jump,
							&key_flyup, &key_flydown, &key_flycenter,
							&key_lookup, &key_lookdown, &key_lookcenter,
							&key_invleft, &key_invright, &key_invquery,
							&key_invuse, &key_invpop, &key_mission, &key_invkey,
							&key_invhome, &key_invend, &key_invdrop,
							&key_useartifact, &key_pause, &key_usehealth,
							&key_weapon1, &key_weapon2, &key_weapon3,
							&key_weapon4, &key_weapon5, &key_weapon6,
							&key_weapon7, &key_weapon8,
							&key_arti_quartz, &key_arti_urn, &key_arti_bomb,
							&key_arti_tome, &key_arti_ring, &key_arti_chaosdevice,
							&key_arti_shadowsphere, &key_arti_wings,
							&key_arti_torch, &key_arti_morph,
							&key_arti_all, &key_arti_health, &key_arti_poisonbag,
							&key_arti_blastradius, &key_arti_teleport,
							&key_arti_teleportother, &key_arti_egg,
							&key_arti_invulnerability,
							&key_prevweapon, &key_nextweapon, NULL};

static int* menu_nav[] = {&key_menu_activate, &key_menu_up, &key_menu_down,
							&key_menu_left, &key_menu_right, &key_menu_back,
							&key_menu_forward, &key_menu_del, NULL};

static int* shortcuts[] = {&key_menu_help, &key_menu_save, &key_menu_load,
							&key_menu_volume, &key_menu_detail, &key_menu_qsave,
							&key_menu_endgame, &key_menu_messages, &key_spy,
							&key_menu_qload, &key_menu_quit, &key_menu_gamma,
							&key_menu_nextlevel, &key_menu_reloadlevel,
							&key_menu_incscreen, &key_menu_decscreen,
							&key_menu_screenshot, &key_menu_cleanscreenshot,
							&key_message_refresh, &key_multi_msg,
							&key_multi_msgplayer[0], &key_multi_msgplayer[1],
							&key_multi_msgplayer[2], &key_multi_msgplayer[3]};

static int* map_keys[] = {&key_map_north, &key_map_south, &key_map_east,
							&key_map_west, &key_map_zoomin, &key_map_zoomout,
							&key_map_toggle, &key_map_maxzoom, &key_map_follow,
							&key_map_grid, &key_map_mark, &key_map_clearmark,
							&key_map_overlay, &key_map_rotate,
							NULL};

static void UpdateJoybSpeed()
{
	if (always_run)
	{
		/*
			<Janizdreg> if you want to pick one for chocolate doom to use,
						pick 29, since that is the most universal one that
						also works with heretic, hexen and strife =P

			NB. This choice also works with original, ultimate and final exes.
		*/

		joybspeed = 29;
	}
	else
	{
		joybspeed = 2;
	}
}

static int VarInGroup(int* variable, int** group)
{
	unsigned i;

	for (i = 0; group[i] != NULL; ++i)
	{
		if (group[i] == variable)
		{
			return 1;
		}
	}

	return 0;
}

static void CheckKeyGroup(int* variable, int** group)
{
	unsigned i;

	// Don't check unless the variable is in this group.

	if (!VarInGroup(variable, group))
	{
		return;
	}

	// If another variable has the same value as the new value, reset it.

	for (i = 0; group[i] != NULL; ++i)
	{
		if (*variable == *group[i] && group[i] != variable)
		{
			// A different key has the same value. Clear the existing
			// value. This ensures that no two keys can have the same
			// value.

			*group[i] = 0;
		}
	}
}

// Callback invoked when a key control is set

static void KeySetCallback(cudadoom::txt::UNCAST_ARG(widget), cudadoom::txt::UNCAST_ARG(variable))
{
	cudadoom::txt::CAST_ARG(int, variable);

	CheckKeyGroup(variable, controls);
	CheckKeyGroup(variable, menu_nav);
	CheckKeyGroup(variable, shortcuts);
	CheckKeyGroup(variable, map_keys);
}

// Add a label and keyboard input to the specified table.

static void AddKeyControl(cudadoom::txt::Table* table, std::string name, int* var)
{
	table->AddWidget(std::make_unique<cudadoom::txt::Label>(name));
	table->AddWidget(std::make_unique<cudadoom::txt::txt_key_input_t>(var));

	cudadoom::txt::SignalConnect(key_input, "set", KeySetCallback, var);
}

static void AddSectionLabel(cudadoom::txt::UNCAST_ARG(table), std::string title,
	bool add_space)
{
	cudadoom::txt::CAST_ARG(cudadoom::txt::txt_table_t, table);
	char buf[64];

	if (add_space)
	{
		cudadoom::txt::AddWidgets(table,
			cudadoom::txt::NewStrut(0, 1),
			cudadoom::txt::TABLE_EOL,
			NULL);
	}

	M_snprintf(buf, sizeof(buf), " - %s - ", title);

	cudadoom::txt::AddWidgets(table,
		cudadoom::txt::NewLabel(buf),
		cudadoom::txt::TABLE_EOL,
		NULL);
}
static void ConfigExtraKeys(cudadoom::txt::UNCAST_ARG(widget), cudadoom::txt::UNCAST_ARG(unused))
{
	cudadoom::txt::Window* window;
	cudadoom::txt::txt_scrollpane_t* scrollpane;
	cudadoom::txt::txt_table_t* table;
	bool extra_keys = gamemission == GameMission::heretic
		|| gamemission == GameMission::hexen
		|| gamemission == GameMission::strife;

	window = cudadoom::txt::NewWindow("Extra keyboard controls");

	window->SetWindowHelpURL(WINDOW_HELP_URL);

	table = cudadoom::txt::NewTable(2);

	cudadoom::txt::SetColumnWidths(table, 21, 9);

	if (extra_keys || 1) // Crispy
	{
		// When we have extra controls, a scrollable pane must be used.

		scrollpane = cudadoom::txt::NewScrollPane(0, 13, table);
		window->AddWidget(scrollpane);

		if (gamemission == GameMission::doom)
		{
			table->AddSectionLabel("View", false);

			table->AddKeyControl("Look up [*]", &key_lookup);
			table->AddKeyControl("Look down [*]", &key_lookdown);
			table->AddKeyControl("Center view [*]", &key_lookcenter);

			table->AddSectionLabel("Movement", false);
			table->AddKeyControl("Move Forward (alt.)", &key_alt_up);
			table->AddKeyControl("Move Backward (alt.)", &key_alt_down);
			table->AddKeyControl("Strafe Left (alt.)", &key_alt_strafeleft);
			table->AddKeyControl("Strafe Right (alt.)", &key_alt_straferight);
			table->AddKeyControl("Toggle always run", &key_toggleautorun);
			table->AddKeyControl("Toggle vert. mouse", &key_togglenovert);
			table->AddKeyControl("Quick Reverse", &key_reverse);
		}
		else if (gamemission == GameMission::heretic)
		{
			table->AddSectionLabel("View", false);

			table->AddKeyControl("Look up", &key_lookup);
			table->AddKeyControl("Look down", &key_lookdown);
			table->AddKeyControl("Center view", &key_lookcenter);

			table->AddSectionLabel("Movement", false);
			table->AddKeyControl("Move Forward (alt.)", &key_alt_up);
			table->AddKeyControl("Move Backward (alt.)", &key_alt_down);
			table->AddKeyControl("Strafe Left (alt.)", &key_alt_strafeleft);
			table->AddKeyControl("Strafe Right (alt.)", &key_alt_straferight);
			table->AddKeyControl("Toggle always run", &key_toggleautorun);
			table->AddKeyControl("Toggle vert. mouse", &key_togglenovert);
		}
		else
		{
			table->AddSectionLabel("View", false);

			table->AddKeyControl("Look up", &key_lookup);
			table->AddKeyControl("Look down", &key_lookdown);
			table->AddKeyControl("Center view", &key_lookcenter);
		}

		if (gamemission == GameMission::heretic || gamemission == GameMission::hexen)
		{
			table->AddSectionLabel("Flying", true);

			table->AddKeyControl("Fly up", &key_flyup);
			table->AddKeyControl("Fly down", &key_flydown);
			table->AddKeyControl("Fly center", &key_flycenter);
		}

		if (gamemission != GameMission::doom)
		{
			table->AddSectionLabel("Inventory", true);

			table->AddKeyControl("Inventory left", &key_invleft);
			table->AddKeyControl("Inventory right", &key_invright);
		}

		if (gamemission == GameMission::strife)
		{
			table->AddKeyControl("Home", &key_invhome);
			table->AddKeyControl("End", &key_invend);
			table->AddKeyControl("Query", &key_invquery);
			table->AddKeyControl("Drop", &key_invdrop);
			table->AddKeyControl("Show weapons", &key_invpop);
			table->AddKeyControl("Show mission", &key_mission);
			table->AddKeyControl("Show keys", &key_invkey);
			table->AddKeyControl("Use", &key_invuse);
			table->AddKeyControl("Use health", &key_usehealth);
		}
		else
			if (gamemission == GameMission::heretic || gamemission == GameMission::hexen)
			{
				table->AddKeyControl("Use artifact", &key_useartifact);
			}

		if (gamemission == GameMission::heretic)
		{
			table->AddSectionLabel("Artifacts", true);

			table->AddKeyControl("Quartz Flask", &key_arti_quartz);
			table->AddKeyControl("Mystic Urn", &key_arti_urn);
			table->AddKeyControl("Timebomb", &key_arti_bomb);
			table->AddKeyControl("Tome of Power", &key_arti_tome);
			table->AddKeyControl("Ring of Invincibility ", &key_arti_ring);
			table->AddKeyControl("Chaos Device", &key_arti_chaosdevice);
			table->AddKeyControl("Shadowsphere", &key_arti_shadowsphere);
			table->AddKeyControl("Wings of Wrath", &key_arti_wings);
			table->AddKeyControl("Torch", &key_arti_torch);
			table->AddKeyControl("Morph Ovum", &key_arti_morph);
		}

		if (gamemission == GameMission::hexen)
		{
			table->AddSectionLabel("Artifacts", true);

			table->AddKeyControl("One of each", &key_arti_all);
			table->AddKeyControl("Quartz Flask", &key_arti_health);
			table->AddKeyControl("Flechette", &key_arti_poisonbag);
			table->AddKeyControl("Disc of Repulsion", &key_arti_blastradius);
			table->AddKeyControl("Chaos Device", &key_arti_teleport);
			table->AddKeyControl("Banishment Device", &key_arti_teleportother);
			table->AddKeyControl("Porkalator", &key_arti_egg);
			table->AddKeyControl("Icon of the Defender",
				&key_arti_invulnerability);
		}
	}
	else
	{
		window->AddWidget(table);
	}

	table->AddSectionLabel("Weapons", extra_keys);

	table->AddKeyControl("Weapon 1", &key_weapon1);
	table->AddKeyControl("Weapon 2", &key_weapon2);
	table->AddKeyControl("Weapon 3", &key_weapon3);
	table->AddKeyControl("Weapon 4", &key_weapon4);
	table->AddKeyControl("Weapon 5", &key_weapon5);
	table->AddKeyControl("Weapon 6", &key_weapon6);
	table->AddKeyControl("Weapon 7", &key_weapon7);
	table->AddKeyControl("Weapon 8", &key_weapon8);
	table->AddKeyControl("Previous weapon", &key_prevweapon);
	table->AddKeyControl("Next weapon", &key_nextweapon);
}

static void OtherKeysDialog(cudadoom::txt::UNCAST_ARG(widget), cudadoom::txt::UNCAST_ARG(unused))
{
	auto window = std::make_unique<cudadoom::txt::Window>("Other keys");
	window->SetWindowHelpURL(WINDOW_HELP_URL);

	auto table = std::make_unique<cudadoom::txt::Table>(2);

	table->SetColumnWidths(25, 9);

	table->AddSectionLabel("Menu navigation", false);

	table->AddKeyControl("Activate menu", &key_menu_activate);
	table->AddKeyControl("Move cursor up", &key_menu_up);
	table->AddKeyControl("Move cursor down", &key_menu_down);
	table->AddKeyControl("Move slider left", &key_menu_left);
	table->AddKeyControl("Move slider right", &key_menu_right);
	table->AddKeyControl("Go to previous menu", &key_menu_back);
	table->AddKeyControl("Activate menu item", &key_menu_forward);
	table->AddKeyControl("Confirm action", &key_menu_confirm);
	table->AddKeyControl("Cancel action", &key_menu_abort);
	table->AddKeyControl("Delete savegame", &key_menu_del);

	table->AddSectionLabel("Shortcut keys", true);

	table->AddKeyControl("Pause game", &key_pause);
	table->AddKeyControl("Help screen", &key_menu_help);
	table->AddKeyControl("Save game", &key_menu_save);
	table->AddKeyControl("Load game", &key_menu_load);
	table->AddKeyControl("Sound volume", &key_menu_volume);
	table->AddKeyControl("Toggle detail", &key_menu_detail);
	table->AddKeyControl("Quick save", &key_menu_qsave);
	table->AddKeyControl("End game", &key_menu_endgame);
	table->AddKeyControl("Toggle messages", &key_menu_messages);
	table->AddKeyControl("Quick load", &key_menu_qload);
	table->AddKeyControl("Quit game", &key_menu_quit);
	table->AddKeyControl("Toggle gamma", &key_menu_gamma);
	table->AddKeyControl("Multiplayer spy", &key_spy);
	table->AddKeyControl("Go to next level", &key_menu_nextlevel);
	table->AddKeyControl("Restart level/demo", &key_menu_reloadlevel);

	table->AddKeyControl("Increase screen size", &key_menu_incscreen);
	table->AddKeyControl("Decrease screen size", &key_menu_decscreen);
	table->AddKeyControl("Save a screenshot", &key_menu_screenshot);
	table->AddKeyControl("Save a clean screenshot", &key_menu_cleanscreenshot);

	table->AddKeyControl("Display last message", &key_message_refresh);
	table->AddKeyControl("Finish recording demo", &key_demo_quit);

	table->AddSectionLabel("Map", true);
	table->AddKeyControl("Toggle map", &key_map_toggle);
	table->AddKeyControl("Zoom in", &key_map_zoomin);
	table->AddKeyControl("Zoom out", &key_map_zoomout);
	table->AddKeyControl("Maximum zoom out", &key_map_maxzoom);
	table->AddKeyControl("Follow mode", &key_map_follow);
	table->AddKeyControl("Pan north", &key_map_north);
	table->AddKeyControl("Pan south", &key_map_south);
	table->AddKeyControl("Pan east", &key_map_east);
	table->AddKeyControl("Pan west", &key_map_west);
	table->AddKeyControl("Toggle grid", &key_map_grid);
	table->AddKeyControl("Mark location", &key_map_mark);
	table->AddKeyControl("Clear all marks", &key_map_clearmark);
	table->AddKeyControl("Overlay mode", &key_map_overlay);
	table->AddKeyControl("Rotate mode", &key_map_rotate);

	table->AddSectionLabel("Multiplayer", true);

	table->AddKeyControl("Send message", &key_multi_msg);
	table->AddKeyControl("- to player 1", &key_multi_msgplayer[0]);
	table->AddKeyControl("- to player 2", &key_multi_msgplayer[1]);
	table->AddKeyControl("- to player 3", &key_multi_msgplayer[2]);
	table->AddKeyControl("- to player 4", &key_multi_msgplayer[3]);

	if (gamemission == GameMission::hexen || gamemission == GameMission::strife)
	{
		table->AddKeyControl("- to player 5", &key_multi_msgplayer[4]);
		table->AddKeyControl("- to player 6", &key_multi_msgplayer[5]);
		table->AddKeyControl("- to player 7", &key_multi_msgplayer[6]);
		table->AddKeyControl("- to player 8", &key_multi_msgplayer[7]);
	}

	table->AddWidget(std::make_unique<cudadoom::txt::txt_scrollpane_t>(0, 13, table));
}

void ConfigKeyboard(cudadoom::txt::UNCAST_ARG(widget), void* user_data)
{
	cudadoom::txt::Window* window;
	cudadoom::txt::CheckBox* run_control;

	always_run = joybspeed >= 20;

	window = cudadoom::txt::NewWindow("Keyboard configuration");

	window->SetWindowHelpURL(WINDOW_HELP_URL);

	// The window is on a 5-column grid layout that looks like:
	// Label | Control | | Label | Control
	// There is a small gap between the two conceptual "columns" of
	// controls, just for spacing.
	window->SetTableColumns(5);
	window->SetColumnWidths(15, 8, 2, 15, 8);

	window->AddWidget(cudadoom::txt::NewSeparator("Movement"));
	window->AddKeyControl("Move Forward", &key_up);
	window->AddWidget(cudadoom::txt::TABLE_EMPTY);
	window->AddKeyControl("Strafe Left", &key_strafeleft);

	window->AddKeyControl("Move Backward", &key_down);
	window->AddWidget(cudadoom::txt::TABLE_EMPTY);
	window->AddKeyControl("Strafe Right", &key_straferight);

	window->AddKeyControl("Turn Left", &key_left);
	window->AddWidget(cudadoom::txt::TABLE_EMPTY);
	window->AddKeyControl("Speed On", &key_speed);

	window->AddKeyControl("Turn Right", &key_right);
	window->AddWidget(cudadoom::txt::TABLE_EMPTY);
	window->AddKeyControl("Strafe On", &key_strafe);

	if (gamemission == hexen || gamemission == strife)
	{
		window->AddKeyControl("Jump", &key_jump);
	}
	else
		if (gamemission == doom) // Crispy
		{
			window->AddKeyControl("Jump [*]", &key_jump);
		}

	window->AddWidget(cudadoom::txt::NewSeparator("Action"));
	window->AddKeyControl("Fire/Attack", &key_fire);
	window->AddWidget(cudadoom::txt::TABLE_EMPTY);
	window->AddKeyControl("Use", &key_use);

	window.AddWidgets(
		cudadoom::txt::NewButton2("More controls...", ConfigExtraKeys, NULL),
		cudadoom::txt::TABLE_OVERFLOW_RIGHT,
		cudadoom::txt::TABLE_EMPTY,
		cudadoom::txt::NewButton2("Other keys...", OtherKeysDialog, NULL),
		cudadoom::txt::TABLE_OVERFLOW_RIGHT,

		cudadoom::txt::NewSeparator("Misc."),
		run_control = cudadoom::txt::NewCheckBox("Always run", &always_run),
		cudadoom::txt::TABLE_EOL,
		cudadoom::txt::NewInvertedCheckBox("Use native keyboard mapping", &vanilla_keyboard_mapping),
		cudadoom::txt::TABLE_EOL,
		NULL);

	run_control->SignalConnect("changed", UpdateJoybSpeed, NULL);
	window->SetWindowAction(cudadoom::txt::HORIZ_CENTER, TestConfigAction());
}

void BindKeyboardVariables()
{
	M_BindIntVariable("vanilla_keyboard_mapping", &vanilla_keyboard_mapping);
}
