/**********************************************************************************************************************************************\
	Copyright(C) 2005-2014 Simon Howard

	This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License
	as published by the Free Software Foundation; either version 2 of the License, or (at your option) any later version.

	This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
\**********************************************************************************************************************************************/
#pragma once

#include "../../derma/common.h"

#include "SDL.h"

#include "../../textscreen/txt_widget.h"
#include "../../textscreen/txt_window.h"

enum class txt_joystick_axis_direction_t
{
	JOYSTICK_AXIS_HORIZONTAL,
	JOYSTICK_AXIS_VERTICAL
};

enum class txt_joystick_axis_stage_t
{
	CONFIG_CENTER,		// "Center the joystick and press a button..."
	CONFIG_STAGE1,		// "Top or left and press a button..."
	CONFIG_STAGE2		// [Optional] "Bottom or right and press a button..."
};

// Callback invoked when calibration is completed.
typedef void (*txt_joystick_axis_callback_t)();

struct txt_joystick_axis_t
{
	Widget widget;
	int *axis;
	int *invert;
	txt_joystick_axis_direction_t dir;

	// Only used when configuring:

	// Configuration prompt window and label.
	Window* config_window;
	txt_label_t* config_label;

	// SDL joystick handle for reading joystick state.
	SDL_Joystick *joystick;

	// "Bad" joystick axes. Sometimes an axis can be stuck or "bad". An
	// example I found is that if you unplug the nunchuck extension from
	// a Wii remote, the axes from the nunchuck can be stuck at one of
	// the maximum values. These have to be ignored, so when we ask the
	// user to center the joystick, we look for bad axes that are not
	// close to zero.
	bool *bad_axis;

	// Stage we have reached in configuring joystick axis.
	txt_joystick_axis_stage_t config_stage;

	// Button to press to advance to next stage.
	int config_button;

	// Callback invoked when the axis is calibrated.
	txt_joystick_axis_callback_t callback;
};

txt_joystick_axis_t* NewJoystickAxis(int *axis, int *invert, txt_joystick_axis_direction_t dir);

// Configure a joystick axis widget.
// axis: The axis widget to configure.
// using_button: If non-negative, use this joystick button as the button to expect from the user. Otherwise, ask.
void ConfigureJoystickAxis(txt_joystick_axis_t* axis, int using_button, txt_joystick_axis_callback_t callback);
