/**********************************************************************************************************************************************\
	Copyright(C) 2005-2014 Simon Howard

	This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License
	as published by the Free Software Foundation; either version 2 of the License, or (at your option) any later version.

	This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
\**********************************************************************************************************************************************/

#include "txt_keyinput.h"

#include "doomkeys.h"
#include "m_misc.h"

#include "../../textscreen/txt_window.h"
#include "../../textscreen/txt_gui.h"
#include "../../textscreen/txt_io.h"
#include "../../textscreen/txt_label.h"
#include "../../textscreen/txt_utf8.h"

constexpr size_t KEY_INPUT_WIDTH{8};

static int KeyPressCallback(Window* window, int key, TXT_UNCAST_ARG(key_input))
{
	TXT_CAST_ARG(txt_key_input_t, key_input);

	if (key != KEY_ESCAPE)
	{
		// Got the key press. Save to the variable and close the window.

		*key_input->variable = key;

		if (key_input->check_conflicts)
		{
			TXT_EmitSignal(key_input, "set");
		}

		TXT_CloseWindow(window);

		// Return to normal input mode now that we have the key.
		TXT_SetInputMode(TXT_InputType::normal);

		return 1;
	}
	else
	{
		return 0;
	}
}

static void ReleaseGrab(TXT_UNCAST_ARG(window), TXT_UNCAST_ARG(unused))
{
	// SDL2-TODO: Needed?
	// SDL_WM_GrabInput(SDL_GRAB_OFF);
}

static void OpenPromptWindow(txt_key_input_t* key_input)
{
	Window* window;

	// Silently update when the shift button is held down.

	key_input->check_conflicts = !TXT_GetModifierState(ModifierType::shift);

	window = TXT_MessageBox(NULL, "Press the new key...");

	TXT_SetKeyListener(window, KeyPressCallback, key_input);

	// Switch to raw input mode while we're grabbing the key.
	TXT_SetInputMode(TXT_InputType::raw);

	// Grab input while reading the key. On Windows Mobile
	// handheld devices, the hardware keypresses are only
	// detected when input is grabbed.

	// SDL2-TODO: Needed?
	//SDL_WM_GrabInput(SDL_GRAB_ON);
	TXT_SignalConnect(window, "closed", ReleaseGrab, NULL);
}

static void TXT_KeyInputSizeCalc(TXT_UNCAST_ARG(key_input))
{
	TXT_CAST_ARG(txt_key_input_t, key_input);

	// All keyinputs are the same size.

	key_input->widget.w = KEY_INPUT_WIDTH;
	key_input->widget.h = 1;
}


static void TXT_KeyInputDrawer(TXT_UNCAST_ARG(key_input))
{
	TXT_CAST_ARG(txt_key_input_t, key_input);
	char buf[20];
	int i;

	if (*key_input->variable == 0)
	{
		M_StringCopy(buf, "(none)", sizeof(buf));
	}
	else
	{
		TXT_GetKeyDescription(*key_input->variable, buf, sizeof(buf));
	}

	TXT_SetWidgetBG(key_input);
	TXT_FGColor(ColorType::bright_white);

	TXT_DrawString(buf);

	for (i = TXT_UTF8_Strlen(buf); i < KEY_INPUT_WIDTH; ++i)
	{
		TXT_DrawString(" ");
	}
}

static void TXT_KeyInputDestructor(TXT_UNCAST_ARG(key_input))
{
}

static int TXT_KeyInputKeyPress(TXT_UNCAST_ARG(key_input), int key)
{
	TXT_CAST_ARG(txt_key_input_t, key_input);

	if (key == KEY_ENTER)
	{
		// Open a window to prompt for the new key press

		OpenPromptWindow(key_input);

		return 1;
	}

	if (key == KEY_BACKSPACE || key == KEY_DEL)
	{
		*key_input->variable = 0;
	}

	return 0;
}

static void TXT_KeyInputMousePress(TXT_UNCAST_ARG(widget), int x, int y, int b)
{
	TXT_CAST_ARG(txt_key_input_t, widget);

	// Clicking is like pressing enter

	if (b == TXT_MOUSE_LEFT)
	{
		TXT_KeyInputKeyPress(widget, KEY_ENTER);
	}
}

WidgetClass txt_key_input_class =
{
	TXT_AlwaysSelectable,
	TXT_KeyInputSizeCalc,
	TXT_KeyInputDrawer,
	TXT_KeyInputKeyPress,
	TXT_KeyInputDestructor,
	TXT_KeyInputMousePress,
	NULL,
};

txt_key_input_t* TXT_NewKeyInput(int *variable)
{
	txt_key_input_t* key_input = static_cast<decltype(key_input)>(malloc(sizeof(txt_key_input_t)));

	TXT_InitWidget(key_input, &txt_key_input_class);
	key_input->variable = variable;

	return key_input;
}

