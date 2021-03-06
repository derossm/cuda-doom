/**********************************************************************************************************************************************\
	Copyright(C) 1993-1996 Id Software, Inc.
	Copyright(C) 2005-2014 Simon Howard

	This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License
	as published by the Free Software Foundation; either version 2 of the License, or (at your option) any later version.

	This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

	DESCRIPTION:
		Menu widget stuff, episode selection and such.
\**********************************************************************************************************************************************/
#pragma once

#include "../../derma/common.h"

#include "../d_event.h"

// Called by main loop, saves config file and calls I_Quit when user exits. Even when the menu is not displayed,
// this can resize the view and change game parameters. Does all the real work of the menu interaction.
bool M_Responder(EventType* ev);

// Called by main loop, only used for menu (skull cursor) animation.
void M_Ticker();

// Called by main loop, draws the menus directly into the screen buffer.
void M_Drawer();

// Called by D_DoomMain, loads the config file.
void M_Init();

// Called by intro code to force menu up upon a keypress, does nothing if menu is already up.
void M_StartControlPanel();

// Blocky mode, has default, 0 = high, 1 = normal
int detailLevel = 0;
int screenblocks = 10; // increased

// Show messages has default, 0 = off, 1 = on
int showMessages = 1;
