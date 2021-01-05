/**********************************************************************************************************************************************\
	Copyright(C) 2005-2014 Simon Howard

	This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License
	as published by the Free Software Foundation; either version 2 of the License, or (at your option) any later version.

	This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
\**********************************************************************************************************************************************/
#pragma once

#include "../derma/common.h"
#include "txt_common.h"

#include "../src/doomkeys.h"

#include "txt_main.h"
#include "txt_io.h"
#include "txt_gui.h"

//#include "txt_window.h"
//#include "txt_separator.h"

namespace cudadoom::txt
{

#ifdef HELP_KEY
#undef HELP_KEY
#endif
constexpr size_t HELP_KEY{KEY_F1};
constexpr size_t MAXWINDOWS{128};

class Window;

typedef void (*TxtIdleCallback)(void* user_data);

void AddDesktopWindow(Window* win);
void RemoveDesktopWindow(Window* win);
void DrawDesktop();
void DispatchEvents();
void DrawWindow(Window* window);
void SetWindowFocus(Window* window, int focused);
int WindowKeyPress(Window* window, int c);

/**
 * Set the title displayed at the top of the screen.
 *
 * @param title			The title to display (UTF-8 format).
 */
void SetDesktopTitle(std::string title);

/**
 * Exit the currently-running main loop and return from the
 * @ref GUIMainLoop function.
 */
void ExitMainLoop();

/**
 * Start the main event loop. At least one window must have been
 * opened prior to running this function. When no windows are left
 * open, the event loop exits.
 *
 * It is possible to trigger an exit from this function using the
 * @ref ExitMainLoop function.
 */
void GUIMainLoop();

/**
 * Get the top window on the desktop that is currently receiving
 * inputs.
 *
 * @return	The active window, or NULL if no windows are present.
 */
Window* GetActiveWindow();

/**
 * Set a callback function to be invoked periodically by the main
 * loop code.
 *
 * @param callback		The callback to invoke, or NULL to cancel
 *						an existing callback.
 * @param user_data		Extra data to pass to the callback.
 * @param period		Maximum time between invoking each callback.
 *						eg. a value of 200 will cause the callback
 *						to be invoked at least once every 200ms.
 */
void SetPeriodicCallback(TxtIdleCallback callback, void* user_data, unsigned period);

/**
 * Raise the z-position of the given window relative to other windows.
 *
 * @param window		The window to raise.
 * @return				Non-zero if the window was raised successfully,
 *						or zero if the window could not be raised further.
 */
int RaiseWindow(Window* window);

/**
 * Lower the z-position of the given window relative to other windows.
 *
 * @param window		The window to make inactive.
 * @return				Non-zero if the window was lowered successfully,
 *						or zero if the window could not be lowered further.
 */
int LowerWindow(Window* window);

std::string desktop_title;
static Window* all_windows[MAXWINDOWS];
static int num_windows = 0;
static int main_loop_running = 0;

static TxtIdleCallback periodic_callback = nullptr;
static void* periodic_callback_data;
static unsigned periodic_callback_period;

void AddDesktopWindow(Window* window)
{
	// Previously-top window loses focus:
	if (num_windows > 0)
	{
		all_windows[num_windows - 1]->SetWindowFocus(false);
	}

	all_windows[num_windows] = window;
	++num_windows;

	// New window gains focus:
	window->SetWindowFocus(true);
}

void RemoveDesktopWindow(Window* win)
{
	// Window must lose focus if it's being removed:
	win->SetWindowFocus(true);
	size_t to{0};
	for (size_t from{0}; from < num_windows; ++from)
	{
		if (all_windows[from] != win)
		{
			all_windows[to] = all_windows[from];
			++to;
		}
	}

	num_windows = to;

	// Top window gains focus:
	if (num_windows > 0)
	{
		all_windows[num_windows - 1]->SetWindowFocus(true);
	}
}

Window* GetActiveWindow()
{
	if (num_windows == 0)
	{
		return nullptr;
	}

	return all_windows[num_windows - 1];
}

bool RaiseWindow(Window* window)
{
	for (size_t i{0}; i < num_windows - 1; ++i)
	{
		if (all_windows[i] == window)
		{
			all_windows[i] = all_windows[i + 1];
			all_windows[i + 1] = window;

			if (i == num_windows - 2)
			{
				SetWindowFocus(all_windows[i], 0);
				SetWindowFocus(window, 1);
			}

			return true;
		}
	}

	// Window not in the list, or at the end of the list (top) already.
	return false;
}

int LowerWindow(Window* window)
{
	for (size_t i{0}; i < num_windows - 1; ++i)
	{
		if (all_windows[i + 1] == window)
		{
			all_windows[i + 1] = all_windows[i];
			all_windows[i] = window;

			if (i == num_windows - 2)
			{
				SetWindowFocus(window, 0);
				SetWindowFocus(all_windows[i + 1], 1);
			}

			return 1;
		}
	}

	// Window not in the list, or at the start of the list (bottom) already.
	return 0;
}

static void DrawDesktopBackground(std::string title)
{
	auto screendata{GetScreenData()};

	// Fill the screen with gradient characters
	auto p{screendata};

	for (size_t i{0}; i < SCREEN_W * SCREEN_H; ++i)
	{
		*p = 0xb1;
		++p;
		*p = ColorType::grey | (ColorType::blue << 4);
		++p;
	}

	// Draw the top and bottom banners
	p = screendata;

	for (size_t i{0}; i<SCREEN_W; ++i)
	{
		*p = ' ';
		++p;
		*p = ColorType::black | (ColorType::grey << 4);
		++p;
	}

	p = screendata + (SCREEN_H - 1) * SCREEN_W * 2;

	for (size_t i{0}; i<SCREEN_W; ++i)
	{
		*p = ' ';
		++p;
		*p = ColorType::black | (ColorType::grey << 4);
		++p;
	}

	// Print the title
	GotoXY(0, 0);
	FGColor(ColorType::black);
	BGColor(ColorType::grey, false);

	PutChar(' ');
	Puts(title);
}

static void DrawHelpIndicator()
{
	char keybuf[10];
	ColorType fgcolor;
	int x;
	int y;

	GetKeyDescription(HELP_KEY, keybuf, sizeof(keybuf));

	GetMousePosition(&x, &y);

	if (y == 0 && x >= SCREEN_W - 9)
	{
		fgcolor = ColorType::grey;
		BGColor(ColorType::black, false);
	}
	else
	{
		fgcolor = ColorType::black;
		BGColor(ColorType::grey, false);
	}

	GotoXY(SCREEN_W - 9, 0);

	FGColor(ColorType::bright_white);
	DrawString(" ");
	DrawString(keybuf);

	FGColor(fgcolor);
	DrawString("=Help ");
}

void SetDesktopTitle(std::string& _title)
{
	//free(desktop_title);
	desktop_title = std::string(_title);
	//SetWindowTitle(title);
}

void DrawDesktop()
{
	Window* active_window;
	std::string title;

	InitClipArea();

	if (desktop_title.empty())
	{
		title = "";
	}
	else
	{
		title = desktop_title;
	}

	DrawDesktopBackground(title);

	active_window = GetActiveWindow();
	if (active_window && active_window->help_url)
	{
		DrawHelpIndicator();
	}

	for (size_t i{0}; i < num_windows; ++i)
	{
		DrawWindow(all_windows[i]);
	}

	UpdateScreen();
}

// Fallback function to handle key/mouse events that are not handled by the active window.
static void DesktopInputEvent(int c)
{
	int x;
	int y;

	switch (c)
	{
		case MOUSE_LEFT:
			GetMousePosition(&x, &y);

			// Clicking the top-right of the screen is equivalent to pressing the help key.
			if (y == 0 && x >= SCREEN_W - 9)
			{
				DesktopInputEvent(HELP_KEY);
			}
			break;

		case HELP_KEY:
			auto active_window{GetActiveWindow()};
			if (active_window)
			{
				OpenWindowHelpURL(active_window);
			}
			break;

		default:
			break;
	}
}

void DispatchEvents()
{
	int c;

	while ((c = GetChar()) > 0)
	{
		auto active_window{GetActiveWindow()};

		if (active_window && !active_window->KeyPress(c))
		{
			DesktopInputEvent(c);
		}
	}
}

void ExitMainLoop()
{
	main_loop_running = 0;
}

void DrawASCIITable()
{
	char buf[10];

	auto screendata{GetScreenData()};

	FGColor(ColorType::bright_white);
	BGColor(ColorType::black, false);

	for (size_t y{0}; y < 16; ++y)
	{
		for (size_t x{0}; x < 16; ++x)
		{
			int n = y * 16 + x;

			GotoXY(x * 5, y);
			snprintf(buf, sizeof(buf), "%02x	", n);
			Puts(buf);

			// Write the character directly to the screen memory buffer:
			screendata[(y * SCREEN_W + x * 5 + 3) * 2] = n;
		}
	}

	UpdateScreen();
}

void SetPeriodicCallback(TxtIdleCallback callback, void* user_data, unsigned period)
{
	periodic_callback = callback;
	periodic_callback_data = user_data;
	periodic_callback_period = period;
}

void GUIMainLoop()
{
	main_loop_running = 1;

	while (main_loop_running)
	{
		DispatchEvents();

		// After the last window is closed, exit the loop
		if (num_windows <= 0)
		{
			ExitMainLoop();
			continue;
		}

		DrawDesktop();
		//DrawASCIITable();

		if (!periodic_callback)
		{
			Sleep(0);
		}
		else
		{
			Sleep(periodic_callback_period);

			periodic_callback(periodic_callback_data);
		}
	}
}

} /* END NAMESPACE cudadoom::txt */
