/**********************************************************************************************************************************************\
	Copyright(C) 2005-2014 Simon Howard

	This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License
	as published by the Free Software Foundation; either version 2 of the License, or (at your option) any later version.

	This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

	Original Description:
		CheckBox widget.

		A checkbox is used to control bool values that may be either on or off. The widget has a label that is displayed to the right of the
		checkbox indicator. The widget tracks an integer variable; if the variable is non-zero, the checkbox is checked, while if it is zero,
		the checkbox is unchecked. It is also possible to create "inverted" checkboxes where this logic is reversed.

		When a checkbox is changed, it emits the "changed" signal.
\**********************************************************************************************************************************************/
#pragma once

#include "txt_common.h"

#include "txt_widget.h"

namespace cudadoom::txt
{


class CheckBox : public WidgetBase<CheckBox>
{
	::std::string label;
	int variable;
	bool inverted;

public:

	CheckBox() {} //: widget_class{Selectable, CalculateSize, Draw, KeyPress, MousePress, SetLayout, SetFocus, Destroy}

	inline bool Selectable() const noexcept override final
	{
		return true;
	}

	inline void CalculateSize() noexcept override final
	{
		// Minimum width is the string length + right-side space for padding

		//width = label.size() + 5;
		//height = 1;
	}

	inline void Draw() noexcept override final
	{
/*
		SavedColors colors;

		SaveColors(&colors);
		FGColor(ColorType::bright_cyan);
		DrawString("(");

		FGColor(ColorType::bright_white);

		if ((*variable != 0) ^ inverted)
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
		SetWidgetBG();
		DrawString(label);

		for (size_t i{label.size()}; i < width - 4; ++i)
		{
			DrawString(" ");
		}
/**/
	}

	inline bool KeyPress(Keys key) noexcept override final
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

	inline void SetLayout() noexcept override final {}

	inline void SetFocus(bool _focus) noexcept override final {}

	inline void Destroy() noexcept override final {}
/*
	WidgetClass txt_checkbox_class =
	{
		AlwaysSelectable,
		CheckBoxSizeCalc,
		CheckBoxDrawer,
		CheckBoxKeyPress,
		CheckBoxDestructor,
		CheckBoxMousePress,
		NULL,
	};

	CheckBox(::std::string& _label, int* _variable) :
		widget_class{Selectable, CalculateSize, Draw, KeyPress, MousePress, SetLayout, SetFocus, Destroy}
	{
		label = ::std::string(_label);
		variable = _variable;
		inverted = false;
	}

	auto NewInvertedCheckBox(::std::string& _label, int* _variable)
	{
		CheckBox result(_label, _variable);
		result.inverted = true;

		return ::std::move(result);
	}
/**/
};

} // end namespace cudadoom::txt
