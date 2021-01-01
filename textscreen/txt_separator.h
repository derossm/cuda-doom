/**********************************************************************************************************************************************\
	Copyright(C) 2005-2014 Simon Howard

	This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License
	as published by the Free Software Foundation; either version 2 of the License, or (at your option) any later version.

	This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
\**********************************************************************************************************************************************/
#pragma once

#include "../derma/common.h"

#include "txt_main.h"
#include "txt_widget.h"
#include "txt_window.h"
#include "txt_utf8.h"
#include "txt_io.h"
#include "txt_gui.h"

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
struct Separator : Widget
{
	std::string label;

	void SizeCalc()
	{
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
	}

	void Drawer()
	{
		int x;
		int y;

		GetXY(&x, &y);

		// Draw separator. Go back one character and draw two extra
		// to overlap the window borders.

		DrawSeparator(x-2, y, width + 4);

		if (label != "")
		{
			GotoXY(x, y);

			FGColor(ColorType::bright_green);
			DrawString(" ");
			DrawString(separator->label);
			DrawString(" ");
		}
	}

	void Destructor()
	{
	}

	void SetLabel(Separator* separator, std::string& _label)
	{
		if (label != "")
		{
			label = std::string(_label);
		}
		else
		{
			label = std::string{};
		}
	}

	WidgetClass txt_separator_class{
		NeverSelectable,
		SeparatorSizeCalc,
		SeparatorDrawer,
		nullptr,
		SeparatorDestructor,
		nullptr,
		nullptr
	};

	Separator(std::string& _label) : label(_label)
	{
	}
};

} /* END NAMESPACE cudadoom::txt */
