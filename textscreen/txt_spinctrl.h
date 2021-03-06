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
 * Spin control widget.
 *
 * A spin control widget works as an input box that can be used to
 * set numeric values, but also has buttons that allow its value
 * to be increased or decreased.
 */

class SpinControl : public WidgetBase<SpinControl>
{
public:
	bool editing{false};

	::std::variant<float, int>* value;
	::std::variant<float, int> min;
	::std::variant<float, int> max;
	::std::variant<float, int> step;

	::std::string buffer;

public:

	SpinControl() {} //: widget_class{Selectable, CalculateSize, Draw, KeyPress, MousePress, SetLayout, SetFocus, Destroy}

	SpinControl(int* _value, int _min, int _max) {} //: widget_class{Selectable, CalculateSize, Draw, KeyPress, MousePress, SetLayout, SetFocus, Destroy},
		//value{_value}, min{_min}, max{_max}, step{1}

	SpinControl(float* _value, float _min, float _max) {} //: widget_class{Selectable, CalculateSize, Draw, KeyPress, MousePress, SetLayout, SetFocus, Destroy},
		//value{_value}, min{_min}, max{_max}, step{0.1f}

	inline bool Selectable() const noexcept override final
	{
		return true;
	}

	inline void CalculateSize() noexcept override final {}

	inline void Draw() noexcept override final {}

	inline bool KeyPress(Keys key) noexcept override final
	{
/*
		if (key == KEY_ENTER || key == ' ')
		{
			EmitSignal("changed");
			return true;
		}
/**/
		return false;
	}

	inline bool MousePress(MouseEvent evt) noexcept override final
	{
/*
		if (evt.button == MOUSE_LEFT)
		{
			// Equivalent to pressing enter
			return KeyPress(KEY_ENTER);
		}
/**/
		return false;
	}

	inline void SetLayout() noexcept override final {}

	inline void SetFocus(bool _focus) noexcept override final {}

	inline void Destroy() noexcept override final {}
/*
	// Generate the format string to be used for displaying floats
	void FloatFormatString(float step, ::std::string buf, size_t buf_len)
	{
		int precision;

		precision = (int)ceil(-log(step) / log(10));

		if (precision > 0)
		{
			snprintf(buf, buf_len, "%%.%if", precision);
		}
		else
		{
			//StringCopy(buf, "%.1f", buf_len);
		}
	}

	// Number of characters needed to represent a character
	unsigned IntWidth(int val)
	{
		char buf[25];

		snprintf(buf, sizeof(buf), "%i", val);

		return strlen(buf);
	}

	unsigned FloatWidth(float val, float step)
	{
		// Calculate the width of the int value
		unsigned result = IntWidth((int)val);

		// Add a decimal part if the precision specifies it
		unsigned precision = (unsigned)ceil(-log(step) / log(10));

		if (precision > 0)
		{
			result += precision + 1;
		}

		return result;
	}

	// Returns the minimum width of the input box
	unsigned Width()
	{
		unsigned minw;
		unsigned maxw;

		if (::std::holds_alternative<float>(min))
		{
			minw = FloatWidth(::std::get<float>(min), ::std::get<float>(step));
			maxw = FloatWidth(::std::get<float>(max), ::std::get<float>(step));
		}
		else if (::std::holds_alternative<int>(min))
		{
			minw = IntWidth(::std::get<int>(min));
			maxw = IntWidth(::std::get<int>(max));
		}

		// Choose the wider of the two values. Add one so that there is always space for the cursor when editing.
		if (minw > maxw)
		{
			return minw;
		}
		else
		{
			return maxw;
		}
	}

	void SizeCalc()
	{
		//width = Width() + 5;
		//height = 1;
	}

	void SetBuffer()
	{
		char format[25];

		if (::std::holds_alternative<float>(min))
		{
			FloatFormatString(::std::get<float>(step), format, sizeof(format));
			snprintf(buffer.c_str(), buffer.length(), format, ::std::get<float>(value));
		}
		else if (::std::holds_alternative<int>(min))
		{
			snprintf(buffer.c_str(), buffer.length(), "%i", ::std::get<int>(value));
		}

	}

	void Drawer()
	{
		SavedColors colors;
		SaveColors(&colors);

		FGColor(ColorType::bright_cyan);
		DrawCodePageString("\x1b ");

		RestoreColors(&colors);

		// Choose background color
		if (focused && editing)
		{
			BGColor(ColorType::black, false);
		}
		else
		{
			SetWidgetBG();
		}

		if (!editing)
		{
			SetBuffer();
		}

		unsigned i{0};

		int bw{buffer.length()};
		unsigned padding{width - bw - 4};

		while (i < padding)
		{
			DrawString(" ");
			++i;
		}

		DrawString(buffer);
		i += bw;

		while (i < width - 4)
		{
			DrawString(" ");
			++i;
		}

		RestoreColors(&colors);
		FGColor(ColorType::bright_cyan);
		DrawCodePageString(" \x1a");
	}

	void AddCharacter(int key)
	{
		//if (UTF8_Strlen(buffer) < SpinControlWidth() && strlen(buffer) < buffer_len - 2)
		//{
			//buffer[strlen(buffer) + 1] = '\0';
			//buffer[strlen(buffer)] = key;
		//}
		buffer = buffer + key;
	}

	void Backspace()
	{
		//if (UTF8_Strlen(spincontrol->buffer) > 0)
		//{
			//spincontrol->buffer[strlen(spincontrol->buffer) - 1] = '\0';
		//}
	}

	void EnforceLimits()
	{
		if (::std::holds_alternative<float>(min))
		{
			if (::std::get<float>(*value) > ::std::get<float>(max))
			{
				*value = max;
			}
			else if (::std::get<float>(*value) < ::std::get<float>(min))
			{
				*value = min;
			}
		}
		else if (::std::holds_alternative<int>(min))
		{
			if (::std::get<int>(*value) > ::std::get<int>(max))
			{
				*value = max;
			}
			else if (::std::get<int>(*value) < ::std::get<int>(min))
			{
				*value = min;
			}
		}
	}

	void FinishEditing()
	{
		if (::std::holds_alternative<float>(min))
		{
			*value = (float)atof(buffer.c_str());
		}
		else if (::std::holds_alternative<int>(min))
		{
			*value = atoi(buffer.c_str());
		}

		editing = false;
		EnforceLimits();
	}

	bool KeyPress(Keys key)
	{
		// Enter to enter edit mode
		if (editing)
		{
			if (key == KEY_ENTER)
			{
				FinishEditing();
				return true;
			}

			if (key == KEY_ESCAPE)
			{
				// Abort without saving value
				editing = false;
				return true;
			}

			if (isdigit(key) || key == '-' || key == '.')
			{
				AddCharacter(key);
				return true;
			}

			if (key == KEY_BACKSPACE)
			{
				Backspace();
				return true;
			}
		}
		else
		{
			// Non-editing mode
			if (key == KEY_ENTER)
			{
				editing = true;
				StringCopy(buffer, "", buffer_len);
				return true;
			}
			if (key == KEY_LEFTARROW)
			{
				*value -= step;
				EnforceLimits(spincontrol);

				return true;
			}

			if (key == KEY_RIGHTARROW)
			{
				*value += step;
				EnforceLimits(spincontrol);

				return true;
			}
		}

		return false;
	}

	void MousePress(int x, int y, int b)
	{
		unsigned rel_x{x - coordinates.x};

		if (rel_x < 2)
		{
			KeyPress(KEY_LEFTARROW);
		}
		else if (rel_x >= width - 2)
		{
			KeyPress(KEY_RIGHTARROW);
		}
	}

	void Focused()
	{
		FinishEditing();
	}
/**/
};

} // end namespace cudadoom::txt
