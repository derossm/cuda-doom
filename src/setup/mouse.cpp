/**********************************************************************************************************************************************\
	Copyright(C) 2005-2014 Simon Howard

	This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License
	as published by the Free Software Foundation; either version 2 of the License, or (at your option) any later version.

	This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
\**********************************************************************************************************************************************/


#include "crispy.h"
#include "textscreen.h"
#include "doomtype.h"
#include "m_config.h"
#include "m_controls.h"

#include "execute.h"
#include "txt_mouseinput.h"

#include "mode.h"
#include "mouse.h"

#define WINDOW_HELP_URL "https://www.chocolate-doom.org/setup-mouse"

static int usemouse = 1;

static int mouseSensitivity = 5;
static int mouseSensitivity_x2 = 5; // [crispy]
static float mouse_acceleration = 2.0;
static int mouse_threshold = 10;
static int mouseSensitivity_y = 5; // [crispy]
static float mouse_acceleration_y = 1.0; // [crispy]
static int mouse_threshold_y = 0; // [crispy]
static int grabmouse = 1;

int novert = 1;

static int *all_mouse_buttons[] = {
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
	&mousebmouselook, // [crispy]
	&mousebreverse, // [crispy]
	&mousebinvleft,
	&mousebinvright
};

static void MouseSetCallback(cudadoom::txt::TXT_UNCAST_ARG(widget), cudadoom::txt::TXT_UNCAST_ARG(variable))
{
	cudadoom::txt::TXT_CAST_ARG(int, variable);
	unsigned int i;

	// Check if the same mouse button is used for a different action
	// If so, set the other action(s) to -1 (unset)

	for (i=0; i<arrlen(all_mouse_buttons); ++i)
	{
		if (*all_mouse_buttons[i] == *variable
			&& all_mouse_buttons[i] != variable)
		{
			*all_mouse_buttons[i] = -1;
		}
	}
}

static void AddMouseControl(cudadoom::txt::TXT_UNCAST_ARG(table), const char *label, int *var)
{
	cudadoom::txt::TXT_CAST_ARG(cudadoom::txt::txt_table_t, table);
	cudadoom::txt::txt_mouse_input_t *mouse_input;

	cudadoom::txt::TXT_AddWidget(table, cudadoom::txt::TXT_NewLabel(label));

	mouse_input = cudadoom::txt::TXT_NewMouseInput(var);
	cudadoom::txt::TXT_AddWidget(table, mouse_input);

	cudadoom::txt::TXT_SignalConnect(mouse_input, "set", MouseSetCallback, var);
}

static void ConfigExtraButtons(cudadoom::txt::TXT_UNCAST_ARG(widget), cudadoom::txt::TXT_UNCAST_ARG(unused))
{
	cudadoom::txt::txt_window_t *window;
	cudadoom::txt::txt_table_t *buttons_table;

	window = cudadoom::txt::TXT_NewWindow("Additional mouse buttons");

	cudadoom::txt::TXT_SetWindowHelpURL(window, WINDOW_HELP_URL);

	cudadoom::txt::TXT_AddWidgets(window,
					buttons_table = cudadoom::txt::TXT_NewTable(4),
					NULL);

	cudadoom::txt::TXT_SetColumnWidths(buttons_table, 16, 11, 14, 10);

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

	if (gamemission == doom) // [crispy]
	{
		AddMouseControl(buttons_table, "Quick Reverse", &mousebreverse);
		AddMouseControl(buttons_table, "Mouse Look [*]", &mousebmouselook);
		AddMouseControl(buttons_table, "Jump [*]", &mousebjump);
	}
}

void ConfigMouse(cudadoom::txt::TXT_UNCAST_ARG(widget), void *user_data)
{
	cudadoom::txt::txt_window_t *window;

	window = cudadoom::txt::TXT_NewWindow("Mouse configuration");

	cudadoom::txt::TXT_SetTableColumns(window, 2);

	cudadoom::txt::TXT_SetWindowAction(window, cudadoom::txt::TXT_HORIZ_CENTER, TestConfigAction());
	cudadoom::txt::TXT_SetWindowHelpURL(window, WINDOW_HELP_URL);

	if (gamemission == doom) // [crispy]
	{
	cudadoom::txt::TXT_AddWidgets(window,
					cudadoom::txt::TXT_NewCheckBox("Enable mouse", &usemouse),
					cudadoom::txt::TXT_TABLE_OVERFLOW_RIGHT,
					cudadoom::txt::TXT_NewInvertedCheckBox("Allow vertical mouse movement",
											&novert),
					cudadoom::txt::TXT_TABLE_OVERFLOW_RIGHT,
					cudadoom::txt::TXT_NewCheckBox("Grab mouse in windowed mode",
									&grabmouse),
					cudadoom::txt::TXT_TABLE_OVERFLOW_RIGHT,
					cudadoom::txt::TXT_NewCheckBox("Double click acts as \"use\"",
									&dclick_use),
					cudadoom::txt::TXT_TABLE_OVERFLOW_RIGHT,

					cudadoom::txt::TXT_NewSeparator("Mouse motion"),
					cudadoom::txt::TXT_NewLabel("Speed (h/turn)"),
					cudadoom::txt::TXT_NewSpinControl(&mouseSensitivity, 0, 255), // [crispy] extended range
					cudadoom::txt::TXT_NewLabel("Speed (h/strafe)"),
					cudadoom::txt::TXT_NewSpinControl(&mouseSensitivity_x2, 0, 255), // [crispy] extended range
					cudadoom::txt::TXT_NewLabel("Acceleration (h)"),
					cudadoom::txt::TXT_NewFloatSpinControl(&mouse_acceleration, 1.0, 5.0),
					cudadoom::txt::TXT_NewLabel("Acceleration threshold (h)"),
					cudadoom::txt::TXT_NewSpinControl(&mouse_threshold, 0, 32),
					cudadoom::txt::TXT_NewLabel("Speed (v)"),
					cudadoom::txt::TXT_NewSpinControl(&mouseSensitivity_y, 0, 255), // [crispy] extended range
					cudadoom::txt::TXT_NewLabel("Acceleration (v)"),
					cudadoom::txt::TXT_NewFloatSpinControl(&mouse_acceleration_y, 1.0, 5.0),
					cudadoom::txt::TXT_NewLabel("Acceleration threshold (v)"),
					cudadoom::txt::TXT_NewSpinControl(&mouse_threshold_y, 0, 32),

					cudadoom::txt::TXT_NewSeparator("Buttons"),
					NULL);
	}
	else
	{
	cudadoom::txt::TXT_AddWidgets(window,
					cudadoom::txt::TXT_NewCheckBox("Enable mouse", &usemouse),
					cudadoom::txt::TXT_TABLE_OVERFLOW_RIGHT,
					cudadoom::txt::TXT_NewInvertedCheckBox("Allow vertical mouse movement",
											&novert),
					cudadoom::txt::TXT_TABLE_OVERFLOW_RIGHT,
					cudadoom::txt::TXT_NewCheckBox("Grab mouse in windowed mode",
									&grabmouse),
					cudadoom::txt::TXT_TABLE_OVERFLOW_RIGHT,
					cudadoom::txt::TXT_NewCheckBox("Double click acts as \"use\"",
									&dclick_use),
					cudadoom::txt::TXT_TABLE_OVERFLOW_RIGHT,

					cudadoom::txt::TXT_NewSeparator("Mouse motion"),
					cudadoom::txt::TXT_NewLabel("Speed"),
					cudadoom::txt::TXT_NewSpinControl(&mouseSensitivity, 1, 256),
					cudadoom::txt::TXT_NewLabel("Acceleration"),
					cudadoom::txt::TXT_NewFloatSpinControl(&mouse_acceleration, 1.0, 5.0),
					cudadoom::txt::TXT_NewLabel("Acceleration threshold"),
					cudadoom::txt::TXT_NewSpinControl(&mouse_threshold, 0, 32),

					cudadoom::txt::TXT_NewSeparator("Buttons"),
					NULL);
	}

	AddMouseControl(window, "Fire/Attack", &mousebfire);
	AddMouseControl(window, "Use", &mousebuse);

	cudadoom::txt::TXT_AddWidget(window,
					cudadoom::txt::TXT_NewButton2("More controls...", ConfigExtraButtons, NULL));
}

void BindMouseVariables()
{
	M_BindIntVariable("use_mouse",				&usemouse);
	M_BindIntVariable("novert",					&novert);
	M_BindIntVariable("grabmouse",				&grabmouse);
	M_BindIntVariable("mouse_sensitivity",		&mouseSensitivity);
	M_BindIntVariable("mouse_threshold",			&mouse_threshold);
	M_BindFloatVariable("mouse_acceleration",	&mouse_acceleration);
	if (gamemission == doom) // [crispy]
	{
	M_BindIntVariable("mouse_sensitivity_x2",	&mouseSensitivity_x2);
	M_BindIntVariable("mouse_sensitivity_y",		&mouseSensitivity_y);
	M_BindIntVariable("mouse_threshold_y",		&mouse_threshold_y);
	M_BindFloatVariable("mouse_acceleration_y", &mouse_acceleration_y);
	M_BindIntVariable("crispy_mouselook",		&crispy->mouselook);
	}
}
