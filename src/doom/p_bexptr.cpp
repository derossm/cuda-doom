/**********************************************************************************************************************************************\
	Copyright(C) 1993-1996 Id Software, Inc.
	Copyright(C) 1999 id Software, Chi Hoang, Lee Killough, Jim Flynn, Rand Phares, Ty Halderman
	Copyright(C) 2005-2014 Simon Howard
	Copyright(C) 2017 Fabian Greffrath

	This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License
	as published by the Free Software Foundation; either version 2 of the License, or (at your option) any later version.

	This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

	DESCRIPTION:
		additional BOOM and MBF code pointers
\**********************************************************************************************************************************************/

#include "p_local.h"
#include "m_random.h"
#include "s_sound.h"

extern void A_Explode();
extern void A_FaceTarget();

extern bool P_CheckMeleeRange(MapObject* actor);
extern void P_Thrust(Player* player, angle_t angle, fixed_t move);

// killough 11/98: kill an object
void A_Die(MapObject* actor)
{
	P_DamageMobj(actor, NULL, NULL, actor->health);
}

// killough 8/9/98: same as A_Explode, except that the damage is variable
void A_Detonate(MapObject* mo)
{
	P_RadiusAttack(mo, mo->target, mo->info->damage);
}

// killough 9/98: a mushroom explosion effect, sorta :)
// Original idea: Linguica
void A_Mushroom(MapObject* actor)
{
	int n = actor->info->damage;

	// Mushroom parameters are part of code pointer's state
	fixed_t misc1 = actor->state->misc1 ? actor->state->misc1 : FRACUNIT * 4;
	fixed_t misc2 = actor->state->misc2 ? actor->state->misc2 : FRACUNIT / 2;

	A_Explode(actor);				// make normal explosion

	for (size_t i = -n; i <= n; i += 8)	// launch mushroom cloud
		for (size_t j = -n; j <= n; j += 8)
		{
			MapObject target = *actor, * mo;
			target.x += i << FRACBITS;	// Aim in many directions from source
			target.y += j << FRACBITS;
			target.z += P_AproxDistance(i, j) * misc1;			// Aim fairly high
			mo = P_SpawnMissile(actor, &target, mobjtype_t::MT_FATSHOT);	// Launch fireball
			mo->momx = FixedMul(mo->momx, misc2);
			mo->momy = FixedMul(mo->momy, misc2);				// Slow down a bit
			mo->momz = FixedMul(mo->momz, misc2);
			//mo->flags &= ~mobjflag_e::MF_NOGRAVITY;	// Make debris fall under gravity
			mo->flags.set(mobjflag_e::MF_NOGRAVITY, false);
		}
}

// killough 10/98: this emulates the beta version's lost soul attacks
void A_BetaSkullAttack(MapObject* actor)
{
	if (!actor->target || actor->target->type == mobjtype_t::MT_SKULL)
	{
		return;
	}
	S_StartSound(actor, actor->info->attacksound);
	A_FaceTarget(actor);
	int damage = (P_Random() % 8 + 1) * actor->info->damage;
	P_DamageMobj(actor->target, actor, actor, damage);
}

void A_Stop(MapObject* actor)
{
	actor->momx = actor->momy = actor->momz = 0;
}

// The following were inspired by Len Pitre
// A small set of highly-sought-after code pointers
void A_Spawn(MapObject* mo)
{
	if (mo->state->misc1)
	{
		//MapObject* newmobj =
		P_SpawnMobj(mo->x, mo->y, (mo->state->misc2 << FRACBITS) + mo->z, mo->state->misc1 - 1);
		//newmobj->flags = (newmobj->flags & ~mobjflag_e::MF_FRIEND) | (mo->flags & mobjflag_e::MF_FRIEND);
	}
}

void A_Turn(MapObject* mo)
{
	mo->angle += (angle_t)(((uint64_t)mo->state->misc1 << 32) / 360);
}

void A_Face(MapObject* mo)
{
	mo->angle = (angle_t)(((uint64_t)mo->state->misc1 << 32) / 360);
}

void A_Scratch(MapObject* mo)
{
	A_FaceTarget(mo);
	if (mo->target && P_CheckMeleeRange(mo))
	{
		if (mo->state->misc2)
		{
			S_StartSound(mo, mo->state->misc2);
		}
		else
		{
			// ()0;
			P_DamageMobj(mo->target, mo, mo, mo->state->misc1);
		}
	}
	else
	{
		// ()0;
		// this was probably (void)0, but I replaced all (void) with () when getting rid of function(void) declarations
		// Did this programmer think comma operators are required by ?: conditionals? i.e. "(E1 , E2) ? T : F1, F2"
		// so they had to come up with something to skip the "first" argument in the comma pairs?
	}

	// See https://en.cppreference.com/w/cpp/language/operator_other for more on the comma operator, which should never be used this way.

	// The following monstrosity is translated above; left as an example of what never to do with terneries and comma operators
	//mo->target && (A_FaceTarget(mo), P_CheckMeleeRange(mo)) ?
		//mo->state->misc2 ? S_StartSound(mo, mo->state->misc2) : () 0,
		//P_DamageMobj(mo->target, mo, mo, mo->state->misc1) : () 0;
}

void A_PlaySound(MapObject* mo)
{
	S_StartSound(mo->state->misc2 ? NULL : mo, mo->state->misc1);
}

// [crispy] this is pretty much the only action pointer that makes sense for both mobj and pspr states
void A_RandomJump(MapObject* mo, Player* player, pspdef_t* psp)
{
	// [crispy] first, try to apply to pspr states
	if (player && psp)
	{
		if (Crispy_Random() < psp->state->misc2)
		{
			extern void P_SetPsprite(Player * player, int position, statenum_t stnum);

			P_SetPsprite(player, psp - &player->psprites[0], psp->state->misc1);
		}
	}
	else
		// [crispy] second, apply to mobj states
		if (mo)
		{
			if (Crispy_Random() < mo->state->misc2)
			{
				P_SetMobjState(mo, mo->state->misc1);
			}
		}
}

//
// This allows linedef effects to be activated inside deh frames.
//

void A_LineEffect(MapObject* mo)
{
	//if (!(mo->intflags & MIF_LINEDONE))				// Unless already used up
	{
		line_t junk = *lines;							// Fake linedef set to 1st
		if ((junk.special = (short)mo->state->misc1))	// Linedef type
		{
			Player player;
			Player* oldplayer = mo->player;				// Remember player status
			mo->player = &player;						// Fake player
			player.health = 100;						// Alive player
			junk.tag = (short)mo->state->misc2;			// Sector tag for linedef
			if (!P_UseSpecialLine(mo, &junk, 0))		// Try using it
			{
				P_CrossSpecialLinePtr(&junk, 0, mo);	// Try crossing it
			}
			//if (!junk.special)						// If type cleared,
				//mo->intflags |= MIF_LINEDONE;			// no more for this thing
			mo->player = oldplayer;						// Restore player status

			// WEIRD: Why create a fake player just to give it health and then discard?
		}
	}
}

//
// A_FireOldBFG
//
// This function emulates Doom's Pre-Beta BFG
// By Lee Killough 6/6/98, 7/11/98, 7/19/98, 8/20/98
//
// This code may not be used in other mods without appropriate credit given.
// Code leeches will be telefragged.
void A_FireOldBFG(MapObject* mobj, Player* player, pspdef_t* psp)
{
	int type = mobjtype_t::MT_PLASMA1;
	extern void P_CheckMissileSpawn(MapObject * th);

	if (!player) return; // [crispy] let pspr action pointers get called from mobj states

	if (crispy->recoil && !(player->flags & mobjflag_e::MF_NOCLIP))
		P_Thrust(player, ANG180 + player->angle,
			512 * 20);//recoil_values[wp_plasma][0]);

	player->ammo[std::size_t(weaponinfo[std::size_t(player->readyweapon)].ammo)]--;

	player->extralight = 2;

	do
	{
		MapObject* th, * mo = player;
		angle_t an = mo->angle;
		angle_t an1 = ((P_Random(/* pr_bfg */) & 127) - 64) * (ANG90 / 768) + an;
		angle_t an2 = ((P_Random(/* pr_bfg */) & 127) - 64) * (ANG90 / 640) + ANG90;
		//extern int autoaim;

		//if (autoaim || !beta_emulation)
		{
			// killough 8/2/98: make autoaiming prefer enemies
			int mask = 0; //mobjflag_e::MF_FRIEND;
			fixed_t slope;
			if (critical->freeaim == FREEAIM_DIRECT)
			{
				slope = PLAYER_SLOPE(player);
			}
			else
			{
				do
				{
					slope = P_AimLineAttack(mo, an, 16 * 64 * FRACUNIT); //, mask);
					if (!linetarget)
					{
						slope = P_AimLineAttack(mo, an += 1 << 26, 16 * 64 * FRACUNIT); //, mask);
					}
					if (!linetarget)
					{
						slope = P_AimLineAttack(mo, an -= 2 << 26, 16 * 64 * FRACUNIT); //, mask);
					}
					if (!linetarget)
					{
						slope = (critical->freeaim == FREEAIM_BOTH) ? PLAYER_SLOPE(player) : 0, an = mo->angle;
					}
				} while (mask && (mask = 0, !linetarget));		// killough 8/2/98
			}
			an1 += an - mo->angle;
			// consider negative slope
			if (slope < 0)
			{
				an2 -= tantoangle[-slope >> DBITS];
			}
			else
			{
				an2 += tantoangle[slope >> DBITS];
			}
		}

		th = P_SpawnMobj(mo->x, mo->y, mo->z + 62 * FRACUNIT - player->psprites[std::size_t(psprnum_t::ps_weapon)].sy, type);
		th->target = mo; // P_SetTarget(&th->target, mo);
		th->angle = an1;
		th->momx = finecosine[an1 >> ANGLETOFINESHIFT] * 25;
		th->momy = finesine[an1 >> ANGLETOFINESHIFT] * 25;
		th->momz = finetangent[an2 >> ANGLETOFINESHIFT] * 25;
		// suppress interpolation of player missiles for the first tic
		th->interp = -1;
		P_CheckMissileSpawn(th);
	} while ((type != mobjtype_t::MT_PLASMA2) && (type = mobjtype_t::MT_PLASMA2)); //killough: obfuscated!
}
