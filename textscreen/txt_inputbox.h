/**********************************************************************************************************************************************\
	Copyright(C) 2005-2014 Simon Howard

	This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License
	as published by the Free Software Foundation; either version 2 of the License, or (at your option) any later version.

	This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

	DESCRIPTION:
		InputBox is-a InputBase is-a Widget
		A widget specialized to both display values and to allow user input of new values. I/O
\**********************************************************************************************************************************************/
#pragma once

#include "txt_common.h"

#include "txt_widget.h"

#include "txt_gui.h"
#include "txt_io.h"

namespace cudadoom::txt
{

template<typename T>
class InputBase : public WidgetBase<T>
{
	//T _val;
	::std::string buffer;
	bool isEditing{false};

public:
	virtual ~InputBase() = default;

	//using type = T;

	//InputBox(T _val = nullptr, int _width = 0) noexcept : buffer{::std::string(_val)},
		//widget_class{Selectable, CalculateSize, Draw, KeyPress, MousePress, SetLayout, SetFocus, Destroy},
		//callback_table{NewCallbackTable()},
		//focused{false},
		//visible{true},
		//align{AlignHorizontal::left},
		//width{_width}
	//{
		/*	if (_val)
			{
				buffer = ::std::string(valueIn);
			}
			else
			{
				buffer = ::std::string("");
			} */
	//}

	inline bool Selectable() const noexcept override
	{
		return true;
	}

	inline void CalculateSize() noexcept override {}

	inline void Draw() noexcept override {}

	inline bool KeyPress(Keys key) noexcept override
	{
		if (key == Keys::ENTER || key == Keys::SPACE)
		{
			//EmitSignal("changed");
			return true;
		}

		return false;
/*
		unsigned c;

		if (!editing)
		{
			if (key == Keys::ENTER)
			{
				StartEditing();
				return true;
			}

			// Backspace or delete erases the contents of the box.
			if (key == Keys::DEL || key == Keys::BACKSPACE)
			{
				//free(*((T)inputbox->value));
				//*((T)inputbox->value) = strdup("");
			}

			return false;
		}

		if (key == Keys::ENTER)
		{
			FinishEditing();
		}

		if (key == Keys::ESCAPE)
		{
			StopEditing();
		}

		if (key == Keys::BACKSPACE)
		{
			Backspace();
		}

		c = KEY_TO_UNICODE(key);

		// Add character to the buffer, but only if it's a printable character that we can represent on the screen.
		if (isprint(c) || (c >= 128 && UnicodeCharacter(c) >= 0))
		{
			AddCharacter(c);
		}
/**/
	}

	inline bool MousePress(MouseEvent evt) noexcept override
	{
		if (evt.button == MOUSE_LEFT)
		{
			if (!isEditing)
			{
				return KeyPress(Keys::ENTER);
			}
		}

		return false;
	}

	inline void SetLayout() noexcept override {}

	inline void SetFocus(bool _focus) noexcept override {}

	inline void Destroy() noexcept override {}

	//void set(T _val) noexcept
	//{
		//if (_val)
		//{
			//buffer = ::std::string(_val);
		//}
	//}

	::std::string get() const noexcept
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
		//chars{[&]()
		//{
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
		//}()};

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
		//::std::string p;

		//len = UTF8_Strlen(buffer);

		//if (len > 0)
		//{
			//p = UTF8_SkipChars(buffer, len - 1);
			//*p = '\0';
		//}
	}

	void addCharacter(int key) noexcept
	{
		//::std::string end;
		//::std::string p;

		//if (UTF8_Strlen(buffer) < size)
		//{
			// Add character to the buffer
			//end = buffer + strlen(buffer);
			//p = EncodeUTF8(end, key);
			//*p = '\0';
		//}
	}
};

class InputBox : public InputBase<InputBox>
{

};

/**
* Create a new input box widget for controlling a string value.
* @param value	Pointer to a string variable that contains a pointer to the current value of the input box. The value should be allocated dynamically;
*				when the string is changed it will be freed and the variable set to point to the new string value. String will be in UTF-8 format.
* @param width	Width of the input box, in characters.
* @return		Pointer to the new input box widget.
*/
//auto NewInputBox(::std::string& value, int width);

/**
* Create a new input box widget for controlling an integer value.
* @param value	Pointer to an integer variable containing the value of the input box.
* @param width	Width of the input box, in characters.
* @return		Pointer to the new input box widget.
*/
//auto NewIntInputBox(int value, int width);

} // end namespace cudadoom::txt
