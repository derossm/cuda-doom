/**********************************************************************************************************************************************\
	Copyright(C) 1993-1996 Id Software, Inc.
	Copyright(C) 2005-2014 Simon Howard

	This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License
	as published by the Free Software Foundation; either version 2 of the License, or (at your option) any later version.

	This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
\**********************************************************************************************************************************************/
#include "../derma/stdafx.h"

//#include <string>

#include "../../derma/enumbitset.h"

#include "doomdef.h"
#include "doomkeys.h"
#include "doomstat.h"

#include "deh_main.h"
#include "deh_misc.h"
#include "deh_bexpars.h" // bex_pars[]

#include "z_zone.h"
#include "f_finale.h"
#include "m_argv.h"
#include "m_controls.h"
#include "m_misc.h"
#include "m_menu.h"
#include "m_random.h"
#include "i_system.h"
#include "i_timer.h"
#include "i_input.h"
#include "i_swap.h"
#include "i_video.h"

#include "p_setup.h"
#include "p_saveg.h"
#include "p_extsaveg.h"
#include "p_tick.h"

#include "d_main.h"

#include "wi_stuff.h"
#include "hu_stuff.h"
#include "st_stuff.h"
#include "am_map.h"
#include "statdump.h"

// Needs access to LFB.
#include "v_video.h"

#include "w_wad.h"

#include "p_local.h"

#include "s_sound.h"

// Data.
#include "dstrings.h"
#include "sounds.h"

// SKY handling - still the wrong place.
#include "r_data.h"
#include "r_sky.h"

#include "g_game.h"
#include "v_trans.h" // colored "always run" message

constexpr size_t SAVEGAMESIZE{0x2c000};

void G_ReadDemoTiccmd(ticcmd_t* cmd);
void G_WriteDemoTiccmd(ticcmd_t* cmd);
void G_PlayerReborn(int player);

void G_DoReborn(int playernum);

void G_DoLoadLevel();
void G_DoNewGame();
void G_DoPlayDemo();
void G_DoCompleted();
void G_DoVictory();
void G_DoWorldDone();
void G_DoSaveGame();

// Gamestate the last time G_Ticker was called.

GameState_t oldgamestate;

GameAction_t gameaction;
GameState_t gamestate;
SkillType gameskill;
bool respawnmonsters;
int gameepisode;
int gamemap;

// If non-zero, exit the level after this number of minutes.

TimeType timelimit;

bool paused;
bool sendpause;					// send a pause event next tic
bool sendsave;					// send a save event next tic
bool usergame;				// ok to save / end game

bool timingdemo;				// if true, exit with report on completion
bool nodrawers;				// for comparative timing purposes
TimeType starttime;				// for comparative timing purposes

bool viewactive;

int deathmatch;				// only if started as net death
bool netgame;				// only true if packets are broadcast
bool playeringame[MAX_PLAYERS];
Player players[MAX_PLAYERS];

bool turbodetected[MAX_PLAYERS];

int consoleplayer;			// player taking events and displaying
int displayplayer;			// view being displayed
TimeType levelstarttic;			// gametic at level start
int totalkills, totalitems, totalsecret;	// for intermission
int extrakills;				// count spawned monsters
TimeType totalleveltimes;		// CPhipps - total time for all completed levels
TimeType demostarttic;			// fix revenant internal demo bug

char* demoname;
const char* orig_demoname; // the name originally chosen for the demo, i.e. without "-00000"
bool demorecording;
bool longtics;				// cph's doom 1.91 longtics hack
bool lowres_turn;			// low resolution turning for longtics
bool demoplayback;
bool netdemo;
byte* demobuffer;
byte* demo_p;
byte* demoend;
bool singledemo;				// quit after playing a demo from cmdline

bool precache = true;		// if true, load all graphics at start

bool testcontrols = false;	// Invoked by setup to test controls
int testcontrols_mousespeed;

wbstartstruct_t wminfo;					// parms for world map / intermission

byte consistancy[MAX_PLAYERS][BACKUPTICS];

#define MAXPLMOVE (forwardmove[1])

constexpr size_t TURBOTHRESHOLD{0x32};

fixed_t forwardmove[2] = {0x19, 0x32};
fixed_t sidemove[2] = {0x18, 0x28};
fixed_t angleturn[3] = {640, 1280, 320};	// + slow turn

static int* weapon_keys[] = {
	&key_weapon1,
	&key_weapon2,
	&key_weapon3,
	&key_weapon4,
	&key_weapon5,
	&key_weapon6,
	&key_weapon7,
	&key_weapon8
};

// Set to -1 or +1 to switch to the previous or next weapon.

static int next_weapon = 0;

// Used for prev/next weapon keys.

static const struct
{
	WeaponType weapon;
	WeaponType weapon_num;
} weapon_order_table[] = {
	{ WeaponType::wp_fist,			WeaponType::wp_fist },
	{ WeaponType::wp_chainsaw,		WeaponType::wp_fist },
	{ WeaponType::wp_pistol,			WeaponType::wp_pistol },
	{ WeaponType::wp_shotgun,			WeaponType::wp_shotgun },
	{ WeaponType::wp_supershotgun,	WeaponType::wp_shotgun },
	{ WeaponType::wp_chaingun,		WeaponType::wp_chaingun },
	{ WeaponType::wp_missile,			WeaponType::wp_missile },
	{ WeaponType::wp_plasma,			WeaponType::wp_plasma },
	{ WeaponType::wp_bfg,				WeaponType::wp_bfg }
};

constexpr size_t SLOWTURNTICS{6};

constexpr size_t NUMKEYS{256};
constexpr size_t MAX_JOY_BUTTONS{20};

static bool gamekeydown[NUMKEYS];
static int turnheld;		// for accelerative turning
static int lookheld;		// for accelerative looking

static bool mousearray[MAX_MOUSE_BUTTONS + 1];
static bool* mousebuttons = &mousearray[1]; // allow [-1]

// mouse values are used once
int mousex;
int mousex2;
int mousey;

static TimeType dclicktime;
static bool dclickstate;
static int dclicks;
static TimeType dclicktime2;
static bool dclickstate2;
static int dclicks2;

// joystick values are repeated
static int joyxmove;
static int joyymove;
static int joystrafemove;
static int joylook;
static bool joyarray[MAX_JOY_BUTTONS + 1];
static bool* joybuttons = &joyarray[1];		// allow [-1]

static char savename[256]; // moved here, made static
static int savegameslot;
static char savedescription[32];

constexpr size_t BODYQUESIZE{32};

MapObject* bodyque[BODYQUESIZE];
int bodyqueslot;

int vanilla_savegame_limit = 1;
int vanilla_demo_limit = 1;

// store last cmd to track joins
static ticcmd_t* last_cmd = NULL;

int G_CmdChecksum(ticcmd_t* cmd)
{
	size_t i;
	int sum = 0;

	for (i = 0; i < sizeof(*cmd) / 4 - 1; ++i)
		sum += ((int*)cmd)[i];

	return sum;
}

static bool WeaponSelectable(WeaponType weapon)
{
	// Can't select the super shotgun in Doom 1.

	if (weapon == WeaponType::wp_supershotgun && !crispy->havessg)
	{
		return false;
	}

	// These weapons aren't available in shareware.

	if ((weapon == WeaponType::wp_plasma || weapon == WeaponType::wp_bfg)
		&& gamemission == GameMission::doom && gamemode == GameMode::shareware)
	{
		return false;
	}

	// Can't select a weapon if we don't own it.

	if (!players[::std::size_t(consoleplayer)].weaponowned[::std::size_t(weapon)])
	{
		return false;
	}

	// Can't select the fist if we have the chainsaw, unless
	// we also have the berserk pack.

	if (weapon == WeaponType::wp_fist
		&& players[::std::size_t(consoleplayer)].weaponowned[::std::size_t(WeaponType::wp_chainsaw)]
		&& !players[::std::size_t(consoleplayer)].powers[::std::size_t(PowerType_t::pw_strength)])
	{
		return false;
	}

	return true;
}

static int G_NextWeapon(int direction)
{
	WeaponType weapon;
	int start_i;
	int i;

	// Find index in the table.

	if (players[consoleplayer].pendingweapon == WeaponType::wp_nochange)
	{
		weapon = players[consoleplayer].readyweapon;
	}
	else
	{
		weapon = players[consoleplayer].pendingweapon;
	}

	for (i = 0; i < arrlen(weapon_order_table); ++i)
	{
		if (weapon_order_table[i].weapon == weapon)
		{
			break;
		}
	}

	// Switch weapon. Don't loop forever.
	start_i = i;
	do
	{
		i += direction;
		i = (i + arrlen(weapon_order_table)) % arrlen(weapon_order_table);
	} while (i != start_i && !WeaponSelectable(weapon_order_table[i].weapon));

	return weapon_order_table[i].weapon_num;
}

// holding down the "Run" key may trigger special behavior,
// e.g. quick exit, clean screenshots, resurrection from savegames
bool speedkeydown()
{
	return (key_speed < NUMKEYS&& gamekeydown[key_speed]) || (joybspeed < MAX_JOY_BUTTONS&& joybuttons[joybspeed]);
}

//
// G_BuildTiccmd
// Builds a ticcmd from all of the available inputs
// or reads it from the demo buffer.
// If recording a demo, write it out
//
void G_BuildTiccmd(ticcmd_t* cmd, TimeType maketic)
{
	int i;
	bool strafe;
	bool bstrafe;
	int speed;
	int tspeed;
	int lspeed;
	int forward;
	int side;
	int look;
	Player* const player = &players[consoleplayer];
	static char playermessage[48];

	memset(cmd, 0, sizeof(ticcmd_t));

	cmd->consistancy = consistancy[consoleplayer][maketic % BACKUPTICS];

	strafe = gamekeydown[key_strafe] || mousebuttons[mousebstrafe] || joybuttons[joybstrafe];

	// fraggle: support the old "joyb_speed = 31" hack which
	// allowed an autorun effect

	// when "always run" is active,
	// pressing the "run" key will result in walking
	speed = key_speed >= NUMKEYS || joybspeed >= MAX_JOY_BUTTONS;
	speed ^= speedkeydown();

	forward = side = look = 0;

	// use two stage accelerative turning
	// on the keyboard and joystick
	if (joyxmove < 0 || joyxmove > 0 || gamekeydown[key_right] || gamekeydown[key_left])
		turnheld += ticdup;
	else
		turnheld = 0;

	if (turnheld < SLOWTURNTICS)
		tspeed = 2;				// slow turn
	else
		tspeed = speed;

	// use two stage accelerative looking
	if (gamekeydown[key_lookdown] || gamekeydown[key_lookup])
	{
		lookheld += ticdup;
	}
	else
	{
		lookheld = 0;
	}
	if (lookheld < SLOWTURNTICS)
	{
		lspeed = 1;
	}
	else
	{
		lspeed = 2;
	}

	// add quick 180° reverse
	if (gamekeydown[key_reverse] || mousebuttons[mousebreverse])
	{
		cmd->angleturn += ANG180 >> FRACBITS;
		gamekeydown[key_reverse] = false;
		mousebuttons[mousebreverse] = false;
	}

	// toggle "always run"
	if (gamekeydown[key_toggleautorun])
	{
		static int joybspeed_old = 2;

		if (joybspeed >= MAX_JOY_BUTTONS)
		{
			joybspeed = joybspeed_old;
		}
		else
		{
			joybspeed_old = joybspeed;
			joybspeed = 29;
		}

		M_snprintf(playermessage, sizeof(playermessage), "ALWAYS RUN %s%s",
			crstr[CR_GREEN],
			(joybspeed >= MAX_JOY_BUTTONS) ? "ON" : "OFF");
		player->message = playermessage;
		S_StartSound(NULL, sfxenum_t::sfx_swtchn);

		gamekeydown[key_toggleautorun] = false;
	}

	// Toggle vertical mouse movement
	if (gamekeydown[key_togglenovert])
	{
		novert = !novert;

		M_snprintf(playermessage, sizeof(playermessage),
			"vertical mouse movement %s%s",
			crstr[CR_GREEN],
			!novert ? "ON" : "OFF");
		player->message = playermessage;
		S_StartSound(NULL, sfxenum_t::sfx_swtchn);

		gamekeydown[key_togglenovert] = false;
	}

	// extra high precision IDMYPOS variant, updates for 10 seconds
	if (player->powers[::std::size_t(PowerType_t::pw_mapcoords)])
	{
		M_snprintf(playermessage, sizeof(playermessage),
			"X=%.10f Y=%.10f A=%d",
			(double)player->x / FRACUNIT,
			(double)player->y / FRACUNIT,
			player->angle >> 24);
		player->message = playermessage;

		player->powers[::std::size_t(PowerType_t::pw_mapcoords)]--;

		// discard instead of going static
		if (!player->powers[::std::size_t(PowerType_t::pw_mapcoords)])
		{
			player->message = "";
		}
	}

	// let movement keys cancel each other out
	if (strafe)
	{
		if (gamekeydown[key_right])
		{
			// fprintf(stderr, "strafe right\n");
			side += sidemove[speed];
		}
		if (gamekeydown[key_left])
		{
			//	fprintf(stderr, "strafe left\n");
			side -= sidemove[speed];
		}
		if (joyxmove > 0)
			side += sidemove[speed];
		if (joyxmove < 0)
			side -= sidemove[speed];

	}
	else
	{
		if (gamekeydown[key_right])
			cmd->angleturn -= angleturn[tspeed];
		if (gamekeydown[key_left])
			cmd->angleturn += angleturn[tspeed];
		if (joyxmove > 0)
			cmd->angleturn -= angleturn[tspeed];
		if (joyxmove < 0)
			cmd->angleturn += angleturn[tspeed];
	}

	if (gamekeydown[key_up] || gamekeydown[key_alt_up]) // add key_alt_*
	{
		// fprintf(stderr, "up\n");
		forward += forwardmove[speed];
	}
	if (gamekeydown[key_down] || gamekeydown[key_alt_down]) // add key_alt_*
	{
		// fprintf(stderr, "down\n");
		forward -= forwardmove[speed];
	}

	if (joyymove < 0)
		forward += forwardmove[speed];
	if (joyymove > 0)
		forward -= forwardmove[speed];

	if (gamekeydown[key_strafeleft] || gamekeydown[key_alt_strafeleft] // add key_alt_*
		|| joybuttons[joybstrafeleft]
		|| mousebuttons[mousebstrafeleft]
		|| joystrafemove < 0)
	{
		side -= sidemove[speed];
	}

	if (gamekeydown[key_straferight] || gamekeydown[key_alt_straferight] // add key_alt_*
		|| joybuttons[joybstraferight]
		|| mousebuttons[mousebstraferight]
		|| joystrafemove > 0)
	{
		side += sidemove[speed];
	}

	// look up/down/center keys
	if (crispy->freelook)
	{
		static unsigned kbdlookctrl = 0;

		if (gamekeydown[key_lookup] || joylook < 0)
		{
			look = lspeed;
			kbdlookctrl += ticdup;
		}
		else
			if (gamekeydown[key_lookdown] || joylook > 0)
			{
				look = -lspeed;
				kbdlookctrl += ticdup;
			}
			else
				// keyboard lookspring
				if (gamekeydown[key_lookcenter] || (crispy->freelook == FREELOOK_SPRING && kbdlookctrl))
				{
					look = TOCENTER;
					kbdlookctrl = 0;
				}
	}

	// jump keys
	if (critical->jump)
	{
		if (gamekeydown[key_jump] || mousebuttons[mousebjump]
			|| joybuttons[joybjump])
		{
			cmd->arti |= AFLAG_JUMP;
		}
	}

	// buttons
	cmd->chatchar = HU_dequeueChatChar();

	if (gamekeydown[key_fire] || mousebuttons[mousebfire]
		|| joybuttons[joybfire])
		cmd->buttons |= buttoncode::BT_ATTACK;

	if (gamekeydown[key_use]
		|| joybuttons[joybuse]
		|| mousebuttons[mousebuse])
	{
		cmd->buttons |= buttoncode::BT_USE;
		// clear double clicks if hit use button
		dclicks = 0;
	}

	// If the previous or next weapon button is pressed, the
	// next_weapon variable is set to change weapons when
	// we generate a ticcmd. Choose a new weapon.

	if (gamestate == GameState_t::GS_LEVEL && next_weapon != 0)
	{
		i = G_NextWeapon(next_weapon);
		cmd->buttons |= (int)buttoncode::BT_CHANGE;
		cmd->buttons |= i << (int)buttoncode::BT_WEAPONSHIFT;
	}
	else
	{
		// Check weapon keys.

		for (i = 0; i < arrlen(weapon_keys); ++i)
		{
			int key = *weapon_keys[i];

			if (gamekeydown[key])
			{
				cmd->buttons |= (int)buttoncode::BT_CHANGE;
				cmd->buttons |= (int)i << buttoncode::BT_WEAPONSHIFT;
				break;
			}
		}
	}

	next_weapon = 0;

	// mouse
	if (mousebuttons[mousebforward])
	{
		forward += forwardmove[speed];
	}
	if (mousebuttons[mousebbackward])
	{
		forward -= forwardmove[speed];
	}

	if (dclick_use)
	{
		// forward double click
		if (mousebuttons[mousebforward] != dclickstate && dclicktime > 1)
		{
			dclickstate = mousebuttons[mousebforward];
			if (dclickstate)
				++dclicks;
			if (dclicks == 2)
			{
				cmd->buttons |= (int)buttoncode::BT_USE;
				dclicks = 0;
			}
			else
				dclicktime = 0;
		}
		else
		{
			dclicktime += ticdup;
			if (dclicktime > 20)
			{
				dclicks = 0;
				dclickstate = 0;
			}
		}

		// strafe double click
		bstrafe =
			mousebuttons[mousebstrafe]
			|| joybuttons[joybstrafe];
		if (bstrafe != dclickstate2 && dclicktime2 > 1)
		{
			dclickstate2 = bstrafe;
			if (dclickstate2)
				++dclicks2;
			if (dclicks2 == 2)
			{
				cmd->buttons |= (int)buttoncode::BT_USE;
				dclicks2 = 0;
			}
			else
				dclicktime2 = 0;
		}
		else
		{
			dclicktime2 += ticdup;
			if (dclicktime2 > 20)
			{
				dclicks2 = 0;
				dclickstate2 = 0;
			}
		}
	}

	// mouse look
	if ((crispy->freelook && mousebuttons[mousebmouselook]) ||
		crispy->mouselook)
	{
		cmd->lookdir = mouse_y_invert ? -mousey : mousey;
	}
	else
		if (!novert)
		{
			forward += mousey;
		}

	// single click on mouse look button centers view
	if (crispy->freelook)
	{
		static unsigned mbmlookctrl = 0;

		// single click view centering
		if (mousebuttons[mousebmouselook]) // clicked
		{
			mbmlookctrl += ticdup;
		}
		else
			// released
			if (mbmlookctrl)
			{
				if (crispy->freelook == FREELOOK_SPRING || mbmlookctrl < SLOWTURNTICS) // short click
				{
					look = TOCENTER;
				}
				mbmlookctrl = 0;
			}
	}

	if (strafe)
		side += mousex2 * 2;
	else
		cmd->angleturn -= mousex * 0x8;

	if (mousex == 0)
	{
		// No movement in the previous frame

		testcontrols_mousespeed = 0;
	}

	mousex = mousex2 = mousey = 0;

	if (forward > MAXPLMOVE)
		forward = MAXPLMOVE;
	else if (forward < -MAXPLMOVE)
		forward = -MAXPLMOVE;
	if (side > MAXPLMOVE)
		side = MAXPLMOVE;
	else if (side < -MAXPLMOVE)
		side = -MAXPLMOVE;

	cmd->forwardmove += forward;
	cmd->sidemove += side;

	// lookdir delta is stored in the lower 4 bits of the lookfly variable
	if (player->playerstate == PlayerState::live)
	{
		if (look < 0)
		{
			look += 16;
		}
		cmd->lookfly = look;
	}

	// special buttons
	if (sendpause)
	{
		sendpause = false;
		// ignore un-pausing in menus during demo recording
		if (!(menuactive && demorecording && paused) && gameaction != GameAction_t::ga_loadgame)
		{
			cmd->buttons = (int)buttoncode::BT_SPECIAL | (int)buttoncode::BTS_PAUSE;
		}
	}

	if (sendsave)
	{
		sendsave = false;
		cmd->buttons = (int)buttoncode::BT_SPECIAL | (int)buttoncode::BTS_SAVEGAME | (savegameslot << (int)buttoncode::BTS_SAVESHIFT);
	}

	if (crispy->fliplevels)
	{
		cmd->angleturn = -cmd->angleturn;
		cmd->sidemove = -cmd->sidemove;
	}

	// low-res turning
	if (lowres_turn)
	{
		static short carry = 0;
		short desired_angleturn;

		desired_angleturn = cmd->angleturn + carry;

		// round angleturn to the nearest 256 unit boundary
		// for recording demos with single byte values for turn
		cmd->angleturn = (desired_angleturn + 128) & 0xff00;

		// Carry forward the error from the reduced resolution to the
		// next tic, so that successive small movements can accumulate.
		carry = desired_angleturn - cmd->angleturn;
	}
}

//
// G_DoLoadLevel
//
void G_DoLoadLevel()
{
	int i;

	// Set the sky map.
	// First thing, we have a dummy sky texture name,
	// a flat. The data is in the WAD only because
	// we look for an actual index, instead of simply
	// setting one.
	skyflatnum = R_FlatNumForName(DEH_String(SKYFLATNAME));

	// The "Sky never changes in Doom II" bug was fixed in
	// the id Anthology version of doom2.exe for Final Doom.
	// correct "Sky never changes in Doom II" bug
	if ((gamemode == GameMode::commercial)
		&& (gameversion == GameVersion::exe_final2 || gameversion == GameVersion::exe_chex || true))
	{
		::std::string skytexturename;

		if (gamemap < 12)
		{
			if ((gameepisode == 2 || gamemission == GameMission::pack_nerve) && gamemap >= 4 && gamemap <= 8)
				skytexturename = "SKY3";
			else
				skytexturename = "SKY1";
		}
		else if (gamemap < 21)
		{
			// BLACKTWR (MAP25) and TEETH (MAP31 and MAP32)
			if ((gameepisode == 3 || gamemission == GameMission::pack_master) && gamemap >= 19)
				skytexturename = "SKY3";
			else
				// BLOODSEA and MEPHISTO (both MAP07)
				if ((gameepisode == 3 || gamemission == GameMission::pack_master) && (gamemap == 14 || gamemap == 15))
					skytexturename = "SKY1";
				else
					skytexturename = "SKY2";
		}
		else
		{
			skytexturename = "SKY3";
		}

		skytexturename = DEH_String(skytexturename);

		skytexture = R_TextureNumForName(skytexturename);
	}
	// sky texture scales
	R_InitSkyMap();

	levelstarttic = gametic;		// for time calculation

	if (wipegamestate == GameState_t::GS_LEVEL)
		wipegamestate = -1;				// force a wipe

	gamestate = GameState_t::GS_LEVEL;

	for (i = 0; i < MAX_PLAYERS; ++i)
	{
		turbodetected[i] = false;
		if (playeringame[i] && players[i].playerstate == PlayerState::dead)
			players[i].playerstate = PlayerState::reborn;
		memset(players[i].frags, 0, sizeof(players[i].frags));
	}

	// update the "singleplayer" variable
	CheckCrispySingleplayer(!demorecording && !demoplayback && !netgame);

	// pistol start
	if (crispy->pistolstart)
	{
		if (crispy->singleplayer)
		{
			G_PlayerReborn(0);
		}
		else if ((demoplayback || netdemo) && !singledemo)
		{
			// no-op - silently ignore pistolstart when playing demo from the
			// demo reel
		}
		else
		{
			const char message[] = "The -pistolstart option is not supported"
				" for demos and\n"
				" network play.";
			if (!demo_p) demorecording = false;
			I_Error(message);
		}
	}

	P_SetupLevel(gameepisode, gamemap, 0, gameskill);
	displayplayer = consoleplayer;		// view the guy you are playing
	gameaction = GameAction_t::ga_nothing;
	Z_CheckHeap();

	// clear cmd building stuff

	memset(gamekeydown, 0, sizeof(gamekeydown));
	joyxmove = joyymove = joystrafemove = joylook = 0;
	mousex = mousex2 = mousey = 0;
	sendpause = sendsave = paused = false;
	memset(mousearray, 0, sizeof(mousearray));
	memset(joyarray, 0, sizeof(joyarray));

	if (testcontrols)
	{
		players[consoleplayer].message = "Press escape to quit.";
	}
}

static void SetJoyButtons(unsigned buttons_mask)
{
	int i;

	for (i = 0; i < MAX_JOY_BUTTONS; ++i)
	{
		int button_on = (buttons_mask & (1 << i)) != 0;

		// Detect button press:

		if (!joybuttons[i] && button_on)
		{
			// Weapon cycling:

			if (i == joybprevweapon)
			{
				next_weapon = -1;
			}
			else if (i == joybnextweapon)
			{
				next_weapon = 1;
			}
		}

		joybuttons[i] = button_on;
	}
}

static void SetMouseButtons(unsigned buttons_mask)
{
	int i;

	for (i = 0; i < MAX_MOUSE_BUTTONS; ++i)
	{
		unsigned button_on = (buttons_mask & (1 << i)) != 0;

		// Detect button press:

		if (!mousebuttons[i] && button_on)
		{
			if (i == mousebprevweapon)
			{
				next_weapon = -1;
			}
			else if (i == mousebnextweapon)
			{
				next_weapon = 1;
			}
		}

		mousebuttons[i] = button_on;
	}
}

// G_Responder
// Get info needed to make ticcmd_ts for the players.
bool G_Responder(EventType* ev)
{
	// allow spy mode changes even during the demo
	if (gamestate == GameState_t::GS_LEVEL && ev->type == evtype_t::ev_keydown
		&& ev->data1 == key_spy && (singledemo || !deathmatch))
	{
		// spy mode
		do
		{
			++displayplayer;
			if (displayplayer == MAX_PLAYERS)
				displayplayer = 0;
		} while (!playeringame[displayplayer] && displayplayer != consoleplayer);
		return true;
	}

	// any other key pops up menu if in demos
	if (gameaction == GameAction_t::ga_nothing && !singledemo &&
		(demoplayback || gamestate == GameState_t::GS_DEMOSCREEN)
		)
	{
		if (ev->type == evtype_t::ev_keydown ||
			(ev->type == evtype_t::ev_mouse && ev->data1) ||
			(ev->type == evtype_t::ev_joystick && ev->data1))
		{
			M_StartControlPanel();
			// play a sound if the menu is activated with a different key than ESC
			if (crispy->soundfix)
				S_StartSound(nullptr, sfxenum_t::sfx_swtchn);
			return true;
		}
		return false;
	}

	if (gamestate == GameState_t::GS_LEVEL)
	{
#if 0
		if (devparm && ev->type == ev_keydown && ev->data1 == ';')
		{
			G_DeathMatchSpawnPlayer(0);
			return true;
		}
#endif
		if (HU_Responder(ev))
			return true;	// chat ate the event
		if (ST_Responder(ev))
			return true;	// status window ate it
		if (AM_Responder(ev))
			return true;	// automap ate it
	}

	if (gamestate == GameState_t::GS_FINALE)
	{
		if (F_Responder(ev))
			return true;	// finale ate the event
	}

	if (testcontrols && ev->type == evtype_t::ev_mouse)
	{
		// If we are invoked by setup to test the controls, save the
		// mouse speed so that we can display it on-screen.
		// Perform a low pass filter on this so that the thermometer
		// appears to move smoothly.

		testcontrols_mousespeed = abs(ev->data2);
	}

	// If the next/previous weapon keys are pressed, set the next_weapon
	// variable to change weapons when the next ticcmd is generated.

	if (ev->type == evtype_t::ev_keydown && ev->data1 == key_prevweapon)
	{
		next_weapon = -1;
	}
	else if (ev->type == evtype_t::ev_keydown && ev->data1 == key_nextweapon)
	{
		next_weapon = 1;
	}

	switch (ev->type)
	{
	case evtype_t::ev_keydown:
		if (ev->data1 == key_pause)
		{
			sendpause = true;
		}
		else if (ev->data1 < NUMKEYS)
		{
			gamekeydown[ev->data1] = true;
		}

		return true;	// eat key down events

	case evtype_t::ev_keyup:
		if (ev->data1 < NUMKEYS)
			gamekeydown[ev->data1] = false;
		return false;	// always let key up events filter down

	case evtype_t::ev_mouse:
		SetMouseButtons(ev->data1);
		if (mouseSensitivity)
			mousex = ev->data2 * (mouseSensitivity + 5) / 10;
		else
			mousex = 0; // disable entirely
		if (mouseSensitivity_x2)
			mousex2 = ev->data2 * (mouseSensitivity_x2 + 5) / 10; // separate sensitivity for strafe
		else
			mousex2 = 0; // disable entirely
		if (mouseSensitivity_y)
			mousey = ev->data3 * (mouseSensitivity_y + 5) / 10; // separate sensitivity for y-axis
		else
			mousey = 0; // disable entirely
		return true;	// eat events

	case evtype_t::ev_joystick:
		SetJoyButtons(ev->data1);
		joyxmove = ev->data2;
		joyymove = ev->data3;
		joystrafemove = ev->data4;
		joylook = ev->data5;
		return true;	// eat events

	default:
		break;
	}

	return false;
}

// re-read game parameters from command line
static void G_ReadGameParms()
{
	respawnparm = M_CheckParm("-respawn");
	fastparm = M_CheckParm("-fast");
	nomonsters = M_CheckParm("-nomonsters");
}

// take a screenshot after rendering the next frame
static void G_CrispyScreenShot()
{
	// increase screenshot filename limit
	V_ScreenShot("DOOM%04i.%s");
	players[consoleplayer].message = DEH_String("screen shot");
	crispy->cleanscreenshot = 0;
	crispy->screenshotmsg = 2;
}

// Make ticcmd_ts for the players.
void G_Ticker()
{
	// do player reborns if needed
	for (size_t i{0}; i < MAX_PLAYERS; ++i)
		if (playeringame[i] && players[i].playerstate == PlayerState::reborn)
			G_DoReborn(i);

	// do things to change the game state
	while (gameaction != GameAction_t::ga_nothing)
	{
		switch (gameaction)
		{
		case GameAction_t::ga_loadlevel:
			G_DoLoadLevel();
			break;
		case GameAction_t::ga_newgame:
			// re-read game parameters from command line
			G_ReadGameParms();
			G_DoNewGame();
			break;
		case GameAction_t::ga_loadgame:
			// re-read game parameters from command line
			G_ReadGameParms();
			G_DoLoadGame();
			break;
		case GameAction_t::ga_savegame:
			G_DoSaveGame();
			break;
		case GameAction_t::ga_playdemo:
			G_DoPlayDemo();
			break;
		case GameAction_t::ga_completed:
			G_DoCompleted();
			break;
		case GameAction_t::ga_victory:
			F_StartFinale();
			break;
		case GameAction_t::ga_worlddone:
			G_DoWorldDone();
			break;
		case GameAction_t::ga_screenshot:
			// redraw view without weapons and HUD
			if (gamestate == GameState_t::GS_LEVEL && (crispy->cleanscreenshot || crispy->screenshotmsg == 1))
			{
				crispy->screenshotmsg = 4;
				crispy->post_rendering_hook = G_CrispyScreenShot;
			}
			else
			{
				G_CrispyScreenShot();
			}
			gameaction = GameAction_t::ga_nothing;
			break;
		case GameAction_t::ga_nothing:
			break;
		}
	}

	// get commands, check consistancy, and build new consistancy check
	auto buf = (gametic / ticdup) % BACKUPTICS;

	for (size_t i{0}; i < MAX_PLAYERS; ++i)
	{
		if (playeringame[i])
		{
			auto cmd = &players[i].cmd;

			memcpy(cmd, &netcmds[i], sizeof(ticcmd_t));

			if (demoplayback)
			{
				G_ReadDemoTiccmd(cmd);
			}

			// do not record tics while still playing back in demo continue mode
			if (demorecording && !demoplayback)
			{
				G_WriteDemoTiccmd(cmd);
			}

			// check for turbo cheats

			// check ~ 4 seconds whether to display the turbo message. store if the turbo threshold was exceeded in any tics
			// over the past 4 seconds. offset the checking period for each player so messages are not displayed at the same time.
			if (cmd->forwardmove > TURBOTHRESHOLD)
			{
				turbodetected[i] = true;
			}

			if ((gametic & 31) == 0 && ((gametic >> 5) % MAX_PLAYERS) == i && turbodetected[i])
			{
				static char turbomessage[80];
				extern ::std::string player_names[4];
				M_snprintf(turbomessage, sizeof(turbomessage), "%s is turbo!", player_names[i]);
				players[consoleplayer].message = turbomessage;
				turbodetected[i] = false;
			}

			if (netgame && !netdemo && !(gametic % ticdup))
			{
				if (gametic > BACKUPTICS && consistancy[i][buf] != cmd->consistancy)
				{
					I_Error("consistency failure (%i should be %i)", cmd->consistancy, consistancy[i][buf]);
				}
				if (players[i])
				{
					consistancy[i][buf] = players[i]->x;
				}
				else
				{
					consistancy[i][buf] = rndindex;
				}
			}
		}
	}

	// check for special buttons
	for (size_t i{0}; i < MAX_PLAYERS; ++i)
	{
		if (playeringame[i])
		{
			if ((int)players[i].cmd.buttons & (int)buttoncode::BT_SPECIAL)
			{
				switch ((int)players[i].cmd.buttons & (int)buttoncode::BT_SPECIALMASK)
				{
				case (int)buttoncode::BTS_PAUSE:
					paused ^= 1;
					if (paused)
					{
						S_PauseSound();
					}
					// Fixed bug when music was hearable with zero volume
					else if (musicVolume)
					{
						S_ResumeSound();
					}

					break;

				case (int)buttoncode::BTS_SAVEGAME:
					// never override savegames by demo playback
					if (demoplayback)
					{
						break;
					}

					if (!savedescription[0])
					{
						M_StringCopy(savedescription, "NET GAME", sizeof(savedescription));
					}

					savegameslot = ((int)players[i].cmd.buttons & (int)buttoncode::BTS_SAVEMASK) >> (int)buttoncode::BTS_SAVESHIFT;
					gameaction = GameAction_t::ga_savegame;
					// un-pause immediately after saving
					// (impossible to send save and pause specials within the same tic)
					if (demorecording && paused)
					{
						sendpause = true;
					}
					break;
				}
			}
		}
	}

	// Have we just finished displaying an intermission screen?

	if (oldgamestate == GameState_t::GS_INTERMISSION && gamestate != GameState_t::GS_INTERMISSION)
	{
		WI_End();
	}

	oldgamestate = gamestate;
	oldleveltime = leveltime;

	// do main actions
	switch (gamestate)
	{
	case GameState_t::GS_LEVEL:
		P_Ticker();
		ST_Ticker();
		AM_Ticker();
		HU_Ticker();
		break;
	case GameState_t::GS_INTERMISSION:
		WI_Ticker();
		break;
	case GameState_t::GS_FINALE:
		F_Ticker();
		break;
	case GameState_t::GS_DEMOSCREEN:
		D_PageTicker();
		break;
	}
}

// PLAYER STRUCTURE FUNCTIONS
// also see P_SpawnPlayer in P_Things

//
// G_InitPlayer
// Called at the start.
// Called by the game initialization functions.
//
void G_InitPlayer(int player)
{
	// clear everything else to defaults
	G_PlayerReborn(player);
}

//
// G_PlayerFinishLevel
// Can when a player completes a level.
//
void G_PlayerFinishLevel(int player)
{
	Player* p = &players[player];

	// FIXME
	//memset(p->powers, 0, sizeof(p->powers));
	//memset(p->cards, 0, sizeof(p->cards));
	//memset(p->tryopen, 0, sizeof(p->tryopen)); // blinking key or skull in the status bar
	p->flags.set(mobjflag_e::MF_SHADOW, false); // cancel invisibility
	p->extralight = 0;			// cancel gun flashes
	p->fixedcolormap = 0;		// cancel ir gogles
	p->damagecount = 0;			// no palette changes
	p->bonuscount = 0;
	// reset additional player properties

	p->lookdir = 0;
	p->oldlookdir = 0;
	p->centering = 0;
	p->jumpTics = 0;
	p->recoilpitch = 0;
	p->oldrecoilpitch = 0;
	p->psp_dy_max = 0;
	p->btuse = 0;
	p->btuse_tics = 0;
}

//
// G_PlayerReborn
// Called after a player dies
// almost everything is cleared and initialized
//
void G_PlayerReborn(int player)
{
	Player* p;
	int i;
	int frags[MAX_PLAYERS];
	int killcount;
	int itemcount;
	int secretcount;

	// FIXME
	//memcpy(frags, players[player].frags, sizeof(frags));
	killcount = players[player].killcount;
	itemcount = players[player].itemcount;
	secretcount = players[player].secretcount;

	p = &players[player];
	// FIXME
	//memset(p, 0, sizeof(*p));

	// FIXME
	//memcpy(players[player].frags, frags, sizeof(players[player].frags));
	players[player].killcount = killcount;
	players[player].itemcount = itemcount;
	players[player].secretcount = secretcount;

	p->usedown = p->attackdown = true;	// don't do anything immediately
	p->playerstate = PlayerState::live;
	p->health = deh_initial_health;		// Use dehacked value
	// negative player health
	p->neghealth = p->health;
	p->pendingweapon = WeaponType::wp_pistol;
	p->readyweapon = WeaponType::wp_pistol;
	p->weaponowned[::std::size_t(WeaponType::wp_fist)] = true;
	p->weaponowned[::std::size_t(WeaponType::wp_pistol)] = true;
	p->ammo[::std::size_t(AmmoType::am_clip)] = deh_initial_bullets;

	for (i = 0; i < AmmoType::NUMAMMO; ++i)
	{
		p->maxammo[i] = maxammo[i];
	}
}

//
// G_CheckSpot
// Returns false if the player cannot be respawned
// at the given mapthing_t spot
// because something is occupying it
//
void P_SpawnPlayer(mapthing_t* mthing);

bool G_CheckSpot(int playernum, mapthing_t* mthing)
{
	fixed_t x;
	fixed_t y;
	subsector_t* ss;
	MapObject* mo;
	int i;

	if (!players[playernum])
	{
		// first spawn of level, before corpses
		for (i = 0; i < playernum; ++i)
		{
			if (players[i].x == mthing->x << FRACBITS && players[i].y == mthing->y << FRACBITS)
			{
				return false;
			}
		}
		return true;
	}

	x = mthing->x << FRACBITS;
	y = mthing->y << FRACBITS;

	if (!P_CheckPosition(players[playernum], x, y))
	{
		return false;
	}

	// flush an old corpse if needed
	if (bodyqueslot >= BODYQUESIZE)
	{
		P_RemoveMobj(bodyque[bodyqueslot % BODYQUESIZE]);
	}

	bodyque[bodyqueslot % BODYQUESIZE] = players[playernum];
	++bodyqueslot;

	// spawn a teleport fog
	ss = R_PointInSubsector(x, y);

	// The code in the released source looks like this:
	//
	//	an = ( ANG45 * (((unsigned) mthing->angle)/45) ) >> ANGLETOFINESHIFT;
	//	mo = P_SpawnMobj (x+20*finecosine[an], y+20*finesine[an], ss->sector->floorheight, mobjtype_t::MT_TFOG);
	//
	// But 'an' can be a signed value in the DOS version. This means that
	// we get a negative index and the lookups into finecosine/finesine
	// end up dereferencing values in finetangent[].
	// A player spawning on a deathmatch start facing directly west spawns
	// "silently" with no spawn fog. Emulate this.
	//
	// This code is imported from PrBoom+.

	{
		fixed_t xa;
		fixed_t ya;
		int an;

		// This calculation overflows in Vanilla Doom, but here we deliberately
		// avoid integer overflow as it is undefined behavior, so the value of
		// 'an' will always be positive.
		an = (ANG45 >> ANGLETOFINESHIFT) * ((int)mthing->angle / 45);

		switch (an)
		{
		case 4096: // -4096:
			xa = finetangent[2048];	// finecosine[-4096]
			ya = finetangent[0];		// finesine[-4096]
			break;
		case 5120: // -3072:
			xa = finetangent[3072];	// finecosine[-3072]
			ya = finetangent[1024];	// finesine[-3072]
			break;
		case 6144: // -2048:
			xa = finesine[0];			// finecosine[-2048]
			ya = finetangent[2048];	// finesine[-2048]
			break;
		case 7168: // -1024:
			xa = finesine[1024];		// finecosine[-1024]
			ya = finetangent[3072];	// finesine[-1024]
			break;
		case 0:
		case 1024:
		case 2048:
		case 3072:
			xa = finecosine[an];
			ya = finesine[an];
			break;
		default:
			I_Error("G_CheckSpot: unexpected angle %d\n", an);
			xa = ya = 0;
			break;
		}
		mo = P_SpawnMobj(x + 20 * xa, y + 20 * ya, ss->sector->floorheight, mobjtype_t::MT_TFOG);
	}

	if (players[consoleplayer].viewz != 1)
	{
		S_StartSound(mo, sfxenum_t::sfx_telept);	// don't start sound on first frame
	}
	return true;
}

//
// G_DeathMatchSpawnPlayer
// Spawns a player at one of the random death match spots
// called at level load and each death
//
void G_DeathMatchSpawnPlayer(int playernum)
{
	int i;
	int j;
	int selections;

	selections = deathmatch_p - deathmatchstarts;
	if (selections < 4)
		I_Error("Only %i deathmatch spots, 4 required", selections);

	for (j = 0; j < 20; ++j)
	{
		i = P_Random() % selections;
		if (G_CheckSpot(playernum, &deathmatchstarts[i]))
		{
			deathmatchstarts[i].type = playernum + 1;
			P_SpawnPlayer(&deathmatchstarts[i]);
			return;
		}
	}

	// no good spot, so the player will probably get stuck
	P_SpawnPlayer(&playerstarts[playernum]);
}

// clear the "savename" variable,
// i.e. restart level from scratch upon resurrection
static inline void G_ClearSavename()
{
	M_StringCopy(savename, "", sizeof(savename));
}

//
// G_DoReborn
//
void G_DoReborn(int playernum)
{
	int i;

	if (!netgame)
	{
		// if the player dies and the game has been loaded or saved
		// in the mean time, reload that savegame instead of restarting the level
		// when "Run" is pressed upon resurrection
		if (crispy->singleplayer && *savename && speedkeydown())
		{
			gameaction = GameAction_t::ga_loadgame;
		}
		else
		{
			// reload the level from scratch
			gameaction = GameAction_t::ga_loadlevel;
			G_ClearSavename();
		}
	}
	else
	{
		// respawn at the start

		// first dissasociate the corpse
		// FIXME
		//players[playernum].player = nullptr;

		// spawn at random spot if in death match
		if (deathmatch)
		{
			G_DeathMatchSpawnPlayer(playernum);
			return;
		}

		if (G_CheckSpot(playernum, &playerstarts[playernum]))
		{
			P_SpawnPlayer(&playerstarts[playernum]);
			return;
		}

		// try to spawn at one of the other players spots
		for (i = 0; i < MAX_PLAYERS; ++i)
		{
			if (G_CheckSpot(playernum, &playerstarts[i]))
			{
				playerstarts[i].type = playernum + 1;	// fake as other player
				P_SpawnPlayer(&playerstarts[i]);
				playerstarts[i].type = i + 1;		// restore
				return;
			}
			// he's going to be inside something. Too bad.
		}

		P_SpawnPlayer(&playerstarts[playernum]);
	}
}

void G_ScreenShot()
{
	gameaction = GameAction_t::ga_screenshot;
}

// DOOM Par Times
int pars[6][10] =
{
	{0},
	{0,30,75,120,90,165,180,180,30,165},
	{0,90,90,90,120,90,360,240,30,170},
	{0,90,45,90,150,90,90,165,30,135},
	// Episode 4 par times from the BFG Edition
	{0,165,255,135,150,180,390,135,360,180},
	// Episode 5 par times from Sigil v1.21
	{0,90,150,360,420,780,420,780,300,660}
};

// DOOM II Par Times
int cpars[32] =
{
	30,90,120,120,90,150,120,120,270,90,	// 1-10
	210,150,150,150,210,150,420,150,210,150,	// 11-20
	240,150,180,150,150,300,330,420,300,180,	// 21-30
	120,30					// 31-32
};

// No Rest For The Living par times from the BFG Edition
static int npars[9] =
{
	75,105,120,105,210,105,165,105,135
};

bool secretexit;
extern ::std::string pagename;

void G_ExitLevel()
{
	secretexit = false;
	G_ClearSavename();
	gameaction = GameAction_t::ga_completed;
}

// Here's for the german edition.
void G_SecretExitLevel()
{
	// IF NO WOLF3D LEVELS, NO SECRET EXIT!
	if ((gamemode == GameMode::commercial) && (W_CheckNumForName("map31") < 0))
	{
		secretexit = false;
	}
	else
	{
		secretexit = true;
	}
	G_ClearSavename();
	gameaction = GameAction_t::ga_completed;
}

// format time for level statistics
constexpr size_t TIMESTRSIZE{16};
static void G_FormatLevelStatTime(::std::string str, TimeType tics)
{
	int exitHours;
	int exitMinutes;
	float exitTime;
	float exitSeconds;

	exitTime = (float)tics / 35;
	exitHours = exitTime / 3600;
	exitTime -= exitHours * 3600;
	exitMinutes = exitTime / 60;
	exitTime -= exitMinutes * 60;
	exitSeconds = exitTime;

	if (exitHours)
	{
		M_snprintf(str, TIMESTRSIZE, "%d:%02d:%05.2f", exitHours, exitMinutes, exitSeconds);
	}
	else
	{
		M_snprintf(str, TIMESTRSIZE, "%01d:%05.2f", exitMinutes, exitSeconds);
	}
}

// Write level statistics upon exit
static void G_WriteLevelStat()
{
	static FILE* fstream = NULL;

	int i;
	int playerKills = 0;
	int playerItems = 0;
	int playerSecrets = 0;

	char levelString[8];
	char levelTimeString[TIMESTRSIZE];
	char totalTimeString[TIMESTRSIZE];
	::std::string decimal;

	if (fstream == NULL)
	{
		fstream = fopen("levelstat.txt", "w");

		if (fstream == NULL)
		{
			fprintf(stderr, "G_WriteLevelStat: Unable to open levelstat.txt for writing!\n");
			return;
		}
	}

	if (gamemode == GameMode::commercial)
	{
		M_snprintf(levelString, sizeof(levelString), "MAP%02d", gamemap);
	}
	else
	{
		M_snprintf(levelString, sizeof(levelString), "E%dM%d", gameepisode, gamemap);
	}

	G_FormatLevelStatTime(levelTimeString, leveltime);
	G_FormatLevelStatTime(totalTimeString, totalleveltimes + leveltime);

	// Total time ignores centiseconds
	decimal = strchr(totalTimeString, '.');
	if (decimal != NULL)
	{
		*decimal = '\0';
	}

	for (i = 0; i < MAX_PLAYERS; ++)
	{
		if (playeringame[i])
		{
			playerKills += players[i].killcount;
			playerItems += players[i].itemcount;
			playerSecrets += players[i].secretcount;
		}
	}

	fprintf(fstream, "%s%s - %s (%s) K: %d/%d I: %d/%d S: %d/%d\n",
		levelString, (secretexit ? "s" : ""),
		levelTimeString, totalTimeString, playerKills, totalkills,
		playerItems, totalitems, playerSecrets, totalsecret);
}

void G_DoCompleted()
{
	int i;

	// Write level statistics upon exit
	if (M_ParmExists("-levelstat"))
	{
		G_WriteLevelStat();
	}

	gameaction = GameAction_t::ga_nothing;

	for (i = 0; i < MAX_PLAYERS; ++i)
		if (playeringame[i])
		{
			G_PlayerFinishLevel(i);		// take away cards and stuff
		}

	if (automapactive)
	{
		AM_Stop();
	}

	if (gamemode != GameMode::commercial)
	{
		// Chex Quest ends after 5 levels, rather than 8.
		if (gameversion == GameVersion::exe_chex)
		{
			// display tally screen after Chex Quest E1M5
			/*
			if (gamemap == 5)
			{
				gameaction = GameAction_t::ga_victory;
				return;
			}
			*/
		}
		else
		{
			switch (gamemap)
			{
				// display tally screen after ExM8
				/*
					case 8:
					gameaction = GameAction_t::ga_victory;
					return;
				*/
			case 9:
				for (i = 0; i < MAX_PLAYERS; ++i)
				{
					players[i].didsecret = true;
				}
				break;
			}
		}
	}

	// disable redundant code
	/*
	//#if 0 Hmmm - why?
		if ( (gamemap == 8)
			&& (gamemode != GameMode::commercial) )
		{
		// victory
		gameaction = GameAction_t::ga_victory;
		return;
		}

		if ( (gamemap == 9)
			&& (gamemode != GameMode::commercial) )
		{
		// exit secret level
		for (i=0 ; i<MAX_PLAYERS ; ++i)
			players[i].didsecret = true;
		}
	//#endif
	*/
	wminfo.didsecret = players[consoleplayer].didsecret;
	wminfo.epsd = gameepisode - 1;
	wminfo.last = gamemap - 1;

	// wminfo.next is 0 biased, unlike gamemap
	if (gamemission == GameMission::pack_nerve && gamemap <= 9)
	{
		if (secretexit)
		{
			switch (gamemap)
			{
			case 4:
				wminfo.next = 8; break;
			}
		}
		else
		{
			switch (gamemap)
			{
			case 9:
				wminfo.next = 4; break;
			default:
				wminfo.next = gamemap;
			}
		}
	}
	else if (gamemission == GameMission::pack_master && gamemap <= 21)
	{
		wminfo.next = gamemap;
	}
	else if (gamemode == GameMode::commercial)
	{
		if (secretexit)
		{
			if (gamemap == 2 && critical->havemap33)
			{
				wminfo.next = 32;
			}
			else
				switch (gamemap)
				{
				case 15:
					wminfo.next = 30;
					break;
				case 31:
					wminfo.next = 31;
					break;
				}
		}
		else
		{
			if (gamemap == 33 && critical->havemap33)
			{
				wminfo.next = 2;
			}
			else
			{
				switch (gamemap)
				{
				case 31:
				case 32:
					wminfo.next = 15;
					break;
				default:
					wminfo.next = gamemap;
				}
			}
		}
	}
	else
	{
		if (secretexit)
		{
			if (critical->havee1m10 && gameepisode == 1 && gamemap == 1)
			{
				wminfo.next = 9; // go to secret level E1M10 "Sewers"
			}
			else
			{
				wminfo.next = 8;	// go to secret level
			}
		}
		else if (gamemap == 9)
		{
			// returning from secret level
			switch (gameepisode)
			{
			case 1:
				wminfo.next = 3;
				break;
			case 2:
				wminfo.next = 5;
				break;
			case 3:
			case 5:
				wminfo.next = 6;
				break;
			case 4:
				wminfo.next = 2;
				break;
			}
		}
		else if (critical->havee1m10 && gameepisode == 1 && gamemap == 10)
		{
			wminfo.next = 1; // returning from secret level E1M10 "Sewers"
		}
		else
		{
			wminfo.next = gamemap;			// go to next level
		}
	}

	wminfo.maxkills = totalkills;
	wminfo.maxitems = totalitems;
	wminfo.maxsecret = totalsecret;
	wminfo.maxfrags = 0;

	// Set par time. Exceptions are added for purposes of
	// statcheck regression testing.
	if (gamemode == GameMode::commercial)
	{
		// map33 reads its par time from beyond the cpars[] array
		if (gamemap == 33)
		{
			int cpars32;

			memcpy(&cpars32, DEH_String(GAMMALVL0), sizeof(int));
			cpars32 = LONG(cpars32);

			wminfo.partime = TICRATE * cpars32;
		}
		// support [PARS] sections in BEX files
		else if (bex_cpars[gamemap - 1])
		{
			wminfo.partime = TICRATE * bex_cpars[gamemap - 1];
		}
		// par times for NRFTL
		else if (gamemission == GameMission::pack_nerve)
		{
			wminfo.partime = TICRATE * npars[gamemap - 1];
		}
		else
		{
			wminfo.partime = TICRATE * cpars[gamemap - 1];
		}
	}
	// Doom episode 4 doesn't have a par time, so this overflows into the cpars array.
	else if (gameepisode < 4 || (gameepisode == 4 && crispy->singleplayer) || gameepisode == 5)
	{
		// support [PARS] sections in BEX files
		if (bex_pars[gameepisode][gamemap])
		{
			wminfo.partime = TICRATE * bex_pars[gameepisode][gamemap];
		}
		else
			wminfo.partime = TICRATE * pars[gameepisode][gamemap];
	}
	else
	{
		wminfo.partime = TICRATE * cpars[gamemap];
	}

	wminfo.pnum = consoleplayer;

	for (i = 0; i < MAX_PLAYERS; ++i)
	{
		wminfo.plyr[i].in = playeringame[i];
		wminfo.plyr[i].skills = players[i].killcount;
		wminfo.plyr[i].sitems = players[i].itemcount;
		wminfo.plyr[i].ssecret = players[i].secretcount;
		wminfo.plyr[i].stime = leveltime;
		memcpy(wminfo.plyr[i].frags, players[i].frags, sizeof(wminfo.plyr[i].frags));
	}

	// CPhipps - total time for all completed levels
	// cph - modified so that only whole seconds are added to the totalleveltimes
	// value; so our total is compatible with the "naive" total of just adding
	// the times in seconds shown for each level. Also means our total time
	// will agree with Compet-n.
	wminfo.totaltimes = (totalleveltimes += (leveltime - leveltime % TICRATE));

	gamestate = GameState_t::GS_INTERMISSION;
	viewactive = false;
	automapactive = false;

	// no statdump output for ExM8
	if (gamemode == GameMode::commercial || gamemap != 8)
	{
		StatCopy(&wminfo);
	}

	WI_Start(&wminfo);
}

void G_WorldDone()
{
	gameaction = GameAction_t::ga_worlddone;

	if (secretexit)
	{
		// special-casing for E1M10 "Sewers" support
		// i.e. avoid drawing the splat for E1M9 already
		if (!crispy->havee1m10 || gameepisode != 1 || gamemap != 1)
		{
			players[consoleplayer].didsecret = true;
		}
	}

	if (gamemission == GameMission::pack_nerve)
	{
		switch (gamemap)
		{
		case 8:
			F_StartFinale();
			break;
		}
	}
	else if (gamemission == GameMission::pack_master)
	{
		switch (gamemap)
		{
		case 20:
			if (secretexit)
				break;
		case 21:
			F_StartFinale();
			break;
		}
	}
	else if (gamemode == GameMode::commercial)
	{
		switch (gamemap)
		{
		case 15:
		case 31:
			if (!secretexit)
			{
				break;
			}
		case 6:
		case 11:
		case 20:
		case 30:
			F_StartFinale();
			break;
		}
	}
	// display tally screen after ExM8
	else if (gamemap == 8 || (gameversion == GameVersion::exe_chex && gamemap == 5))
	{
		gameaction = GameAction_t::ga_victory;
	}
}

void G_DoWorldDone()
{
	gamestate = GameState_t::GS_LEVEL;
	gamemap = wminfo.next + 1;
	G_DoLoadLevel();
	gameaction = GameAction_t::ga_nothing;
	viewactive = true;
}

//
// G_InitFromSavegame
// Can be called by the startup code or the menu task.
//
extern bool setsizeneeded;
void R_ExecuteSetViewSize();

void G_LoadGame(::std::string name)
{
	M_StringCopy(savename, name, sizeof(savename));
	gameaction = GameAction_t::ga_loadgame;
}

TimeType savedleveltime = 0; // moved here for level time logging
void G_DoLoadGame()
{

	// loaded game must always be single player.
	// Needed for ability to use a further game loading, as well as
	// cheat codes and other single player only specifics.
	if (startloadgame == -1)
	{
		netdemo = false;
		netgame = false;
		deathmatch = false;
	}
	gameaction = GameAction_t::ga_nothing;

	save_stream = fopen(savename, "rb");

	if (save_stream == NULL)
	{
		I_Error("Could not load savegame %s", savename);
	}

	// read extended savegame data
	if (crispy->extsaveg)
	{
		// first pass, read "savewadfilename"
		P_ReadExtendedSaveGameData(0);
	}
	// check if WAD file is valid to restore saved map
	if (savewadfilename)
	{
		// strings are not equal
		if (!savemaplumpinfo || iequals(savewadfilename, W_WadNameForLump(savemaplumpinfo)))
		{
			M_ForceLoadGame();
			fclose(save_stream);
			return;
		}
		else if (savewadfilename != W_WadNameForLump(savemaplumpinfo))
		{
			free(savewadfilename);
		}
	}
	savewadfilename = NULL;

	savegame_error = false;

	if (!P_ReadSaveGameHeader())
	{
		// indicate game version mismatch
		extern void M_LoadGameVerMismatch();
		M_LoadGameVerMismatch();
		fclose(save_stream);
		return;
	}

	savedleveltime = leveltime;

	// load a base level
	G_InitNew(gameskill, gameepisode, gamemap);

	leveltime = savedleveltime;
	savedleveltime = 0;

	// dearchive all the modifications
	P_UnArchivePlayers();
	P_UnArchiveWorld();
	P_UnArchiveThinkers();
	P_UnArchiveSpecials();
	P_RestoreTargets(); // restore mobj->target and mobj->tracer pointers

	if (!P_ReadSaveGameEOF())
	{
		I_Error("Bad savegame");
	}

	// read more extended savegame data
	if (crispy->extsaveg)
	{
		P_ReadExtendedSaveGameData(1);
	}

	fclose(save_stream);

	if (setsizeneeded)
	{
		R_ExecuteSetViewSize();
	}

	// draw the pattern into the back screen
	R_FillBackScreen();

	// if the player is dead in this savegame,
	// do not consider it for reload
	if (players[consoleplayer].health <= 0)
	{
		G_ClearSavename();
	}

	// once loaded from the command line,
	// the next savegame will be loaded from the menu
	startloadgame = -1;
}

// Called by the menu task.
// Description is a 24 byte text string
void G_SaveGame(int slot, ::std::string description)
{
	savegameslot = slot;
	M_StringCopy(savedescription, description, sizeof(savedescription));
	sendsave = true;
}

void G_DoSaveGame()
{
	::std::string savegame_file{P_SaveGameFile(savegameslot)};
	::std::string temp_savegame_file{P_TempSaveGameFile()};
	::std::string recovery_savegame_file;

	// Open the savegame file for writing. We write to a temporary file and then rename it at the end if it was successfully written.
	// This prevents an existing savegame from being overwritten by a corrupted one, or if a savegame buffer overrun occurs.
	save_stream = fopen(temp_savegame_file, "wb");

	if (save_stream == NULL)
	{
		// Failed to save the game, so we're going to have to abort. But to be nice, save to somewhere else before we call I_Error().
		recovery_savegame_file = M_TempFile("recovery.dsg");
		save_stream = fopen(recovery_savegame_file, "wb");
		if (save_stream == NULL)
		{
			I_Error("Failed to open either '%s' or '%s' to write savegame.", temp_savegame_file, recovery_savegame_file);
		}
	}

	savegame_error = false;

	P_WriteSaveGameHeader(savedescription);

	// some logging when saving
	{
		const TimeType ltime = leveltime / TICRATE;
		const TimeType ttime = (totalleveltimes + leveltime) / TICRATE;
		extern ::std::string skilltable[];

		fprintf(stderr, "G_DoSaveGame: Episode %d, Map %d, %s, Time %d:%02d:%02d, Total %d:%02d:%02d.\n",
			gameepisode, gamemap, skilltable[BETWEEN(0, 5, (int)gameskill + 1)],
			ltime / 3600, (ltime % 3600) / 60, ltime % 60, ttime / 3600, (ttime % 3600) / 60, ttime % 60);
	}

	P_ArchivePlayers();
	P_ArchiveWorld();
	P_ArchiveThinkers();
	P_ArchiveSpecials();

	P_WriteSaveGameEOF();
	// write extended savegame data
	if (crispy->extsaveg)
	{
		P_WriteExtendedSaveGameData();
	}

	// unconditionally disable savegame and demo limits
	/*
	// Enforce the same savegame size limit as in Vanilla Doom,
	// except if the vanilla_savegame_limit setting is turned off.

	if (vanilla_savegame_limit && ftell(save_stream) > SAVEGAMESIZE)
	{
		I_Error("Savegame buffer overrun");
	}
	*/

	// Finish up, close the savegame file.

	fclose(save_stream);

	if (!recovery_savegame_file.empty())
	{
		// We failed to save to the normal location, but we wrote a
		// recovery file to the temp directory. Now we can bomb out
		// with an error.
		I_Error("Failed to open savegame file '%s' for writing.\nBut your game has been saved to '%s' for recovery.",
			temp_savegame_file, recovery_savegame_file);
	}

	// Now rename the temporary savegame file to the actual savegame
	// file, overwriting the old savegame if there was one there.

	remove(savegame_file.c_str());
	rename(temp_savegame_file.c_str(), savegame_file.c_str());

	gameaction = GameAction_t::ga_nothing;
	M_StringCopy(savedescription, "", sizeof(savedescription));
	M_StringCopy(savename, savegame_file, sizeof(savename));

	players[consoleplayer].message = DEH_String(GGSAVED);

	// draw the pattern into the back screen
	R_FillBackScreen();
}

// Can be called by the startup code or the menu task,
// consoleplayer, displayplayer, playeringame[] should be set.
SkillType d_skill;
int d_episode;
int d_map;

void G_DeferedInitNew(SkillType skill, int episode, int map)
{
	d_skill = skill;
	d_episode = episode;
	d_map = map;
	G_ClearSavename();
	gameaction = GameAction_t::ga_newgame;

	// if a new game is started during demo recording, start a new demo
	if (demorecording)
	{
		// reset IDDT cheat when re-starting map during demo recording
		void AM_ResetIDDTcheat();
		AM_ResetIDDTcheat();

		G_CheckDemoStatus();
		Z_Free(demoname);

		G_RecordDemo(orig_demoname);
		G_BeginRecording();
	}
}

void G_DoNewGame()
{
	demoplayback = false;
	netdemo = false;
	netgame = false;
	deathmatch = false;
	playeringame[1] = playeringame[2] = playeringame[3] = 0;
	// do not reset -respawn, -fast and -nomonsters parameters
	/*
	respawnparm = false;
	fastparm = false;
	nomonsters = false;
	*/
	consoleplayer = 0;
	G_InitNew(d_skill, d_episode, d_map);
	gameaction = GameAction_t::ga_nothing;
}

void G_InitNew(SkillType skill, int episode, int map)
{
	::std::string skytexturename;
	int i;
	// make sure "fast" parameters are really only applied once
	static bool fast_applied;

	if (paused)
	{
		paused = false;
		S_ResumeSound();
	}

	/*
	// Note: This commented-out block of code was added at some point
	// between the DOS version(s) and the Doom source release. It isn't
	// found in disassemblies of the DOS version and causes IDCLEV and
	// the -warp command line parameter to behave differently.
	// This is left here for posterity.

	// This was quite messy with SPECIAL and commented parts.
	// Supposedly hacks to make the latest edition work.
	// It might not work properly.
	if (episode < 1)
		episode = 1;

	if ( gamemode == GameMode::retail )
	{
		if (episode > 4)
	episode = 4;
	}
	else if ( gamemode == GameMode::shareware )
	{
		if (episode > 1)
		episode = 1;	// only start episode 1 on shareware
	}
	else
	{
		if (episode > 3)
	episode = 3;
	}
	*/

	if (skill > SkillType::sk_nightmare)
		skill = SkillType::sk_nightmare;

	// if NRFTL is not available, "episode 2" may mean The Master Levels ("episode 3")
	if (gamemode == GameMode::commercial)
	{
		if (episode < 1)
		{
			episode = 1;
		}
		else if (episode == 2 && !crispy->havenerve)
		{
			episode = crispy->havemaster ? 3 : 1;
		}
	}

	// only fix episode/map if it doesn't exist
	if (P_GetNumForMap(episode, map, false) < 0)
	{
		if (gameversion >= GameVersion::exe_ultimate)
		{
			if (episode == 0)
			{
				episode = 4;
			}
		}
		else
		{
			if (episode < 1)
			{
				episode = 1;
			}
			if (episode > 3)
			{
				episode = 3;
			}
		}

		if (episode > 1 && gamemode == GameMode::shareware)
		{
			episode = 1;
		}

		if (map < 1)
		{
			map = 1;
		}

		if ((map > 9) && (gamemode != GameMode::commercial))
		{
			// support E1M10 "Sewers"
			if (!crispy->havee1m10 || episode != 1)
			{
				map = 9;
			}
			else
			{
				map = 10;
			}
		}
	}

	M_ClearRandom();

	if (skill == SkillType::sk_nightmare || respawnparm)
	{
		respawnmonsters = true;
	}
	else
	{
		respawnmonsters = false;
	}

	// make sure "fast" parameters are really only applied once
	if ((fastparm || skill == SkillType::sk_nightmare) && !fast_applied)
	{
		for (i = ::std::size_t(statenum_t::S_SARG_RUN1); i <= ::std::size_t(statenum_t::S_SARG_PAIN2); ++i)
		{
			// Fix infinite loop caused by Demon speed bug
			if (states[i].tics > 1)
			{
				states[i].tics >>= 1;
			}
		}
		mobjinfo[::std::size_t(mobjtype_t::MT_BRUISERSHOT)].speed = 20 * FRACUNIT;
		mobjinfo[::std::size_t(mobjtype_t::MT_HEADSHOT)].speed = 20 * FRACUNIT;
		mobjinfo[::std::size_t(mobjtype_t::MT_TROOPSHOT)].speed = 20 * FRACUNIT;
		fast_applied = true;
	}
	else if (!fastparm && skill != SkillType::sk_nightmare && fast_applied)
	{
		for (i = ::std::size_t(statenum_t::S_SARG_RUN1); i <= ::std::size_t(statenum_t::S_SARG_PAIN2); ++i)
		{
			states[i].tics <<= 1;
		}
		mobjinfo[::std::size_t(mobjtype_t::MT_BRUISERSHOT)].speed = 15 * FRACUNIT;
		mobjinfo[::std::size_t(mobjtype_t::MT_HEADSHOT)].speed = 10 * FRACUNIT;
		mobjinfo[::std::size_t(mobjtype_t::MT_TROOPSHOT)].speed = 10 * FRACUNIT;
		fast_applied = false;
	}

	// force players to be initialized upon first level load
	for (i = 0; i < MAX_PLAYERS; ++i)
	{
		players[i].playerstate = PlayerState::reborn;
	}

	usergame = true;				// will be set false if a demo
	paused = false;
	demoplayback = false;
	automapactive = false;
	viewactive = true;
	gameepisode = episode;
	gamemap = map;
	gameskill = skill;

	// CPhipps - total time for all completed levels
	totalleveltimes = 0;
	defdemotics = 0;
	demostarttic = gametic; // fix revenant internal demo bug

	// Set the sky to use.
	//
	// Note: This IS broken, but it is how Vanilla Doom behaves.
	// See http://doomwiki.org/wiki/Sky_never_changes_in_Doom_II.
	//
	// Because we set the sky here at the start of a game, not at the
	// start of a level, the sky texture never changes unless we
	// restore from a saved game. This was fixed before the Doom
	// source release, but this IS the way Vanilla DOS Doom behaves.

	if (gamemode == GameMode::commercial)
	{
		skytexturename = DEH_String("SKY3");
		skytexture = R_TextureNumForName(skytexturename);
		if (gamemap < 21)
		{
			skytexturename = DEH_String(gamemap < 12 ? "SKY1" : "SKY2");
			skytexture = R_TextureNumForName(skytexturename);
		}
	}
	else
	{
		switch (gameepisode)
		{
		default:
		case 1:
			skytexturename = "SKY1";
			break;
		case 2:
			skytexturename = "SKY2";
			break;
		case 3:
			skytexturename = "SKY3";
			break;
		case 4:		// Special Edition sky
			skytexturename = "SKY4";
			break;
		case 5:		// Sigil
			skytexturename = "SKY5_ZD";
			if (R_CheckTextureNumForName(DEH_String(skytexturename)) == -1)
			{
				skytexturename = "SKY3";
			}
			break;
		}
		skytexturename = DEH_String(skytexturename);
		skytexture = R_TextureNumForName(skytexturename);
	}

	G_DoLoadLevel();
}

constexpr size_t DEMOMARKER{0x80};

// demo progress bar and timer widget
TimeType defdemotics = 0;
TimeType deftotaldemotics;
// moved here
static ::std::string defdemoname;

void G_ReadDemoTiccmd(ticcmd_t* cmd)
{
	if (*demo_p == DEMOMARKER)
	{
		last_cmd = cmd; // remember last cmd to track joins

		// end of demo data stream
		G_CheckDemoStatus();
		return;
	}

	// if demo playback is quit by pressing 'q',
	// stay in the game, hand over controls to the player and
	// continue recording the demo under a different name
	if (gamekeydown[key_demo_quit] && singledemo && !netgame)
	{
		byte* actualbuffer = demobuffer;
		::std::string actualname = ::std::string(defdemoname);

		gamekeydown[key_demo_quit] = false;

		// find a new name for the continued demo
		G_RecordDemo(actualname);
		free(actualname);

		// discard the newly allocated demo buffer
		Z_Free(demobuffer);
		demobuffer = actualbuffer;

		last_cmd = cmd; // remember last cmd to track joins

		// continue recording
		G_CheckDemoStatus();
		return;
	}

	cmd->forwardmove = ((char)*demo_p);
	++demo_p;
	cmd->sidemove = ((char)*demo_p);
	++demo_p;

	// If this is a longtics demo, read back in higher resolution

	if (longtics)
	{
		cmd->angleturn = *demo_p;
		++demo_p;
		cmd->angleturn |= (*demo_p) << 8;
		++demo_p;
	}
	else
	{
		cmd->angleturn = ((unsigned char)*demo_p) << 8;
		++demo_p;
	}

	cmd->buttons = (unsigned char)*demo_p;
	++demo_p;

	// increase demo tics counter
	// applies to both recording and playback,
	// because G_WriteDemoTiccmd() calls G_ReadDemoTiccmd() once
	++defdemotics;
}

// Increase the size of the demo buffer to allow unlimited demos

static void IncreaseDemoBuffer()
{
	int current_length;
	byte* new_demobuffer;
	byte* new_demop;
	int new_length;

	// Find the current size
	current_length = demoend - demobuffer;

	// Generate a new buffer twice the size
	new_length = current_length * 2;

	new_demobuffer = Z_Malloc<decltype(new_demobuffer)>(new_length, pu_tags_t::PU_STATIC, 0);
	new_demop = new_demobuffer + (demo_p - demobuffer);

	// Copy over the old data
	memcpy(new_demobuffer, demobuffer, current_length);

	// Free the old buffer and point the demo pointers at the new buffer.
	Z_Free(demobuffer);

	demobuffer = new_demobuffer;
	demo_p = new_demop;
	demoend = demobuffer + new_length;
}

void G_WriteDemoTiccmd(ticcmd_t* cmd)
{
	byte* demo_start;

	if (gamekeydown[key_demo_quit])			// press q to end demo recording
	{
		G_CheckDemoStatus();
	}

	demo_start = demo_p;

	*demo_p = cmd->forwardmove;
	++demo_p;
	*demo_p = cmd->sidemove;
	++demo_p;

	// If this is a longtics demo, record in higher resolution
	if (longtics)
	{
		*demo_p = (cmd->angleturn & 0xff);
		++demo_p;
		*demo_p = (cmd->angleturn >> 8) & 0xff;
		++demo_p;
	}
	else
	{
		*demo_p = cmd->angleturn >> 8;
		++demo_p;
	}

	*demo_p = cmd->buttons;
	++demo_p;

	// reset demo pointer back
	demo_p = demo_start;

	if (demo_p > demoend - 16)
	{
		// unconditionally disable savegame and demo limits
		/*
		if (vanilla_demo_limit)
		{
			// no more space
			G_CheckDemoStatus();
			return;
		}
		else
		*/
		{
			// Vanilla demo limit disabled: unlimited demo lengths!
			IncreaseDemoBuffer();
		}
	}

	G_ReadDemoTiccmd(cmd);			// make SURE it is exactly the same
}

void G_RecordDemo(::std::string name)
{
	size_t demoname_size;
	int i;
	int maxsize;

	// demo file name suffix counter
	static unsigned j = 0;
	FILE* fp = NULL;

	// the name originally chosen for the demo, i.e. without "-00000"
	if (!orig_demoname)
	{
		orig_demoname = name;
	}

	usergame = false;
	demoname_size = strlen(name) + 5 + 6; // + 6 for "-00000"
	demoname = Z_Malloc<decltype(demoname)>(demoname_size, pu_tags_t::PU_STATIC, NULL);
	M_snprintf(demoname, demoname_size, "%s.lmp", name);

	// prevent overriding demos by adding a file name suffix
	for (; j <= 99999 && (fp = fopen(demoname, "rb")) != NULL; ++j)
	{
		M_snprintf(demoname, demoname_size, "%s-%05d.lmp", name, j);
		fclose(fp);
	}

	maxsize = 0x20000;

	// Specify the demo buffer size (KiB)
	i = M_CheckParmWithArgs("-maxdemo", 1);
	if (i)
	{
		maxsize = atoi(myargv[i + 1]) * 1024;
	}
	demobuffer = Z_Malloc<decltype(demobuffer)>(maxsize, pu_tags_t::PU_STATIC, NULL);
	demoend = demobuffer + maxsize;

	demorecording = true;
}

// Get the demo version code appropriate for the version set in gameversion.
int G_VanillaVersionCode()
{
	switch (gameversion)
	{
	case GameVersion::exe_doom_1_666:
		return 106;
	case GameVersion::exe_doom_1_7:
		return 107;
	case GameVersion::exe_doom_1_8:
		return 108;
	case GameVersion::exe_doom_1_9:
	default: // All other versions are variants on v1.9:
		return 109;
	}
}

void G_BeginRecording()
{
	int i;

	demo_p = demobuffer;

	// Record a high resolution "Doom 1.91" demo.
	longtics = D_NonVanillaRecord(M_ParmExists("-longtics"), "Doom 1.91 demo format");

	// If not recording a longtics demo, record in low res
	lowres_turn = !longtics;

	if (longtics)
	{
		*demo_p = DOOM_191_VERSION;
	}
	else if (gameversion > GameVersion::exe_doom_1_2)
	{
		*demo_p = G_VanillaVersionCode();
	}

	*demo_p = gameskill;
	++demo_p;
	*demo_p = gameepisode;
	++demo_p;
	*demo_p = gamemap;
	++demo_p;
	if (longtics || gameversion > GameVersion::exe_doom_1_2)
	{
		*demo_p = deathmatch;
		++demo_p;
		*demo_p = respawnparm;
		++demo_p;
		*demo_p = fastparm;
		++demo_p;
		*demo_p = nomonsters;
		++demo_p;
		*demo_p = consoleplayer;
		++demo_p;
	}

	for (i = 0; i < MAX_PLAYERS; ++i)
	{
		*demo_p = playeringame[i];
		++demo_p;
	}
}

void G_DeferedPlayDemo(::std::string name)
{
	defdemoname = name;
	gameaction = GameAction_t::ga_playdemo;

	// fast-forward demo up to the desired map
	// in demo warp mode or to the end of the demo in continue mode
	if (crispy->demowarp || demorecording)
	{
		nodrawers = true;
		singletics = true;
	}
}

// Generate a string describing a demo version
static ::std::string DemoVersionDescription(int version)
{
	static char resultbuf[16];

	switch (version)
	{
	case 104:
		return "v1.4";
	case 105:
		return "v1.5";
	case 106:
		return "v1.6/v1.666";
	case 107:
		return "v1.7/v1.7a";
	case 108:
		return "v1.8";
	case 109:
		return "v1.9";
	case 111:
		return "v1.91 hack demo?";
	default:
		break;
	}

	// Unknown version. Perhaps this is a pre-v1.4 IWAD? If the version
	// byte is in the range 0-4 then it can be a v1.0-v1.2 demo.
	if (version >= 0 && version <= 4)
	{
		return "v1.0/v1.1/v1.2";
	}
	else
	{
		M_snprintf(resultbuf, sizeof(resultbuf), "%i.%i (unknown)", version / 100, version % 100);
		return resultbuf;
	}
}

void G_DoPlayDemo()
{
	SkillType skill;
	int i;
	int lumpnum;
	int episode;
	int map;
	int demoversion;
	bool olddemo = false;
	int lumplength;

	// in demo continue mode free the obsolete demo buffer
	// of size 'maxsize' previously allocated in G_RecordDemo()
	if (demorecording)
	{
		Z_Free(demobuffer);
	}

	lumpnum = W_GetNumForName(defdemoname);
	gameaction = GameAction_t::ga_nothing;
	demobuffer = W_CacheLumpNum<byte>(lumpnum, pu_tags_t::PU_STATIC);
	demo_p = demobuffer;

	// ignore empty demo lumps
	lumplength = W_LumpLength(lumpnum);
	if (lumplength < 0xd)
	{
		demoplayback = true;
		G_CheckDemoStatus();
		return;
	}

	demoversion = *demo_p;
	++demo_p;

	if (demoversion >= 0 && demoversion <= 4)
	{
		olddemo = true;
		--demo_p;
	}

	longtics = false;

	// Longtics demos use the modified format that is generated by cph's
	// hacked "v1.91" doom exe. This is a non-vanilla extension.
	if (D_NonVanillaPlayback(demoversion == DOOM_191_VERSION, lumpnum, "Doom 1.91 demo format"))
	{
		longtics = true;
	}
	else if (demoversion != G_VanillaVersionCode() && !(gameversion <= GameVersion::exe_doom_1_2 && olddemo))
	{
		::std::string message = "Demo is from a different game version!\n"
			"(read %i, should be %i)\n"
			"\n"
			"*** You may need to upgrade your version "
			"of Doom to v1.9. ***\n"
			"	See: https://www.doomworld.com/classicdoom"
			"/info/patches.php\n"
			"	This appears to be %s.";

		if (singledemo)
		{
			I_Error(message, demoversion, G_VanillaVersionCode(), DemoVersionDescription(demoversion));
		}
		// make non-fatal
		else
		{
			fprintf(stderr, message, demoversion, G_VanillaVersionCode(), DemoVersionDescription(demoversion));
			fprintf(stderr, "\n");
			demoplayback = true;
			G_CheckDemoStatus();
			return;
		}
	}

	skill = *demo_p;
	++demo_p;
	episode = *demo_p;
	++demo_p;
	map = *demo_p;
	++demo_p;
	if (!olddemo)
	{
		deathmatch = *demo_p;
		++demo_p;
		respawnparm = *demo_p;
		++demo_p;
		fastparm = *demo_p;
		++demo_p;
		nomonsters = *demo_p;
		++demo_p;
		consoleplayer = *demo_p;
		++demo_p;
	}
	else
	{
		deathmatch = 0;
		respawnparm = 0;
		fastparm = 0;
		nomonsters = 0;
		consoleplayer = 0;
	}

	for (i = 0; i < MAX_PLAYERS; ++i)
	{
		playeringame[i] = *demo_p;
		++demo_p;
	}

	if (playeringame[1] || M_CheckParm("-solo-net") > 0 || M_CheckParm("-netdemo") > 0)
	{
		netgame = true;
		netdemo = true;
		// impossible to continue a multiplayer demo
		demorecording = false;
	}

	// don't spend a lot of time in loadlevel
	precache = false;
	// support playing demos from savegames
	if (startloadgame >= 0)
	{
		M_StringCopy(savename, P_SaveGameFile(startloadgame), sizeof(savename));
		G_DoLoadGame();
	}
	else
	{
		G_InitNew(skill, episode, map);
	}
	precache = true;
	starttime = I_GetTime();
	demostarttic = gametic; // fix revenant internal demo bug

	usergame = false;
	demoplayback = true;
	// update the "singleplayer" variable
	CheckCrispySingleplayer(!demorecording && !demoplayback && !netgame);

	// demo progress bar
	{
		int i;
		int numplayersingame = 0;
		byte* demo_ptr = demo_p;

		for (i = 0; i < MAX_PLAYERS; ++i)
		{
			if (playeringame[i])
			{
				++numplayersingame;
			}
		}

		deftotaldemotics = defdemotics = 0;

		while (*demo_ptr != DEMOMARKER && (demo_ptr - demobuffer) < lumplength)
		{
			demo_ptr += numplayersingame * (longtics ? 5 : 4);
			++deftotaldemotics;
		}
	}
}

void G_TimeDemo(::std::string name)
{
	// Disable rendering the screen entirely.
	nodrawers = M_CheckParm("-nodraw");

	timingdemo = true;
	singletics = true;

	defdemoname = name;
	gameaction = GameAction_t::ga_playdemo;
}

// Called after a death or level completion to allow demos to be cleaned up
// Returns true if a new demo loop action will take place
bool G_CheckDemoStatus()
{
	TimeType endtime;

	// catch the last cmd to track joins
	ticcmd_t* cmd = last_cmd;
	last_cmd = NULL;

	if (timingdemo)
	{
		float fps;
		TimeType realtics;

		endtime = I_GetTime();
		realtics = endtime - starttime;
		fps = ((float)gametic * TICRATE) / realtics;

		// Prevent recursive calls
		timingdemo = false;
		demoplayback = false;

		I_Error("timed %i gametics in %i realtics (%f fps)", gametic, realtics, fps);
	}

	if (demoplayback)
	{
		W_ReleaseLumpName(defdemoname);
		demoplayback = false;
		netdemo = false;
		netgame = false;
		deathmatch = false;
		playeringame[1] = playeringame[2] = playeringame[3] = 0;
		// leave game parameters intact when continuing a demo
		if (!demorecording)
		{
			respawnparm = false;
			fastparm = false;
			nomonsters = false;
		}
		consoleplayer = 0;

		// in demo continue mode increase the demo buffer and
		// continue recording once we are done with playback
		if (demorecording)
		{
			demoend = demo_p;
			IncreaseDemoBuffer();

			nodrawers = false;
			singletics = false;

			// start music for the current level
			if (gamestate == GameState_t::GS_LEVEL)
			{
				S_Start();
			}

			// record demo join
			if (cmd != NULL)
			{
				cmd->buttons |= buttoncode::BT_JOIN;
			}

			return true;
		}

		if (singledemo)
		{
			I_Quit();
		}
		else
		{
			D_AdvanceDemo();
		}

		return true;
	}

	if (demorecording)
	{
		*demo_p = DEMOMARKER;
		++demo_p;
		M_WriteFile(demoname, demobuffer, demo_p - demobuffer);
		Z_Free(demobuffer);
		demorecording = false;
		// if a new game is started during demo recording, start a new demo
		if (gameaction != GameAction_t::ga_newgame)
		{
			I_Error("Demo %s recorded", demoname);
		}
		else
		{
			fprintf(stderr, "Demo %s recorded\n", demoname);
		}
	}

	return false;
}
