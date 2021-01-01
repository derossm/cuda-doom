/**********************************************************************************************************************************************\
	Copyright(C) 2005-2014 Simon Howard

	This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License
	as published by the Free Software Foundation; either version 2 of the License, or (at your option) any later version.

	This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
\**********************************************************************************************************************************************/
//#pragma once

//#include "../derma/common.h"

export module txt_window_action;

import std.core;

import textscreen;

import txt_main;
import txt_widget;
import txt_window;
import txt_utf8;
import txt_io;
import txt_gui;

export
{

namespace cudadoom::txt
{

/**
 * Window action widget.
 *
 * A window action is attached to a window and corresponds to a keyboard shortcut that is active within that window.
 * When the key is pressed, the action is triggered. When a window action is triggered, the "pressed" signal is emitted.
 */
class WindowAction : Widget
{
	int key;
	std::string label;

public:
	WidgetClass txt_window_action_class{
		AlwaysSelectable,
		WindowActionSizeCalc,
		WindowActionDrawer,
		WindowActionKeyPress,
		WindowActionDestructor,
		WindowActionMousePress,
		NULL
	};

	WindowAction(int _key, std::string _label) : widget_class{&txt_window_action_class)}, key{_key}, label{std::string(_label)}
	{
	}

	void SizeCalc()
	{
		char buf[10];

		GetKeyDescription(key, buf, sizeof(buf));

		// Width is label length, plus key description length, plus '=' and two surrounding spaces.
		width = label.length() + UTF8_Strlen(buf) + 3;
		height = 1;
	}

	void Drawer()
	{
		char buf[10];

		GetKeyDescription(key, buf, sizeof(buf));

		auto hovering{HoveringOverWidget()};
		SetWidgetBG();

		DrawString(" ");
		FGColor(hovering ? ColorType::bright_white : ColorType::bright_green);
		DrawString(buf);
		FGColor(ColorType::bright_cyan);
		DrawString("=");

		FGColor(ColorType::bright_white);
		DrawString(label);
		DrawString(" ");
	}

	void Destructor()
	{
	}

	auto KeyPress(int _key)
	{
		if (tolower(_key) == tolower(key))
		{
			EmitSignal("pressed");
			return true;
		}

		return false;
	}

	void MousePress(int x, int y, int b)
	{
		// Simulate a press of the key
		if (b == MOUSE_LEFT)
		{
			KeyPress(key);
		}
	}

	// FIXME
	void WindowCloseCallback(Window* window)
	{
		window->CloseWindow();
	}

	void WindowSelectCallback(Window* window)
	{
		window->WidgetKeyPress(KEY_ENTER);
	}
};

// An action with the name "close" the closes the window
WindowAction* NewWindowEscapeAction(Window* window)
{
	auto action{WindowAction(KEY_ESCAPE, "Close")};
	SignalConnect(action, "pressed", WindowCloseCallback, window);

	return action;
}

// Exactly the same as the above, but the button is named "abort"
WindowAction* NewWindowAbortAction(Window* window)
{
	auto action{WindowAction(KEY_ESCAPE, "Abort")};
	SignalConnect(action, "pressed", WindowCloseCallback, window);

	return action;
}

WindowAction* NewWindowSelectAction(Window* window)
{
	auto action{WindowAction(KEY_ENTER, "Select")};
	SignalConnect(action, "pressed", WindowSelectCallback, window);

	return action;
}

} /* END NAMESPACE cudadoom::txt */

/**
 * Create a new window action.
 *
 * @param key			The keyboard key that triggers this action.
 * @param label			Label to display for this action in the tray at the bottom of the window (UTF-8 format).
 * @return				Pointer to the new window action widget.
 */
//WindowAction* NewWindowAction(int key, const char* label);

/**
 * Create a new window action that closes the window when the escape key is pressed. The label "Close" is used.
 *
 * @param window		The window to close.
 * @return				Pointer to the new window action widget.
 */
//WindowAction* NewWindowEscapeAction(Window* window);

/**
 * Create a new window action that closes the window when the escape key is pressed. The label "Abort" is used.
 *
 * @param window		The window to close.
 * @return				Pointer to the new window action widget.
 */
//WindowAction* NewWindowAbortAction(Window* window);

/**
 * Create a new "select" window action. This does not really do anything, but reminds the user that "enter" can be pressed to
 * activate the currently-selected widget.
 *
 * @param window		The window.
 * @return				Pointer to the new window action widget.
 */
//WindowAction* NewWindowSelectAction(Window* window);

}
