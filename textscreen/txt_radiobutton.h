/**********************************************************************************************************************************************\
	Copyright(C) 2005-2014 Simon Howard

	This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License
	as published by the Free Software Foundation; either version 2 of the License, or (at your option) any later version.

	This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
\**********************************************************************************************************************************************/
#pragma once
// DECOUPLE
#include "../derma/common.h"
//////////

#include "txt_common.h"

#include "txt_widget.h"

#include "txt_main.h"
#include "txt_utf8.h"
#include "txt_io.h"
#include "txt_gui.h"

//#include "txt_window.h"

// DECOUPLE
#include "../src/doomkeys.h"

namespace cudadoom::txt
{

/**
 * A radio button widget.
 *
 * Radio buttons are typically used in groups, to allow a value to be
 * selected from a range of options. Each radio button corresponds
 * to a particular option that may be selected. A radio button
 * has an indicator to indicate whether it is the currently-selected
 * value, and a text label.
 *
 * Internally, a radio button tracks an integer variable that may take
 * a range of different values. Each radio button has a particular
 * value associated with it; if the variable is equal to that value,
 * the radio button appears selected. If a radio button is pressed
 * by the user through the GUI, the variable is set to its value.
 *
 * When a radio button is selected, the "selected" signal is emitted.
 */
class RadioButton : public Widget<RadioButton>
{
	std::string label;
	int* variable;
	int value;

public:

	RadioButton() //: widget_class{Selectable, CalculateSize, Draw, KeyPress, MousePress, SetLayout, SetFocus, Destroy}
	{}

	inline bool Selectable() const noexcept override final
	{
		return true;
	}

	inline void CalculateSize() noexcept override final
	{}

	inline void Draw() noexcept override final
	{}

	inline bool KeyPress(KeyEvent key) noexcept override final
	{
/*
		if (key == KEY_ENTER || key == ' ')
		{
			EmitSignal("changed");
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
/*
	static void RadioButtonSizeCalc(UNCAST_ARG(radiobutton))
	{
		CAST_ARG(RadioButton, radiobutton);

		// Minimum width is the string length + right-side spaces for padding

		radiobutton->widget.w = UTF8_Strlen(radiobutton->label) + 5;
		radiobutton->widget.h = 1;
	}

	static void RadioButtonDrawer(UNCAST_ARG(radiobutton))
	{

		CAST_ARG(RadioButton, radiobutton);
		SavedColors colors;
		int i;
		int w;

		w = radiobutton->widget.w;

		SaveColors(&colors);
		FGColor(ColorType::bright_cyan);
		DrawString("(");

		FGColor(ColorType::bright_white);

		if (*radiobutton->variable == radiobutton->value)
		{
			DrawCodePageString("\x07");
		}
		else
		{
			DrawString(" ");
		}

		FGColor(ColorType::bright_cyan);

		DrawString(") ");

		RestoreColors(&colors);
		SetWidgetBG(radiobutton);

		DrawString(radiobutton->label);

		for (i = UTF8_Strlen(radiobutton->label); i < w - 5; ++i)
		{
			DrawString(" ");
		}

	}

	static void RadioButtonDestructor(UNCAST_ARG(radiobutton))
	{
		CAST_ARG(RadioButton, radiobutton);

		free(radiobutton->label);
	}

	static int RadioButtonKeyPress(UNCAST_ARG(radiobutton), int key)
	{
		CAST_ARG(RadioButton, radiobutton);

		if (key == KEY_ENTER || key == ' ')
		{
			if (*radiobutton->variable != radiobutton->value)
			{
				*radiobutton->variable = radiobutton->value;
				EmitSignal(radiobutton, "selected");
			}
			return 1;
		}

		return 0;
	}

	static void RadioButtonMousePress(UNCAST_ARG(radiobutton), int x, int y, int b)
	{
		CAST_ARG(RadioButton, radiobutton);

		if (b == MOUSE_LEFT)
		{
			// Equivalent to pressing enter

			RadioButtonKeyPress(radiobutton, KEY_ENTER);
		}
	}

	WidgetClass txt_radiobutton_class{
		AlwaysSelectable,
		RadioButtonSizeCalc,
		RadioButtonDrawer,
		RadioButtonKeyPress,
		RadioButtonDestructor,
		RadioButtonMousePress,
		NULL
	};

	RadioButton(std::string& _label, int* variable, int value) : label{std::string(_label)}, variable{variable}, value{value}
	{
	}

	void SetRadioButtonLabel(RadioButton* radiobutton, std::string& _label)
	{
		label = std::string(_label);
	}
/**/
};

} // END NAMESPACE cudadoom::txt
