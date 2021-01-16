/**********************************************************************************************************************************************\
	Copyright(C) 1993-1996 Id Software, Inc.
	Copyright(C) 2005-2014 Simon Howard

	This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License
	as published by the Free Software Foundation; either version 2 of the License, or (at your option) any later version.

	This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

	DESCRIPTION:
		Player related stuff. Bobbing POV/weapon, movement. Pending weapon.
\**********************************************************************************************************************************************/
#include "../derma/stdafx.h"

#include "doomdef.h"
#include "d_event.h"

#include "p_local.h"

#include "doomstat.h"

// Index of the special effects (INVUL inverse) map.
constexpr size_t INVERSECOLORMAP{32};

//
// Movement.
//

// 16 pixels of bob
constexpr size_t MAXBOB{0x100000};

// variable player view bob
static const fixed_t crispy_bobfactor[3] = {4, 3, 0};

bool onground;

// Moves the given origin along a given angle.
void P_Thrust(Player* player, angle_t angle, fixed_t move)
{
	angle >>= ANGLETOFINESHIFT;

	player->momx += FixedMul(move, finecosine[angle]);
	player->momy += FixedMul(move, finesine[angle]);
}

// Calculate the walking / running height adjustment
void P_CalcHeight(Player* player)
{
	int angle;
	fixed_t bob;

	// Regular movement bobbing (needs to be calculated for gun swing even if not on ground)
	// OPTIMIZE: tablify angle Note: a LUT allows for effects like a ramp with low health.
	player->bob = FixedMul(player->momx, player->momx) + FixedMul(player->momy, player->momy);

	player->bob >>= 2;

	if (player->bob > MAXBOB)
	{
		player->bob = MAXBOB;
	}

	// variable player view bob
	player->bob2 = crispy_bobfactor[crispy->bobfactor] * player->bob / 4;

	if ((player->cheats & CheatType::CF_NOMOMENTUM) || !onground)
	{
		player->viewz = player->z + VIEWHEIGHT;

		if (player->viewz > player->ceilingz - 4 * FRACUNIT)
		{
			player->viewz = player->ceilingz - 4 * FRACUNIT;
		}

		// fix player viewheight in NOMOMENTUM mode
		//player->viewz = player->z + player->viewheight;
		return;
	}

	angle = (FINEANGLES / 20 * leveltime) & FINEMASK;
	bob = FixedMul(player->bob2 / 2, finesine[angle]); // variable player view bob

	// move viewheight
	if (player->playerstate == PlayerState::live)
	{
		player->viewheight += player->deltaviewheight;

		if (player->viewheight > VIEWHEIGHT)
		{
			player->viewheight = VIEWHEIGHT;
			player->deltaviewheight = 0;
		}

		if (player->viewheight < VIEWHEIGHT / 2)
		{
			player->viewheight = VIEWHEIGHT / 2;
			if (player->deltaviewheight <= 0)
			{
				player->deltaviewheight = 1;
			}
		}

		if (player->deltaviewheight)
		{
			player->deltaviewheight += FRACUNIT / 4;
			if (!player->deltaviewheight)
			{
				player->deltaviewheight = 1;
			}
		}
	}
	player->viewz = player->z + player->viewheight + bob;

	if (player->viewz > player->ceilingz - 4 * FRACUNIT)
	{
		player->viewz = player->ceilingz - 4 * FRACUNIT;
	}
}

void P_MovePlayer(Player* player)
{
	ticcmd_t* cmd;
	int look;

	cmd = &player->cmd;

	player->angle += (cmd->angleturn << FRACBITS);

	// Do not let the player control movement if not onground.
	onground = (player->z <= player->floorz);
	// give full control in no-clipping mode
	onground |= (player->flags & mobjflag_e::MF_NOCLIP);

	if (cmd->forwardmove && onground)
	{
		P_Thrust(player, player->angle, cmd->forwardmove * 2048);
	}
	else if (cmd->forwardmove && critical->jump)
	{
		// in-air movement is only possible with jumping enabled
		P_Thrust(player, player->angle, FRACUNIT >> 8);
	}

	if (cmd->sidemove && onground)
	{
		P_Thrust(player, player->angle - ANG90, cmd->sidemove * 2048);
	}
	else if (cmd->sidemove && critical->jump)
	{
		// in-air movement is only possible with jumping enabled
		P_Thrust(player, player->angle, FRACUNIT >> 8);
	}

	if ((cmd->forwardmove || cmd->sidemove) && player->state == &states[::std::size_t(statenum_t::S_PLAY)])
	{
		P_SetMobjState(player, statenum_t::S_PLAY_RUN1);
	}

	// apply lookdir delta
	look = cmd->lookfly & 15;
	if (look > 7)
	{
		look -= 16;
	}
	if (look)
	{
		if (look == TOCENTER)
		{
			player->centering = true;
		}
		else
		{
			cmd->lookdir = MLOOKUNIT * 5 * look;
		}
	}
	if (!menuactive && !demoplayback)
	{
		player->lookdir = BETWEEN(-LOOKDIRMIN * MLOOKUNIT, LOOKDIRMAX * MLOOKUNIT, player->lookdir + cmd->lookdir);
	}
}

// Fall on your face when dying. Decrease POV height to floor height.
constexpr size_t ANG5{(ANG90 / 18)};

void P_DeathThink(Player* player)
{
	angle_t angle;
	angle_t delta;

	P_MovePsprites(player);

	// fall to the ground
	if (player->viewheight > 6 * FRACUNIT)
	{
		player->viewheight -= FRACUNIT;
	}

	if (player->viewheight < 6 * FRACUNIT)
	{
		player->viewheight = 6 * FRACUNIT;
	}

	player->deltaviewheight = 0;
	onground = (player->z <= player->floorz);
	P_CalcHeight(player);

	if (player->attacker && player->attacker != player)
	{
		angle = R_PointToAngle2(player->x, player->y, player->attacker->x, player->attacker->y);

		delta = angle - player->angle;

		if (delta < ANG5 || delta >(unsigned) - ANG5)
		{
			// Looking at killer, so fade damage flash down.
			player->angle = angle;

			if (player->damagecount)
			{
				player->damagecount--;
			}
		}
		else if (delta < ANG180)
		{
			player->angle += ANG5;
		}
		else
		{
			player->angle -= ANG5;
		}
	}
	else if (player->damagecount)
	{
		player->damagecount--;
	}

	if (player->cmd.buttons & buttoncode::BT_USE)
	{
		player->playerstate = PlayerState::reborn;
	}
}

void P_PlayerThink(Player* player)
{
	ticcmd_t* cmd;
	WeaponType newweapon;

	// Assume we can interpolate at the beginning of the tic.
	player->interp = true;

	// Store starting position for player interpolation.
	player->oldx = player->x;
	player->oldy = player->y;
	player->oldz = player->z;
	player->oldangle = player->angle;
	player->oldviewz = player->viewz;
	player->oldlookdir = player->lookdir;
	player->oldrecoilpitch = player->recoilpitch;

	// update weapon sound source coordinates
	if (player->so != player)
	{
		memcpy(player->so, player, sizeof(degenmobj_t));
	}

	// fixme: do this in the cheat code
	if (player->cheats & CheatType::CF_NOCLIP)
	{
		//player->flags |= mobjflag_e::MF_NOCLIP;
		player->flags.set(mobjflag_e::MF_NOCLIP, true);
	}
	else
	{
		//player->flags &= ~mobjflag_e::MF_NOCLIP;
		player->flags.set(mobjflag_e::MF_NOCLIP, false);
	}

	// chain saw run forward
	cmd = &player->cmd;
	//if (player->flags & mobjflag_e::MF_JUSTATTACKED)
	if (player->flags.test(mobjflag_e::MF_JUSTATTACKED))
	{
		cmd->angleturn = 0;
		cmd->forwardmove = 0xc800 / 512;
		cmd->sidemove = 0;
		//player->flags &= ~mobjflag_e::MF_JUSTATTACKED;
		player->flags.set(mobjflag_e::MF_JUSTATTACKED, false);
	}

	// center view
	// e.g. after teleporting, dying, jumping and on demand
	if (player->centering)
	{
		if (player->lookdir > 0)
		{
			player->lookdir -= 8 * MLOOKUNIT;
		}
		else if (player->lookdir < 0)
		{
			player->lookdir += 8 * MLOOKUNIT;
		}

		if (abs(player->lookdir) < 8 * MLOOKUNIT)
		{
			player->lookdir = 0;
			player->centering = false;
		}
	}

	// weapon recoil pitch
	if (player->recoilpitch)
	{
		if (player->recoilpitch > 0)
		{
			player->recoilpitch -= 1;
		}
		else if (player->recoilpitch < 0)
		{
			player->recoilpitch += 1;
		}
	}

	if (player->playerstate == PlayerState::dead)
	{
		P_DeathThink(player);
		return;
	}

	// negative player health
	player->neghealth = player->health;

	// delay next possible jump
	if (player->jumpTics)
	{
		player->jumpTics--;
	}

	// Move around. Reactiontime is used to prevent movement for a bit after a teleport.
	if (player->reactiontime)
	{
		player->reactiontime--;
	}
	else
	{
		P_MovePlayer(player);
	}

	P_CalcHeight(player);

	if (player->subsector->sector->special)
		P_PlayerInSpecialSector(player);

	// jumping: apply vertical momentum
	if (cmd->arti)
	{
		if ((cmd->arti & AFLAG_JUMP) && onground && !player->jumpTics)
		{
			// Hexen sets 9; Strife adds 8
			player->momz = (7 + crispy->jump) * FRACUNIT;
			player->jumpTics = 18;
			// squat down weapon sprite a bit
			if (crispy->weaponsquat)
			{
				player->psp_dy_max = -player->momz >> 2;
			}
		}
	}

	// Check for weapon change.

	// A special event has no other buttons.
	if (cmd->buttons & buttoncode::BT_SPECIAL)
	{
		cmd->buttons = 0;
	}

	if (cmd->buttons & buttoncode::BT_CHANGE)
	{
		// The actual changing of the weapon is done when the weapon psprite can do it (read: not in the middle of an attack).
		newweapon = (cmd->buttons & buttoncode::BT_WEAPONMASK) >> buttoncode::BT_WEAPONSHIFT;

		if (newweapon == WeaponType::wp_fist && player->weaponowned[::std::size_t(WeaponType::wp_chainsaw)]
			&& !(player->readyweapon == WeaponType::wp_chainsaw && player->powers[::std::size_t(PowerType_t::pw_strength)]))
		{
			newweapon = WeaponType::wp_chainsaw;
		}

		if ((crispy->havessg) && newweapon == WeaponType::wp_shotgun
			&& player->weaponowned[::std::size_t(WeaponType::wp_supershotgun)] && player->readyweapon != WeaponType::wp_supershotgun)
		{
			newweapon = WeaponType::wp_supershotgun;
		}

		if (player->weaponowned[::std::size_t(newweapon)] && newweapon != player->readyweapon)
		{
			// Do not go to plasma or BFG in shareware, even if cheated.
			if ((newweapon != WeaponType::wp_plasma && newweapon != WeaponType::wp_bfg) || (gamemode != GameMode::shareware))
			{
				player->pendingweapon = newweapon;
			}
		}
	}

	// check for use
	if (cmd->buttons & buttoncode::BT_USE)
	{
		if (!player->usedown)
		{
			P_UseLines(player);
			player->usedown = true;
			// "use" button timer
			if (crispy->btusetimer)
			{
				player->btuse = leveltime;
				player->btuse_tics = 5 * TICRATE / 2; // 2.5 seconds
			}
		}
	}
	else
	{
		player->usedown = false;
	}

	// cycle psprites
	P_MovePsprites(player);

	// Counters, time dependend power ups.

	// Strength counts up to diminish fade.
	if (player->powers[::std::size_t(PowerType_t::pw_strength)])
	{
		player->powers[::std::size_t(PowerType_t::pw_strength)]++;
	}

	if (player->powers[::std::size_t(PowerType_t::pw_invulnerability)])
	{
		player->powers[::std::size_t(PowerType_t::pw_invulnerability)]--;
	}

	if (player->powers[::std::size_t(PowerType_t::pw_invisibility)])
	{
		if (!--player->powers[::std::size_t(PowerType_t::pw_invisibility)])
		{
			//player->flags &= ~mobjflag_e::MF_SHADOW;
			player->flags.set(mobjflag_e::MF_SHADOW, false)
		}
	}

	if (player->powers[::std::size_t(PowerType_t::pw_infrared)])
	{
		player->powers[::std::size_t(PowerType_t::pw_infrared)]--;
	}

	if (player->powers[::std::size_t(PowerType_t::pw_ironfeet)])
	{
		player->powers[::std::size_t(PowerType_t::pw_ironfeet)]--;
	}

	if (player->damagecount)
	{
		player->damagecount--;
	}

	if (player->bonuscount)
	{
		player->bonuscount--;
	}

	// Handling colormaps.
	if (player->powers[::std::size_t(PowerType_t::pw_invulnerability)])
	{
		if (player->powers[::std::size_t(PowerType_t::pw_invulnerability)] > 4 * 32 || (player->powers[::std::size_t(PowerType_t::pw_invulnerability)] & 8))
		{
			player->fixedcolormap = INVERSECOLORMAP;
		}
		else
		{
			// Visor effect when Invulnerability is fading out
			player->fixedcolormap = player->powers[::std::size_t(PowerType_t::pw_infrared)] ? 1 : 0;
		}
	}
	else if (player->powers[::std::size_t(PowerType_t::pw_infrared)])
	{
		if (player->powers[::std::size_t(PowerType_t::pw_infrared)] > 4 * 32 || (player->powers[::std::size_t(PowerType_t::pw_infrared)] & 8))
		{
			// almost full bright
			player->fixedcolormap = 1;
		}
		else
			player->fixedcolormap = 0;
	}
}
	else
	{
	player->fixedcolormap = 0;
	}
}
