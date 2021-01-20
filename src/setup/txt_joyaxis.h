/**********************************************************************************************************************************************\
	Copyright(C) 2005-2014 Simon Howard

	This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License
	as published by the Free Software Foundation; either version 2 of the License, or (at your option) any later version.

	This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
\**********************************************************************************************************************************************/
#pragma once

#include "../../derma/common.h"

#include "../../textscreen/txt_widget.h"
#include "../../textscreen/txt_window.h"
#include "../../textscreen/txt_gui.h"
#include "../../textscreen/txt_io.h"

#include "../i_joystick.h"
#include "../i_system.h"
#include "../m_controls.h"
#include "../m_misc.h"

#include "joystick.h"

enum class txt_joystick_axis_direction_t
{
	JOYSTICK_AXIS_HORIZONTAL,
	JOYSTICK_AXIS_VERTICAL
};

enum class txt_joystick_axis_stage_t
{
	CONFIG_CENTER,		// "Center the joystick and press a button..."
	CONFIG_STAGE1,		// "Top or left and press a button..."
	CONFIG_STAGE2		// Optional: "Bottom or right and press a button..."
};

// Callback invoked when calibration is completed.
typedef void (*txt_joystick_axis_callback_t)();

struct txt_joystick_axis_t
{
	Widget widget;
	int* axis;
	int* invert;
	txt_joystick_axis_direction_t dir;

	// Only used when configuring:

	// Configuration prompt window and label.
	Window* config_window;
	txt_label_t* config_label;

	// SDL joystick handle for reading joystick state.
	SDL_Joystick* joystick;

	// "Bad" joystick axes. Sometimes an axis can be stuck or "bad". An
	// example I found is that if you unplug the nunchuck extension from
	// a Wii remote, the axes from the nunchuck can be stuck at one of
	// the maximum values. These have to be ignored, so when we ask the
	// user to center the joystick, we look for bad axes that are not
	// close to zero.
	bool* bad_axis;

	// Stage we have reached in configuring joystick axis.
	txt_joystick_axis_stage_t config_stage;

	// Button to press to advance to next stage.
	int config_button;

	// Callback invoked when the axis is calibrated.
	txt_joystick_axis_callback_t callback;
};

txt_joystick_axis_t* NewJoystickAxis(int* axis, int* invert, txt_joystick_axis_direction_t dir);

// Configure a joystick axis widget.
// axis: The axis widget to configure.
// using_button: If non-negative, use this joystick button as the button to expect from the user. Otherwise, ask.
void ConfigureJoystickAxis(txt_joystick_axis_t* axis, int using_button, txt_joystick_axis_callback_t callback);

constexpr size_t JOYSTICK_AXIS_WIDTH{20};

static ::std::string CalibrationLabel(txt_joystick_axis_t* joystick_axis)
{
	switch (joystick_axis->config_stage)
	{
	case CONFIG_CENTER:
		return "Center the D-pad or joystick,\nand press a button.";

	case CONFIG_STAGE1:
		if (joystick_axis->dir == JOYSTICK_AXIS_VERTICAL)
		{
			return "Push the D-pad or joystick up,\nand press the button.";
		}
		else
		{
			return "Push the D-pad or joystick to the\nleft, and press the button.";
		}

	case CONFIG_STAGE2:
		if (joystick_axis->dir == JOYSTICK_AXIS_VERTICAL)
		{
			return "Push the D-pad or joystick down,\nand press the button.";
		}
		else
		{
			return "Push the D-pad or joystick to the\nright, and press the button.";
		}
	}

	return nullptr;
}

static void SetCalibrationLabel(txt_joystick_axis_t* joystick_axis)
{
	TXT_SetLabel(joystick_axis->config_label, CalibrationLabel(joystick_axis));
}

// Search all axes on joystick being configured; find a button that is
// pressed (other than the calibrate button). Returns the button number.

static int FindPressedAxisButton(txt_joystick_axis_t* joystick_axis)
{
	for (size_t i{0}; i < SDL_JoystickNumButtons(joystick_axis->joystick); ++i)
	{
		if (i == joystick_axis->config_button)
		{
			continue;
		}

		if (SDL_JoystickGetButton(joystick_axis->joystick, i))
		{
			return i;
		}
	}

	return -1;
}

// Look for a hat that isn't centered. Returns the encoded hat axis.
static int FindUncenteredHat(SDL_Joystick* joystick, int* axis_invert)
{
	for (size_t i{0}; i < SDL_JoystickNumHats(joystick); ++i)
	{
		auto hatval = SDL_JoystickGetHat(joystick, i);

		switch (hatval)
		{
		case SDL_HAT_LEFT:
		case SDL_HAT_RIGHT:
			*axis_invert = hatval != SDL_HAT_LEFT;
			return CREATE_HAT_AXIS(i, HAT_AXIS_HORIZONTAL);

		case SDL_HAT_UP:
		case SDL_HAT_DOWN:
			*axis_invert = hatval != SDL_HAT_UP;
			return CREATE_HAT_AXIS(i, HAT_AXIS_VERTICAL);

			// If the hat is centered, or is not pointing in a definite direction, then ignore it.
			// We don't accept the hat being pointed to the upper-left for example, because it's ambiguous.
		case SDL_HAT_CENTERED:
		default:
			break;
		}
	}

	// None found.
	return -1;
}

static bool CalibrateAxis(txt_joystick_axis_t* joystick_axis)
{
	// Check all axes to find which axis has the largest value.
	// We test for one axis at a time, so eg. when we prompt to push the joystick left, whichever axis has the largest value is the left axis.
	int best_axis{0};
	int best_value{0};
	int best_invert{0};

	for (size_t i{0}; i < SDL_JoystickNumAxes(joystick_axis->joystick); ++i)
	{
		Sint16 axis_value{SDL_JoystickGetAxis(joystick_axis->joystick, i)};

		if (joystick_axis->bad_axis[i])
		{
			continue;
		}

		if (abs(axis_value) > best_value)
		{
			best_value = abs(axis_value);
			best_invert = axis_value > 0;
			best_axis = i;
		}
	}

	// Did we find one axis that had a significant value?
	if (best_value > 32768 / 4)
	{
		// Save the best values we have found
		*joystick_axis->axis = best_axis;
		*joystick_axis->invert = best_invert;
		return true;
	}

	// Otherwise, maybe this is a "button axis", like the PS3 SIXAXIS controller that exposes the D-pad as four individual buttons. Search for a button.
	if (size_t i{FindPressedAxisButton(joystick_axis)}; i >= 0)
	{
		*joystick_axis->axis = CREATE_BUTTON_AXIS(i, 0);
		*joystick_axis->invert = 0;
		return true;
	}

	// Maybe it's a D-pad that is presented as a hat.
	// This sounds weird but gamepads like this really do exist; an example is the Nyko AIRFLO Ex.
	if (size_t i{FindUncenteredHat(joystick_axis->joystick, joystick_axis->invert)}; i >= 0)
	{
		*joystick_axis->axis = i;
		return true;
	}

	// User pressed the button without pushing the joystick anywhere.
	return false;
}

static bool SetButtonAxisPositive(txt_joystick_axis_t* joystick_axis)
{
	if (auto button{FindPressedAxisButton(joystick_axis)}; button >= 0)
	{
		*joystick_axis->axis |= CREATE_BUTTON_AXIS(0, button);
		return true;
	}

	return false;
}

static void IdentifyBadAxes(txt_joystick_axis_t* joystick_axis)
{
	free(joystick_axis->bad_axis);
	joystick_axis->bad_axis = calloc(SDL_JoystickNumAxes(joystick_axis->joystick), sizeof(bool));

	// Look for uncentered axes.
	for (size_t i{0}; i < SDL_JoystickNumAxes(joystick_axis->joystick); ++i)
	{
		auto val{SDL_JoystickGetAxis(joystick_axis->joystick, i)};

		joystick_axis->bad_axis[i] = abs(val) > (32768 / 5);

		if (joystick_axis->bad_axis[i])
		{
			printf("Ignoring uncentered joystick axis #%i: %i\n", i, val);
		}
	}
}

static int NextCalibrateStage(txt_joystick_axis_t* joystick_axis)
{
	switch (joystick_axis->config_stage)
	{
	case CONFIG_CENTER:
		return CONFIG_STAGE1;

		// After pushing to the left, there are two possibilities:
		// either it is a button axis, in which case we need to find the other button, or we can just move on to the next axis.
	case CONFIG_STAGE1:
		if (IS_BUTTON_AXIS(*joystick_axis->axis))
		{
			return CONFIG_STAGE2;
		}
		else
		{
			return CONFIG_CENTER;
		}

	case CONFIG_STAGE2:
		return CONFIG_CENTER;
	}

	return -1;
}

static int EventCallback(SDL_Event* event, txt_joystick_axis_t* joystick_axis)
{
	bool advance;

	if (event->type != SDL_JOYBUTTONDOWN)
	{
		return 0;
	}

	// At this point, we have a button press.
	// In the first "center" stage, we're just trying to work out which
	// joystick is being configured and which button the user is pressing.
	if (joystick_axis->config_stage == CONFIG_CENTER)
	{
		joystick_axis->config_button = event->jbutton.button;
		IdentifyBadAxes(joystick_axis);

		// Advance to next stage.
		joystick_axis->config_stage = CONFIG_STAGE1;
		SetCalibrationLabel(joystick_axis);

		return 1;
	}

	// In subsequent stages, the user is asked to push in a specific direction and press the button.
	// They must push the same button as they did before; this is necessary to support button axes.
	if (event->jbutton.which == SDL_JoystickInstanceID(joystick_axis->joystick)
		&& event->jbutton.button == joystick_axis->config_button)
	{
		switch (joystick_axis->config_stage)
		{
		default:
		case CONFIG_STAGE1:
			advance = CalibrateAxis(joystick_axis);
			break;

		case CONFIG_STAGE2:
			advance = SetButtonAxisPositive(joystick_axis);
			break;
		}

		// Advance to the next calibration stage?
		if (advance)
		{
			joystick_axis->config_stage = NextCalibrateStage(joystick_axis);
			SetCalibrationLabel(joystick_axis);

			// Finished?
			if (joystick_axis->config_stage == CONFIG_CENTER)
			{
				TXT_CloseWindow(joystick_axis->config_window);

				if (joystick_axis->callback != NULL)
				{
					joystick_axis->callback();
				}
			}

			return 1;
		}
	}

	return 0;
}

static void CalibrateWindowClosed(TXT_UNCAST_ARG(widget), txt_joystick_axis_t* joystick_axis)
{
	TXT_CAST_ARG(txt_joystick_axis_t, joystick_axis);

	free(joystick_axis->bad_axis);
	joystick_axis->bad_axis = NULL;

	SDL_JoystickClose(joystick_axis->joystick);
	SDL_JoystickEventState(SDL_DISABLE);
	SDL_QuitSubSystem(SDL_INIT_JOYSTICK);
	TXT_SDL_SetEventCallback(NULL, NULL);
}

void TXT_ConfigureJoystickAxis(txt_joystick_axis_t* joystick_axis, int using_button, txt_joystick_axis_callback_t callback)
{
	// Open the joystick first.
	if (SDL_Init(SDL_INIT_JOYSTICK) < 0)
	{
		return;
	}

	joystick_axis->joystick = SDL_JoystickOpen(joystick_index);
	if (joystick_axis->joystick == NULL)
	{
		TXT_MessageBox(NULL, "Please configure a controller first!");
		return;
	}

	SDL_JoystickEventState(SDL_ENABLE);

	// Build the prompt window.
	joystick_axis->config_window = TXT_NewWindow("Gamepad/Joystick calibration");
	TXT_AddWidgets(joystick_axis->config_window, TXT_NewStrut(0, 1), joystick_axis->config_label = TXT_NewLabel(""), TXT_NewStrut(0, 1), NULL);

	TXT_SetWindowAction(joystick_axis->config_window, TXT_HORIZ_LEFT, NULL);
	TXT_SetWindowAction(joystick_axis->config_window, TXT_HORIZ_CENTER, TXT_NewWindowAbortAction(joystick_axis->config_window));
	TXT_SetWindowAction(joystick_axis->config_window, TXT_HORIZ_RIGHT, NULL);
	TXT_SetWidgetAlign(joystick_axis->config_window, TXT_HORIZ_CENTER);

	if (using_button >= 0)
	{
		joystick_axis->config_stage = CONFIG_STAGE1;
		joystick_axis->config_button = using_button;
		IdentifyBadAxes(joystick_axis);
	}
	else
	{
		joystick_axis->config_stage = CONFIG_CENTER;
	}

	SetCalibrationLabel(joystick_axis);

	// Close the joystick and shut down joystick subsystem when the window is closed.
	TXT_SignalConnect(joystick_axis->config_window, "closed", CalibrateWindowClosed, joystick_axis);

	TXT_SDL_SetEventCallback(EventCallback, joystick_axis);

	// When successfully calibrated, invoke this callback:
	joystick_axis->callback = callback;
}

static void TXT_JoystickAxisSizeCalc(txt_joystick_axis_t* joystick_axis)
{
	// All joystickinputs are the same size.
	joystick_axis->widget.w = JOYSTICK_AXIS_WIDTH;
	joystick_axis->widget.h = 1;
}

static void TXT_JoystickAxisDrawer(txt_joystick_axis_t* joystick_axis)
{
	::std::string buf;

	if (*joystick_axis->axis < 0)
	{
		buf = "(none)";
	}
	else if (IS_BUTTON_AXIS(*joystick_axis->axis))
	{
		int neg;
		int pos;

		neg = BUTTON_AXIS_NEG(*joystick_axis->axis);
		pos = BUTTON_AXIS_POS(*joystick_axis->axis);
		buf = ::std::string("BUTTONS #") + ::std::string(neg) + ::std::string("+#") + ::std::string(pos));
	}
	else if (IS_HAT_AXIS(*joystick_axis->axis))
	{
		int hat;
		int dir;

		hat = HAT_AXIS_HAT(*joystick_axis->axis);
		dir = HAT_AXIS_DIRECTION(*joystick_axis->axis);

		M_snprintf(buf, sizeof(buf), "HAT #%i (%s)", hat, dir == HAT_AXIS_HORIZONTAL ? "horizontal" : "vertical");
	}
	else
	{
		M_snprintf(buf, sizeof(buf), "AXIS #%i", *joystick_axis->axis);
	}

	TXT_SetWidgetBG(joystick_axis);
	TXT_FGColor(ColorType::bright_white);

	TXT_DrawString(buf);

	for (size_t i{buf.length()}; i < joystick_axis->widget.w; ++i)
	{
		TXT_DrawString(" ");
	}
}

static void TXT_JoystickAxisDestructor(txt_joystick_axis_t* joystick_axis) {}

static int TXT_JoystickAxisKeyPress(txt_joystick_axis_t* joystick_axis, int key)
{
	if (key == Keys::ENTER)
	{
		TXT_ConfigureJoystickAxis(joystick_axis, -1, NULL);
		return 1;
	}

	if (key == Keys::BACKSPACE || key == Keys::DEL)
	{
		*joystick_axis->axis = -1;
	}

	return 0;
}

static void TXT_JoystickAxisMousePress(txt_joystick_axis_t* widget, int x, int y, int b)
{
	// Clicking is like pressing enter
	if (b == TXT_MOUSE_LEFT)
	{
		TXT_JoystickAxisKeyPress(widget, Keys::ENTER);
	}
}

WidgetClass txt_joystick_axis_class =
{
	TXT_AlwaysSelectable,
	TXT_JoystickAxisSizeCalc,
	TXT_JoystickAxisDrawer,
	TXT_JoystickAxisKeyPress,
	TXT_JoystickAxisDestructor,
	TXT_JoystickAxisMousePress,
	NULL,
};

txt_joystick_axis_t* TXT_NewJoystickAxis(int* axis, int* invert, txt_joystick_axis_direction_t dir)
{
	txt_joystick_axis_t* joystick_axis{static_cast<txt_joystick_axis_t*>(malloc(sizeof(txt_joystick_axis_t)))};

	TXT_InitWidget(joystick_axis, &txt_joystick_axis_class);
	joystick_axis->axis = axis;
	joystick_axis->invert = invert;
	joystick_axis->dir = dir;
	joystick_axis->bad_axis = NULL;

	return joystick_axis;
}
