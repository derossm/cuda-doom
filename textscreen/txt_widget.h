/**********************************************************************************************************************************************\
	Copyright(C) 2005-2014 Simon Howard

	This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License
	as published by the Free Software Foundation; either version 2 of the License, or (at your option) any later version.

	This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
\**********************************************************************************************************************************************/
#pragma once

#include <memory>
#include <string>
#include <functional>

#include "txt_common.h"

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

template<typename T, typename U = T>
class Widget
{
private:
	::std::weak_ptr<Widget<U>> parent{nullptr};
	//WidgetClass<T> widget_class;

	// These are set automatically when the window is drawn and should not be set manually.
	int64_t x{0};
	int64_t y{0};
	int width{0};
	int height{0};

	//CallbackTable callbackTable;

	AlignHorizontal align{};

	bool b_visible{false};
	bool b_focused{false};

public:
	using Type = T;

	//================================================================================================================================
	// ctors, dtors, etc
	Widget() //: widget_class{Selectable, CalculateSize, Draw, KeyPress, MousePress, SetLayout, SetFocus, Destroy}
	{

	}

	virtual ~Widget()
	{}

	//================================================================================================================================

	virtual inline bool Selectable() const noexcept
	{
		return true;
	}

	virtual inline void CalculateSize() noexcept
	{
		//size_calc(widget);
	}

	virtual inline void Draw() noexcept
	{
		SavedColors colors;

		// The drawing function might change the fg/bg colors, so make sure we restore them after it's done.
		SaveColors(&colors);

		// For convenience...
		//GotoXY(x, y);

		// Call drawer method
		//drawer();

		//RestoreColors(&colors);
	}

	virtual inline bool KeyPress(KeyEvent evt) noexcept
	{
		return false;//key_press(widget, key);
	}

	virtual inline bool MousePress(MouseEvent evt) noexcept
	{
		//mouse_press(x, y, b);
		return false;
	}

	virtual inline void SetLayout() noexcept
	{
		//layout();
	}

	virtual inline void SetFocus(bool _focus) noexcept
	{
		//layout();
	}

	virtual inline void Destroy() noexcept
	{}

	inline bool visible() const noexcept
	{
		return b_visible;
	}

	inline bool focused() const noexcept
	{
		return b_focused;
	}

	inline void setVisible() noexcept
	{
		b_visible = true;
	}

	inline void unsetVisible() noexcept
	{
		b_visible = false;
	}

	inline void setFocus() noexcept
	{
		b_focused = true;
	}

	inline void unsetFocus() noexcept
	{
		b_focused = false;
	}

	inline void toggleVisible() noexcept
	{
		b_visible = !b_visible;
	}

	inline void toggleFocus() noexcept
	{
		b_focused = !b_focused;
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

	inline void SignalConnect(::std::string&& signal, ::std::function<void(void*)>&& handle, UserData&& user) noexcept
	{
		//callback_table.connect(::std::move(signal), ::std::move(handle), ::std::move(user));
	}

	inline void EmitSignal(::std::string signal_name) noexcept
	{
		//auto table{callback_table};

		// Don't destroy the table while we're searching through it (one of the callbacks may destroy this window)
		//RefCallbackTable(table);

		// Search the table for all callbacks with this name and invoke the functions.
		//for (size_t i{0}; i < table.size(); ++i)
		{
			//if (!strcmp(table->callbacks[i].signal_name, signal_name))
			{
				//table->callbacks[i].func(widget, table->callbacks[i].user_data);
			}
		}

		// Finished using the table
		//UnrefCallbackTable(table);
	}

	inline void SetAlign(AlignHorizontal _align) noexcept
	{
		align = _align;
	}

	inline bool AlwaysSelectable() noexcept
	{
		return true;
	}

	inline bool NeverSelectable() noexcept
	{
		return false;
	}

	inline bool ContainsWidget() noexcept
	{
		return false;
	}

	inline bool HoveringOverWidget() noexcept
	{

	}

	inline void SetWidgetBG() noexcept
	{

	}

};

} // end namespace cudadoom::txt
