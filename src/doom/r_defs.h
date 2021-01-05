/**********************************************************************************************************************************************\
	Copyright(C) 1993-1996 Id Software, Inc.
	Copyright(C) 2005-2014 Simon Howard

	This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License
	as published by the Free Software Foundation; either version 2 of the License, or (at your option) any later version.

	This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

	DESCRIPTION:
		Refresh/rendering module, shared data struct definitions.
\**********************************************************************************************************************************************/
#pragma once

#include "../../derma/common.h"

#include "doomdef.h"

#include "m_fixed.h"
#include "d_think.h"
#include "p_mobj.h"
#include "i_video.h"
#include "v_patch.h"

// Silhouette, needed for clipping Segs (mainly) and sprites representing things.
constexpr size_t SIL_NONE{0};
constexpr size_t SIL_BOTTOM{1};
constexpr size_t SIL_TOP{2};
constexpr size_t SIL_BOTH{3};

constexpr size_t MAXDRAWSEGS{256};

//
// INTERNAL MAP TYPES
// used by play and refresh
//

// Your plain vanilla vertex.
// Note: transformed values not buffered locally,
// like some DOOM-alikes ("wt", "WebView") did.
struct vertex_t
{
	fixed_t x;
	fixed_t y;

	// [crispy] remove slime trails
	// vertex coordinates *only* used in rendering that have been
	// moved towards the linedef associated with their seg by projecting them
	// using the law of cosines in p_setup.c:P_RemoveSlimeTrails();
	fixed_t r_x;
	fixed_t r_y;
	bool moved;
};

// Forward of LineDefs, for Sectors.
struct line_t;

// Each sector has a degenmobj_t in its center
// for sound origin purposes.
// I suppose this does not handle sound from
// moving objects (doppler), because
// position is prolly just buffered, not
// updated.
struct degenmobj_t
{
	thinker_t thinker;	// not used for anything
	fixed_t x;
	fixed_t y;
	fixed_t z;

};

// The SECTORS record, at runtime.
// Stores things/mobjs.
struct sector_t
{
	fixed_t floorheight;
	fixed_t ceilingheight;
	short floorpic;
	short ceilingpic;
	short lightlevel;
	short special;
	short tag;

	// 0 = untraversed, 1,2 = sndlines -1
	int soundtraversed;

	// thing that made a sound (or null)
	MapObject* soundtarget;

	// mapblock bounding box for height changes
	int blockbox[4];

	// origin for any sounds played by the sector
	degenmobj_t soundorg;

	// if == validcount, already checked
	int validcount;

	// list of mobjs in sector
	MapObject* thinglist;

	// thinker_t for reversable actions
	void* specialdata;

	int linecount;
	line_t** lines;	// [linecount] size

	// [crispy] WiggleFix: [kb] for R_FixWiggle()
	int cachedheight;
	int scaleindex;

	// [crispy] add support for MBF sky tranfers
	int sky;

	// [AM] Previous position of floor and ceiling before
	//		think. Used to interpolate between positions.
	fixed_t oldfloorheight;
	fixed_t oldceilingheight;

	// [AM] Gametic when the old positions were recorded.
	//		Has a dual purpose; it prevents movement thinkers
	//		from storing old positions twice in a tic, and
	//		prevents the renderer from attempting to interpolate
	//		if old values were not updated recently.
	TimeType oldgametic;

	// [AM] Interpolated floor and ceiling height.
	//		Calculated once per tic and used inside
	//		the renderer.
	fixed_t interpfloorheight;
	fixed_t interpceilingheight;

	// [crispy] revealed secrets
	short oldspecial;
};

// The SideDef.
struct side_t
{
	// add this to the calculated texture column
	fixed_t textureoffset;

	// add this to the calculated texture top
	fixed_t rowoffset;

	// Texture indices.
	// We do not maintain names here.
	short toptexture;
	short bottomtexture;
	short midtexture;

	// Sector the SideDef is facing.
	sector_t* sector;

	// [crispy] smooth texture scrolling
	fixed_t basetextureoffset;
};

// Move clipping aid for LineDefs.
enum class slopetype_t
{
	ST_HORIZONTAL,
	ST_VERTICAL,
	ST_POSITIVE,
	ST_NEGATIVE
};

struct line_t
{
	// Vertices, from v1 to v2.
	vertex_t* v1;
	vertex_t* v2;

	// Precalculated v2 - v1 for side checking.
	fixed_t dx;
	fixed_t dy;

	// Animation related.
	unsigned short flags; // [crispy] extended nodes
	short special;
	short tag;

	// Visual appearance: SideDefs.
	// sidenum[1] will be -1 (NO_INDEX) if one sided
	unsigned short sidenum[2]; // [crispy] extended nodes

	// Neat. Another bounding box, for the extent
	// of the LineDef.
	fixed_t bbox[4];

	// To aid move clipping.
	slopetype_t slopetype;

	// Front and back sector.
	// Note: redundant? Can be retrieved from SideDefs.
	sector_t* frontsector;
	sector_t* backsector;

	// if == validcount, already checked
	int validcount;

	// thinker_t for reversable actions
	void* specialdata;

	// [crispy] calculate sound origin of line to be its midpoint
	degenmobj_t soundorg;
};

// A SubSector.
// References a Sector.
// Basically, this is a list of LineSegs,
// indicating the visible walls that define
// (all or some) sides of a convex BSP leaf.
struct subsector_t
{
	sector_t* sector;
	int numlines; // [crispy] extended nodes
	int firstline; // [crispy] extended nodes
};

//
// The LineSeg.
//
struct seg_t
{
	vertex_t* v1;
	vertex_t* v2;

	fixed_t offset;

	angle_t angle;

	side_t* sidedef;
	line_t* linedef;

	// Sector references.
	// Could be retrieved from linedef, too.
	// backsector is NULL for one sided lines
	sector_t* frontsector;
	sector_t* backsector;

	uint32_t length; // [crispy] fix long wall wobble
	angle_t r_angle; // [crispy] re-calculated angle used for rendering
	int fakecontrast;
};

//
// BSP node.
//
struct node_t
{
	// Partition line.
	fixed_t x;
	fixed_t y;
	fixed_t dx;
	fixed_t dy;

	// Bounding box for each child.
	fixed_t bbox[2][4];

	// If NF_SUBSECTOR its a subsector.
	int children[2]; // [crispy] extended nodes
};

// PC direct to screen pointers
//B UNUSED - keep till detailshift in r_draw.c resolved
//extern byte* destview;
//extern byte* destscreen;

//
// OTHER TYPES
//

// This could be wider for >8 bit display.
// Indeed, true color support is posibble
// precalculating 24bpp lightmap/colormap LUT.
// from darkening PLAYPAL to all black.
// Could even us emore than 32 levels.

using lighttable_t = pixel_t;

// ?
struct drawseg_t
{
	seg_t* curline;
	int x1;
	int x2;

	fixed_t scale1;
	fixed_t scale2;
	fixed_t scalestep;

	// 0=none, 1=bottom, 2=top, 3=both
	int silhouette;

	// do not clip sprites above this
	fixed_t bsilheight;

	// do not clip sprites below this
	fixed_t tsilheight;

	// Pointers to lists for sprite clipping,
	// all three adjusted so [x1] is first value.
	int* sprtopclip; // [crispy] 32-bit integer math
	int* sprbottomclip; // [crispy] 32-bit integer math
	int* maskedtexturecol; // [crispy] 32-bit integer math
};

// A vissprite_t is a thing
// that will be drawn during a refresh.
// I.e. a sprite object that is partly visible.
struct vissprite_t
{
	// Doubly linked list.
	struct vissprite_t* prev;
	struct vissprite_t* next;

	int x1;
	int x2;

	// for line side calculation
	fixed_t gx;
	fixed_t gy;

	// global bottom / top for silhouette clipping
	fixed_t gz;
	fixed_t gzt;

	// horizontal position of x1
	fixed_t startfrac;

	fixed_t scale;

	// negative if flipped
	fixed_t xiscale;

	fixed_t texturemid;
	int patch;

	// for color translation and shadow draw,
	// maxbright frames as well
	// [crispy] brightmaps for select sprites
	lighttable_t* colormap[2];
	byte* brightmap;

	int mobjflags;
	// [crispy] color translation table for blood colored by monster class
	byte* translation;
#ifdef CRISPY_TRUECOLOR
	const pixel_t(*blendfunc)(const pixel_t fg, const pixel_t bg);
#endif
};

// Sprites are patches with a special naming convention
// so they can be recognized by R_InitSprites.
// The base name is NNNNFx or NNNNFxFx, with
// x indicating the rotation, x = 0, 1-7.
// The sprite and frame specified by a thing_t
// is range checked at run time.
// A sprite is a patch_t that is assumed to represent
// a three dimensional object and may have multiple
// rotations pre drawn.
// Horizontal flipping is used to save space,
// thus NNNNF2F5 defines a mirrored patch.
// Some sprites will only have one picture used
// for all views: NNNNF0
struct spriteframe_t
{
	// If false use 0 for any position.
	// Note: as eight entries are available,
	// we might as well insert the same name eight times.
	int rotate; // [crispy] we use a value of 2 for 16 sprite rotations

	// Lump to use for view angles 0-7.
	short lump[16]; // [crispy] support 16 sprite rotations

	// Flip bit (1 = flip) to use for view angles 0-7.
	byte flip[16]; // [crispy] support 16 sprite rotations
};

// A sprite definition:
// a number of animation frames.
struct spritedef_t
{
	int numframes;
	spriteframe_t* spriteframes;
};

// Now what is a visplane, anyway?
struct visplane_t
{
	fixed_t height;
	int picnum;
	int lightlevel;
	int minx;
	int maxx;

	// leave pads for [minx-1]/[maxx+1]
	unsigned pad1; // [crispy] hires / 32-bit integer math

	// Here lies the rub for all
	// dynamic resize/change of resolution.
	unsigned top[MAXWIDTH]; // [crispy] hires / 32-bit integer math
	unsigned pad2; // [crispy] hires / 32-bit integer math
	unsigned pad3; // [crispy] hires / 32-bit integer math

	// See above.
	unsigned bottom[MAXWIDTH]; // [crispy] hires / 32-bit integer math
	unsigned pad4; // [crispy] hires / 32-bit integer math
};

struct laserpatch_t
{
	char c;
	char a[9];
	int l;
	int w;
	int h;
};

extern laserpatch_t* laserpatch;
