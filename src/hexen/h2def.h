/**********************************************************************************************************************************************\
	Copyright(C) 1993-1996 Id Software, Inc.
	Copyright(C) 1993-2008 Raven Software
	Copyright(C) 2005-2014 Simon Howard

	This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License
	as published by the Free Software Foundation; either version 2 of the License, or (at your option) any later version.

	This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
\**********************************************************************************************************************************************/
#pragma once

#include "../../derma/common.h"

#include "st_start.h"
#include "d_ticcmd.h"
#include "d_event.h"
#include "d_mode.h"
#include "m_fixed.h"
#include "tables.h"
#include "d_loop.h"
#include "net_defs.h"

#define HEXEN_VERSION			110
#define HEXEN_VERSION_TEXT		"v1.1"

// if rangecheck is undefined, most parameter validation debugging code will not be compiled
#ifndef NORANGECHECKING
#define RANGECHECK
#endif

inline constexpr auto HEXEN_VERSIONTEXT{
	[&]()->auto
	{
		if constexpr (gamemode == GameMode_t::shareware)
		{
			return "DEMO 10 16 95"
		}
		else
		{
			return "VERSION 1.1 MAR 22 1996 (BCP)";
		}
	}
()};

#include "xddefs.h"
#include "textdefs.h"
#include "info.h"

/*
===============================================================================

						GLOBAL TYPES

===============================================================================
*/
inline constexpr size_t MAX_HINT_SIZE{80ul};
inline constexpr size_t MAX_PLAYERS{8ul};

inline constexpr size_t BT_ATTACK{1ul};
inline constexpr size_t BT_USE{2ul};
inline constexpr size_t BT_CHANGE{4ul};						// if true, the next 3 bits hold weapon num

inline constexpr size_t BT_WEAPON_MASK{8ul + 16ul + 32ul};
inline constexpr size_t BT_WEAPON_SHIFT{3ul};

inline constexpr size_t BT_SPECIAL{128};					// game events, not really buttons
inline constexpr size_t BT_SPECIAL_MASK{3ul};

inline constexpr size_t BTS_SAVE_MASK{4ul + 8ul + 16ul};
inline constexpr size_t BTS_SAVE_SHIFT{2ul};

inline constexpr size_t BTS_PAUSE{1ul};						// pause the game
inline constexpr size_t BTS_SAVE_GAME{2ul};					// save the game at each console, savegame slot numbers occupy the second byte of buttons

inline constexpr size_t MAX_SPECIAL_ARGS{5};

// The top 3 bits of the artifact field in the ticcmd_t struct are used as additional flags
#define AFLAG_MASK			0x3F
#define AFLAG_SUICIDE		0x40
#define AFLAG_JUMP			0x80

enum class GameState_t
{
	GS_LEVEL,
	GS_INTERMISSION,
	GS_FINALE,
	GS_DEMOSCREEN
};

enum class GameAction_t
{
	ga_nothing,
	ga_loadlevel,
	ga_initnew,
	ga_newgame,
	ga_loadgame,
	ga_savegame,
	ga_playdemo,
	ga_completed,
	ga_leavemap,
	ga_singlereborn,
	ga_victory,
	ga_worlddone,
	ga_screenshot
};

enum class Wipe_t
{
	wipe_0,
	wipe_1,
	wipe_2,
	wipe_3,
	wipe_4,
	NUMWIPES,
	wipe_random
};

/*
===============================================================================

							MAPOBJ DATA

===============================================================================
*/

// think_t is a function pointer to a routine to handle an actor
typedef void (*think_t) ();

struct thinker_t
{
	thinker_t* prev;
	thinker_t* next;
	think_t function;
};

struct player_t;

union specialval_t
{
	intptr_t i;
	mobj_t* m;
	player_t* p;
};

struct mobj_t;

struct mobj_t
{
	thinker_t thinker;				// thinker node

// info for drawing
	fixed_t x;
	fixed_t y;
	fixed_t z;
	mobj_t* snext;					// links in sector (if needed)
	mobj_t* sprev;
	angle_t angle;
	spritenum_t sprite;				// used to find patch_t and flip value
	int frame;						// might be ord with FF_FULLBRIGHT

// interaction info
	mobj_t* bnext;					// links in blocks (if needed)
	mobj_t* bprev;
	subsector_t* subsector;
	fixed_t floorz;					// closest together of contacted secs
	fixed_t ceilingz;
	fixed_t floorpic;				// contacted sec floorpic
	fixed_t radius;					// for movement checking
	fixed_t height;
	fixed_t momx;					// momentums
	fixed_t momy;
	fixed_t momz;
	int validcount;					// if == validcount, already checked
	mobjtype_t type;
	mobjinfo_t* info;				// &mobjinfo[mobj->type]
	int tics;						// state tic counter
	state_t* state;
	int damage;						// For missiles
	int flags;
	int flags2;						// Heretic flags
	specialval_t special1;			// Special info
	specialval_t special2;			// Special info
	int health;
	int movedir;					// 0-7
	int movecount;					// when 0, select a new dir
	mobj_t* target;					// thing being chased/attacked (or NULL) also the originator for missiles
	int reactiontime;				// if non 0, don't attack yet used by player to freeze a bit after teleporting
	int threshold;					// if > 0, the target will be chased no matter what (even if shot)
	player_t* player;				// only valid if type == mobjtype_t::MT_PLAYER
	int lastlook;					// player number last looked for
	fixed_t floorclip;				// value to use for floor clipping
	int archiveNum;					// Identity during archive
	short tid;						// thing identifier
	byte special;					// special
	byte args[MAX_SPECIAL_ARGS];	// special arguments
};

// each sector has a degenmobj_t in it's center for sound origin purposes
struct degenmobj_t
{
	thinker_t thinker;			// not used for anything
	fixed_t x;
	fixed_t y;
	fixed_t z;
};

//
// frame flags
//
#define FF_FULLBRIGHT	0x8000 // flag in thing->frame
#define FF_FRAMEMASK	0x7fff

// --- mobj.flags ---

#define MF_SPECIAL		1		// call P_SpecialThing when touched
#define MF_SOLID		2
#define MF_SHOOTABLE	4
#define MF_NOSECTOR		8		// don't use the sector links
																		// (invisible but touchable)
#define MF_NOBLOCKMAP	16		// don't use the blocklinks
																		// (inert but displayable)
#define MF_AMBUSH		32
#define MF_JUSTHIT		64		// try to attack right back
#define MF_JUSTATTACKED	128		// take at least one step before attacking
#define MF_SPAWNCEILING	256		// hang from ceiling instead of floor
#define MF_NOGRAVITY	512		// don't apply gravity every tic

// movement flags
#define MF_DROPOFF		0x400	// allow jumps from high places
#define MF_PICKUP		0x800	// for players to pick up items
#define MF_NOCLIP		0x1000 // player cheat
#define MF_SLIDE		0x2000 // keep info about sliding along walls
#define MF_FLOAT		0x4000 // allow moves to any height, no gravity
#define MF_TELEPORT		0x8000 // don't cross lines or look at heights
#define MF_MISSILE		0x10000 // don't hit same species, explode on block

#define MF_ALTSHADOW	0x20000 // alternate translucent draw
#define MF_SHADOW		0x40000 // use translucent draw (shadow demons / invis)
#define MF_NOBLOOD		0x80000 // don't bleed when shot (use puff)
#define MF_CORPSE		0x100000		// don't stop moving halfway off a step
#define MF_INFLOAT		0x200000		// floating to a height for a move, don't
																		// auto float to target's height

#define MF_COUNTKILL	0x400000		// count towards intermission kill total
#define MF_ICECORPSE	0x800000		// a frozen corpse (for blasting)

#define MF_SKULLFLY		0x1000000		// skull in flight
#define MF_NOTDMATCH	0x2000000		// don't spawn in death match (key cards)

//#define MF_TRANSLATION 0xc000000		// if 0x4 0x8 or 0xc, use a translation
#define MF_TRANSLATION	0x1c000000		// use a translation table (>>MF_TRANSHIFT)
#define MF_TRANSSHIFT	26		// table for player colormaps


// --- mobj.flags2 ---

#define MF2_LOGRAV			0x00000001		// alternate gravity setting
#define MF2_WINDTHRUST		0x00000002		// gets pushed around by the wind
																				// specials
#define MF2_FLOORBOUNCE		0x00000004		// bounces off the floor
#define MF2_BLASTED			0x00000008		// missile will pass through ghosts
#define MF2_FLY				0x00000010		// fly mode is active
#define MF2_FLOORCLIP		0x00000020		// if feet are allowed to be clipped
#define MF2_SPAWNFLOAT		0x00000040		// spawn random float z
#define MF2_NOTELEPORT		0x00000080		// does not teleport
#define MF2_RIP				0x00000100		// missile rips through solid
																				// targets
#define MF2_PUSHABLE		0x00000200		// can be pushed by other moving
																				// mobjs
#define MF2_SLIDE			0x00000400		// slides against walls
#define MF2_ONMOBJ			0x00000800		// mobj is resting on top of another
																				// mobj
#define MF2_PASSMOBJ		0x00001000		// Enable z block checking. If on,
																				// this flag will allow the mobj to
																				// pass over/under other mobjs.
#define MF2_CANNOTPUSH		0x00002000		// cannot push other pushable mobjs
#define MF2_DROPPED			0x00004000		// dropped by a demon
#define MF2_BOSS			0x00008000		// mobj is a major boss
#define MF2_FIREDAMAGE		0x00010000		// does fire damage
#define MF2_NODMGTHRUST		0x00020000		// does not thrust target when
																				// damaging
#define MF2_TELESTOMP		0x00040000		// mobj can stomp another
#define MF2_FLOATBOB		0x00080000		// use float bobbing z movement
#define MF2_DONTDRAW		0x00100000		// don't generate a vissprite
#define MF2_IMPACT			0x00200000		// an MF_MISSILE mobj can activate
																				// SPAC_IMPACT
#define MF2_PUSHWALL		0x00400000		// mobj can push walls
#define MF2_MCROSS			0x00800000		// can activate monster cross lines
#define MF2_PCROSS			0x01000000		// can activate projectile cross lines
#define MF2_CANTLEAVEFLOORPIC 0x02000000		// stay within a certain floor type
#define MF2_NONSHOOTABLE	0x04000000		// mobj is totally non-shootable,
																				// but still considered solid
#define MF2_INVULNERABLE	0x08000000		// mobj is invulnerable
#define MF2_DORMANT			0x10000000		// thing is dormant
#define MF2_ICEDAMAGE		0x20000000		// does ice damage
#define MF2_SEEKERMISSILE	0x40000000		// is a seeker (for reflection)
#define MF2_REFLECTIVE		0x80000000		// reflects missiles

//=============================================================================

// ===== Player Class Types =====
enum class pclass_t
{
	PCLASS_FIGHTER,
	PCLASS_CLERIC,
	PCLASS_MAGE,
	PCLASS_PIG,
	NUMCLASSES
};

enum class PlayerState_t
{
	PST_LIVE,					// playing
	PST_DEAD,					// dead on the ground
	PST_REBORN					// ready to restart
};

// psprites are scaled shapes directly on the view screen
// coordinates are given for a 320*200 view screen
enum class psprnum_t
{
	ps_weapon,
	ps_flash,
	NUMPSPRITES
};

struct pspdef_t
{
	state_t* state;				// a NULL state means not active
	int tics;
	fixed_t sx;
	fixed_t sy;
};

enum class KeyType_t
{
	KEY_1,
	KEY_2,
	KEY_3,
	KEY_4,
	KEY_5,
	KEY_6,
	KEY_7,
	KEY_8,
	KEY_9,
	KEY_A,
	KEY_B,
	NUMKEYS
};

enum class ArmorType_t
{
	ARMOR_ARMOR,
	ARMOR_SHIELD,
	ARMOR_HELMET,
	ARMOR_AMULET,
	NUMARMOR
};

enum class WeaponType_t
{
	WP_FIRST,
	WP_SECOND,
	WP_THIRD,
	WP_FOURTH,
	NUMWEAPONS,
	WP_NOCHANGE
};

enum class ManaType_t
{
	MANA_1,
	MANA_2,
	NUMMANA,
	MANA_BOTH,
	MANA_NONE
};

#define MAX_MANA	200

#define WPIECE1		1
#define WPIECE2		2
#define WPIECE3		4

struct weaponinfo_t
{
	ManaType_t mana;
	int upstate;
	int downstate;
	int readystate;
	int atkstate;
	int holdatkstate;
	int flashstate;
};

extern weaponinfo_t WeaponInfo[(size_t)WeaponType_t::NUMWEAPONS][(size_t)pclass_t::NUMCLASSES];

enum class ArtiType_t
{
	arti_none,
	arti_invulnerability,
	arti_health,
	arti_superhealth,
	arti_healingradius,
	arti_summon,
	arti_torch,
	arti_egg,
	arti_fly,
	arti_blastradius,
	arti_poisonbag,
	arti_teleportother,
	arti_speed,
	arti_boostmana,
	arti_boostarmor,
	arti_teleport,
	// Puzzle artifacts
	arti_firstpuzzitem,
	arti_puzzskull = ArtiType_t::arti_firstpuzzitem,
	arti_puzzgembig,
	arti_puzzgemred,
	arti_puzzgemgreen1,
	arti_puzzgemgreen2,
	arti_puzzgemblue1,
	arti_puzzgemblue2,
	arti_puzzbook1,
	arti_puzzbook2,
	arti_puzzskull2,
	arti_puzzfweapon,
	arti_puzzcweapon,
	arti_puzzmweapon,
	arti_puzzgear1,
	arti_puzzgear2,
	arti_puzzgear3,
	arti_puzzgear4,
	NUMARTIFACTS
};

enum class PowerType_t
{
	pw_None,
	pw_invulnerability,
	pw_allmap,
	pw_infrared,
	pw_flight,
	pw_shield,
	pw_health2,
	pw_speed,
	pw_minotaur,
	NUMPOWERS
};

inline constexpr size_t INVULN_TICS{30*35};
inline constexpr size_t INVIS_TICS{60*35};
inline constexpr size_t INFRA_TICS{120*35};
inline constexpr size_t IRON_TICS{60*35};
inline constexpr size_t WPNLEV2_TICS{40*35};
inline constexpr size_t FLIGHT_TICS{60*35};
inline constexpr size_t SPEED_TICS{45*35};
inline constexpr size_t MORPH_TICS{40*35};
inline constexpr size_t MAULATOR_TICS{25*35};
inline constexpr size_t MESSAGE_TICS{4*35};
inline constexpr size_t BLINK_THRESHOLD{4*35};

inline constexpr size_t NUM_WEAPONS{(size_t)WeaponType_t::NUMWEAPONS};
inline constexpr size_t NUM_ARMOR{(size_t)ArmorType_t::NUMARMOR};
inline constexpr size_t NUM_MANA{(size_t)ManaType_t::NUMMANA};
inline constexpr size_t NUM_ARTIFACTS{(size_t)ArtiType_t::NUMARTIFACTS};
inline constexpr size_t NUM_INVENTORY_SLOTS{(size_t)ArtiType_t::NUMARTIFACTS};
inline constexpr size_t NUM_POWERS{(size_t)PowerType_t::NUMPOWERS};
inline constexpr size_t NUM_P_SPRITES{(size_t)psprnum_t::NUMPSPRITES};

struct inventory_t
{
	int type;
	int count;
};

/*
================
=
= player_t
=
================
*/

struct player_t
{
	mobj_t* mo;
	PlayerState_t playerstate;
	ticcmd_t cmd;

	pclass_t playerClass;						// player class type

	fixed_t viewz;								// focal origin above r.z
	fixed_t viewheight;							// base height above floor for viewz
	fixed_t deltaviewheight;					// squat speed
	fixed_t bob;								// bounded/scaled total momentum

	int flyheight;
	int lookdir;
	bool centering;
	int health;									// only used between levels, mo->health is used during levels
	int armorpoints[NUM_ARMOR];

	inventory_t inventory[NUM_INVENTORY_SLOTS];
	ArtiType_t readyArtifact;
	int artifactCount;
	int inventorySlotNum;
	int powers[NUM_POWERS];
	int keys;
	int pieces;									// Fourth Weapon pieces
	signed int frags[MAX_PLAYERS];				// kills of other players
	WeaponType_t readyweapon;
	WeaponType_t pendingweapon;					// wp_nochange if not changing
	bool weaponowned[NUM_WEAPONS];
	int mana[NUM_MANA];
	int attackdown;								// true if button down last tic
	int usedown;
	int cheats;									// bit flags

	int refire;									// refired shots are less accurate

	int killcount;								// for intermission
	int itemcount;
	int secretcount;
	char message[MAX_HINT_SIZE];				// hint messages
	int messageTics;							// counter for showing messages
	short ultimateMessage;
	short yellowMessage;
	int damagecount;							// for screen flashing
	int bonuscount;
	int poisoncount;							// screen flash for poison damage
	mobj_t* poisoner;							// NULL for non-player mobjs
	mobj_t* attacker;							// who did damage (NULL for floors)
	int extralight;								// so gun flashes light up areas
	int fixedcolormap;							// can be set to REDCOLORMAP, etc
	int colormap;								// 0-3 for which color to draw player
	pspdef_t psprites[NUM_P_SPRITES];			// view sprites (gun, etc)
	int morphTics;								// player is a pig if > 0
	unsigned int jumpTics;						// delay the next jump for a moment
	unsigned int worldTimer;					// total time the player's been playing
};

#define CF_NOCLIP		1
#define CF_GODMODE		2
#define CF_NOMOMENTUM	4						// not really a cheat, just a debug aid

#define SBARHEIGHT		(39 << crispy->hires)	// status bar height at bottom of screen

void NET_SendFrags(player_t* player);

/*
===============================================================================

					GLOBAL VARIABLES

===============================================================================
*/

#define TELEFOGHEIGHT (32*FRACUNIT)

extern GameMode_t gamemode;					// Always commercial

extern GameAction_t gameaction;

extern bool paused;

extern bool DevMaps;						// true = map development mode
extern char *DevMapsDir;					// development maps directory

extern bool nomonsters;						// checkparm of -nomonsters

extern bool respawnparm;					// checkparm of -respawn

extern bool randomclass;					// checkparm of -randclass

extern bool debugmode;						// checkparm of -debug

extern bool usergame;						// ok to save / end game

extern bool ravpic;							// checkparm of -ravpic

extern bool altpal;							// checkparm to use an alternate palette routine

extern bool cdrom;							// true if cd-rom mode active ("-cdrom")

extern bool deathmatch;						// only if started as net death

extern bool netgame;						// only true if >1 player

extern bool cmdfrag;						// true if a CMD_FRAG packet should be sent out every kill

extern bool playeringame[MAX_PLAYERS];
extern pclass_t PlayerClass[MAX_PLAYERS];

extern int consoleplayer;					// player taking events and displaying

extern int displayplayer;

extern int viewangleoffset;					// ANG90 = left side, ANG270 = right

extern player_t players[MAX_PLAYERS];

extern bool DebugSound;						// debug flag for displaying sound info

extern bool demoplayback;
extern bool demoextend;						// allow demos to persist through exit/respawn
extern int maxzone;							// Maximum chunk allocated for zone heap

// Truncate angleturn in ticcmds to nearest 256.
// Used when recording Vanilla demos in netgames.
extern bool lowres_turn;

extern int Sky1Texture;
extern int Sky2Texture;

extern GameState_t gamestate;
extern skill_t gameskill;
extern int gameepisode;
extern int gamemap;
extern int prevmap;
extern int levelstarttic;		// gametic at level start
extern int leveltime;			// tics in game play for par

extern ticcmd_t *netcmds;

#define MAXDEATHMATCHSTARTS 16
extern mapthing_t *deathmatch_p;
extern mapthing_t deathmatchstarts[MAXDEATHMATCHSTARTS];

// Position indicator for cooperative net-play reborn
extern int RebornPosition;

#define MAX_PLAYER_STARTS 8
extern mapthing_t playerstarts[MAX_PLAYER_STARTS][MAX_PLAYERS];
extern int maxplayers;

extern int mouseSensitivity;

extern bool precache;		// if true, load all graphics at level load

extern byte* screen;		// off screen work buffer, from V_video.c

extern bool singledemo;		// quit after playing a demo from cmdline

extern int bodyqueslot;
extern skill_t startskill;
extern int startepisode;
extern int startmap;
extern bool autostart;

extern bool testcontrols;
extern int testcontrols_mousespeed;

extern int vanilla_savegame_limit;
extern int vanilla_demo_limit;

/*
===============================================================================

					GLOBAL FUNCTIONS

===============================================================================
*/

#include "w_wad.h"
#include "z_zone.h"

//----------
//BASE LEVEL
//----------
void H2_Main();
// not a globally visible function, just included for source reference
// calls all startup code
// parses command line options
// if not overrided, calls N_AdvanceDemo

void H2_GameLoop();
// not a globally visible function, just included for source reference
// called by H2_Main, never exits
// manages timing and IO
// calls all ?_Responder, ?_Ticker, and ?_Drawer functions
// calls I_GetTime, I_StartFrame, and I_StartTic

//---------
//SYSTEM IO
//---------
byte* I_AllocLow(int length);
// allocates from low memory under dos, just mallocs under unix

// haleyjd: was WATCOMC, again preserved for historical interest as in Heretic
#if 0
extern bool useexterndriver;

#define EBT_FIRE			1
#define EBT_OPENDOOR		2
#define EBT_SPEED			4
#define EBT_STRAFE			8
#define EBT_MAP				0x10
#define EBT_INVENTORYLEFT	0x20
#define EBT_INVENTORYRIGHT	0x40
#define EBT_USEARTIFACT		0x80
#define EBT_FLYDROP			0x100
#define EBT_CENTERVIEW		0x200
#define EBT_PAUSE			0x400
#define EBT_WEAPONCYCLE		0x800
#define EBT_JUMP			0x1000

typedef struct
{
	short vector;				// Interrupt vector

	signed char moveForward;	// forward/backward (maxes at 50)
	signed char moveSideways;	// strafe (maxes at 24)
	short angleTurn;			// turning speed (640 [slow] 1280 [fast])
	short angleHead;			// head angle (+2080 [left] : 0 [center] : -2048 [right])
	signed char pitch;			// look up/down (-110 : +90)
	signed char flyDirection;	// flyheight (+1/-1)
	unsigned short buttons;		// EBT_* flags
} externdata_t;
#endif

//----
//GAME
//----

void G_DeathMatchSpawnPlayer(int playernum);

void G_InitNew(skill_t skill, int episode, int map);

void G_DeferedInitNew(skill_t skill, int episode, int map);
// can be called by the startup code or M_Responder
// a normal game starts at map 1, but a warp test can start elsewhere

void G_DeferredNewGame(skill_t skill);

void G_DeferedPlayDemo(const char* demo);

void G_LoadGame(int slot);
// can be called by the startup code or M_Responder
// calls P_SetupLevel or W_EnterWorld
void G_DoLoadGame();

void G_SaveGame(int slot, char* description);
// called by M_Responder

void G_RecordDemo(skill_t skill, int numplayers, int episode, int map, const char* name);
// only called by startup code

void G_PlayDemo(char* name);
void G_TimeDemo(char* name);

void G_TeleportNewMap(int map, int position);

void G_Completed(int map, int position);
//void G_ExitLevel ();
//void G_SecretExitLevel ();

void G_StartNewGame(skill_t skill);
void G_StartNewInit();

void G_WorldDone();

void G_Ticker();
bool G_Responder(event_t* ev);

void G_ScreenShot();

//-------
//SV_SAVE
//-------

#define HXS_VERSION_TEXT "HXS Ver 2.37"
#define HXS_VERSION_TEXT_LENGTH 16
#define HXS_DESCRIPTION_LENGTH 24

extern char* SavePath;

void SV_SaveGame(int slot, const char* description);
void SV_SaveMap(bool savePlayers);
void SV_LoadGame(int slot);
void SV_MapTeleport(int map, int position);
void SV_LoadMap();
void SV_InitBaseSlot();
void SV_UpdateRebornSlot();
void SV_ClearRebornSlot();
bool SV_RebornSlotAvailable();
int SV_GetRebornSlot();

//-----
//PLAY
//-----

void P_Ticker();
// called by C_Ticker
// can call G_PlayerExited
// carries out all thinking of monsters and players

void P_SetupLevel(int episode, int map, int playermask, skill_t skill);
// called by W_Ticker

void P_Init();
// called by startup code

int P_GetMapCluster(int map);
int P_TranslateMap(int map);
int P_GetMapCDTrack(int map);
int P_GetMapWarpTrans(int map);
int P_GetMapNextMap(int map);
int P_GetMapSky1Texture(int map);
int P_GetMapSky2Texture(int map);
char *P_GetMapName(int map);
fixed_t P_GetMapSky1ScrollDelta(int map);
fixed_t P_GetMapSky2ScrollDelta(int map);
bool P_GetMapDoubleSky(int map);
bool P_GetMapLightning(int map);
bool P_GetMapFadeTable(int map);
char *P_GetMapSongLump(int map);
void P_PutMapSongLump(int map, char* lumpName);
int P_GetCDStartTrack();
int P_GetCDEnd1Track();
int P_GetCDEnd2Track();
int P_GetCDEnd3Track();
int P_GetCDIntermissionTrack();
int P_GetCDTitleTrack();

//-------
//REFRESH
//-------

extern bool setsizeneeded;

extern bool BorderNeedRefresh;
extern bool BorderTopRefresh;

extern int UpdateState;
// define the different areas for the dirty map
#define I_NOUPDATE	0
#define I_FULLVIEW	1
#define I_STATBAR	2
#define I_MESSAGES	4
#define I_FULLSCRN	8

void R_RenderPlayerView(player_t* player);
// called by G_Drawer

void R_Init();
// called by startup code

void R_DrawViewBorder();
void R_DrawTopBorder();
// if the view size is not full screen, draws a border around it

void R_SetViewSize(int blocks, int detail);
// called by M_Responder

int R_FlatNumForName(const char* name);

int R_TextureNumForName(const char* name);
int R_CheckTextureNumForName(const char* name);
// called by P_Ticker for switches and animations
// returns the texture number for the texture name

//----
//MISC
//----
extern int localQuakeHappening[MAX_PLAYERS];

int M_DrawText(int x, int y, bool direct, char* string);

//------------------------------
// SC_man.c
//------------------------------

void SC_Open(const char* name);
void SC_OpenLump(const char* name);
void SC_OpenFile(const char* name);
void SC_Close();
bool SC_GetString();
void SC_MustGetString();
void SC_MustGetStringName(char* name);
bool SC_GetNumber();
void SC_MustGetNumber();
void SC_UnGet();
//bool SC_Check();
bool SC_Compare(const char* text);
int SC_MatchString(const char** strings);
int SC_MustMatchString(const char** strings);
void SC_ScriptError(const char* message);

extern char* sc_String;
extern int sc_Number;
extern int sc_Line;
extern bool sc_End;
extern bool sc_Crossed;
extern bool sc_FileScripts;
extern const char* sc_ScriptsDir;

//------------------------------
// SN_sonix.c
//------------------------------

enum
{
	SEQ_PLATFORM,
	SEQ_PLATFORM_HEAVY,			// same script as a normal platform
	SEQ_PLATFORM_METAL,
	SEQ_PLATFORM_CREAK,			// same script as a normal platform
	SEQ_PLATFORM_SILENCE,
	SEQ_PLATFORM_LAVA,
	SEQ_PLATFORM_WATER,
	SEQ_PLATFORM_ICE,
	SEQ_PLATFORM_EARTH,
	SEQ_PLATFORM_METAL2,
	SEQ_DOOR_STONE,
	SEQ_DOOR_HEAVY,
	SEQ_DOOR_METAL,
	SEQ_DOOR_CREAK,
	SEQ_DOOR_SILENCE,
	SEQ_DOOR_LAVA,
	SEQ_DOOR_WATER,
	SEQ_DOOR_ICE,
	SEQ_DOOR_EARTH,
	SEQ_DOOR_METAL2,
	SEQ_ESOUND_WIND,
	SEQ_NUMSEQ
};

enum class seqtype_t
{
	SEQTYPE_STONE,
	SEQTYPE_HEAVY,
	SEQTYPE_METAL,
	SEQTYPE_CREAK,
	SEQTYPE_SILENCE,
	SEQTYPE_LAVA,
	SEQTYPE_WATER,
	SEQTYPE_ICE,
	SEQTYPE_EARTH,
	SEQTYPE_METAL2,
	SEQTYPE_NUMSEQ
};

void SN_InitSequenceScript();
void SN_StartSequence(mobj_t* mobj, int sequence);
void SN_StartSequenceName(mobj_t* mobj, char* name);
void SN_StopSequence(mobj_t* mobj);
void SN_UpdateActiveSequences();
void SN_StopAllSequences();
int SN_GetSequenceOffset(int sequence, int* sequencePtr);
void SN_ChangeNodeData(int nodeNum, int seqOffset, int delayTics, int volume, int currentSoundID);

struct seqnode_t
{
	int* sequencePtr;
	int sequence;
	mobj_t* mobj;
	int currentSoundID;
	int delayTics;
	int volume;
	int stopSound;
	seqnode_t* prev;
	seqnode_t* next;
};

extern int ActiveSequences;
extern seqnode_t* SequenceListHead;

//----------------------
// Interlude (IN_lude.c)
//----------------------

#define MAX_INTRMSN_MESSAGE_SIZE 1024

extern bool intermission;
extern char ClusterMessage[MAX_INTRMSN_MESSAGE_SIZE];

void IN_Start();
void IN_Ticker();
void IN_Drawer();

//----------------------
// Chat mode (CT_chat.c)
//----------------------

void CT_Init();
void CT_Drawer();
bool CT_Responder(event_t* ev);
void CT_Ticker();
char CT_dequeueChatChar();

extern bool chatmodeon;

//--------------------
// Finale (F_finale.c)
//--------------------

void F_Drawer();
void F_Ticker();
void F_StartFinale();

//----------------------
// STATUS BAR (SB_bar.c)
//----------------------

extern int inv_ptr;
extern int curpos;
void SB_Init();
void SB_SetClassData();
bool SB_Responder(event_t* event);
void SB_Ticker();
void SB_Drawer();
void Draw_TeleportIcon();
void Draw_SaveIcon();
void Draw_LoadIcon();

//-----------------
// MENU (MN_menu.c)
//-----------------

void MN_Init();
void MN_ActivateMenu();
void MN_DeactivateMenu();
bool MN_Responder(event_t* event);
void MN_Ticker();
void MN_Drawer();
void MN_DrTextA(const char* text, int x, int y);
void MN_DrTextAYellow(const char* text, int x, int y);
int MN_TextAWidth(const char* text);
void MN_DrTextB(const char* text, int x, int y);
int MN_TextBWidth(const char* text);

extern int messageson;

#include "sounds.h"
