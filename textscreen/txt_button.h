/**********************************************************************************************************************************************\
	Copyright(C) 2005-2014 Simon Howard

	This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License
	as published by the Free Software Foundation; either version 2 of the License, or (at your option) any later version.

	This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
\**********************************************************************************************************************************************/
#pragma once

#include "../derma/common.h"
#include "txt_common.h"

#include "doomkeys.h"

#include "txt_widget.h"

#include "txt_main.h"
#include "txt_utf8.h"
#include "txt_io.h"
#include "txt_gui.h"

//#include "txt_window.h"

namespace cudadoom::txt
{

/**
 * Button widget.
 * A button is a widget that can be selected to perform some action.
 * When a button is pressed, it emits the "pressed" signal.
 */
class Button : public Widget<Button>
{
	std::string label;

public:

	Button(std::string& _label) : label(_label),
							 widget_class{Selectable, CalculateSize, Draw, KeyPress, MousePress, SetLayout, SetFocus, Destroy}
	{
	}

	Button(std::string& _label, WidgetSignalFunc _handle, UserData _user) :
							widget_class{Selectable, CalculateSize, Draw, KeyPress, MousePress, SetLayout, SetFocus, Destroy}
	{
		SetButtonLabel(_label);
		SignalConnect(std::string("pressed"), _handle, _user);
	}

	bool Selectable() override final const noexcept
	{
	}

	void CalculateSize() override final const noexcept
	{
		width = label.length();
		height = 1;
	}

	void Draw() override final const noexcept
	{
		SetWidgetBG();

		DrawString(label);

		for (size_t i{label.length()}; i < width; ++i)
		{
			DrawString(" ");
		}
	}

	bool KeyPress(KeyType key) override final const noexcept
	{
		if (key == KEY_ENTER)
		{
			EmitSignal("pressed");
			return true;
		}

		return false;
	}

	bool MousePress(MouseEvent evt) override final const noexcept
	{
		if (evt.button == MOUSE_LEFT)
		{
			// Equivalent to pressing enter
			return KeyPress(KEY_ENTER);
		}

		return false;
	}

	void SetLayout() override final const noexcept
	{
	}

	void SetFocus() override final const noexcept
	{
	}

	void Destroy() override final const noexcept
	{
	}

	void SetButtonLabel(std::string& _label)
	{
		label = std::string(_label);
	}
};

} /* END NAMESPACE cudadoom::txt */
