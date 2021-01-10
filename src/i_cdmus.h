/**********************************************************************************************************************************************\
	Copyright(C) 1993-1996 Id Software, Inc.
	Copyright(C) 1993-2008 Raven Software

	This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License
	as published by the Free Software Foundation; either version 2 of the License, or (at your option) any later version.

	This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

	DESCRIPTION:
		Hexen CD interface.
\**********************************************************************************************************************************************/
#pragma once

#include "../derma/common.h"

#include "SDL.h"

#include "doomtype.h"

constexpr int CDERR_NOTINSTALLED{10};		// MSCDEX not installed
constexpr int CDERR_NOAUDIOSUPPORT{11};		// CD-ROM Doesn't support audio
constexpr int CDERR_NOAUDIOTRACKS{12};		// Current CD has no audio tracks
constexpr int CDERR_BADDRIVE{20};			// Bad drive number
constexpr int CDERR_BADTRACK{21};			// Bad track number
constexpr int CDERR_IOCTLBUFFMEM{22};		// Not enough low memory for IOCTL
constexpr int CDERR_DEVREQBASE{100};		// DevReq errors

int cd_Error;

int I_CDMusInit()
{
	fprintf(stderr, "I_CDMusInit: CD music playback is no longer supported! Please use digital music packs instead:\n"
		"https://www.chocolate-doom.org/wiki/index.php/Digital_music_packs\n");
	return -1;
}

// We cannot print status messages inline during startup, they must be deferred until after I_CDMusInit has returned.
void I_CDMusPrintStartup() {}

int I_CDMusPlay(int track)
{
	return 0;
}

int I_CDMusStop()
{
	return 0;
}

int I_CDMusResume()
{
	return 0;
}

int I_CDMusSetVolume(int volume)
{
	return 0;
}

int I_CDMusFirstTrack()
{
	return 0;
}

int I_CDMusLastTrack()
{
	return 0;
}

int I_CDMusTrackLength(int track_num)
{
	return 0;
}
