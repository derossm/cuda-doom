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
#define INACTIVE_WINDOW_BACKGROUND ColorType::black
#define ACTIVE_WINDOW_BACKGROUND ColorType::blue
#define HOVER_BACKGROUND ColorType::cyan

//---------------------------------------------------------------------------
// txt_main.h
//---------------------------------------------------------------------------

#define KEY_TO_MOUSE_BUTTON(x) ( (x) >= MOUSE_BASE && (x) < MOUSE_BASE + MAX_MOUSE_BUTTONS ? (x) - MOUSE_BASE : -1 )

// Convert a key value to a Unicode character:
#define KEY_TO_UNICODE(x) ( (x) < 128 ? (x) : (x) >= UNICODE_BASE ? ((x) - UNICODE_BASE + 128) : 0 )

// Convert a Unicode character to a key value:
#define UNICODE_TO_KEY(u) ( (u) < 128 ? (u) : ((u) - 128 + UNICODE_BASE) )

#ifdef __GNUC__
	#define PRINTF_ATTR(fmt, first) __attribute__((format(printf, fmt, first)))
#else // __GNUC__
	#define PRINTF_ATTR(fmt, first)
#endif // __GNUC__

//---------------------------------------------------------------------------
// txt_widget.h
//---------------------------------------------------------------------------
#ifndef DOXYGEN
	#define UNCAST_ARG_NAME(name) uncast_ ## name
	#define UNCAST_ARG(name) void* UNCAST_ARG_NAME(name)
	#define CAST_ARG(type, name) type* name = (type*) uncast_ ## name
#else
	#define UNCAST_ARG(name) cudadoom::txt::Widget *name
#endif

//---------------------------------------------------------------------------
// txt_table.h
//---------------------------------------------------------------------------

// Magic value that if used in a table, will indicate that the cell is empty and the widget in the cell to the left can overflow into it.
#define TABLE_OVERFLOW_RIGHT (&txt_table_overflow_right)

// Magic value that if used in a table, will indicate that the cell is empty and the widget in the cell above it can overflow down into it.
#define TABLE_OVERFLOW_DOWN (&txt_table_overflow_down)

// Magic value that if given to AddWidget(), will pad out all columns until the end of line.
#define TABLE_EOL (&txt_table_eol)

// Indicates empty space to AddWidgets(). Equivalent to AddWidget(table, NULL), except AddWidgets() uses NULL for end of input.
#define TABLE_EMPTY (&txt_table_empty)
