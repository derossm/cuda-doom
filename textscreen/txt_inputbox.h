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

#include "txt_widget.h"

#include "txt_main.h"
#include "txt_utf8.h"
#include "txt_io.h"
#include "txt_gui.h"

//#include "txt_window.h"

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

template<typename T>
class InputBox : public Widget<InputBox>
{
	//T _val;
	std::string buffer;
	bool isEditing{false};

public:

	using type = T;

	InputBox(T _val = nullptr, int _width = 0) noexcept : buffer{std::string(_val)},
		widget_class{Selectable, CalculateSize, Draw, KeyPress, MousePress, SetLayout, SetFocus, Destroy},
		callback_table{NewCallbackTable()},
		focused{false},
		visible{true},
		align{AlignHorizontal::left},
		width{_width}
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

	bool Selectable() noexcept override final
	{
		return true;
	}

	void CalculateSize() noexcept override final
	{	}

	void Draw() noexcept override final
	{	}

	bool KeyPress(Keytype key) noexcept override final
	{
		if (key == KEY_ENTER || key == ' ')
		{
			EmitSignal("changed");
			return true;
		}

		return false;
	}

	bool MousePress(MouseEvent evt) noexcept override final
	{
		if (evt.button == MOUSE_LEFT)
		{
			// Equivalent to pressing enter
			return KeyPress(KEY_ENTER);
		}

		return false
	}

	void SetLayout() noexcept override final
	{	}

	void SetFocus(bool _focus) noexcept override final
	{	}

	void Destroy() noexcept override final
	{	}

	void set(T _val) noexcept
	{
		//if (_val)
		//{
			//buffer = std::string(_val);
		//}
	}

	std::string get() const noexcept
	{
		return buffer;
	}

	bool editing() const noexcept
	{
		return isEditing;
	}

	void toggleEditing() noexcept
	{
		isEditing = !isEditing;
	}

	void startEditing() noexcept
	{
		isEditing = true;
	}

	void stopEditing() noexcept
	{
		isEditing = false;
	}

	void inputBoxSizeCalc() noexcept
	{
		// Enough space for the box + cursor

		//w = size + 1;
		//h = 1;
	}

	void inputBoxDrawer() noexcept
	{
		//auto focused = focused();
		//auto w = width;

		// Select the background color based on whether we are currently editing, and if not, whether the widget is focused.
		//if (editing() && focused)
		//{
			//BGColor(ColorType::black, false);
		//}
		//else
		//{
			//SetWidgetBG();
		//}

		//if (!editing())
		//{
			// If not editing, use the current value from inputbox->value.
			//SetBufferFromValue();
		//}

		// If string size exceeds the widget's width, show only the end.
		auto chars{[&]()
		{
				//if (UTF8_Strlen(buffer) > w - 1)
				//{
					//auto len{UTF8_Strlen(buffer)};
					//static_assert(w > 0);
					//static_assert(len >= w);
					//DrawCodePageString("\xae");
					//DrawString(UTF8_SkipChars(buffer, (len - w) + 2));
					//return = w - 1;
				//}
				//else
				//{
					//DrawString(buffer);
					//return UTF8_Strlen(buffer);
				//}
			}()};

		//if (chars < w && editing && focused)
		//{
			//BGColor(ColorType::black, true);
			//DrawString("_");
			//++chars;
		//}

		//for (auto i{chars}; i < w; ++i)
		//{
			//DrawString(" ");
		//}
	}

	void backspace() noexcept
	{
		//unsigned len;
		//std::string p;

		//len = UTF8_Strlen(buffer);

		//if (len > 0)
		//{
			//p = UTF8_SkipChars(buffer, len - 1);
			//*p = '\0';
		//}
	}

	void addCharacter(int key) noexcept
	{
		//std::string end;
		//std::string p;

		//if (UTF8_Strlen(buffer) < size)
		//{
			// Add character to the buffer
			//end = buffer + strlen(buffer);
			//p = EncodeUTF8(end, key);
			//*p = '\0';
		//}
	}

	bool keyPress(int key) noexcept
	{
		unsigned c;

		if (!editing)
		{
			if (key == KEY_ENTER)
			{
				StartEditing();
				return true;
			}

			// Backspace or delete erases the contents of the box.
			if (key == KEY_DEL || key == KEY_BACKSPACE)
			{
				//free(*((T)inputbox->value));
				//*((T)inputbox->value) = strdup("");
			}

			return false;
		}

		if (key == KEY_ENTER)
		{
			FinishEditing();
		}

		if (key == KEY_ESCAPE)
		{
			StopEditing();
		}

		if (key == KEY_BACKSPACE)
		{
			Backspace();
		}

		c = KEY_TO_UNICODE(key);

		// Add character to the buffer, but only if it's a printable character that we can represent on the screen.
		if (isprint(c) || (c >= 128 && UnicodeCharacter(c) >= 0))
		{
			AddCharacter(c);
		}

		return true;
	}

	void mousePress(MouseEvent evt) noexcept
	{
		if (evt.button == MOUSE_LEFT)
		{
			// Make mouse clicks start editing the box
			if (!isEditing)
			{
				// Send a simulated keypress to start editing
				KeyPress(KEY_ENTER);
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
auto NewInputBox(std::string& value, int width)
{
	return InputBox(value, width);
}

/**
 * Create a new input box widget for controlling an integer value.
 *
 * @param value		Pointer to an integer variable containing the value of the input box.
 * @param width		Width of the input box, in characters.
 * @return			Pointer to the new input box widget.
 */
auto NewIntInputBox(int value, int width)
{
	return InputBox(value, width);
}

} /* END NAMESPACE cudadoom::txt */
