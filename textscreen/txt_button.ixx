/**********************************************************************************************************************************************\
	Copyright(C) 2005-2014 Simon Howard

	This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License
	as published by the Free Software Foundation; either version 2 of the License, or (at your option) any later version.

	This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
\**********************************************************************************************************************************************/
//#pragma once

//#include "../derma/common.h"

//#include "doomkeys.h"

export module txt_button;

import std.core;

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
 * Button widget.
 * A button is a widget that can be selected to perform some action.
 * When a button is pressed, it emits the "pressed" signal.
 */
struct Button : Widget
{
	std::string label;

	void SizeCalc()
	{
		width = label.length();
		height = 1;
	}

	void Drawer()
	{
		SetWidgetBG();

		DrawString(label);

		for (size_t i{label.length()}; i < width; ++i)
		{
			DrawString(" ");
		}
	}

	bool KeyPress(int key)
	{
		if (key == KEY_ENTER)
		{
			EmitSignal("pressed");
			return true;
		}

		return false;
	}

	void MousePress(int x, int y, int b)
	{
		if (b == MOUSE_LEFT)
		{
			// Equivalent to pressing enter
			KeyPress(KEY_ENTER);
		}
	}

	WidgetClass ButtonClass =
	{
		AlwaysSelectable,
		ButtonSizeCalc,
		ButtonDrawer,
		ButtonKeyPress,
		ButtonDestructor,
		ButtonMousePress,
		NULL
	};

	void SetButtonLabel(std::string& _label)
	{
		label = std::string(_label);
	}

	Button(std::string& _label) : label(_label)
	{
	}

	Button(std::string& _label, WidgetSignalFunc _handle, UserData _user)
	{
		SetButtonLabel(_label);
		SignalConnect(std::string("pressed"), _handle, _user);

		return button;
	}
};

} /* END NAMESPACE cudadoom::txt */

}
