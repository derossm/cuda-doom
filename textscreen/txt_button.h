/**********************************************************************************************************************************************\
	Copyright(C) 2005-2014 Simon Howard

	This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License
	as published by the Free Software Foundation; either version 2 of the License, or (at your option) any later version.

	This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
\**********************************************************************************************************************************************/
#pragma once
// DECOUPLE
//#include "../derma/common.h"
#include "../derma/keybinds.h"

#include <string>

#include "txt_common.h"
#include "txt_widget.h"

//#include "txt_main.h"
//#include "txt_utf8.h"
//#include "txt_io.h"
//#include "txt_gui.h"

namespace cudadoom::txt
{

/**
 * Button widget.
 * A button is a widget that can be selected to perform some action. When a button is pressed, it emits the "pressed" signal.
 */
class Button : public Widget<Button>
{
	std::string label;

public:

	Button(std::string& _label) //: label(_label),
		//widget_class{Selectable, CalculateSize, Draw, KeyPress, MousePress, SetLayout, SetFocus, Destroy}
	{
	}

	Button(std::string& _label, UserData _user) //:
		//widget_class{Selectable, CalculateSize, Draw, KeyPress, MousePress, SetLayout, SetFocus, Destroy}
	{
		//SetButtonLabel(_label);
		//SignalConnect(std::string("pressed"), _handle, _user);
	}

	inline bool Selectable() const noexcept override final
	{}

	inline void CalculateSize() noexcept override final
	{
		//width = label.length();
		//height = 1;
	}

	inline void Draw() noexcept override final
	{
/*
		SetWidgetBG();

		DrawString(label);

		for (size_t i{label.length()}; i < width; ++i)
		{
			DrawString(" ");
		}
/**/
	}

	inline bool KeyPress(KeyEvent key) noexcept override final
	{
/*
		if (key == KEY_ENTER)
		{
			EmitSignal("pressed");
			return true;
		}
/**/
		return false;
	}

	inline bool MousePress(MouseEvent evt) noexcept override final
	{
/*
		if (evt.button == MOUSE_LEFT)
		{
			// Equivalent to pressing enter
			return KeyPress(KEY_ENTER);
		}
/**/
		return false;
	}

	inline void SetLayout() noexcept override final
	{}

	inline void SetFocus(bool _focus) noexcept override final
	{}

	inline void Destroy() noexcept override final
	{}

	void SetButtonLabel(std::string& _label)
	{
		label = std::string(_label);
	}
};

} // END NAMESPACE cudadoom::txt
