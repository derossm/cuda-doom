/**********************************************************************************************************************************************\
	Copyright(C) 1993-1996 Id Software, Inc.
	Copyright(C) 2005-2014 Simon Howard

	This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License
	as published by the Free Software Foundation; either version 2 of the License, or (at your option) any later version.

	This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

	DESCRIPTION:
		Game completion, final screen animation.
\**********************************************************************************************************************************************/

#include <string>

// Functions.
#include "deh_main.h"
#include "i_system.h"
#include "i_swap.h"
#include "z_zone.h"
#include "v_video.h"
#include "w_wad.h"
#include "s_sound.h"

// Data.
#include "d_main.h"
#include "dstrings.h"
#include "sounds.h"

#include "doomstat.h"
#include "r_state.h"
#include "m_controls.h"		// key_*
#include "m_random.h"		// Crispy_Random()

#include "hu_stuff.h"

extern patch_t* hu_font[HU_FONTSIZE];

enum class finalestage_t
{
	F_STAGE_TEXT,
	F_STAGE_ARTSCREEN,
	F_STAGE_CAST
};

// Stage of animation:
finalestage_t finalestage;

unsigned finalecount;

constexpr int TEXTSPEED{3};
constexpr int TEXTWAIT{250};

struct textscreen_t
{
	GameMission mission;
	int episode;
	int level;
	std::string background;
	std::string text;
};

static textscreen_t textscreens[]{
	{ GameMission::doom,			1, 8,	"FLOOR4_8",	E1TEXT },
	{ GameMission::doom,			2, 8,	"SFLR6_1",	E2TEXT },
	{ GameMission::doom,			3, 8,	"MFLR8_4",	E3TEXT },
	{ GameMission::doom,			4, 8,	"MFLR8_3",	E4TEXT },
	{ GameMission::doom,			5, 8,	"FLOOR7_2",	E5TEXT }, // Sigil

	{ GameMission::doom2,			1, 6,	"SLIME16",	C1TEXT },
	{ GameMission::doom2,			1, 11,	"RROCK14",	C2TEXT },
	{ GameMission::doom2,			1, 20,	"RROCK07",	C3TEXT },
	{ GameMission::doom2,			1, 30,	"RROCK17",	C4TEXT },
	{ GameMission::doom2,			1, 15,	"RROCK13",	C5TEXT },
	{ GameMission::doom2,			1, 31,	"RROCK19",	C6TEXT },

	{ GameMission::pack_tnt,		1, 6,	"SLIME16",	T1TEXT },
	{ GameMission::pack_tnt,		1, 11,	"RROCK14",	T2TEXT },
	{ GameMission::pack_tnt,		1, 20,	"RROCK07",	T3TEXT },
	{ GameMission::pack_tnt,		1, 30,	"RROCK17",	T4TEXT },
	{ GameMission::pack_tnt,		1, 15,	"RROCK13",	T5TEXT },
	{ GameMission::pack_tnt,		1, 31,	"RROCK19",	T6TEXT },

	{ GameMission::pack_plut,		1, 6,	"SLIME16",	P1TEXT },
	{ GameMission::pack_plut,		1, 11,	"RROCK14",	P2TEXT },
	{ GameMission::pack_plut,		1, 20,	"RROCK07",	P3TEXT },
	{ GameMission::pack_plut,		1, 30,	"RROCK17",	P4TEXT },
	{ GameMission::pack_plut,		1, 15,	"RROCK13",	P5TEXT },
	{ GameMission::pack_plut,		1, 31,	"RROCK19",	P6TEXT },

	{ GameMission::pack_nerve,	1, 8,	"SLIME16",	N1TEXT },
	{ GameMission::pack_master,	1, 20,	"SLIME16",	M1TEXT },
	{ GameMission::pack_master,	1, 21,	"SLIME16",	M2TEXT }
};

std::string finaletext;
std::string finaleflat;
std::string finaletext_rw;

void F_StartCast();
void F_CastTicker();
bool F_CastResponder(EventType* ev);
void F_CastDrawer();

extern void A_RandomJump();

void F_StartFinale()
{
	gameaction = GameAction_t::ga_nothing;
	gamestate = GameState_t::GS_FINALE;
	viewactive = false;
	automapactive = false;

	if (logical_gamemission == GameMission::doom)
	{
		S_ChangeMusic(musicenum_t::mus_victor, true);
	}
	else
	{
		S_ChangeMusic(musicenum_t::mus_read_m, true);
	}

	// Find the right screen and set the text and background

	for (size_t i{0}; i < arrlen(textscreens); ++i)
	{
		textscreen_t* screen = &textscreens[i];

		// Hack for Chex Quest
		if (gameversion == GameVersion::exe_chex && screen->mission == GameMission::doom)
		{
			screen->level = 5;
		}

		if (logical_gamemission == screen->mission && (logical_gamemission != GameMission::doom || gameepisode == screen->episode)
			&& gamemap == screen->level)
		{
			finaletext = screen->text;
			finaleflat = screen->background;
		}
	}

	// Do dehacked substitutions of strings
	finaletext = DEH_String(finaletext);
	finaleflat = DEH_String(finaleflat);

	finaletext_rw = std::string(finaletext);

	finalestage = finalestage_t::F_STAGE_TEXT;
	finalecount = 0;
}

bool F_Responder(EventType* event)
{
	if (finalestage == finalestage_t::F_STAGE_CAST)
	{
		return F_CastResponder(event);
	}

	return false;
}

void F_Ticker()
{
	// check for skipping
	if (size_t i; (gamemode == GameMode::commercial) && (finalecount > 50))
	{
		// go on to the next level
		for (i = 0; i < MAX_PLAYERS; ++i)
		{
			if ((bool)players[i].cmd.buttons)
			{
				break;
			}
		}

		if (i < MAX_PLAYERS)
		{
			if (gamemission == GameMission::pack_nerve && gamemap == 8)
			{
				F_StartCast();
			}
			else if (gamemission == GameMission::pack_master && (gamemap == 20 || gamemap == 21))
			{
				F_StartCast();
			}
			else if (gamemap == 30)
			{
				F_StartCast();
			}
			else
			{
				gameaction = GameAction_t::ga_worlddone;
			}
		}
	}

	// advance animation
	++finalecount;

	if (finalestage == finalestage_t::F_STAGE_CAST)
	{
		F_CastTicker();
		return;
	}

	if (gamemode == GameMode::commercial)
	{
		return;
	}

	if (finalestage == finalestage_t::F_STAGE_TEXT && finalecount > finaletext.length() * TEXTSPEED + TEXTWAIT)
	{
		finalecount = 0;
		finalestage = finalestage_t::F_STAGE_ARTSCREEN;
		wipegamestate = (GameState_t)-1;		// force a wipe

		if (gameepisode == 3)
		{
			S_StartMusic(musicenum_t::mus_bunny);
		}
	}
}

// add line breaks for lines exceeding screenwidth
static inline bool F_AddLineBreak(std::string c)
{
	/*
		while (c-- > finaletext_rw)
		{
			if (*c == '\n')
			{
				return false;
			}
			else if (*c == ' ')
			{
				*c = '\n';
				return true;
			}
		}

		return false;
	*/
}

void F_TextWrite()
{
	/*
		byte* src;
		pixel_t* dest;

		int x;
		int y;
		int w;
		int count;
		std::string ch; // [crispy] un-const
		int c;
		int cx;
		int cy;

		// erase the entire screen to a tiled background
		src = W_CacheLumpName<byte>(finaleflat, pu_tags_t::PU_CACHE);
		dest = I_VideoBuffer;

		for (y=0; y<SCREENHEIGHT ; ++y)
		{
	#ifndef CRISPY_TRUECOLOR
			for (x=0; x < SCREENWIDTH/64; ++x)
			{
				memcpy(dest, src+((y&63)<<6), 64);
				dest += 64;
			}
			if (SCREENWIDTH&63)
			{
				memcpy(dest, src+((y&63)<<6), SCREENWIDTH&63);
				dest += (SCREENWIDTH&63);
			}
	#else
			for (x=0; x < SCREENWIDTH; ++x)
			{
				*dest = colormaps[src[((y&63)<<6) + (x&63)]];
				++dest;
			}
	#endif
		}

		V_MarkRect(0, 0, SCREENWIDTH, SCREENHEIGHT);

		// draw some of the text onto the screen
		cx = 10;
		cy = 10;
		ch = finaletext_rw;

		count = ((int)finalecount - 10) / TEXTSPEED;
		if (count < 0)
		{
			count = 0;
		}

		for ( ; count; --count)
		{
			c = *ch;
			++ch;
			if (!c)
			{
				break;
			}

			if (c == '\n')
			{
				cx = 10;
				cy += 11;
				continue;
			}

			c = toupper(c) - HU_FONTSTART;
			if (c < 0 || c> HU_FONTSIZE)
			{
				cx += 4;
				continue;
			}

			w = SHORT (hu_font[c]->width);
			if (cx+w > ORIGWIDTH)
			{
				// [crispy] add line breaks for lines exceeding screenwidth
				if (F_AddLineBreak(ch))
				{
					continue;
				}
				else
				break;
			}
			// prevent text from being drawn off-screen vertically
			if (cy + SHORT(hu_font[c]->height) > ORIGHEIGHT)
			{
				break;
			}
			V_DrawPatch(cx, cy, hu_font[c]);
			cx+=w;
		}
	*/
}

// Final DOOM 2 animation
// Casting by id Software in order of appearance
struct castinfo_t
{
	const char* name;
	mobjtype_t type;
};

castinfo_t castorder[]{
	{ CC_ZOMBIE, mobjtype_t::MT_POSSESSED },
	{ CC_SHOTGUN, mobjtype_t::MT_SHOTGUY },
	{ CC_HEAVY, mobjtype_t::MT_CHAINGUY },
	{ CC_IMP, mobjtype_t::MT_TROOP },
	{ CC_DEMON, mobjtype_t::MT_SERGEANT },
	{ CC_LOST, mobjtype_t::MT_SKULL },
	{ CC_CACO, mobjtype_t::MT_HEAD },
	{ CC_HELL, mobjtype_t::MT_KNIGHT },
	{ CC_BARON, mobjtype_t::MT_BRUISER },
	{ CC_ARACH, mobjtype_t::MT_BABY },
	{ CC_PAIN, mobjtype_t::MT_PAIN },
	{ CC_REVEN, mobjtype_t::MT_UNDEAD },
	{ CC_MANCU, mobjtype_t::MT_FATSO },
	{ CC_ARCH, mobjtype_t::MT_VILE },
	{ CC_SPIDER, mobjtype_t::MT_SPIDER },
	{ CC_CYBER, mobjtype_t::MT_CYBORG },
	{ CC_HERO, mobjtype_t::MT_PLAYER },
	{ nullptr, mobjtype_t::MT_NULL }
};

int castnum;
TimeType casttics;
const state_t* caststate;
bool castdeath;
int castframes;
int castonmelee;
bool castattacking;
static char castangle; // turnable cast
static char castskip; // skippable cast
static bool castflip; // flippable death sequence

// randomize seestate and deathstate sounds in the cast
static sfxenum_t F_RandomizeSound(sfxenum_t sound)
{
	if (!crispy->soundfix)
	{
		return sound;
	}

	switch (sound)
	{
		// actor->info->seesound, from p_enemy.c:A_Look()
	case sfxenum_t::sfx_posit1:
	case sfxenum_t::sfx_posit2:
	case sfxenum_t::sfx_posit3:
		return sfxenum_t((int)sfxenum_t::sfx_posit1 + Crispy_Random() % 3);
		break;

	case sfxenum_t::sfx_bgsit1:
	case sfxenum_t::sfx_bgsit2:
		return sfxenum_t((int)sfxenum_t::sfx_bgsit1 + Crispy_Random() % 2);
		break;

		// actor->info->deathsound, from p_enemy.c:A_Scream()
	case sfxenum_t::sfx_podth1:
	case sfxenum_t::sfx_podth2:
	case sfxenum_t::sfx_podth3:
		return sfxenum_t((int)sfxenum_t::sfx_podth1 + Crispy_Random() % 3);
		break;

	case sfxenum_t::sfx_bgdth1:
	case sfxenum_t::sfx_bgdth2:
		return sfxenum_t((int)sfxenum_t::sfx_bgdth1 + Crispy_Random() % 2);
		break;

	default:
		return sound;
		break;
	}
}

extern void A_BruisAttack();
extern void A_BspiAttack();
extern void A_CPosAttack();
extern void A_CPosRefire();
extern void A_CyberAttack();
extern void A_FatAttack1();
extern void A_FatAttack2();
extern void A_FatAttack3();
extern void A_HeadAttack();
extern void A_PainAttack();
extern void A_PosAttack();
extern void A_SargAttack();
extern void A_SkelFist();
extern void A_SkelMissile();
extern void A_SkelWhoosh();
extern void A_SkullAttack();
extern void A_SPosAttack();
extern void A_TroopAttack();
extern void A_VileTarget();

struct actionsound_t
{
	const void* const action;
	const sfxenum_t sound;
	const bool early;
};

static const std::array<const actionsound_t, 19> actionsounds{
	actionsound_t{ A_PosAttack,		sfxenum_t::sfx_pistol,	false },
	actionsound_t{ A_SPosAttack,	sfxenum_t::sfx_shotgn,	false },
	actionsound_t{ A_CPosAttack,	sfxenum_t::sfx_shotgn,	false },
	actionsound_t{ A_CPosRefire,	sfxenum_t::sfx_shotgn,	false },
	actionsound_t{ A_VileTarget,	sfxenum_t::sfx_vilatk,	true },
	actionsound_t{ A_SkelWhoosh,	sfxenum_t::sfx_skeswg,	false },
	actionsound_t{ A_SkelFist,		sfxenum_t::sfx_skepch,	false },
	actionsound_t{ A_SkelMissile,	sfxenum_t::sfx_skeatk,	true },
	actionsound_t{ A_FatAttack1,	sfxenum_t::sfx_firsht,	false },
	actionsound_t{ A_FatAttack2,	sfxenum_t::sfx_firsht,	false },
	actionsound_t{ A_FatAttack3,	sfxenum_t::sfx_firsht,	false },
	actionsound_t{ A_HeadAttack,	sfxenum_t::sfx_firsht,	true },
	actionsound_t{ A_BruisAttack,	sfxenum_t::sfx_firsht,	true },
	actionsound_t{ A_TroopAttack,	sfxenum_t::sfx_claw,	false },
	actionsound_t{ A_SargAttack,	sfxenum_t::sfx_sgtatk,	true },
	actionsound_t{ A_SkullAttack,	sfxenum_t::sfx_sklatk,	false },
	actionsound_t{ A_PainAttack,	sfxenum_t::sfx_sklatk,	true },
	actionsound_t{ A_BspiAttack,	sfxenum_t::sfx_plasma,	false },
	actionsound_t{ A_CyberAttack,	sfxenum_t::sfx_rlaunc,	false }
};

// play attack sound based on state action function (instead of state number)
static sfxenum_t F_SoundForState(statenum_t st)
{
	void* const castaction = (void*)caststate->action.acv;
	void* const nextaction = (void*)(&states[std::size_t(caststate->nextstate)])->action.acv;

	// fix Doomguy in casting sequence
	if (!castaction)
	{
		if (st == statenum_t::S_PLAY_ATK2)
		{
			return sfxenum_t::sfx_dshtgn;
		}
		else
		{
			return sfxenum_t::sfx_None;
		}
	}
	else
	{
		for (size_t i{0}; i < actionsounds.size(); ++i)
		{
			const actionsound_t* const as = &actionsounds[i];

			if ((!as->early && castaction == as->action) || (as->early && nextaction == as->action))
			{
				return as->sound;
			}
		}
	}

	return sfxenum_t::sfx_None;
}

void F_StartCast()
{
	wipegamestate = (GameState_t)-1;		// force a screen wipe
	castnum = 0;
	caststate = &states[std::size_t(mobjinfo[std::size_t(castorder[castnum].type)].seestate)];
	casttics = caststate->tics;
	castdeath = false;
	finalestage = finalestage_t::F_STAGE_CAST;
	castframes = 0;
	castonmelee = 0;
	castattacking = false;
	S_ChangeMusic(musicenum_t::mus_evil, true);
}

void F_CastTicker()
{
	statenum_t st;
	sfxenum_t sfx;

	if (--casttics > 0)
	{
		return;			// not time to change state yet
	}

	if (caststate->tics == -1 || caststate->nextstate == statenum_t::S_NULL || castskip) // [crispy] skippable cast
	{
		if (castskip)
		{
			castnum += castskip;
			castskip = 0;
		}
		else
		{
			// switch from deathstate to next monster
			++castnum;
		}

		castdeath = false;

		if (castorder[castnum].name == nullptr)
		{
			castnum = 0;
		}

		if ((bool)mobjinfo[std::size_t(castorder[castnum].type)].seesound)
		{
			S_StartSound(nullptr, F_RandomizeSound(mobjinfo[std::size_t(castorder[castnum].type)].seesound));
		}

		caststate = &states[std::size_t(mobjinfo[std::size_t(castorder[castnum].type)].seestate)];
		castframes = 0;
		castangle = 0; // turnable cast
		castflip = false; // flippable death sequence
	}
	else
	{
		// just advance to next state in animation
		// fix Doomguy in casting sequence
		/*
		if (!castdeath && caststate == &states[S_PLAY_ATK1])
			goto stopattack;	// Oh, gross hack!
		*/
		// Allow A_RandomJump() in deaths in cast sequence
		if (caststate->action.acp1 == (actionf_p1)A_RandomJump && Crispy_Random() < caststate->misc2)
		{
			st = (statenum_t)caststate->misc1;
		}
		else
		{
			// fix Doomguy in casting sequence
			if (!castdeath && caststate == &states[std::size_t(statenum_t::S_PLAY_ATK1)])
			{
				st = statenum_t::S_PLAY_ATK2;
			}
			else if (!castdeath && caststate == &states[std::size_t(statenum_t::S_PLAY_ATK2)])
			{
				goto stopattack;	// Oh, gross hack!
			}
			else
			{
				st = caststate->nextstate;
			}
		}
		caststate = &states[std::size_t(st)];
		++castframes;

		sfx = F_SoundForState(st);
		/*
				// sound hacks....
				switch (st)
				{
					case statenum_t::S_PLAY_ATK2: sfx = sfxenum_t::sfx_dshtgn; break; // [crispy] fix Doomguy in casting sequence
					case statenum_t::S_POSS_ATK2: sfx = sfxenum_t::sfx_pistol; break;
					case statenum_t::S_SPOS_ATK2: sfx = sfxenum_t::sfx_shotgn; break;
					case statenum_t::S_VILE_ATK2: sfx = sfxenum_t::sfx_vilatk; break;
					case statenum_t::S_SKEL_FIST2: sfx = sfxenum_t::sfx_skeswg; break;
					case statenum_t::S_SKEL_FIST4: sfx = sfxenum_t::sfx_skepch; break;
					case statenum_t::S_SKEL_MISS2: sfx = sfxenum_t::sfx_skeatk; break;
					case statenum_t::S_FATT_ATK8:
					case statenum_t::S_FATT_ATK5:
					case statenum_t::S_FATT_ATK2: sfx = sfxenum_t::sfx_firsht; break;
					case statenum_t::S_CPOS_ATK2:
					case statenum_t::S_CPOS_ATK3:
					case statenum_t::S_CPOS_ATK4: sfx = sfxenum_t::sfx_shotgn; break;
					case statenum_t::S_TROO_ATK3: sfx = sfxenum_t::sfx_claw; break;
					case statenum_t::S_SARG_ATK2: sfx = sfxenum_t::sfx_sgtatk; break;
					case statenum_t::S_BOSS_ATK2:
					case statenum_t::S_BOS2_ATK2:
					case statenum_t::S_HEAD_ATK2: sfx = sfxenum_t::sfx_firsht; break;
					case statenum_t::S_SKULL_ATK2: sfx = sfxenum_t::sfx_sklatk; break;
					case statenum_t::S_SPID_ATK2:
					case statenum_t::S_SPID_ATK3: sfx = sfxenum_t::sfx_shotgn; break;
					case statenum_t::S_BSPI_ATK2: sfx = sfxenum_t::sfx_plasma; break;
					case statenum_t::S_CYBER_ATK2:
					case statenum_t::S_CYBER_ATK4:
					case statenum_t::S_CYBER_ATK6: sfx = sfxenum_t::sfx_rlaunc; break;
					case statenum_t::S_PAIN_ATK3: sfx = sfxenum_t::sfx_sklatk; break;
					default: sfx = 0; break;
				}
		*/
		if ((bool)sfx)
		{
			S_StartSound(nullptr, sfx);
		}
	}

	if (!castdeath && castframes == 12)
	{
		// go into attack frame
		castattacking = true;
		if (castonmelee)
		{
			caststate = &states[std::size_t(mobjinfo[std::size_t(castorder[castnum].type)].meleestate)];
		}
		else
		{
			caststate = &states[std::size_t(mobjinfo[std::size_t(castorder[castnum].type)].missilestate)];
		}
		castonmelee ^= 1;
		if (caststate == &states[std::size_t(statenum_t::S_NULL)])
		{
			if (castonmelee)
			{
				caststate = &states[std::size_t(mobjinfo[std::size_t(castorder[castnum].type)].meleestate)];
			}
			else
			{
				caststate = &states[std::size_t(mobjinfo[std::size_t(castorder[castnum].type)].missilestate)];
			}
		}
	}

	if (castattacking)
	{
		if (castframes == 24 || caststate == &states[std::size_t(mobjinfo[std::size_t(castorder[castnum].type)].seestate)])
		{
		stopattack:
			castattacking = false;
			castframes = 0;
			caststate = &states[std::size_t(mobjinfo[std::size_t(castorder[castnum].type)].seestate)];
		}
	}

	casttics = caststate->tics;
	if (casttics == -1)
	{
		// Allow A_RandomJump() in deaths in cast sequence
		if (caststate->action.acp1 == (actionf_p1)A_RandomJump)
		{
			if (Crispy_Random() < caststate->misc2)
			{
				caststate = &states[std::size_t(caststate->misc1)];
			}
			else
			{
				caststate = &states[std::size_t(caststate->nextstate)];
			}

			casttics = caststate->tics;
		}

		if (casttics == -1)
		{
			casttics = 15;
		}
	}
}

bool F_CastResponder(EventType* ev)
{
	bool xdeath = false;

	if (ev->type != evtype_t::ev_keydown)
	{
		return false;
	}

	// make monsters turnable in cast ...
	if (ev->data1 == key_left)
	{
		if (++castangle > 7)
		{
			castangle = 0;
		}
		return false;
	}
	else if (ev->data1 == key_right)
	{
		if (--castangle < 0)
		{
			castangle = 7;
		}
		return false;
	}
	// ... and allow to skip through them ..
	else if (ev->data1 == key_strafeleft || ev->data1 == key_alt_strafeleft)
	{
		castskip = castnum ? -1 : arrlen(castorder) - 2;
		return false;
	}
	else if (ev->data1 == key_straferight || ev->data1 == key_alt_straferight)
	{
		castskip = +1;
		return false;
	}
	// ... and finally turn them into gibbs
	if (ev->data1 == key_speed)
	{
		xdeath = true;
	}

	if (castdeath)
	{
		return true;			// already in dying frames
	}

	// go into death frame
	castdeath = true;
	if (xdeath && (bool)mobjinfo[std::size_t(castorder[castnum].type)].xdeathstate)
	{
		caststate = &states[std::size_t(mobjinfo[std::size_t(castorder[castnum].type)].xdeathstate)];
	}
	else
	{
		caststate = &states[std::size_t(mobjinfo[std::size_t(castorder[castnum].type)].deathstate)];
	}
	casttics = caststate->tics;
	// Allow A_RandomJump() in deaths in cast sequence
	if (casttics == -1 && caststate->action.acp1 == (actionf_p1)A_RandomJump)
	{
		if (Crispy_Random() < caststate->misc2)
		{
			caststate = &states[caststate->misc1];
		}
		else
		{
			caststate = &states[std::size_t(caststate->nextstate)];
		}
		casttics = caststate->tics;
	}
	castframes = 0;
	castattacking = false;
	if (xdeath && (bool)mobjinfo[std::size_t(castorder[castnum].type)].xdeathstate)
	{
		S_StartSound(nullptr, sfxenum_t::sfx_slop);
	}
	else if ((bool)mobjinfo[std::size_t(castorder[castnum].type)].deathsound)
	{
		S_StartSound(nullptr, F_RandomizeSound(mobjinfo[std::size_t(castorder[castnum].type)].deathsound));
	}

	// flippable death sequence
	castflip = crispy->flipcorpses && castdeath
		&& ((int)mobjinfo[std::size_t(castorder[castnum].type)].flags & (int)mobjflag_e::MF_FLIPPABLE) && (Crispy_Random() & 1);

	return true;
}

void F_CastPrint(std::string text)
{
	/*
		std::string ch;
		int c;
		int cx;
		int w;
		int width;

		// find width
		ch = text;
		width = 0;

		while (ch)
		{
			c = *ch;
			++ch;
			if (!c)
			{
				break;
			}
			c = toupper(c) - HU_FONTSTART;
			if (c < 0 || c> HU_FONTSIZE)
			{
				width += 4;
				continue;
			}

			w = SHORT (hu_font[c]->width);
			width += w;
		}

		// draw it
		cx = ORIGWIDTH/2-width/2;
		ch = text;
		while (ch)
		{
			c = *ch;
			++ch;
			if (!c)
			{
				break;
			}
			c = toupper(c) - HU_FONTSTART;
			if (c < 0 || c> HU_FONTSIZE)
			{
				cx += 4;
				continue;
			}

			w = SHORT (hu_font[c]->width);
			V_DrawPatch(cx, 180, hu_font[c]);
			cx+=w;
		}
	*/
}

void F_CastDrawer()
{
	// erase the entire screen to a background
	V_DrawPatchFullScreen(W_CacheLumpName<patch_t>(DEH_String("BOSSBACK"), pu_tags_t::PU_CACHE), false);

	F_CastPrint(DEH_String(castorder[castnum].name));

	// draw the current frame in the middle of the screen
	spritedef_t* sprdef = &sprites[std::size_t(caststate->sprite)];
	// the TNT1 sprite is not supposed to be rendered anyway
	if (!sprdef->numframes && caststate->sprite == spritenum_t::SPR_TNT1)
	{
		return;
	}
	spriteframe_t* sprframe = &sprdef->spriteframes[caststate->frame & FF_FRAMEMASK];
	int lump = sprframe->lump[castangle]; // turnable cast
	bool flip = (bool)sprframe->flip[castangle] ^ castflip; // turnable cast, flippable death sequence

	patch_t* patch = W_CacheLumpNum<patch_t>(lump + firstspritelump, pu_tags_t::PU_CACHE);
	if (flip)
	{
		V_DrawPatchFlipped(ORIGWIDTH / 2, 170, patch);
	}
	else
	{
		V_DrawPatch(ORIGWIDTH / 2, 170, patch);
	}
}

static fixed_t dxi;
static fixed_t dy;
static fixed_t dyi;

void F_DrawPatchCol(int x, patch_t* patch, int col)
{
	column_t* column;
	byte* source;
	pixel_t* dest;
	pixel_t* desttop;
	int count;

	column = (column_t*)((byte*)patch + LONG(patch->columnofs[col]));
	desttop = I_VideoBuffer + x;

	// step through the posts in a column
	while (column->topdelta != 0xff)
	{
		int srccol = 0;
		source = (byte*)column + 3;
		dest = desttop + ((column->topdelta * dy) >> FRACBITS) * SCREENWIDTH;
		count = (column->length * dy) >> FRACBITS;

		while (count--)
		{
			*dest = source[srccol >> FRACBITS];
			srccol += dyi;
			dest += SCREENWIDTH;
		}
		column = (column_t*)((byte*)column + column->length + 4);
	}
}

void F_BunnyScroll()
{
	int scrolled;
	int x;
	patch_t* p1;
	patch_t* p2;
	char name[10];
	int stage;
	static int laststage;
	int p2offset;
	int p1offset;
	int pillar_width;

	dxi = (ORIGWIDTH << FRACBITS) / NONWIDEWIDTH;
	dy = (SCREENHEIGHT << FRACBITS) / ORIGHEIGHT;
	dyi = (ORIGHEIGHT << FRACBITS) / SCREENHEIGHT;

	p1 = W_CacheLumpName<patch_t>(DEH_String("PFUB2"), pu_tags_t::PU_LEVEL);
	p2 = W_CacheLumpName<patch_t>(DEH_String("PFUB1"), pu_tags_t::PU_LEVEL);

	// fill pillarboxes in widescreen mode
	pillar_width = (SCREENWIDTH - (p1->width << FRACBITS) / dxi) / 2;

	if (pillar_width > 0)
	{
		V_DrawFilledBox(0, 0, pillar_width, SCREENHEIGHT, 0);
		V_DrawFilledBox(SCREENWIDTH - pillar_width, 0, pillar_width, SCREENHEIGHT, 0);
	}
	else
	{
		pillar_width = 0;
	}

	// Calculate the portion of PFUB2 that would be offscreen at original res.
	p1offset = (ORIGWIDTH - p1->width) / 2;

	if (p2->width == ORIGWIDTH)
	{
		// Unity or original PFUBs.
		// PFUB1 only contains the pixels that scroll off.
		p2offset = ORIGWIDTH - p1offset;
	}
	else
	{
		// Widescreen mod PFUBs.
		// Right side of PFUB2 and left side of PFUB1 are identical.
		p2offset = ORIGWIDTH + p1offset;
	}

	V_MarkRect(0, 0, SCREENWIDTH, SCREENHEIGHT);

	scrolled = (ORIGWIDTH - ((int)finalecount - 230) / 2);
	if (scrolled > ORIGWIDTH)
	{
		scrolled = ORIGWIDTH;
	}
	if (scrolled < 0)
	{
		scrolled = 0;
	}

	for (x = pillar_width; x < SCREENWIDTH - pillar_width; ++x)
	{
		int x2 = ((x * dxi) >> FRACBITS) - WIDESCREENDELTA + scrolled;

		if (x2 < p2offset)
		{
			F_DrawPatchCol(x, p1, x2 - p1offset);
		}
		else
		{
			F_DrawPatchCol(x, p2, x2 - p2offset);
		}
	}

	if (finalecount < 1130)
	{
		return;
	}
	if (finalecount < 1180)
	{
		V_DrawPatch((ORIGWIDTH - 13 * 8) / 2,
			(ORIGHEIGHT - 8 * 8) / 2,
			W_CacheLumpName<patch_t>(DEH_String("END0"), pu_tags_t::PU_CACHE));
		laststage = 0;
		return;
	}

	stage = (finalecount - 1180) / 5;
	if (stage > 6)
	{
		stage = 6;
	}
	if (stage > laststage)
	{
		S_StartSound(nullptr, sfxenum_t::sfx_pistol);
		laststage = stage;
	}

	DEH_snprintf(name, 10, "END%i", stage);
	V_DrawPatch((ORIGWIDTH - 13 * 8) / 2,
		(ORIGHEIGHT - 8 * 8) / 2,
		W_CacheLumpName<patch_t>(name, pu_tags_t::PU_CACHE));
}

static void F_ArtScreenDrawer()
{
	std::string lumpname;

	if (gameepisode == 3)
	{
		F_BunnyScroll();
	}
	else
	{
		switch (gameepisode)
		{
		case 1:
			if (gameversion >= GameVersion::exe_ultimate)
			{
				lumpname = "CREDIT";
			}
			else
			{
				lumpname = "HELP2";
			}
			break;
		case 2:
			lumpname = "VICTORY2";
			break;
		case 4:
			lumpname = "ENDPIC";
			break;
		case 5:
			lumpname = "SIGILEND";
			if (W_CheckNumForName(DEH_String(lumpname)) == -1)
			{
				return;
			}
			break;
		default:
			return;
		}

		lumpname = DEH_String(lumpname);

		V_DrawPatchFullScreen(W_CacheLumpName<patch_t>(lumpname, pu_tags_t::PU_CACHE), false);
	}
}

void F_Drawer()
{
	switch (finalestage)
	{
	case finalestage_t::F_STAGE_CAST:
		F_CastDrawer();
		break;
	case finalestage_t::F_STAGE_TEXT:
		F_TextWrite();
		break;
	case finalestage_t::F_STAGE_ARTSCREEN:
		F_ArtScreenDrawer();
		break;
	}
}
