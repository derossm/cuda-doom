/**********************************************************************************************************************************************\
	Copyright(C) 2005-2014 Simon Howard

	This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License
	as published by the Free Software Foundation; either version 2 of the License, or (at your option) any later version.

	This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
\**********************************************************************************************************************************************/
#pragma once

#include "../../derma/common.h"

#include "../doomtype.h"
#include "../m_config.h"
#include "../m_controls.h"
#include "../crispy.h"

#include "mode.h"
#include "execute.h"

#include "txt_mouseinput.h"

#define WINDOW_HELP_URL "https://www.chocolate-doom.org/setup-mouse"

extern int novert;

void ConfigMouse(void* widget, void* user_data);
void BindMouseVariables();

static int usemouse = 1;

static int mouseSensitivity = 5;
static int mouseSensitivity_x2 = 5;
static float mouse_acceleration = 2.0;
static int mouse_threshold = 10;
static int mouseSensitivity_y = 5;
static float mouse_acceleration_y = 1.0;
static int mouse_threshold_y = 0;
static int grabmouse = 1;

int novert = 1;

static int* all_mouse_buttons[] = {
	&mousebfire,
	&mousebstrafe,
	&mousebforward,
	&mousebstrafeleft,
	&mousebstraferight,
	&mousebbackward,
	&mousebuse,
	&mousebjump,
	&mousebprevweapon,
	&mousebnextweapon,
	&mousebmouselook,
	&mousebreverse,
	&mousebinvleft,
	&mousebinvright
};

static void MouseSetCallback(txt::UNCAST_ARG(widget), txt::UNCAST_ARG(variable))
{
	txt::CAST_ARG(int, variable);
	unsigned i;

	// Check if the same mouse button is used for a different action
	// If so, set the other action(s) to -1 (unset)

	for (i = 0; i < arrlen(all_mouse_buttons); ++i)
	{
		if (*all_mouse_buttons[i] == *variable
			&& all_mouse_buttons[i] != variable)
		{
			*all_mouse_buttons[i] = -1;
		}
	}
}

static void AddMouseControl(txt::UNCAST_ARG(table), ::std::string label, int* var)
{
	txt::CAST_ARG(txt::txt_table_t, table);
	txt::txt_mouse_input_t* mouse_input;

	txt::AddWidget(table, txt::NewLabel(label));

	mouse_input = txt::NewMouseInput(var);
	txt::AddWidget(table, mouse_input);

	txt::SignalConnect(mouse_input, "set", MouseSetCallback, var);
}

static void ConfigExtraButtons(txt::UNCAST_ARG(widget), txt::UNCAST_ARG(unused))
{
	txt::Window* window;
	txt::txt_table_t* buttons_table;

	window = txt::NewWindow("Additional mouse buttons");

	txt::SetWindowHelpURL(window, WINDOW_HELP_URL);

	txt::AddWidgets(window,
		buttons_table = txt::NewTable(4),
		NULL);

	txt::SetColumnWidths(buttons_table, 16, 11, 14, 10);

	AddMouseControl(buttons_table, "Move forward", &mousebforward);
	AddMouseControl(buttons_table, "Strafe left", &mousebstrafeleft);
	AddMouseControl(buttons_table, "Move backward", &mousebbackward);
	AddMouseControl(buttons_table, "Strafe right", &mousebstraferight);
	AddMouseControl(buttons_table, "Previous weapon", &mousebprevweapon);
	AddMouseControl(buttons_table, "Strafe on", &mousebstrafe);
	AddMouseControl(buttons_table, "Next weapon", &mousebnextweapon);

	if (gamemission == heretic)
	{
		AddMouseControl(buttons_table, "Inventory left", &mousebinvleft);
		AddMouseControl(buttons_table, "Inventory right", &mousebinvright);
	}

	if (gamemission == hexen || gamemission == strife)
	{
		AddMouseControl(buttons_table, "Jump", &mousebjump);
	}

	if (gamemission == doom)
	{
		AddMouseControl(buttons_table, "Quick Reverse", &mousebreverse);
		AddMouseControl(buttons_table, "Mouse Look [*]", &mousebmouselook);
		AddMouseControl(buttons_table, "Jump [*]", &mousebjump);
	}
}

void ConfigMouse(txt::UNCAST_ARG(widget), void* user_data)
{
	txt::Window* window;

	window = txt::NewWindow("Mouse configuration");

	txt::SetTableColumns(window, 2);

	txt::SetWindowAction(window, txt::HORIZ_CENTER, TestConfigAction());
	txt::SetWindowHelpURL(window, WINDOW_HELP_URL);

	if (gamemission == doom)
	{
		txt::AddWidgets(window,
			txt::NewCheckBox("Enable mouse", &usemouse),
			txt::TABLE_OVERFLOW_RIGHT,
			txt::NewInvertedCheckBox("Allow vertical mouse movement",
				&novert),
			txt::TABLE_OVERFLOW_RIGHT,
			txt::NewCheckBox("Grab mouse in windowed mode",
				&grabmouse),
			txt::TABLE_OVERFLOW_RIGHT,
			txt::NewCheckBox("Double click acts as \"use\"",
				&dclick_use),
			txt::TABLE_OVERFLOW_RIGHT,

			txt::NewSeparator("Mouse motion"),
			txt::NewLabel("Speed (h/turn)"),
			txt::NewSpinControl(&mouseSensitivity, 0, 255), // extended range
			txt::NewLabel("Speed (h/strafe)"),
			txt::NewSpinControl(&mouseSensitivity_x2, 0, 255), // extended range
			txt::NewLabel("Acceleration (h)"),
			txt::NewFloatSpinControl(&mouse_acceleration, 1.0, 5.0),
			txt::NewLabel("Acceleration threshold (h)"),
			txt::NewSpinControl(&mouse_threshold, 0, 32),
			txt::NewLabel("Speed (v)"),
			txt::NewSpinControl(&mouseSensitivity_y, 0, 255), // extended range
			txt::NewLabel("Acceleration (v)"),
			txt::NewFloatSpinControl(&mouse_acceleration_y, 1.0, 5.0),
			txt::NewLabel("Acceleration threshold (v)"),
			txt::NewSpinControl(&mouse_threshold_y, 0, 32),

			txt::NewSeparator("Buttons"),
			NULL);
	}
	else
	{
		txt::AddWidgets(window,
			txt::NewCheckBox("Enable mouse", &usemouse),
			txt::TABLE_OVERFLOW_RIGHT,
			txt::NewInvertedCheckBox("Allow vertical mouse movement",
				&novert),
			txt::TABLE_OVERFLOW_RIGHT,
			txt::NewCheckBox("Grab mouse in windowed mode",
				&grabmouse),
			txt::TABLE_OVERFLOW_RIGHT,
			txt::NewCheckBox("Double click acts as \"use\"",
				&dclick_use),
			txt::TABLE_OVERFLOW_RIGHT,

			txt::NewSeparator("Mouse motion"),
			txt::NewLabel("Speed"),
			txt::NewSpinControl(&mouseSensitivity, 1, 256),
			txt::NewLabel("Acceleration"),
			txt::NewFloatSpinControl(&mouse_acceleration, 1.0, 5.0),
			txt::NewLabel("Acceleration threshold"),
			txt::NewSpinControl(&mouse_threshold, 0, 32),

			txt::NewSeparator("Buttons"),
			NULL);
	}

	AddMouseControl(window, "Fire/Attack", &mousebfire);
	AddMouseControl(window, "Use", &mousebuse);

	txt::AddWidget(window,
		txt::NewButton2("More controls...", ConfigExtraButtons, NULL));
}

void BindMouseVariables()
{
	M_BindVariable<int>("use_mouse", &usemouse);
	M_BindVariable<int>("novert", &novert);
	M_BindVariable<int>("grabmouse", &grabmouse);
	M_BindVariable<int>("mouse_sensitivity", &mouseSensitivity);
	M_BindVariable<int>("mouse_threshold", &mouse_threshold);
	M_BindFloatVariable("mouse_acceleration", &mouse_acceleration);
	if (gamemission == doom)
	{
		M_BindVariable<int>("mouse_sensitivity_x2", &mouseSensitivity_x2);
		M_BindVariable<int>("mouse_sensitivity_y", &mouseSensitivity_y);
		M_BindVariable<int>("mouse_threshold_y", &mouse_threshold_y);
		M_BindFloatVariable("mouse_acceleration_y", &mouse_acceleration_y);
		M_BindVariable<int>("crispy_mouselook", &crispy->mouselook);
	}
}
