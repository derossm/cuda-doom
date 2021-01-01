/**********************************************************************************************************************************************\
	Copyright(C) 2016 Simon Howard

	This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License
	as published by the Free Software Foundation; either version 2 of the License, or (at your option) any later version.

	This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
\**********************************************************************************************************************************************/
//#pragma once

//#include "../derma/common.h"

export module txt_conditional;

import std.core;

import textscreen;

import txt_widget;
import txt_strut;

export
{

namespace cudadoom::txt
{

/**
 * Conditional widget.
 *
 * A conditional widget contains another widget, and conditionally
 * shows or hides it based on the value of a variable.
 */
struct Conditional
{
	Widget widget;
	int *var;
	int expected_value;
	Widget* child;

	int ConditionTrue()
	{
		return *var == expected_value;
	}

	int CondSelectable()
	{
		return ConditionTrue() && child->SelectableWidget();
	}

	void CondSizeCalc()
	{
		if (!ConditionTrue())
		{
			width = 0;
			height = 0;
		}
		else
		{
			child->CalcWidgetSize();
			width = child->width;
			height = child->height;
		}
	}

	void CondLayout()
	{
		if (ConditionTrue())
		{
			child->coordinates.x = coordinates.x;
			child->coordinates.y = coordinates.y;
			child->LayoutWidget();
		}
	}

	void CondDrawer()
	{
		if (ConditionTrue())
		{
			child->DrawWidget();
		}
	}

	void CondDestructor()
	{
		//DestroyWidget(child);
	}

	void CondFocused(int focused)
	{
		if (ConditionTrue())
		{
			child->SetWidgetFocus(focused);
		}
	}

	int CondKeyPress(int key)
	{
		if (ConditionTrue())
		{
			return child->WidgetKeyPress(key);
		}

		return 0;
	}

	void CondMousePress(int x, int y, int b)
	{
		if (ConditionTrue())
		{
			child->WidgetMousePress(x, y, b);
		}
	}

	WidgetClass txt_conditional_class =
	{
		CondSelectable,
		CondSizeCalc,
		CondDrawer,
		CondKeyPress,
		CondDestructor,
		CondMousePress,
		CondLayout,
		CondFocused,
	};

	Conditional(int* _var, int _expected_value, Widget* _child) : var(_var), expected_value(_expected_value), child(_child)
	{
		child->parent = (Widget*)this;
	}

	// "Static" conditional that returns an empty strut if the given static value is false. Kind of like a conditional but we only evaluate it at creation time.
	Widget* If(int conditional, Widget* child)
	{
		if (conditional)
		{
			return child;
		}
		else
		{
			//txt_strut_t* nullwidget;
			//DestroyWidget(child);
			//nullwidget = NewStrut(0, 0);
			//return &nullwidget->widget;
			// FIXME
			return nullptr;
		}
	}
};

} /* END NAMESPACE cudadoom::txt */

}
