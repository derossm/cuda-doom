/**********************************************************************************************************************************************\
	Copyright(C) 2005-2014 Simon Howard

	This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License
	as published by the Free Software Foundation; either version 2 of the License, or (at your option) any later version.

	This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
\**********************************************************************************************************************************************/
#pragma once

#include "txt_common.h"

#include "txt_widget.h"

namespace cudadoom::txt
{

/**
 * Window action widget.
 *
 * A window action is attached to a window and corresponds to a keyboard shortcut that is active within that window.
 * When the key is pressed, the action is triggered. When a window action is triggered, the "pressed" signal is emitted.
 */

class WindowAction : public WidgetBase<WindowAction>
{
	Keys key;
	::std::string label;

public:

	WindowAction(Keys key, ::std::string label) : key{key}, label{::std::string(label)} {}
/*
	// An action with the name "close" the closes the window
	WindowAction* NewWindowEscapeAction(Window* window)
	{
		auto action{WindowAction(Keys::ESCAPE, "Close")};
		SignalConnect(action, "pressed", WindowCloseCallback, window);

		return action;
	}

	// Exactly the same as the above, but the button is named "abort"
	WindowAction* NewWindowAbortAction(Window* window)
	{
		auto action{WindowAction(Keys::ESCAPE, "Abort")};
		SignalConnect(action, "pressed", WindowCloseCallback, window);

		return action;
	}

	WindowAction* NewWindowSelectAction(Window* window)
	{
		auto action{WindowAction(Keys::ENTER, "Select")};
		SignalConnect(action, "pressed", WindowSelectCallback, window);

		return action;
	}
/**/

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

	inline bool KeyPress(Keys _key) noexcept override final
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

	inline bool MousePress(MouseEvent _evt) noexcept override final
	{
/*
		// Simulate a press of the key
		if (_evt.button == MOUSE_LEFT)
		{
			return KeyPress(Keys::ENTER);
		}
/**/
		return false;
	}

	inline void SetLayout() noexcept override final {}

	inline void SetFocus(bool _focus) noexcept override final {}

	inline void Destroy() noexcept override final {}

	// FIXME
	inline void WindowCloseCallback(Window* window) noexcept
	{
		//window->CloseWindow();
	}

	inline void WindowSelectCallback(Window* window) noexcept
	{
		//window->WidgetKeyPress(Keys::ENTER);
	}
};

} // end namespace cudadoom::txt
