//
// Copyright(C) 2005-2014 Simon Howard
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.	See the
// GNU General Public License for more details.
//

// NOTE: MAJOR WORK IN PROGRESS, REQUIRES RE-WORKING MANY OTHER SYSTEMS FIRST BEFORE DESIGNING A NEW CLASS HERE
#pragma once

#ifndef TXT_INPUTBOX_H
#define TXT_INPUTBOX_H

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

#include "txt_widget.h"

#include <string>
#include <variant>
//template<typename T>
struct txt_inputbox_t
{
	txt_widget_t widget;
	std::string buffer;
	bool editing;
	std::variant<char**, int*> value;

	//using type = T;
};
#include <memory>

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "doomkeys.h"

#include "txt_inputbox.h"
#include "txt_gui.h"
#include "txt_io.h"
#include "txt_main.h"
#include "txt_utf8.h"
#include "txt_window.h"

/**
 * Create a new input box widget for controlling a string value.
 *
 * @param value		Pointer to a string variable that contains
 *						a pointer to the current value of the
 *						input box. The value should be allocated
 *						dynamically; when the string is changed it
 *						will be freed and the variable set to point
 *						to the new string value. String will be in
 *						UTF-8 format.
 * @param size			Width of the input box, in characters.
 * @return				Pointer to the new input box widget.
 */

auto TXT_NewInputBox(char** value, int size)
{
	return InputBox<char**>().NewInputBox(value, size);
}

/**
 * Create a new input box widget for controlling an integer value.
 *
 * @param value		Pointer to an integer variable containing
 *						the value of the input box.
 * @param size			Width of the input box, in characters.
 * @return				Pointer to the new input box widget.
 */

auto TXT_NewIntInputBox(int* value, int size)
{
	return InputBox<int*>().NewInputBox(value, size);
}

template<typename T, typename U>
class StrongType
{
	T value_;
public:
	explicit StrongType(const T& value) noexcept : value_(value) {}
	//T get() { return value_; }
};

struct KeyType{};
using Key = StrongType<int, KeyType>;

template<typename T>
class InputBox
{
	static void SetBufferFromValue(txt_inputbox_t *inputbox) noexcept
	{
		T& value{inputbox->value};
		if (inputbox->widget.widget_class == &txt_inputbox_class)
		{
			if (value != nullptr)
			{
				TXT_StringCopy(inputbox->buffer, *value, strnlen(*value, inputbox->buffer_len) + 1);
			}
			else
			{
				TXT_StringCopy(inputbox->buffer, "", inputbox->buffer_len);
			}
		}
		else if (inputbox->widget.widget_class == &txt_int_inputbox_class)
		{
			TXT_snprintf(inputbox->buffer, inputbox->buffer_len, "%i", *value);
		}
	}

	static void StartEditing(txt_inputbox_t *inputbox)
	{
		// Integer input boxes start from an empty buffer:

		if (inputbox->widget.widget_class == &txt_int_inputbox_class)
		{
			TXT_StringCopy(inputbox->buffer, "", inputbox->buffer_len);
		}
		else
		{
			SetBufferFromValue(inputbox);
		}

		// Switch to text input mode so we get shifted input.
		TXT_SetInputMode(TXT_INPUT_TEXT);
		inputbox->editing = 1;
	}

	static void StopEditing(txt_inputbox_t *inputbox) noexcept
	{
		if (inputbox->editing)
		{
			// Switch back to normal input mode.
			TXT_SetInputMode(TXT_INPUT_NORMAL);
			inputbox->editing = 0;
		}
	}

	static void FinishEditing(txt_inputbox_t *inputbox) noexcept
	{
		if (!inputbox->editing)
		{
			return;
		}

		// Save the new value back to the variable.

		if (inputbox->widget.widget_class == &txt_inputbox_class)
		{
			free(*((T)inputbox->value));
			*((T)inputbox->value) = strdup(inputbox->buffer);
		}
		else if (inputbox->widget.widget_class == &txt_int_inputbox_class)
		{
			*((T)inputbox->value) = atoi(inputbox->buffer);
		}

		TXT_EmitSignal(&inputbox->widget, "changed");

		StopEditing(inputbox);
	}

	static void TXT_InputBoxSizeCalc(TXT_UNCAST_ARG(inputbox)) noexcept
	{
		TXT_CAST_ARG(txt_inputbox_t, inputbox);

		// Enough space for the box + cursor

		inputbox->widget.w = inputbox->size + 1;
		inputbox->widget.h = 1;
	}

	static void TXT_InputBoxDrawer(TXT_UNCAST_ARG(inputbox)) noexcept
	{
		TXT_CAST_ARG(txt_inputbox_t, inputbox);
		//int focused;
		//int chars;
		//int w;

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

	static void TXT_InputBoxDestructor(TXT_UNCAST_ARG(inputbox)) noexcept
	{
		TXT_CAST_ARG(txt_inputbox_t, inputbox);

		StopEditing(inputbox);
		free(inputbox->buffer);
	}

	static void Backspace(txt_inputbox_t *inputbox) noexcept
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

	static void AddCharacter(txt_inputbox_t *inputbox, int key) noexcept
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

	static int TXT_InputBoxKeyPress(TXT_UNCAST_ARG(inputbox), int key) noexcept
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

	void TXT_InputBoxMousePress(TXT_UNCAST_ARG(inputbox), int x, int y, int key) noexcept
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

	void TXT_InputBoxFocused(TXT_UNCAST_ARG(inputbox), int focused) noexcept
	{
		TXT_CAST_ARG(txt_inputbox_t, inputbox);

		// Stop editing when we lose focus.

		if (inputbox->editing && !focused)
		{
			FinishEditing(inputbox);
		}
	}

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

	txt_widget_class_t txt_int_inputbox_class =
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
	auto NewInputBox(txt_widget_class_t *widget_class, T value, int size) noexcept
	{
		auto inputBox = std::make_unique<txt_input_box_t>;
		//txt_inputbox_t *inputbox;

		//inputbox = malloc(sizeof(txt_inputbox_t));

		TXT_InitWidget(inputbox, widget_class);
		inputbox->value = value;
		inputbox->size = size;
		// 'size' is the maximum number of characters that can be entered,
		// but for a UTF-8 string, each character can take up to four characters.
		inputbox->buffer_len = size * 4 + 1;
		inputbox->buffer = malloc(inputbox->buffer_len);
		inputbox->editing = 0;

		return inputbox;
	}

	InputBox() noexcept
	{
		//widget->callback_table = TXT_NewCallbackTable();
		//widget->parent = nullptr;

		// Not focused until we hear otherwise.

		//widget->focused = 0;

		// Visible by default.

		//widget->visible = 1;

		// Align left by default

		//widget->align = TXT_HORIZ_LEFT;
	}
};


#endif /* #ifndef TXT_INPUTBOX_H */
