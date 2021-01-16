/**********************************************************************************************************************************************\
	Copyright(C) 1993-1996 Id Software, Inc.
	Copyright(C) 2005-2014 Simon Howard

	This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License
	as published by the Free Software Foundation; either version 2 of the License, or (at your option) any later version.

	This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

	DESCRIPTION:
		Play functions, animation, global header.
\**********************************************************************************************************************************************/
#pragma once

#include "../../derma/stdafx.h"

#include "../../derma/common.h"

#include "doomdef.h"

#include "r_local.h"

#include "p_spec.h"

constexpr int TOCENTER{-8};
constexpr size_t AFLAG_JUMP{0x80};
constexpr size_t FLOATSPEED{(FRACUNIT * 4)};

constexpr size_t MAXHEALTH{100};
constexpr size_t VIEWHEIGHT{(41 * FRACUNIT)};

// mapblocks are used to check movement
// against lines and things
constexpr size_t MAPBLOCKUNITS{128};
constexpr size_t MAPBLOCKSIZE{(MAPBLOCKUNITS * FRACUNIT)};
constexpr size_t MAPBLOCKSHIFT{(FRACBITS + 7)};
constexpr size_t MAPBMASK{(MAPBLOCKSIZE - 1)};
constexpr size_t MAPBTOFRAC{(MAPBLOCKSHIFT - FRACBITS)};

// player radius for movement checking
constexpr size_t PLAYERRADIUS{16 * FRACUNIT};

// MAXRADIUS is for precalculated sector block boxes
// the spider demon is larger,
// but we do not have any moving sectors nearby
constexpr size_t MAXRADIUS{32 * FRACUNIT};

constexpr size_t GRAVITY{FRACUNIT};
constexpr size_t MAXMOVE{(30 * FRACUNIT)};

constexpr size_t USERANGE{(64 * FRACUNIT)};
constexpr size_t MELEERANGE{(64 * FRACUNIT)};
constexpr size_t MISSILERANGE{(32 * 64 * FRACUNIT)};

// follow a player exlusively for 3 seconds
constexpr size_t BASETHRESHOLD{100};

//
// P_TICK
//

// both the head and tail of the thinker list
extern thinker_t thinkercap;

void P_InitThinkers();
void P_AddThinker(thinker_t* thinker);
void P_RemoveThinker(thinker_t* thinker);

//
// P_PSPR
//
void P_SetupPsprites(Player* curplayer);
void P_MovePsprites(Player* curplayer);
void P_DropWeapon(Player* player);

//
// P_USER
//
constexpr size_t MLOOKUNIT{8};
#define PLAYER_SLOPE(a) ((((a)->lookdir / MLOOKUNIT) << FRACBITS) / 173)
void P_PlayerThink(Player* player);

//
// P_MOBJ
//
constexpr int ONFLOORZ{INT_MIN};
constexpr int ONCEILINGZ{INT_MAX};

// Time interval for item respawning.
constexpr size_t ITEMQUESIZE{128};

extern mapthing_t itemrespawnque[ITEMQUESIZE];
extern TimeType itemrespawntime[ITEMQUESIZE];
extern int iquehead;
extern int iquetail;

void P_RespawnSpecials();

MapObject* P_SpawnMobj(fixed_t x, fixed_t y, fixed_t z, mobjtype_t type);

void P_RemoveMobj(MapObject* th);
MapObject* P_SubstNullMobj(MapObject* th);
bool P_SetMobjState(MapObject* mobj, statenum_t state);
void P_MobjThinker(MapObject* mobj);
MapObject* Crispy_PlayerSO(int p); // weapon sound sources

void P_SpawnPuff(fixed_t x, fixed_t y, fixed_t z);
void P_SpawnBlood(fixed_t x, fixed_t y, fixed_t z, int damage, MapObject* target);
MapObject* P_SpawnMissile(MapObject* source, MapObject* dest, mobjtype_t type);
void P_SpawnPlayerMissile(MapObject* source, mobjtype_t type);

void P_SpawnPuffSafe(fixed_t x, fixed_t y, fixed_t z, bool safe);

//
// P_ENEMY
//
void P_NoiseAlert(MapObject* target, MapObject* emmiter);

//
// P_MAPUTL
//
struct divline_t
{
	fixed_t x;
	fixed_t y;
	fixed_t dx;
	fixed_t dy;
};

struct intercept_t
{
	fixed_t frac;		// along trace line
	bool isaline;
	union
	{
		MapObject* thing;
		line_t* line;
	} d;
};

// Extended MAXINTERCEPTS, to allow for intercepts overrun emulation.

constexpr size_t MAXINTERCEPTS_ORIGINAL{128};
constexpr size_t MAXINTERCEPTS{(MAXINTERCEPTS_ORIGINAL + 61)};

//extern intercept_t intercepts[MAXINTERCEPTS]; // remove INTERCEPTS limit
extern intercept_t* intercept_p;

typedef bool (*traverser_t) (intercept_t* in);

fixed_t P_AproxDistance(fixed_t dx, fixed_t dy);
int P_PointOnLineSide(fixed_t x, fixed_t y, line_t* line);
int P_PointOnDivlineSide(fixed_t x, fixed_t y, divline_t* line);
void P_MakeDivline(line_t* li, divline_t* dl);
fixed_t P_InterceptVector(divline_t* v2, divline_t* v1);
int P_BoxOnLineSide(fixed_t* tmbox, line_t* ld);

extern fixed_t opentop;
extern fixed_t openbottom;
extern fixed_t openrange;
extern fixed_t lowfloor;

void P_LineOpening(line_t* linedef);

bool P_BlockLinesIterator(int x, int y, bool(*func)(line_t*));
bool P_BlockThingsIterator(int x, int y, bool(*func)(MapObject*));

constexpr size_t PT_ADDLINES{1};
constexpr size_t PT_ADDTHINGS{2};
constexpr size_t PT_EARLYOUT{4};

extern divline_t trace;

bool P_PathTraverse(fixed_t x1, fixed_t y1, fixed_t x2, fixed_t y2, int flags, bool (*trav) (intercept_t*));

void P_UnsetThingPosition(MapObject* thing);
void P_SetThingPosition(MapObject* thing);

//
// P_MAP
//

// If "floatok" true, move would be ok
// if within "tmfloorz - tmceilingz".
extern bool floatok;
extern fixed_t tmfloorz;
extern fixed_t tmceilingz;

extern line_t* ceilingline;

// fraggle: I have increased the size of this buffer. In the original Doom,
// overrunning past this limit caused other bits of memory to be overwritten,
// affecting demo playback. However, in doing so, the limit was still
// exceeded. So we have to support more than 8 specials.
//
// We keep the original limit, to detect what variables in memory were
// overwritten (see SpechitOverrun())

constexpr size_t MAXSPECIALCROSS{20};
constexpr size_t MAXSPECIALCROSS_ORIGINAL{8};

extern line_t** spechit; // remove SPECHIT limit
extern int numspechit;

bool P_CheckPosition(MapObject* thing, fixed_t x, fixed_t y);
bool P_TryMove(MapObject* thing, fixed_t x, fixed_t y);
bool P_TeleportMove(MapObject* thing, fixed_t x, fixed_t y);
void P_SlideMove(MapObject* mo);
bool P_CheckSight(MapObject* t1, MapObject* t2);
void P_UseLines(Player* player);

bool P_ChangeSector(sector_t* sector, bool crunch);

extern MapObject* linetarget;	// who got hit (or NULL)

fixed_t P_AimLineAttack(MapObject* t1, angle_t angle, fixed_t distance);

void P_LineAttack(MapObject* t1, angle_t angle, fixed_t distance, fixed_t slope, int damage);

void P_RadiusAttack(MapObject* spot, MapObject* source, int damage);

//
// P_SETUP
//
extern byte* rejectmatrix;	// for fast sight rejection
extern int32_t* blockmaplump;	// offsets in blockmap are from here // BLOCKMAP limit
extern int32_t* blockmap; // BLOCKMAP limit
extern int bmapwidth;
extern int bmapheight;	// in mapblocks
extern fixed_t bmaporgx;
extern fixed_t bmaporgy;	// origin of block map
extern MapObject** blocklinks;	// for thing chains

// factor out map lump name and number finding into a separate function
extern int P_GetNumForMap(int episode, int map, bool critical);

// blinking key or skull in the status bar
constexpr size_t KEYBLINKMASK{0x8};
constexpr size_t KEYBLINKTICS{(7 * KEYBLINKMASK)};
extern int st_keyorskull[3];

//
// P_INTER
//
extern int maxammo[::std::size_t(AmmoType::NUMAMMO)];
extern int clipammo[::std::size_t(AmmoType::NUMAMMO)];

void P_TouchSpecialThing(MapObject* special, MapObject* toucher);

void P_DamageMobj(MapObject* target, MapObject* inflictor, MapObject* source, int damage);
