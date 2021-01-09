/**********************************************************************************************************************************************\
	Copyright(C) 1993-1996 Id Software, Inc.
	Copyright(C) 2005-2014 Simon Howard

	This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License
	as published by the Free Software Foundation; either version 2 of the License, or (at your option) any later version.

	This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
\**********************************************************************************************************************************************/
#pragma once

#include <string>

#include <array>

#include "../../derma/common.h"

#include "d_items.h"
#include "p_pspr.h"
#include "p_mobj.h"
#include "d_ticcmd.h"

#include "net_defs.h"

enum class PlayerState
{
	// Playing or camping.
	live,
	// Dead on the ground, view follows killer.
	dead,
	// Ready to restart/respawn???
	reborn

};

// Player internal flags, for cheats and debug.
enum class CheatType
{
	// No clipping, walk through barriers.
	CF_NOCLIP = 1,
	// No damage, no health loss.
	CF_GODMODE = 2,
	// Not really a cheat, just a debug aid.
	CF_NOMOMENTUM = 4,
	// monsters don't target
	CF_NOTARGET = 8

};

class Player : public MapObject
{
public:

	// Who did damage (NULL for floors/ceilings).
	MapObject* attacker;
	//::std::weak_ptr<MapObject> attacker;

	// weapon sound source
	MapObject* so;

	// For intermission stats.
	int killcount{0};
	int itemcount{0};
	int secretcount{0};

	// This is only used between levels, mo->health is used during levels.
	int health;
	int armorpoints;

	// Armor type is 0-2.
	int armortype;

	// True if button down last tic.
	int attackdown;
	int usedown;

	// Bit flags, for cheats and debug.
	// See cheat_t, above.
	int cheats;

	// Refired shots are less accurate.
	int refire;

	// For screen flashing (red or bright).
	int damagecount;
	int bonuscount;

	// So gun flashes light up areas.
	int extralight;

	// Current PLAYPAL, ??? can be set to REDCOLORMAP for pain, etc.
	int fixedcolormap;

	// Player skin colorshift, 0-3 for which color to draw player.
	int colormap;

	// negative player health
	int neghealth;

	// "use" button timer
	int btuse;

	// free look / mouse look
	int lookdir;
	int oldlookdir;

	// Determine POV, including viewpoint bobbing during movement. Focal origin above r.z
	fixed_t viewz;
	// Base height above floor for viewz.
	fixed_t viewheight;
	// Bob/squat speed.
	fixed_t deltaviewheight;
	// bounded/scaled total momentum.
	fixed_t bob;

	// weapon recoil pitch
	fixed_t recoilpitch;
	fixed_t oldrecoilpitch;

	// squat down weapon sprite
	fixed_t psp_dy_max;

	// variable player view bob
	fixed_t bob2;

	// Previous position of viewz before think. Used to interpolate between camera positions.
	angle_t oldviewz;

	TimeType btuse_tics;
	// jumping
	TimeType jumpTics;

	// Power ups. invinc and invis are tic counters.
	::std::array<int, ::std::size_t(PowerType_t::NUMPOWERS)> powers; // showfps and mapcoords are now "powers"
	::std::array<int, ::std::size_t(WeaponType::NUMWEAPONS)> weaponowned;
	::std::array<int, ::std::size_t(AmmoType::NUMAMMO)> ammo;
	::std::array<int, ::std::size_t(AmmoType::NUMAMMO)> maxammo;
	// Frags, kills of other players.
	::std::array<int, MAX_PLAYERS> frags;

	// Overlay view sprites (gun, etc).
	::std::array<pspdef_t, ::std::size_t(psprnum_t::NUMPSPRITES)> psprites;

	::std::array<bool, ::std::size_t(CardType::NUMCARDS)> cards;

	// Hint messages.
	::std::string message;

	// how centered "Secret Revealed!" message
	::std::string centermessage;

	ticcmd_t cmd;

	// True if secret level has been done.
	bool didsecret;

	// free look / mouse look
	bool centering;

	bool backpack;

	// blinking key or skull in the status bar
	bool tryopen[::std::size_t(CardType::NUMCARDS)];

	WeaponType readyweapon;

	// Is wp_nochange if not changing.
	WeaponType pendingweapon;

	PlayerState playerstate;
};

struct wbplayerstruct_t
{
	bool in;	// whether the player is in game

	// Player stats, kills, collected items etc.
	int skills;
	int sitems;
	int ssecret;
	int frags[4];
	int score;		// current score on entry, modified on return

	TimeType stime;
};

struct wbstartstruct_t
{
	int epsd;		// episode # (0-2)

	// previous and next levels, origin 0
	int last;
	int next;

	int maxkills;
	int maxitems;
	int maxsecret;
	int maxfrags;

	// index of this player in game
	int pnum;

	// the par time
	TimeType partime;
	// total game time for completed levels so far
	TimeType totaltimes;

	// if true, splash the secret level
	bool didsecret;

	wbplayerstruct_t plyr[MAX_PLAYERS];
};
