/**********************************************************************************************************************************************\
	Copyright(C) 1993-1996 Id Software, Inc.
	Copyright(C) 2005-2014 Simon Howard

	This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License
	as published by the Free Software Foundation; either version 2 of the License, or (at your option) any later version.

	This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

	DESCRIPTION:
		Status bar code.
		Does the face/direction indicator animatin.
		Does palette indicators as well (red pain/berserk, bright pickup)
\**********************************************************************************************************************************************/

#include <string>

#include "i_swap.h" // SHORT()
#include "i_system.h"
#include "i_video.h"
#include "z_zone.h"
#include "m_argv.h" // M_ParmExists()
#include "m_misc.h"
#include "m_random.h"
#include "w_wad.h"

#include "deh_main.h"
#include "deh_misc.h"
#include "doomdef.h"
#include "doomkeys.h"

#include "g_game.h"

#include "st_stuff.h"
#include "st_lib.h"
#include "r_local.h"

#include "p_local.h"
#include "p_inter.h"

#include "am_map.h"
#include "m_cheat.h"

#include "s_sound.h"

// Needs access to LFB.
#include "v_video.h"

// State.
#include "doomstat.h"

// Data.
#include "dstrings.h"
#include "sounds.h"

#include "v_trans.h" // colored cheat messages

extern int screenblocks; // for the Crispy HUD
extern bool inhelpscreens; // prevent palette changes

//
// STATUS BAR DATA
//

// Palette indices.
// For damage/bonus red-/gold-shifts
constexpr size_t STARTREDPALS{1};
constexpr size_t STARTBONUSPALS{9};
constexpr size_t NUMREDPALS{8};
constexpr size_t NUMBONUSPALS{4};
// Radiation suit, green shift.
constexpr size_t RADIATIONPAL{13};

// N/256*100% probability
// that the normal face state will change
constexpr size_t ST_FACEPROBABILITY{96};

// For Responder
constexpr size_t ST_TOGGLECHAT{KEY_ENTER};

// Location of status bar
constexpr size_t ST_X{0};
constexpr size_t ST_X2{104};

constexpr size_t ST_FX{143};
constexpr size_t ST_FY{169};

// Should be set to patch width
// for tall numbers later on
#define ST_TALLNUMWIDTH (tallnum[0]->width)

// Number of status faces.
constexpr size_t ST_NUMPAINFACES{5};
constexpr size_t ST_NUMSTRAIGHTFACES{3};
constexpr size_t ST_NUMTURNFACES{2};
constexpr size_t ST_NUMSPECIALFACES{3};

constexpr size_t ST_FACESTRIDE{(ST_NUMSTRAIGHTFACES + ST_NUMTURNFACES + ST_NUMSPECIALFACES)};

constexpr size_t ST_NUMEXTRAFACES{2};

constexpr size_t ST_NUMFACES{(ST_FACESTRIDE * ST_NUMPAINFACES + ST_NUMEXTRAFACES)};

constexpr size_t ST_TURNOFFSET{(ST_NUMSTRAIGHTFACES)};
constexpr size_t ST_OUCHOFFSET{(ST_TURNOFFSET + ST_NUMTURNFACES)};
constexpr size_t ST_EVILGRINOFFSET{(ST_OUCHOFFSET + 1)};
constexpr size_t ST_RAMPAGEOFFSET{(ST_EVILGRINOFFSET + 1)};
constexpr size_t ST_GODFACE{(ST_NUMPAINFACES * ST_FACESTRIDE)};
constexpr size_t ST_DEADFACE{(ST_GODFACE + 1)};

constexpr size_t ST_FACESX{143};
constexpr size_t ST_FACESY{168};

constexpr size_t ST_EVILGRINCOUNT{(2 * TICRATE)};
constexpr size_t ST_STRAIGHTFACECOUNT{(TICRATE / 2)};
constexpr size_t ST_TURNCOUNT{(1 * TICRATE)};
constexpr size_t ST_OUCHCOUNT{(1 * TICRATE)};
constexpr size_t ST_RAMPAGEDELAY{(2 * TICRATE)};

constexpr size_t ST_MUCHPAIN{20};

// Location and size of statistics,
// justified according to widget type.
// Problem is, within which space? STbar? Screen?
// Note: this could be read in by a lump.
//		Problem is, is the stuff rendered
//		into a buffer,
//		or into the frame buffer?

// in non-widescreen mode WIDESCREENDELTA is 0 anyway
#define ST_WIDESCREENDELTA ((screenblocks >= CRISPY_HUD + 3 && (!automapactive || crispy->automapoverlay)) ? WIDESCREENDELTA : 0)

// AMMO number pos.
constexpr size_t ST_AMMOWIDTH{3};
#define ST_AMMOX (44 - ST_WIDESCREENDELTA)
constexpr size_t ST_AMMOY{171};

// HEALTH number pos.
constexpr size_t ST_HEALTHWIDTH{3};
#define ST_HEALTHX (90 - ST_WIDESCREENDELTA)
constexpr size_t ST_HEALTHY{171};

// Weapon pos.
#define ST_ARMSX (111 - ST_WIDESCREENDELTA)
constexpr size_t ST_ARMSY{172};
#define ST_ARMSBGX (104 - ST_WIDESCREENDELTA)
constexpr size_t ST_ARMSBGY{168};
constexpr size_t ST_ARMSXSPACE{12};
constexpr size_t ST_ARMSYSPACE{10};

// Frags pos.
#define ST_FRAGSX (138 - ST_WIDESCREENDELTA)
constexpr size_t ST_FRAGSY{171};
constexpr size_t ST_FRAGSWIDTH{2};

// ARMOR number pos.
constexpr size_t ST_ARMORWIDTH{3};
#define ST_ARMORX (221 + ST_WIDESCREENDELTA)
constexpr size_t ST_ARMORY{171};

// Key icon positions.
constexpr size_t ST_KEY0WIDTH{8};
constexpr size_t ST_KEY0HEIGHT{5};
#define ST_KEY0X (239 + ST_WIDESCREENDELTA)
constexpr size_t ST_KEY0Y{171};
constexpr size_t ST_KEY1WIDTH{ST_KEY0WIDTH};
#define ST_KEY1X (239 + ST_WIDESCREENDELTA)
constexpr size_t ST_KEY1Y{181};
constexpr size_t ST_KEY2WIDTH{ST_KEY0WIDTH};
#define ST_KEY2X (239 + ST_WIDESCREENDELTA)
constexpr size_t ST_KEY2Y{191};

// Ammunition counter.
constexpr size_t ST_AMMO0WIDTH{3};
constexpr size_t ST_AMMO0HEIGHT{6};
#define ST_AMMO0X (288 + ST_WIDESCREENDELTA)
constexpr size_t ST_AMMO0Y{173};
constexpr size_t ST_AMMO1WIDTH{ST_AMMO0WIDTH};
#define ST_AMMO1X (288 + ST_WIDESCREENDELTA)
constexpr size_t ST_AMMO1Y{179};
constexpr size_t ST_AMMO2WIDTH{ST_AMMO0WIDTH};
#define ST_AMMO2X (288 + ST_WIDESCREENDELTA)
constexpr size_t ST_AMMO2Y{191};
constexpr size_t ST_AMMO3WIDTH{ST_AMMO0WIDTH};
#define ST_AMMO3X (288 + ST_WIDESCREENDELTA)
constexpr size_t ST_AMMO3Y{185};

// Indicate maximum ammunition.
// Only needed because backpack exists.
constexpr size_t ST_MAXAMMO0WIDTH{3};
constexpr size_t ST_MAXAMMO0HEIGHT{5};
#define ST_MAXAMMO0X (314 + ST_WIDESCREENDELTA)
constexpr size_t ST_MAXAMMO0Y{173};
constexpr size_t ST_MAXAMMO1WIDTH{ST_MAXAMMO0WIDTH};
#define ST_MAXAMMO1X (314 + ST_WIDESCREENDELTA)
constexpr size_t ST_MAXAMMO1Y{179};
constexpr size_t ST_MAXAMMO2WIDTH{ST_MAXAMMO0WIDTH};
#define ST_MAXAMMO2X (314 + ST_WIDESCREENDELTA)
constexpr size_t ST_MAXAMMO2Y{191};
constexpr size_t ST_MAXAMMO3WIDTH{ST_MAXAMMO0WIDTH};
#define ST_MAXAMMO3X (314 + ST_WIDESCREENDELTA)
constexpr size_t ST_MAXAMMO3Y{185};

// pistol
constexpr size_t ST_WEAPON0X{110};
constexpr size_t ST_WEAPON0Y{172};

// shotgun
constexpr size_t ST_WEAPON1X{122};
constexpr size_t ST_WEAPON1Y{172};

// chain gun
constexpr size_t ST_WEAPON2X{134};
constexpr size_t ST_WEAPON2Y{172};

// missile launcher
constexpr size_t ST_WEAPON3X{110};
constexpr size_t ST_WEAPON3Y{181};

// plasma gun
constexpr size_t ST_WEAPON4X{122};
constexpr size_t ST_WEAPON4Y{181};

// bfg
constexpr size_t ST_WEAPON5X{134};
constexpr size_t ST_WEAPON5Y{181};

// WPNS title
constexpr size_t ST_WPNSX{109};
constexpr size_t ST_WPNSY{191};

// DETH title
constexpr size_t ST_DETHX{109};
constexpr size_t ST_DETHY{191};

//Incoming messages window location
//UNUSED
// #define ST_MSGTEXTX (viewwindowx)
// #define ST_MSGTEXTY (viewwindowy+viewheight-18)
constexpr size_t ST_MSGTEXTX{0};
constexpr size_t ST_MSGTEXTY{0};
// Dimensions given in characters.
constexpr size_t ST_MSGWIDTH{52};
// Or shall I say, in lines?
constexpr size_t ST_MSGHEIGHT{1};

constexpr size_t ST_OUTTEXTX{0};
constexpr size_t ST_OUTTEXTY{6};

// Width, in characters again.
constexpr size_t ST_OUTWIDTH{52};
// Height, in lines.
constexpr size_t ST_OUTHEIGHT{1};

#define ST_MAPTITLEX (ORIGWIDTH - ST_MAPWIDTH * ST_CHATFONTWIDTH)

constexpr size_t ST_MAPTITLEY{0};
constexpr size_t ST_MAPHEIGHT{1};

typedef void (*load_callback_t)(::std::string lumpname, patch_t** variable);

// graphics are drawn to a backing screen and blitted to the real screen
pixel_t* st_backing_screen;

// main player in game
static Player* plyr;

// ST_Start() has just been called
static bool st_firsttime;

// lump number for PLAYPAL
static int lu_palette;

// used for timing
static unsigned st_clock;

// used for making messages go away
static int st_msgcounter = 0;

// used when in chat
static st_chatstateenum_t st_chatstate;

// whether in automap or first-person
static st_stateenum_t st_gamestate;

// whether left-side main status bar is active
static bool st_statusbaron;

// distinguish classic status bar with background and player face from Crispy HUD
static bool st_crispyhud;
static bool st_classicstatusbar;
static bool st_statusbarface;

// whether status bar chat is active
static bool st_chat;

// value of st_chat before message popped up
static bool st_oldchat;

// whether chat window has the cursor on
static bool st_cursoron;

// !deathmatch
static bool st_notdeathmatch;

// !deathmatch && st_statusbaron
static bool st_armson;

// !deathmatch
static bool st_fragson;

// main bar left
static patch_t* sbar;

// main bar right, for doom 1.0
static patch_t* sbarr;

// 0-9, tall numbers
static patch_t* tallnum[10];

// tall % sign
static patch_t* tallpercent;

// 0-9, short, yellow (,different!) numbers
static patch_t* shortnum[10];

// 3 key-cards, 3 skulls
static patch_t* keys[::std::size_t(CardType::NUMCARDS) + 3]; // support combined card and skull keys

// face status patches
static patch_t* faces[ST_NUMFACES];

// face background
static patch_t* faceback;

// main bar right
static patch_t* armsbg;

// weapon ownership patches
static patch_t* arms[6][2];

// ready-weapon widget
static st_number_t w_ready;

// in deathmatch only, summary of frags stats
static st_number_t w_frags;

// health widget
static st_percent_t w_health;

// arms background
static st_binicon_t w_armsbg;

// weapon ownership widgets
static st_multicon_t w_arms[6];
// show SSG availability in the Shotgun slot of the arms widget
static int st_shotguns;

// face status widget
static st_multicon_t w_faces;

// keycard widgets
static st_multicon_t w_keyboxes[3];

// armor widget
static st_percent_t w_armor;

// ammo widgets
static st_number_t w_ammo[4];

// max ammo widgets
static st_number_t w_maxammo[4];

// number of frags so far in deathmatch
static int st_fragscount;

// used to use appopriately pained face
static int st_oldhealth = -1;

// used for evil grin
static bool oldweaponsowned[::std::size_t(WeaponType::NUMWEAPONS)];

// count until face changes
static int st_facecount = 0;

// current face index, used by w_faces
static int st_faceindex = 0;

// holds key-type for each key box on bar
static int keyboxes[3];
// blinking key or skull in the status bar
int st_keyorskull[3];

// a random number per tick
static int st_randomnumber;

cheatseq_t cheat_mus = CHEAT("idmus", 2);
cheatseq_t cheat_god = CHEAT("iddqd", 0);
cheatseq_t cheat_ammo = CHEAT("idkfa", 0);
cheatseq_t cheat_ammonokey = CHEAT("idfa", 0);
cheatseq_t cheat_noclip = CHEAT("idspispopd", 0);
cheatseq_t cheat_commercial_noclip = CHEAT("idclip", 0);

cheatseq_t cheat_powerup[8] = // idbehold0
{
	CHEAT("idbeholdv", 0),
	CHEAT("idbeholds", 0),
	CHEAT("idbeholdi", 0),
	CHEAT("idbeholdr", 0),
	CHEAT("idbeholda", 0),
	CHEAT("idbeholdl", 0),
	CHEAT("idbehold", 0),
	CHEAT("idbehold0", 0), // idbehold0
};

cheatseq_t cheat_choppers = CHEAT("idchoppers", 0);
cheatseq_t cheat_clev = CHEAT("idclev", 2);
cheatseq_t cheat_mypos = CHEAT("idmypos", 0);

// pseudo cheats to eat up the first digit typed after a cheat expecting two parameters
cheatseq_t cheat_mus1 = CHEAT("idmus", 1);
cheatseq_t cheat_clev1 = CHEAT("idclev", 1);

// new cheats
cheatseq_t cheat_weapon = CHEAT("tntweap", 1);
cheatseq_t cheat_massacre = CHEAT("tntem", 0); // PrBoom+
cheatseq_t cheat_massacre2 = CHEAT("killem", 0); // MBF
cheatseq_t cheat_massacre3 = CHEAT("fhhall", 0); // Doom95
cheatseq_t cheat_hom = CHEAT("tnthom", 0);
cheatseq_t cheat_notarget = CHEAT("notarget", 0); // PrBoom+
cheatseq_t cheat_notarget2 = CHEAT("fhshh", 0); // Doom95
cheatseq_t cheat_spechits = CHEAT("spechits", 0);
cheatseq_t cheat_nomomentum = CHEAT("nomomentum", 0);
cheatseq_t cheat_showfps = CHEAT("showfps", 0);
cheatseq_t cheat_showfps2 = CHEAT("idrate", 0); // PrBoom+
cheatseq_t cheat_goobers = CHEAT("goobers", 0);
cheatseq_t cheat_version = CHEAT("version", 0); // Russian Doom
cheatseq_t cheat_skill = CHEAT("skill", 0);
static char msg[ST_MSGWIDTH];

// restrict cheat usage
static inline int cht_CheckCheatSP(cheatseq_t* cht, char key)
{
	if (!cht_CheckCheat(cht, key))
	{
		return false;
	}
	else if (!crispy->singleplayer)
	{
		plyr->message = "Cheater!";
		return false;
	}
	return true;
}

//
// STATUS BAR CODE
//
void ST_Stop();

void ST_refreshBackground(bool force)
{

	if (st_classicstatusbar || force)
	{
		V_UseBuffer(st_backing_screen);

		// this is our own local copy of R_FillBackScreen() to
		// fill the entire background of st_backing_screen with the bezel pattern,
		// so it appears to the left and right of the status bar in widescreen mode
		if ((SCREENWIDTH >> crispy->hires) != ST_WIDTH)
		{
			int x;
			int y;
			byte* src;
			pixel_t* dest;
			::std::string name = (gamemode == GameMode::commercial) ? DEH_String("GRNROCK") : DEH_String("FLOOR7_2");

			src = W_CacheLumpName<patch_t>(name, pu_tags_t::PU_CACHE);
			dest = st_backing_screen;

			for (y = SCREENHEIGHT - (ST_HEIGHT << crispy->hires); y < SCREENHEIGHT; ++y)
			{
				for (x = 0; x < SCREENWIDTH; ++x)
				{
#ifndef CRISPY_TRUECOLOR
					* dest = src[((y & 63) << 6) + (x & 63)];
					++dest;
#else
					* dest = colormaps[src[((y & 63) << 6) + (x & 63)]];
					++dest;
#endif
				}
			}

			// preserve bezel bottom edge
			if (scaledviewwidth == SCREENWIDTH)
			{
				patch_t* const patch = W_CacheLumpName<patch_t>(DEH_String("brdr_b"), pu_tags_t::PU_CACHE);

				for (x = 0; x < WIDESCREENDELTA; x += 8)
				{
					V_DrawPatch(x - WIDESCREENDELTA, 0, patch);
					V_DrawPatch(ORIGWIDTH + WIDESCREENDELTA - x - 8, 0, patch);
				}
			}
		}

		// center unity rerelease wide status bar
		if (sbar->width > ORIGWIDTH && sbar->leftoffset == 0)
		{
			V_DrawPatch(ST_X + (ORIGWIDTH - sbar->width) / 2, 0, sbar);
		}
		else
		{
			V_DrawPatch(ST_X, 0, sbar);
		}

		// draw right side of bar if needed (Doom 1.0)
		if (sbarr)
		{
			V_DrawPatch(ST_ARMSBGX, 0, sbarr);
		}

		// back up arms widget background
		if (!deathmatch)
		{
			V_DrawPatch(ST_ARMSBGX, 0, armsbg);
		}

		if (netgame)
		{
			V_DrawPatch(ST_FX, 0, faceback);
		}

		V_RestoreBuffer();

		// copy entire SCREENWIDTH, to preserve the pattern to the left and right of the status bar in widescren mode
		if (!force)
		{
			V_CopyRect(ST_X, 0, st_backing_screen, SCREENWIDTH >> crispy->hires, ST_HEIGHT, ST_X, ST_Y);
		}
		else if (WIDESCREENDELTA > 0 && !st_firsttime)
		{
			V_CopyRect(0, 0, st_backing_screen, WIDESCREENDELTA, ST_HEIGHT, 0, ST_Y);
			V_CopyRect(ORIGWIDTH + WIDESCREENDELTA, 0, st_backing_screen, WIDESCREENDELTA, ST_HEIGHT, ORIGWIDTH + WIDESCREENDELTA, ST_Y);
		}
	}

}

// adapted from boom202s/M_CHEAT.C:467-498
static int ST_cheat_massacre()
{
	int killcount = 0;
	thinker_t* th;
	extern int numbraintargets;
	extern void A_PainDie(MapObject*);

	for (th = thinkercap.next; th != &thinkercap; th = th->next)
	{
		if (th->function.acp1 == (actionf_p1)P_MobjThinker)
		{
			MapObject* mo = (MapObject*)th;

			if ((int)mo->flags & (int)mobjflag_e::MF_COUNTKILL || mo->type == mobjtype_t::MT_SKULL)
			{
				if (mo->health > 0)
				{
					P_DamageMobj(mo, NULL, NULL, 10000);
					++killcount;
				}
				if (mo->type == mobjtype_t::MT_PAIN)
				{
					A_PainDie(mo);
					P_SetMobjState(mo, statenum_t::S_PAIN_DIE6);
				}
			}
		}
	}

	// disable brain spitters
	numbraintargets = -1;

	return killcount;
}

// trigger all special lines available on the map
static int ST_cheat_spechits()
{
	int speciallines = 0;
	bool origcards[::std::size_t(CardType::NUMCARDS)];
	line_t dummy;

	// temporarily give all keys
	for (size_t i{0}; i < ::std::size_t(CardType::NUMCARDS); ++i)
	{
		origcards[i] = plyr->cards[i];
		plyr->cards[i] = true;
	}

	for (size_t i{0}; i < numlines; ++i)
	{
		if (lines[i].special)
		{
			// do not trigger level exit switches/lines or teleporters
			if (lines[i].special == 11 || lines[i].special == 51 ||
				lines[i].special == 52 || lines[i].special == 124 ||
				lines[i].special == 39 || lines[i].special == 97)
			{
				continue;
			}

			// special without tag --> DR linedef type
			// do not change door direction if it is already moving
			if (lines[i].tag == 0 &&
				lines[i].sidenum[1] != NO_INDEX &&
				sides[lines[i].sidenum[1]].sector->specialdata)
			{
				continue;
			}

			P_CrossSpecialLine(i, 0, plyr);
			P_ShootSpecialLine(plyr, &lines[i]);
			P_UseSpecialLine(plyr, &lines[i], 0);

			++speciallines;
		}
	}

	for (size_t i{0}; i < ::std::size_t(CardType::NUMCARDS); ++i)
	{
		plyr->cards[i] = origcards[i];
	}

	// trigger tag 666/667 events
	dummy.tag = 666;
	if (gamemode == GameMode::commercial)
	{
		// Master Levels in PC slot 7
		if (gamemap == 7 || (gamemission == GameMission::pack_master && (gamemap == 14 || gamemap == 15 || gamemap == 16)))
		{
			// Mancubi
			speciallines += EV_DoFloor(&dummy, floor_e::lowerFloorToLowest);

			// Arachnotrons
			dummy.tag = 667;
			speciallines += EV_DoFloor(&dummy, floor_e::raiseToTexture);
			dummy.tag = 666;
		}
	}
	else
	{
		if (gameepisode == 1)
		{
			// Barons of Hell
			speciallines += EV_DoFloor(&dummy, floor_e::lowerFloorToLowest);
		}
		else if (gameepisode == 4)
		{
			if (gamemap == 6)
			{
				// Cyberdemons
				speciallines += EV_DoDoor(&dummy, vldoor_e::vld_blazeOpen);
			}
			else if (gamemap == 8)
			{
				// Spider Masterminds
				speciallines += EV_DoFloor(&dummy, floor_e::lowerFloorToLowest);
			}
		}
	}
	// Keens (no matter which level they are on)
	// this call will be ignored if the tagged sector is already moving
	// so actions triggered in the condition above will have precedence
	speciallines += EV_DoDoor(&dummy, vldoor_e::vld_open);

	return (speciallines);
}

// only give available weapons
static bool WeaponAvailable(int w)
{
	if (w < 0 || w >= (int)WeaponType::NUMWEAPONS)
	{
		return false;
	}

	if (w == (int)WeaponType::wp_supershotgun && !crispy->havessg)
	{
		return false;
	}

	if ((w == (int)WeaponType::wp_bfg || w == (int)WeaponType::wp_plasma) && gamemode == GameMode::shareware)
	{
		return false;
	}

	return true;
}

// give or take backpack
static void GiveBackpack(bool give)
{
	if (give && !plyr->backpack)
	{
		for (size_t i{0}; i < ::std::size_t(AmmoType::NUMAMMO); ++i)
		{
			plyr->maxammo[i] *= 2;
		}
		plyr->backpack = true;
	}
	else if (!give && plyr->backpack)
	{
		for (size_t i{0}; i < ::std::size_t(AmmoType::NUMAMMO); ++i)
		{
			plyr->maxammo[i] /= 2;
		}
		plyr->backpack = false;
	}
}

// FIXME - this is the function they all warned you about, the hideous DO EVERYTHING HERE function
// Respond to keyboard input events, intercept cheats.
bool ST_Responder(EventType* ev)
{
	// Filter automap on/off.
	if (ev->type == evtype_t::ev_keyup && ((ev->data1 & 0xffff0000) == AM_MSGHEADER))
	{
		switch (ev->data1)
		{
		case AM_MSGENTERED:
			st_gamestate = st_stateenum_t::AutomapState;
			st_firsttime = true;
			break;

		case AM_MSGEXITED:
			//fprintf(stderr, "AM exited\n");
			st_gamestate = st_stateenum_t::FirstPersonState;
			break;
		}
	}
	// if a user keypress...
	else if (ev->type == evtype_t::ev_keydown)
	{
		if (!netgame && gameskill != SkillType::sk_nightmare)
		{
			// 'dqd' cheat for toggleable god mode
			if (cht_CheckCheatSP(&cheat_god, ev->data2))
			{
				// dead players are first respawned at the current position
				mapthing_t mt{0};
				if (plyr->playerstate == PlayerState::dead)
				{
					int an;
					extern void P_SpawnPlayer(mapthing_t * mthing);

					mt.x = plyr->x >> FRACBITS;
					mt.y = plyr->y >> FRACBITS;
					mt.angle = (plyr->angle + ANG45 / 2) * (uint64_t)45 / ANG45;
					mt.type = consoleplayer + 1;
					P_SpawnPlayer(&mt);

					// spawn a teleport fog
					an = plyr->angle >> ANGLETOFINESHIFT;
					P_SpawnMobj(plyr->x + 20 * finecosine[an], plyr->y + 20 * finesine[an], plyr->z, mobjtype_t::MT_TFOG);
					S_StartSound(plyr, (int)sfxenum_t::sfx_slop);
				}

				plyr->cheats ^= (int)CheatType::CF_GODMODE;
				if (plyr->cheats & (int)CheatType::CF_GODMODE)
				{
					if (plyr)
					{
						plyr->health = 100;
					}

					plyr->health = deh_god_mode_health;
					plyr->message = DEH_String(STSTR_DQDON);
				}
				else
				{
					plyr->message = DEH_String(STSTR_DQDOFF);
				}

				// eat key press when respawning
				if (mt.type)
				{
					return true;
				}
			}
			// 'fa' cheat for killer fucking arsenal
			else if (cht_CheckCheatSP(&cheat_ammonokey, ev->data2))
			{
				plyr->armorpoints = deh_idfa_armor;
				plyr->armortype = deh_idfa_armor_class;

				// give backpack
				GiveBackpack(true);

				for (size_t i{0}; i < (int)WeaponType::NUMWEAPONS; ++i)
				{
					if (WeaponAvailable(i)) // only give available weapons
					{
						plyr->weaponowned[i] = true;
					}
				}

				for (size_t i{0}; i < (int)AmmoType::NUMAMMO; ++i)
				{
					plyr->ammo[i] = plyr->maxammo[i];
				}

				// trigger evil grin now
				plyr->bonuscount += 2;

				plyr->message = DEH_String(STSTR_FAADDED);
			}
			// 'kfa' cheat for key full ammo
			else if (cht_CheckCheatSP(&cheat_ammo, ev->data2))
			{
				plyr->armorpoints = deh_idkfa_armor;
				plyr->armortype = deh_idkfa_armor_class;

				// give backpack
				GiveBackpack(true);

				for (size_t i{0}; i < (int)WeaponType::NUMWEAPONS; ++i)
				{
					if (WeaponAvailable(i)) // only give available weapons
					{
						plyr->weaponowned[i] = true;
					}
				}

				for (size_t i{0}; i < (int)AmmoType::NUMAMMO; ++i)
				{
					plyr->ammo[i] = plyr->maxammo[i];
				}

				for (size_t i{0}; i < (int)CardType::NUMCARDS; ++i)
				{
					plyr->cards[i] = true;
				}

				// trigger evil grin now
				plyr->bonuscount += 2;

				plyr->message = DEH_String(STSTR_KFAADDED);
			}
			// 'mus' cheat for changing music
			else if (cht_CheckCheat(&cheat_mus, ev->data2))
			{
				char buf[3];
				int musnum;

				plyr->message = DEH_String(STSTR_MUS);
				cht_GetParam(&cheat_mus, buf);

				// Note: The original v1.9 had a bug that tried to play back
				// the Doom II music regardless of gamemode. This was fixed
				// in the Ultimate Doom executable so that it would work for
				// the Doom 1 music as well.

				// restart current music if IDMUS00 is entered
				if (buf[0] == '0' && buf[1] == '0')
				{
					S_ChangeMusic(0, 2);
					// eat key press, i.e. don't change weapon upon music change
					return true;
				}
				else
					// [JN] Fixed: using a proper IDMUS selection for shareware
					// and registered game versions.
					if (gamemode == GameMode::commercial /* || gameversion < exe_ultimate */)
					{
						musnum = (int)musicenum_t::mus_runnin + (buf[0] - '0') * 10 + buf[1] - '0' - 1;

						/*
						if (((buf[0]-'0')*10 + buf[1]-'0') > 35
						&& gameversion >= GameVersion::exe_doom_1_8)
						*/
						// prevent crash with IDMUS00
						if (musnum < (int)musicenum_t::mus_runnin || musnum >= (int)musicenum_t::NUMMUSIC)
						{
							plyr->message = DEH_String(STSTR_NOMUS);
						}
						else
						{
							S_ChangeMusic(musnum, 1);
							// eat key press, i.e. don't change weapon upon music change
							return true;
						}
					}
					else
					{
						musnum = (int)musicenum_t::mus_e1m1 + (buf[0] - '1') * 9 + (buf[1] - '1');

						/*
						if (((buf[0]-'1')*9 + buf[1]-'1') > 31)
						*/
						// prevent crash with IDMUS0x or IDMUSx0
						// support dedicated music tracks for the 4th episode
						if (musnum < (int)musicenum_t::mus_e1m1 || musnum >= (int)musicenum_t::mus_runnin || S_music[musnum].lumpnum == -1)
						{
							plyr->message = DEH_String(STSTR_NOMUS);
						}
						else
						{
							S_ChangeMusic(musnum, 1);
							// eat key press, i.e. don't change weapon upon music change
							return true;
						}
					}
			}
			// eat up the first digit typed after a cheat expecting two parameters
			else if (cht_CheckCheat(&cheat_mus1, ev->data2))
			{
				char buf[2];

				cht_GetParam(&cheat_mus1, buf);

				return isdigit(buf[0]);
			}
			// allow both idspispopd and idclip cheats in all gamemissions
			else if (cht_CheckCheatSP(&cheat_noclip, ev->data2) || cht_CheckCheatSP(&cheat_commercial_noclip, ev->data2))
			{
				// Noclip cheat.
				// For Doom 1, use the idspipsopd cheat; for all others, use
				// idclip
				plyr->cheats ^= (int)CheatType::CF_NOCLIP;

				if (plyr->cheats & (int)CheatType::CF_NOCLIP)
				{
					plyr->message = DEH_String(STSTR_NCON);
				}
				else
				{
					plyr->message = DEH_String(STSTR_NCOFF);
				}
			}

			// 'behold?' power-up cheats
			for (size_t i{0}; i < 6; ++i)
			{
				if (i < 4 ? cht_CheckCheatSP(&cheat_powerup[i], ev->data2) : cht_CheckCheat(&cheat_powerup[i], ev->data2))
				{
					if (!plyr->powers[i])
					{
						P_GivePower(plyr, i);
					}
					else if (i != (int)PowerType_t::pw_strength && i != (int)PowerType_t::pw_allmap) // disable full Automap
					{
						plyr->powers[i] = 1;
					}
					else
					{
						plyr->powers[i] = 0;
					}

					plyr->message = DEH_String(STSTR_BEHOLDX);
				}
			}

			// idbehold0
			if (cht_CheckCheatSP(&cheat_powerup[7], ev->data2))
			{
				// FIXME
				//memset(plyr->powers., 0, sizeof(plyr->powers));
				plyr->flags = (mobjflag_e)((unsigned)plyr->flags & ~(unsigned)mobjflag_e::MF_SHADOW); // cancel invisibility
				plyr->message = DEH_String(STSTR_BEHOLDX);
			}

			// 'behold' power-up menu
			if (cht_CheckCheat(&cheat_powerup[6], ev->data2))
			{
				plyr->message = DEH_String(STSTR_BEHOLD);
			}
			// 'choppers' invulnerability & chainsaw
			else if (cht_CheckCheatSP(&cheat_choppers, ev->data2))
			{
				plyr->weaponowned[::std::size_t(WeaponType::wp_chainsaw)] = true;
				plyr->powers[::std::size_t(PowerType_t::pw_invulnerability)] = true;
				plyr->message = DEH_String(STSTR_CHOPPERS);
			}
			// 'mypos' for player position
			else if (cht_CheckCheat(&cheat_mypos, ev->data2))
			{
				/*
				static char buf[ST_MSGWIDTH];
				M_snprintf(buf, sizeof(buf), "ang=0x%x;x,y=(0x%x,0x%x)",
							players[consoleplayer].mo->angle,
							players[consoleplayer].mo->x,
							players[consoleplayer].mo->y);
				plyr->message = buf;
				*/
				// extra high precision IDMYPOS variant, updates for 10 seconds
				plyr->powers[::std::size_t(PowerType_t::pw_mapcoords)] = 10 * TICRATE;
			}

			// now follow "critical" Crispy Doom specific cheats
			// implement Boom's "tntem" cheat
			else if (cht_CheckCheatSP(&cheat_massacre, ev->data2)
				|| cht_CheckCheatSP(&cheat_massacre2, ev->data2)
				|| cht_CheckCheatSP(&cheat_massacre3, ev->data2))
			{
				int killcount = ST_cheat_massacre();
				::std::string const monster = (gameversion == GameVersion::exe_chex) ? "Flemoid" : "Monster";
				::std::string const killed = (gameversion == GameVersion::exe_chex) ? "returned" : "killed";

				//M_snprintf(msg, sizeof(msg), "%s%d %s%s%s %s", crstr[CR_GOLD],
				//			killcount, crstr[CR_NONE], monster, (killcount == 1) ? "" : "s", killed);
				//plyr->message = msg;
			}
			// implement Crispy Doom's "spechits" cheat
			else if (cht_CheckCheatSP(&cheat_spechits, ev->data2))
			{
				int triggeredlines = ST_cheat_spechits();

				//M_snprintf(msg, sizeof(msg), "%s%d %sSpecial Line%s Triggered", crstr[CR_GOLD],
				//		triggeredlines, crstr[CR_NONE], (triggeredlines == 1) ? "" : "s");
				//plyr->message = msg;
			}
			// implement PrBoom+'s "notarget" cheat
			else if (cht_CheckCheatSP(&cheat_notarget, ev->data2) || cht_CheckCheatSP(&cheat_notarget2, ev->data2))
			{
				plyr->cheats ^= (int)CheatType::CF_NOTARGET;

				if (plyr->cheats & (int)CheatType::CF_NOTARGET)
				{
					thinker_t* th;

					// let mobjs forget their target and tracer
					//for (th = thinkercap.next; th != &thinkercap; th = th->next)
					//{
						//if (th->function.acp1 == (actionf_p1)P_MobjThinker)
						//{
							//MapObject *const mo = (MapObject*)th;

							// FIXME
							/* if (mo->target && mo->target->player)
							{
								mo->target = nullptr;
							}

							if (mo->tracer && mo->tracer->player)
							{
								mo->tracer = nullptr;
							} */
							//}
						//}
						// let sectors forget their soundtarget
					for (size_t i{0}; i < numsectors; ++i)
					{
						sector_t* const sector = &sectors[i];

						sector->soundtarget = nullptr;
					}
				}

				/* M_snprintf(msg, sizeof(msg), "Notarget Mode %s%s", crstr[CR_GREEN],
							(plyr->cheats & (int)CheatType::CF_NOTARGET) ? "ON" : "OFF"); */
				plyr->message = msg;
			}
			// implement "nomomentum" cheat, ne debug aid -- pretty useless, though
			else if (cht_CheckCheatSP(&cheat_nomomentum, ev->data2))
			{
				plyr->cheats ^= (int)CheatType::CF_NOMOMENTUM;

				/* M_snprintf(msg, sizeof(msg), "Nomomentum Mode %s%s", crstr[CR_GREEN],
						(plyr->cheats & (int)CheatType::CF_NOMOMENTUM) ? "ON" : "OFF"); */
				plyr->message = msg;
			}
			// implement Crispy Doom's "goobers" cheat, ne easter egg
			else if (cht_CheckCheatSP(&cheat_goobers, ev->data2))
			{
				extern void EV_DoGoobers();

				EV_DoGoobers();

				/* M_snprintf(msg, sizeof(msg), "Get Psyched!"); */
				plyr->message = msg;
			}
			// implement Boom's "tntweap?" weapon cheats
			else if (cht_CheckCheatSP(&cheat_weapon, ev->data2))
			{
				char buf[2];
				int w;

				cht_GetParam(&cheat_weapon, buf);
				w = *buf - '1';

				// TNTWEAP0 takes away all weapons and ammo except for the pistol and 50 bullets
				if (w == -1)
				{
					GiveBackpack(false);
					plyr->powers[::std::size_t(PowerType_t::pw_strength)] = 0;

					for (size_t i{0}; i < ::std::size_t(WeaponType::NUMWEAPONS); ++i)
					{
						oldweaponsowned[i] = plyr->weaponowned[i] = false;
					}
					oldweaponsowned[::std::size_t(WeaponType::wp_fist)] = plyr->weaponowned[::std::size_t(WeaponType::wp_fist)] = true;
					oldweaponsowned[::std::size_t(WeaponType::wp_pistol)] = plyr->weaponowned[::std::size_t(WeaponType::wp_pistol)] = true;

					for (size_t i{0}; i < ::std::size_t(AmmoType::NUMAMMO); ++i)
					{
						plyr->ammo[i] = 0;
					}
					plyr->ammo[::std::size_t(AmmoType::am_clip)] = deh_initial_bullets;

					if (plyr->readyweapon > WeaponType::wp_pistol)
					{
						plyr->pendingweapon = WeaponType::wp_pistol;
					}

					plyr->message = "All weapons removed!";

					return true;
				}

				// only give available weapons
				if (!WeaponAvailable(w))
				{
					return false;
				}

				// make '1' apply beserker strength toggle
				if (w == (int)WeaponType::wp_fist)
				{
					if (!plyr->powers[::std::size_t(PowerType_t::pw_strength)])
					{
						P_GivePower(plyr, (int)PowerType_t::pw_strength);
						S_StartSound(NULL, sfxenum_t::sfx_getpow);
						plyr->message = DEH_String(GOTBERSERK);
					}
					else
					{
						plyr->powers[::std::size_t(PowerType_t::pw_strength)] = 0;
						plyr->message = DEH_String(STSTR_BEHOLDX);
					}
				}
				else
				{
					if (!plyr->weaponowned[w])
					{
						extern bool P_GiveWeapon(Player * player, WeaponType weapon, bool dropped);
						extern ::std::string const WeaponPickupMessages[::std::size_t(WeaponType::NUMWEAPONS)];

						P_GiveWeapon(plyr, (WeaponType)w, false);
						S_StartSound(NULL, sfxenum_t::sfx_wpnup);

						if (w > 1)
						{
							plyr->message = DEH_String(WeaponPickupMessages[w]);
						}

						// trigger evil grin now
						plyr->bonuscount += 2;
					}
					else
					{
						// no reason for evil grin
						oldweaponsowned[w] = plyr->weaponowned[w] = false;

						// removed current weapon, select another one
						if (w == (int)plyr->readyweapon)
						{
							extern bool P_CheckAmmo(Player * player);

							P_CheckAmmo(plyr);
						}
					}
				}

				/*  if (!plyr->message)
					{
						M_snprintf(msg, sizeof(msg), "Weapon %s%d%s %s", crstr[CR_GOLD], w + 1, crstr[CR_NONE],
									plyr->weaponowned[w] ? "added" : "removed");
						plyr->message = msg;
					} */
			}
		}

		// now follow "harmless" Crispy Doom specific cheats
		// implement Crispy Doom's "showfps" cheat, ne debug aid
		if (cht_CheckCheat(&cheat_showfps, ev->data2) || cht_CheckCheat(&cheat_showfps2, ev->data2))
		{
			plyr->powers[::std::size_t(PowerType_t::pw_showfps)] ^= 1;
		}
		// implement Boom's "tnthom" cheat
		else if (cht_CheckCheat(&cheat_hom, ev->data2))
		{
			crispy->flashinghom = !crispy->flashinghom;

			/* M_snprintf(msg, sizeof(msg), "HOM Detection %s%s", crstr[CR_GREEN], (crispy->flashinghom) ? "ON" : "OFF"); */
			plyr->message = msg;
		}
		// Show engine version, build date and SDL version
		else if (cht_CheckCheat(&cheat_version, ev->data2))
		{
#ifndef BUILD_DATE
#define BUILD_DATE __DATE__
#endif
			/* M_snprintf(msg, sizeof(msg), "%s (%s) x%ld SDL%s",
						PACKAGE_STRING,
						BUILD_DATE,
						(long) sizeof(void*) * CHAR_BIT,
						crispy->sdlversion); */
#undef BUILD_DATE
			plyr->message = msg;
			fprintf(stderr, "%s\n", msg);
		}
		// Show skill level
		else if (cht_CheckCheat(&cheat_skill, ev->data2))
		{
			extern ::std::string skilltable[];

			/* M_snprintf(msg, sizeof(msg), "Skill: %s", skilltable[BETWEEN(0,5,(int) gameskill+1)]); */
			plyr->message = msg;
		}

		// 'clev' change-level cheat
		if (!netgame && cht_CheckCheat(&cheat_clev, ev->data2) && !menuactive) // prevent only half the screen being updated
		{
			char buf[3];
			int epsd;
			int map;

			cht_GetParam(&cheat_clev, buf);

			if (gamemode == GameMode::commercial)
			{
				if (gamemission == GameMission::pack_master)
				{
					epsd = 3;
				}
				else if (gamemission == GameMission::pack_nerve)
				{
					epsd = 2;
				}
				else
				{
					epsd = 0;
				}
				map = (buf[0] - '0') * 10 + buf[1] - '0';
			}
			else
			{
				epsd = buf[0] - '0';
				map = buf[1] - '0';

				// Chex.exe always warps to episode 1.
				if (gameversion == GameVersion::exe_chex)
				{
					if (epsd > 1)
					{
						epsd = 1;
					}
					if (map > 5)
					{
						map = 5;
					}
				}
			}
			// only fix episode/map if it doesn't exist
			if (P_GetNumForMap(epsd, map, false) < 0)
			{
				// Catch invalid maps.
				if (gamemode != GameMode::commercial)
				{
					// allow IDCLEV0x to work in Doom 1
					if (epsd == 0)
					{
						epsd = gameepisode;
					}
					if (epsd < 1)
					{
						return false;
					}
					if (epsd > 4)
					{
						// Sigil
						if (!(crispy->haved1e5 && epsd == 5))
						{
							return false;
						}
					}
					if (epsd == 4 && gameversion < GameVersion::exe_ultimate)
					{
						return false;
					}
					// IDCLEV00 restarts current map
					if ((map == 0) && (buf[0] - '0' == 0))
					{
						map = gamemap;
					}
					// support E1M10 "Sewers"
					if ((map == 0 || map > 9) && crispy->havee1m10 && epsd == 1)
					{
						map = 10;
					}
					if (map < 1)
					{
						return false;
					}
					if (map > 9)
					{
						// support E1M10 "Sewers"
						if (!(crispy->havee1m10 && epsd == 1 && map == 10))
							return false;
					}
				}
				else
				{
					// IDCLEV00 restarts current map
					if ((map == 0) && (buf[0] - '0' == 0))
					{
						map = gamemap;
					}
					if (map < 1)
					{
						return false;
					}
					if (map > 40)
					{
						return false;
					}
					if (map > 9 && gamemission == GameMission::pack_nerve)
					{
						return false;
					}
					if (map > 21 && gamemission == GameMission::pack_master)
					{
						return false;
					}
				}
			}
			// prevent idclev to nonexistent levels exiting the game
			if (P_GetNumForMap(epsd, map, false) >= 0)
			{
				// So be it.
				plyr->message = DEH_String(STSTR_CLEV);
				// allow IDCLEV during demo playback and warp to the requested map
				if (demoplayback)
				{
					if (map > gamemap)
					{
						crispy->demowarp = map;
						nodrawers = true;
						singletics = true;
						return true;
					}
					else
					{
						return false;
					}
				}
				else
				{
					G_DeferedInitNew(gameskill, epsd, map);
				}
				// eat key press, i.e. don't change weapon upon level change
				return true;
			}
		}
		// eat up the first digit typed after a cheat expecting two parameters
		else if (!netgame && cht_CheckCheat(&cheat_clev1, ev->data2) && !menuactive)
		{
			char buf[2];

			cht_GetParam(&cheat_clev1, buf);

			return isdigit(buf[0]);
		}
	}
	return false;
}

int ST_calcPainOffset()
{
	int health;
	static int lastcalc;
	static int oldhealth = -1;

	health = plyr->health > 100 ? 100 : plyr->health;

	if (health != oldhealth)
	{
		lastcalc = ST_FACESTRIDE * (((100 - health) * ST_NUMPAINFACES) / 101);
		oldhealth = health;
	}

	return lastcalc;
}

// This is a not-very-pretty routine which handles
// the face states and their timing.
// the precedence of expressions is:
// dead > evil grin > turned head > straight ahead
//
// fix status bar face hysteresis
static int faceindex;
void ST_updateFaceWidget()
{
	angle_t badguyangle;
	angle_t diffang;
	static int lastattackdown = -1;
	static int priority = 0;
	bool doevilgrin;

	// fix status bar face hysteresis
	int painoffset;
	// no evil grin or rampage face in god mode
	const bool invul = (plyr->cheats & (int)CheatType::CF_GODMODE) || plyr->powers[::std::size_t(PowerType_t::pw_invulnerability)];

	painoffset = ST_calcPainOffset();

	if (priority < 10)
	{
		// dead
		if (!plyr->health)
		{
			priority = 9;
			painoffset = 0;
			faceindex = ST_DEADFACE;
			st_facecount = 1;
		}
	}

	if (priority < 9)
	{
		if (plyr->bonuscount)
		{
			// picking up bonus
			doevilgrin = false;

			for (size_t i{0}; i < ::std::size_t(WeaponType::NUMWEAPONS); ++i)
			{
				if (oldweaponsowned[i] != plyr->weaponowned[i])
				{
					doevilgrin = true;
					oldweaponsowned[i] = plyr->weaponowned[i];
				}
			}
			// no evil grin in god mode
			if (doevilgrin && !invul)
			{
				// evil grin if just picked up weapon
				priority = 8;
				st_facecount = ST_EVILGRINCOUNT;
				faceindex = ST_EVILGRINOFFSET;
			}
		}
	}

	if (priority < 8)
	{
		if (plyr->damagecount && plyr->attacker && plyr->attacker)
		{
			// being attacked
			priority = 7;

			// show "Ouch Face" as intended
			if (st_oldhealth - plyr->health > ST_MUCHPAIN)
			{
				// raise "Ouch Face" priority
				priority = 8;
				st_facecount = ST_TURNCOUNT;
				faceindex = ST_OUCHOFFSET;
			}
			else
			{
				badguyangle = R_PointToAngle2(plyr->x, plyr->y, plyr->attacker->x, plyr->attacker->y);
				bool i{false};

				if (badguyangle > plyr->angle)
				{
					// whether right or left
					diffang = badguyangle - plyr->angle;
					i = diffang > ANG180;
				}
				else
				{
					// whether left or right
					diffang = plyr->angle - badguyangle;
					i = diffang <= ANG180;
				} // confusing, aint it?

				st_facecount = ST_TURNCOUNT;

				if (diffang < ANG45)
				{
					// head-on
					faceindex = ST_RAMPAGEOFFSET;
				}
				else if (i)
				{
					// turn face right
					faceindex = ST_TURNOFFSET;
				}
				else
				{
					// turn face left
					faceindex = ST_TURNOFFSET + 1;
				}
			}
		}
	}

	if (priority < 7)
	{
		// getting hurt because of your own damn stupidity
		if (plyr->damagecount)
		{
			// show "Ouch Face" as intended
			if (st_oldhealth - plyr->health > ST_MUCHPAIN)
			{
				priority = 7;
				st_facecount = ST_TURNCOUNT;
				faceindex = ST_OUCHOFFSET;
			}
			else
			{
				priority = 6;
				st_facecount = ST_TURNCOUNT;
				faceindex = ST_RAMPAGEOFFSET;
			}
		}
	}

	if (priority < 6)
	{
		// rapid firing
		if (plyr->attackdown)
		{
			if (lastattackdown == -1)
			{
				lastattackdown = ST_RAMPAGEDELAY;
			}
			// no rampage face in god mode
			else if (!--lastattackdown && !invul)
			{
				priority = 5;
				faceindex = ST_RAMPAGEOFFSET;
				st_facecount = 1;
				lastattackdown = 1;
			}
		}
		else
		{
			lastattackdown = -1;
		}
	}

	if (priority < 5)
	{
		// invulnerability
		if (invul)
		{
			priority = 4;

			painoffset = 0;
			faceindex = ST_GODFACE;
			st_facecount = 1;
		}
	}

	// look left or look right if the facecount has timed out
	if (!st_facecount)
	{
		faceindex = st_randomnumber % 3;
		st_facecount = ST_STRAIGHTFACECOUNT;
		priority = 0;
	}

	--st_facecount;

	// fix status bar face hysteresis
	st_faceindex = painoffset + faceindex;
}

void ST_updateWidgets()
{
	static int largeammo = 1994; // means "n/a"

	// must redirect the pointer if the ready weapon has changed.
	// if (w_ready.data != plyr->readyweapon)
	// {
	if (weaponinfo[::std::size_t(plyr->readyweapon)].ammo == AmmoType::am_noammo)
	{
		w_ready.num = &largeammo;
	}
	else
	{
		w_ready.num = &plyr->ammo[::std::size_t(weaponinfo[::std::size_t(plyr->readyweapon)].ammo)];
	}
	//{
	// static TimeType tic=0;
	// static int dir=-1;
	// if (!(tic&15))
	//	plyr->ammo[weaponinfo[plyr->readyweapon].ammo]+=dir;
	// if (plyr->ammo[weaponinfo[plyr->readyweapon].ammo] == -100)
	//	dir = 1;
	// ++tic;
	//}
	w_ready.data = (int)plyr->readyweapon;

	// if (*w_ready.on)
	// STlib_updateNum(&w_ready, true);
	// refresh weapon change
	//}

	// update keycard multiple widgets
	for (size_t i{0}; i < 3; ++i)
	{
		keyboxes[i] = plyr->cards[i] ? i : -1;

		if (plyr->cards[i + 3])
		{
			keyboxes[i] = (keyboxes[i] == -1) ? i + 3 : i + 6; // support combined card and skull keys
		}

		// blinking key or skull in the status bar
		if (plyr->tryopen[i])
		{
#if defined(CRISPY_KEYBLINK_WITH_SOUND)
			if (!(plyr->tryopen[i] & (2 * KEYBLINKMASK - 1)))
			{
				S_StartSound(NULL, sfxenum_t::sfx_itemup);
			}
#endif
#if defined(CRISPY_KEYBLINK_IN_CLASSIC_HUD)
			if (st_classicstatusbar && !(plyr->tryopen[i] & (KEYBLINKMASK - 1)))
			{
				st_firsttime = true;
			}
#endif
			// FIXME
			//plyr->tryopen[i]--;
#if !defined(CRISPY_KEYBLINK_IN_CLASSIC_HUD)
			if (st_crispyhud)
#endif
			{
				keyboxes[i] = (plyr->tryopen[i] & KEYBLINKMASK) ? i + st_keyorskull[i] : -1;
			}

			if (!plyr->tryopen[i])
			{
				w_keyboxes[i].oldinum = -1;
			}
		}
	}

	// refresh everything if this is him coming back to life
	ST_updateFaceWidget();

	// used by the w_armsbg widget
	st_notdeathmatch = !deathmatch;

	// used by w_arms[] widgets
	st_armson = st_statusbaron && !deathmatch;

	// used by w_frags widget
	st_fragson = deathmatch && st_statusbaron;
	st_fragscount = 0;

	for (size_t i{0}; i < MAX_PLAYERS; ++i)
	{
		if (i != consoleplayer)
		{
			st_fragscount += plyr->frags[i];
		}
		else
		{
			st_fragscount -= plyr->frags[i];
		}
	}

	// get rid of chat window if up because of message
	if (!--st_msgcounter)
	{
		st_chat = st_oldchat;
	}
}

static int st_widescreendelta;

void ST_Ticker()
{
	++st_clock;
	st_randomnumber = M_Random();
	ST_updateWidgets();
	st_oldhealth = plyr->health;
}

static int st_palette = 0;

void ST_doPaletteStuff()
{
	int palette;
#ifndef CRISPY_TRUECOLOR
	byte* pal;
#endif
	int bzc;

	int cnt = plyr->damagecount;

	if (plyr->powers[::std::size_t(PowerType_t::pw_strength)])
	{
		// slowly fade the berzerk out
		bzc = 12 - (plyr->powers[::std::size_t(PowerType_t::pw_strength)] >> 6);

		if (bzc > cnt)
		{
			cnt = bzc;
		}
	}

	if (cnt)
	{
		palette = (cnt + 7) >> 3;

		if (palette >= NUMREDPALS)
		{
			palette = NUMREDPALS - 1;
		}

		// tune down a bit so the menu remains legible
		if (menuactive || paused)
		{
			palette >>= 1;
		}

		palette += STARTREDPALS;
	}
	else if (plyr->bonuscount && plyr->health > 0) // never show the yellow bonus palette for a dead player
	{
		palette = (plyr->bonuscount + 7) >> 3;

		if (palette >= NUMBONUSPALS)
		{
			palette = NUMBONUSPALS - 1;
		}

		palette += STARTBONUSPALS;
	}
	else if (plyr->powers[::std::size_t(PowerType_t::pw_ironfeet)] > 4 * 32 || plyr->powers[::std::size_t(PowerType_t::pw_ironfeet)] & 8)
	{
		palette = RADIATIONPAL;
	}
	else
	{
		palette = 0;
	}

	// In Chex Quest, the player never sees red. Instead, the
	// radiation suit palette is used to tint the screen green,
	// as though the player is being covered in goo by an
	// attacking flemoid.
	if (gameversion == GameVersion::exe_chex && palette >= STARTREDPALS && palette < STARTREDPALS + NUMREDPALS)
	{
		palette = RADIATIONPAL;
	}

	// prevent palette changes when in help screen or Crispness menu
	if (inhelpscreens)
	{
		palette = 0;
	}

	if (palette != st_palette)
	{
		st_palette = palette;
#ifndef CRISPY_TRUECOLOR
		pal = W_CacheLumpNum<byte>(lu_palette, pu_tags_t::PU_CACHE) + palette * 768;
		I_SetPalette(pal);
#else
		I_SetPalette(palette);
#endif
	}
}

// return ammo/health/armor widget color
static byte* ST_WidgetColor(int i)
{
	if (!(crispy->coloredhud & COLOREDHUD_BAR))
	{
		return nullptr;
	}

	switch ((hudcolor_t)i)
	{
	case hudcolor_t::hudcolor_ammo:
	{
		if (weaponinfo[::std::size_t(plyr->readyweapon)].ammo == AmmoType::am_noammo)
		{
			return nullptr;
		}
		else
		{
			int ammo = plyr->ammo[::std::size_t(weaponinfo[::std::size_t(plyr->readyweapon)].ammo)];
			int fullammo = maxammo[::std::size_t(weaponinfo[::std::size_t(plyr->readyweapon)].ammo)];

			if (ammo < fullammo / 4)
			{
				return cr[CR_RED];
			}
			else if (ammo < fullammo / 2)
			{
				return cr[CR_GOLD];
			}
			else if (ammo <= fullammo)
			{
				return cr[CR_GREEN];
			}
			else
			{
				return cr[CR_BLUE];
			}
		}
		break;
	}
	case hudcolor_t::hudcolor_health:
	{
		int health = plyr->health;

		// Invulnerability powerup and God Mode cheat turn Health values gray
		if (plyr->cheats & (int)CheatType::CF_GODMODE || plyr->powers[::std::size_t(PowerType_t::pw_invulnerability)])
		{
			return cr[CR_GRAY];
		}
		else if (health < 25)
		{
			return cr[CR_RED];
		}
		else if (health < 50)
		{
			return cr[CR_GOLD];
		}
		else if (health <= 100)
		{
			return cr[CR_GREEN];
		}
		else
		{
			return cr[CR_BLUE];
		}

		break;
	}
	case hudcolor_t::hudcolor_frags:
	{
		int frags = st_fragscount;

		if (frags < 0)
		{
			return cr[CR_RED];
		}
		else if (frags == 0)
		{
			return cr[CR_GOLD];
		}
		else
		{
			return cr[CR_GREEN];
		}

		break;
	}
	case hudcolor_t::hudcolor_armor:
	{
		// Invulnerability powerup and God Mode cheat turn Armor values gray
		if (plyr->cheats & (int)CheatType::CF_GODMODE || plyr->powers[::std::size_t(PowerType_t::pw_invulnerability)])
		{
			return cr[CR_GRAY];
		}
		// color by armor type
		else if (plyr->armortype >= 2)
		{
			return cr[CR_BLUE];
		}
		else if (plyr->armortype == 1)
		{
			return cr[CR_GREEN];
		}
		else if (plyr->armortype == 0)
		{
			return cr[CR_RED];
		}
		/*
					// alternatively, color by armor points
					int armor = plyr->armorpoints;

					if (armor < 25)
					{
						return cr[CR_RED];
					}
					else if (armor < 50)
					{
						return cr[CR_GOLD];
					}
					else if (armor <= 100)
					{
						return cr[CR_GREEN];
					}
					else
					{
						return cr[CR_BLUE];
					}
		*/
		break;
	}
	}

	return nullptr;
}

// draw the gibbed death state frames in the Health widget in sync with the actual player sprite
static inline void ST_DrawGibbedPlayerSprites()
{
	state_t const* state = plyr->state;
	spritedef_t* sprdef;
	spriteframe_t* sprframe;
	patch_t* patch;

	sprdef = &sprites[::std::size_t(state->sprite)];

	// the TNT1 sprite is not supposed to be rendered anyway
	if (!sprdef->numframes && plyr->sprite == spritenum_t::SPR_TNT1)
	{
		return;
	}

	sprframe = &sprdef->spriteframes[state->frame & FF_FRAMEMASK];
	patch = W_CacheLumpNum<patch_t>(sprframe->lump[0] + firstspritelump, pu_tags_t::PU_CACHE);

	if ((int)plyr->flags & (int)mobjflag_e::MF_TRANSLATION)
	{
		dp_translation = translationtables - 256 + (((int)plyr->flags & (int)mobjflag_e::MF_TRANSLATION) >> ((int)mobjflag_e::MF_TRANSSHIFT - 8));
	}

	V_DrawPatch(ST_HEALTHX - 17, 186, patch);
	dp_translation = NULL;
}

void ST_drawWidgets(bool refresh)
{
	int i;
	bool gibbed = false;

	// used by w_arms[] widgets
	st_armson = st_statusbaron && !deathmatch;

	// used by w_frags widget
	st_fragson = deathmatch && st_statusbaron;

	dp_translation = ST_WidgetColor((int)hudcolor_t::hudcolor_ammo);
	STlib_updateNum(&w_ready, refresh);
	dp_translation = NULL;

	// draw "special widgets" in the Crispy HUD
	if (st_crispyhud)
	{
		// draw berserk pack instead of no ammo if appropriate
		if (plyr->readyweapon == WeaponType::wp_fist && plyr->powers[::std::size_t(PowerType_t::pw_strength)])
		{
			static int lump = -1;
			patch_t* patch;

			if (lump == -1)
			{
				lump = W_CheckNumForName(DEH_String("PSTRA0"));

				if (lump == -1)
				{
					lump = W_CheckNumForName(DEH_String("MEDIA0"));
				}
			}

			patch = W_CacheLumpNum<patch_t>(lump, pu_tags_t::PU_CACHE);

			// (23,179) is the center of the Ammo widget
			V_DrawPatch(ST_AMMOX - 21 - SHORT(patch->width) / 2 + SHORT(patch->leftoffset),
				179 - SHORT(patch->height) / 2 + SHORT(patch->topoffset), patch);

		}

		// draw the gibbed death state frames in the Health widget in sync with the actual player sprite
		if (plyr->health <= 0 && plyr->state - states >= (int)mobjinfo[::std::size_t(plyr->type)].xdeathstate)
		{
			ST_DrawGibbedPlayerSprites();
			gibbed = true;
		}
	}

	for (i = 0;i < 4; ++i)
	{
		STlib_updateNum(&w_ammo[i], refresh);
		STlib_updateNum(&w_maxammo[i], refresh);
	}

	if (!gibbed)
	{
		dp_translation = ST_WidgetColor((int)hudcolor_t::hudcolor_health);
		// negative player health
		w_health.n.num = crispy->neghealth ? &plyr->neghealth : &plyr->health;
		STlib_updatePercent(&w_health, refresh);
	}
	dp_translation = ST_WidgetColor((int)hudcolor_t::hudcolor_armor);
	STlib_updatePercent(&w_armor, refresh);
	dp_translation = NULL;

	STlib_updateBinIcon(&w_armsbg, refresh);

	// show SSG availability in the Shotgun slot of the arms widget
	st_shotguns = plyr->weaponowned[::std::size_t(WeaponType::wp_shotgun)] | plyr->weaponowned[::std::size_t(WeaponType::wp_supershotgun)];

	for (i = 0;i < 6; ++i)
	{
		STlib_updateMultIcon(&w_arms[i], refresh);
	}

	// draw the actual face widget background
	if (st_crispyhud && (screenblocks % 3 == 0))
	{
		V_CopyRect(ST_FX + WIDESCREENDELTA, 1, st_backing_screen, SHORT(faceback->width), ST_HEIGHT - 1, ST_FX + WIDESCREENDELTA, ST_Y + 1);
	}

	STlib_updateMultIcon(&w_faces, refresh);

	for (i = 0;i < 3; ++i)
	{
		STlib_updateMultIcon(&w_keyboxes[i], refresh);
	}

	dp_translation = ST_WidgetColor((int)hudcolor_t::hudcolor_frags);
	STlib_updateNum(&w_frags, refresh);

	dp_translation = NULL;
}

void ST_doRefresh()
{
	st_firsttime = false;

	// draw status bar background to off-screen buff
	ST_refreshBackground(false);

	// and refresh all widgets
	ST_drawWidgets(true);
}

void ST_diffDraw()
{
	// update all widgets
	ST_drawWidgets(false);
}

void ST_Drawer(bool fullscreen, bool refresh)
{
	st_statusbaron = (!fullscreen) || (automapactive && !crispy->automapoverlay);
	// immediately redraw status bar after help screens have been shown
	st_firsttime = st_firsttime || refresh || inhelpscreens;

	// distinguish classic status bar with background and player face from Crispy HUD
	st_crispyhud = screenblocks >= CRISPY_HUD && (!automapactive || crispy->automapoverlay);
	st_classicstatusbar = st_statusbaron && !st_crispyhud;
	st_statusbarface = st_classicstatusbar || (st_crispyhud && (screenblocks % 3 == 0));

	// re-calculate widget coordinates on demand
	if (st_widescreendelta != ST_WIDESCREENDELTA)
	{
		void ST_createWidgets();
		ST_createWidgets();
	}

	if (crispy->cleanscreenshot == 2)
	{
		return;
	}

	// Do red-/gold-shifts from damage/items
	ST_doPaletteStuff();

	// translucent HUD
	if (st_crispyhud && (screenblocks % 3 == 2))
	{
		dp_translucent = true;
	}

	// If just after ST_Start(), refresh all
	if (st_firsttime)
	{
		ST_doRefresh();
	}
	// Otherwise, update as little as possible
	else
	{
		ST_diffDraw();
	}

	dp_translucent = false;
}

// Iterates through all graphics to be loaded or unloaded, along with
// the variable they use, invoking the specified callback function.
static void ST_loadUnloadGraphics(load_callback_t callback)
{
	int facenum;

	char namebuf[9];

	// Load the numbers, tall and short
	for (size_t i{0}; i < 10; ++i)
	{
		DEH_snprintf(namebuf, 9, "STTNUM%d", i);
		callback(namebuf, &tallnum[i]);

		DEH_snprintf(namebuf, 9, "STYSNUM%d", i);
		callback(namebuf, &shortnum[i]);
	}

	// Load percent key.
	//Note: why not load STMINUS here, too?

	callback(DEH_String("STTPRCNT"), &tallpercent);

	// key cards
	for (size_t i{0}; i < ::std::size_t(CardType::NUMCARDS); ++i)
	{
		DEH_snprintf(namebuf, 9, "STKEYS%d", i);
		callback(namebuf, &keys[i]);
	}

	// arms background
	callback(DEH_String("STARMS"), &armsbg);

	// arms ownership widgets
	for (size_t i{0}; i < 6; ++i)
	{
		DEH_snprintf(namebuf, 9, "STGNUM%d", i + 2);

		// gray #
		callback(namebuf, &arms[i][0]);

		// yellow #
		arms[i][1] = shortnum[i + 2];
	}

	// face backgrounds for different color players
	DEH_snprintf(namebuf, 9, "STFB%d", consoleplayer);
	callback(namebuf, &faceback);

	// status bar background bits
	if (W_CheckNumForName("STBAR") >= 0)
	{
		callback(DEH_String("STBAR"), &sbar);
		sbarr = NULL;
	}
	else
	{
		callback(DEH_String("STMBARL"), &sbar);
		callback(DEH_String("STMBARR"), &sbarr);
	}

	// face states
	facenum = 0;
	for (size_t i{0}; i < ST_NUMPAINFACES; ++i)
	{
		for (size_t j{0}; j < ST_NUMSTRAIGHTFACES; ++j)
		{
			DEH_snprintf(namebuf, 9, "STFST%d%d", i, j);
			callback(namebuf, &faces[facenum]);
			++facenum;
		}
		DEH_snprintf(namebuf, 9, "STFTR%d0", i);	// turn right
		callback(namebuf, &faces[facenum]);
		++facenum;
		DEH_snprintf(namebuf, 9, "STFTL%d0", i);	// turn left
		callback(namebuf, &faces[facenum]);
		++facenum;
		DEH_snprintf(namebuf, 9, "STFOUCH%d", i);	// ouch!
		callback(namebuf, &faces[facenum]);
		++facenum;
		DEH_snprintf(namebuf, 9, "STFEVL%d", i);	// evil grin ;)
		callback(namebuf, &faces[facenum]);
		++facenum;
		DEH_snprintf(namebuf, 9, "STFKILL%d", i);	// pissed off
		callback(namebuf, &faces[facenum]);
		++facenum;
	}

	callback(DEH_String("STFGOD0"), &faces[facenum]);
	++facenum;
	callback(DEH_String("STFDEAD0"), &faces[facenum]);
	++facenum;
}

static void ST_loadCallback(::std::string lumpname, patch_t** variable)
{
	*variable = W_CacheLumpName<patch_t>(lumpname, pu_tags_t::PU_STATIC);
}

void ST_loadGraphics()
{
	ST_loadUnloadGraphics(ST_loadCallback);
}

void ST_loadData()
{
	lu_palette = W_GetNumForName(DEH_String("PLAYPAL"));
	ST_loadGraphics();

	// support combined card and skull keys (if provided by PWAD) i.e. only for display in the status bar
	for (size_t i{::std::size_t(CardType::NUMCARDS)}; i < ::std::size_t(CardType::NUMCARDS) + 3; ++i)
	{
		char lumpname[9];
		int lumpnum;

		DEH_snprintf(lumpname, 9, "STKEYS%d", i);
		lumpnum = W_CheckNumForName(lumpname);

		keys[i] = (lumpnum != -1) ? W_CacheLumpNum<patch_t>(lumpnum, pu_tags_t::PU_STATIC) : (patch_t*)keys[i - 3];
	}
}

static void ST_unloadCallback(::std::string lumpname, patch_t** variable)
{
	W_ReleaseLumpName(lumpname);
	*variable = NULL;
}

void ST_unloadGraphics()
{
	ST_loadUnloadGraphics(ST_unloadCallback);
}

void ST_unloadData()
{
	ST_unloadGraphics();
}

void ST_initData()
{
	st_firsttime = true;
	plyr = &players[consoleplayer];

	st_clock = 0;
	st_chatstate = st_chatstateenum_t::StartChatState;
	st_gamestate = st_stateenum_t::FirstPersonState;

	st_statusbaron = true;
	st_oldchat = st_chat = false;
	st_cursoron = false;

	faceindex = 0; // fix status bar face hysteresis across level changes
	st_faceindex = 0;
	st_palette = -1;

	st_oldhealth = -1;

	for (size_t i{0}; i < ::std::size_t(WeaponType::NUMWEAPONS); ++i)
	{
		oldweaponsowned[i] = plyr->weaponowned[i];
	}

	for (size_t i{0}; i < 3; ++i)
	{
		keyboxes[i] = -1;
	}

	STlib_init();
}

void ST_createWidgets()
{
	int i;

	// re-calculate WIDESCREENDELTA
	I_GetScreenDimensions();
	st_widescreendelta = ST_WIDESCREENDELTA;

	// ready weapon ammo
	STlib_initNum(&w_ready,
		ST_AMMOX,
		ST_AMMOY,
		tallnum,
		&plyr->ammo[::std::size_t(weaponinfo[::std::size_t(plyr->readyweapon)].ammo)],
		&st_statusbaron,
		ST_AMMOWIDTH);

	// the last weapon type
	w_ready.data = (int)plyr->readyweapon;

	// health percentage
	STlib_initPercent(&w_health,
		ST_HEALTHX,
		ST_HEALTHY,
		tallnum,
		&plyr->health,
		&st_statusbaron,
		tallpercent);

	// arms background
	STlib_initBinIcon(&w_armsbg,
		ST_ARMSBGX,
		ST_ARMSBGY,
		armsbg,
		&st_notdeathmatch,
		&st_classicstatusbar);

	// weapons owned
	for (i = 0;i < 6; ++i)
	{
		STlib_initMultIcon(&w_arms[i],
			ST_ARMSX + (i % 3) * ST_ARMSXSPACE,
			ST_ARMSY + (i / 3) * ST_ARMSYSPACE,
			arms[i],
			&plyr->weaponowned[i + 1],
			&st_armson);
	}
	// show SSG availability in the Shotgun slot of the arms widget
	w_arms[1].inum = &st_shotguns;

	// frags sum
	STlib_initNum(&w_frags,
		ST_FRAGSX,
		ST_FRAGSY,
		tallnum,
		&st_fragscount,
		&st_fragson,
		ST_FRAGSWIDTH);

	// faces
	STlib_initMultIcon(&w_faces,
		ST_FACESX,
		ST_FACESY,
		faces,
		&st_faceindex,
		&st_statusbarface);

	// armor percentage - should be colored later
	STlib_initPercent(&w_armor,
		ST_ARMORX,
		ST_ARMORY,
		tallnum,
		&plyr->armorpoints,
		&st_statusbaron, tallpercent);

	// keyboxes 0-2
	STlib_initMultIcon(&w_keyboxes[0],
		ST_KEY0X,
		ST_KEY0Y,
		keys,
		&keyboxes[0],
		&st_statusbaron);

	STlib_initMultIcon(&w_keyboxes[1],
		ST_KEY1X,
		ST_KEY1Y,
		keys,
		&keyboxes[1],
		&st_statusbaron);

	STlib_initMultIcon(&w_keyboxes[2],
		ST_KEY2X,
		ST_KEY2Y,
		keys,
		&keyboxes[2],
		&st_statusbaron);

	// ammo count (all four kinds)
	STlib_initNum(&w_ammo[0],
		ST_AMMO0X,
		ST_AMMO0Y,
		shortnum,
		&plyr->ammo[0],
		&st_statusbaron,
		ST_AMMO0WIDTH);

	STlib_initNum(&w_ammo[1],
		ST_AMMO1X,
		ST_AMMO1Y,
		shortnum,
		&plyr->ammo[1],
		&st_statusbaron,
		ST_AMMO1WIDTH);

	STlib_initNum(&w_ammo[2],
		ST_AMMO2X,
		ST_AMMO2Y,
		shortnum,
		&plyr->ammo[2],
		&st_statusbaron,
		ST_AMMO2WIDTH);

	STlib_initNum(&w_ammo[3],
		ST_AMMO3X,
		ST_AMMO3Y,
		shortnum,
		&plyr->ammo[3],
		&st_statusbaron,
		ST_AMMO3WIDTH);

	// max ammo count (all four kinds)
	STlib_initNum(&w_maxammo[0],
		ST_MAXAMMO0X,
		ST_MAXAMMO0Y,
		shortnum,
		&plyr->maxammo[0],
		&st_statusbaron,
		ST_MAXAMMO0WIDTH);

	STlib_initNum(&w_maxammo[1],
		ST_MAXAMMO1X,
		ST_MAXAMMO1Y,
		shortnum,
		&plyr->maxammo[1],
		&st_statusbaron,
		ST_MAXAMMO1WIDTH);

	STlib_initNum(&w_maxammo[2],
		ST_MAXAMMO2X,
		ST_MAXAMMO2Y,
		shortnum,
		&plyr->maxammo[2],
		&st_statusbaron,
		ST_MAXAMMO2WIDTH);

	STlib_initNum(&w_maxammo[3],
		ST_MAXAMMO3X,
		ST_MAXAMMO3Y,
		shortnum,
		&plyr->maxammo[3],
		&st_statusbaron,
		ST_MAXAMMO3WIDTH);

}

static bool st_stopped = true;

void ST_Start()
{
	if (!st_stopped)
		ST_Stop();

	ST_initData();
	ST_createWidgets();
	st_stopped = false;

	// correctly color the status bar face background in multiplayer
	// demos recorded by another player than player 1
	if (netgame && consoleplayer)
	{
		char namebuf[8];

		DEH_snprintf(namebuf, 7, "STFB%d", consoleplayer);
		faceback = W_CacheLumpName<patch_t>(namebuf, pu_tags_t::PU_STATIC);
	}
}

void ST_Stop()
{
	if (st_stopped)
		return;

#ifndef CRISPY_TRUECOLOR
	I_SetPalette(W_CacheLumpNum<byte>(lu_palette, pu_tags_t::PU_CACHE));
#else
	I_SetPalette(0);
#endif

	st_stopped = true;
}

void ST_Init()
{
	// colorize the confusing 'behold' power-up menu
	// FIXME
	/* if (!DEH_HasStringReplacement(STSTR_BEHOLD) && !M_ParmExists("-nodeh"))
	{
	char str_behold[80];
	M_snprintf(str_behold, sizeof(str_behold),
				"in%sV%suln, %sS%str, %sI%snviso, %sR%sad, %sA%sllmap, or %sL%site-amp",
				crstr[CR_GOLD], crstr[CR_NONE],
				crstr[CR_GOLD], crstr[CR_NONE],
				crstr[CR_GOLD], crstr[CR_NONE],
				crstr[CR_GOLD], crstr[CR_NONE],
				crstr[CR_GOLD], crstr[CR_NONE],
				crstr[CR_GOLD], crstr[CR_NONE]);
	DEH_AddStringReplacement(STSTR_BEHOLD, str_behold);
	} */

	ST_loadData();
	// FIXME
	//st_backing_screen = (pixel_t*)Z_Malloc(MAXWIDTH * (ST_HEIGHT << 1) * sizeof(*st_backing_screen), pu_tags_t::PU_STATIC, 0);
}

// Demo Timer widget
void ST_DrawDemoTime(const TimeType time)
{
	char buffer[16];
	const TimeType mins = time / (60 * TICRATE);
	const float secs = (float)(time % (60 * TICRATE)) / TICRATE;
	const int w = shortnum[0]->width;
	int n;
	int x;

	// FIXME
	//n = M_snprintf(buffer, sizeof(buffer), "%02i %05.02f", mins, secs);

	x = (viewwindowx >> crispy->hires) + (scaledviewwidth >> crispy->hires) - WIDESCREENDELTA;

	// draw the Demo Timer widget with gray numbers
	dp_translation = cr[CR_GRAY];
	dp_translucent = (gamestate == GameState_t::GS_LEVEL);

	while (n-- > 0)
	{
		const int c = buffer[n] - '0';

		x -= w;

		if (c >= 0 && c <= 9)
		{
			V_DrawPatch(x, viewwindowy >> crispy->hires, shortnum[c]);
		}
	}

	dp_translation = NULL;
	dp_translucent = false;
}
