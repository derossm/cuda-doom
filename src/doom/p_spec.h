/**********************************************************************************************************************************************\
	Copyright(C) 1993-1996 Id Software, Inc.
	Copyright(C) 2005-2014 Simon Howard

	This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License
	as published by the Free Software Foundation; either version 2 of the License, or (at your option) any later version.

	This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

	DESCRIPTION:
		Implements special effects: Texture animation, height or lighting changes according to adjacent sectors, respective utility functions, etc.
\**********************************************************************************************************************************************/
#pragma once

#include "../../derma/common.h"

//
// End-level timer (-TIMER option)
//
extern bool levelTimer;
extern int levelTimeCount;

//		Define values for map objects
constexpr size_t MO_TELEPORTMAN{14};

// at game start
void P_InitPicAnims();

// at map load
void P_SpawnSpecials();

// every tic
void P_UpdateSpecials();

// when needed
bool P_UseSpecialLine(MapObject* thing, line_t* line, int side);

void P_ShootSpecialLine(MapObject* thing, line_t* line);

void P_CrossSpecialLine(int linenum, int side, MapObject* thing);

// [crispy] more MBF code pointers
void P_CrossSpecialLinePtr(line_t* line, int side, MapObject* thing);

void P_PlayerInSpecialSector(Player* player);

int twoSided(int sector, int line);

sector_t* getSector(int currentSector, int line, int side);

side_t* getSide(int currentSector, int line, int side);

fixed_t P_FindLowestFloorSurrounding(sector_t* sec);
fixed_t P_FindHighestFloorSurrounding(sector_t* sec);

fixed_t P_FindNextHighestFloor(sector_t* sec, int currentheight);

fixed_t P_FindLowestCeilingSurrounding(sector_t* sec);
fixed_t P_FindHighestCeilingSurrounding(sector_t* sec);

int P_FindSectorFromLineTag(line_t* line, int start);

int P_FindMinSurroundingLight(sector_t* sector, int max);

sector_t* getNextSector(line_t* line, sector_t* sec);

//
// SPECIAL
//
int EV_DoDonut(line_t* line);

//
// P_LIGHTS
//
struct fireflicker_t
{
	thinker_t thinker;
	sector_t* sector;
	int count;
	int maxlight;
	int minlight;
};

struct lightflash_t
{
	thinker_t thinker;
	sector_t* sector;
	int count;
	int maxlight;
	int minlight;
	TimeType maxtime;
	TimeType mintime;
};

struct strobe_t
{
	thinker_t thinker;
	sector_t* sector;
	int count;
	int minlight;
	int maxlight;
	TimeType darktime;
	TimeType brighttime;
};

struct glow_t
{
	thinker_t thinker;
	sector_t* sector;
	int minlight;
	int maxlight;
	int direction;

};

constexpr size_t GLOWSPEED{8};
constexpr size_t STROBEBRIGHT{5};
constexpr size_t FASTDARK{15};
constexpr size_t SLOWDARK{35};

void P_SpawnFireFlicker(sector_t* sector);
void T_LightFlash(lightflash_t* flash);
void P_SpawnLightFlash(sector_t* sector);
void T_StrobeFlash(strobe_t* flash);

void P_SpawnStrobeFlash(sector_t* sector, int fastOrSlow, int inSync);

void EV_StartLightStrobing(line_t* line);
void EV_TurnTagLightsOff(line_t* line);

void EV_LightTurnOn(line_t* line, int bright);

void T_Glow(glow_t* g);
void P_SpawnGlowingLight(sector_t* sector);

//
// P_SWITCH
//
// [crispy] add PACKEDATTR for reading SWITCHES lumps from memory
struct switchlist_t
{
	char name1[9];
	char name2[9];
	short episode;

};

enum class bwhere_e
{
	top,
	middle,
	bottom

};

struct button_t
{
	line_t* line;
	bwhere_e where;
	int btexture;
	TimeType btimer;
	degenmobj_t* soundorg;

};

// max # of wall switches in a level
constexpr size_t MAXSWITCHES{50};

// 4 players, 4 buttons each at once, max.
constexpr size_t MAXBUTTONS{16};

// 1 second, in ticks.
constexpr size_t BUTTONTIME{35};

extern button_t* buttonlist;
extern int maxbuttons;

void P_ChangeSwitchTexture(line_t* line, int useAgain);

void P_InitSwitchList();

//
// P_PLATS
//
enum class plat_e
{
	up,
	down,
	waiting,
	in_stasis
};

enum class plattype_e
{
	perpetualRaise,
	downWaitUpStay,
	raiseAndChange,
	raiseToNearestAndChange,
	blazeDWUS
};

struct plat_t
{
	thinker_t thinker;
	sector_t* sector;
	fixed_t speed;
	fixed_t low;
	fixed_t high;
	int wait;
	int count;
	plat_e status;
	plat_e oldstatus;
	bool crush;
	int tag;
	plattype_e type;
};

constexpr size_t PLATWAIT{3};
constexpr size_t PLATSPEED{FRACUNIT};
constexpr size_t MAXPLATS{30 * 256};

extern plat_t* activeplats[MAXPLATS];

void T_PlatRaise(plat_t* plat);

int EV_DoPlat(line_t* line, plattype_e type, int amount);

void P_AddActivePlat(plat_t* plat);
void P_RemoveActivePlat(plat_t* plat);
void EV_StopPlat(line_t* line);
void P_ActivateInStasis(int tag);

//
// P_DOORS
//
enum class vldoor_e
{
	vld_normal,
	vld_close30ThenOpen,
	vld_close,
	vld_open,
	vld_raiseIn5Mins,
	vld_blazeRaise,
	vld_blazeOpen,
	vld_blazeClose
};

struct vldoor_t
{
	thinker_t thinker;
	vldoor_e type;
	sector_t* sector;
	fixed_t topheight;
	fixed_t speed;

	// 1 = up, 0 = waiting at top, -1 = down
	int direction;

	// tics to wait at the top
	int topwait;
	// (keep in case a door going down is reset)
	// when it reaches 0, start going down
	int topcountdown;
};

constexpr size_t VDOORSPEED{FRACUNIT * 2};
constexpr size_t VDOORWAIT{150};

void EV_VerticalDoor(line_t* line, MapObject* thing);

int EV_DoDoor(line_t* line, vldoor_e type);

int EV_DoLockedDoor(line_t* line, vldoor_e type, MapObject* thing);

void T_VerticalDoor(vldoor_t* door);
void P_SpawnDoorCloseIn30(sector_t* sec);

void P_SpawnDoorRaiseIn5Mins(sector_t* sec, int secnum);

#if 0 // UNUSED
//
//		Sliding doors...
//
enum class sd_e
{
	sd_opening,
	sd_waiting,
	sd_closing
};

enum class sdt_e
{
	sdt_openOnly,
	sdt_closeOnly,
	sdt_openAndClose
};

struct slidedoor_t
{
	thinker_t thinker;
	sdt_e type;
	line_t* line;
	int frame;
	int whichDoorIndex;
	TimeType timer;
	sector_t* frontsector;
	sector_t* backsector;
	sd_e status;
};

struct slidename_t
{
	char frontFrame1[9];
	char frontFrame2[9];
	char frontFrame3[9];
	char frontFrame4[9];
	char backFrame1[9];
	char backFrame2[9];
	char backFrame3[9];
	char backFrame4[9];
};

struct slideframe_t
{
	int frontFrames[4];
	int backFrames[4];
};

// how many frames of animation
constexpr size_t SNUMFRAMES{4};

constexpr size_t SDOORWAIT{35 * 3};
constexpr size_t SWAITTICS{4};

// how many diff. types of anims
constexpr size_t MAXSLIDEDOORS{5};

void P_InitSlidingDoorFrames();

void EV_SlidingDoor(line_t* line, MapObject* thing);
#endif

//
// P_CEILNG
//
enum class ceiling_e
{
	lowerToFloor,
	raiseToHighest,
	lowerAndCrush,
	crushAndRaise,
	fastCrushAndRaise,
	silentCrushAndRaise
};

struct ceiling_t
{
	thinker_t thinker;
	ceiling_e type;
	sector_t* sector;
	fixed_t bottomheight;
	fixed_t topheight;
	fixed_t speed;
	bool crush;

	// 1 = up, 0 = waiting, -1 = down
	int direction;

	// ID
	int tag;
	int olddirection;

};

constexpr size_t CEILSPEED{FRACUNIT};
constexpr size_t CEILWAIT{150};
constexpr size_t MAXCEILINGS{30};

extern ceiling_t* activeceilings[MAXCEILINGS];

int EV_DoCeiling(line_t* line, ceiling_e type);

void T_MoveCeiling(ceiling_t* ceiling);
void P_AddActiveCeiling(ceiling_t* c);
void P_RemoveActiveCeiling(ceiling_t* c);
int EV_CeilingCrushStop(line_t* line);
void P_ActivateInStasisCeiling(line_t* line);

//
// P_FLOOR
//
enum class floor_e
{
	// lower floor to highest surrounding floor
	lowerFloor,

	// lower floor to lowest surrounding floor
	lowerFloorToLowest,

	// lower floor to highest surrounding floor VERY FAST
	turboLower,

	// raise floor to lowest surrounding CEILING
	raiseFloor,

	// raise floor to next highest surrounding floor
	raiseFloorToNearest,

	// raise floor to shortest height texture around it
	raiseToTexture,

	// lower floor to lowest surrounding floor
	// and change floorpic
	lowerAndChange,

	raiseFloor24,
	raiseFloor24AndChange,
	raiseFloorCrush,

	// raise to next highest floor, turbo-speed
	raiseFloorTurbo,
	donutRaise,
	raiseFloor512
};

enum class stair_e
{
	build8,	// slowly build by 8
	turbo16	// quickly build by 16

};

struct floormove_t
{
	thinker_t thinker;
	floor_e type;
	bool crush;
	sector_t* sector;
	int direction;
	int newspecial;
	short texture;
	fixed_t floordestheight;
	fixed_t speed;
};

constexpr size_t FLOORSPEED{FRACUNIT};

enum class result_e
{
	ok,
	crushed,
	pastdest
};

result_e T_MovePlane(sector_t* sector, fixed_t speed, fixed_t dest, bool crush, int floorOrCeiling, int direction);

int EV_BuildStairs(line_t* line, stair_e type);

int EV_DoFloor(line_t* line, floor_e floortype);

void T_MoveFloor(floormove_t* floor);

//
// P_TELEPT
//
int EV_Teleport(line_t* line, int side, MapObject* thing);
