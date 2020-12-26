/**********************************************************************************************************************************************\
	Copyright(C) 1993-1996 Id Software, Inc.
	Copyright(C) 2005-2014 Simon Howard

	This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License
	as published by the Free Software Foundation; either version 2 of the License, or (at your option) any later version.

	This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

	DESCRIPTION:
	Teleportation.
\**********************************************************************************************************************************************/

#include "doomdef.h"
#include "doomstat.h"
#include "s_sound.h"
#include "p_local.h"
#include "sounds.h"
#include "r_state.h"

bool EV_Teleport(line_t* line, int side, mobj_t* thing)
{
	// don't teleport missiles
	if (thing->flags & MF_MISSILE)
	{
		return false;
	}

	// Don't teleport if hit back of line, so you can get out of teleporter.
	if (side == 1)
	{
		return false;
	}

	auto tag{line->tag};
	for (size_t i{0ull}; i < numsectors; ++i)
	{
		if (sectors[i].tag == tag)
		{
			for (auto thinker{thinkercap.next}; thinker != &thinkercap; thinker = thinker->next)
			{
				// not a mobj
				if (thinker->function.acp1 != (actionf_p1)P_MobjThinker)
				{
					continue;
				}

				auto m{(mobj_t*)thinker};

				// not a teleportman
				if (m->type != mobjtype_t::MT_TELEPORTMAN)
				{
					continue;
				}

				auto sector{m->subsector->sector};
				// wrong sector
				if (sector-sectors != i)
				{
					continue;
				}

				auto oldx{thing->x};
				auto oldy{thing->y};
				auto oldz{thing->z};

				if (!P_TeleportMove (thing, m->x, m->y))
				{
					return false;
				}

				// The first Final Doom executable does not set thing->z
				// when teleporting. This quirk is unique to this
				// particular version; the later version included in
				// some versions of the Id Anthology fixed this.

				if (gameversion != GameVersion_t::exe_final)
				{
					thing->z = thing->floorz;
				}

				if (thing->player)
				{
					thing->player->viewz = thing->z+thing->player->viewheight;
					// [crispy] center view after teleporting
					thing->player->centering = true;
				}

				// spawn teleport fog at source and destination
				auto fog{P_SpawnMobj(oldx, oldy, oldz, mobjtype_t::MT_TFOG)};
				S_StartSound(fog, sfx_telept);
				auto an{m->angle >> ANGLETOFINESHIFT};
				fog = P_SpawnMobj(m->x+20*finecosine[an], m->y+20*finesine[an], thing->z, mobjtype_t::MT_TFOG);

				// emit sound, where?
				S_StartSound(fog, sfx_telept);

				// don't move for a bit
				if (thing->player)
				{
					thing->reactiontime = 18;
				}

				thing->angle = m->angle;
				thing->momx = thing->momy = thing->momz = 0;
				return true;
			}
		}
	}

	return false;
}
