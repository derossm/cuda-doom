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
 * Label widget.
 *
 * A label widget does nothing except show a text label.
 */
class Label : public Widget<Label>
{
	Widget widget;
	std::string label;
	char** lines;
	unsigned w;
	unsigned h;
	int fgcolor;
	int bgcolor;

public:

	Label() : widget_class{Selectable, CalculateSize, Draw, KeyPress, MousePress, SetLayout, SetFocus, Destroy}
	{	}

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
};

/**
 * Create a new label widget.
 *
 * @param label			String to display in the widget (UTF-8 format).
 * @return				Pointer to the new label widget.
 */
txt_label_t* NewLabel(std::string label);

/**
 * Set the string displayed in a label widget.
 *
 * @param label			The widget.
 * @param value			The string to display (UTF-8 format).
 */
void SetLabel(txt_label_t* label, std::string value);

/**
 * Set the background color of a label widget.
 *
 * @param label			The widget.
 * @param color			The background color to use.
 */

void SetBGColor(txt_label_t* label, ColorType color);

/**
 * Set the foreground color of a label widget.
 *
 * @param label			The widget.
 * @param color			The foreground color to use.
 */
void SetFGColor(txt_label_t* label, ColorType color);

static void LabelSizeCalc(UNCAST_ARG(label))
{
	CAST_ARG(txt_label_t, label);

	label->widget.w = label->w;
	label->widget.h = label->h;
}

static void LabelDrawer(UNCAST_ARG(label))
{
	CAST_ARG(txt_label_t, label);
	unsigned x;
	unsigned y;
	int origin_x;
	int origin_y;
	unsigned align_indent = 0;
	unsigned w;
	unsigned sw;

	w = label->widget.w;

	if (label->bgcolor >= 0)
	{
		BGColor(label->bgcolor, false);
	}
	if (label->fgcolor >= 0)
	{
		FGColor(label->fgcolor);
	}

	GetXY(&origin_x, &origin_y);

	for (y = 0; y < label->h; ++y)
	{
		// Calculate the amount to indent this line due to the align
		// setting
		sw = UTF8_Strlen(label->lines[y]);
		switch (label->widget.align)
		{
		case HORIZ_LEFT:
			align_indent = 0;
			break;
		case HORIZ_CENTER:
			align_indent = (label->w - sw) / 2;
			break;
		case HORIZ_RIGHT:
			align_indent = label->w - sw;
			break;
		}

		// Draw this line

		GotoXY(origin_x, origin_y + y);

		// Gap at the start

		for (x = 0; x < align_indent; ++x)
		{
			DrawString(" ");
		}

		// The string itself

		DrawString(label->lines[y]);
		x += sw;

		// Gap at the end

		for (; x < w; ++x)
		{
			DrawString(" ");
		}
	}
}

static void LabelDestructor(UNCAST_ARG(label))
{
	CAST_ARG(txt_label_t, label);

	free(label->label);
	free(label->lines);
}

WidgetClass txt_label_class =
{
	NeverSelectable,
	LabelSizeCalc,
	LabelDrawer,
	NULL,
	LabelDestructor,
	NULL,
	NULL,
};

void SetLabel(txt_label_t* label, std::string value)
{
	std::string p;
	unsigned y;

	// Free back the old label

	free(label->label);
	free(label->lines);

	// Set the new value

	label->label = strdup(value);

	// Work out how many lines in this label

	label->h = 1;

	for (p = label->label; *p != '\0'; ++p)
	{
		if (*p == '\n')
		{
			++label->h;
		}
	}

	// Split into lines

	label->lines = static_cast<decltype(label->lines)>(malloc(sizeof(char*) * label->h));
	label->lines[0] = label->label;
	y = 1;

	for (p = label->label; *p != '\0'; ++p)
	{
		if (*p == '\n')
		{
			label->lines[y] = p + 1;
			*p = '\0';
			++y;
		}
	}

	label->w = 0;

	for (y = 0; y < label->h; ++y)
	{
		unsigned line_len;

		line_len = UTF8_Strlen(label->lines[y]);

		if (line_len > label->w)
			label->w = line_len;
	}
}

txt_label_t* NewLabel(std::string text)
{
	txt_label_t* label;

	label = static_cast<decltype(label)>(malloc(sizeof(txt_label_t)));

	InitWidget(label, &txt_label_class);
	label->label = NULL;
	label->lines = NULL;

	// Default colors

	label->bgcolor = -1;
	label->fgcolor = -1;

	SetLabel(label, text);

	return label;
}

void SetFGColor(txt_label_t* label, ColorType color)
{
	label->fgcolor = color;
}

void SetBGColor(txt_label_t* label, ColorType color)
{
	label->bgcolor = color;
}

} // END NAMESPACE cudadoom::txt
