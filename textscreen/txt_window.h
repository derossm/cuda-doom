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

#include "doomkeys.h"

#include "txt_widget.h"
#include "txt_table.h"

#include "txt_main.h"
#include "txt_io.h"
#include "txt_gui.h"

#include "txt_desktop.h"
#include "txt_label.h"
#include "txt_separator.h"
#include "txt_window_action.h"

namespace cudadoom::txt
{

/**
 * A window.
 *
 * A window contains widgets, and may also be treated as a table (@ref txt_table_t) containing a single column.
 *
 * Windows can be created using @ref NewWindow and closed using @ref CloseWindow. When a window is closed, it emits the "closed" signal.
 *
 * In addition to the widgets within a window, windows also have a "tray" area at their bottom containing window action widgets.
 * These widgets allow keyboard shortcuts to trigger common actions. Each window has three slots for keyboard shortcuts. By default,
 * the left slot contains an action to close the window when the escape button is pressed, while the right slot contains an
 * action to activate the currently-selected widget.
 */

 // Callback function for window key presses
typedef int (*WindowKeyPress)(int key, void* user_data);
typedef int (*WindowMousePress)(int x, int y, int b, void* user_data);

class Window : public Table<Window>
{
public:
	// Base class: all windows are tables with one column.
	//Table table;

	std::string title;

	// Screen coordinates of the window
	AlignVertical verticalAlign;
	AlignHorizontal horizontalAlign;
	Vec2 coordinates;

	// Actions that appear in the box at the bottom of the window
	std::array<std::unique_ptr<Widget>, 3> actions;

	// Callback functions to invoke when keys/mouse buttons are pressed
	WindowKeyPress key_listener{nullptr};
	void* key_listener_data{nullptr};
	WindowMousePress mouse_listener{nullptr};
	void* mouse_listener_data{nullptr};

	// These are set automatically when the window is drawn
	int window_x;
	int window_y;
	unsigned window_w;
	unsigned window_h;

	// URL of a webpage with help about this window. If set, a help key indicator is shown while this window is active.
	std::string help_url;

	Window(std::string& _title) noexcept : title{_title}, coordinates{.x{SCREEN_W / 2}, .y{SCREEN_H / 2}},
		verticalAlign{AlignVertical::center}, horizontalAlign{AlignHorizontal::center},
		widget_class{Selectable, CalculateSize, Draw, KeyPress, MousePress, SetLayout, SetFocus, Destroy}
	{
		AddWidget(Separator(nullptr));

		AddDesktopWindow(this);

		// Default actions
		SetWindowAction(AlignHorizontal::left, NewWindowEscapeAction(this));
		SetWindowAction(AlignHorizontal::right, NewWindowSelectAction(this));
	}

	bool Selectable() noexcept override final
	{
		return true;
	}

	void CalculateSize() noexcept override final
	{}

	void SetWindowAction(AlignHorizontal position, Widget& action) noexcept
	{
		if (actions[size_t(position)])
		{
			DestroyWidget(actions[size_t(position)]);
		}

		actions[size_t(position)] = std::make_unique(action);

		// maintain parent pointer
		//action.parent = &widget;
		// FIXME
	}

	void CloseWindow() noexcept
	{
		EmitSignal("closed");
		RemoveDesktopWindow();
	}

	void CalcWindowPosition() noexcept
	{
		switch (horizontalAlign)
		{
		case AlignHorizontal::left:
			window_x = coordinates.x;
			break;
		case AlignHorizontal::center:
			window_x = coordinates.x - (window_w / 2);
			break;
		case AlignHorizontal::right:
			window_x = coordinates.x - (window_w - 1);
			break;
		}

		switch (verticalAlign)
		{
		case AlignVertical::top:
			window_y = coordinates.y;
			break;
		case AlignVertical::center:
			window_y = coordinates.y - (window_h / 2);
			break;
		case AlignVertical::bottom:
			window_y = coordinates.y - (window_h - 1);
			break;
		}
	}

	void LayoutActionArea() noexcept
	{
		Widget* widget;

		// We need to calculate the available horizontal space for the center action widget, so that we can center it within it.
		// To start with, we have the entire action area available.
		auto space_available{window_w};
		auto space_left_offset{0};

		// Left action
		if (actions[std::size_t(AlignHorizontal::left)])
		{
			widget = actions[std::size_t(AlignHorizontal::left)];

			CalcWidgetSize(widget);

			widget->x = window_x + 1;
			widget->y = window_y + window_h - widget->height - 1;

			// Adjust available space:
			space_available -= widget->width;
			space_left_offset += widget->width;

			LayoutWidget(widget);
		}

		// Draw the right action
		if (actions[std::size_t(AlignHorizontal::right)])
		{
			widget = actions[std::size_t(AlignHorizontal::right)];

			CalcWidgetSize(widget);

			widget->x = window_x + window_w - 1 - widget->width;
			widget->y = window_y + window_h - widget->height - 1;

			// Adjust available space:
			space_available -= widget->width;

			LayoutWidget(widget);
		}

		// Draw the center action
		if (actions[std::size_t(AlignHorizontal::center)])
		{
			widget = actions[std::size_t(AlignHorizontal::center)];

			CalcWidgetSize(widget);

			// The left and right widgets have left a space sandwiched between them. Center this widget within that space.
			widget->x = window_x + space_left_offset + (space_available - widget->width) / 2;
			widget->y = window_y + window_h - widget->height - 1;

			LayoutWidget(widget);
		}
	}

	void DrawActionArea() noexcept
	{
		for (size_t i{0}; i < 3; ++i)
		{
			if (actions[i])
			{
				DrawWidget(actions[i]);
			}
		}
	}

	void CalcActionAreaSize(unsigned* w, unsigned* h) noexcept
	{
		*w = 0;
		*h = 0;

		// Calculate the width of all the action widgets and use this to create an overall min. width of the action area
		for (size_t i{0}; i < 3; ++i)
		{
			auto widget = (Widget*)actions[i];

			if (widget)
			{
				CalcWidgetSize(widget);
				*w += widget->width;

				if (widget->height > *h)
				{
					*h = widget->height;
				}
			}
		}
	}

	// Sets size and position of all widgets in a window
	void SetLayout() noexcept override final
	{
		auto widget = (Widget*)window;

		// Calculate size of table
		CalculateSize();

		// Widgets area: add one character of padding on each side
		auto widget_w = widget->width + 2;

		unsigned actionarea_w;
		unsigned actionarea_h;

		// Calculate the size of the action area; Make window wide enough to action area
		CalcActionAreaSize(&actionarea_w, &actionarea_h);

		if (actionarea_w > widget_w)
		{
			widget_w = actionarea_w;
		}

		// Set the window size based on widgets_w
		window_w = widget_w + 2;
		window_h = widget->height + 1;

		// If the window has a title, add an extra two lines
		if (title)
		{
			window_h += 2;
		}

		// If the window has an action area, add extra lines
		if (actionarea_h > 0)
		{
			window_h += actionarea_h + 1;
		}

		// Use the x,y position as the centerpoint and find the location to draw the window.

		CalcWindowPosition();

		// Set the table size and position
		widget->width = widget_w - 2;
		// widgets->h // already set
		widget->x = window_x + 2;
		widget->y = window_y;

		if (title)
		{
			widget->y += 2;
		}

		// Layout the table and action area
		LayoutActionArea();
		//SetLayout();
	}

	void Draw() noexcept override final
	{
		LayoutWindow();

		if (table.widget.focused())
		{
			BGColor(ACTIVE_WINDOW_BACKGROUND, false);
		}
		else
		{
			BGColor(INACTIVE_WINDOW_BACKGROUND, false);
		}

		FGColor(ColorType::bright_white);

		// Draw the window
		DrawWindowFrame(title, window_x, window_y, window_w, window_h);

		// Draw all widgets
		DrawWidget();

		// Draw an action area, if we have one
		auto widget = (Widget*)window;

		if (widget->y + widget->height < window_y + window_h - 1)
		{
			// Separator for action area
			DrawSeparator(window_x, widget->y + widget->height, window_w);

			// Action area at the window bottom
			DrawActionArea();
		}
	}

	void SetWindowPosition(AlignHorizontal horiz_align, AlignVertical vert_align, int x, int y) noexcept
	{
		verticalAlign = vert_align;
		horizontalAlign = horiz_align;
		coordinates.x = x;
		coordinates.y = y;
	}

	bool MousePress(MouseEvent evt) override final noexcept
	{
		auto [button, x, y] = evt;

		// Lay out the window, set positions and sizes of all widgets
		SetLayout();

		// Get the current mouse position
		GetMousePosition(&x, &y);

		// Try the mouse button listener; This happens whether it is in the window range or not
		if (mouse_listener)
		{
			// Mouse listener can eat button presses
			if (mouse_listener(x, y, b, mouse_listener_data))
			{
				return 1;
			}
		}

		{
			// Is it within the table range?
			auto widget = (Widget*)window;

			if (x >= widget->x && x < (int)(widget->x + widget->width) && y >= widget->y && y < (int)(widget->y + widget->height))
			{
				WidgetMousePress(window, x, y, b);
				return 1;
			}
		}

		// Was one of the action area buttons pressed?
		for (size_t i{0}; i < 3; ++i)
		{
			auto widget = actions[i];

			if (widget && x >= widget->x && x < (int)(widget->x + widget->width) && y >= widget->y && y < (int)(widget->y + widget->height))
			{
				// Main table temporarily loses focus when action area button is clicked. This way, any active input boxes that depend
				// on having focus will save their values before the action is performed.
				auto was_focused = table.widget.focused();
				SetWidgetFocus(0);
				SetWidgetFocus(was_focused);

				// Pass through mouse press.
				WidgetMousePress(widget, x, y, b);
				return 1;
			}
		}

		return 0;
	}

	bool KeyPress(KeyType c) override final noexcept
	{
		// Is this a mouse button ?
		if (c >= MOUSE_BASE && c < MOUSE_BASE + MAX_MOUSE_BUTTONS)
		{
			return MouseButtonPress(c);
		}

		// Try the window key spy
		if (key_listener)
		{
			// key listener can eat keys
			if (key_listener(c, key_listener_data))
			{
				return 1;
			}
		}

		// Send to the currently selected widget:
		if (WidgetKeyPress(c))
		{
			return 1;
		}

		// Try all of the action buttons
		for (size_t i{0}; i < 3; ++i)
		{
			if (actions[i] && WidgetKeyPress(actions[i], c))
			{
				return 1;
			}
		}

		return 0;
	}

	void SetKeyListener(WindowKeyPress _key_listener, void* _user_data) noexcept
	{
		key_listener = _key_listener;
		key_listener_data = _user_data;
	}

	void SetMouseListener(WindowMousePress _mouse_listener, void* _user_data) noexcept
	{
		mouse_listener = _mouse_listener;
		mouse_listener_data = _user_data;
	}

	void SetFocus(bool _focus) noexcept override final
	{
		SetWidgetFocus(focused);
	}

	void SetWindowHelpURL(std::string&& _help_url) noexcept
	{
		help_url(std::forward<std::string>(_help_url));
	}

#ifdef _WIN32
	void OpenURL(std::string& url) noexcept
	{
		ShellExecute(NULL, "open", url, NULL, NULL, SW_SHOWNORMAL);
	}
#else
	void OpenURL(std::string url) noexcept
	{
		size_t cmd_len = strlen(url) + 30;
		std::string cmd = static_cast<decltype(cmd)>(malloc(cmd_len));

#if defined(__MACOSX__)
		snprintf(cmd, cmd_len, "open \"%s\"", url);
#else
		// The Unix situation sucks as usual, but the closest thing to a standard that exists is the xdg-utils package.
		if (system("xdg-open --version 2>/dev/null") != 0)
		{
			fprintf(stderr, "xdg-utils is not installed. Can't open this URL:\n%s\n", url);
			free(cmd);
			return;
		}

		snprintf(cmd, cmd_len, "xdg-open \"%s\"", url);
#endif

		auto retval = system(cmd);
		free(cmd);
		if (retval != 0)
		{
			fprintf(stderr, "OpenURL: error executing '%s'; return code %d\n", cmd, retval);
		}
	}
#endif /* #ifdef _WIN32 */

	void OpenWindowHelpURL() noexcept
	{
		if (help_url)
		{
			OpenURL(help_url);
		}
	}

	void MessageBox(std::string& title, std::string& message, ...) noexcept
	{
		Window* window;
		char buf[256];
		va_list args;

		va_start(args, message);
		vsnprintf(buf, sizeof(buf), message, args);
		va_end(args);

		window = NewWindow(title);
		AddWidget(window, NewLabel(buf));

		SetWindowAction(AlignHorizontal::left, nullptr);
		SetWindowAction(AlignHorizontal::center, NewWindowEscapeAction(window));
		SetWindowAction(AlignHorizontal::right, nullptr);
	}
};

/**
 * Open a new window.
 *
 * @param title		Title to display in the titlebar of the new window (UTF-8 format).
 * @return			Pointer to a new @ref Window structure representing the new window.
 */
Window* NewWindow(std::string title);

/**
 * Close a window.
 *
 * @param window		Tine window to close.
 */
void CloseWindow(Window* window);

/**
 * Set the position of a window on the screen.
 *
 * The position is specified as a pair of x, y, coordinates on the
 * screen. These specify the position of a particular point on the
 * window. The following are some examples:
 *
 * <code>
 *	// Centered on the screen:
 *
 *	SetWindowPosition(window, CENTER, CENTER, SCREEN_W / 2, SCREEN_H / 2);
 *
 *	// Horizontally centered, with top of the window on line 6:
 *
 *	SetWindowPosition(window, CENTER, VERT_TOP, SCREEN_W / 2, 6);
 *
 *	// Top-left of window at 20, 6:
 *
 *	SetWindowPosition(window, HORIZ_LEFT, VERT_TOP, 20, 6);
 * </code>
 *
 * @param window		The window.
 * @param horiz_align	Horizontal location on the window for the X position.
 * @param vert_align	Vertical location on the window for the Y position.
 * @param x				X coordinate (horizontal axis) for window position.
 * @param y				Y coordinate (vertical axis) for window position.
 */
void SetWindowPosition(Window* window, AlignHorizontal horiz_align, AlignVertical vert_align, int x, int y);

/**
 * Set a window action for a given window.
 *
 * Each window can have up to three window actions, which provide
 * keyboard shortcuts that can be used within a given window.
 *
 * @param window	The window.
 * @param position	The window action slot to set (left, center or right).
 * @param action	The window action widget. If this is NULL, any
 *					current window action in the given slot is removed.
 */
void SetWindowAction(Window* window, AlignHorizontal position, UNCAST_ARG(action));

/**
 * Set a callback function to be invoked whenever a key is pressed within
 * a window.
 *
 * @param window		The window.
 * @param key_listener	Callback function.
 * @param user_data		User-specified pointer to pass to the callback function.
 */
void SetKeyListener(Window* window, TxtWindowKeyPress key_listener, void* user_data);

/**
 * Set a callback function to be invoked whenever a mouse button is pressed
 * within a window.
 *
 * @param window			The window.
 * @param mouse_listener	Callback function.
 * @param user_data			User-specified pointer to pass to the callback function.
 */
void SetMouseListener(Window* window, TxtWindowMousePress mouse_listener, void* user_data);

/**
 * Open a window displaying a message.
 *
 * @param title			Title of the window (UTF-8 format).
 * @param message		The message to display in the window (UTF-8 format).
 * @return				The new window.
 */
Window* MessageBox(std::string title, std::string message, ...);

/**
 * Set the help URL for the given window.
 *
 * @param window		The window.
 * @param help_url		String containing URL of the help page for this
 *						window, or NULL to set no help for this window.
 */
void SetWindowHelpURL(Window* window, std::string help_url);

/**
 * Open the help URL for the given window, if one is set.
 *
 * @param window			The window.
 */
void OpenWindowHelpURL(Window* window);

} /* END NAMESPACE cudadoom::txt */
