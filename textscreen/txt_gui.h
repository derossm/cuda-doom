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

#include "txt_defines.h"

namespace cudadoom::txt
{

struct txt_cliparea_t
{
	int x1;
	int x2;
	int y1;
	int y2;
	txt_cliparea_t* next;
};

void DrawDesktopBackground(std::string title);
void DrawWindowFrame(std::string title, int x, int y, int w, int h);
void DrawSeparator(int x, int y, int w);
void DrawCodePageString(std::string s);
void DrawString(std::string s);
int CanDrawCharacter(unsigned c);

void DrawHorizScrollbar(int x, int y, int w, int cursor, int range);
void DrawVertScrollbar(int x, int y, int h, int cursor, int range);

void InitClipArea();
void PushClipArea(int x1, int x2, int y1, int y2);
void PopClipArea();

} // END NAMESPACE cudadoom::txt
