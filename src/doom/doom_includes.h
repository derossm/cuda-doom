/**********************************************************************************************************************************************\
	Copyright(C) 1993-1996 Id Software, Inc.
	Copyright(C) 2005-2014 Simon Howard

	This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License
	as published by the Free Software Foundation; either version 2 of the License, or (at your option) any later version.

	This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

	DESCRIPTION:
		DOOM main program (D_DoomMain) and game loop (D_DoomLoop),plus functions to determine game mode (shareware, registered),
		parse command line parameters, configure game parameters (turbo), and call the startup functions.
\**********************************************************************************************************************************************/
#pragma once

#include "../../build/config.h"

#include "../../derma/common.h"

#include "../deh_main.h"

#include "../d_iwad.h"

#include "../z_zone.h"
#include "../w_main.h"
#include "../w_wad.h"
#include "../v_diskicon.h"
#include "../v_video.h"

#include "../m_argv.h"
#include "../m_config.h"
#include "../m_controls.h"
#include "../m_misc.h"

#include "../i_endoom.h"
#include "../i_input.h"
#include "../i_joystick.h"
#include "../i_system.h"
#include "../i_timer.h"
#include "../i_video.h"

#include "../net_client.h"
#include "../net_dedicated.h"
#include "../net_query.h"

#include "doomdef.h"
#include "doomstat.h"

#include "dstrings.h"
#include "sounds.h"
#include "d_pwad.h"
#include "f_finale.h"
#include "f_wipe.h"

#include "s_sound.h"

#include "m_menu.h"
#include "p_saveg.h"

#include "g_game.h"

#include "hu_stuff.h"
#include "wi_stuff.h"
#include "st_stuff.h"
#include "am_map.h"

#include "p_setup.h"
#include "r_local.h"
#include "statdump.h"

#include "d_main.h"
