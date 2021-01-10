/**********************************************************************************************************************************************\
	Copyright(C) 2005-2014 Simon Howard

	This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License
	as published by the Free Software Foundation; either version 2 of the License, or (at your option) any later version.

	This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
\**********************************************************************************************************************************************/
#pragma once

#include "txt_common.h"
#include "txt_widget.h"

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

class Strut : public WidgetBase<Strut>
{
	//void Draw() override final = delete;
	//bool KeyPress(Keys key) override final = delete;
	//bool MousePress(MouseEvent evt) override final = delete;
	//void SetLayout() override final = delete;
	//void SetFocus(bool _focus) override final = delete;
public:
	Strut(int _width, int _height, WidgetBase<Strut>& _parent) //: width{_width}, height{_height}, parent{&_parent},
		//widget_class{Selectable, CalculateSize, nullptr, nullptr, nullptr, nullptr, nullptr, Destroy}
	{
	}

	inline bool Selectable() const noexcept override final
	{
		return false;
	}

	inline void CalculateSize() noexcept override final
	{
		// Minimum width is the string length + two spaces for padding
	}

	inline void Destroy() noexcept override final {}

};

} // end namespace cudadoom::txt
