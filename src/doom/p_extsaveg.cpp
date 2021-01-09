/**********************************************************************************************************************************************\
	Copyright(C) 1993-1996 Id Software, Inc.
	Copyright(C) 2005-2014 Simon Howard
	Copyright(C) 2016 Fabian Greffrath

	This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License
	as published by the Free Software Foundation; either version 2 of the License, or (at your option) any later version.

	This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

	DESCRIPTION:
		Archiving: Extended SaveGame I/O.
\**********************************************************************************************************************************************/

#include <string>

#include "config.h"
#include "doomstat.h"
#include "doomtype.h"
#include "m_misc.h"
#include "p_extsaveg.h"
#include "p_local.h"
#include "p_saveg.h"
#include "p_setup.h"
#include "s_sound.h"
#include "s_musinfo.h"
#include "z_zone.h"

constexpr size_t MAX_LINE_LEN{260};
constexpr size_t MAX_STRING_LEN{80};

::std::string line;
::std::string string;

static void P_WritePackageTarname(::std::string key)
{
	M_snprintf(line, MAX_LINE_LEN, "%s %s\n", key, PACKAGE_VERSION);
	fputs(line, save_stream);
}

// maplumpinfo->wad_file->basename

::std::string savewadfilename = NULL;

static void P_WriteWadFileName(::std::string key)
{
	M_snprintf(line, MAX_LINE_LEN, "%s %s\n", key, W_WadNameForLump(maplumpinfo));
	fputs(line, save_stream);
}

static void P_ReadWadFileName(::std::string key)
{
	if (!savewadfilename &&
		// only check if loaded from the menu,
		// we have no chance to show a dialog otherwise
		startloadgame == -1)
	{
		if (sscanf(line, "%s", string) == 1 &&
			!strncmp(string, key, MAX_STRING_LEN))
		{
			if (sscanf(line, "%*s %s", string) == 1)
			{
				savewadfilename = strdup(string);
			}
		}
	}
}

// extrakills

static void P_WriteExtraKills(::std::string key)
{
	if (extrakills)
	{
		M_snprintf(line, MAX_LINE_LEN, "%s %d\n", key, extrakills);
		fputs(line, save_stream);
	}
}

static void P_ReadExtraKills(::std::string key)
{
	int value;

	if (sscanf(line, "%s %d", string, &value) == 2 &&
		!strncmp(string, key, MAX_STRING_LEN))
	{
		extrakills = value;
	}
}

// totalleveltimes

static void P_WriteTotalLevelTimes(::std::string key)
{
	if (totalleveltimes)
	{
		M_snprintf(line, MAX_LINE_LEN, "%s %d\n", key, totalleveltimes);
		fputs(line, save_stream);
	}
}

static void P_ReadTotalLevelTimes(::std::string key)
{
	int value;

	if (sscanf(line, "%s %d", string, &value) == 2 &&
		!strncmp(string, key, MAX_STRING_LEN))
	{
		totalleveltimes = value;
	}
}

// T_FireFlicker()

extern void T_FireFlicker(fireflicker_t* flick);

static void P_WriteFireFlicker(::std::string key)
{
	thinker_t* th;

	for (th = thinkercap.next; th != &thinkercap; th = th->next)
	{
		if (th->function.acp1 == (actionf_p1)T_FireFlicker)
		{
			fireflicker_t* flick = (fireflicker_t*)th;

			M_snprintf(line, MAX_LINE_LEN, "%s %d %d %d %d\n",
				key,
				(int)(flick->sector - sectors),
				(int)flick->count,
				(int)flick->maxlight,
				(int)flick->minlight);
			fputs(line, save_stream);
		}
	}
}

static void P_ReadFireFlicker(::std::string key)
{
	int sector;
	int count;
	int maxlight;
	int minlight;

	if (sscanf(line, "%s %d %d %d %d\n",
		string,
		&sector,
		&count,
		&maxlight,
		&minlight) == 5 &&
		!strncmp(string, key, MAX_STRING_LEN))
	{
		fireflicker_t* flick;

		flick = Z_Malloc<decltype(*flick)>(sizeof(*flick), pu_tags_t::PU_LEVEL, NULL);

		flick->sector = &sectors[sector];
		flick->count = count;
		flick->maxlight = maxlight;
		flick->minlight = minlight;

		flick->thinker.function.acp1 = (actionf_p1)T_FireFlicker;

		P_AddThinker(&flick->thinker);
	}
}

// sector->soundtarget

static void P_WriteSoundTarget(::std::string key)
{
	int i;
	sector_t* sector;

	for (i = 0, sector = sectors; i < numsectors; ++i, ++sector)
	{
		if (sector->soundtarget)
		{
			M_snprintf(line, MAX_LINE_LEN, "%s %d %d\n",
				key,
				i,
				P_ThinkerToIndex((thinker_t*)sector->soundtarget));
			fputs(line, save_stream);
		}
	}
}

static void P_ReadSoundTarget(::std::string key)
{
	int sector;
	int target;

	if (sscanf(line, "%s %d %d\n",
		string,
		&sector,
		&target) == 3 &&
		!strncmp(string, key, MAX_STRING_LEN))
	{
		sectors[sector].soundtarget = (MapObject*)P_IndexToThinker(target);
	}
}

// sector->oldspecial

static void P_WriteOldSpecial(::std::string key)
{
	int i;
	sector_t* sector;

	for (i = 0, sector = sectors; i < numsectors; ++i, ++sector)
	{
		if (sector->oldspecial)
		{
			M_snprintf(line, MAX_LINE_LEN, "%s %d %d\n",
				key,
				i,
				sector->oldspecial);
			fputs(line, save_stream);
		}
	}
}

static void P_ReadOldSpecial(::std::string key)
{
	int sector;
	int oldspecial;

	if (sscanf(line, "%s %d %d\n",
		string,
		&sector,
		&oldspecial) == 3 &&
		!strncmp(string, key, MAX_STRING_LEN))
	{
		sectors[sector].oldspecial = oldspecial;
	}
}

// buttonlist[]

extern void P_StartButton(line_t* line, bwhere_e w, int texture, TimeType time);

static void P_WriteButton(::std::string key)
{
	int i;

	for (i = 0; i < maxbuttons; ++i)
	{
		button_t* button = &buttonlist[i];

		if (button->btimer)
		{
			M_snprintf(line, MAX_LINE_LEN, "%s %d %d %d %d\n", key,
						(int)(button->line - lines), (int)button->where, (int)button->btexture, (int)button->btimer);
			fputs(line, save_stream);
		}
	}
}

static void P_ReadButton(::std::string key)
{
	int linedef;
	int where;
	int btexture;
	int btimer;

	if (sscanf(line, "%s %d %d %d %d\n", string, &linedef, &where, &btexture, &btimer) == 5 && !strncmp(string, key, MAX_STRING_LEN))
	{
		P_StartButton(&lines[linedef], where, btexture, btimer);
	}
}

// numbraintargets, braintargeton
extern int numbraintargets;
extern int braintargeton;

static void P_WriteBrainTarget(::std::string key)
{
	thinker_t* th;

	for (th = thinkercap.next; th != &thinkercap; th = th->next)
	{
		if (th->function.acp1 == (actionf_p1)P_MobjThinker)
		{
			MapObject* mo = (MapObject*)th;

			if (mo->state == &states[::std::size_t(statenum_t::S_BRAINEYE1)])
			{
				M_snprintf(line, MAX_LINE_LEN, "%s %d %d\n",
					key,
					numbraintargets,
					braintargeton);
				fputs(line, save_stream);

				// return after the first brain spitter is found
				return;
			}
		}
	}
}

static void P_ReadBrainTarget(::std::string key)
{
	int numtargets;
	int targeton;

	if (sscanf(line, "%s %d %d", string, &numtargets, &targeton) == 3 && !strncmp(string, key, MAX_STRING_LEN))
	{
		numbraintargets = 0; // force A_BrainAwake()
		braintargeton = targeton;
	}
}

// markpoints[]

extern void AM_GetMarkPoints(int* n, long* p);
extern void AM_SetMarkPoints(int n, long* p);

static void P_WriteMarkPoints(::std::string key)
{
	int n;
	long p[20];

	AM_GetMarkPoints(&n, p);

	if (p[0] != -1)
	{
		M_snprintf(line, MAX_LINE_LEN, "%s %d %ld %ld %ld %ld %ld %ld %ld %ld %ld %ld %ld %ld %ld %ld %ld %ld %ld %ld %ld %ld\n",
			key, n,
			p[0], p[1], p[2], p[3], p[4],
			p[5], p[6], p[7], p[8], p[9],
			p[10], p[11], p[12], p[13], p[14],
			p[15], p[16], p[17], p[18], p[19]);
		fputs(line, save_stream);
	}
}

static void P_ReadMarkPoints(::std::string key)
{
	int n;
	long p[20];

	if (sscanf(line, "%s %d %ld %ld %ld %ld %ld %ld %ld %ld %ld %ld %ld %ld %ld %ld %ld %ld %ld %ld %ld %ld\n",
		string, &n,
		&p[0], &p[1], &p[2], &p[3], &p[4],
		&p[5], &p[6], &p[7], &p[8], &p[9],
		&p[10], &p[11], &p[12], &p[13], &p[14],
		&p[15], &p[16], &p[17], &p[18], &p[19]) == 22 &&
		!strncmp(string, key, MAX_STRING_LEN))
	{
		AM_SetMarkPoints(n, p);
	}
}

// players[]->lookdir

static void P_WritePlayersLookdir(::std::string key)
{
	int i;

	for (i = 0; i < MAX_PLAYERS; ++i)
	{
		if (playeringame[i] && players[i].lookdir)
		{
			M_snprintf(line, MAX_LINE_LEN, "%s %d %d\n", key, i, players[i].lookdir);
			fputs(line, save_stream);
		}
	}
}

static void P_ReadPlayersLookdir(::std::string key)
{
	int i;
	int value;

	if (sscanf(line, "%s %d %d", string, &i, &value) == 3 &&
		!strncmp(string, key, MAX_STRING_LEN) &&
		i < MAX_PLAYERS &&
		(crispy->freelook || crispy->mouselook))
	{
		players[i].lookdir = value;
	}
}

// musinfo.current_item

static void P_WriteMusInfo(::std::string key)
{
	if (musinfo.current_item > 0 && musinfo.items[0] > 0)
	{
		char lump[9], orig[9];

		strncpy(lump, lumpinfo[musinfo.current_item]->name, 8);
		strncpy(orig, lumpinfo[musinfo.items[0]]->name, 8);

		M_snprintf(line, MAX_LINE_LEN, "%s %s %s\n", key, lump, orig);
		fputs(line, save_stream);
	}
}

static void P_ReadMusInfo(::std::string key)
{
	int items;
	char lump[9] = {0}, orig[9] = {0};

	items = sscanf(line, "%s %s %s", string, lump, orig);

	if (items >= 2 &&
		!strncmp(string, key, MAX_STRING_LEN))
	{
		int i;

		if ((i = W_CheckNumForName(lump)) > 0)
		{
			memset(&musinfo, 0, sizeof(musinfo));
			musinfo.current_item = i;
			musinfo.from_savegame = true;
			S_ChangeMusInfoMusic(i, true);
		}

		if (items == 3 &&
			(i = W_CheckNumForName(orig)) > 0)
		{
			musinfo.items[0] = i;
		}
	}
}

struct extsavegdata_t
{
	::std::string key;
	void (*extsavegwritefn) (::std::string key);
	void (*extsavegreadfn) (::std::string key);
	const int pass;
};

static const extsavegdata_t extsavegdata[] =
{
	// @FORKS: please change this if you are going to introduce incompatible changes!
	{"crispy-doom", P_WritePackageTarname, NULL, 0},
	{"wadfilename", P_WriteWadFileName, P_ReadWadFileName, 0},
	{"extrakills", P_WriteExtraKills, P_ReadExtraKills, 1},
	{"totalleveltimes", P_WriteTotalLevelTimes, P_ReadTotalLevelTimes, 1},
	{"fireflicker", P_WriteFireFlicker, P_ReadFireFlicker, 1},
	{"soundtarget", P_WriteSoundTarget, P_ReadSoundTarget, 1},
	{"oldspecial", P_WriteOldSpecial, P_ReadOldSpecial, 1},
	{"button", P_WriteButton, P_ReadButton, 1},
	{"braintarget", P_WriteBrainTarget, P_ReadBrainTarget, 1},
	{"markpoints", P_WriteMarkPoints, P_ReadMarkPoints, 1},
	{"playerslookdir", P_WritePlayersLookdir, P_ReadPlayersLookdir, 1},
	{"musinfo", P_WriteMusInfo, P_ReadMusInfo, 0},
};

void P_WriteExtendedSaveGameData()
{
	int i;

	line = static_cast<decltype(line)>(malloc(MAX_LINE_LEN));

	for (i = 0; i < arrlen(extsavegdata); ++i)
	{
		extsavegdata[i].extsavegwritefn(extsavegdata[i].key);
	}

	free(line);
}

static void P_ReadKeyValuePairs(int pass)
{
	while (fgets(line, MAX_LINE_LEN, save_stream))
	{
		if (sscanf(line, "%s", string) == 1)
		{
			int i;

			for (i = 1; i < arrlen(extsavegdata); ++i)
			{
				if (extsavegdata[i].extsavegreadfn &&
					extsavegdata[i].pass == pass &&
					!strncmp(string, extsavegdata[i].key, MAX_STRING_LEN))
				{
					extsavegdata[i].extsavegreadfn(extsavegdata[i].key);
				}
			}
		}
	}
}

// pointer to the info struct for the map lump about to load
lumpinfo_t* savemaplumpinfo = NULL;

void P_ReadExtendedSaveGameData(int pass)
{
	long p, curpos, endpos;
	byte episode, map;
	int lumpnum = -1;

	line = static_cast<decltype(line)>(malloc(MAX_LINE_LEN));
	string = static_cast<decltype(string)>(malloc(MAX_STRING_LEN));

	// two-pass reading of extended savegame data
	if (pass == 1)
	{
		P_ReadKeyValuePairs(1);

		free(line);
		free(string);

		return;
	}

	curpos = ftell(save_stream);

	// check which map we would want to load
	fseek(save_stream, SAVESTRINGSIZE + VERSIONSIZE + 1, SEEK_SET); // + 1 for "gameskill"
	if (fread(&episode, 1, 1, save_stream) == 1 &&
		fread(&map, 1, 1, save_stream) == 1)
	{
		lumpnum = P_GetNumForMap((int)episode, (int)map, false);
	}

	if (lumpnum >= 0)
	{
		savemaplumpinfo = lumpinfo[lumpnum];
	}
	else
	{
		// unavailable map!
		savemaplumpinfo = NULL;
	}

	// read key/value pairs past the end of the regular savegame data
	fseek(save_stream, 0, SEEK_END);
	endpos = ftell(save_stream);

	for (p = endpos - 1; p > 0; --p)
	{
		byte curbyte;

		fseek(save_stream, p, SEEK_SET);

		if (fread(&curbyte, 1, 1, save_stream) < 1)
		{
			break;
		}

		if (curbyte == SAVEGAME_EOF)
		{
			if (!fgets(line, MAX_LINE_LEN, save_stream))
			{
				continue;
			}

			if (sscanf(line, "%s", string) == 1 &&
				!strncmp(string, extsavegdata[0].key, MAX_STRING_LEN))
			{
				P_ReadKeyValuePairs(0);
				break;
			}
		}
	}

	free(line);
	free(string);

	// back to where we started
	fseek(save_stream, curpos, SEEK_SET);
}
