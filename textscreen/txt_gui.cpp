/**********************************************************************************************************************************************\
	Copyright(C) 2005-2014 Simon Howard

	This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License
	as published by the Free Software Foundation; either version 2 of the License, or (at your option) any later version.

	This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
\**********************************************************************************************************************************************/

#include "txt_gui.h"

#include "txt_main.h"
#include "txt_utf8.h"
#include "txt_io.h"

#define VALID_X(x) ((x) >= cliparea->x1 && (x) < cliparea->x2)
#define VALID_Y(y) ((y) >= cliparea->y1 && (y) < cliparea->y2)

namespace cudadoom::txt
{

// Array of border characters for drawing windows. The array looks like this:
//
// +-++
// | ||
// +-++
// +-++
static const int borders[4][4] =
{
	{0xda, 0xc4, 0xc2, 0xbf},
	{0xb3, ' ', 0xb3, 0xb3},
	{0xc3, 0xc4, 0xc5, 0xb4},
	{0xc0, 0xc4, 0xc1, 0xd9},
};

static txt_cliparea_t* cliparea = NULL;

void DrawDesktopBackground(std::string title)
{
	int i;
	unsigned char* screendata;
	unsigned char* p;

	screendata = GetScreenData();

	// Fill the screen with gradient characters

	p = screendata;

	for (i = 0; i < SCREEN_W * SCREEN_H; ++i)
	{
		*p = 0xb1;
		++p;
		*p = ColorType::grey | (ColorType::blue << 4);
		++p;
	}

	// Draw the top and bottom banners

	p = screendata;

	for (i = 0; i < SCREEN_W; ++i)
	{
		*p = ' ';
		++p;
		*p = ColorType::black | (ColorType::grey << 4);
		++p;
	}

	p = screendata + (SCREEN_H - 1) * SCREEN_W * 2;

	for (i = 0; i < SCREEN_W; ++i)
	{
		*p = ' ';
		++p;
		*p = ColorType::black | (ColorType::grey << 4);
		++p;
	}

	// Print the title

	GotoXY(0, 0);
	FGColor(ColorType::black);
	BGColor(ColorType::grey, false);

	DrawString(" ");
	DrawString(title);
}

void DrawShadow(int x, int y, int w, int h)
{
	unsigned char* screendata;
	unsigned char* p;
	int x1, y1;

	screendata = GetScreenData();

	for (y1 = y; y1 < y + h; ++y1)
	{
		p = screendata + (y1 * SCREEN_W + x) * 2;

		for (x1 = x; x1 < x + w; ++x1)
		{
			if (VALID_X(x1) && VALID_Y(y1))
			{
				p[1] = ColorType::dark_grey;
			}

			p += 2;
		}
	}
}

void DrawWindowFrame(std::string title, int x, int y, int w, int h)
{
	SavedColors colors;
	int x1, y1;
	int bx, by;

	SaveColors(&colors);
	FGColor(ColorType::bright_cyan);

	for (y1 = y; y1 < y + h; ++y1)
	{
		// Select the appropriate row and column in the borders
		// array to pick the appropriate character to draw at
		// this location.
		//
		// Draw a horizontal line on the third line down, so we
		// draw a box around the title.

		by = y1 == y ? 0 :
			y1 == y + 2 && title != NULL ? 2 :
			y1 == y + h - 1 ? 3 : 1;

		for (x1 = x; x1 < x + w; ++x1)
		{
			bx = x1 == x ? 0 :
				x1 == x + w - 1 ? 3 : 1;

			if (VALID_X(x1) && VALID_Y(y1))
			{
				GotoXY(x1, y1);
				PutChar(borders[by][bx]);
			}
		}
	}

	// Draw the title

	if (title != NULL)
	{
		GotoXY(x + 1, y + 1);
		BGColor(ColorType::grey, false);
		FGColor(ColorType::blue);

		for (x1 = 0; x1 < w - 2; ++x1)
		{
			DrawString(" ");
		}

		GotoXY(x + (w - UTF8_Strlen(title)) / 2, y + 1);
		DrawString(title);
	}

	// Draw the window's shadow.

	DrawShadow(x + 2, y + h, w, 1);
	DrawShadow(x + w, y + 1, 2, h);

	RestoreColors(&colors);
}

void DrawSeparator(int x, int y, int w)
{
	SavedColors colors;
	unsigned char* data;
	int x1;
	int b;

	data = GetScreenData();

	SaveColors(&colors);
	FGColor(ColorType::bright_cyan);

	if (!VALID_Y(y))
	{
		return;
	}

	data += (y * SCREEN_W + x) * 2;

	for (x1 = x; x1 < x + w; ++x1)
	{
		GotoXY(x1, y);

		b = x1 == x ? 0 :
			x1 == x + w - 1 ? 3 :
			1;

		if (VALID_X(x1))
		{
			// Read the current value from the screen
			// Check that it matches what the window should look like if
			// there is no separator, then apply the separator

			if (*data == borders[1][b])
			{
				PutChar(borders[2][b]);
			}
		}

		data += 2;
	}

	RestoreColors(&colors);
}

// Alternative to DrawString() where the argument is a "code page
// string" - characters are in native code page format and not UTF-8.
void DrawCodePageString(std::string s)
{
	int x, y;
	int x1;
	std::string p;

	GetXY(&x, &y);

	if (VALID_Y(y))
	{
		x1 = x;

		for (p = s; *p != '\0'; ++p)
		{
			if (VALID_X(x1))
			{
				GotoXY(x1, y);
				// FIXME
				//PutChar(*p);
			}

			x1 += 1;
		}
	}

	GotoXY(x + s.length(), y);
}

static void PutUnicodeChar(unsigned c)
{
	int d;

	// Treat control characters specially.
	if (c == '\n' || c == '\b')
	{
		PutChar(c);
		return;
	}

	// Map Unicode character into the symbol used to represent it in this
	// code page. For unrepresentable characters, print a fallback instead.
	// Note that we use PutSymbol() here because we just want to do a
	// raw write into the screen buffer.
	d = UnicodeCharacter(c);

	if (d >= 0)
	{
		PutSymbol(d);
	}
	else
	{
		PutSymbol('\xa8');
	}
}

void DrawString(std::string s)
{
	int x, y;
	int x1;
	std::string p;
	unsigned c;

	GetXY(&x, &y);

	if (VALID_Y(y))
	{
		x1 = x;

		for (p = s; *p != '\0'; )
		{
			c = DecodeUTF8(&p);

			if (c == 0)
			{
				break;
			}

			if (VALID_X(x1))
			{
				GotoXY(x1, y);
				PutUnicodeChar(c);
			}

			x1 += 1;
		}
	}

	GotoXY(x + UTF8_Strlen(s), y);
}

void DrawHorizScrollbar(int x, int y, int w, int cursor, int range)
{
	SavedColors colors;
	int x1;
	int cursor_x;

	if (!VALID_Y(y))
	{
		return;
	}

	SaveColors(&colors);
	FGColor(ColorType::black);
	BGColor(ColorType::grey, false);

	GotoXY(x, y);
	PutChar('\x1b');

	cursor_x = x + 1;

	if (range > 0)
	{
		cursor_x += (cursor * (w - 3)) / range;
	}

	if (cursor_x > x + w - 2)
	{
		cursor_x = x + w - 2;
	}

	for (x1 = x + 1; x1 < x + w - 1; ++x1)
	{
		if (VALID_X(x1))
		{
			if (x1 == cursor_x)
			{
				PutChar('\xdb');
			}
			else
			{
				PutChar('\xb1');
			}
		}
	}

	PutChar('\x1a');
	RestoreColors(&colors);
}

void DrawVertScrollbar(int x, int y, int h, int cursor, int range)
{
	SavedColors colors;
	int y1;
	int cursor_y;

	if (!VALID_X(x))
	{
		return;
	}

	SaveColors(&colors);
	FGColor(ColorType::black);
	BGColor(ColorType::grey, false);

	GotoXY(x, y);
	PutChar('\x18');

	cursor_y = y + 1;

	if (cursor_y > y + h - 2)
	{
		cursor_y = y + h - 2;
	}

	if (range > 0)
	{
		cursor_y += (cursor * (h - 3)) / range;
	}

	for (y1 = y + 1; y1 < y + h - 1; ++y1)
	{
		if (VALID_Y(y1))
		{
			GotoXY(x, y1);

			if (y1 == cursor_y)
			{
				PutChar('\xdb');
			}
			else
			{
				PutChar('\xb1');
			}
		}
	}

	GotoXY(x, y + h - 1);
	PutChar('\x19');
	RestoreColors(&colors);
}

void InitClipArea()
{
	if (cliparea == NULL)
	{
		cliparea = static_cast<decltype(cliparea)>(malloc(sizeof(txt_cliparea_t)));
		cliparea->x1 = 0;
		cliparea->x2 = SCREEN_W;
		cliparea->y1 = 0;
		cliparea->y2 = SCREEN_H;
		cliparea->next = NULL;
	}
}

void PushClipArea(int x1, int x2, int y1, int y2)
{
	txt_cliparea_t* newarea;

	newarea = static_cast<decltype(newarea)>(malloc(sizeof(txt_cliparea_t)));

	// Set the new clip area to the intersection of the old
	// area and the new one.

	newarea->x1 = cliparea->x1;
	newarea->x2 = cliparea->x2;
	newarea->y1 = cliparea->y1;
	newarea->y2 = cliparea->y2;

	if (x1 > newarea->x1)
		newarea->x1 = x1;
	if (x2 < newarea->x2)
		newarea->x2 = x2;
	if (y1 > newarea->y1)
		newarea->y1 = y1;
	if (y2 < newarea->y2)
		newarea->y2 = y2;

#if 0
	printf("New scrollable area: %i,%i-%i,%i\n", x1, y1, x2, y2);
#endif

	// Hook into the list

	newarea->next = cliparea;
	cliparea = newarea;
}

void PopClipArea()
{
	txt_cliparea_t* next_cliparea;

	// Never pop the last entry

	if (cliparea->next == NULL)
		return;

	// Unlink the last entry and delete

	next_cliparea = cliparea->next;
	free(cliparea);
	cliparea = next_cliparea;
}

} /* END NAMESPACE cudadoom::txt */
