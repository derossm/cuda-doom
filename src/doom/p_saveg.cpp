/**********************************************************************************************************************************************\
	Copyright(C) 1993-1996 Id Software, Inc.
	Copyright(C) 2005-2014 Simon Howard

	This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License
	as published by the Free Software Foundation; either version 2 of the License, or (at your option) any later version.

	This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

	DESCRIPTION:
		Archiving: SaveGame I/O.
\**********************************************************************************************************************************************/

#include <string>

#include "dstrings.h"
#include "deh_main.h"
#include "i_system.h"
#include "z_zone.h"
#include "p_local.h"
#include "p_saveg.h"

// State.
#include "doomstat.h"
#include "g_game.h"
#include "m_misc.h"
#include "r_state.h"

FILE* save_stream;
int savegamelength;
bool savegame_error;
static int restoretargets_fail;

// Get the filename of a temporary file to write the savegame to. After the file has been successfully saved, it will be renamed to the real file.
::std::string P_TempSaveGameFile()
{
	return ::std::string(savegamedir + "temp.dsg");
}

// Get the filename of the save game file to use for the specified slot.
::std::string P_SaveGameFile(int slot)
{
	//static size_t filename_size = 0;
	//char basename[32];

	// FIXME
	::std::string filename;
	//DEH_snprintf(basename, 32, SAVEGAMENAME "%d.dsg", slot);
	//M_snprintf(filename, filename_size, "%s%s", savegamedir, basename);

	return filename;
}

// Endian-safe integer read/write functions
static byte saveg_read8()
{
	byte result = -1;

	if (fread(&result, 1, 1, save_stream) < 1)
	{
		if (!savegame_error)
		{
			fprintf(stderr, "saveg_read8: Unexpected end of file while reading save game\n");

			savegame_error = true;
		}
	}

	return result;
}

static void saveg_write8(byte value)
{
	if (fwrite(&value, 1, 1, save_stream) < 1)
	{
		if (!savegame_error)
		{
			fprintf(stderr, "saveg_write8: Error while writing save game\n");

			savegame_error = true;
		}
	}
}

static short saveg_read16()
{
	int result{saveg_read8()};
	result |= saveg_read8() << 8;

	return result;
}

static void saveg_write16(short value)
{
	saveg_write8(value & 0xff);
	saveg_write8((value >> 8) & 0xff);
}

static int saveg_read32()
{
	int result;

	result = saveg_read8();
	result |= saveg_read8() << 8;
	result |= saveg_read8() << 16;
	result |= saveg_read8() << 24;

	return result;
}

static void saveg_write32(int value)
{
	saveg_write8(value & 0xff);
	saveg_write8((value >> 8) & 0xff);
	saveg_write8((value >> 16) & 0xff);
	saveg_write8((value >> 24) & 0xff);
}

// Pad to 4-byte boundaries

static void saveg_read_pad()
{
	unsigned long pos{ftell(save_stream)};

	int padding{(4 - (pos & 3)) & 3};

	for (size_t i{0}; i < padding; ++i)
	{
		saveg_read8();
	}
}

static void saveg_write_pad()
{
	unsigned long pos{ftell(save_stream)};

	int padding{(4 - (pos & 3)) & 3};

	for (size_t i{0}; i < padding; ++i)
	{
		saveg_write8(0);
	}
}

static void* saveg_readp()
{
	return (void*)(intptr_t)saveg_read32();
}

static void saveg_writep(const void* p)
{
	saveg_write32((intptr_t)p);
}

// Enum values are 32-bit integers.
#define saveg_read_enum saveg_read32
#define saveg_write_enum saveg_write32

//
// Structure read/write functions
//

static void saveg_read_mapthing_t(mapthing_t* str)
{
	str->x = saveg_read16();
	str->y = saveg_read16();
	str->angle = saveg_read16();
	str->type = saveg_read16();
	str->options = saveg_read16();
}

static void saveg_write_mapthing_t(mapthing_t* str)
{
	saveg_write16(str->x);
	saveg_write16(str->y);
	saveg_write16(str->angle);
	saveg_write16(str->type);
	saveg_write16(str->options);
}

static void saveg_read_actionf_t(actionf_t* str)
{
	str->acp1 = saveg_readp();
}

static void saveg_write_actionf_t(actionf_t* str)
{
	saveg_writep(str->acp1);
}

#define saveg_read_think_t saveg_read_actionf_t
#define saveg_write_think_t saveg_write_actionf_t

static void saveg_read_thinker_t(thinker_t* str)
{
	str->prev = saveg_readp();
	str->next = saveg_readp();
	saveg_read_think_t(&str->function);
}

static void saveg_write_thinker_t(thinker_t* str)
{
	saveg_writep(str->prev);
	saveg_writep(str->next);
	saveg_write_think_t(&str->function);
}

static void saveg_read_mobj_t(MapObject* str)
{
	saveg_read_thinker_t(&str->thinker);
	str->x = saveg_read32();
	str->y = saveg_read32();
	str->z = saveg_read32();
	str->sectorNext = saveg_readp();
	str->sectorPrev = saveg_readp();
	str->angle = saveg_read32();
	str->sprite = saveg_read_enum();
	str->frame = saveg_read32();
	str->blockNext = saveg_readp();
	str->blockPrev = saveg_readp();
	str->subsector = saveg_readp();
	str->floorz = saveg_read32();
	str->ceilingz = saveg_read32();
	str->radius = saveg_read32();
	str->height = saveg_read32();
	str->momx = saveg_read32();
	str->momy = saveg_read32();
	str->momz = saveg_read32();
	str->validcount = saveg_read32();
	str->type = saveg_read_enum();
	str->info = saveg_readp();
	str->tics = saveg_read32();
	str->state = &states[saveg_read32()];
	str->flags = saveg_read32();
	str->health = saveg_read32();
	str->movedir = saveg_read32();
	str->movecount = saveg_read32();
	str->target = saveg_readp();
	str->reactiontime = saveg_read32();
	str->threshold = saveg_read32();

	if (auto pl{saveg_read32()}; pl > 0)
	{
		// FIXME
		//str->player = &players[pl - 1];
		//str->player = str;
		//str->player->so = Crispy_PlayerSO(pl - 1); // weapon sound sources
	}
	else
	{
		//str->player = nullptr;
	}

	str->lastlook = saveg_read32();
	saveg_read_mapthing_t(&str->spawnpoint);
	str->tracer = saveg_readp();
}

// enumerate all thinker pointers
uint32_t P_ThinkerToIndex(thinker_t* thinker)
{
	if (!thinker)
	{
		return 0;
	}

	size_t i{0};
	for (auto th{thinkercap.next}; th != &thinkercap; th = th->next)
	{
		if (th->function.acp1 == (actionf_p1)P_MobjThinker)
		{
			++i;
			if (th == thinker)
			{
				return i;
			}
		}
	}

	return 0;
}

// replace indizes with corresponding pointers
thinker_t* P_IndexToThinker(uint32_t index)
{
	if (!index)
	{
		return nullptr;
	}

	size_t i{0};
	for (auto th{thinkercap.next}; th != &thinkercap; th = th->next)
	{
		if (th->function.acp1 == (actionf_p1)P_MobjThinker)
		{
			++i;
			if (i == index)
			{
				return th;
			}
		}
	}

	++restoretargets_fail;

	return nullptr;
}

static void saveg_write_mobj_t(MapObject* str)
{
	saveg_write_thinker_t(&str->thinker);
	saveg_write32(str->x);
	saveg_write32(str->y);
	saveg_write32(str->z);
	saveg_writep(str->sectorNext);
	saveg_writep(str->sectorPrev);
	saveg_write32(str->angle);
	saveg_write_enum(str->sprite);
	saveg_write32(str->frame);
	saveg_writep(str->blockNext);
	saveg_writep(str->blockPrev);
	saveg_writep(str->subsector);
	saveg_write32(str->floorz);
	saveg_write32(str->ceilingz);
	saveg_write32(str->radius);
	saveg_write32(str->height);
	saveg_write32(str->momx);
	saveg_write32(str->momy);
	saveg_write32(str->momz);
	saveg_write32(str->validcount);
	saveg_write_enum(str->type);
	saveg_writep(str->info);
	saveg_write32(str->tics);
	saveg_write32(str->state - states);
	saveg_write32(str->flags);
	saveg_write32(str->health);
	saveg_write32(str->movedir);
	saveg_write32(str->movecount);
	// instead of the actual pointer, store the corresponding index in the mobj->target field
	saveg_writep((void*)(uintptr_t)P_ThinkerToIndex((thinker_t*)str->target));
	saveg_write32(str->reactiontime);
	saveg_write32(str->threshold);

	if (str->player)
	{
		saveg_write32(str->player - players + 1);
	}
	else
	{
		saveg_write32(0);
	}

	saveg_write32(str->lastlook);
	saveg_write_mapthing_t(&str->spawnpoint);
	// instead of the actual pointer, store the corresponding index in the mobj->tracers field
	saveg_writep((void*)(uintptr_t)P_ThinkerToIndex((thinker_t*)str->tracer));
}

static void saveg_read_ticcmd_t(ticcmd_t* str)
{
	str->forwardmove = saveg_read8();
	str->sidemove = saveg_read8();
	str->angleturn = saveg_read16();
	str->consistancy = saveg_read16();
	str->chatchar = saveg_read8();
	str->buttons = saveg_read8();
}

static void saveg_write_ticcmd_t(ticcmd_t* str)
{
	saveg_write8(str->forwardmove);
	saveg_write8(str->sidemove);
	saveg_write16(str->angleturn);
	saveg_write16(str->consistancy);
	saveg_write8(str->chatchar);
	saveg_write8(str->buttons);
}

static void saveg_read_pspdef_t(pspdef_t* str)
{
	if (auto state{saveg_read32()}; state > 0)
	{
		str->state = &states[state];
	}
	else
	{
		str->state = NULL;
	}

	str->tics = saveg_read32();
	str->sx = saveg_read32();
	str->sy = saveg_read32();
	str->dy = 0;
	str->sx2 = str->sx;
	str->sy2 = str->sy;
}

static void saveg_write_pspdef_t(pspdef_t* str)
{
	if (str->state)
	{
		saveg_write32(str->state - states);
	}
	else
	{
		saveg_write32(0);
	}

	saveg_write32(str->tics);
	saveg_write32(str->sx);
	saveg_write32(str->sy);
}

static void saveg_read_Player(Player* str)
{
	// MapObject* mo;
	// FIXME
	//str->mo = saveg_readp();

	str->playerstate = saveg_read_enum();
	saveg_read_ticcmd_t(&str->cmd);
	str->viewz = saveg_read32();
	str->viewheight = saveg_read32();
	str->deltaviewheight = saveg_read32();
	str->bob = saveg_read32();
	str->bob2 = str->bob;
	str->health = saveg_read32();
	str->neghealth = str->health;
	str->armorpoints = saveg_read32();
	str->armortype = saveg_read32();

	for (size_t i{0}; i < PowerType_t::NUMPOWERS; ++i)
	{
		str->powers[i] = saveg_read32();
	}

	for (size_t i{0}; i < CardType::NUMCARDS; ++i)
	{
		str->cards[i] = saveg_read32();
	}

	str->backpack = saveg_read32();

	for (size_t i{0}; i < MAX_PLAYERS; ++i)
	{
		str->frags[i] = saveg_read32();
	}

	str->readyweapon = saveg_read_enum();
	str->pendingweapon = saveg_read_enum();

	for (size_t i{0}; i < WeaponType::NUMWEAPONS; ++i)
	{
		str->weaponowned[i] = saveg_read32();
	}

	for (size_t i{0}; i < AmmoType::NUMAMMO; ++i)
	{
		str->ammo[i] = saveg_read32();
	}

	for (size_t i{0}; i < AmmoType::NUMAMMO; ++i)
	{
		str->maxammo[i] = saveg_read32();
	}

	str->attackdown = saveg_read32();
	str->usedown = saveg_read32();
	str->cheats = saveg_read32();
	str->refire = saveg_read32();
	str->killcount = saveg_read32();
	str->itemcount = saveg_read32();
	str->secretcount = saveg_read32();
	str->message = saveg_readp();
	str->damagecount = saveg_read32();
	str->bonuscount = saveg_read32();
	str->attacker = saveg_readp();
	str->extralight = saveg_read32();
	str->fixedcolormap = saveg_read32();
	str->colormap = saveg_read32();

	for (size_t i{0}; i < psprnum_t::NUMPSPRITES; ++i)
	{
		saveg_read_pspdef_t(&str->psprites[i]);
	}

	str->didsecret = saveg_read32();
}

static void saveg_write_Player(Player* str)
{
	// MapObject* mo;
	// FIXME
	//saveg_writep(str->mo);

	// PlayerState_t playerstate;
	saveg_write_enum(str->playerstate);
	saveg_write_ticcmd_t(&str->cmd);
	saveg_write32(str->viewz);
	saveg_write32(str->viewheight);
	saveg_write32(str->deltaviewheight);
	saveg_write32(str->bob);
	saveg_write32(str->health);
	saveg_write32(str->armorpoints);
	saveg_write32(str->armortype);

	for (size_t i{0}; i < PowerType_t::NUMPOWERS; ++i)
	{
		saveg_write32(str->powers[i]);
	}

	for (size_t i{0}; i < CardType::NUMCARDS; ++i)
	{
		saveg_write32(str->cards[i]);
	}

	saveg_write32(str->backpack);

	for (size_t i{0}; i < MAX_PLAYERS; ++i)
	{
		saveg_write32(str->frags[i]);
	}

	saveg_write_enum(str->readyweapon);
	saveg_write_enum(str->pendingweapon);

	for (size_t i{0}; i < WeaponType::NUMWEAPONS; ++i)
	{
		saveg_write32(str->weaponowned[i]);
	}

	for (size_t i{0}; i < AmmoType::NUMAMMO; ++i)
	{
		saveg_write32(str->ammo[i]);
	}

	for (size_t i{0}; i < AmmoType::NUMAMMO; ++i)
	{
		saveg_write32(str->maxammo[i]);
	}

	saveg_write32(str->attackdown);
	saveg_write32(str->usedown);
	saveg_write32(str->cheats);
	saveg_write32(str->refire);
	saveg_write32(str->killcount);
	saveg_write32(str->itemcount);
	saveg_write32(str->secretcount);
	saveg_writep(str->message);
	saveg_write32(str->damagecount);
	saveg_write32(str->bonuscount);
	saveg_writep(str->attacker);
	saveg_write32(str->extralight);
	saveg_write32(str->fixedcolormap);
	saveg_write32(str->colormap);

	for (size_t i{0}; i < psprnum_t::NUMPSPRITES; ++i)
	{
		saveg_write_pspdef_t(&str->psprites[i]);
	}

	saveg_write32(str->didsecret);
}

static void saveg_read_ceiling_t(ceiling_t* str)
{
	saveg_read_thinker_t(&str->thinker);

	str->type = saveg_read_enum();

	auto sector{saveg_read32()};
	str->sector = &sectors[sector];
	str->bottomheight = saveg_read32();
	str->topheight = saveg_read32();
	str->speed = saveg_read32();
	str->crush = saveg_read32();
	str->direction = saveg_read32();
	str->tag = saveg_read32();
	str->olddirection = saveg_read32();
}

static void saveg_write_ceiling_t(ceiling_t* str)
{
	saveg_write_thinker_t(&str->thinker);

	saveg_write_enum(str->type);
	saveg_write32(str->sector - sectors);
	saveg_write32(str->bottomheight);
	saveg_write32(str->topheight);
	saveg_write32(str->speed);
	saveg_write32(str->crush);
	saveg_write32(str->direction);
	saveg_write32(str->tag);
	saveg_write32(str->olddirection);
}

static void saveg_read_vldoor_t(vldoor_t* str)
{
	saveg_read_thinker_t(&str->thinker);

	str->type = saveg_read_enum();

	auto sector{saveg_read32()};
	str->sector = &sectors[sector];
	str->topheight = saveg_read32();
	str->speed = saveg_read32();
	str->direction = saveg_read32();
	str->topwait = saveg_read32();
	str->topcountdown = saveg_read32();
}

static void saveg_write_vldoor_t(vldoor_t* str)
{
	saveg_write_thinker_t(&str->thinker);

	saveg_write_enum(str->type);
	saveg_write32(str->sector - sectors);
	saveg_write32(str->topheight);
	saveg_write32(str->speed);
	saveg_write32(str->direction);
	saveg_write32(str->topwait);
	saveg_write32(str->topcountdown);
}

static void saveg_read_floormove_t(floormove_t* str)
{
	saveg_read_thinker_t(&str->thinker);

	str->type = saveg_read_enum();
	str->crush = saveg_read32();

	auto sector{saveg_read32()};
	str->sector = &sectors[sector];
	str->direction = saveg_read32();
	str->newspecial = saveg_read32();
	str->texture = saveg_read16();
	str->floordestheight = saveg_read32();
	str->speed = saveg_read32();
}

static void saveg_write_floormove_t(floormove_t* str)
{
	saveg_write_thinker_t(&str->thinker);

	saveg_write_enum(str->type);
	saveg_write32(str->crush);
	saveg_write32(str->sector - sectors);
	saveg_write32(str->direction);
	saveg_write32(str->newspecial);
	saveg_write16(str->texture);
	saveg_write32(str->floordestheight);
	saveg_write32(str->speed);
}

static void saveg_read_plat_t(plat_t* str)
{
	saveg_read_thinker_t(&str->thinker);

	auto sector{saveg_read32()};
	str->sector = &sectors[sector];
	str->speed = saveg_read32();
	str->low = saveg_read32();
	str->high = saveg_read32();
	str->wait = saveg_read32();
	str->count = saveg_read32();
	str->status = saveg_read_enum();
	str->oldstatus = saveg_read_enum();
	str->crush = saveg_read32();
	str->tag = saveg_read32();
	str->type = saveg_read_enum();
}

static void saveg_write_plat_t(plat_t* str)
{
	saveg_write_thinker_t(&str->thinker);

	saveg_write32(str->sector - sectors);
	saveg_write32(str->speed);
	saveg_write32(str->low);
	saveg_write32(str->high);
	saveg_write32(str->wait);
	saveg_write32(str->count);
	saveg_write_enum(str->status);
	saveg_write_enum(str->oldstatus);
	saveg_write32(str->crush);
	saveg_write32(str->tag);
	saveg_write_enum(str->type);
}

static void saveg_read_lightflash_t(lightflash_t* str)
{
	saveg_read_thinker_t(&str->thinker);

	auto sector{saveg_read32()};
	str->sector = &sectors[sector];
	str->count = saveg_read32();
	str->maxlight = saveg_read32();
	str->minlight = saveg_read32();
	str->maxtime = saveg_read32();
	str->mintime = saveg_read32();
}

static void saveg_write_lightflash_t(lightflash_t* str)
{
	saveg_write_thinker_t(&str->thinker);

	saveg_write32(str->sector - sectors);
	saveg_write32(str->count);
	saveg_write32(str->maxlight);
	saveg_write32(str->minlight);
	saveg_write32(str->maxtime);
	saveg_write32(str->mintime);
}

static void saveg_read_strobe_t(strobe_t* str)
{
	saveg_read_thinker_t(&str->thinker);

	auto sector{saveg_read32()};
	str->sector = &sectors[sector];
	str->count = saveg_read32();
	str->minlight = saveg_read32();
	str->maxlight = saveg_read32();
	str->darktime = saveg_read32();
	str->brighttime = saveg_read32();
}

static void saveg_write_strobe_t(strobe_t* str)
{
	saveg_write_thinker_t(&str->thinker);

	saveg_write32(str->sector - sectors);
	saveg_write32(str->count);
	saveg_write32(str->minlight);
	saveg_write32(str->maxlight);
	saveg_write32(str->darktime);
	saveg_write32(str->brighttime);
}

static void saveg_read_glow_t(glow_t* str)
{
	saveg_read_thinker_t(&str->thinker);

	auto sector{saveg_read32()};
	str->sector = &sectors[sector];
	str->minlight = saveg_read32();
	str->maxlight = saveg_read32();
	str->direction = saveg_read32();
}

static void saveg_write_glow_t(glow_t* str)
{
	saveg_write_thinker_t(&str->thinker);

	saveg_write32(str->sector - sectors);
	saveg_write32(str->minlight);
	saveg_write32(str->maxlight);
	saveg_write32(str->direction);
}

void P_WriteSaveGameHeader(::std::string description)
{
	char name[VERSIONSIZE];

	{
		size_t i{0};
		for (; description[i] != '\0'; ++i)
		{
			saveg_write8(description[i]);
		}

		for (; i < SAVESTRINGSIZE; ++i)
		{
			saveg_write8(0);
		}
	}

	memset(name, 0, sizeof(name));
	M_snprintf(name, sizeof(name), "version %i", G_VanillaVersionCode());

	for (size_t i{0}; i < VERSIONSIZE; ++i)
	{
		saveg_write8(name[i]);
	}

	saveg_write8((byte)gameskill);
	saveg_write8(gameepisode);
	saveg_write8(gamemap);

	for (size_t i{0}; i < MAX_PLAYERS; ++i)
	{
		saveg_write8(playeringame[i]);
	}

	saveg_write8((leveltime >> 16) & 0xff);
	saveg_write8((leveltime >> 8) & 0xff);
	saveg_write8(leveltime & 0xff);
}

bool P_ReadSaveGameHeader()
{
	// skip the description field
	for (size_t i{0}; i < SAVESTRINGSIZE; ++i)
	{
		saveg_read8();
	}

	char read_vcheck[VERSIONSIZE];
	for (size_t i{0}; i < VERSIONSIZE; ++i)
	{
		read_vcheck[i] = saveg_read8();
	}

	char vcheck[VERSIONSIZE];
	memset(vcheck, 0, sizeof(vcheck));
	M_snprintf(vcheck, sizeof(vcheck), "version %i", G_VanillaVersionCode());

	if (strcmp(read_vcheck, vcheck) != 0)
	{
		return false;				// bad version
	}

	gameskill = (SkillType)saveg_read8();
	gameepisode = saveg_read8();
	gamemap = saveg_read8();

	for (size_t i{0}; i < MAX_PLAYERS; ++i)
	{
		playeringame[i] = saveg_read8();
	}

	// get the times
	auto a{saveg_read8()};
	auto b{saveg_read8()};
	auto c{saveg_read8()};
	leveltime = (a << 16) + (b << 8) + c;

	return true;
}

bool P_ReadSaveGameEOF()
{
	return (int)saveg_read8() == SAVEGAME_EOF;
}

void P_WriteSaveGameEOF()
{
	saveg_write8(SAVEGAME_EOF);
}

void P_ArchivePlayers()
{
	for (size_t i{0}; i < MAX_PLAYERS; ++i)
	{
		if (!playeringame[i])
		{
			continue;
		}

		saveg_write_pad();

		saveg_write_Player(&players[i]);
	}
}

void P_UnArchivePlayers()
{
	for (size_t i{0}; i < MAX_PLAYERS; ++i)
	{
		if (!playeringame[i])
		{
			continue;
		}

		saveg_read_pad();

		saveg_read_Player(&players[i]);

		// will be set when unarc thinker
		players[i].message.clear();
		players[i].attacker = nullptr;
	}
}

void P_ArchiveWorld()
{
	// do sectors
	auto sec{sectors};
	for (size_t i{0}; i < numsectors; ++i, ++sec)
	{
		saveg_write16(sec->floorheight >> FRACBITS);
		saveg_write16(sec->ceilingheight >> FRACBITS);
		saveg_write16(sec->floorpic);
		saveg_write16(sec->ceilingpic);
		saveg_write16(sec->lightlevel);
		saveg_write16(sec->special);		// needed?
		saveg_write16(sec->tag);		// needed?
	}

	// do lines
	auto li{lines};
	for (size_t i{0}; i < numlines; ++i, ++li)
	{
		saveg_write16(li->flags);
		saveg_write16(li->special);
		saveg_write16(li->tag);
		for (size_t j{0}; j < 2; ++j)
		{
			if (li->sidenum[j] == NO_INDEX)
			{
				// extended nodes
				continue;
			}

			auto si = &sides[li->sidenum[j]];
			saveg_write16(si->textureoffset >> FRACBITS);
			saveg_write16(si->rowoffset >> FRACBITS);
			saveg_write16(si->toptexture);
			saveg_write16(si->bottomtexture);
			saveg_write16(si->midtexture);
		}
	}
}

void P_UnArchiveWorld()
{
	// add overflow guard for the flattranslation[] array
	extern int numflats;

	// do sectors
	auto sec{sectors};
	for (size_t i{0}; i < numsectors; ++i, ++sec)
	{
		sec->floorheight = saveg_read16() << FRACBITS;
		sec->ceilingheight = saveg_read16() << FRACBITS;

		auto floorpic = saveg_read16();
		auto ceilingpic = saveg_read16();

		sec->lightlevel = saveg_read16();
		sec->special = saveg_read16();		// needed?
		sec->tag = saveg_read16();			// needed?
		sec->specialdata = 0;
		sec->soundtarget = 0;

		// add overflow guard for the flattranslation[] array
		if (floorpic >= 0 && floorpic < numflats)
		{
			sec->floorpic = floorpic;
		}
		if (ceilingpic >= 0 && ceilingpic < numflats)
		{
			sec->ceilingpic = ceilingpic;
		}
	}

	// do lines
	auto li{lines};
	for (size_t i{0}; i < numlines; ++i, ++li)
	{
		li->flags = saveg_read16();
		li->special = saveg_read16();
		li->tag = saveg_read16();
		for (size_t j{0}; j < 2; ++j)
		{
			if (li->sidenum[j] == NO_INDEX)
			{
				// extended nodes
				continue;
			}

			auto si{&sides[li->sidenum[j]]};
			si->textureoffset = saveg_read16() << FRACBITS;
			si->rowoffset = saveg_read16() << FRACBITS;
			si->toptexture = saveg_read16();
			si->bottomtexture = saveg_read16();
			si->midtexture = saveg_read16();
		}
	}
}

enum class thinkerclass_e
{
	tc_end,
	tc_mobj

};

void P_ArchiveThinkers()
{
	// save off the current thinkers
	for (auto th{thinkercap.next}; th != &thinkercap; th = th->next)
	{
		if (th->function.acp1 == (actionf_p1)P_MobjThinker)
		{
			saveg_write8((byte)thinkerclass_e::tc_mobj);
			saveg_write_pad();
			saveg_write_mobj_t((MapObject*)th);

			continue;
		}

		// I_Error("P_ArchiveThinkers: Unknown thinker function");
	}

	// add a terminating marker
	saveg_write8((byte)thinkerclass_e::tc_end);
}

void P_UnArchiveThinkers()
{
	byte tclass;
	thinker_t* currentthinker;
	thinker_t* next;
	MapObject* mobj;

	// remove all the current thinkers
	currentthinker = thinkercap.next;
	while (currentthinker != &thinkercap)
	{
		next = currentthinker->next;

		if (currentthinker->function.acp1 == (actionf_p1)P_MobjThinker)
		{
			P_RemoveMobj((MapObject*)currentthinker);
		}
		else
		{
			Z_Free(currentthinker);
		}

		currentthinker = next;
	}
	P_InitThinkers();

	// read in saved thinkers
	while (1)
	{
		tclass = saveg_read8();
		switch (tclass)
		{
		case thinkerclass_e::tc_end:
			return;	// end of list

		case thinkerclass_e::tc_mobj:
			saveg_read_pad();
			mobj = Z_Malloc<decltype(*mobj)>(sizeof(*mobj), pu_tags_t::PU_LEVEL, NULL);
			saveg_read_mobj_t(mobj);

			// restore mobj->target and mobj->tracer fields
			//mobj->target = NULL;
			//mobj->tracer = NULL;
			P_SetThingPosition(mobj);
			mobj->info = &mobjinfo[::std::size_t(mobj->type)];
			// Fix for falling down into a wall after savegame loaded
			//mobj->floorz = mobj->subsector->sector->floorheight;
			//mobj->ceilingz = mobj->subsector->sector->ceilingheight;
			mobj->thinker.function.acp1 = (actionf_p1)P_MobjThinker;
			P_AddThinker(&mobj->thinker);
			break;

		default:
			I_Error("Unknown tclass %i in savegame", tclass);
		}

	}

}

// after all the thinkers have been restored, replace all indices in
// the mobj->target and mobj->tracers fields by the corresponding current pointers again
void P_RestoreTargets()
{
	for (auto th{thinkercap.next}; th != &thinkercap; th = th->next)
	{
		if (th->function.acp1 == (actionf_p1)P_MobjThinker)
		{
			auto mo = (MapObject*)th;
			mo->target = (MapObject*)P_IndexToThinker((uintptr_t)mo->target);
			mo->tracer = (MapObject*)P_IndexToThinker((uintptr_t)mo->tracer);
		}
	}

	if (restoretargets_fail)
	{
		fprintf(stderr, "P_RestoreTargets: Failed to restore %d target pointers.\n", restoretargets_fail);
		restoretargets_fail = 0;
	}
}

enum class specials_e
{
	tc_ceiling,
	tc_door,
	tc_floor,
	tc_plat,
	tc_flash,
	tc_strobe,
	tc_glow,
	tc_endspecials

};

// Things to handle:
//
// T_MoveCeiling, (ceiling_t: sector_t* swizzle), - active list
// T_VerticalDoor, (vldoor_t: sector_t* swizzle),
// T_MoveFloor, (floormove_t: sector_t* swizzle),
// T_LightFlash, (lightflash_t: sector_t* swizzle),
// T_StrobeFlash, (strobe_t: sector_t*),
// T_Glow, (glow_t: sector_t*),
// T_PlatRaise, (plat_t: sector_t*), - active list
void P_ArchiveSpecials()
{
	thinker_t* th;
	int i;

	// save off the current thinkers
	for (th = thinkercap.next; th != &thinkercap; th = th->next)
	{
		if (th->function.acv == (actionf_v)NULL)
		{
			for (i = 0; i < MAXCEILINGS; ++i)
			{
				if (activeceilings[i] == (ceiling_t*)th)
				{
					break;
				}
			}

			if (i < MAXCEILINGS)
			{
				saveg_write8(specials_e::tc_ceiling);
				saveg_write_pad();
				saveg_write_ceiling_t((ceiling_t*)th);
			}
			// save plats in statis
			for (i = 0; i < MAXPLATS; ++i)
			{
				if (activeplats[i] == (plat_t*)th)
				{
					break;
				}
			}

			if (i < MAXPLATS)
			{
				saveg_write8(specials_e::tc_plat);
				saveg_write_pad();
				saveg_write_plat_t((plat_t*)th);
			}
			continue;
		}

		if (th->function.acp1 == (actionf_p1)T_MoveCeiling)
		{
			saveg_write8(specials_e::tc_ceiling);
			saveg_write_pad();
			saveg_write_ceiling_t((ceiling_t*)th);
			continue;
		}

		if (th->function.acp1 == (actionf_p1)T_VerticalDoor)
		{
			saveg_write8(specials_e::tc_door);
			saveg_write_pad();
			saveg_write_vldoor_t((vldoor_t*)th);
			continue;
		}

		if (th->function.acp1 == (actionf_p1)T_MoveFloor)
		{
			saveg_write8(specials_e::tc_floor);
			saveg_write_pad();
			saveg_write_floormove_t((floormove_t*)th);
			continue;
		}

		if (th->function.acp1 == (actionf_p1)T_PlatRaise)
		{
			saveg_write8(specials_e::tc_plat);
			saveg_write_pad();
			saveg_write_plat_t((plat_t*)th);
			continue;
		}

		if (th->function.acp1 == (actionf_p1)T_LightFlash)
		{
			saveg_write8(specials_e::tc_flash);
			saveg_write_pad();
			saveg_write_lightflash_t((lightflash_t*)th);
			continue;
		}

		if (th->function.acp1 == (actionf_p1)T_StrobeFlash)
		{
			saveg_write8(specials_e::tc_strobe);
			saveg_write_pad();
			saveg_write_strobe_t((strobe_t*)th);
			continue;
		}

		if (th->function.acp1 == (actionf_p1)T_Glow)
		{
			saveg_write8(specials_e::tc_glow);
			saveg_write_pad();
			saveg_write_glow_t((glow_t*)th);
			continue;
		}
	}

	// add a terminating marker
	saveg_write8(specials_e::tc_endspecials);
}

void P_UnArchiveSpecials()
{
	byte tclass;
	ceiling_t* ceiling;
	vldoor_t* door;
	floormove_t* floor;
	plat_t* plat;
	lightflash_t* flash;
	strobe_t* strobe;
	glow_t* glow;

	// read in saved thinkers
	while (1)
	{
		tclass = saveg_read8();

		switch (tclass)
		{
		case specials_e::tc_endspecials:
			return;	// end of list

		case specials_e::tc_ceiling:
			saveg_read_pad();
			ceiling = Z_Malloc<decltype(*ceiling)>(sizeof(*ceiling), pu_tags_t::PU_LEVEL, NULL);
			saveg_read_ceiling_t(ceiling);
			ceiling->sector->specialdata = ceiling;

			if (ceiling->thinker.function.acp1)
			{
				ceiling->thinker.function.acp1 = (actionf_p1)T_MoveCeiling;
			}

			P_AddThinker(&ceiling->thinker);
			P_AddActiveCeiling(ceiling);
			break;

		case specials_e::tc_door:
			saveg_read_pad();
			door = Z_Malloc<decltype(*door)>(sizeof(*door), pu_tags_t::PU_LEVEL, NULL);
			saveg_read_vldoor_t(door);
			door->sector->specialdata = door;
			door->thinker.function.acp1 = (actionf_p1)T_VerticalDoor;
			P_AddThinker(&door->thinker);
			break;

		case specials_e::tc_floor:
			saveg_read_pad();
			floor = Z_Malloc<decltype(*floor)>(sizeof(*floor), pu_tags_t::PU_LEVEL, NULL);
			saveg_read_floormove_t(floor);
			floor->sector->specialdata = floor;
			floor->thinker.function.acp1 = (actionf_p1)T_MoveFloor;
			P_AddThinker(&floor->thinker);
			break;

		case specials_e::tc_plat:
			saveg_read_pad();
			plat = Z_Malloc<decltype(*plat)>(sizeof(*plat), pu_tags_t::PU_LEVEL, NULL);
			saveg_read_plat_t(plat);
			plat->sector->specialdata = plat;

			if (plat->thinker.function.acp1)
			{
				plat->thinker.function.acp1 = (actionf_p1)T_PlatRaise;
			}

			P_AddThinker(&plat->thinker);
			P_AddActivePlat(plat);
			break;

		case specials_e::tc_flash:
			saveg_read_pad();
			flash = Z_Malloc<decltype(*flash)>(sizeof(*flash), pu_tags_t::PU_LEVEL, NULL);
			saveg_read_lightflash_t(flash);
			flash->thinker.function.acp1 = (actionf_p1)T_LightFlash;
			P_AddThinker(&flash->thinker);
			break;

		case specials_e::tc_strobe:
			saveg_read_pad();
			strobe = Z_Malloc<decltype(*strobe)>(sizeof(*strobe), pu_tags_t::PU_LEVEL, NULL);
			saveg_read_strobe_t(strobe);
			strobe->thinker.function.acp1 = (actionf_p1)T_StrobeFlash;
			P_AddThinker(&strobe->thinker);
			break;

		case specials_e::tc_glow:
			saveg_read_pad();
			glow = Z_Malloc<decltype(*glow)>(sizeof(*glow), pu_tags_t::PU_LEVEL, NULL);
			saveg_read_glow_t(glow);
			glow->thinker.function.acp1 = (actionf_p1)T_Glow;
			P_AddThinker(&glow->thinker);
			break;

		default:
			I_Error("P_UnarchiveSpecials:Unknown tclass %i in savegame", tclass);
		}
	}

}

