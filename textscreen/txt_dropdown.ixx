/**********************************************************************************************************************************************\
	Copyright(C) 2005-2014 Simon Howard

	This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License
	as published by the Free Software Foundation; either version 2 of the License, or (at your option) any later version.

	This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
\**********************************************************************************************************************************************/
//#pragma once

//#include "../derma/common.h"

//#include "doomkeys.h"

export module txt_dropdown;

import std.core;

import textscreen;

import txt_main;
import txt_widget;
import txt_window;
import txt_button;
import txt_utf8;
import txt_io;
import txt_gui;

export
{

namespace cudadoom::txt
{

/**
 * Dropdown list widget.
 *
 * A dropdown list allows the user to select from a list of values,
 * which appears when the list is selected.
 *
 * When the value of a dropdown list is changed, the "changed" signal
 * is emitted.
 */

// Drop-down list box.
struct txt_dropdown_list_t
{
	Widget widget;
	int* variable;
	const char** values;
	int num_values;
};

struct callback_data_t
{
	Window* window;
	txt_dropdown_list_t* list;
	int item;
};

/**
 * Create a new dropdown list widget.
 *
 * The parameters specify a list of string labels, and a pointer to an
 * integer variable. The variable contains the current "value" of the
 * list, as an index within the list of labels.
 *
 * @param variable		Pointer to the variable containing the
 *						list's value.
 * @param values			Pointer to an array of strings containing
 *						the labels to use for the list (UTF-8 format).
 * @param num_values		The number of variables in the list.
 */
txt_dropdown_list_t* NewDropdownList(int* variable, const char** values, int num_values);

// Check if the selected value for a list is valid
static int ValidSelection(txt_dropdown_list_t* list)
{
	return *list->variable >= 0 && *list->variable < list->num_values;
}

// Calculate the Y position for the selector window
static int SelectorWindowY(txt_dropdown_list_t* list)
{
	int result;

	if (ValidSelection(list))
	{
		result = list->widget.y - 1 - *list->variable;
	}
	else
	{
		result = list->widget.y - 1 - (list->num_values / 2);
	}

	// Keep dropdown inside the screen.

	if (result < 1)
	{
		result = 1;
	}
	else if (result + list->num_values > (SCREEN_H - 3))
	{
		result = SCREEN_H - list->num_values - 3;
	}

	return result;
}

// Called when a button in the selector window is pressed
static void ItemSelected(UNCAST_ARG(button), UNCAST_ARG(callback_data))
{
	CAST_ARG(callback_data_t, callback_data);

	// Set the variable
	*callback_data->list->variable = callback_data->item;

	EmitSignal(callback_data->list, "changed");

	// Close the window
	CloseWindow(callback_data->window);
}

// Free callback data when the window is closed
static void FreeCallbackData(UNCAST_ARG(list),
								UNCAST_ARG(callback_data))
{
	CAST_ARG(callback_data_t, callback_data);

	free(callback_data);
}

// Catch presses of escape and close the window.
static int SelectorWindowListener(Window* window, int key, void* user_data)
{
	if (key == KEY_ESCAPE)
	{
		CloseWindow(window);
		return 1;
	}

	return 0;
}

static int SelectorMouseListener(Window* window, int x, int y, int b, void* unused)
{
	Widget *win;

	win = (Widget*) window;

	if (x < win->x || x > win->x + win->w || y < win->y || y > win->y + win->h)
	{
		CloseWindow(window);
		return 1;
	}

	return 0;
}

// Open the dropdown list window to select an item
static void OpenSelectorWindow(txt_dropdown_list_t* list)
{
	Window* window;
	int i;

	// Open a simple window with no title bar or action buttons.
	window = NewWindow(NULL);

	SetWindowAction(window, HORIZ_LEFT, NULL);
	SetWindowAction(window, HORIZ_CENTER, NULL);
	SetWindowAction(window, HORIZ_RIGHT, NULL);

	// Position the window so that the currently selected item appears over the top of the list widget.
	SetWindowPosition(window, HORIZ_LEFT, VERT_TOP,
							list->widget.x - 2, SelectorWindowY(list));

	// Add a button to the window for each option in the list.
	for (i=0; i<list->num_values; ++i)
	{
		Button* button;
		callback_data_t* data;

		button = NewButton(list->values[i]);

		AddWidget(window, button);

		// Callback struct

		data = static_cast<decltype(data)>(malloc(sizeof(callback_data_t)));
		data->list = list;
		data->window = window;
		data->item = i;

		// When the button is pressed, invoke the button press callback
		SignalConnect(button, "pressed", ItemSelected, data);

		// When the window is closed, free back the callback struct
		SignalConnect(window, "closed", FreeCallbackData, data);

		// Is this the currently-selected value? If so, select the button in the window as the default.
		if (i == *list->variable)
		{
			SelectWidget(window, button);
		}
	}

	// Catch presses of escape in this window and close it.
	SetKeyListener(window, SelectorWindowListener, NULL);
	SetMouseListener(window, SelectorMouseListener, NULL);
}

static int DropdownListWidth(txt_dropdown_list_t* list)
{
	int i;
	int result;

	// Find the maximum string width
	result = 0;

	for (i=0; i<list->num_values; ++i)
	{
		int w = UTF8_Strlen(list->values[i]);
		if (w > result)
		{
			result = w;
		}
	}

	return result;
}

static void DropdownListSizeCalc(UNCAST_ARG(list))
{
	CAST_ARG(txt_dropdown_list_t, list);

	list->widget.w = DropdownListWidth(list);
	list->widget.h = 1;
}

static void DropdownListDrawer(UNCAST_ARG(list))
{
	CAST_ARG(txt_dropdown_list_t, list);
	unsigned i;
	const char* str;

	// Set bg/fg text colors.

	SetWidgetBG(list);

	// Select a string to draw from the list, if the current value is in range. Otherwise fall back to a default.
	if (ValidSelection(list))
	{
		str = list->values[*list->variable];
	}
	else
	{
		str = "???";
	}

	// Draw the string and fill to the end with spaces
	DrawString(str);

	for (i = UTF8_Strlen(str); i < list->widget.w; ++i)
	{
		DrawString(" ");
	}
}

static void DropdownListDestructor(UNCAST_ARG(list))
{
}

static int DropdownListKeyPress(UNCAST_ARG(list), int key)
{
	CAST_ARG(txt_dropdown_list_t, list);

	if (key == KEY_ENTER)
	{
		OpenSelectorWindow(list);
		return 1;
	}

	return 0;
}

static void DropdownListMousePress(UNCAST_ARG(list), int x, int y, int b)
{
	CAST_ARG(txt_dropdown_list_t, list);

	// Left mouse click does the same as selecting and pressing enter
	if (b == MOUSE_LEFT)
	{
		DropdownListKeyPress(list, KEY_ENTER);
	}
}

WidgetClass txt_dropdown_list_class =
{
	AlwaysSelectable,
	DropdownListSizeCalc,
	DropdownListDrawer,
	DropdownListKeyPress,
	DropdownListDestructor,
	DropdownListMousePress,
	NULL,
};

txt_dropdown_list_t* NewDropdownList(int *variable, const char** values, int num_values)
{
	txt_dropdown_list_t* list;

	list = static_cast<decltype(list)>(malloc(sizeof(txt_dropdown_list_t)));

	InitWidget(list, &txt_dropdown_list_class);
	list->variable = variable;
	list->values = values;
	list->num_values = num_values;

	return list;
}

} /* END NAMESPACE cudadoom::txt */

}
