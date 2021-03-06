/**********************************************************************************************************************************************\
	Copyright(C) 1993-1996 Id Software, Inc.
	Copyright(C) 2005-2014 Simon Howard

	This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License
	as published by the Free Software Foundation; either version 2 of the License, or (at your option) any later version.

	This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

	DESCRIPTION:
		Internally used data structures for virtually everything, lots of other stuff.
\**********************************************************************************************************************************************/
#pragma once

#include "../../derma/common.h"

#include "../i_timer.h"
#include "../d_mode.h"

// Difficulty/skill settings/filters.

// Skill flags.
constexpr size_t MTF_EASY{1};
constexpr size_t MTF_NORMAL{2};
constexpr size_t MTF_HARD{4};

// Deaf monsters/do not react to sound.
constexpr size_t MTF_AMBUSH{8};

// DOOM version
constexpr size_t DOOM_VERSION{109};

// Version code for cph's longtics hack ("v1.91")
constexpr size_t DOOM_191_VERSION{111};

// If rangecheck is undefined, most parameter validation debugging code will not be compiled
//#define RANGE_CHECK

// The maximum number of players, multiplayer/networking.
constexpr size_t MAX_PLAYERS{4};

// The current state of the game: whether we are playing, gazing at the intermission screen, the game final animation, or a demo.
enum class GameState_t
{
	GS_LEVEL,
	GS_INTERMISSION,
	GS_FINALE,
	GS_DEMOSCREEN,
};

enum class GameAction_t
{
	ga_nothing,
	ga_loadlevel,
	ga_newgame,
	ga_loadgame,
	ga_savegame,
	ga_playdemo,
	ga_completed,
	ga_victory,
	ga_worlddone,
	ga_screenshot
};

// Key cards.
enum class CardType
{
	it_bluecard,
	it_yellowcard,
	it_redcard,
	it_blueskull,
	it_yellowskull,
	it_redskull,

	NUMCARDS
};

// The defined weapons, including a marker indicating user has not changed weapon.
enum class WeaponType
{
	wp_fist,
	wp_pistol,
	wp_shotgun,
	wp_chaingun,
	wp_missile,
	wp_plasma,
	wp_bfg,
	wp_chainsaw,
	wp_supershotgun,

	NUMWEAPONS,

	// No pending weapon change.
	wp_nochange
};

// Ammunition types defined.
enum class AmmoType
{
	am_clip,	// Pistol / chaingun ammo.
	am_shell,	// Shotgun / double barreled shotgun.
	am_cell,	// Plasma rifle, BFG.
	am_misl,	// Missile launcher.
	NUMAMMO,
	am_noammo	// Unlimited for chainsaw / fist.
};

// Power up artifacts.
enum class PowerType_t
{
	pw_invulnerability,
	pw_strength,
	pw_invisibility,
	pw_ironfeet,
	pw_allmap,
	pw_infrared,
	// showfps and mapcoords are now "powers"
	pw_showfps,
	pw_mapcoords,
	NUMPOWERS,
};

// Power up durations, how many seconds till expiration, assuming TICRATE is 35 ticks/second.
enum class PowerDuration_t
{
	INVULNTICS = (30 * TICRATE),
	INVISTICS = (60 * TICRATE),
	INFRATICS = (120 * TICRATE),
	IRONTICS = (60 * TICRATE)
};
