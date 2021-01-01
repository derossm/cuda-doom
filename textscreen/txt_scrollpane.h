/**********************************************************************************************************************************************\
	Copyright(C) 2005-2014 Simon Howard

	This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License
	as published by the Free Software Foundation; either version 2 of the License, or (at your option) any later version.

	This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
\**********************************************************************************************************************************************/
#pragma once

#include "../derma/common.h"

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
struct txt_scrollpane_t
{
	Widget widget;
	int w;
	int h;
	int x;
	int y;
	int expand_w;
	int expand_h;
	Widget* child;
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
