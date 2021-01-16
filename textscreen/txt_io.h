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

#include "../derma/stdafx.h"

//#include <string>

#include "../derma/common.h"

#include "txt_common.h"

namespace cudadoom::txt
{

static int cur_x = 0;
static int cur_y = 0;
static ColorType fgcolor = ColorType::grey;
static ColorType bgcolor = ColorType::black;

// Read the current state of modifier keys that are held down.
//int GetModifierState(ModifierType mod);
// Retrieve the current position of the mouse
//void GetMousePosition(int* x, int* y);
// Change mode for text input.
//void SetInputMode(InputType mode);

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

} // end namespace cudadoom::txt
