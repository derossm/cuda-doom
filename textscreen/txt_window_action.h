/**********************************************************************************************************************************************\
	Copyright(C) 2005-2014 Simon Howard

	This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License
	as published by the Free Software Foundation; either version 2 of the License, or (at your option) any later version.

	This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
\**********************************************************************************************************************************************/
#pragma once
// DECOUPLE
//#include "../derma/common.h"

#include <string>

#include "txt_common.h"
#include "txt_widget.h"

//#include "txt_main.h"
//#include "txt_utf8.h"
//#include "txt_io.h"
//#include "txt_gui.h"

namespace cudadoom::txt
{

/**
 * Window action widget.
 *
 * A window action is attached to a window and corresponds to a keyboard shortcut that is active within that window.
 * When the key is pressed, the action is triggered. When a window action is triggered, the "pressed" signal is emitted.
 */
class WindowAction : public Widget<WindowAction>
{
	int key;
	std::string label;

public:

	WindowAction(int _key, std::string _label) //: widget_class<WindowAction>{}, key{_key}, label{std::string(_label)},
		//widget_class{Selectable, CalculateSize, Draw, KeyPress, MousePress, SetLayout, SetFocus, Destroy}
	{
	}

	inline bool Selectable() const noexcept override final
	{
		return false;
	}

	inline void CalculateSize() noexcept override final
	{
		//char buf[10];

		//GetKeyDescription(key, buf, sizeof(buf));

		// Width is label length, plus key description length, plus '=' and two surrounding spaces.
		//width = label.length() + UTF8_Strlen(buf) + 3;
		//height = 1;
	}

	inline void Draw() noexcept override final
	{
/*
		char buf[10];

		GetKeyDescription(key, buf, sizeof(buf));

		auto hovering{HoveringOverWidget()};
		SetWidgetBG();

		DrawString(" ");
		FGColor(hovering ? ColorType::bright_white : ColorType::bright_green);
		DrawString(buf);
		FGColor(ColorType::bright_cyan);
		DrawString("=");

		FGColor(ColorType::bright_white);
		DrawString(label);
		DrawString(" ");
/**/
	}

	inline bool KeyPress(KeyEvent _key) noexcept override final
	{
/*
		if (tolower(_key) == tolower(key))
		{
			EmitSignal("pressed");
			return true;
		}
/**/
		return false;
	}

	inline bool MousePress(MouseEvent evt) noexcept override final
	{
/*
		// Simulate a press of the key
		if (evt.button == MOUSE_LEFT)
		{
			return KeyPress(KEY_ENTER);
		}
/**/
		return false;
	}

	inline void SetLayout() noexcept override final
	{}

	inline void SetFocus(bool _focus) noexcept override final
	{}

	inline void Destroy() noexcept override final
	{}

	// FIXME
	inline void WindowCloseCallback(Window* window) noexcept
	{
		//window->CloseWindow();
	}

	inline void WindowSelectCallback(Window* window) noexcept
	{
		//window->WidgetKeyPress(KEY_ENTER);
	}
};
/*
// An action with the name "close" the closes the window
WindowAction* NewWindowEscapeAction(Window* window)
{
	auto action{WindowAction(KEY_ESCAPE, "Close")};
	SignalConnect(action, "pressed", WindowCloseCallback, window);

	return action;
}

// Exactly the same as the above, but the button is named "abort"
WindowAction* NewWindowAbortAction(Window* window)
{
	auto action{WindowAction(KEY_ESCAPE, "Abort")};
	SignalConnect(action, "pressed", WindowCloseCallback, window);

	return action;
}

WindowAction* NewWindowSelectAction(Window* window)
{
	auto action{WindowAction(KEY_ENTER, "Select")};
	SignalConnect(action, "pressed", WindowSelectCallback, window);

	return action;
}
/**/
} // END NAMESPACE cudadoom::txt

/**
 * Create a new window action.
 *
 * @param key			The keyboard key that triggers this action.
 * @param label			Label to display for this action in the tray at the bottom of the window (UTF-8 format).
 * @return				Pointer to the new window action widget.
 */
 //WindowAction* NewWindowAction(int key, std::string label);

 /**
  * Create a new window action that closes the window when the escape key is pressed. The label "Close" is used.
  *
  * @param window		The window to close.
  * @return				Pointer to the new window action widget.
  */
  //WindowAction* NewWindowEscapeAction(Window* window);

  /**
   * Create a new window action that closes the window when the escape key is pressed. The label "Abort" is used.
   *
   * @param window		The window to close.
   * @return				Pointer to the new window action widget.
   */
   //WindowAction* NewWindowAbortAction(Window* window);

   /**
	* Create a new "select" window action. This does not really do anything, but reminds the user that "enter" can be pressed to
	* activate the currently-selected widget.
	*
	* @param window		The window.
	* @return				Pointer to the new window action widget.
	*/
	//WindowAction* NewWindowSelectAction(Window* window);