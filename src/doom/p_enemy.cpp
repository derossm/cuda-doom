/**********************************************************************************************************************************************\
	Copyright(C) 1993-1996 Id Software, Inc.
	Copyright(C) 2005-2014 Simon Howard

	This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License
	as published by the Free Software Foundation; either version 2 of the License, or (at your option) any later version.

	This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

	DESCRIPTION:
		Enemy thinking, AI. Action Pointer Functions that are associated with states/frames.
\**********************************************************************************************************************************************/
#include "../../derma/stdafx.h"

#include "m_random.h"
#include "../i_system.h"

#include "doomdef.h"
#include "p_local.h"

#include "s_sound.h"

#include "g_game.h"

// State.
#include "doomstat.h"
#include "r_state.h"

// Data.
#include "sounds.h"

enum class dirtype_t
{
	DI_EAST,
	DI_NORTHEAST,
	DI_NORTH,
	DI_NORTHWEST,
	DI_WEST,
	DI_SOUTHWEST,
	DI_SOUTH,
	DI_SOUTHEAST,
	DI_NODIR,
	NUMDIRS
};

//
// P_NewChaseDir related LUT.
//
dirtype_t opposite[] =
{
	dirtype_t::DI_WEST, dirtype_t::DI_SOUTHWEST, dirtype_t::DI_SOUTH, dirtype_t::DI_SOUTHEAST,
	dirtype_t::DI_EAST, dirtype_t::DI_NORTHEAST, dirtype_t::DI_NORTH, dirtype_t::DI_NORTHWEST, dirtype_t::DI_NODIR
};

dirtype_t diags[] =
{
	dirtype_t::DI_NORTHWEST, dirtype_t::DI_NORTHEAST, dirtype_t::DI_SOUTHWEST, dirtype_t::DI_SOUTHEAST
};

void A_Fall(MapObject* actor);

//
// ENEMY THINKING
// Enemies are allways spawned
// with targetplayer = -1, threshold = 0
// Most monsters are spawned unaware of all players,
// but some can be made preaware
//

//
// Called by P_NoiseAlert.
// Recursively traverse adjacent sectors,
// sound blocking lines cut off traversal.
//
MapObject* soundtarget;

void P_RecursiveSound(sector_t* sec, int soundblocks)
{
	int i;
	line_t* check;
	sector_t* other;

	// wake up all monsters in this sector
	if (sec->validcount == validcount && sec->soundtraversed <= soundblocks + 1)
	{
		return;		// already flooded
	}

	sec->validcount = validcount;
	sec->soundtraversed = soundblocks + 1;
	sec->soundtarget = soundtarget;

	for (i = 0; i < sec->linecount; ++i)
	{
		check = sec->lines[i];
		if (!(check->flags & ML_TWOSIDED))
		{
			continue;
		}

		P_LineOpening(check);

		if (openrange <= 0)
		{
			continue; // closed door
		}

		if (sides[check->sidenum[0]].sector == sec)
		{
			other = sides[check->sidenum[1]].sector;
		}
		else
		{
			other = sides[check->sidenum[0]].sector;
		}

		if (check->flags & ML_SOUNDBLOCK)
		{
			if (!soundblocks)
			{
				P_RecursiveSound(other, 1);
			}
		}
		else
		{
			P_RecursiveSound(other, soundblocks);
		}
	}
}

// If a monster yells at a player, it will alert other monsters to the player.
void P_NoiseAlert(MapObject* target, MapObject* emmiter)
{
	// monsters are deaf with NOTARGET cheat
	if (target && target->player && (target->player->cheats & CheatType::CF_NOTARGET))
	{
		return;
	}

	soundtarget = target;
	++validcount;
	P_RecursiveSound(emmiter->subsector->sector, 0);
}

bool P_CheckMeleeRange(MapObject* actor)
{
	MapObject* pl;
	fixed_t dist;
	fixed_t range;

	if (!actor->target)
	{
		return false;
	}

	pl = actor->target;
	dist = P_AproxDistance(pl->x - actor->x, pl->y - actor->y);

	if (gameversion <= GameVersion::exe_doom_1_2)
	{
		range = MELEERANGE;
	}
	else
	{
		range = MELEERANGE - 20 * FRACUNIT + pl->info->radius;
	}

	if (dist >= range)
	{
		return false;
	}

	if (!P_CheckSight(actor, actor->target))
	{
		return false;
	}

	// height check for melee attacks
	if (critical->overunder && pl->player)
	{
		if (pl->z >= actor->z + actor->height || actor->z >= pl->z + pl->height)
		{
			return false;
		}
	}

	return true;
}

bool P_CheckMissileRange(MapObject* actor)
{
	fixed_t dist;

	if (!P_CheckSight(actor, actor->target))
	{
		return false;
	}

	if (actor->flags & (int)mobjflag_e::MF_JUSTHIT)
	{
		// the target just hit the enemy, so fight back!
		actor->flags &= ~mobjflag_e::MF_JUSTHIT;
		return true;
	}

	if (actor->reactiontime)
	{
		return false;	// do not attack yet
	}

	// OPTIMIZE: get this from a global checksight
	dist = P_AproxDistance(actor->x - actor->target->x, actor->y - actor->target->y) - 64 * FRACUNIT;

	if (!(bool)actor->info->meleestate)
	{
		dist -= 128 * FRACUNIT;	// no melee attack, so fire more
	}

	dist >>= FRACBITS;

	// generalization of the Arch Vile's different attack range
	if (actor->info->maxattackrange > 0)
	{
		if (dist > actor->info->maxattackrange)
		{
			return false;	// too far away
		}
	}

	// generalization of the Revenant's different melee threshold
	if (actor->info->meleethreshold > 0)
	{
		if (dist < actor->info->meleethreshold)
		{
			return false;	// close for fist attack
		}
	}

	// generalize missile chance for Cyb, Spider, Revenant & Lost Soul
	if (actor->info->missilechancemult != FRACUNIT)
	{
		dist = FixedMul(dist, actor->info->missilechancemult);
	}

	// generalization of Min Missile Chance values hardcoded in vanilla
	if (dist > actor->info->minmissilechance)
	{
		dist = actor->info->minmissilechance;
	}

	if (P_Random() < dist)
	{
		return false;
	}

	return true;
}

// Move in the current direction, returns false if the move is blocked.
fixed_t xspeed[8] = {FRACUNIT,47000,0,-47000,-FRACUNIT,-47000,0,47000};
fixed_t yspeed[8] = {0,47000,FRACUNIT,47000,0,-47000,-FRACUNIT,-47000};

bool P_Move(MapObject* actor)
{
	fixed_t tryx;
	fixed_t tryy;

	line_t* ld;

	// warning: 'catch', 'throw', and 'try' are all C++ reserved words
	bool try_ok;
	bool good;

	if (actor->movedir == dirtype_t::DI_NODIR)
	{
		return false;
	}

	if ((unsigned)actor->movedir >= 8)
	{
		I_Error("Weird actor->movedir!");
	}

	tryx = actor->x + actor->info->speed * xspeed[actor->movedir];
	tryy = actor->y + actor->info->speed * yspeed[actor->movedir];

	try_ok = P_TryMove(actor, tryx, tryy);

	if (!try_ok)
	{
		// open any specials
		if (actor->flags & mobjflag_e::MF_FLOAT && floatok)
		{
			// must adjust height
			if (actor->z < tmfloorz)
			{
				actor->z += FLOATSPEED;
			}
			else
			{
				actor->z -= FLOATSPEED;
			}

			actor->flags |= mobjflag_e::MF_INFLOAT;
			return true;
		}

		if (!numspechit)
		{
			return false;
		}

		actor->movedir = dirtype_t::DI_NODIR;
		good = false;
		while (numspechit--)
		{
			ld = spechit[numspechit];
			// if the special is not a door
			// that can be opened,
			// return false
			if (P_UseSpecialLine(actor, ld, 0))
			{
				good = true;
			}
		}
		return good;
	}
	else
	{
		actor->flags &= ~mobjflag_e::MF_INFLOAT;
	}

	if (!(actor->flags & mobjflag_e::MF_FLOAT))
	{
		actor->z = actor->floorz;
	}

	return true;
}

// Attempts to move actor on in its current (ob->moveangle) direction.
// If blocked by either a wall or an actor returns FALSE
// If move is either clear or blocked only by a door, returns TRUE and sets...
// If a door is in the way, an OpenDoor call is made to start it opening.
bool P_TryWalk(MapObject* actor)
{
	if (!P_Move(actor))
	{
		return false;
	}

	actor->movecount = P_Random() & 15;
	return true;
}

void P_NewChaseDir(MapObject* actor)
{
	fixed_t deltax;
	fixed_t deltay;

	dirtype_t d[3];

	int tdir;
	dirtype_t olddir;

	dirtype_t turnaround;

	if (!actor->target)
	{
		I_Error("P_NewChaseDir: called with no target");
	}

	olddir = actor->movedir;
	turnaround = opposite[::std::size_t(olddir)];

	deltax = actor->target->x - actor->x;
	deltay = actor->target->y - actor->y;

	if (deltax > 10 * FRACUNIT)
	{
		d[1] = dirtype_t::DI_EAST;
	}
	else if (deltax < -10 * FRACUNIT)
	{
		d[1] = dirtype_t::DI_WEST;
	}
	else
	{
		d[1] = dirtype_t::DI_NODIR;
	}

	if (deltay < -10 * FRACUNIT)
	{
		d[2] = dirtype_t::DI_SOUTH;
	}
	else if (deltay > 10 * FRACUNIT)
	{
		d[2] = dirtype_t::DI_NORTH;
	}
	else
	{
		d[2] = dirtype_t::DI_NODIR;
	}

	// try direct route
	if (d[1] != dirtype_t::DI_NODIR && d[2] != dirtype_t::DI_NODIR)
	{
		actor->movedir = diags[((deltay < 0) << 1) + (deltax > 0)];
		if (actor->movedir != (int)turnaround && P_TryWalk(actor))
		{
			return;
		}
	}

	// try other directions
	if (P_Random() > 200 || abs(deltay) > abs(deltax))
	{
		tdir = d[1];
		d[1] = d[2];
		d[2] = tdir;
	}

	if (d[1] == turnaround)
	{
		d[1] = dirtype_t::DI_NODIR;
	}

	if (d[2] == turnaround)
	{
		d[2] = dirtype_t::DI_NODIR;
	}

	if (d[1] != dirtype_t::DI_NODIR)
	{
		actor->movedir = d[1];
		if (P_TryWalk(actor))
		{
			// either moved forward or attacked
			return;
		}
	}

	if (d[2] != dirtype_t::DI_NODIR)
	{
		actor->movedir = d[2];

		if (P_TryWalk(actor))
		{
			return;
		}
	}

	// there is no direct path to the player, so pick another direction.
	if (olddir != dirtype_t::DI_NODIR)
	{
		actor->movedir = olddir;

		if (P_TryWalk(actor))
		{
			return;
		}
	}

	// randomly determine direction of search
	if (P_Random() & 1)
	{
		for (tdir = (int)dirtype_t::DI_EAST; tdir <= (int)dirtype_t::DI_SOUTHEAST; ++tdir)
		{
			if (tdir != (int)turnaround)
			{
				actor->movedir = tdir;

				if (P_TryWalk(actor))
				{
					return;
				}
			}
		}
	}
	else
	{
		for (tdir = (int)dirtype_t::DI_SOUTHEAST; tdir != ((int)dirtype_t::DI_EAST - 1); --tdir)
		{
			if (tdir != (int)turnaround)
			{
				actor->movedir = tdir;

				if (P_TryWalk(actor))
				{
					return;
				}
			}
		}
	}

	if (turnaround != dirtype_t::DI_NODIR)
	{
		actor->movedir = turnaround;
		if (P_TryWalk(actor))
		{
			return;
		}
	}

	actor->movedir = dirtype_t::DI_NODIR;	// can not move
}

// If allaround is false, only look 180 degrees in front.
// Returns true if a player is targeted.
bool P_LookForPlayers(MapObject* actor, bool allaround)
{
	int c;
	int stop;
	Player* player;
	angle_t an;
	fixed_t dist;

	c = 0;
	stop = (actor->lastlook - 1) & 3;

	for (; ; actor->lastlook = (actor->lastlook + 1) & 3)
	{
		if (!playeringame[actor->lastlook])
		{
			continue;
		}

		if (c++ == 2 || actor->lastlook == stop)
		{
			// done looking
			return false;
		}

		player = &players[actor->lastlook];

		// monsters don't look for players with NOTARGET cheat
		if (player->cheats & CheatType::CF_NOTARGET)
		{
			continue;
		}

		if (player->health <= 0)
		{
			continue;		// dead
		}

		if (!P_CheckSight(actor, player))
		{
			continue;		// out of sight
		}

		if (!allaround)
		{
			an = R_PointToAngle2(actor->x, actor->y, player->x, player->y) - actor->angle;

			if (an > ANG90 && an < ANG270)
			{
				dist = P_AproxDistance(player->x - actor->x, player->y - actor->y);
				// if real close, react anyway
				if (dist > MELEERANGE)
				{
					continue;	// behind back
				}
			}
		}

		actor->target = player;
		return true;
	}

	return false;
}

// A_KeenDie
// DOOM II special, map 32.
// Uses special tag 666.
void A_KeenDie(MapObject* mo)
{
	thinker_t* th;
	MapObject* mo2;
	line_t junk;

	A_Fall(mo);

	// scan the remaining thinkers to see if all Keens are dead
	for (th = thinkercap.next; th != &thinkercap; th = th->next)
	{
		if (th->function.acp1 != (actionf_p1)P_MobjThinker)
		{
			continue;
		}

		mo2 = (MapObject*)th;
		if (mo2 != mo && mo2->type == mo->type && mo2->health > 0)
		{
			// other Keen not dead
			return;
		}
	}

	junk.tag = 666;
	EV_DoDoor(&junk, vldoor_e::vld_open);
}

//
// ACTION ROUTINES
//

// Stay in state until a player is sighted.
void A_Look(MapObject* actor)
{
	MapObject* targ;

	actor->threshold = 0;	// any shot will wake up
	targ = actor->subsector->sector->soundtarget;

	// monsters don't look for players with NOTARGET cheat
	if (targ && targ->player && (targ->player->cheats & CheatType::CF_NOTARGET))
	{
		return;
	}

	if (targ && (targ->flags & mobjflag_e::MF_SHOOTABLE))
	{
		actor->target = targ;

		if (actor->flags & mobjflag_e::MF_AMBUSH)
		{
			if (P_CheckSight(actor, actor->target))
			{
				goto seeyou;
			}
		}
		else
		{
			goto seeyou;
		}
	}

	if (!P_LookForPlayers(actor, false))
	{
		return;
	}

	// go into chase state
seeyou:
	if (actor->info->seesound)
	{
		int sound;

		switch (actor->info->seesound)
		{
		case sfxenum_t::sfx_posit1:
		case sfxenum_t::sfx_posit2:
		case sfxenum_t::sfx_posit3:
			sound = sfxenum_t::sfx_posit1 + P_Random() % 3;
			break;

		case sfxenum_t::sfx_bgsit1:
		case sfxenum_t::sfx_bgsit2:
			sound = sfxenum_t::sfx_bgsit1 + P_Random() % 2;
			break;

		default:
			sound = actor->info->seesound;
			break;
		}

		if (actor->type == mobjtype_t::MT_SPIDER || actor->type == mobjtype_t::MT_CYBORG)
		{
			// full volume
			// prevent from adding up volume
			crispy->soundfull ? S_StartSoundOnce(nullptr, sound) : S_StartSound(nullptr, sound);
		}
		else
		{
			S_StartSound(actor, sound);
		}

		// make seesounds uninterruptible
		if (crispy->soundfull)
		{
			S_UnlinkSound(actor);
		}
	}

	P_SetMobjState(actor, actor->info->seestate);
}

// A_Chase
// Actor has a melee attack,
// so it tries to close as fast as possible
void A_Chase(MapObject* actor)
{
	int delta;

	if (actor->reactiontime)
	{
		actor->reactiontime--;
	}

	// modify target threshold
	if (actor->threshold)
	{
		if (gameversion > GameVersion::exe_doom_1_2 && (!actor->target || actor->target->health <= 0))
		{
			actor->threshold = 0;
		}
		else
		{
			actor->threshold--;
		}
	}

	// turn towards movement direction if not there yet
	if (actor->movedir < 8)
	{
		actor->angle &= (7 << 29);// (bits 1, 2, 3 kept, all others false)
		delta = actor->angle - (actor->movedir << 29);

		// FEATURE NOTE FIXME
		// compare 3 least significant bits of angle with the 3 bits of movedir
		// the first bit of these 3 bit patterns now indicates the sign, because of the shifting
		// pretty crazy to combine arithmetic with bit operations, to exploit the 32 bit sign bit
		// with the assumption it's actually a 32 bit integer
		// SO, LET'S MAKE A CLASS TO HANDLE THIS PROPERLY!! YAY YAY YA!!

		// Consider how these values are to be used next, to see if we really even need shifts and such nonsense:
		// FOUR CASES (A > 0 && M > 0; 0 > A && 0 > M; A > 0 > M; M > 0 > A):

		// 1)
		// if A 000, 001, 010, 011 then angle is positive
		// if M 000, 001, 010, 011 then movedir is positive
		// then A - M is positive if A's first bit is lower than M's

		// 2)
		// if A 100, 101, 110, 111 then angle is negative
		// if D 100, 101, 110, 111 then movedir is negative
		// then A - M is  if A's first bit is  than M's

		// 3)
		// if A 000, 001, 010, 011 then angle is positive
		// if M 100, 101, 110, 111 then movedir is positive
		// then A - M is  if A's first bit is  than M's

		// 4)
		// if A 100, 101, 110, 111 then angle is negative
		// if D 000, 001, 010, 011 then movedir is positive
		// then A - M is  if A's first bit is  than M's

		if (delta > 0)
		{
			actor->angle -= ANG90 / 2;
		}
		else if (delta < 0)
		{
			actor->angle += ANG90 / 2;
		}
	}

	if (!actor->target || !(actor->target->flags & mobjflag_e::MF_SHOOTABLE))
	{
		// look for a new target
		if (P_LookForPlayers(actor, true))
		{
			return;	// got a new target
		}

		P_SetMobjState(actor, actor->info->spawnstate);
		return;
	}

	// do not attack twice in a row
	if (actor->flags & mobjflag_e::MF_JUSTATTACKED)
	{
		actor->flags &= ~mobjflag_e::MF_JUSTATTACKED;
		if (gameskill != SkillType::sk_nightmare && !fastparm)
		{
			P_NewChaseDir(actor);
		}
		return;
	}

	// check for melee attack
	if (actor->info->meleestate && P_CheckMeleeRange(actor))
	{
		if (actor->info->attacksound)
		{
			S_StartSound(actor, actor->info->attacksound);
		}

		P_SetMobjState(actor, actor->info->meleestate);
		return;
	}

	// check for missile attack
	if ((bool)actor->info->missilestate)
	{
		if (gameskill < SkillType::sk_nightmare && !fastparm && actor->movecount)
		{
			goto nomissile;
		}

		if (!P_CheckMissileRange(actor))
		{
			goto nomissile;
		}

		P_SetMobjState(actor, actor->info->missilestate);
		actor->flags = (int)actor->flags | (int)mobjflag_e::MF_JUSTATTACKED;
		return;
	}

	// ?
nomissile:
	// possibly choose another target
	if (netgame && !actor->threshold && !P_CheckSight(actor, actor->target))
	{
		if (P_LookForPlayers(actor, true))
		{
			return;	// got a new target
		}
	}

	// chase towards player
	if (--actor->movecount < 0 || !P_Move(actor))
	{
		P_NewChaseDir(actor);
	}

	// make active sound
	if (actor->info->activesound && P_Random() < 3)
	{
		S_StartSound(actor, actor->info->activesound);
	}
}

void A_FaceTarget(MapObject* actor)
{
	if (!actor->target)
	{
		return;
	}

	actor->flags &= ~mobjflag_e::MF_AMBUSH;

	actor->angle = R_PointToAngle2(actor->x, actor->y, actor->target->x, actor->target->y);

	if (actor->target->flags & mobjflag_e::MF_SHADOW)
	{
		actor->angle += P_SubRandom() << 21;
	}
}

void A_PosAttack(MapObject* actor)
{
	int angle;
	int damage;
	int slope;

	if (!actor->target)
	{
		return;
	}

	A_FaceTarget(actor);
	angle = actor->angle;
	slope = P_AimLineAttack(actor, angle, MISSILERANGE);

	S_StartSound(actor, sfxenum_t::sfx_pistol);
	angle += P_SubRandom() << 20;
	damage = ((P_Random() % 5) + 1) * 3;
	P_LineAttack(actor, angle, MISSILERANGE, slope, damage);
}

void A_SPosAttack(MapObject* actor)
{
	int i;
	int angle;
	int bangle;
	int damage;
	int slope;

	if (!actor->target)
	{
		return;
	}

	S_StartSound(actor, sfxenum_t::sfx_shotgn);
	A_FaceTarget(actor);
	bangle = actor->angle;
	slope = P_AimLineAttack(actor, bangle, MISSILERANGE);

	for (i = 0; i < 3; ++i)
	{
		angle = bangle + (P_SubRandom() << 20);
		damage = ((P_Random() % 5) + 1) * 3;
		P_LineAttack(actor, angle, MISSILERANGE, slope, damage);
	}
}

void A_CPosAttack(MapObject* actor)
{
	int angle;
	int bangle;
	int damage;
	int slope;

	if (!actor->target)
	{
		return;
	}

	S_StartSound(actor, sfxenum_t::sfx_shotgn);
	A_FaceTarget(actor);
	bangle = actor->angle;
	slope = P_AimLineAttack(actor, bangle, MISSILERANGE);

	angle = bangle + (P_SubRandom() << 20);
	damage = ((P_Random() % 5) + 1) * 3;
	P_LineAttack(actor, angle, MISSILERANGE, slope, damage);
}

void A_CPosRefire(MapObject* actor)
{
	// keep firing unless target got out of sight
	A_FaceTarget(actor);

	if (P_Random() < 40)
	{
		return;
	}

	if (!actor->target || actor->target->health <= 0 || !P_CheckSight(actor, actor->target))
	{
		P_SetMobjState(actor, actor->info->seestate);
	}
}

void A_SpidRefire(MapObject* actor)
{
	// keep firing unless target got out of sight
	A_FaceTarget(actor);

	if (P_Random() < 10)
	{
		return;
	}

	if (!actor->target || actor->target->health <= 0 || !P_CheckSight(actor, actor->target))
	{
		P_SetMobjState(actor, actor->info->seestate);
	}
}

void A_BspiAttack(MapObject* actor)
{
	if (!actor->target)
	{
		return;
	}

	A_FaceTarget(actor);

	// launch a missile
	P_SpawnMissile(actor, actor->target, mobjtype_t::MT_ARACHPLAZ);
}

void A_TroopAttack(MapObject* actor)
{
	int damage;

	if (!actor->target)
	{
		return;
	}

	A_FaceTarget(actor);
	if (P_CheckMeleeRange(actor))
	{
		S_StartSound(actor, sfxenum_t::sfx_claw);
		damage = (P_Random() % 8 + 1) * 3;
		P_DamageMobj(actor->target, actor, actor, damage);
		return;
	}

	// launch a missile
	P_SpawnMissile(actor, actor->target, mobjtype_t::MT_TROOPSHOT);
}

void A_SargAttack(MapObject* actor)
{
	int damage;

	if (!actor->target)
	{
		return;
	}

	A_FaceTarget(actor);

	if (gameversion > GameVersion::exe_doom_1_2)
	{
		if (!P_CheckMeleeRange(actor))
		{
			return;
		}
	}

	damage = ((P_Random() % 10) + 1) * 4;

	if (gameversion <= GameVersion::exe_doom_1_2)
	{
		P_LineAttack(actor, actor->angle, MELEERANGE, 0, damage);
	}
	else
	{
		P_DamageMobj(actor->target, actor, actor, damage);
	}
}

void A_HeadAttack(MapObject* actor)
{
	int damage;

	if (!actor->target)
	{
		return;
	}

	A_FaceTarget(actor);
	if (P_CheckMeleeRange(actor))
	{
		damage = (P_Random() % 6 + 1) * 10;
		P_DamageMobj(actor->target, actor, actor, damage);
		return;
	}

	// launch a missile
	P_SpawnMissile(actor, actor->target, mobjtype_t::MT_HEADSHOT);
}

void A_CyberAttack(MapObject* actor)
{
	if (!actor->target)
	{
		return;
	}

	A_FaceTarget(actor);
	P_SpawnMissile(actor, actor->target, mobjtype_t::MT_ROCKET);
}

void A_BruisAttack(MapObject* actor)
{
	int damage;

	if (!actor->target)
	{
		return;
	}

	// face the enemy
// A_FaceTarget(actor);
	if (P_CheckMeleeRange(actor))
	{
		S_StartSound(actor, sfxenum_t::sfx_claw);
		damage = (P_Random() % 8 + 1) * 10;
		P_DamageMobj(actor->target, actor, actor, damage);
		return;
	}

	// launch a missile
	P_SpawnMissile(actor, actor->target, mobjtype_t::MT_BRUISERSHOT);
}

//
// A_SkelMissile
//
void A_SkelMissile(MapObject* actor)
{
	MapObject* mo;

	if (!actor->target)
	{
		return;
	}

	A_FaceTarget(actor);
	actor->z += 16 * FRACUNIT;	// so missile spawns higher
	mo = P_SpawnMissile(actor, actor->target, mobjtype_t::MT_TRACER);
	actor->z -= 16 * FRACUNIT;	// back to normal

	mo->x += mo->momx;
	mo->y += mo->momy;
	mo->tracer = actor->target;
}

int TRACEANGLE = 0xc000000;

void A_Tracer(MapObject* actor)
{
	angle_t exact;
	fixed_t dist;
	fixed_t slope;
	MapObject* dest;
	MapObject* th;
	extern TimeType demostarttic;

	// fix revenant internal demo bug
	if ((gametic - demostarttic) & 3)
	{
		return;
	}

	// spawn a puff of smoke behind the rocket
	P_SpawnPuff(actor->x, actor->y, actor->z);

	th = P_SpawnMobj(actor->x - actor->momx, actor->y - actor->momy, actor->z, mobjtype_t::MT_SMOKE);

	th->momz = FRACUNIT;
	th->tics -= P_Random() & 3;
	if (th->tics < 1)
	{
		th->tics = 1;
	}

	// adjust direction
	dest = actor->tracer;

	if (!dest || dest->health <= 0)
	{
		return;
	}

	// change angle
	exact = R_PointToAngle2(actor->x, actor->y, dest->x, dest->y);

	if (exact != actor->angle)
	{
		if (exact - actor->angle > 0x80000000)
		{
			actor->angle -= TRACEANGLE;
			if (exact - actor->angle < 0x80000000)
			{
				actor->angle = exact;
			}
		}
		else
		{
			actor->angle += TRACEANGLE;
			if (exact - actor->angle > 0x80000000)
			{
				actor->angle = exact;
			}
		}
	}

	exact = actor->angle >> ANGLETOFINESHIFT;
	actor->momx = FixedMul(actor->info->speed, finecosine[exact]);
	actor->momy = FixedMul(actor->info->speed, finesine[exact]);

	// change slope
	dist = P_AproxDistance(dest->x - actor->x,
		dest->y - actor->y);

	dist = dist / actor->info->speed;

	if (dist < 1)
	{
		dist = 1;
	}
	slope = (dest->z + 40 * FRACUNIT - actor->z) / dist;

	if (slope < actor->momz)
	{
		actor->momz -= FRACUNIT / 8;
	}
	else
	{
		actor->momz += FRACUNIT / 8;
	}
}

void A_SkelWhoosh(MapObject* actor)
{
	if (!actor->target)
	{
		return;
	}
	A_FaceTarget(actor);
	S_StartSound(actor, sfxenum_t::sfx_skeswg);
}

void A_SkelFist(MapObject* actor)
{
	int damage;

	if (!actor->target)
	{
		return;
	}

	A_FaceTarget(actor);

	if (P_CheckMeleeRange(actor))
	{
		damage = ((P_Random() % 10) + 1) * 6;
		S_StartSound(actor, sfxenum_t::sfx_skepch);
		P_DamageMobj(actor->target, actor, actor, damage);
	}
}

// PIT_VileCheck
// Detect a corpse that could be raised.
MapObject* corpsehit;
MapObject* vileobj;
fixed_t viletryx;
fixed_t viletryy;

bool PIT_VileCheck(MapObject* thing)
{
	int maxdist;
	bool check;

	if (!(thing->flags & mobjflag_e::MF_CORPSE))
	{
		return true;	// not a monster
	}

	if (thing->tics != -1)
	{
		return true;	// not lying still yet
	}

	if (thing->info->raisestate == statenum_t::S_NULL)
	{
		return true;	// monster doesn't have a raise state
	}

	maxdist = thing->info->radius + mobjinfo[::std::size_t(mobjtype_t::MT_VILE)].radius;

	if (abs(thing->x - viletryx) > maxdist || abs(thing->y - viletryy) > maxdist)
	{
		return true;		// not actually touching
	}

	corpsehit = thing;
	corpsehit->momx = corpsehit->momy = 0;
	corpsehit->height <<= 2;
	check = P_CheckPosition(corpsehit, corpsehit->x, corpsehit->y);
	corpsehit->height >>= 2;

	if (!check)
	{
		return true;		// doesn't fit here
	}

	return false;		// got one, so stop checking
}

// A_VileChase
// Check for ressurecting a body
void A_VileChase(MapObject* actor)
{
	int xl;
	int xh;
	int yl;
	int yh;

	int bx;
	int by;

	mobjinfo_t* info;
	MapObject* temp;

	if (actor->movedir != dirtype_t::DI_NODIR)
	{
		// check for corpses to raise
		viletryx = actor->x + actor->info->speed * xspeed[actor->movedir];
		viletryy = actor->y + actor->info->speed * yspeed[actor->movedir];

		xl = (viletryx - bmaporgx - MAXRADIUS * 2) >> MAPBLOCKSHIFT;
		xh = (viletryx - bmaporgx + MAXRADIUS * 2) >> MAPBLOCKSHIFT;
		yl = (viletryy - bmaporgy - MAXRADIUS * 2) >> MAPBLOCKSHIFT;
		yh = (viletryy - bmaporgy + MAXRADIUS * 2) >> MAPBLOCKSHIFT;

		vileobj = actor;
		for (bx = xl; bx <= xh; ++bx)
		{
			for (by = yl; by <= yh; ++by)
			{
				// Call PIT_VileCheck to check
				// whether object is a corpse
				// that canbe raised.
				if (!P_BlockThingsIterator(bx, by, PIT_VileCheck))
				{
					// got one!
					temp = actor->target;
					actor->target = corpsehit;
					A_FaceTarget(actor);
					actor->target = temp;

					P_SetMobjState(actor, statenum_t::S_VILE_HEAL1);
					S_StartSound(corpsehit, sfxenum_t::sfx_slop);
					info = corpsehit->info;

					P_SetMobjState(corpsehit, info->raisestate);
					corpsehit->height <<= 2;
					corpsehit->flags = info->flags;
					corpsehit->health = info->spawnhealth;
					corpsehit->target = NULL;

					// count resurrected monsters
					++extrakills;

					// resurrected pools of gore ("ghost monsters") are translucent
					if (corpsehit->height == 0 && corpsehit->radius == 0)
					{
						corpsehit->flags |= mobjflag_e::MF_TRANSLUCENT;
						fprintf(stderr, "A_VileChase: Resurrected ghost monster (%d) at (%d/%d)!\n",
							corpsehit->type, corpsehit->x >> FRACBITS, corpsehit->y >> FRACBITS);
					}

					return;
				}
			}
		}
	}

	// Return to normal attack.
	A_Chase(actor);
}

//
// A_VileStart
//
void A_VileStart(MapObject* actor)
{
	S_StartSound(actor, sfxenum_t::sfx_vilatk);
}

//
// A_Fire
// Keep fire in front of player unless out of sight
//
void A_Fire(MapObject* actor);

void A_StartFire(MapObject* actor)
{
	S_StartSound(actor, sfxenum_t::sfx_flamst);
	A_Fire(actor);
}

void A_FireCrackle(MapObject* actor)
{
	S_StartSound(actor, sfxenum_t::sfx_flame);
	A_Fire(actor);
}

void A_Fire(MapObject* actor)
{
	MapObject* dest;
	MapObject* target;
	unsigned an;

	dest = actor->tracer;
	if (!dest)
	{
		return;
	}

	target = P_SubstNullMobj(actor->target);

	// don't move it if the vile lost sight
	if (!P_CheckSight(target, dest))
	{
		return;
	}

	an = dest->angle >> ANGLETOFINESHIFT;

	P_UnsetThingPosition(actor);
	actor->x = dest->x + FixedMul(24 * FRACUNIT, finecosine[an]);
	actor->y = dest->y + FixedMul(24 * FRACUNIT, finesine[an]);
	actor->z = dest->z;
	P_SetThingPosition(actor);
}

// Spawn the hellfire
void A_VileTarget(MapObject* actor)
{
	MapObject* fog;

	if (!actor->target)
	{
		return;
	}

	A_FaceTarget(actor);

	fog = P_SpawnMobj(actor->target->x, actor->target->x, actor->target->z, mobjtype_t::MT_FIRE);

	actor->tracer = fog;
	fog->target = actor;
	fog->tracer = actor->target;
	// play DSFLAMST sound when Arch-Vile spawns fire attack
	if (crispy->soundfix && I_GetSfxLumpNum(&S_sfx[::std::size_t(sfxenum_t::sfx_flamst)]) != -1)
	{
		S_StartSound(fog, sfxenum_t::sfx_flamst);
		// make DSFLAMST sound uninterruptible
		if (crispy->soundfull)
		{
			S_UnlinkSound(fog);
		}
	}

	A_Fire(fog);
}

void A_VileAttack(MapObject* actor)
{
	MapObject* fire;
	int an;

	if (!actor->target)
	{
		return;
	}

	A_FaceTarget(actor);

	if (!P_CheckSight(actor, actor->target))
	{
		return;
	}

	S_StartSound(actor, sfxenum_t::sfx_barexp);
	P_DamageMobj(actor->target, actor, actor, 20);
	actor->target->momz = 1000 * FRACUNIT / actor->target->info->mass;

	an = actor->angle >> ANGLETOFINESHIFT;

	fire = actor->tracer;

	if (!fire)
	{
		return;
	}

	// move the fire between the vile and the player
	fire->x = actor->target->x - FixedMul(24 * FRACUNIT, finecosine[an]);
	fire->y = actor->target->y - FixedMul(24 * FRACUNIT, finesine[an]);
	P_RadiusAttack(fire, actor, 70);
}

// Mancubus attack, firing three missiles (bruisers) in three different directions? Doesn't look like it.
constexpr size_t FATSPREAD{ANG90 / 8}

void A_FatRaise(MapObject* actor)
{
	A_FaceTarget(actor);
	S_StartSound(actor, sfxenum_t::sfx_manatk);
}

void A_FatAttack1(MapObject* actor)
{
	MapObject* mo;
	MapObject* target;
	int an;

	A_FaceTarget(actor);

	// Change direction to ...
	actor->angle += FATSPREAD;
	target = P_SubstNullMobj(actor->target);
	P_SpawnMissile(actor, target, mobjtype_t::MT_FATSHOT);

	mo = P_SpawnMissile(actor, target, mobjtype_t::MT_FATSHOT);
	mo->angle += FATSPREAD;
	an = mo->angle >> ANGLETOFINESHIFT;
	mo->momx = FixedMul(mo->info->speed, finecosine[an]);
	mo->momy = FixedMul(mo->info->speed, finesine[an]);
}

void A_FatAttack2(MapObject* actor)
{
	MapObject* mo;
	MapObject* target;
	int an;

	A_FaceTarget(actor);
	// Now here choose opposite deviation.
	actor->angle -= FATSPREAD;
	target = P_SubstNullMobj(actor->target);
	P_SpawnMissile(actor, target, mobjtype_t::MT_FATSHOT);

	mo = P_SpawnMissile(actor, target, mobjtype_t::MT_FATSHOT);
	mo->angle -= FATSPREAD * 2;
	an = mo->angle >> ANGLETOFINESHIFT;
	mo->momx = FixedMul(mo->info->speed, finecosine[an]);
	mo->momy = FixedMul(mo->info->speed, finesine[an]);
}

void A_FatAttack3(MapObject* actor)
{
	MapObject* mo;
	MapObject* target;
	int an;

	A_FaceTarget(actor);

	target = P_SubstNullMobj(actor->target);

	mo = P_SpawnMissile(actor, target, mobjtype_t::MT_FATSHOT);
	mo->angle -= FATSPREAD / 2;
	an = mo->angle >> ANGLETOFINESHIFT;
	mo->momx = FixedMul(mo->info->speed, finecosine[an]);
	mo->momy = FixedMul(mo->info->speed, finesine[an]);

	mo = P_SpawnMissile(actor, target, mobjtype_t::MT_FATSHOT);
	mo->angle += FATSPREAD / 2;
	an = mo->angle >> ANGLETOFINESHIFT;
	mo->momx = FixedMul(mo->info->speed, finecosine[an]);
	mo->momy = FixedMul(mo->info->speed, finesine[an]);
}

// SkullAttack
// Fly at the player like a missile.
constexpr size_t SKULLSPEED{20 * FRACUNIT};

void A_SkullAttack(MapObject* actor)
{
	MapObject* dest;
	angle_t an;
	int dist;

	if (!actor->target)
	{
		return;
	}

	dest = actor->target;
	actor->flags |= mobjflag_e::MF_SKULLFLY;

	S_StartSound(actor, actor->info->attacksound);
	A_FaceTarget(actor);
	an = actor->angle >> ANGLETOFINESHIFT;
	actor->momx = FixedMul(SKULLSPEED, finecosine[an]);
	actor->momy = FixedMul(SKULLSPEED, finesine[an]);
	dist = P_AproxDistance(dest->x - actor->x, dest->y - actor->y);
	dist = dist / SKULLSPEED;

	if (dist < 1)
	{
		dist = 1;
	}
	actor->momz = (dest->z + (dest->height >> 1) - actor->z) / dist;
}

// Spawn a lost soul and launch it at the target
void A_PainShootSkull(MapObject* actor, angle_t angle)
{
	fixed_t x;
	fixed_t y;
	fixed_t z;

	MapObject* newmobj;
	angle_t an;
	int prestep;
	int count;
	thinker_t* currentthinker;

	// count total number of skull currently on the level
	count = 0;

	currentthinker = thinkercap.next;
	while (currentthinker != &thinkercap)
	{
		if ((currentthinker->function.acp1 == (actionf_p1)P_MobjThinker) && ((MapObject*)currentthinker)->type == mobjtype_t::MT_SKULL)
		{
			++count;
		}

		currentthinker = currentthinker->next;
	}

	// if there are allready 20 skulls on the level,
	// don't spit another one
	if (count > 20)
	{
		return;
	}

	// okay, there's playe for another one
	an = angle >> ANGLETOFINESHIFT;

	prestep = 4 * FRACUNIT + 3 * (actor->info->radius + mobjinfo[::std::size_t(mobjtype_t::MT_SKULL)].radius) / 2;

	x = actor->x + FixedMul(prestep, finecosine[an]);
	y = actor->y + FixedMul(prestep, finesine[an]);
	z = actor->z + 8 * FRACUNIT;

	newmobj = P_SpawnMobj(x, y, z, mobjtype_t::MT_SKULL);

	// Check for movements.
	if (!P_TryMove(newmobj, newmobj->x, newmobj->y))
	{
		// kill it immediately
		P_DamageMobj(newmobj, actor, actor, 10000);
		return;
	}

	// Lost Souls bleed Puffs
	if (crispy->coloredblood)
	{
		newmobj->flags |= mobjflag_e::MF_NOBLOOD;
	}

	newmobj->target = actor->target;
	A_SkullAttack(newmobj);
}

// Spawn a lost soul and launch it at the target
void A_PainAttack(MapObject* actor)
{
	if (!actor->target)
	{
		return;
	}

	A_FaceTarget(actor);
	A_PainShootSkull(actor, actor->angle);
}

void A_PainDie(MapObject* actor)
{
	A_Fall(actor);
	A_PainShootSkull(actor, actor->angle + ANG90);
	A_PainShootSkull(actor, actor->angle + ANG180);
	A_PainShootSkull(actor, actor->angle + ANG270);
}

void A_Scream(MapObject* actor)
{
	sfxenum_t sound;

	switch (actor->info->deathsound)
	{
	case sfxenum_t::sfx_None:
		return;

	case sfxenum_t::sfx_podth1:
	case sfxenum_t::sfx_podth2:
	case sfxenum_t::sfx_podth3:
		sound = sfxenum_t::sfx_podth1 + P_Random() % 3;
		break;

	case sfxenum_t::sfx_bgdth1:
	case sfxenum_t::sfx_bgdth2:
		sound = sfxenum_t::sfx_bgdth1 + P_Random() % 2;
		break;

	default:
		sound = actor->info->deathsound;
		break;
	}

	// Check for bosses.
	if (actor->type == mobjtype_t::MT_SPIDER || actor->type == mobjtype_t::MT_CYBORG)
	{
		// full volume
		// prevent from adding up volume
		crispy->soundfull ? S_StartSoundOnce(nullptr, sound) : S_StartSound(nullptr, sound);
	}
	else
	{
		S_StartSound(actor, sound);
	}
}

void A_XScream(MapObject* actor)
{
	S_StartSound(actor, sfxenum_t::sfx_slop);
}

void A_Pain(MapObject* actor)
{
	if (actor->info->painsound)
	{
		S_StartSound(actor, actor->info->painsound);
	}
}

void A_Fall(MapObject* actor)
{
	// actor is on ground, it can be walked over
	actor->flags &= ~mobjflag_e::MF_SOLID;

	// So change this if corpse objects
	// are meant to be obstacles.
}

void A_Explode(MapObject* thingy)
{
	P_RadiusAttack(thingy, thingy->target, 128);
}

// Check whether the death of the specified monster type is allowed to trigger the end of episode special action.
// This behavior changed in v1.9, the most notable effect of which was to break uac_dead.wad
static bool CheckBossEnd(mobjtype_t motype)
{
	if (gameversion < GameVersion::exe_ultimate)
	{
		if (gamemap != 8)
		{
			return false;
		}

		// Baron death on later episodes is nothing special.
		if (motype == mobjtype_t::MT_BRUISER && gameepisode != 1)
		{
			return false;
		}

		return true;
	}
	else
	{
		// New logic that appeared in Ultimate Doom.
		// Looks like the logic was overhauled while adding in the episode 4 support. Now bosses only trigger on their specific episode.
		switch (gameepisode)
		{
		case 1:
			return gamemap == 8 && motype == mobjtype_t::MT_BRUISER;

		case 2:
			return gamemap == 8 && motype == mobjtype_t::MT_CYBORG;

		case 3:
			return gamemap == 8 && motype == mobjtype_t::MT_SPIDER;

		case 4:
			return (gamemap == 6 && motype == mobjtype_t::MT_CYBORG) || (gamemap == 8 && motype == mobjtype_t::MT_SPIDER);

		case 5:
			return false;

		default:
			return gamemap == 8;
		}
	}
}

// A_BossDeath
// Possibly trigger special effects if on first boss level
void A_BossDeath(MapObject* mo)
{
	thinker_t* th;
	MapObject* mo2;
	line_t junk;
	int i;

	if (gamemode == GameMode::commercial)
	{
		// Master Levels in PC slot 7
		if (gamemap != 7 && !(gamemission == GameMission::pack_master && (gamemap == 14 || gamemap == 15 || gamemap == 16)))
		{
			return;
		}

		if ((mo->type != mobjtype_t::MT_FATSO) && (mo->type != mobjtype_t::MT_BABY))
		{
			return;
		}
	}
	else
	{
		if (!CheckBossEnd(mo->type))
		{
			return;
		}
	}

	// make sure there is a player alive for victory
	for (i = 0; i < MAX_PLAYERS; ++i)
	{
		if (playeringame[i] && players[i].health > 0)
		{
			break;
		}
	}

	if (i == MAX_PLAYERS)
	{
		return;	// no one left alive, so do not end game
	}

	// scan the remaining thinkers to see
	// if all bosses are dead
	for (th = thinkercap.next; th != &thinkercap; th = th->next)
	{
		if (th->function.acp1 != (actionf_p1)P_MobjThinker)
		{
			continue;
		}

		mo2 = (MapObject*)th;
		if (mo2 != mo && mo2->type == mo->type && mo2->health > 0)
		{
			// other boss not dead
			return;
		}
	}

	// victory!
	if (gamemode == GameMode::commercial)
	{
		if (gamemap == 7 || (gamemission == GameMission::pack_master && (gamemap == 14 || gamemap == 15 || gamemap == 16)))
		{
			if (mo->type == mobjtype_t::MT_FATSO)
			{
				junk.tag = 666;
				EV_DoFloor(&junk, floor_e::lowerFloorToLowest);
				return;
			}
			else if (mo->type == mobjtype_t::MT_BABY)
			{
				junk.tag = 667;
				EV_DoFloor(&junk, floor_e::raiseToTexture);
				return;
			}
		}
	}
	else
	{
		switch (gameepisode)
		{
		case 1:
			junk.tag = 666;
			EV_DoFloor(&junk, floor_e::lowerFloorToLowest);
			return;

		case 4:
			switch (gamemap)
			{
			case 6:
				junk.tag = 666;
				EV_DoDoor(&junk, vldoor_e::vld_blazeOpen);
				return;

			case 8:
				junk.tag = 666;
				EV_DoFloor(&junk, floor_e::lowerFloorToLowest);
				return;

			default:
				break;
			}
			break;

		default:
			break;
		}
	}

	G_ExitLevel();
}

void A_Hoof(MapObject* mo)
{
	S_StartSound(mo, sfxenum_t::sfx_hoof);
	A_Chase(mo);
}

void A_Metal(MapObject* mo)
{
	S_StartSound(mo, sfxenum_t::sfx_metal);
	A_Chase(mo);
}

void A_BabyMetal(MapObject* mo)
{
	S_StartSound(mo, sfxenum_t::sfx_bspwlk);
	A_Chase(mo);
}

void A_OpenShotgun2(MapObject* mobj, Player* player, pspdef_t* psp)
{
	if (!player)
	{
		return; // let pspr action pointers get called from mobj states
	}
	S_StartSound(player->so, sfxenum_t::sfx_dbopn); // weapon sound source
}

void A_LoadShotgun2(MapObject* mobj, Player* player, pspdef_t* psp)
{
	if (!player)
	{
		return; // let pspr action pointers get called from mobj states
	}
	S_StartSound(player->so, sfxenum_t::sfx_dbload); // weapon sound source
}

void A_ReFire(MapObject* mobj, Player* player, pspdef_t* psp);

void A_CloseShotgun2(MapObject* mobj, Player* player, pspdef_t* psp)
{
	if (!player)
	{
		return; // let pspr action pointers get called from mobj states
	}
	S_StartSound(player->so, sfxenum_t::sfx_dbcls); // weapon sound source
	A_ReFire(NULL, player, psp); // let pspr action pointers get called from mobj states
}

MapObject** braintargets = NULL;
int numbraintargets = 0; // initialize
int braintargeton = 0;
static int maxbraintargets; // remove braintargets limit

void A_BrainAwake(MapObject* mo)
{
	thinker_t* thinker;
	MapObject* m;

	// find all the target spots
	numbraintargets = 0;
	braintargeton = 0;

	for (thinker = thinkercap.next; thinker != &thinkercap; thinker = thinker->next)
	{
		if (thinker->function.acp1 != (actionf_p1)P_MobjThinker)
		{
			continue;	// not a mobj
		}

		m = (MapObject*)thinker;

		if (m->type == mobjtype_t::MT_BOSSTARGET)
		{
			// remove braintargets limit
			if (numbraintargets == maxbraintargets)
			{
				maxbraintargets = maxbraintargets ? 2 * maxbraintargets : 32;
				braintargets = I_Realloc(braintargets, maxbraintargets * sizeof(*braintargets));

				if (maxbraintargets > 32)
				{
					fprintf(stderr, "R_BrainAwake: Raised braintargets limit to %d.\n", maxbraintargets);
				}
			}

			braintargets[numbraintargets] = m;
			++numbraintargets;
		}
	}

	S_StartSound(nullptr, sfxenum_t::sfx_bossit);

	// prevent braintarget overflow
	// (e.g. in two subsequent maps featuring a brain spitter)
	if (braintargeton >= numbraintargets)
	{
		braintargeton = 0;
	}

	// no spawn spots available
	if (numbraintargets == 0)
	{
		numbraintargets = -1;
	}
}

void A_BrainPain(MapObject* mo)
{
	// prevent from adding up volume
	crispy->soundfull ? S_StartSoundOnce(nullptr, sfxenum_t::sfx_bospn) : S_StartSound(nullptr, sfxenum_t::sfx_bospn);
}

void A_BrainScream(MapObject* mo)
{
	int x;
	int y;
	int z;
	MapObject* th;

	for (x = mo->x - 196 * FRACUNIT; x < mo->x + 320 * FRACUNIT; x += FRACUNIT * 8)
	{
		y = mo->y - 320 * FRACUNIT;
		z = 128 + P_Random() * 2 * FRACUNIT;
		th = P_SpawnMobj(x, y, z, mobjtype_t::MT_ROCKET);
		th->momz = P_Random() * 512;

		P_SetMobjState(th, statenum_t::S_BRAINEXPLODE1);

		th->tics -= P_Random() & 7;
		if (th->tics < 1)
		{
			th->tics = 1;
		}
	}

	S_StartSound(nullptr, sfxenum_t::sfx_bosdth);
}

void A_BrainExplode(MapObject* mo)
{
	int x;
	int y;
	int z;
	MapObject* th;

	x = mo->x + P_SubRandom() * 2048;
	y = mo->y;
	z = 128 + P_Random() * 2 * FRACUNIT;
	th = P_SpawnMobj(x, y, z, mobjtype_t::MT_ROCKET);
	th->momz = P_Random() * 512;

	P_SetMobjState(th, statenum_t::S_BRAINEXPLODE1);

	th->tics -= P_Random() & 7;
	if (th->tics < 1)
	{
		th->tics = 1;
	}

	// brain explosions are translucent
	th->flags |= mobjflag_e::MF_TRANSLUCENT;
}

void A_BrainDie(MapObject* mo)
{
	G_ExitLevel();
}

void A_BrainSpit(MapObject* mo)
{
	MapObject* targ;
	MapObject* newmobj;

	static int easy = 0;

	easy ^= 1;
	if (gameskill <= SkillType::sk_easy && (!easy))
	{
		return;
	}

	// avoid division by zero by recalculating the number of spawn spots
	if (numbraintargets == 0)
	{
		A_BrainAwake(nullptr);
	}

	// still no spawn spots available
	if (numbraintargets == -1)
	{
		return;
	}

	// shoot a cube at current target
	targ = braintargets[braintargeton];
	if (numbraintargets == 0)
	{
		I_Error("A_BrainSpit: numbraintargets was 0 (vanilla crashes here)");
	}
	braintargeton = (braintargeton + 1) % numbraintargets;

	// spawn brain missile
	newmobj = P_SpawnMissile(mo, targ, mobjtype_t::MT_SPAWNSHOT);
	newmobj->target = targ;
	newmobj->reactiontime =
		((targ->y - mo->y) / newmobj->momy) / newmobj->state->tics;

	S_StartSound(nullptr, sfxenum_t::sfx_bospit);
}

void A_SpawnFly(MapObject* mo);

// travelling cube sound
void A_SpawnSound(MapObject* mo)
{
	S_StartSound(mo, sfxenum_t::sfx_boscub);
	A_SpawnFly(mo);
}

void A_SpawnFly(MapObject* mo)
{
	MapObject* newmobj;
	MapObject* fog;
	MapObject* targ;
	int r;
	mobjtype_t type;

	if (--mo->reactiontime)
	{
		return;	// still flying
	}

	targ = P_SubstNullMobj(mo->target);

	// First spawn teleport fog.
	fog = P_SpawnMobj(targ->x, targ->y, targ->z, mobjtype_t::MT_SPAWNFIRE);
	S_StartSound(fog, sfxenum_t::sfx_telept);

	// Randomly select monster to spawn.
	r = P_Random();

	// Probability distribution (kind of :),
	// decreasing likelihood.
	if (r < 50)
	{
		type = mobjtype_t::MT_TROOP;
	}
	else if (r < 90)
	{
		type = mobjtype_t::MT_SERGEANT;
	}
	else if (r < 120)
	{
		type = mobjtype_t::MT_SHADOWS;
	}
	else if (r < 130)
	{
		type = mobjtype_t::MT_PAIN;
	}
	else if (r < 160)
	{
		type = mobjtype_t::MT_HEAD;
	}
	else if (r < 162)
	{
		type = mobjtype_t::MT_VILE;
	}
	else if (r < 172)
	{
		type = mobjtype_t::MT_UNDEAD;
	}
	else if (r < 192)
	{
		type = mobjtype_t::MT_BABY;
	}
	else if (r < 222)
	{
		type = mobjtype_t::MT_FATSO;
	}
	else if (r < 246)
	{
		type = mobjtype_t::MT_KNIGHT;
	}
	else
	{
		type = mobjtype_t::MT_BRUISER;
	}

	newmobj = P_SpawnMobj(targ->x, targ->y, targ->z, type);

	// count spawned monsters
	++extrakills;

	if (P_LookForPlayers(newmobj, true))
	{
		P_SetMobjState(newmobj, newmobj->info->seestate);
	}

	// telefrag anything in this spot
	P_TeleportMove(newmobj, newmobj->x, newmobj->y);

	// remove self (i.e., cube).
	P_RemoveMobj(mo);
}

void A_PlayerScream(MapObject* mo)
{
	// Default death sound.
	sfxenum_t sound = sfxenum_t::sfx_pldeth;

	if ((gamemode == GameMode::commercial) && (mo->health < -50))
	{
		// IF THE PLAYER DIES
		// LESS THAN -50% WITHOUT GIBBING
		sound = sfxenum_t::sfx_pdiehi;
	}

	S_StartSound(mo, sound);
}
