/**********************************************************************************************************************************************\
	Copyright(C) 2005-2014 Simon Howard

	This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License
	as published by the Free Software Foundation; either version 2 of the License, or (at your option) any later version.

	This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
\**********************************************************************************************************************************************/
//#pragma once

//#include "../derma/common.h"

export module txt_inputbox;

import std.core;

import textscreen;

import txt_main;
import txt_widget;
import txt_window;
import txt_utf8;
import txt_io;
import txt_gui;

export
{

namespace cudadoom::txt
{
/**
 * Input box widget.
 *
 * An input box is a widget that displays a value, which can be
 * selected to enter a new value.
 *
 * Input box widgets can be of an integer or string type.
 */

template<typename T, typename U>
class StrongType
{
	T value;
public:
	explicit StrongType(const T& _val) noexcept : value{_val} {}
	//T get() { return value; }
};

struct KeyType{};
using Key = StrongType<int, KeyType>;

class DepType
{
};

template<typename T>
class InputBox : public Widget
{
	//T _val;
	std::string buffer;
	bool _editing{false};

	WidgetClass txt_inputbox_class
	{
		AlwaysSelectable,
		InputBoxSizeCalc,
		InputBoxDrawer,
		InputBoxKeyPress,
		InputBoxDestructor,
		InputBoxMousePress,
		nullptr,
		InputBoxFocused,
	};

public:
	using type = T;

	InputBox(T _val = nullptr, size_t widthIn = 0) noexcept :	buffer{std::string(_val)};
																widget_class{&txt_inputbox_class},
																callback_table{NewCallbackTable()},
																focused{false},
																visible{true},
																align{HORIZ_LEFT},
																width{widthIn}
	{
	/* 	if (_val)
		{
			buffer = std::string(valueIn);
		}
		else
		{
			buffer = std::string("");
		} */
	}

	void set(T _val) noexcept
	{
		if (_val)
		{
			buffer = std::string(_val);
		}
	}

	std::string get() const noexcept
	{
		return buffer;
	}

	bool editing() const noexcept
	{
		return _editing;
	}

	void toggleEditing() noexcept
	{
		_editing = !_editing;
	}

	void startEditing() noexcept
	{
		_editing = true;
	}

	void stopEditing() noexcept
	{
		_editing = false;
	}

	void inputBoxSizeCalc(UNCAST_ARG(inputbox)) noexcept
	{
		CAST_ARG(txt_inputbox_t, inputbox);

		// Enough space for the box + cursor

		inputbox->widget.w = inputbox->size + 1;
		inputbox->widget.h = 1;
	}

	void inputBoxDrawer(UNCAST_ARG(inputbox)) noexcept
	{
		CAST_ARG(txt_inputbox_t, inputbox);

		auto focused = inputbox->widget.focused();
		auto w = inputbox->widget.width;

		// Select the background color based on whether we are currently editing, and if not, whether the widget is focused.
		if (inputbox->editing() && focused)
		{
			BGColor(ColorType::black, false);
		}
		else
		{
			SetWidgetBG(inputbox);
		}

		if (!inputbox->editing())
		{
			// If not editing, use the current value from inputbox->value.
			SetBufferFromValue(inputbox);
		}

		// If string size exceeds the widget's width, show only the end.
		auto chars{[&]()
		{
			if (UTF8_Strlen(inputbox->buffer) > w - 1)
			{
				auto len{UTF8_Strlen(inputbox->buffer)};
				static_assert(w > 0);
				static_assert(len >= w);
				DrawCodePageString("\xae");
				DrawString(UTF8_SkipChars(inputbox->buffer, (len - w) + 2));
				return = w - 1;
			}
			else
			{
				DrawString(inputbox->buffer);
				return UTF8_Strlen(inputbox->buffer);
			}
		}()};

		if (chars < w && inputbox->editing && focused)
		{
			BGColor(ColorType::black, true);
			DrawString("_");
			++chars;
		}

		for (auto i{chars}; i < w; ++i)
		{
			DrawString(" ");
		}
	}

	void backspace(txt_inputbox_t* inputbox) noexcept
	{
		unsigned len;
		char* p;

		len = UTF8_Strlen(inputbox->buffer);

		if (len > 0)
		{
			p = UTF8_SkipChars(inputbox->buffer, len - 1);
			*p = '\0';
		}
	}

	void addCharacter(txt_inputbox_t* inputbox, int key) noexcept
	{
		char* end;
		char* p;

		if (UTF8_Strlen(inputbox->buffer) < inputbox->size)
		{
			// Add character to the buffer
			end = inputbox->buffer + strlen(inputbox->buffer);
			p = EncodeUTF8(end, key);
			*p = '\0';
		}
	}

	int keyPress(UNCAST_ARG(inputbox), int key) noexcept
	{
		CAST_ARG(txt_inputbox_t, inputbox);
		unsigned c;

		if (!inputbox->editing)
		{
			if (key == KEY_ENTER)
			{
				StartEditing(inputbox);
				return 1;
			}

			// Backspace or delete erases the contents of the box.
			if ((key == KEY_DEL || key == KEY_BACKSPACE) && inputbox->widget.widget_class == &txt_inputbox_class)
			{
				free(*((T)inputbox->value));
				*((T)inputbox->value) = strdup("");
			}

			return 0;
		}

		if (key == KEY_ENTER)
		{
			FinishEditing(inputbox);
		}

		if (key == KEY_ESCAPE)
		{
			StopEditing(inputbox);
		}

		if (key == KEY_BACKSPACE)
		{
			Backspace(inputbox);
		}

		c = KEY_TO_UNICODE(key);

		// Add character to the buffer, but only if it's a printable character that we can represent on the screen.
		if (isprint(c) || (c >= 128 && UnicodeCharacter(c) >= 0))
		{
			AddCharacter(inputbox, c);
		}

		return 1;
	}

	void mousePress(UNCAST_ARG(inputbox), int x, int y, int key) noexcept
	{
		CAST_ARG(txt_inputbox_t, inputbox);

		if (key == MOUSE_LEFT)
		{
			// Make mouse clicks start editing the box
			if (!inputbox->editing)
			{
				// Send a simulated keypress to start editing
				WidgetKeyPress(inputbox, KEY_ENTER);
			}
		}
	}
};

/**
 * Create a new input box widget for controlling a string value.
 *
 * @param value		Pointer to a string variable that contains
 *					a pointer to the current value of the
 *					input box. The value should be allocated
 *					dynamically; when the string is changed it
 *					will be freed and the variable set to point
 *					to the new string value. String will be in
 *					UTF-8 format.
 * @param width		Width of the input box, in characters.
 * @return			Pointer to the new input box widget.
 */
auto NewInputBox(char** value, size_t width)
{
	return InputBox<char**>(value, width);
}

/**
 * Create a new input box widget for controlling an integer value.
 *
 * @param value		Pointer to an integer variable containing the value of the input box.
 * @param width		Width of the input box, in characters.
 * @return			Pointer to the new input box widget.
 */
auto NewIntInputBox(int* value, size_t width)
{
	return InputBox<int*>(value, width);
}

} /* END NAMESPACE cudadoom::txt */

}
