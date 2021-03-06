/**********************************************************************************************************************************************\
	Copyright(C) 1993-1996 Id Software, Inc.
	Copyright(C) 2005-2014 Simon Howard

	This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License
	as published by the Free Software Foundation; either version 2 of the License, or (at your option) any later version.

	This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

	DESCRIPTION:
		Map Objects, MObj, definition and handling.
\**********************************************************************************************************************************************/
#pragma once

#include "../../derma/common.h"

// Basics.
#include "../tables.h"
#include "../m_fixed.h"

// We need the WAD data structure for Map things, from the THINGS lump.
#include "doomdata.h"

// We need the thinker_t stuff.
#include "d_think.h"

// States are tied to finite states are tied to animation frames. Needs precompiled tables/data structures.
#include "info.h"

// NOTES: MapObject
//
// mobj_ts are used to tell the refresh where to draw an image,
// tell the world simulation when objects are contacted,
// and tell the sound driver how to position a sound.
//
// The refresh uses the next and prev links to follow
// lists of things in sectors as they are being drawn.
// The sprite, frame, and angle elements determine which patch_t
// is used to draw the sprite if it is visible.
// The sprite and frame values are allmost allways set
// from state_t structures.
// The statescr.exe utility generates the states.h and states.c
// files that contain the sprite/frame numbers from the
// statescr.txt source file.
// The xyz origin point represents a point at the bottom middle
// of the sprite (between the feet of a biped).
// This is the default origin position for patch_ts grabbed
// with lumpy.exe.
// A walking creature will have its z equal to the floor
// it is standing on.
//
// The sound code uses the x,y, and subsector fields
// to do stereo positioning of any sound effited by the MapObject.
//
// The play simulation uses the blocklinks, x,y,z, radius, height
// to determine when mobj_ts are touching each other,
// touching lines in the map, or hit by trace lines (gunshots,
// lines of sight, etc).
// The MapObject->flags element has various bit flags
// used by the simulation.
//
// Every MapObject is linked into a single sector
// based on its origin coordinates.
// The subsector_t is found with R_PointInSubsector(x,y),
// and the sector_t can be found with subsector->sector.
// The sector links are only used by the rendering code,
// the play simulation does not care about them at all.
//
// Any MapObject that needs to be acted upon by something else
// in the play world (block movement, be shot, etc) will also
// need to be linked into the blockmap.
// If the thing has the mobjflag_e::MF_NOBLOCK flag set, it will not use
// the block links. It can still interact with other things,
// but only as the instigator (missiles will run into other
// things, but nothing can run into a missile).
// Each block in the grid is 128*128 units, and knows about
// every line_t that it contains a piece of, and every
// interactable MapObject that has its origin contained.
//
// A valid MapObject is a MapObject that has the proper subsector_t
// filled in for its xy coordinates and is linked into the
// sector from which the subsector was made, or has the
// mobjflag_e::MF_NOSECTOR flag set (the subsector_t needs to be valid
// even if mobjflag_e::MF_NOSECTOR is set), and is linked into a blockmap
// block or has the mobjflag_e::MF_NOBLOCKMAP flag set.
// Links should only be modified by the P_[Un]SetThingPosition()
// functions.
// Do not change the mobjflag_e::MF_NO? flags while a thing is valid.
//
// Any questions?

struct subsector_t;
class Player;

// Map Object definition.
class MapObject
{
public:
	// More list: links in sector (if needed)
	MapObject* sectorNext;
	MapObject* sectorPrev;

	// Interaction info, by BLOCKMAP. Links in blocks (if needed).
	MapObject* blockNext;
	MapObject* blockPrev;

	// Thing being chased/attacked (or NULL), also the originator for missiles.
	MapObject* target;

	// Thing being chased/attacked for tracers.
	MapObject* tracer;

	mobjinfo_t* info;		// &mobjinfo[mobj->type]

	subsector_t* subsector;

	state_t* state;

	int frame;				// might be ORed with FF_FULLBRIGHT

	// If == validcount, already checked.
	int validcount;

	int health;

	// Movement direction, movement generation (zig-zagging).
	dirtype_t movedir;		// 0-7
	int movecount;			// when 0, select a new dir

	// If >0, the target will be chased no matter what (even if shot)
	int threshold;

	// Additional info record for player avatars only. Only valid if type == mobjtype_t::MT_PLAYER
	//Player* player;

	// Player number last looked for.
	int lastlook;

	// If true, ok to interpolate this tic.
	int interp;

	// Previous position of mobj before think. Used to interpolate between positions.
	fixed_t oldx;
	fixed_t oldy;
	fixed_t oldz;

	// The closest interval over all contacted Sectors.
	fixed_t floorz;
	fixed_t ceilingz;

	// For movement checking.
	fixed_t radius;
	fixed_t height;

	// Momentums, used to update position.
	fixed_t momx;
	fixed_t momy;
	fixed_t momz;

	// Info for drawing: position.
	fixed_t x;
	fixed_t y;
	fixed_t z;

	//More drawing info: to determine current sprite.
	angle_t angle;			// orientation

	angle_t oldangle;

	TimeType tics;			// state tic counter
	// Reaction time: if non 0, don't attack yet. Used by player to freeze a bit after teleporting.
	TimeType reactiontime;

	mobjtype_t type;

	spritenum_t sprite;		// used to find patch_t and flip value

	// List: thinker links.
	thinker_t thinker;

	// For nightmare respawn.
	mapthing_t spawnpoint;

	cudadoom::BitSet<mobjflag_e, mobjflag_e::size> flags;
};
