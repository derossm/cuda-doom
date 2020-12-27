/**********************************************************************************************************************************************\
	Copyright(C) 1993-1996 Id Software, Inc.
	Copyright(C) 1993-2008 Raven Software
	Copyright(C) 2005-2014 Simon Howard

	This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License
	as published by the Free Software Foundation; either version 2 of the License, or (at your option) any later version.

	This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

// all external data is defined here
// most of the data is loaded into different structures at run time
\**********************************************************************************************************************************************/
#pragma once

#include "../../derma/common.h"

#ifndef __DOOMDATA__
#define __DOOMDATA__

#include "..\doomtype.h"

/*
===============================================================================
						map level types
===============================================================================
*/

// lump order in a map wad
enum
{
	ML_LABEL,
	ML_THINGS,
	ML_LINEDEFS,
	ML_SIDEDEFS,
	ML_VERTEXES,
	ML_SEGS,
	ML_SSECTORS,
	ML_NODES,
	ML_SECTORS,
	ML_REJECT,
	ML_BLOCKMAP
};

typedef PACKED_STRUCT (
{
	short x;
	short y;
}) mapvertex_t;

typedef PACKED_STRUCT (
{
	short textureoffset;
	short rowoffset;
	char toptexture[8];
	char bottomtexture[8];
	char midtexture[8];
	short sector;					// on viewer's side
}) mapsidedef_t;

typedef PACKED_STRUCT (
{
	short v1;
	short v2;
	short flags;
	short special;
	short tag;
	short sidenum[2];				// sidenum[1] will be -1 if one sided
}) maplinedef_t;

#define ML_BLOCKING			1
#define ML_BLOCKMONSTERS	2
#define ML_TWOSIDED			4		// backside will not be present at all if not two sided

// if a texture is pegged, the texture will have the end exposed to air held
// constant at the top or bottom of the texture (stairs or pulled down things)
// and will move with a height change of one of the neighbor sectors
// Unpegged textures allways have the first row of the texture at the top
// pixel of the line for both top and bottom textures (windows)
#define ML_DONTPEGTOP		8
#define ML_DONTPEGBOTTOM	16

#define ML_SECRET			32		// don't map as two sided: IT'S A SECRET!
#define ML_SOUNDBLOCK		64		// don't let sound cross two of these
#define ML_DONTDRAW			128		// don't draw on the automap
#define ML_MAPPED			256		// set if allready drawn in automap

typedef PACKED_STRUCT (
{
	short floorheight;
	short ceilingheight;
	char floorpic[8];
	char ceilingpic[8];
	short lightlevel;
	short special;
	short tag;
}) mapsector_t;

typedef PACKED_STRUCT (
{
	short numsegs;
	short firstseg;				// segs are stored sequentially
}) mapsubsector_t;

typedef PACKED_STRUCT (
{
	short v1;
	short v2;
	short angle;
	short linedef;
	short side;
	short offset;
}) mapseg_t;

#define NF_SUBSECTOR	0x8000

typedef PACKED_STRUCT (
{
	short x;					// partition line
	short y;
	short dx;
	short dy;
	short bbox[2][4];			// bounding box for each child
	unsigned short children[2];	// if NF_SUBSECTOR its a subsector
}) mapnode_t;

typedef PACKED_STRUCT (
{
	short x;
	short y;
	short angle;
	short type;
	short options;
}) mapthing_t;

#define MTF_EASY		1
#define MTF_NORMAL		2
#define MTF_HARD		4
#define MTF_AMBUSH		8

/*
===============================================================================
						texture definition
===============================================================================
*/

typedef PACKED_STRUCT (
{
	short originx;
	short originy;
	short patch;
	short stepdir;
	short colormap;
}) mappatch_t;

typedef PACKED_STRUCT (
{
	char name[8];
	bool masked;
	short width;
	short height;
	int obsolete;
	short patchcount;
	mappatch_t patches[1];
}) maptexture_t;


/*
===============================================================================
							graphics
===============================================================================
*/

// a pic is an unmasked block of pixels
typedef struct
{
	byte width;
	byte height;
	byte data;
} pic_t;

#endif // __DOOMDATA__
