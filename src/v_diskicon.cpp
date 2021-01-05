/**********************************************************************************************************************************************\
	Copyright(C) 1993-1996 Id Software, Inc.
	Copyright(C) 2005-2014 Simon Howard

	This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License
	as published by the Free Software Foundation; either version 2 of the License, or (at your option) any later version.

	This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

	DESCRIPTION:
		Disk load indicator.
\**********************************************************************************************************************************************/

#include "doomtype.h"
#include "deh_str.h"
#include "i_swap.h"
#include "i_video.h"
#include "m_argv.h"
#include "v_video.h"
#include "w_wad.h"
#include "z_zone.h"

#include "v_diskicon.h"

// Only display the disk icon if more then this much bytes have been read during the previous tic.
static const int diskicon_threshold{20*1024};

// Two buffers: disk_data contains the data representing the disk icon
// (raw, not a patch_t) while saved_background is an equivalently-sized
// buffer where we save the background data while the disk is on screen.
static pixel_t* disk_data;
static pixel_t* saved_background;

static int loading_disk_xoffs{0};
static int loading_disk_yoffs{0};

// Number of bytes read since the last call to V_DrawDiskIcon().
static size_t recent_bytes_read{0};
static bool disk_drawn;

static void CopyRegion(pixel_t* dest, int dest_pitch, pixel_t* src, int src_pitch, int w, int h)
{
	pixel_t* s{src};
	pixel_t* d{dest};
	for (size_t y{0}; y < h; ++y)
	{
		memcpy(d, s, w * sizeof(*d));
		s += src_pitch;
		d += dest_pitch;
	}
}

static void SaveDiskData(std::string disk_lump, int xoffs, int yoffs)
{
	// Allocate a complete temporary screen where we'll draw the patch.
	pixel_t* tmpscreen = Z_Malloc<decltype(tmpscreen)>(SCREENWIDTH * SCREENHEIGHT * sizeof(*tmpscreen), pu_tags_t::PU_STATIC, NULL);
	memset(tmpscreen, 0, SCREENWIDTH * SCREENHEIGHT * sizeof(*tmpscreen));
	V_UseBuffer(tmpscreen);

	// Buffer where we'll save the disk data.

	if (disk_data != NULL)
	{
		Z_Free(disk_data);
		disk_data = NULL;
	}

	disk_data = Z_Malloc<decltype(disk_data)>(LOADING_DISK_W * LOADING_DISK_H * sizeof(*disk_data), pu_tags_t::PU_STATIC, NULL);

	// Draw the patch and save the result to disk_data.
	patch_t* disk = W_CacheLumpName<patch_t>(disk_lump, pu_tags_t::PU_STATIC);
	V_DrawPatch((loading_disk_xoffs >> crispy->hires) - WIDESCREENDELTA, loading_disk_yoffs >> crispy->hires, disk);
	CopyRegion(disk_data, LOADING_DISK_W, tmpscreen + yoffs * SCREENWIDTH + xoffs, SCREENWIDTH, LOADING_DISK_W, LOADING_DISK_H);
	W_ReleaseLumpName(disk_lump);

	V_RestoreBuffer();
	Z_Free(tmpscreen);
}

void V_EnableLoadingDisk(std::string lump_name, int xoffs, int yoffs)
{
	loading_disk_xoffs = xoffs;
	loading_disk_yoffs = yoffs;

	if (saved_background != NULL)
	{
		Z_Free(saved_background);
		saved_background = NULL;
	}

	saved_background = Z_Malloc<decltype(saved_background)>(LOADING_DISK_W * LOADING_DISK_H * sizeof(*saved_background), pu_tags_t::PU_STATIC, NULL);
	SaveDiskData(lump_name, xoffs, yoffs);
}

void V_BeginRead(size_t nbytes)
{
	recent_bytes_read += nbytes;
}

static pixel_t* DiskRegionPointer()
{
	return I_VideoBuffer + loading_disk_yoffs * SCREENWIDTH + loading_disk_xoffs;
}

void V_DrawDiskIcon()
{
	if (disk_data != NULL && recent_bytes_read > diskicon_threshold)
	{
		// Save the background behind the disk before we draw it.
		CopyRegion(saved_background, LOADING_DISK_W, DiskRegionPointer(), SCREENWIDTH, LOADING_DISK_W, LOADING_DISK_H);

		// Write the disk to the screen buffer.
		CopyRegion(DiskRegionPointer(), SCREENWIDTH, disk_data, LOADING_DISK_W, LOADING_DISK_W, LOADING_DISK_H);
		disk_drawn = true;
	}

	recent_bytes_read = 0;
}

void V_RestoreDiskBackground()
{
	if (disk_drawn)
	{
		// Restore the background.
		CopyRegion(DiskRegionPointer(), SCREENWIDTH, saved_background, LOADING_DISK_W, LOADING_DISK_W, LOADING_DISK_H);

		disk_drawn = false;
	}
}
