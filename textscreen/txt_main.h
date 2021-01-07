/**********************************************************************************************************************************************\
	Copyright(C) 2005-2014 Simon Howard

	This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License
	as published by the Free Software Foundation; either version 2 of the License, or (at your option) any later version.

	This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

	DESCRIPTION:
		Base interface that abstracts the text mode screen.
\**********************************************************************************************************************************************/
#pragma once
// DECOUPLE
#include "../derma/common.h"
//////////

#include "txt_defines.h"
#include "txt_sdl.h"

namespace cudadoom::txt
{

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
constexpr int MOUSE_BASE{256};
constexpr int MOUSE_LEFT{MOUSE_BASE + 0};
constexpr int MOUSE_RIGHT{MOUSE_BASE + 1};
constexpr int MOUSE_MIDDLE{MOUSE_BASE + 2};
constexpr int MOUSE_SCROLLUP{MOUSE_BASE + 3};
constexpr int MOUSE_SCROLLDOWN{MOUSE_BASE + 4};
constexpr int MAX_MOUSE_BUTTONS{32};

// Unicode offset. Unicode values from 128 onwards are offset up into this range, so UNICODE_BASE = Unicode character #128, and so on.
constexpr int UNICODE_BASE{512};

// Screen size
constexpr int SCREEN_W{80};
constexpr int SCREEN_H{25};

constexpr int COLOR_BLINKING{8};

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

//---------------------------------------------------------------------------
// txt_gui.h
//---------------------------------------------------------------------------
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

// Initialize the screen. Returns 1 if successful, 0 if failed.
int Init();

// Shut down text mode emulation.
void Shutdown();

// Get a pointer to the buffer containing the raw screen data.
unsigned char* GetScreenData();

// Update an area of the screen.
void UpdateScreenArea(int x, int y, int w, int h);

// Update the whole screen.
void UpdateScreen();

// Set the RGB value for a particular entry in the color palette:
void SetColor(ColorType color, int r, int g, int b);

// Read a character from the keyboard.
int GetChar();

// Given a Unicode character, get a character that can be used to represent it on the code page being displayed on the screen.
// If the character cannot be represented, this returns -1.
int UnicodeCharacter(unsigned c);

// Read the current state of modifier keys that are held down.
int GetModifierState(ModifierType mod);

// Provides a short description of a key code, placing into the provided buffer. Note that the key is assumed to represent a physical key on the
// keyboard (like that returned by InputType::raw), and the resulting string takes keyboard layout into consideration.
// For example, GetKeyDescription('q') on a French keyboard returns "A". The contents of the filled buffer will be in UTF-8 format,
// but will never contain characters which can't be shown on the screen.
void GetKeyDescription(int key, std::string buf, size_t buf_len);

// Retrieve the current position of the mouse
void GetMousePosition(int* x, int* y);

// Sleep until an event is received or the screen needs updating. Optional timeout in ms (timeout == 0 : sleep forever).
void Sleep(TimeType timeout);

// Change mode for text input.
void SetInputMode(InputType mode);

// Set the window title of the window containing the text mode screen.
void SetWindowTitle(std::string title);

// Safe string copy.
void StringCopy(std::string dest, std::string src, size_t dest_len);

// Safe string concatenate.
void StringConcat(std::string dest, std::string src, size_t dest_len);

// Safe version of vsnprintf().
int vsnprintf(std::string buf, size_t buf_len, std::string s, va_list args);

// Safe version of snprintf().
int snprintf(std::string buf, size_t buf_len, std::string s, ...) PRINTF_ATTR(3, 4);

} // END NAMESPACE cudadoom::txt
