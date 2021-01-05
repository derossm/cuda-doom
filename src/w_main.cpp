/**********************************************************************************************************************************************\
	Copyright(C) 1993-1996 Id Software, Inc.
	Copyright(C) 2005-2014 Simon Howard

	This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License
	as published by the Free Software Foundation; either version 2 of the License, or (at your option) any later version.

	This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

	DESCRIPTION:
		Common code to parse command line, identifying WAD files to load.
\**********************************************************************************************************************************************/


#include "config.h"
#include "d_iwad.h"
#include "i_glob.h"
#include "i_system.h"
#include "m_argv.h"
#include "w_main.h"
#include "w_merge.h"
#include "w_wad.h"
#include "z_zone.h"

// Parse the command line, merging WAD files that are sppecified.
// Returns true if at least one file was added.
bool W_ParseCommandLine()
{
	bool modifiedgame = false;
	int p;

	// Merged PWADs are loaded first, because they are supposed to be
	// modified IWADs.

	//!
	// @arg <files>
	// @category mod
	//
	// Simulates the behavior of deutex's -merge option, merging a PWAD
	// into the main IWAD. Multiple files may be specified.
	//

	p = M_CheckParmWithArgs("-merge", 1);

	if (p > 0)
	{
		for (p = p + 1; p < myargc && myargv[p][0] != '-'; ++p)
		{
			std::string filename;

			modifiedgame = true;

			filename = D_TryFindWADByName(myargv[p]);

			printf(" merging %s\n", filename);
			W_MergeFile(filename);
			free(filename);
		}
	}

	// NWT-style merging:

	// NWT's -merge option:

	//!
	// @arg <files>
	// @category mod
	//
	// Simulates the behavior of NWT's -merge option. Multiple files
	// may be specified.

	p = M_CheckParmWithArgs("-nwtmerge", 1);

	if (p > 0)
	{
		for (p = p + 1; p < myargc && myargv[p][0] != '-'; ++p)
		{
			std::string filename;

			modifiedgame = true;

			filename = D_TryFindWADByName(myargv[p]);

			printf(" performing NWT-style merge of %s\n", filename);
			W_NWTDashMerge(filename);
			free(filename);
		}
	}

	// Add flats

	//!
	// @arg <files>
	// @category mod
	//
	// Simulates the behavior of NWT's -af option, merging flats into
	// the main IWAD directory. Multiple files may be specified.
	//

	p = M_CheckParmWithArgs("-af", 1);

	if (p > 0)
	{
		for (p = p + 1; p < myargc && myargv[p][0] != '-'; ++p)
		{
			std::string filename;

			modifiedgame = true;

			filename = D_TryFindWADByName(myargv[p]);

			printf(" merging flats from %s\n", filename);
			W_NWTMergeFile(filename, W_NWT_MERGE_FLATS);
			free(filename);
		}
	}

	//!
	// @arg <files>
	// @category mod
	//
	// Simulates the behavior of NWT's -as option, merging sprites
	// into the main IWAD directory. Multiple files may be specified.
	//

	p = M_CheckParmWithArgs("-as", 1);

	if (p > 0)
	{
		for (p = p + 1; p < myargc && myargv[p][0] != '-'; ++p)
		{
			std::string filename;

			modifiedgame = true;
			filename = D_TryFindWADByName(myargv[p]);

			printf(" merging sprites from %s\n", filename);
			W_NWTMergeFile(filename, W_NWT_MERGE_SPRITES);
			free(filename);
		}
	}

	//!
	// @arg <files>
	// @category mod
	//
	// Equivalent to "-af <files> -as <files>".
	//

	p = M_CheckParmWithArgs("-aa", 1);

	if (p > 0)
	{
		for (p = p + 1; p < myargc && myargv[p][0] != '-'; ++p)
		{
			std::string filename;

			modifiedgame = true;

			filename = D_TryFindWADByName(myargv[p]);

			printf(" merging sprites and flats from %s\n", filename);
			W_NWTMergeFile(filename, W_NWT_MERGE_SPRITES | W_NWT_MERGE_FLATS);
			free(filename);
		}
	}

	//!
	// @arg <files>
	// @vanilla
	//
	// Load the specified PWAD files.
	//

	p = M_CheckParmWithArgs("-file", 1);
	if (p)
	{
		// the parms after p are wadfile/lump names,
		// until end of parms or another - preceded parm
		modifiedgame = true;			// homebrew levels
		while (++p != myargc && myargv[p][0] != '-')
		{
			std::string filename;

			filename = D_TryFindWADByName(myargv[p]);

			// [crispy] always merge arguments of "-file" parameter
			printf(" merging %s !\n", filename);
			W_MergeFile(filename);
			free(filename);
		}
	}

	//W_PrintDirectory();
	return modifiedgame;
}

// Load all WAD files from the given directory.
void W_AutoLoadWADs(std::string path)
{
	glob_t* glob;
	std::string filename;

	glob = I_StartMultiGlob(path, GLOB_FLAG_NOCASE | GLOB_FLAG_SORTED, "*.wad", "*.lmp", NULL);
	for (;;)
	{
		filename = I_NextGlob(glob);
		if (filename.empty())
		{
			break;
		}
		printf(" [autoload] merging %s\n", filename);
		W_MergeFile(filename);
	}

	I_EndGlob(glob);
}

// Lump names that are unique to particular game types. This lets us check the user is not trying to play with the wrong executable,
// eg. chocolate-doom -iwad hexen.wad.
const struct lump_t
{
	GameMission mission;
	std::string lumpname;
};

std::array<lump_t, 4> unique_lumps{
	lump_t{ GameMission::doom,		"POSSA1" },
	lump_t{ GameMission::heretic,	"IMPXA1" },
	lump_t{ GameMission::hexen,		"ETTNA1" },
	lump_t{ GameMission::strife,	"AGRDA1" }
};

void W_CheckCorrectIWAD(GameMission mission)
{
	auto result = std::ranges::find_first_of(unique_lumps,
		[&mission](auto iter) { return iter.mission == mission || D_SuggestGameName(iter, GameMode::indetermined) });

	/*
		for (auto iter : unique_lumps)
		{
			if (mission != iter.mission)
			{
				lumpindex_t lumpnum = W_CheckNumForName(iter.lumpname);

				if (lumpnum >= 0)
				{
					I_Error("\nYou are trying to use a %s IWAD file with the %s%s binary.\nThis isn't going to work.\n"
							"You probably want to use the %s%s binary.",
							D_SuggestGameName(iter.mission, GameMode::indetermined),
							PROGRAM_PREFIX,
							D_GameMissionString(mission),
							PROGRAM_PREFIX,
							D_GameMissionString(iter.mission));
				}
			}
		}
	*/
}
