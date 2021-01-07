/**********************************************************************************************************************************************\
	Copyright(C) 2005-2014 Simon Howard

	This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License
	as published by the Free Software Foundation; either version 2 of the License, or (at your option) any later version.

	This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

	DESCRIPTION:
		Text mode emulation in SDL
\**********************************************************************************************************************************************/
#pragma once
// DECOUPLE
#include "../derma/common.h"
//////////

#include "txt_main.h"

namespace cudadoom::txt
{

struct SavedColors
{
	ColorType backgroundColor;
	ColorType foregroundColor;
};

void PutSymbol(int c);
void PutChar(int c);
void Puts(std::string s);

void GotoXY(int x, int y);
void GetXY(int* x, int* y);

void FGColor(ColorType color);
void BGColor(ColorType color, bool blinking);

void SaveColors(SavedColors* save);
void RestoreColors(SavedColors* save);

void ClearScreen();

static int cur_x = 0;
static int cur_y = 0;
static ColorType fgcolor = ColorType::grey;
static ColorType bgcolor = ColorType::black;

static void NewLine(unsigned char* screendata)
{
	int i;
	unsigned char* p;

	cur_x = 0;
	++cur_y;

	if (cur_y >= SCREEN_H)
	{
		// Scroll the screen up
		cur_y = SCREEN_H - 1;

		memmove(screendata, screendata + SCREEN_W * 2, SCREEN_W * 2 * (SCREEN_H - 1));

		// Clear the bottom line
		p = screendata + (SCREEN_H - 1) * 2 * SCREEN_W;

		for (i = 0; i < SCREEN_W; ++i)
		{
			*p = ' ';
			++p;
// FIXME
//			*p = fgcolor | (bgcolor << 4);
			++p;
		}
	}
}

static void PutSymbol(unsigned char* screendata, int c)
{
	unsigned char* p;

	p = screendata + cur_y * SCREEN_W * 2 + cur_x * 2;

	// Add a new character to the buffer

// FIXME
	p[0] = c;
//	p[1] = fgcolor | (bgcolor << 4);

	++cur_x;

	if (cur_x >= SCREEN_W)
	{
		NewLine(screendata);
	}
}

// "Blind" version of PutChar() below which doesn't do any interpretation
// of control signals. Just write a particular symbol to the screen buffer.
void PutSymbol(int c)
{
	PutSymbol(GetScreenData(), c);
}

static void PutChar(unsigned char* screendata, int c)
{
	switch (c)
	{
	case '\n':
		NewLine(screendata);
		break;

	case '\b':
		// backspace
		--cur_x;
		if (cur_x < 0)
			cur_x = 0;
		break;

	default:
		PutSymbol(screendata, c);
		break;
	}
}

void PutChar(int c)
{
	PutChar(GetScreenData(), c);
}

void Puts(std::string s)
{
	/*unsigned char* screen;
	std::string p;

	screen = GetScreenData();

	for (p = s; *p != '\0'; ++p)
	{
		PutChar(screen, *p);
	}

	PutChar(screen, '\n');*/
}

void GotoXY(int x, int y)
{
	cur_x = x;
	cur_y = y;
}

void GetXY(int* x, int* y)
{
	*x = cur_x;
	*y = cur_y;
}

void FGColor(ColorType color)
{
	fgcolor = color;
}

void BGColor(ColorType color, bool blinking)
{
	bgcolor = color;
	if (blinking)
	{
// FIXME
//		bgcolor |= COLOR_BLINKING;
	}
}

void SaveColors(SavedColors* save)
{
// FIXME
//	save->bgcolor = bgcolor;
//	save->fgcolor = fgcolor;
}

void RestoreColors(SavedColors* save)
{
// FIXME
//	bgcolor = save->bgcolor;
//	fgcolor = save->fgcolor;
}

void ClearScreen()
{
	auto screen{GetScreenData()};

	for (size_t i{0}; i < SCREEN_W * SCREEN_H; ++i)
	{
		// TODO fix
		screen[i * 2] = ' ';
		//screen[i * 2 + 1] = (bgcolor << 4) | fgcolor;
	}

	cur_x = 0;
	cur_y = 0;
}

} // END NAMESPACE cudadoom::txt
