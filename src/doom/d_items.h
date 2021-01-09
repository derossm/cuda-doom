/**********************************************************************************************************************************************\
	Copyright(C) 1993-1996 Id Software, Inc.
	Copyright(C) 2005-2014 Simon Howard

	This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License
	as published by the Free Software Foundation; either version 2 of the License, or (at your option) any later version.

	This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

	DESCRIPTION:
		Items: key cards, artifacts, weapon, ammunition.
\**********************************************************************************************************************************************/
#pragma once

#include "../../derma/common.h"

#include "doomdef.h"

// We are referring to sprite numbers.
#include "info.h"

// Weapon info: sprite frames, ammunition use.
struct weaponinfo_t
{
	AmmoType ammo;
	statenum_t upstate;
	statenum_t downstate;
	statenum_t readystate;
	statenum_t atkstate;
	statenum_t flashstate;
};

// PSPRITE ACTIONS for waepons.
// This struct controls the weapon animations.
//
// Each entry is:
// ammo/amunition type
// upstate
// downstate
// readystate
// atkstate, i.e. attack/fire/hit frame
// flashstate, muzzle flash
weaponinfo_t weaponinfo[::std::size_t(WeaponType::NUMWEAPONS)]{
	{
	// fist
	AmmoType::am_noammo,
	statenum_t::S_PUNCHUP,
	statenum_t::S_PUNCHDOWN,
	statenum_t::S_PUNCH,
	statenum_t::S_PUNCH1,
	statenum_t::S_NULL
	},
	{
	// pistol
	AmmoType::am_clip,
	statenum_t::S_PISTOLUP,
	statenum_t::S_PISTOLDOWN,
	statenum_t::S_PISTOL,
	statenum_t::S_PISTOL1,
	statenum_t::S_PISTOLFLASH
	},
	{
	// shotgun
	AmmoType::am_shell,
	statenum_t::S_SGUNUP,
	statenum_t::S_SGUNDOWN,
	statenum_t::S_SGUN,
	statenum_t::S_SGUN1,
	statenum_t::S_SGUNFLASH1
	},
	{
	// chaingun
	AmmoType::am_clip,
	statenum_t::S_CHAINUP,
	statenum_t::S_CHAINDOWN,
	statenum_t::S_CHAIN,
	statenum_t::S_CHAIN1,
	statenum_t::S_CHAINFLASH1
	},
	{
	// missile launcher
	AmmoType::am_misl,
	statenum_t::S_MISSILEUP,
	statenum_t::S_MISSILEDOWN,
	statenum_t::S_MISSILE,
	statenum_t::S_MISSILE1,
	statenum_t::S_MISSILEFLASH1
	},
	{
	// plasma rifle
	AmmoType::am_cell,
	statenum_t::S_PLASMAUP,
	statenum_t::S_PLASMADOWN,
	statenum_t::S_PLASMA,
	statenum_t::S_PLASMA1,
	statenum_t::S_PLASMAFLASH1
	},
	{
	// bfg 9000
	AmmoType::am_cell,
	statenum_t::S_BFGUP,
	statenum_t::S_BFGDOWN,
	statenum_t::S_BFG,
	statenum_t::S_BFG1,
	statenum_t::S_BFGFLASH1
	},
	{
	// chainsaw
	AmmoType::am_noammo,
	statenum_t::S_SAWUP,
	statenum_t::S_SAWDOWN,
	statenum_t::S_SAW,
	statenum_t::S_SAW1,
	statenum_t::S_NULL
	},
	{
	// super shotgun
	AmmoType::am_shell,
	statenum_t::S_DSGUNUP,
	statenum_t::S_DSGUNDOWN,
	statenum_t::S_DSGUN,
	statenum_t::S_DSGUN1,
	statenum_t::S_DSGUNFLASH1
	}
};
