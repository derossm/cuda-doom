/**********************************************************************************************************************************************\
	Copyright(C) 2005-2014 Simon Howard

	This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License
	as published by the Free Software Foundation; either version 2 of the License, or (at your option) any later version.

	This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

	NOTE: MAJOR WORK IN PROGRESS, REQUIRES RE-WORKING MANY OTHER SYSTEMS FIRST BEFORE DESIGNING A NEW CLASS HERE
\**********************************************************************************************************************************************/
#pragma once

/**
 * @file txt_inputbox.h
 *
 * Input box widget.
 */

/**
 * Input box widget.
 *
 * An input box is a widget that displays a value, which can be
 * selected to enter a new value.
 *
 * Input box widgets can be of an integer or string type.
 */

#include "derma\common.h"
#include "derma\txt_common.h"

#include "txt_widget.h"
#include "txt_gui.h"
#include "txt_io.h"
#include "txt_main.h"
#include "txt_utf8.h"
#include "txt_window.h"

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
auto TXT_NewInputBox(char** value, size_t width)
{
	return CUDADOOM::TXT::InputBox<char**>(value, width);
}

/**
 * Create a new input box widget for controlling an integer value.
 *
 * @param value		Pointer to an integer variable containing the value of the input box.
 * @param width		Width of the input box, in characters.
 * @return			Pointer to the new input box widget.
 */
auto TXT_NewIntInputBox(int* value, size_t width)
{
	return CUDADOOM::TXT::InputBox<int*>(value, width);
}

template<typename T, typename U>
class StrongType
{
	T _val;
public:
	explicit StrongType(const T& _val) noexcept : _val(value) {}
	//T get() { return value_; }
};

struct KeyType{};
using Key = StrongType<int, KeyType>;

class DepType
{
};

namespace CUDADOOM::TXT
{

template<typename T>
class InputBox : public Widget
{
	//T _val;
	std::string buffer;
	bool _editing{false};

	txt_widget_class_t txt_inputbox_class =
	{
		TXT_AlwaysSelectable,
		TXT_InputBoxSizeCalc,
		TXT_InputBoxDrawer,
		TXT_InputBoxKeyPress,
		TXT_InputBoxDestructor,
		TXT_InputBoxMousePress,
		nullptr,
		TXT_InputBoxFocused,
	};

public:
	using type = T;

	InputBox(T _val = nullptr, size_t widthIn = 0) noexcept :	buffer{std::string(_val)};
																widget_class{&txt_inputbox_class},
																callback_table{TXT_NewCallbackTable()},
																focused{false},
																visible{true},
																align{TXT_HORIZ_LEFT},
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
		if (_val != nullptr)
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

	void inputBoxSizeCalc(TXT_UNCAST_ARG(inputbox)) noexcept
	{
		TXT_CAST_ARG(txt_inputbox_t, inputbox);

		// Enough space for the box + cursor

		inputbox->widget.w = inputbox->size + 1;
		inputbox->widget.h = 1;
	}

	void inputBoxDrawer(TXT_UNCAST_ARG(inputbox)) noexcept
	{
		TXT_CAST_ARG(txt_inputbox_t, inputbox);

		auto focused = inputbox->widget.focused;
		auto w = inputbox->widget.w;

		// Select the background color based on whether we are currently
		// editing, and if not, whether the widget is focused.

		if (inputbox->editing && focused)
		{
			TXT_BGColor(TXT_COLOR_BLACK, 0);
		}
		else
		{
			TXT_SetWidgetBG(inputbox);
		}

		if (!inputbox->editing)
		{
			// If not editing, use the current value from inputbox->value.

			SetBufferFromValue(inputbox);
		}

		// If string size exceeds the widget's width, show only the end.

		auto chars{[&]()
		{
			if (TXT_UTF8_Strlen(inputbox->buffer) > w - 1)
			{
				auto len{TXT_UTF8_Strlen(inputbox->buffer)};
				static_assert(w > 0);
				static_assert(len >= w);
				TXT_DrawCodePageString("\xae");
				TXT_DrawString(TXT_UTF8_SkipChars(inputbox->buffer, (len - w) + 2));
				return = w - 1;
			}
			else
			{
				TXT_DrawString(inputbox->buffer);
				return TXT_UTF8_Strlen(inputbox->buffer);
			}
		}()};

		if (chars < w && inputbox->editing && focused)
		{
			TXT_BGColor(TXT_COLOR_BLACK, 1);
			TXT_DrawString("_");
			++chars;
		}

		for (auto i{chars}; i < w; ++i)
		{
			TXT_DrawString(" ");
		}
	}

	void backspace(txt_inputbox_t *inputbox) noexcept
	{
		unsigned int len;
		char *p;

		len = TXT_UTF8_Strlen(inputbox->buffer);

		if (len > 0)
		{
			p = TXT_UTF8_SkipChars(inputbox->buffer, len - 1);
			*p = '\0';
		}
	}

	void addCharacter(txt_inputbox_t *inputbox, int key) noexcept
	{
		char *end, *p;

		if (TXT_UTF8_Strlen(inputbox->buffer) < inputbox->size)
		{
			// Add character to the buffer

			end = inputbox->buffer + strlen(inputbox->buffer);
			p = TXT_EncodeUTF8(end, key);
			*p = '\0';
		}
	}

	int keyPress(TXT_UNCAST_ARG(inputbox), int key) noexcept
	{
		TXT_CAST_ARG(txt_inputbox_t, inputbox);
		unsigned int c;

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

		c = TXT_KEY_TO_UNICODE(key);

		// Add character to the buffer, but only if it's a printable character
		// that we can represent on the screen.
		if (isprint(c) || (c >= 128 && TXT_UnicodeCharacter(c) >= 0))
		{
			AddCharacter(inputbox, c);
		}

		return 1;
	}

	void mousePress(TXT_UNCAST_ARG(inputbox), int x, int y, int key) noexcept
	{
		TXT_CAST_ARG(txt_inputbox_t, inputbox);

		if (key == TXT_MOUSE_LEFT)
		{
			// Make mouse clicks start editing the box

			if (!inputbox->editing)
			{
				// Send a simulated keypress to start editing

				TXT_WidgetKeyPress(inputbox, KEY_ENTER);
			}
		}
	}
};

}
