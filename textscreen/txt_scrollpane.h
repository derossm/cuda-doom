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

#include "txt_widget.h"

namespace cudadoom::txt
{

/**
 * Scrollable pane widget.
 *
 * A scrollable pane widget is a widget that contains another widget
 * that is larger than it. Scroll bars appear on the side to allow
 * different areas of the contained widget to be seen.
 */
class ScrollPane : public Widget<ScrollPane>
{
	Widget widget;
	int w;
	int h;
	int x;
	int y;
	int expand_w;
	int expand_h;
	Widget* child;

public:

	ScrollPane() : widget_class{Selectable, CalculateSize, Draw, KeyPress, MousePress, SetLayout, SetFocus, Destroy}
	{}

	bool Selectable() noexcept override final
	{
		return true;
	}

	void CalculateSize() noexcept override final
	{}

	void Draw() noexcept override final
	{}

	bool KeyPress(Keytype key) noexcept override final
	{
		if (key == KEY_ENTER || key == ' ')
		{
			EmitSignal("changed");
			return true;
		}

		return false;
	}

	bool MousePress(MouseEvent evt) noexcept override final
	{
		if (evt.button == MOUSE_LEFT)
		{
			// Equivalent to pressing enter
			return KeyPress(KEY_ENTER);
		}

		return false
	}

	void SetLayout() noexcept override final
	{}

	void SetFocus(bool _focus) noexcept override final
	{}

	void Destroy() noexcept override final
	{}
};

/**
 * Create a new scroll pane widget.
 *
 * @param w				Width of the scroll pane, in characters.
 * @param h				Height of the scroll pane, in lines.
 * @param target			The target widget that the scroll pane will
 *						contain.
 * @return				Pointer to the new scroll pane widget.
 */
txt_scrollpane_t* NewScrollPane(int w, int h, UNCAST_ARG(target));

} /* END NAMESPACE cudadoom::txt */
