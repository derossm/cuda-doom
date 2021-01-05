/**********************************************************************************************************************************************\
	Copyright(C) 1993-1996 Id Software, Inc.
	Copyright(C) 2005-2014 Simon Howard

	This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License
	as published by the Free Software Foundation; either version 2 of the License, or (at your option) any later version.

	This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

	DESCRIPTION:
		Exit text-mode ENDOOM screen.
\**********************************************************************************************************************************************/
#pragma once

#include "../derma/common.h"

#include "config.h"
#include "doomtype.h"
#include "i_video.h"

#include "../textscreen/txt_main.h"

constexpr int ENDOOM_W{80};
constexpr int ENDOOM_H{25};

// Displays the text mode ending screen after the game quits
void I_Endoom(byte* endoom_data)
{
	unsigned char* screendata;
	int y;
	int indent;

	// Set up text mode screen
	cudadoom::txt::Init();

	cudadoom::txt::SetWindowTitle(PACKAGE_STRING);
	// SDL2-TODO I_InitWindowTitle();
	// SDL2-TODO I_InitWindowIcon();

	// Write the data to the screen memory
	screendata = cudadoom::txt::GetScreenData();

	indent = (ENDOOM_W - cudadoom::txt::SCREEN_W) / 2;

	for (y=0; y<cudadoom::txt::SCREEN_H; ++y)
	{
		memcpy(screendata + (y * cudadoom::txt::SCREEN_W * 2),
				endoom_data + (y * ENDOOM_W + indent) * 2,
				cudadoom::txt::SCREEN_W * 2);
	}

	// Wait for a keypress
	while (true)
	{
		cudadoom::txt::UpdateScreen();

		if (cudadoom::txt::GetChar() > 0)
		{
			break;
		}

		cudadoom::txt::Sleep(0);
	}

	// Shut down text mode screen
	cudadoom::txt::Shutdown();
}
