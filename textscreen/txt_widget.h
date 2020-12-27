/**********************************************************************************************************************************************\
	Copyright(C) 2005-2014 Simon Howard

	This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License
	as published by the Free Software Foundation; either version 2 of the License, or (at your option) any later version.

	This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
\**********************************************************************************************************************************************/
#pragma once

#include "../derma/common.h"

/**
 * @file txt_widget.h
 *
 * Base "widget" GUI component class.
 */
#ifndef DOXYGEN

#define TXT_UNCAST_ARG_NAME(name) uncast_ ## name
#define TXT_UNCAST_ARG(name) void * TXT_UNCAST_ARG_NAME(name)
#define TXT_CAST_ARG(type, name) type *name = (type *) uncast_ ## name

#else
#define TXT_UNCAST_ARG(name) cudadoom::txt::Widget *name
#endif

namespace cudadoom::txt
{

enum class AlignVertical
{
	TXT_VERT_TOP,
	TXT_VERT_CENTER,
	TXT_VERT_BOTTOM
};

enum class AlignHorizontal
{
	TXT_HORIZ_LEFT,
	TXT_HORIZ_CENTER,
	TXT_HORIZ_RIGHT
};

/**
 * A GUI widget.
 *
 * A widget is an individual component of a GUI. Various different widget
 * types exist.
 *
 * Widgets may emit signals. The types of signal emitted by a widget
 * depend on the type of the widget. It is possible to be notified
 * when a signal occurs using the @ref TXT_SignalConnect function.
 */

typedef void (*WidgetSizeCalc)(TXT_UNCAST_ARG(widget));
typedef void (*WidgetDrawer)(TXT_UNCAST_ARG(widget));
typedef void (*WidgetDestroy)(TXT_UNCAST_ARG(widget));
typedef int (*WidgetKeyPress)(TXT_UNCAST_ARG(widget), int key);
typedef void (*WidgetSignalFunc)(TXT_UNCAST_ARG(widget), void* user_data);
typedef void (*MousePressFunc)(TXT_UNCAST_ARG(widget), int x, int y, int b);
typedef void (*WidgetLayoutFunc)(TXT_UNCAST_ARG(widget));
typedef int (*WidgetSelectableFunc)(TXT_UNCAST_ARG(widget));
typedef void (*WidgetFocusFunc)(TXT_UNCAST_ARG(widget), bool focused);

struct CallbackEntry
{
public:
	char* signal_name{nullptr};
	void* user_data{nullptr};
	WidgetSignalFunc func{nullptr};
};

struct CallbackTable
{
public:
	std::vector<CallbackEntry> callbacks;
	size_t numCallbacks{0ull};
	size_t refCount{0ull};

public:
	CallbackTable() noexcept : callbacks(20)
	{
	}
};

class WidgetClass
{
public:
	WidgetSelectableFunc selectable;
	WidgetSizeCalc size_calc;
	WidgetDrawer drawer;
	WidgetKeyPress key_press;
	WidgetDestroy destructor;
	MousePressFunc mouse_press;
	WidgetLayoutFunc layout;
	WidgetFocusFunc focus_change;

public:
	//TxtWidgetClass(){};
};

class Widget
{
	// TEMP PUBLIC UNTIL ALL DEPENDENTS CONVERTED TODO UPDATE THIS
public:
	Widget* parent{nullptr};
	WidgetClass* widget_class{nullptr};
	CallbackTable* callback_table{nullptr};

	// These are set automatically when the window is drawn and should not be set manually.
	int64_t x{};
	int64_t y{};
	uint64_t width{};
	uint64_t height{};

	AlignHorizontal align{};

	bool _visible{false};
	bool _focused{false};

public:
	inline bool visible() const noexcept
	{
		return _visible;
	}

	inline bool focused() const noexcept
	{
		return _focused;
	}

	inline void toggleVisible() noexcept
	{
		_visible = !_visible;
	}

	inline void toggleFocus() noexcept
	{
		_focused = !_focused;
	}

	inline void setVisible() noexcept
	{
		_visible = true;
	}

	inline void setFocus() noexcept
	{
		_focused = true;
	}

	inline void unsetVisible() noexcept
	{
		_visible = false;
	}

	inline void unsetFocus() noexcept
	{
		_focused = false;
	}

	inline void toggleVisibility() noexcept
	{
		toggleVisible();
	}

	inline void show() noexcept
	{
		setVisible();
	}

	inline void hide() noexcept
	{
		unsetVisible();
	}

	inline void focus() noexcept
	{
		setFocus();
	}

	inline void unfocus() noexcept
	{
		unsetFocus();
	}

	inline void setFocused() noexcept
	{
		setFocus();
	}

	inline void unsetFocused() noexcept
	{
		unsetFocus();
	}

	inline void TXT_CalcWidgetSize(TXT_UNCAST_ARG(widget))
	{
		TXT_CAST_ARG(Widget, widget);

		widget->widget_class->size_calc(widget);
	}

	void TXT_DrawWidget(TXT_UNCAST_ARG(widget));
	void TXT_EmitSignal(TXT_UNCAST_ARG(widget), const char* signal_name);
	int TXT_WidgetKeyPress(TXT_UNCAST_ARG(widget), int key);
	void TXT_WidgetMousePress(TXT_UNCAST_ARG(widget), int x, int y, int b);
	void TXT_DestroyWidget(TXT_UNCAST_ARG(widget));
	void TXT_LayoutWidget(TXT_UNCAST_ARG(widget));
	bool TXT_AlwaysSelectable(TXT_UNCAST_ARG(widget));
	bool TXT_NeverSelectable(TXT_UNCAST_ARG(widget));
	void TXT_SetWidgetFocus(TXT_UNCAST_ARG(widget), bool focused);
	void TXT_SignalConnect(TXT_UNCAST_ARG(widget), const char* signal_name, WidgetSignalFunc func, void* user_data);
	void TXT_SetWidgetAlign(TXT_UNCAST_ARG(widget), AlignHorizontal horiz_align);
	int TXT_SelectableWidget(TXT_UNCAST_ARG(widget));
	bool TXT_HoveringOverWidget(TXT_UNCAST_ARG(widget));
	void TXT_SetWidgetBG(TXT_UNCAST_ARG(widget));
	bool TXT_ContainsWidget(TXT_UNCAST_ARG(haystack), TXT_UNCAST_ARG(needle));
};

void TXT_InitWidget(TXT_UNCAST_ARG(widget), WidgetClass* widget_class);
void TXT_CalcWidgetSize(TXT_UNCAST_ARG(widget));
void TXT_DrawWidget(TXT_UNCAST_ARG(widget));
void TXT_EmitSignal(TXT_UNCAST_ARG(widget), const char* signal_name);
int TXT_WidgetKeyPress(TXT_UNCAST_ARG(widget), int key);
void TXT_WidgetMousePress(TXT_UNCAST_ARG(widget), int x, int y, int b);
void TXT_DestroyWidget(TXT_UNCAST_ARG(widget));
void TXT_LayoutWidget(TXT_UNCAST_ARG(widget));
bool TXT_AlwaysSelectable(TXT_UNCAST_ARG(widget));
bool TXT_NeverSelectable(TXT_UNCAST_ARG(widget));
void TXT_SetWidgetFocus(TXT_UNCAST_ARG(widget), bool focused);

/**
 * Set a callback function to be invoked when a signal occurs.
 *
 * @param widget		The widget to watch.
 * @param signal_name The signal to watch.
 * @param func			The callback function to invoke.
 * @param user_data	User-specified pointer to pass to the callback function.
 */
void TXT_SignalConnect(TXT_UNCAST_ARG(widget), const char* signal_name, WidgetSignalFunc func, void* user_data);

/**
 * Set the policy for how a widget should be aligned within a table.
 * By default, widgets are aligned to the left of the column.
 *
 * @param widget		The widget.
 * @param horiz_align The alignment to use.
 */
void TXT_SetWidgetAlign(TXT_UNCAST_ARG(widget), AlignHorizontal horiz_align);

/**
 * Query whether a widget is selectable with the cursor.
 *
 * @param widget		The widget.
 * @return				Non-zero if the widget is selectable.
 */
bool TXT_SelectableWidget(TXT_UNCAST_ARG(widget));

/**
 * Query whether the mouse is hovering over the specified widget.
 *
 * @param widget		The widget.
 * @return				Non-zero if the mouse cursor is over the widget.
 */
bool TXT_HoveringOverWidget(TXT_UNCAST_ARG(widget));

/**
 * Set the background to draw the specified widget, depending on
 * whether it is selected and the mouse is hovering over it.
 *
 * @param widget		The widget.
 */
void TXT_SetWidgetBG(TXT_UNCAST_ARG(widget));

/**
 * Query whether the specified widget is contained within another
 * widget.
 *
 * @param haystack		The widget that might contain needle.
 * @param needle		The widget being queried.
 */
bool TXT_ContainsWidget(TXT_UNCAST_ARG(haystack), TXT_UNCAST_ARG(needle));

} /* END NAMESPACE cudadoom::txt */

//using TxtWidgetSizeCalc = cudadoom::txt::WidgetSizeCalc;
//using TxtWidgetDrawer = cudadoom::txt::WidgetDrawer;
//using TxtWidgetDestroy = cudadoom::txt::WidgetDestroy;
//using TxtWidgetKeyPress = cudadoom::txt::WidgetKeyPress;
//using TxtWidgetSignalFunc = cudadoom::txt::WidgetSignalFunc;
//using TxtMousePressFunc = cudadoom::txt::MousePressFunc;
//using TxtWidgetLayoutFunc = cudadoom::txt::WidgetLayoutFunc;
//using TxtWidgetSelectableFunc = cudadoom::txt::WidgetSelectableFunc;
//using TxtWidgetFocusFunc = cudadoom::txt::WidgetFocusFunc;

//using txt_callback_t = cudadoom::txt::CallbackEntry;

//using txt_callback_table_t = cudadoom::txt::CallbackTable;

//using cudadoom::txt::WidgetClass = cudadoom::txt::WidgetClass;

//using cudadoom::txt::Widget = cudadoom::txt::Widget;
