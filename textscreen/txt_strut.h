/**********************************************************************************************************************************************\
	Copyright(C) 2005-2014 Simon Howard

	This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License
	as published by the Free Software Foundation; either version 2 of the License, or (at your option) any later version.

	This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
\**********************************************************************************************************************************************/
#pragma once

#include "../derma/common.h"

#include "doomkeys.h"

#include "txt_main.h"
#include "txt_widget.h"
#include "txt_window.h"
#include "txt_io.h"

namespace cudadoom::txt
{

static void StrutSizeCalc(UNCAST_ARG(strut))
{
	CAST_ARG(txt_strut_t, strut);

	// Minimum width is the string length + two spaces for padding

	strut->widget.w = strut->width;
	strut->widget.h = strut->height;
}

static void StrutDrawer(UNCAST_ARG(strut))
{
	// Nothing is drawn for a strut.
}

static void StrutDestructor(UNCAST_ARG(strut))
{
}

static int StrutKeyPress(UNCAST_ARG(strut), int key)
{
	return 0;
}

WidgetClass txt_strut_class =
{
	NeverSelectable,
	StrutSizeCalc,
	StrutDrawer,
	StrutKeyPress,
	StrutDestructor,
	NULL,
	NULL,
};

txt_strut_t* NewStrut(int width, int height)
{
	txt_strut_t* strut = static_cast<decltype(strut)>(malloc(sizeof(txt_strut_t)));

	InitWidget(strut, &txt_strut_class);
	strut->width = width;
	strut->height = height;

	return strut;
}

/**
 * Strut widget.
 *
 * A strut is a widget that takes up a fixed amount of space. It can
 * be visualised as a transparent box. Struts are used to provide
 * spacing between widgets.
 */

struct txt_strut_t
{
	Widget widget;
	int width;
	int height;
};

/**
 * Create a new strut.
 *
 * @param width		Width of the strut, in characters.
 * @param height		Height of the strut, in characters.
 */
txt_strut_t* NewStrut(int width, int height);

} /* END NAMESPACE cudadoom::txt */
