/**********************************************************************************************************************************************\
	Copyright(C) 2005-2014 Simon Howard

	This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License
	as published by the Free Software Foundation; either version 2 of the License, or (at your option) any later version.

	This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
\**********************************************************************************************************************************************/
#pragma once

#include "../../derma/stdafx.h"

#include "../../derma/common.h"
#include "../../derma/keybinds.h"

#include "../../textscreen/txt_widget.h"
#include "../../textscreen/txt_window.h"
#include "../../textscreen/txt_label.h"
#include "../../textscreen/txt_gui.h"
#include "../../textscreen/txt_io.h"

#include "../m_misc.h"

// A mouse input is like an input box. When selected, a box pops up allowing a mouse to be selected.
struct txt_mouse_input_t
{
	txt::Widget widget;
	int* variable;
	int check_conflicts;
};

txt_mouse_input_t* NewMouseInput(int* variable);

// eg. "BUTTON #10"
constexpr size_t MOUSE_INPUT_WIDTH{10};

static int MousePressCallback(Window* window, int x, int y, int b, txt_mouse_input_t* mouse_input)
{
	// Got the mouse press. Save to the variable and close the window.
	*mouse_input->variable = b - TXT_MOUSE_BASE;

	if (mouse_input->check_conflicts)
	{
		TXT_EmitSignal(mouse_input, "set");
	}

	TXT_CloseWindow(window);

	return 1;
}

static void OpenPromptWindow(txt_mouse_input_t* mouse_input)
{
	Window* window;

	// Silently update when the shift key is held down.
	mouse_input->check_conflicts = !TXT_GetModifierState(ModifierType::shift);

	window = TXT_MessageBox(NULL, "Press the new mouse button...");

	TXT_SetMouseListener(window, MousePressCallback, mouse_input);
}

static void TXT_MouseInputSizeCalc(txt_mouse_input_t* mouse_input)
{
	// All mouseinputs are the same size.
	mouse_input->widget.w = MOUSE_INPUT_WIDTH;
	mouse_input->widget.h = 1;
}

static void GetMouseButtonDescription(int button, ::std::string buf, size_t buf_len)
{
	switch (button)
	{
	case 0:
		buf = "LEFT";
		break;
	case 1:
		buf = "RIGHT";
		break;
	case 2:
		buf = "MID";
		break;
	default:
		buf = ::std::string("BUTTON #") + ::std::string(button + 1);
		break;
	}
}

static void TXT_MouseInputDrawer(txt_mouse_input_t* mouse_input)
{
	::std::string buf;

	if (*mouse_input->variable < 0)
	{
		buf = "(none)";
	}
	else
	{
		GetMouseButtonDescription(*mouse_input->variable, buf, sizeof(buf));
	}

	txt::SetWidgetBG(mouse_input);
	txt::FGColor(ColorType::bright_white);

	txt::DrawString(buf);

	for (size_t i{buf.length()}; i < MOUSE_INPUT_WIDTH; ++i)
	{
		txt::DrawString(" ");
	}
}

static void TXT_MouseInputDestructor(txt_mouse_input_t* mouse_input) {}

static int TXT_MouseInputKeyPress(txt_mouse_input_t* mouse_input, int key)
{
	if (key == Keys::ENTER)
	{
		// Open a window to prompt for the new mouse press
		OpenPromptWindow(mouse_input);

		return 1;
	}

	if (key == Keys::BACKSPACE || key == Keys::DEL)
	{
		*mouse_input->variable = -1;
	}

	return 0;
}

static void TXT_MouseInputMousePress(txt_mouse_input_t* widget, int x, int y, int b)
{
	// Clicking is like pressing enter
	if (b == TXT_MOUSE_LEFT)
	{
		TXT_MouseInputKeyPress(widget, Keys::ENTER);
	}
}

txt_mouse_input_t* TXT_NewMouseInput(int* variable)
{
	txt_mouse_input_t* mouse_input;

	mouse_input = static_cast<decltype(mouse_input)>(malloc(sizeof(txt_mouse_input_t)));

	TXT_InitWidget(mouse_input, &txt_mouse_input_class);
	mouse_input->variable = variable;

	return mouse_input;
}
