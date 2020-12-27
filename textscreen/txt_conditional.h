/**********************************************************************************************************************************************\
	Copyright(C) 2016 Simon Howard

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
 * Conditional widget.
 *
 * A conditional widget contains another widget, and conditionally
 * shows or hides it based on the value of a variable.
 */
struct txt_conditional_t
{
	Widget widget;
	int *var;
	int expected_value;
	Widget *child;
};

/**
 * Create a new conditional widget.
 *
 * @param var				The variable to check.
 * @param expected_value If the variable has this value, the widget is shown.
 * @param child			The inner widget to show or hide.
 * @return				Pointer to the new conditional widget.
 */
txt_conditional_t* TXT_NewConditional(int* var, int expected_value, TXT_UNCAST_ARG(child));

/**
 * Return the given child widget if the given bool condition is true.
 *
 * If the condition is not true, the child widget is destroyed and a dummy
 * "null" widget is returned that shows nothing.
 *
 * @param condition		Boolean condition - true or false value.
 * @param child			Widget to conditionally return.
 * @return					Either child (if condition is true) or a null
 *							widget.
 */
Widget* TXT_If(int condition, TXT_UNCAST_ARG(child));

} /* END NAMESPACE cudadoom::txt */
