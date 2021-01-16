/**********************************************************************************************************************************************\
	Copyright(C) 2005-2014 Simon Howard

	This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License
	as published by the Free Software Foundation; either version 2 of the License, or (at your option) any later version.

	This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
\**********************************************************************************************************************************************/
#pragma once

#include "../../derma/stdafx.h"

#include <SDL_joystick.h>

#include "../../derma/common.h"
#include "../../derma/keybinds.h"

#include "../../textscreen/txt_widget.h"
#include "../../textscreen/txt_window.h"
#include "../../textscreen/txt_label.h"
#include "../../textscreen/txt_gui.h"
#include "../../textscreen/txt_io.h"
#include "../../textscreen/txt_sdl.h"

#include "../i_joystick.h"
#include "../i_system.h"
#include "../m_controls.h"
#include "../m_misc.h"

#include "joystick.h"

// A joystick input is like an input box. When selected, a box pops up allowing a joystick button to be pressed to select it.
struct txt_joystick_input_t
{
	Widget widget;
	int* variable;
	Window* prompt_window;
	int check_conflicts;
};

txt_joystick_input_t* NewJoystickInput(int* variable);

constexpr size_t JOYSTICK_INPUT_WIDTH{10};

extern int joystick_physical_buttons[NUM_VIRTUAL_BUTTONS];

// Joystick button variables. The ordering of this array is important. We will always try to map each variable to the virtual button with the
// same array index. For example: joybfire should always be 0, and then we change joystick_physical_buttons[0] to point to the physical joystick
// button that the user wants to use for firing. We do this so that the menus work.
// (the game code is hard coded to interpret button #0 = select menu item, button #1 = go back to previous menu).
static int* all_joystick_buttons[NUM_VIRTUAL_BUTTONS] =
{
	&joybfire,
	&joybuse,
	&joybstrafe,
	&joybspeed,
	&joybstrafeleft,
	&joybstraferight,
	&joybprevweapon,
	&joybnextweapon,
	&joybjump,
	&joybmenu,
	&joybautomap,
};

static int PhysicalForVirtualButton(int vbutton)
{
	if (vbutton < NUM_VIRTUAL_BUTTONS)
	{
		return joystick_physical_buttons[vbutton];
	}
	else
	{
		return vbutton;
	}
}

// Get the virtual button number for the given variable, ie. the variable's index in all_joystick_buttons[NUM_VIRTUAL_BUTTONS].
static int VirtualButtonForVariable(int* variable)
{
	for (size_t i{0}; i < arrlen(all_joystick_buttons); ++i)
	{
		if (variable == all_joystick_buttons[i])
		{
			return i;
		}
	}

	I_Error("Couldn't find virtual button");
	return -1;
}

// Rearrange joystick button configuration to be in "canonical" form:
// each joyb* variable should have a value equal to its index in all_joystick_buttons[NUM_VIRTUAL_BUTTONS] above.
static void CanonicalizeButtons()
{
	int new_mapping[NUM_VIRTUAL_BUTTONS];

	for (size_t i{0}; i < arrlen(all_joystick_buttons); ++i)
	{
		auto vbutton{*all_joystick_buttons[i]};

		// Don't remap the speed key if it's bound to "always run". Also preserve "unbound" variables.
		if ((all_joystick_buttons[i] == &joybspeed && vbutton >= 20) || vbutton < 0)
		{
			new_mapping[i] = i;
		}
		else
		{
			new_mapping[i] = PhysicalForVirtualButton(vbutton);
			*all_joystick_buttons[i] = i;
		}
	}

	for (size_t i{0}; i < NUM_VIRTUAL_BUTTONS; ++i)
	{
		joystick_physical_buttons[i] = new_mapping[i];
	}
}

// Check all existing buttons and clear any using the specified physical button.
static void ClearVariablesUsingButton(int physbutton)
{
	for (size_t i{0}; i < arrlen(all_joystick_buttons); ++i)
	{
		auto vbutton{*all_joystick_buttons[i]};

		if (vbutton >= 0 && physbutton == PhysicalForVirtualButton(vbutton))
		{
			*all_joystick_buttons[i] = -1;
		}
	}
}

// Called in response to SDL events when the prompt window is open:
static int EventCallback(SDL_Event* event, txt_joystick_input_t* joystick_input)
{
	// Got the joystick button press?
	if (event->type == SDL_JOYBUTTONDOWN)
	{
		// Before changing anything, remap button configuration into canonical form, to avoid conflicts.
		CanonicalizeButtons();

		auto vbutton{VirtualButtonForVariable(joystick_input->variable)};
		auto physbutton{event->jbutton.button};

		if (joystick_input->check_conflicts)
		{
			ClearVariablesUsingButton(physbutton);
		}

		// Set mapping.
		*joystick_input->variable = vbutton;
		joystick_physical_buttons[vbutton] = physbutton;

		TXT_CloseWindow(joystick_input->prompt_window);
		return 1;
	}

	return 0;
}

// When the prompt window is closed, disable the event callback function;
// we are no longer interested in receiving notification of events.
static void PromptWindowClosed(Widget* widget, SDL_Joystick* joystick)
{
	SDL_JoystickClose(joystick);
	TXT_SDL_SetEventCallback(NULL, NULL);
	SDL_JoystickEventState(SDL_DISABLE);
	SDL_QuitSubSystem(SDL_INIT_JOYSTICK);
}

static void OpenErrorWindow()
{
	TXT_MessageBox(NULL, "Please configure a controller first!");
}

static void OpenPromptWindow(txt_joystick_input_t* joystick_input)
{
	// Silently update when the shift button is held down.
	joystick_input->check_conflicts = !TXT_GetModifierState(ModifierType::shift);

	if (SDL_Init(SDL_INIT_JOYSTICK) < 0)
	{
		return;
	}

	// Check the current joystick is valid
	SDL_Joystick* joystick{SDL_JoystickOpen(joystick_index)};

	if (joystick == NULL)
	{
		OpenErrorWindow();
		return;
	}

	// Open the prompt window
	Window* window = TXT_MessageBox(NULL, "Press the new button on the controller...");

	TXT_SDL_SetEventCallback(EventCallback, joystick_input);
	TXT_SignalConnect(window, "closed", PromptWindowClosed, joystick);
	joystick_input->prompt_window = window;

	SDL_JoystickEventState(SDL_ENABLE);
}

static void TXT_JoystickInputSizeCalc(txt_joystick_input_t* joystick_input)
{
	// All joystickinputs are the same size.
	joystick_input->widget.w = JOYSTICK_INPUT_WIDTH;
	joystick_input->widget.h = 1;
}

static ::std::string GetJoystickButtonDescription(int vbutton)
{
	return ::std::string("BUTTON #" + ::std::string(PhysicalForVirtualButton(vbutton) + 1));
}

static void TXT_JoystickInputDrawer(txt_joystick_input_t* joystick_input)
{
	::std::string buf;

	if (*joystick_input->variable < 0)
	{
		buf = "(none)";
	}
	else
	{
		buf = GetJoystickButtonDescription(*joystick_input->variable);
	}

	TXT_SetWidgetBG(joystick_input);
	TXT_FGColor(ColorType::bright_white);

	TXT_DrawString(buf);

	for (size_t i{buf.length()}; i < JOYSTICK_INPUT_WIDTH; ++i)
	{
		TXT_DrawString(" ");
	}
}

static void TXT_JoystickInputDestructor(txt_joystick_input_t* joystick_input) {}

static int TXT_JoystickInputKeyPress(txt_joystick_input_t* joystick_input, int key)
{
	if (key == Keys::ENTER)
	{
		// Open a window to prompt for the new joystick press
		OpenPromptWindow(joystick_input);

		return 1;
	}

	if (key == Keys::BACKSPACE || key == Keys::DEL)
	{
		*joystick_input->variable = -1;
	}

	return 0;
}

static void TXT_JoystickInputMousePress(txt_joystick_input_t* widget, int x, int y, int b)
{
	// Clicking is like pressing enter
	if (b == TXT_MOUSE_LEFT)
	{
		TXT_JoystickInputKeyPress(widget, Keys::ENTER);
	}
}

WidgetClass txt_joystick_input_class =
{
	TXT_AlwaysSelectable,
	TXT_JoystickInputSizeCalc,
	TXT_JoystickInputDrawer,
	TXT_JoystickInputKeyPress,
	TXT_JoystickInputDestructor,
	TXT_JoystickInputMousePress,
	NULL,
};

txt_joystick_input_t* TXT_NewJoystickInput(int* variable)
{
	auto joystick_input{static_cast<txt_joystick_input_t*>(malloc(sizeof(txt_joystick_input_t)))};

	TXT_InitWidget(joystick_input, &txt_joystick_input_class);
	joystick_input->variable = variable;

	return joystick_input;
}
