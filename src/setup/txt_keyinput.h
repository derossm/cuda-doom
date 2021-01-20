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
#include "../../textscreen/txt_label.h"
#include "../../textscreen/txt_gui.h"
#include "../../textscreen/txt_io.h"

#include "../m_misc.h"

// A key input is like an input box. When selected, a box pops up allowing a key to be selected.
struct txt_key_input_t
{
	Widget widget;
	int *variable;
	int check_conflicts;
};

txt_key_input_t* NewKeyInput(int *variable);

constexpr size_t KEY_INPUT_WIDTH{8};

static int KeyPressCallback(Window* window, int key, txt_key_input_t* key_input)
{
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

static void ReleaseGrab(txt::Window* window), void* unused)
{
	// SDL2-TODO: Needed?
	// SDL_WM_GrabInput(SDL_GRAB_OFF);
}

static void OpenPromptWindow(txt_key_input_t* key_input)
{
	// Silently update when the shift button is held down.
	key_input->check_conflicts = !TXT_GetModifierState(ModifierType::shift);

	Window* window = TXT_MessageBox(NULL, "Press the new key...");

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

static void TXT_KeyInputSizeCalc(txt_key_input_t* key_input)
{
	// All keyinputs are the same size.
	key_input->widget.w = KEY_INPUT_WIDTH;
	key_input->widget.h = 1;
}

static void TXT_KeyInputDrawer(txt_key_input_t* key_input)
{
	::std::string buf;

	if (*key_input->variable == 0)
	{
		buf = "(none)";
	}
	else
	{
		TXT_GetKeyDescription(*key_input->variable, buf, sizeof(buf));
	}

	TXT_SetWidgetBG(key_input);
	TXT_FGColor(ColorType::bright_white);

	TXT_DrawString(buf);

	for (size_t i{buf.length()}; i < KEY_INPUT_WIDTH; ++i)
	{
		TXT_DrawString(" ");
	}
}

static void TXT_KeyInputDestructor(txt_key_input_t* key_input)
{
}

static int TXT_KeyInputKeyPress(txt_key_input_t* key_input, int key)
{
	if (key == Keys::ENTER)
	{
		// Open a window to prompt for the new key press
		OpenPromptWindow(key_input);

		return 1;
	}

	if (key == Keys::BACKSPACE || key == Keys::DEL)
	{
		*key_input->variable = 0;
	}

	return 0;
}

static void TXT_KeyInputMousePress(txt_key_input_t* widget, int x, int y, int b)
{
	// Clicking is like pressing enter
	if (b == TXT_MOUSE_LEFT)
	{
		TXT_KeyInputKeyPress(widget, Keys::ENTER);
	}
}

txt_key_input_t* TXT_NewKeyInput(int* variable)
{
	txt_key_input_t* key_input{static_cast<txt_key_input_t*>(malloc(sizeof(txt_key_input_t)))};

	TXT_InitWidget(key_input, &txt_key_input_class);
	key_input->variable = variable;

	return key_input;
}
