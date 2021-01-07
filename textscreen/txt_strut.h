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

#include "txt_widget.h"

#include "txt_main.h"
#include "txt_io.h"

//#include "txt_window.h"

namespace cudadoom::txt
{

//WidgetClass txt_strut_class =
//{
	//NeverSelectable,
	//StrutSizeCalc,
	//StrutDrawer,
	//StrutKeyPress,
	//StrutDestructor,
	//NULL,
	//NULL,
//};

/**
 * Strut widget.
 *
 * A strut is a widget that takes up a fixed amount of space. It can
 * be visualised as a transparent box. Struts are used to provide
 * spacing between widgets.
 */

class Strut : public Widget<Strut>
{

	void Draw() override final = delete;
	bool KeyPress(KeyType key) override final = delete;
	bool MousePress(MouseEvent evt) override final = delete;
	void SetLayout() override final = delete;
	void SetFocus(bool _focus) override final = delete;
public:
	Strut(int _width, int _height, Widget& _parent) : width{_width}, height{_height}, parent{&_parent},
		widget_class{Selectable, CalculateSize, nullptr, nullptr, nullptr, nullptr, nullptr, Destroy}
	{
	}

	bool Selectable() noexcept override final
	{
		return false;
	}

	void CalculateSize() noexcept override final
	{
		// Minimum width is the string length + two spaces for padding
	}

	void Destroy() noexcept override final
	{}

};

/**
 * Create a new strut.
 *
 * @param width		Width of the strut, in characters.
 * @param height		Height of the strut, in characters.
 */

} // END NAMESPACE cudadoom::txt
