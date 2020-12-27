/**********************************************************************************************************************************************\
	Copyright(C) 2005-2014 Simon Howard

	This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License
	as published by the Free Software Foundation; either version 2 of the License, or (at your option) any later version.

	This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
\**********************************************************************************************************************************************/

#include "txt_io.h"
#include "txt_widget.h"
#include "txt_gui.h"
#include "txt_desktop.h"

namespace cudadoom::txt
{

CallbackTable* TXT_NewCallbackTable()
{
	CallbackTable* table = static_cast<decltype(table)>(malloc(sizeof(CallbackTable)));
	// TODO finish
	//table->callbacks = nullptr;
	table->numCallbacks = 0;
	table->refCount = 1;

	return table;
}

void TXT_RefCallbackTable(CallbackTable* table)
{
	++table->refCount;
}

void TXT_UnrefCallbackTable(CallbackTable* table)
{
	--table->refCount;

	if (table->refCount == 0)
	{
		// No more references to this table

		for (auto i{0}; i<table->numCallbacks; ++i)
		{
			free(table->callbacks[i].signal_name);
		}

		//free(table->callbacks);
		free(table);
	}
}

[[deprecated]]
void TXT_InitWidget(TXT_UNCAST_ARG(widget), WidgetClass* widget_class)
{
	TXT_CAST_ARG(Widget, widget);

	widget->widget_class = widget_class;
	widget->callback_table = TXT_NewCallbackTable();
	widget->parent = nullptr;

	// Not focused until we hear otherwise.
	widget->setFocus();

	// Visible by default.
	widget->setVisible();

	// Align left by default
	widget->align = AlignHorizontal::TXT_HORIZ_LEFT;
}

void TXT_SignalConnect(TXT_UNCAST_ARG(widget), const char* signal_name, WidgetSignalFunc func, void* user_data)
{
	TXT_CAST_ARG(Widget, widget);

	//txt_callback_table_t*
	auto table{widget->callback_table};

	// Add a new callback to the table
	//table->callbacks = static_cast<decltype(table->callbacks)>(realloc(table->callbacks, sizeof(CallbackEntry) * (table->numCallbacks + 1)));

	//txt_callback_t*
	auto callback{&table->callbacks[table->numCallbacks]};
	++table->numCallbacks;

	callback->signal_name = strdup(signal_name);
	callback->func = func;
	callback->user_data = user_data;
}

void TXT_EmitSignal(TXT_UNCAST_ARG(widget), const char* signal_name)
{
	TXT_CAST_ARG(Widget, widget);

	// txt_callback_table_t*
	auto table{widget->callback_table};

	// Don't destroy the table while we're searching through it
	// (one of the callbacks may destroy this window)
	TXT_RefCallbackTable(table);

	// Search the table for all callbacks with this name and invoke
	// the functions.
	for (size_t i{0}; i < table->numCallbacks; ++i)
	{
		if (!strcmp(table->callbacks[i].signal_name, signal_name))
		{
			table->callbacks[i].func(widget, table->callbacks[i].user_data);
		}
	}

	// Finished using the table
	TXT_UnrefCallbackTable(table);
}

void TXT_DrawWidget(TXT_UNCAST_ARG(widget))
{
	TXT_CAST_ARG(Widget, widget);
	txt_saved_colors_t colors;

	// The drawing function might change the fg/bg colors,
	// so make sure we restore them after it's done.
	TXT_SaveColors(&colors);

	// For convenience...
	TXT_GotoXY(widget->x, widget->y);

	// Call drawer method
	widget->widget_class->drawer(widget);

	TXT_RestoreColors(&colors);
}

// TODO MAKE SURE ALL THIS IS BEING DONE in the REAL destructor
void TXT_DestroyWidget(TXT_UNCAST_ARG(widget))
{
	TXT_CAST_ARG(Widget, widget);

	//widget->widget_class->destructor(widget);
	TXT_UnrefCallbackTable(widget->callback_table);
	//free(widget);
}

int TXT_WidgetKeyPress(TXT_UNCAST_ARG(widget), int key)
{
	TXT_CAST_ARG(Widget, widget);

	if (widget->widget_class->key_press != nullptr)
	{
		return widget->widget_class->key_press(widget, key);
	}

	return 0;
}

void TXT_SetWidgetFocus(TXT_UNCAST_ARG(widget), bool focused)
{
	TXT_CAST_ARG(Widget, widget);

	if (widget == nullptr)
	{
		return;
	}

	if (!widget->focused())
	{
		widget->setFocus();

		if (widget->widget_class->focus_change != nullptr)
		{
			widget->widget_class->focus_change(widget, focused);
		}
	}
}

void TXT_SetWidgetAlign(TXT_UNCAST_ARG(widget), AlignHorizontal horiz_align)
{
	TXT_CAST_ARG(Widget, widget);

	widget->align = horiz_align;
}

void TXT_WidgetMousePress(TXT_UNCAST_ARG(widget), int x, int y, int b)
{
	TXT_CAST_ARG(Widget, widget);

	if (widget->widget_class->mouse_press != nullptr)
	{
		widget->widget_class->mouse_press(widget, x, y, b);
	}
}

void TXT_LayoutWidget(TXT_UNCAST_ARG(widget))
{
	TXT_CAST_ARG(Widget, widget);

	if (widget->widget_class->layout != nullptr)
	{
		widget->widget_class->layout(widget);
	}
}

bool TXT_AlwaysSelectable(TXT_UNCAST_ARG(widget))
{
	return true;
}

bool TXT_NeverSelectable(TXT_UNCAST_ARG(widget))
{
	return false;
}

int TXT_SelectableWidget(TXT_UNCAST_ARG(widget))
{
	TXT_CAST_ARG(Widget, widget);

	if (widget->widget_class->selectable != nullptr)
	{
		return widget->widget_class->selectable(widget);
	}
	else
	{
		return 0;
	}
}

bool TXT_ContainsWidget(TXT_UNCAST_ARG(haystack), TXT_UNCAST_ARG(needle))
{
	TXT_CAST_ARG(Widget, haystack);
	TXT_CAST_ARG(Widget, needle);

	while (needle != nullptr)
	{
		if (needle == haystack)
		{
			return true;
		}

		needle = needle->parent;
	}

	return false;
}

bool TXT_HoveringOverWidget(TXT_UNCAST_ARG(widget))
{
	TXT_CAST_ARG(Widget, widget);
	txt_window_t* active_window;
	int x;
	int y;

	// We can only be hovering over widgets in the active window.
	active_window = TXT_GetActiveWindow();

	if (active_window == nullptr || !TXT_ContainsWidget(active_window, widget))
	{
		return false;
	}

	// Is the mouse cursor within the bounds of the widget?
	TXT_GetMousePosition(&x, &y);

	return (x >= widget->x && x < widget->x + widget->width && y >= widget->y && y < widget->y + widget->height);
}

void TXT_SetWidgetBG(TXT_UNCAST_ARG(widget))
{
	TXT_CAST_ARG(Widget, widget);

	if (widget->focused())
	{
		TXT_BGColor(txt_color_t::TXT_COLOR_GREY, false);
	}
	else if (TXT_HoveringOverWidget(widget))
	{
		TXT_BGColor(TXT_HOVER_BACKGROUND, false);
	}
	else
	{
		// Use normal window background.
	}
}

} /* END NAMESPACE cudadoom::txt */
