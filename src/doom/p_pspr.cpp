/**********************************************************************************************************************************************\
	Copyright(C) 1993-1996 Id Software, Inc.
	Copyright(C) 2005-2014 Simon Howard

	This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License
	as published by the Free Software Foundation; either version 2 of the License, or (at your option) any later version.

	This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

	DESCRIPTION:
		Weapon sprite animation, weapon objects. Action functions for weapons.
\**********************************************************************************************************************************************/
#include "../derma/stdafx.h"

#include "doomdef.h"
#include "d_event.h"

#include "deh_misc.h"

#include "m_random.h"
#include "p_local.h"
#include "s_sound.h"

// State.
#include "doomstat.h"

// Data.
#include "sounds.h"

#include "p_pspr.h"

constexpr size_t LOWERSPEED{FRACUNIT*6};
constexpr size_t RAISESPEED{FRACUNIT*6};

constexpr size_t WEAPONBOTTOM{128*FRACUNIT};
constexpr size_t WEAPONTOP{32*FRACUNIT};

// weapon recoil {thrust, pitch} values
// thrust values from prboom-plus/src/p_pspr.c:73-83
static const int recoil_values[][2] = {
 {10,	0}, // WeaponType::wp_fist
 {10,	4}, // WeaponType::wp_pistol
 {30,	8}, // WeaponType::wp_shotgun
 {10,	4}, // WeaponType::wp_chaingun
 {100, 16}, // WeaponType::wp_missile
 {20,	4}, // WeaponType::wp_plasma
 {100, 20}, // WeaponType::wp_bfg
 {0,	-2}, // WeaponType::wp_chainsaw
 {80, 16}, // WeaponType::wp_supershotgun
};

// add weapon recoil
// adapted from prboom-plus/src/p_pspr.c:484-495 (A_FireSomething ())
extern void P_Thrust(Player* player, angle_t angle, fixed_t move);
void A_Recoil(Player* player)
{
	if (player)
	{
		if (critical->recoil && !((int)player->flags & (int)mobjflag_e::MF_NOCLIP))
		{
			P_Thrust(player, ANG180 + player->angle, 2048 * recoil_values[::std::size_t(::std::size_t(player->readyweapon)][0)]);
		}

		if (crispy->pitch)
		{
			player->recoilpitch = recoil_values[::std::size_t(::std::size_t(player->readyweapon)][1)];
		}
	}
}

void P_SetPsprite(Player* player, int position, statenum_t stnum)
{
	pspdef_t* psp;
	state_t* state;

	psp = &player->psprites[position];

	do
	{
		if (!(bool)stnum)
		{
			// object removed itself
			psp->state = NULL;
			break;
		}

		state = &states[::std::size_t(stnum)];
		psp->state = state;
		psp->tics = state->tics;	// could be 0

		if (state->misc1)
		{
			// coordinate set
			psp->sx = state->misc1 << FRACBITS;
			psp->sy = state->misc2 << FRACBITS;
			// variable weapon sprite bob
			psp->sx2 = psp->sx;
			psp->sy2 = psp->sy;
		}

		// Call action routine. Modified handling.
		if (state->action.acp3)
		{
			state->action.acp3(player, player, psp); // let mobj action pointers get called from pspr states
			if (!psp->state)
			{
				break;
			}
		}

		stnum = psp->state->nextstate;

	} while (!psp->tics);
	// an initial state of 0 could cycle through
}

fixed_t swingx;
fixed_t swingy;

void P_CalcSwing(Player* player)
{
	fixed_t swing;
	int angle;

	// OPTIMIZE: tablify this.
	// A LUT would allow for different modes,
	// and add flexibility.

	swing = player->bob;

	angle = (FINEANGLES/70*leveltime)&FINEMASK;
	swingx = FixedMul( swing, finesine[angle]);

	angle = (FINEANGLES/70*leveltime+FINEANGLES/2)&FINEMASK;
	swingy = -FixedMul( swingx, finesine[angle]);
}

//
// P_BringUpWeapon
// Starts bringing the pending weapon up
// from the bottom of the screen.
// Uses player
//
void P_BringUpWeapon(Player* player)
{
	statenum_t newstate;

	if (player->pendingweapon == WeaponType::wp_nochange)
	player->pendingweapon = player->readyweapon;

	if (player->pendingweapon == WeaponType::wp_chainsaw)
	S_StartSound(player, sfxenum_t::sfx_sawup); // intentionally not weapon sound source

#if 0
	// play "power up" sound when selecting berserk fist...
	if (player->pendingweapon == WeaponType::wp_fist && player->powers[::std::size_t(PowerType_t::pw_strength)])
	{
	// ...only if not playing already
	if (player == &players[consoleplayer])
	{
		S_StartSoundOnce (NULL, sfxenum_t::sfx_getpow);
	}
	}
#endif

	newstate = weaponinfo[::std::size_t(player->pendingweapon)].upstate;

	player->pendingweapon = WeaponType::wp_nochange;
	player->psprites[::std::size_t(psprnum_t::ps_weapon)].sy = WEAPONBOTTOM;
	// squat down weapon sprite
	player->psprites[::std::size_t(psprnum_t::ps_weapon)].dy = 0;

	P_SetPsprite(player, (int)psprnum_t::ps_weapon, newstate);
}

//
// P_CheckAmmo
// Returns true if there is enough ammo to shoot.
// If not, selects the next weapon to use.
//
bool P_CheckAmmo(Player* player)
{
	AmmoType ammo;
	int count;

	ammo = weaponinfo[::std::size_t(player->readyweapon)].ammo;

	// Minimal amount for one shot varies.
	if (player->readyweapon == WeaponType::wp_bfg)
	count = deh_bfg_cells_per_shot;
	else if (player->readyweapon == WeaponType::wp_supershotgun)
	count = 2;	// Double barrel.
	else
	count = 1;	// Regular.

	// force weapon switch if weapon not owned
	// only relevant when removing current weapon with TNTWEAPx cheat
	if (!player->weaponowned[::std::size_t(player->readyweapon)])
	{
	ammo = AmmoType::am_clip; // at least not am_noammo, see below
	count = INT_MAX;
	}

	// Some do not need ammunition anyway.
	// Return if current ammunition sufficient.
	if (ammo == AmmoType::am_noammo || player->ammo[::std::size_t(ammo)] >= count)
	return true;

	// Out of ammo, pick a weapon to change to.
	// Preferences are set here.
	do
	{
	if (player->weaponowned[::std::size_t(WeaponType::wp_plasma)]
		&& player->ammo[::std::size_t(AmmoType::am_cell)]
		&& (gamemode != GameMode::shareware) )
	{
		player->pendingweapon = WeaponType::wp_plasma;
	}
	else if (player->weaponowned[::std::size_t(WeaponType::wp_supershotgun)]
			&& player->ammo[::std::size_t(AmmoType::am_shell)]>2
			&& (crispy->havessg) )
	{
		player->pendingweapon = WeaponType::wp_supershotgun;
	}
	else if (player->weaponowned[::std::size_t(WeaponType::wp_chaingun)]
			&& player->ammo[::std::size_t(AmmoType::am_clip)])
	{
		player->pendingweapon = WeaponType::wp_chaingun;
	}
	else if (player->weaponowned[::std::size_t(WeaponType::wp_shotgun)]
			&& player->ammo[::std::size_t(AmmoType::am_shell)])
	{
		player->pendingweapon = WeaponType::wp_shotgun;
	}
	// allow to remove the pistol via TNTWEAP2
	else if (player->ammo[::std::size_t(AmmoType::am_clip)] && player->weaponowned[::std::size_t(WeaponType::wp_pistol)])
	{
		player->pendingweapon = WeaponType::wp_pistol;
	}
	else if (player->weaponowned[::std::size_t(WeaponType::wp_chainsaw)])
	{
		player->pendingweapon = WeaponType::wp_chainsaw;
	}
	else if (player->weaponowned[::std::size_t(WeaponType::wp_missile)]
			&& player->ammo[::std::size_t(AmmoType::am_misl)])
	{
		player->pendingweapon = WeaponType::wp_missile;
	}
	else if (player->weaponowned[::std::size_t(WeaponType::wp_bfg)]
			&& player->ammo[::std::size_t(AmmoType::am_cell)]>40
			&& (gamemode != GameMode::shareware) )
	{
		player->pendingweapon = WeaponType::wp_bfg;
	}
	else
	{
		// If everything fails.
		player->pendingweapon = WeaponType::wp_fist;
	}

	} while (player->pendingweapon == WeaponType::wp_nochange);

	// Now set appropriate weapon overlay.
	P_SetPsprite(player,
			(int)psprnum_t::ps_weapon,
			weaponinfo[::std::size_t(player->readyweapon)].downstate);

	return false;
}

//
// P_FireWeapon.
//
void P_FireWeapon(Player* player)
{
	statenum_t newstate;

	if (!P_CheckAmmo(player))
	return;

	P_SetMobjState(player, statenum_t::S_PLAY_ATK1);
	newstate = weaponinfo[::std::size_t(player->readyweapon)].atkstate;
	P_SetPsprite(player, (int)psprnum_t::ps_weapon, newstate);
	P_NoiseAlert(player, player);
}

//
// P_DropWeapon
// Player died, so put the weapon away.
//
void P_DropWeapon(Player* player)
{
	P_SetPsprite(player,
			(int)psprnum_t::ps_weapon,
			weaponinfo[::std::size_t(player->readyweapon)].downstate);
}

//
// A_WeaponReady
// The player can fire the weapon
// or change to another weapon at this time.
// Follows after getting weapon up,
// or after previous attack/fire sequence.
//
void A_WeaponReady(MapObject* mobj, Player* player, pspdef_t* psp)
{
	statenum_t newstate;
	int angle;

	if (!player) return; // let pspr action pointers get called from mobj states
	// get out of attack state
	if (player->state == &states[::std::size_t(statenum_t::S_PLAY_ATK1)]
	|| player->state == &states[::std::size_t(statenum_t::S_PLAY_ATK2)] )
	{
	P_SetMobjState(player, statenum_t::S_PLAY);
	}

	if (player->readyweapon == WeaponType::wp_chainsaw && psp->state == &states[::std::size_t(statenum_t::S_SAW)])
	{
	S_StartSound(player->so, (int)sfxenum_t::sfx_sawidl); // weapon sound source
	}

	// check for change
	// if player is dead, put the weapon away
	if (player->pendingweapon != WeaponType::wp_nochange || !player->health)
	{
	// change weapon
	// (pending weapon should allready be validated)
	newstate = weaponinfo[::std::size_t(player->readyweapon)].downstate;
	P_SetPsprite(player, (int)psprnum_t::ps_weapon, newstate);
	return;
	}

	// check for fire
	// the missile launcher and bfg do not auto fire
	if (player->cmd.buttons & buttoncode::BT_ATTACK)
	{
	if ( !player->attackdown
			|| (player->readyweapon != WeaponType::wp_missile
			&& player->readyweapon != WeaponType::wp_bfg) )
	{
		player->attackdown = true;
		P_FireWeapon(player);
		return;
	}
	}
	else
	player->attackdown = false;

	// bob the weapon based on movement speed
	angle = (128*leveltime)&FINEMASK;
	psp->sx = FRACUNIT + FixedMul(player->bob, finecosine[angle]);
	angle &= FINEANGLES/2-1;
	psp->sy = WEAPONTOP + FixedMul(player->bob, finesine[angle]);
}

//
// A_ReFire
// The player can re-fire the weapon
// without lowering it entirely.
//
void A_ReFire(MapObject* mobj, Player* player, pspdef_t* psp)
{
	if (!player) return; // let pspr action pointers get called from mobj states
	// check for fire
	// (if a weaponchange is pending, let it go through instead)
	if ( (player->cmd.buttons & buttoncode::BT_ATTACK)
		&& player->pendingweapon == WeaponType::wp_nochange
		&& player->health)
	{
	player->refire++;
	P_FireWeapon(player);
	}
	else
	{
	player->refire = 0;
	P_CheckAmmo(player);
	}
}

void A_CheckReload(MapObject* mobj, Player* player, pspdef_t* psp)
{
	if (!player) return; // let pspr action pointers get called from mobj states
	P_CheckAmmo(player);
#if 0
	if (player->ammo[am_shell]<2)
	P_SetPsprite(player, psprnum_t::ps_weapon, statenum_t::S_DSNR1);
#endif
}

//
// A_Lower
// Lowers current weapon,
// and changes weapon at bottom.
//
void A_Lower(MapObject* mobj, Player* player, pspdef_t* psp)
{
	if (!player) return; // let pspr action pointers get called from mobj states
	psp->sy += LOWERSPEED;

	// Is already down.
	if (psp->sy < WEAPONBOTTOM)
	return;

	// Player is dead.
	if (player->playerstate == PlayerState::dead)
	{
	psp->sy = WEAPONBOTTOM;

	// don't bring weapon back up
	return;
	}

	// The old weapon has been lowered off the screen,
	// so change the weapon and start raising it
	if (!player->health)
	{
	// Player is dead, so keep the weapon off screen.
	P_SetPsprite(player, (int)psprnum_t::ps_weapon, statenum_t::S_NULL);
	return;
	}

	player->readyweapon = player->pendingweapon;

	P_BringUpWeapon(player);
}

//
// A_Raise
//
void A_Raise(MapObject* mobj, Player* player, pspdef_t* psp)
{
	statenum_t newstate;

	if (!player) return; // let pspr action pointers get called from mobj states
	psp->sy -= RAISESPEED;

	if (psp->sy > WEAPONTOP )
	return;

	psp->sy = WEAPONTOP;

	// The weapon has been raised all the way,
	// so change to the ready state.
	newstate = weaponinfo[::std::size_t(player->readyweapon)].readystate;

	P_SetPsprite(player, (int)psprnum_t::ps_weapon, newstate);
}

//
// A_GunFlash
//
void A_GunFlash(MapObject* mobj, Player* player, pspdef_t* psp)
{
	if (!player) return; // let pspr action pointers get called from mobj states
	P_SetMobjState(player, statenum_t::S_PLAY_ATK2);
	P_SetPsprite(player, (int)psprnum_t::ps_flash, weaponinfo[::std::size_t(player->readyweapon)].flashstate);
}

//
// WEAPON ATTACKS
//

//
// A_Punch
//
void A_Punch(MapObject* mobj, Player* player, pspdef_t* psp)
{
	angle_t angle;
	int damage;
	int slope;

	if (!player) return; // let pspr action pointers get called from mobj states
	damage = (P_Random()%10+1)<<1;

	if (player->powers[::std::size_t(PowerType_t::pw_strength)])
	damage *= 10;

	angle = player->angle;
	angle += P_SubRandom() << 18;
	slope = P_AimLineAttack(player, angle, MELEERANGE);
	P_LineAttack(player, angle, MELEERANGE, slope, damage);

	// turn to face target
	if (linetarget)
	{
	S_StartSound(player->so, (int)sfxenum_t::sfx_punch); // weapon sound source
	player->angle = R_PointToAngle2(player->x,
							player->y,
							linetarget->x,
							linetarget->y);
	}
}

//
// A_Saw
//
void A_Saw(MapObject* mobj, Player* player, pspdef_t* psp)
{
	angle_t angle;
	int damage;
	int slope;

	if (!player) return; // let pspr action pointers get called from mobj states
	damage = 2*(P_Random()%10+1);
	angle = player->angle;
	angle += P_SubRandom() << 18;

	// use meleerange + 1 se the puff doesn't skip the flash
	slope = P_AimLineAttack(player, angle, MELEERANGE+1);
	P_LineAttack(player, angle, MELEERANGE+1, slope, damage);

	A_Recoil(player);

	if (!linetarget)
	{
	S_StartSound(player->so, sfxenum_t::sfx_sawful); // weapon sound source
	return;
	}
	S_StartSound(player->so, sfxenum_t::sfx_sawhit); // weapon sound source

	// turn to face target
	angle = R_PointToAngle2(player->x, player->y, linetarget->x, linetarget->y);
	if (angle - player->angle > ANG180)
	{
	if ((int) (angle - player->angle) < -ANG90/20)
		player->angle = angle + ANG90/21;
	else
		player->angle -= ANG90/20;
	}
	else
	{
	if (angle - player->angle > ANG90/20)
		player->angle = angle - ANG90/21;
	else
		player->angle += ANG90/20;
	}
	player->flags |= mobjflag_e::MF_JUSTATTACKED;
}

// Doom does not check the bounds of the ammo array. As a result,
// it is possible to use an ammo type > 4 that overflows into the
// maxammo array and affects that instead. Through dehacked, for
// example, it is possible to make a weapon that decreases the max
// number of ammo for another weapon. Emulate this.

static void DecreaseAmmo(Player* player, AmmoType ammonum, int amount)
{
	if (ammonum < AmmoType::NUMAMMO)
	{
		player->ammo[::std::size_t(ammonum)] -= amount;
		// never allow less than zero ammo
		if (player->ammo[::std::size_t(ammonum)] < 0)
		{
			player->ammo[::std::size_t(ammonum)] = 0;
		}
	}
	else
	{
		player->maxammo[::std::size_t(ammonum) - ::std::size_t(AmmoType::NUMAMMO)] -= amount;
	}
}

//
// A_FireMissile
//
void A_FireMissile(MapObject* mobj, Player* player, pspdef_t* psp)
{
	if (!player) return; // let pspr action pointers get called from mobj states
	DecreaseAmmo(player, weaponinfo[::std::size_t(player->readyweapon)].ammo, 1);
	P_SpawnPlayerMissile(player, mobjtype_t::MT_ROCKET);
}

//
// A_FireBFG
//
void A_FireBFG(MapObject* mobj, Player* player, pspdef_t* psp)
{
	if (!player) return; // let pspr action pointers get called from mobj states
	DecreaseAmmo(player, weaponinfo[::std::size_t(player->readyweapon)].ammo, deh_bfg_cells_per_shot);
	P_SpawnPlayerMissile(player, mobjtype_t::MT_BFG);
}

//
// A_FirePlasma
//
void A_FirePlasma(MapObject* mobj, Player* player, pspdef_t* psp)
{
	if (!player) return; // let pspr action pointers get called from mobj states
	DecreaseAmmo(player, weaponinfo[::std::size_t(player->readyweapon)].ammo, 1);

	P_SetPsprite(player,
			(int)psprnum_t::ps_flash,
			weaponinfo[::std::size_t(player->readyweapon)].flashstate+(P_Random()&1) );

	P_SpawnPlayerMissile(player, mobjtype_t::MT_PLASMA);
}

//
// P_BulletSlope
// Sets a slope so a near miss is at aproximately
// the height of the intended target
//
fixed_t bulletslope;

void P_BulletSlope(MapObject* mo)
{
	angle_t an;

	if (critical->freeaim == FREEAIM_DIRECT)
	{
	bulletslope = PLAYER_SLOPE(mo->player);
	}
	else
	{
	// see which target is to be aimed at
	an = mo->angle;
	bulletslope = P_AimLineAttack(mo, an, 16*64*FRACUNIT);

	if (!linetarget)
	{
	an += 1<<26;
	bulletslope = P_AimLineAttack(mo, an, 16*64*FRACUNIT);
	if (!linetarget)
	{
		an -= 2<<26;
		bulletslope = P_AimLineAttack(mo, an, 16*64*FRACUNIT);
		if (!linetarget && critical->freeaim == FREEAIM_BOTH)
		{
		bulletslope = PLAYER_SLOPE(mo->player);
		}
	}
	}
	}
}

//
// P_GunShot
//
void P_GunShot(MapObject* mo, bool accurate)
{
	angle_t angle;
	int damage;

	damage = 5*(P_Random()%3+1);
	angle = mo->angle;

	if (!accurate)
	angle += P_SubRandom() << 18;

	P_LineAttack(mo, angle, MISSILERANGE, bulletslope, damage);
}

//
// A_FirePistol
//
void A_FirePistol(MapObject* mobj, Player* player, pspdef_t* psp)
{
	if (!player) return; // let pspr action pointers get called from mobj states
	S_StartSound(player->so, (int)sfxenum_t::sfx_pistol); // weapon sound source

	P_SetMobjState(player, statenum_t::S_PLAY_ATK2);
	DecreaseAmmo(player, weaponinfo[::std::size_t(player->readyweapon)].ammo, 1);

	P_SetPsprite(player,
			(int)psprnum_t::ps_flash,
			weaponinfo[::std::size_t(player->readyweapon)].flashstate);

	P_BulletSlope(player);
	P_GunShot(player, !player->refire);

	A_Recoil(player);
}

//
// A_FireShotgun
//
void A_FireShotgun(MapObject* mobj, Player* player, pspdef_t* psp)
{
	int i;

	if (!player) return; // let pspr action pointers get called from mobj states
	S_StartSound(player->so, (int)sfxenum_t::sfx_shotgn); // weapon sound source
	P_SetMobjState(player, statenum_t::S_PLAY_ATK2);

	DecreaseAmmo(player, weaponinfo[::std::size_t(player->readyweapon)].ammo, 1);

	P_SetPsprite(player,
			(int)psprnum_t::ps_flash,
			weaponinfo[::std::size_t(player->readyweapon)].flashstate);

	P_BulletSlope(player);

	for (i=0 ; i<7 ; ++i)
	P_GunShot(player, false);

	A_Recoil(player);
}

//
// A_FireShotgun2
//
void A_FireShotgun2(MapObject* mobj, Player* player, pspdef_t* psp)
{
	int i;
	angle_t angle;
	int damage;

	if (!player) return; // let pspr action pointers get called from mobj states
	S_StartSound(player->so, (int)sfxenum_t::sfx_dshtgn); // weapon sound source
	P_SetMobjState(player, statenum_t::S_PLAY_ATK2);

	DecreaseAmmo(player, weaponinfo[::std::size_t(player->readyweapon)].ammo, 2);

	P_SetPsprite(player,
			(int)psprnum_t::ps_flash,
			weaponinfo[::std::size_t(player->readyweapon)].flashstate);

	P_BulletSlope(player);

	for (i=0 ; i<20 ; ++i)
	{
	damage = 5*(P_Random()%3+1);
	angle = player->angle;
	angle += P_SubRandom() << ANGLETOFINESHIFT;
	P_LineAttack(player,
				angle,
				MISSILERANGE,
				bulletslope + (P_SubRandom() << 5), damage);
	}

	A_Recoil(player);
}

//
// A_FireCGun
//
void A_FireCGun(MapObject* mobj, Player* player, pspdef_t* psp)
{
	if (!player) return; // let pspr action pointers get called from mobj states
	S_StartSound(player->so, (int)sfxenum_t::sfx_pistol); // weapon sound source

	if (!player->ammo[::std::size_t(weaponinfo[::std::size_t(player->readyweapon)].ammo)])
	return;

	P_SetMobjState(player, statenum_t::S_PLAY_ATK2);
	DecreaseAmmo(player, weaponinfo[::std::size_t(player->readyweapon)].ammo, 1);

	P_SetPsprite(player,
			(int)psprnum_t::ps_flash,
			weaponinfo[::std::size_t(player->readyweapon)].flashstate
			+ psp->state
			- &states[::std::size_t(statenum_t::S_CHAIN1)] );

	P_BulletSlope(player);

	P_GunShot(player, !player->refire);

	A_Recoil(player);
}

//
// ?
//
void A_Light0(MapObject* mobj, Player* player, pspdef_t* psp)
{
	if (!player) return; // let pspr action pointers get called from mobj states
	player->extralight = 0;
}

void A_Light1(MapObject* mobj, Player* player, pspdef_t* psp)
{
	if (!player) return; // let pspr action pointers get called from mobj states
	player->extralight = 1;
}

void A_Light2(MapObject* mobj, Player* player, pspdef_t* psp)
{
	if (!player) return; // let pspr action pointers get called from mobj states
	player->extralight = 2;
}

//
// A_BFGSpray
// Spawn a BFG explosion on every monster in view
//
void A_BFGSpray(MapObject* mo)
{
	int i;
	int j;
	int damage;
	angle_t an;

	// offset angles from its attack angle
	for (i=0 ; i<40 ; ++i)
	{
	an = mo->angle - ANG90/2 + ANG90/40*i;

	// mo->target is the originator (player)
	// of the missile
	P_AimLineAttack(mo->target, an, 16*64*FRACUNIT);

	if (!linetarget)
		continue;

	P_SpawnMobj(linetarget->x,
				linetarget->y,
				linetarget->z + (linetarget->height>>2),
				mobjtype_t::MT_EXTRABFG);

	damage = 0;
	for (j=0; j < 15; ++j)
		damage += (P_Random()&7) + 1;

	P_DamageMobj(linetarget, mo->target,mo->target, damage);
	}
}

void A_BFGsound(MapObject* mobj, Player* player, pspdef_t* psp)
{
	if (!player) return; // let pspr action pointers get called from mobj states
	S_StartSound(player, (int)sfxenum_t::sfx_bfg); // intentionally not weapon sound source
}

// Called at start of level for each player.
void P_SetupPsprites(Player* player)
{
	int i;

	// remove all psprites
	for (i=0; i < ::std::size_t(psprnum_t::NUMPSPRITES); ++i)
	player->psprites[i].state = NULL;

	// spawn the gun
	player->pendingweapon = player->readyweapon;
	P_BringUpWeapon(player);
}

//
// P_MovePsprites
// Called every tic by player thinking routine.
//
void P_MovePsprites(Player* player)
{
	int i;
	pspdef_t* psp;

	psp = &player->psprites[0];
	for (i=0; i < ::std::size_t(psprnum_t::NUMPSPRITES); ++i, ++psp)
	{
	// a null state means not active
	if (psp->state)
	{
		// drop tic count and possibly change state

		// a -1 tic count never changes
		if (psp->tics != -1)
		{
		psp->tics--;
		if (!psp->tics)
			P_SetPsprite(player, i, psp->state->nextstate);
		}
	}
	}

	player->psprites[::std::size_t(psprnum_t::ps_flash)].sx = player->psprites[::std::size_t(psprnum_t::ps_weapon)].sx;
	player->psprites[::std::size_t(psprnum_t::ps_flash)].sy = player->psprites[::std::size_t(psprnum_t::ps_weapon)].sy;

	// apply bobbing (or centering) to the player's weapon sprite
	psp = &player->psprites[0];
	psp->sx2 = psp->sx;
	psp->sy2 = psp->sy;
	if (psp->state)
	{
	// don't center vertically during lowering and raising states
	if (psp->state->misc1 ||
		psp->state->action.acp3 == (actionf_p3)A_Lower ||
		psp->state->action.acp3 == (actionf_p3)A_Raise)
	{
	}
	else
	// not attacking means idle
	if (!player->attackdown ||
		crispy->centerweapon == CENTERWEAPON_BOB)
	{
		angle_t angle = (128 * leveltime) & FINEMASK;
		psp->sx2 = FRACUNIT + FixedMul(player->bob2, finecosine[angle]);
		angle &= FINEANGLES / 2 - 1;
		psp->sy2 = WEAPONTOP + FixedMul(player->bob2, finesine[angle]);
	}
	else
	// center the weapon sprite horizontally and push up vertically
	if (crispy->centerweapon == CENTERWEAPON_CENTER)
	{
		psp->sx2 = FRACUNIT;
		psp->sy2 = WEAPONTOP;
	}
	}

	// squat down weapon sprite a bit after hitting the ground
	if (player->psp_dy_max)
	{
		psp->dy -= FRACUNIT;

		if (psp->dy < player->psp_dy_max)
		{
			psp->dy = -psp->dy;
		}

		if (psp->dy == 0)
		{
			player->psp_dy_max = 0;
		}
	}

	player->psprites[::std::size_t(psprnum_t::ps_flash)].dy = psp->dy;
	player->psprites[::std::size_t(psprnum_t::ps_flash)].sx2 = psp->sx2;
	player->psprites[::std::size_t(psprnum_t::ps_flash)].sy2 = psp->sy2;
}
