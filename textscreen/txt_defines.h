/**********************************************************************************************************************************************\
	Copyright(C) 2005-2014 Simon Howard

	This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License
	as published by the Free Software Foundation; either version 2 of the License, or (at your option) any later version.

	This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

\**********************************************************************************************************************************************/
#pragma once

//---------------------------------------------------------------------------
// txt_gui.h
//---------------------------------------------------------------------------
#define TXT_INACTIVE_WINDOW_BACKGROUND		txt_color_t::TXT_COLOR_BLACK
#define TXT_ACTIVE_WINDOW_BACKGROUND		txt_color_t::TXT_COLOR_BLUE
#define TXT_HOVER_BACKGROUND				txt_color_t::TXT_COLOR_CYAN

//---------------------------------------------------------------------------
// txt_main.h
//---------------------------------------------------------------------------

#define TXT_KEY_TO_MOUSE_BUTTON(x)		( (x) >= TXT_MOUSE_BASE && (x) < TXT_MOUSE_BASE + TXT_MAX_MOUSE_BUTTONS ? (x) - TXT_MOUSE_BASE : -1 )

// Convert a key value to a Unicode character:
#define TXT_KEY_TO_UNICODE(x)			( (x) < 128 ? (x) : (x) >= TXT_UNICODE_BASE ? ((x) - TXT_UNICODE_BASE + 128) : 0 )

// Convert a Unicode character to a key value:
#define TXT_UNICODE_TO_KEY(u)			( (u) < 128 ? (u) : ((u) - 128 + TXT_UNICODE_BASE) )

#ifdef __GNUC__
	#define PRINTF_ATTR(fmt, first) __attribute__((format(printf, fmt, first)))
#else // __GNUC__
	#define PRINTF_ATTR(fmt, first)
#endif // __GNUC__

//---------------------------------------------------------------------------
// txt_widget.h
//---------------------------------------------------------------------------
#ifndef DOXYGEN
	#define TXT_UNCAST_ARG_NAME(name) uncast_ ## name
	#define TXT_UNCAST_ARG(name) void * TXT_UNCAST_ARG_NAME(name)
	#define TXT_CAST_ARG(type, name) type *name = (type *) uncast_ ## name
#else
	#define TXT_UNCAST_ARG(name) cudadoom::txt::Widget *name
#endif

//---------------------------------------------------------------------------
// txt_table.h
//---------------------------------------------------------------------------

// Magic value that if used in a table, will indicate that the cell is empty and the widget in the cell to the left can overflow into it.
#define TXT_TABLE_OVERFLOW_RIGHT (&txt_table_overflow_right)

// Magic value that if used in a table, will indicate that the cell is empty and the widget in the cell above it can overflow down into it.
#define TXT_TABLE_OVERFLOW_DOWN (&txt_table_overflow_down)

// Magic value that if given to TXT_AddWidget(), will pad out all columns until the end of line.
#define TXT_TABLE_EOL (&txt_table_eol)

// Indicates empty space to AddWidgets(). Equivalent to AddWidget(table, NULL), except AddWidgets() uses NULL for end of input.
#define TXT_TABLE_EMPTY (&txt_table_empty)
