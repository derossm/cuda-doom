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

#include "txt_enums.h"
#include "txt_defines.h"
#include "txt_io.h"
#include "txt_gui.h"

#include "txt_desktop.h"

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

template<typename T>
class Widget
{
private:
	Widget* parent{nullptr};
	WidgetClass<T> widget_class;

	// These are set automatically when the window is drawn and should not be set manually.
	int64_t x{0};
	int64_t y{0};
	int width{0};
	int height{0};

	CallbackTable callbackTable;

	AlignHorizontal align{};

	bool b_visible{false};
	bool b_focused{false};

public:
	using Type = T;

	//================================================================================================================================
	// ctors, dtors, etc
	Widget() : widget_class{Selectable, CalculateSize, Draw, KeyPress, MousePress, SetLayout, SetFocus, Destroy}
	{

	}

	virtual ~Widget()
	{}

	//================================================================================================================================

	virtual inline bool Selectable() const noexcept
	{
		return true;
	}

	virtual inline void CalculateSize() const noexcept
	{
		//size_calc(widget);
	}

	virtual inline void Draw() const noexcept
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

	virtual inline bool KeyPress(KeyEvent evt) const noexcept
	{
		return 0;//key_press(widget, key);
	}

	virtual inline bool MousePress(MouseEvent evt) const noexcept
	{
		//mouse_press(x, y, b);
	}

	virtual inline void SetLayout() const noexcept
	{
		//layout();
	}

	virtual inline void SetFocus(bool _focus) const noexcept
	{
		//layout();
	}

	virtual inline void Destroy() const noexcept
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

	inline void SignalConnect(std::string&& signal, std::function<void(void*)>&& handle, UserData&& user) noexcept
	{
		callback_table.connect(std::move(signal), std::move(handle), std::move(user));
	}

	inline void EmitSignal(std::string& signal_name) noexcept
	{
		//auto table{callback_table};

		// Don't destroy the table while we're searching through it (one of the callbacks may destroy this window)
		//RefCallbackTable(table);

		// Search the table for all callbacks with this name and invoke the functions.
		//for (size_t i{0}; i < table.size(); ++i)
		//{
			//if (!strcmp(table->callbacks[i].signal_name, signal_name))
			//{
				//table->callbacks[i].func(widget, table->callbacks[i].user_data);
			//}
		//}

		// Finished using the table
		//UnrefCallbackTable(table);
	}

	// void SetWidgetFocus(bool _focused)
	// {
	// 	if (!focused() && _focused)
	// 	{
	// 		setFocus();
	// 	}
	// 	else if(focused() && !_focused)
	// 	{
	// 		unsetFocus();
	// 	}
	// }

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

	inline bool ContainsWidget(Widget* needle) noexcept
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

	inline bool HoveringOverWidget() noexcept
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

	inline void SetWidgetBG() noexcept
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
 //void SignalConnect(std::string signal_name, WidgetSignalFunc func, void* user_data);

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
