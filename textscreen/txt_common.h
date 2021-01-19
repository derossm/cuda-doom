/**********************************************************************************************************************************************\
	Doom
	Copyright(C) 1993-1996 Id Software, Inc.

	textscreen
	Copyright(C) 2005-2014 Simon Howard

	This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License
	as published by the Free Software Foundation; either version 2 of the License, or (at your option) any later version.

	This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

	------------------------------------------------------------
	Any new code or derivations not subject to prior copyrights:
		Copyright© 2020-2021 Mason DeRoss

		Released under the GNU All-permissive License

		Copying and distribution of this file, with or without modification, are permitted in any medium without royalty,
		provided the copyright notice and this notice are preserved. This file is offered as-is, without any warranty.

	DESCRIPTION:

\**********************************************************************************************************************************************/
#pragma once

#include "../derma/common.h"

/*
//---------------------------------------------------------------------------
// txt_main.h
//---------------------------------------------------------------------------

//#define KEY_TO_MOUSE_BUTTON(x) ( (x) >= MOUSE_BASE && (x) < MOUSE_BASE + MAX_MOUSE_BUTTONS ? (x) - MOUSE_BASE : -1 )

// Convert a key value to a Unicode character:
//#define KEY_TO_UNICODE(x) ( (x) < 128 ? (x) : (x) >= UNICODE_BASE ? ((x) - UNICODE_BASE + 128) : 0 )

// Convert a Unicode character to a key value:
//#define UNICODE_TO_KEY(u) ( (u) < 128 ? (u) : ((u) - 128 + UNICODE_BASE) )

//#ifdef __GNUC__
//#define PRINTF_ATTR(fmt, first) __attribute__((format(printf, fmt, first)))
//#else // __GNUC__
//#define PRINTF_ATTR(fmt, first)
//#endif // __GNUC__

//---------------------------------------------------------------------------
// txt_table.h
//---------------------------------------------------------------------------

// Magic value that if used in a table, will indicate that the cell is empty and the widget in the cell to the left can overflow into it.
//#define TABLE_OVERFLOW_RIGHT (&txt_table_overflow_right)

// Magic value that if used in a table, will indicate that the cell is empty and the widget in the cell above it can overflow down into it.
//#define TABLE_OVERFLOW_DOWN (&txt_table_overflow_down)

// Magic value that if given to AddWidget(), will pad out all columns until the end of line.
//#define TABLE_EOL (&txt_table_eol)

// Indicates empty space to AddWidgets(). Equivalent to AddWidget(table, NULL), except AddWidgets() uses NULL for end of input.
//#define TABLE_EMPTY (&txt_table_empty)

// For the moment, txt_sdl.cpp is the only implementation of the base text mode screen API:

// textscreen key values:
// Key values are difficult because we have to support multiple conflicting address spaces.
// First, Doom's key constants use 0-127 as ASCII and extra values from 128-255 to represent special keys.
// Second, mouse buttons are represented as buttons. Finally, we want to be able to support Unicode.

// So we define different ranges:
// 0-255:	Doom key constants, including ASCII.
// 256-511:	Mouse buttons and other reserved.
// >=512:	Unicode values greater than 127 are offset up into this range.
// Special keypress values that correspond to mouse button clicks

//#ifdef HELP_KEY
//#undef HELP_KEY
//#endif
//constexpr size_t HELP_KEY{KEY_F1};
//typedef void (*TxtIdleCallback)(void* user_data);
*/

namespace cudadoom::txt
{

//========================================
// Forward Declarations
template<class T>
class WidgetClass;

template<class T, class U>
class WidgetBase;
	class Widget;

	// is-a Widget
	template<class T>
	class TableBase;
		// is-a TableBase
		class Table;
		class Window;

	// is-a Widget
	template<class T>
	class InputBase;
		// is-a InputBase
		class InputBox;
		class FileSelect;

	// is-a Widget
	class Button;
	class CheckBox;
	class Conditional;
	class DropDownList;
	class Label;
	class RadioButton;
	class ScrollPane;
	class Separator;
	class SpinControl;
	class Strut;
	class WindowAction;

//========================================
// Textscreen Constants
constexpr short MOUSE_BASE{256};
constexpr short MOUSE_LEFT{MOUSE_BASE + 0};
constexpr short MOUSE_RIGHT{MOUSE_BASE + 1};
constexpr short MOUSE_MIDDLE{MOUSE_BASE + 2};
constexpr short MOUSE_SCROLLUP{MOUSE_BASE + 3};
constexpr short MOUSE_SCROLLDOWN{MOUSE_BASE + 4};
constexpr short MAX_MOUSE_BUTTONS{32};

// Unicode offset. Unicode values from 128 onwards are offset up into this range, so UNICODE_BASE = Unicode character #128, and so on.
constexpr short UNICODE_BASE{512};

// Array of border characters for drawing windows. The array looks like this:

// +-++
// | ||
// +-++
// +-++
constexpr uint8_t borders[4][4] =
{
	{0xda, 0xc4, 0xc2, 0xbf},
	{0xb3, ' ', 0xb3, 0xb3},
	{0xc3, 0xc4, 0xc5, 0xb4},
	{0xc0, 0xc4, 0xc1, 0xd9},
};

constexpr uint8_t SCROLLBAR_VERTICAL{(1 << 0)};
constexpr uint8_t SCROLLBAR_HORIZONTAL{(1 << 1)};

//========================================
// THESE SHOULD BE DYNAMIC REFACTOR FIXME
// Screen size
constexpr short SCREEN_W{80};
constexpr short SCREEN_H{25};

// Time between character blinks in ms
constexpr short BLINK_PERIOD{250};
constexpr short LARGE_FONT_THRESHOLD{144};
constexpr short COLOR_BLINKING{8};

constexpr size_t MAXWINDOWS{128};
//----------------------------------------

//========================================
// enums
enum class ColorType
{
	black,
	blue,
	green,
	cyan,
	red,
	magenta,
	brown,
	grey,
	dark_grey,
	bright_blue,
	bright_green,
	bright_cyan,
	bright_red,
	bright_magenta,
	yellow,
	bright_white
};

constexpr ColorType INACTIVE_WINDOW_BACKGROUND{ColorType::black};
constexpr ColorType ACTIVE_WINDOW_BACKGROUND{ColorType::blue};
constexpr ColorType HOVER_BACKGROUND{ColorType::cyan};

// Modifier keys.
enum class ModifierType
{
	shift,
	ctrl,
	alt
};

// Due to the way the SDL API works, we provide different ways of configuring how we read input events, each of which is useful in different scenarios.
enum class InputType
{
	// "Localized" output that takes software keyboard layout into account, but key shifting has no effect.
	normal,
	// "Raw" input; the keys correspond to physical keyboard layout and software keyboard layout has no effect.
	raw,
	// Used for full text input. Events are fully shifted and localized. However, not all keyboard keys will generate input.
	// Setting this mode may activate the on-screen keyboard, depending on device and OS.
	text
};


enum class Alignment
{
	top,
	middle,
	bottom,
	left,
	center,
	right,
	count
};

enum class AlignVertical
{
	top,
	center,
	bottom
};

enum class AlignHorizontal
{
	left,
	center,
	right
};

//========================================
// Types and Aliases
using point_t = long double;
using UserData = void*;

struct KeyType {};
struct MouseType {};

using Key = StrongType<int, KeyType>;
using MouseButton = StrongType<int, MouseType>;

struct Vec2
{
	point_t x;
	point_t y;
};

struct Vec3
{
	point_t x;
	point_t y;
	point_t z;
};

struct KeyEvent
{
	Key key;
};

struct MouseEvent
{
	int button;
	float x;
	float y;
};

struct SavedColors
{
	ColorType backgroundColor;
	ColorType foregroundColor;
};

} // end namespace cudadoom::txt
