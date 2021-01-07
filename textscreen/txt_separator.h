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

#include "txt_widget.h"

#include "txt_main.h"
#include "txt_utf8.h"
#include "txt_io.h"
#include "txt_gui.h"

//#include "txt_window.h"

namespace cudadoom::txt
{

/**
 * @file txt_separator.h
 *
 * Horizontal separator widget.
 */

 /**
  * Horizontal separator.
  *
  * A horizontal separator appears as a horizontal line divider across
  * the length of the window in which it is added. An optional label
  * allows the separator to be used as a section divider for grouping
  * related controls.
  */
class Separator : public Widget<Separator>
{
	std::string label;

	//bool KeyPress(KeyEvent key) override final = delete;
	//bool MousePress(MouseEvent evt) override final = delete;
	//void SetLayout() override final = delete;
	//void SetFocus(bool _focus) override final = delete;

public:
	Separator() //: widget_class{Selectable, CalculateSize, Draw, nullptr, nullptr, nullptr, nullptr, Destroy}
	{}

	inline bool Selectable() const noexcept override final
	{
		return false;
	}

	inline void CalculateSize() noexcept override final
	{
/*
		if (label != "")
		{
			// Minimum width is the string length + two spaces for padding
			width = label.size() + 2;
		}
		else
		{
			width = 0;
		}

		height = 1;
/**/
	}

	inline void Draw() noexcept override final
	{
/*
		int x;
		int y;

		GetXY(&x, &y);

		// Draw separator. Go back one character and draw two extra
		// to overlap the window borders.

		DrawSeparator(x - 2, y, width + 4);

		if (label != "")
		{
			GotoXY(x, y);

			FGColor(ColorType::bright_green);
			DrawString(" ");
			DrawString(label.c_str());
			DrawString(" ");
		}
/**/
	}

	inline void Destroy() noexcept override final
	{}

	void SetLabel(Separator* separator, std::string& _label)
	{
/*
		if (label != "")
		{
			label = std::string(_label);
		}
		else
		{
			label = std::string{};
		}
/**/
	}

	//Separator(std::string& _label) : label(_label)
	//{}
};

} // END NAMESPACE cudadoom::txt
