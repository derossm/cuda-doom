/**********************************************************************************************************************************************\
	Copyright(C) 2005-2014 Simon Howard

	This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License
	as published by the Free Software Foundation; either version 2 of the License, or (at your option) any later version.

	This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
\**********************************************************************************************************************************************/
#pragma once

#include "../derma/common.h"

#include "txt_enums.h"
#include "txt_defines.h"
#include "txt_desktop.h"
#include "txt_io.h"
#include "txt_gui.h"

namespace cudadoom::txt
{

/**
 * A GUI widget.
 *
 * A widget is an individual component of a GUI. Various different widget types exist.
 *
 * Widgets may emit signals. The types of signal emitted by a widget depend on the type of the widget.
 * It is possible to be notified when a signal occurs using the @ref SignalConnect function.
 */

using UserData = void*;

class CallbackEntry
{
private:
	UserData user;
	std::function<void(UserData)> handle;
	std::string signal;

	CallbackEntry() = delete;

	// do not copy
	CallbackEntry(const CallbackEntry& rhs) = delete;
	CallbackEntry& operator=(const CallbackEntry& rhs) = delete;

public:
	CallbackEntry(std::string _signal, std::function<void(UserData)> _handle, UserData _user) noexcept
		: signal(std::move(_signal)), handle(std::move(_handle)), user(std::move(_user))
	{
	}

	CallbackEntry(CallbackEntry&& rhs) noexcept : signal(std::move(rhs.signal)), handle(std::move(rhs.handle)), user(std::move(rhs.user))
	{
	}

	std::function<void(UserData)> findByName(std::string& _signal)
	{
		if (signal.compare(_signal))
		{
			return handle;
		}
		else
		{
			return nullptr;
		}
	}

	CallbackEntry& operator=(CallbackEntry&& rhs) noexcept
	{
		signal = std::move(rhs.signal);
		handle = std::move(rhs.handle);
		user = std::move(rhs.user);
		return *this;
	}

	~CallbackEntry() = default;

};

class CallbackTable
{
private:
	std::vector<CallbackEntry> callbacks;

	CallbackTable(CallbackTable&&) = delete;
	CallbackTable& operator=(CallbackTable&&) = delete;

	CallbackTable(const CallbackTable&) = delete;
	CallbackTable& operator=(const CallbackTable&) = delete;

public:
	// reserve callbacks in contiguous memory
	CallbackTable() noexcept : callbacks(32)
	{
	}

	~CallbackTable() = default;

	void connect(std::string&& signal, std::function<void(UserData)>&& handle, UserData&& user)
	{
		callbacks.emplace_back(CallbackEntry(std::move(signal), std::move(handle), std::move(user)));
	}

	std::function<void(UserData)> get(std::string& signal)
	{
		for(auto& iter: callbacks)
		{
			if (auto ptr = iter.findByName(signal); ptr)
			{
				return ptr;
			}
		}

		return nullptr;
	}

	size_t size()
	{
		return callbacks.size();
	}
};

class WidgetClass
{
public:
	std::function<int(void)> SelectableWidget{};
	std::function<void(void)> CalcWidgetSize{};
	std::function<void(void)> DrawWidget{};
	std::function<int(int)> WidgetKeyPress{};
	std::function<void(void)> DestroyWidget{};
	std::function<int(int, int, int)> WidgetMousePress{};
	std::function<void(void)> LayoutWidget{};
	std::function<void(bool)> SetWidgetFocus{};
};

class Widget
{
	// TEMP PUBLIC UNTIL ALL DEPENDENTS CONVERTED TODO UPDATE THIS
public:
	Widget* parent{nullptr};
	WidgetClass* widget_class{nullptr};

	// These are set automatically when the window is drawn and should not be set manually.
	int64_t x{0};
	int64_t y{0};
	int64_t width{0};
	int64_t height{0};

	CallbackTable callback_table;

	AlignHorizontal align{};

	bool _visible{false};
	bool _focused{false};

	Widget()
	{
		
	}

	~Widget()
	{
	}

public:
	inline bool visible() const noexcept
	{
		return _visible;
	}

	inline bool focused() const noexcept
	{
		return _focused;
	}

	inline void setVisible() noexcept
	{
		_visible = true;
	}

	inline void unsetVisible() noexcept
	{
		_visible = false;
	}

	inline void setFocus() noexcept
	{
		_focused = true;
	}

	inline void unsetFocus() noexcept
	{
		_focused = false;
	}

	inline void toggleVisible() noexcept
	{
		_visible = !_visible;
	}

	inline void toggleFocus() noexcept
	{
		_focused = !_focused;
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

	inline void CalcWidgetSize()
	{
		size_calc(widget);
	}

	void SignalConnect(std::string&& signal, std::function<void(void*)>&& handle, UserData&& user)
	{
		callback_table.connect(std::move(signal), std::move(handle), std::move(user));
	}

	void EmitSignal(std::string& signal_name)
	{
		auto table{callback_table};

		// Don't destroy the table while we're searching through it (one of the callbacks may destroy this window)
		//RefCallbackTable(table);

		// Search the table for all callbacks with this name and invoke the functions.
		for (size_t i{0}; i < table.size(); ++i)
		{
			//if (!strcmp(table->callbacks[i].signal_name, signal_name))
			//{
				//table->callbacks[i].func(widget, table->callbacks[i].user_data);
			//}
		}

		// Finished using the table
		//UnrefCallbackTable(table);
	}

	void DrawWidget()
	{
		SavedColors colors;

		// The drawing function might change the fg/bg colors, so make sure we restore them after it's done.
		SaveColors(&colors);

		// For convenience...
		GotoXY(x, y);

		// Call drawer method
		//drawer();

		RestoreColors(&colors);
	}

	void DestroyWidget()
	{
	}

	int KeyPress(int key)
	{
		return 0;//key_press(widget, key);
	}

	void SetWidgetFocus(bool _focused)
	{
		if (!focused() && _focused)
		{
			setFocus();
		}
		else if(focused() && !_focused)
		{
			unsetFocus();
		}
	}

	void SetWidgetAlign(AlignHorizontal horiz_align)
	{
		align = horiz_align;
	}

	void MousePress(int x, int y, int b)
	{
		//mouse_press(x, y, b);
	}

	void LayoutWidget()
	{
		//layout();
	}

	bool AlwaysSelectable()
	{
		return true;
	}

	bool NeverSelectable()
	{
		return false;
	}

	int SelectableWidget()
	{
		return 0;//selectable(widget);
	}

	bool ContainsWidget(Widget* needle)
	{
		while (needle)
		{
			if (needle == this)
			{
				return true;
			}

			needle = needle->parent;
		}

		return false;
	}

	bool HoveringOverWidget()
	{
		int _x;
		int _y;

		// We can only be hovering over widgets in the active window.
		auto active_window = (Widget*)GetActiveWindow();

		if (!active_window || !ContainsWidget(active_window))
		{
			return false;
		}

		// Is the mouse cursor within the bounds of the widget?
		GetMousePosition(&_x, &_y);

		return (_x >= x && _x < x + width && _y >= y && _y < y + height);
	}

	void SetWidgetBG()
	{
		if (focused())
		{
			BGColor(ColorType::grey, false);
		}
		else if (HoveringOverWidget())
		{
			BGColor(HOVER_BACKGROUND, false);
		}
		else
		{
			// Use normal window background.
		}
	}
};

/**
 * Set a callback function to be invoked when a signal occurs.
 *
 * @param widget		The widget to watch.
 * @param signal_name	The signal to watch.
 * @param func			The callback function to invoke.
 * @param user_data	User-specified pointer to pass to the callback function.
 */
//void SignalConnect(const char* signal_name, WidgetSignalFunc func, void* user_data);

/**
 * Set the policy for how a widget should be aligned within a table.
 * By default, widgets are aligned to the left of the column.
 *
 * @param widget		The widget.
 * @param horiz_align	The alignment to use.
 */
//void SetWidgetAlign(AlignHorizontal horiz_align);

/**
 * Query whether a widget is selectable with the cursor.
 *
 * @param widget		The widget.
 * @return				Non-zero if the widget is selectable.
 */
//bool SelectableWidget();

/**
 * Query whether the mouse is hovering over the specified widget.
 *
 * @param widget		The widget.
 * @return				Non-zero if the mouse cursor is over the widget.
 */
//bool HoveringOverWidget();

/**
 * Set the background to draw the specified widget, depending on
 * whether it is selected and the mouse is hovering over it.
 *
 * @param widget		The widget.
 */
//void SetWidgetBG();

/**
 * Query whether the specified widget is contained within another
 * widget.
 *
 * @param haystack		The widget that might contain needle.
 * @param needle		The widget being queried.
 */
//bool ContainsWidget(Widget&& needle);

} /* END NAMESPACE cudadoom::txt */
