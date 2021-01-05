/**********************************************************************************************************************************************\
	Copyright(C) 1993-1996 Id Software, Inc.
	Copyright(C) 2005-2014 Simon Howard

	This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License
	as published by the Free Software Foundation; either version 2 of the License, or (at your option) any later version.

	This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

	DESCRIPTION:
		Heads up display
\**********************************************************************************************************************************************/
#pragma once

#include "../../derma/common.h"

#include "d_event.h"

// Globally visible constants.
#define HU_FONTSTART '!'	// the first font characters
#define HU_FONTEND '_'	// the last font characters

// Calculate # of glyphs in font.
#define HU_FONTSIZE (HU_FONTEND - HU_FONTSTART + 1)

constexpr size_t HU_BROADCAST{5};

#define HU_MSGX (0 - WIDESCREENDELTA)
constexpr size_t HU_MSGY{0};
constexpr size_t HU_MSGWIDTH{64}; // in characters
constexpr size_t HU_MSGHEIGHT{1}; // in lines

constexpr size_t HU_MSGTIMEOUT{(4 * TICRATE)};

// HEADS UP TEXT
void HU_Init();
void HU_Start();

bool HU_Responder(EventType* ev);

void HU_Ticker();
void HU_Drawer();
char HU_dequeueChatChar();
void HU_Erase();

extern std::array<std::string, 10> chat_macros;
