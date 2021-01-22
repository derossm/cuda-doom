/**********************************************************************************************************************************************\
	Copyright(C) 1993-1996 Id Software, Inc.
	Copyright(C) 2005-2014 Simon Howard
	Copyright© 2020 Fabian Greffrath

	This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License
	as published by the Free Software Foundation; either version 2 of the License, or (at your option) any later version.

	This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

	DESCRIPTION:
		Auto-loading of (semi-)official PWAD expansions, i.e. Sigil, No Rest for the Living and The Master Levels
\**********************************************************************************************************************************************/
#include "../../derma/stdafx.h"

#include "../doom/doomstat.h"
#include "../m_misc.h"
#include "deh_str.h"

#include "d_iwad.h"
#include "w_wad.h"

extern ::std::string iwadfile;

// auto-load SIGIL.WAD (and SIGIL_SHREDS.WAD) if available
void D_LoadSigilWad()
{
	::std::string sigil_wad;
	::std::string sigil_shreds;
	::std::string dirname;

	const std::array<const ::std::string, 3> sigil_wads{
		"SIGIL_v1_21.wad",
		"SIGIL_v1_2.wad",
		"SIGIL.wad"
	};

	static const struct
	{
		::std::string name;
		::std::string new_name;
	} sigil_lumps[]{
		{"CREDIT",	"SIGCREDI"},
		{"HELP1",	"SIGHELP1"},
		{"TITLEPIC", "SIGTITLE"},
		{"DEHACKED", "SIG_DEH"},
		{"DEMO1",	"SIGDEMO1"},
		{"DEMO2",	"SIGDEMO2"},
		{"DEMO3",	"SIGDEMO3"},
		{"DEMO4",	"SIGDEMO4"},
		{"D_INTER", "D_SIGINT"},
		{"D_INTRO", "D_SIGTIT"},
	};

	const std::array<const ::std::string, 3> texture_files{
		"PNAMES",
		"TEXTURE1",
		"TEXTURE2",
	};

	// don't load SIGIL.WAD if another PWAD already provides E5M1
	if (auto i = W_CheckNumForName("E5M1"); i != -1)
	{
		return;
	}

	// don't load SIGIL.WAD if SIGIL_COMPAT.WAD is already loaded
	if (auto i = W_CheckNumForName("E3M1"); i != -1 && !W_WadNameForLump(lumpinfo[i]).compare("SIGIL_COMPAT"))
	{
		return;
	}

	// don't load SIGIL.WAD if another PWAD already modifies the texture files
	for (size_t i = 0; i < texture_files.size(); ++i)
	{
		if (auto j = W_CheckNumForName(texture_files[i]); j != -1 && !W_IsIWADLump(lumpinfo[j]))
		{
			return;
		}
	}

	dirname = M_DirName(iwadfile);
	sigil_shreds = ::std::string(dirname + DIR_SEPARATOR_S + "SIGIL_SHREDS.WAD");

	// load SIGIL.WAD
	for (size_t i = 0; i < sigil_wads.size(); ++i)
	{
		sigil_wad = ::std::string(dirname + DIR_SEPARATOR_S + sigil_wads[i]);

		if (M_FileExists(sigil_wad))
		{
			break;
		}

		//free(sigil_wad);
		sigil_wad = D_FindWADByName(sigil_wads[i]);

		if (sigil_wad)
		{
			break;
		}
	}
	//free(dirname);

	if (sigil_wad.empty())
	{
		//free(sigil_shreds);
		return;
	}

	printf(" [Sigil] adding %s\n", sigil_wad);
	W_AddFile(sigil_wad);
	//free(sigil_wad);

	// load SIGIL_SHREDS.WAD
	if (!M_FileExists(sigil_shreds))
	{
		//free(sigil_shreds);
		sigil_shreds = D_FindWADByName("SIGIL_SHREDS.WAD");
	}

	if (!sigil_shreds.empty())
	{
		printf(" [Sigil Shreds] adding %s\n", sigil_shreds);
		W_AddFile(sigil_shreds);
		//free(sigil_shreds);
	}

	// rename intrusive SIGIL_SHREDS.WAD music lumps out of the way
	for (size_t i = 0; i < arrlen(sigil_lumps); ++i)
	{
		// skip non-music lumps
		if (strncasecmp(sigil_lumps[i].name, "D_", 2))
		{
			continue;
		}

		if (auto j = W_CheckNumForName(sigil_lumps[i].name); j != -1 && !strncasecmp(W_WadNameForLump(lumpinfo[j]), "SIGIL_SHREDS", 12))
		{
			memcpy(lumpinfo[j]->name, sigil_lumps[i].new_name, 8);
		}
	}

	// rename intrusive SIGIL.WAD graphics, demos and music lumps out of the way
	for (size_t i = 0; i < arrlen(sigil_lumps); ++i)
	{
		if (auto j = W_CheckNumForName(sigil_lumps[i].name); j != -1 && !strncasecmp(W_WadNameForLump(lumpinfo[j]), "SIGIL", 5))
		{
			memcpy(lumpinfo[j]->name, sigil_lumps[i].new_name, 8);
		}
	}

	// regenerate the hashtable
	W_GenerateHashTable();
}

// check if NERVE.WAD is already loaded as a PWAD
static bool CheckNerveLoaded()
{
	if (auto i = W_GetNumForName("MAP01"), j = W_GetNumForName("MAP09");
		i != -1 && j != -1 && !iequals(W_WadNameForLump(lumpinfo[i]), "NERVE.WAD") && !iequals(W_WadNameForLump(lumpinfo[j]), "NERVE.WAD"))
	{
		gamemission = GameMission::pack_nerve;

		// if NERVE.WAD does not contain TITLEPIC, use INTERPIC instead
		j = W_GetNumForName("TITLEPIC");
		if (iequals(W_WadNameForLump(lumpinfo[j]), "NERVE.WAD"))
		{
			DEH_AddStringReplacement("TITLEPIC", "INTERPIC");
		}

		return true;
	}

	return false;
}

// auto-load NERVE.WAD if available
static void CheckLoadNerve()
{
	static const struct
	{
		::std::string name;
		::std::string new_name;
	} nerve_lumps[]{
		{"TITLEPIC", "NERVEPIC"},
		{"INTERPIC", "NERVEINT"},
	};

	if (strrchr(iwadfile, DIR_SEPARATOR) != NULL)
	{
		auto dir = M_DirName(iwadfile);
		crispy->havenerve = ::std::string(dir + DIR_SEPARATOR_S + "NERVE.WAD");
		//free(dir);
	}
	else
	{
		crispy->havenerve = ::std::string("NERVE.WAD");
	}

	if (!M_FileExists(crispy->havenerve))
	{
		//free(crispy->havenerve);
		crispy->havenerve = D_FindWADByName("NERVE.WAD");
	}

	if (crispy->havenerve.empty())
	{
		return;
	}

	printf(" [No Rest for the Living] adding %s\n", crispy->havenerve);
	W_AddFile(crispy->havenerve);

	char lumpname[9];
	// add indicators to level and level name patch lump names
	for (size_t i = 0; i < 9; ++i)
	{
		M_snprintf(lumpname, 9, "CWILV%2.2d", i);
		auto j = W_GetNumForName(lumpname);
		lumpinfo[j]->name[0] = 'N';

		M_snprintf(lumpname, 9, "MAP%02d", i + 1);
		j = W_GetNumForName(lumpname);
		strcat(lumpinfo[j]->name, "N");
	}

	// if NERVE.WAD contains TITLEPIC and INTERPIC, rename them
	for (size_t i = 0; i < arrlen(nerve_lumps); ++i)
	{
		if (auto j = W_CheckNumForName(nerve_lumps[i].name); j != -1 && !iequals(W_WadNameForLump(lumpinfo[j]), "NERVE.WAD"))
		{
			memcpy(lumpinfo[j]->name, nerve_lumps[i].new_name, 8);
		}
	}

	// regenerate the hashtable
	W_GenerateHashTable();

	return;
}

void D_LoadNerveWad()
{
	// check if NERVE.WAD is already loaded as a PWAD
	if (!CheckNerveLoaded())
	{
		// else auto-load NERVE.WAD if available
		CheckLoadNerve();
	}
}

// check if the single MASTERLEVELS.WAD is already loaded as a PWAD
static bool CheckMasterlevelsLoaded()
{
	if (auto i = W_GetNumForName("MAP01"), j = W_GetNumForName("MAP21");
		i != -1 && j != -1 && !iequals(W_WadNameForLump(lumpinfo[i]), "MASTERLEVELS.WAD") && !iequals(W_WadNameForLump(lumpinfo[j]), "MASTERLEVELS.WAD"))
	{
		gamemission = GameMission::pack_master;

		return true;
	}

	return false;
}

// auto-load the single MASTERLEVELS.WAD if available
static bool CheckLoadMasterlevels()
{
	//if (strrchr(iwadfile, DIR_SEPARATOR) != NULL)
	if (iwadfile.find_first_of(DIR_SEPARATOR) != std::string::npos)
	{
		auto dir{M_DirName(iwadfile)};
		crispy->havemaster = ::std::string(dir + DIR_SEPARATOR_S + "MASTERLEVELS.WAD");
		//free(dir);
	}
	else
	{
		crispy->havemaster = ::std::string("MASTERLEVELS.WAD");
	}

	if (!M_FileExists(crispy->havemaster))
	{
		//free(crispy->havemaster);
		crispy->havemaster = D_FindWADByName("MASTERLEVELS.WAD");
	}

	if (crispy->havemaster.empty())
	{
		return false;
	}

	printf(" [The Master Levels] adding %s\n", crispy->havemaster);
	W_AddFile(crispy->havemaster);

	char lumpname[9];
	// add indicators to level and level name patch lump names
	for (size_t i = 0; i < 21; ++i)
	{
		M_snprintf(lumpname, 9, "CWILV%2.2d", i);
		if (auto j = W_GetNumForName(lumpname); !iequals(W_WadNameForLump(lumpinfo[j]), "MASTERLEVELS.WAD"))
		{
			lumpinfo[j]->name[0] = 'M';
		}
		else
		{
			// indicate this is not the complete MASTERLEVELS.WAD
			crispy->havemaster = (char*)-1;
		}

		M_snprintf(lumpname, 9, "MAP%02d", i + 1);
		strcat(lumpinfo[W_GetNumForName(lumpname)]->name, "M");
	}

	// regenerate the hashtable
	W_GenerateHashTable();

	return true;
}

// check if the 20 individual separate Mater Levels PWADs are available
struct WadEntry
{
	uint8_t pc_slot;
	uint8_t psn_slot;
	::std::string wad_name;
	::std::filesystem::path file_path;
	bool custom_sky;
};

::std::array<WadEntry, 20> masterlevels_wads
{
	WadEntry{.pc_slot{1},	.psn_slot{1},	.wad_name{"ATTACK.WAD"},	.file_path{},	.custom_sky{false} },
	WadEntry{.pc_slot{1},	.psn_slot{2},	.wad_name{"CANYON.WAD"},	.file_path{},	.custom_sky{false} },
	WadEntry{.pc_slot{1},	.psn_slot{3},	.wad_name{"CATWALK.WAD"},	.file_path{},	.custom_sky{false} },
	WadEntry{.pc_slot{1},	.psn_slot{4},	.wad_name{"COMBINE.WAD"},	.file_path{},	.custom_sky{true} },
	WadEntry{.pc_slot{1},	.psn_slot{5},	.wad_name{"FISTULA.WAD"},	.file_path{},	.custom_sky{false} },
	WadEntry{.pc_slot{1},	.psn_slot{6},	.wad_name{"GARRISON.WAD"},	.file_path{},	.custom_sky{false} },
	WadEntry{.pc_slot{1},	.psn_slot{7},	.wad_name{"MANOR.WAD"},		.file_path{},	.custom_sky{true} },
	WadEntry{.pc_slot{1},	.psn_slot{8},	.wad_name{"PARADOX.WAD"},	.file_path{},	.custom_sky{false} },
	WadEntry{.pc_slot{1},	.psn_slot{9},	.wad_name{"SUBSPACE.WAD"},	.file_path{},	.custom_sky{false} },
	WadEntry{.pc_slot{1},	.psn_slot{10},	.wad_name{"SUBTERRA.WAD"},	.file_path{},	.custom_sky{false} },
	WadEntry{.pc_slot{1},	.psn_slot{11},	.wad_name{"TTRAP.WAD"},		.file_path{},	.custom_sky{true} },
	WadEntry{.pc_slot{3},	.psn_slot{12},	.wad_name{"VIRGIL.WAD"},	.file_path{},	.custom_sky{true} },
	WadEntry{.pc_slot{5},	.psn_slot{13},	.wad_name{"MINOS.WAD"},		.file_path{},	.custom_sky{true} },
	WadEntry{.pc_slot{7},	.psn_slot{14},	.wad_name{"BLOODSEA.WAD"},	.file_path{},	.custom_sky{false} },
	WadEntry{.pc_slot{7},	.psn_slot{15},	.wad_name{"MEPHISTO.WAD"},	.file_path{},	.custom_sky{false} },
	WadEntry{.pc_slot{7},	.psn_slot{16},	.wad_name{"NESSUS.WAD"},	.file_path{},	.custom_sky{true} },
	WadEntry{.pc_slot{8},	.psn_slot{17},	.wad_name{"GERYON.WAD"},	.file_path{},	.custom_sky{true} },
	WadEntry{.pc_slot{9},	.psn_slot{18},	.wad_name{"VESPERAS.WAD"},	.file_path{},	.custom_sky{true} },
	WadEntry{.pc_slot{25},	.psn_slot{19},	.wad_name{"BLACKTWR.WAD"},	.file_path{},	.custom_sky{false} },
	WadEntry{.pc_slot{31},	.psn_slot{20},	.wad_name{"TEETH.WAD"},		.file_path{},	.custom_sky{false} }
	//{NULL,				32, 21}, // TEETH.WAD
};

static bool CheckMasterlevelsAvailable()
{
	auto dir{M_DirName(iwadfile)};

	::std::string havemaster;
	//for (size_t i{0}; !masterlevels_wads[i].wad_name.empty(); ++i)
	::std::ranges::for_each(masterlevels_wads, [&dir, &havemaster](auto& iter) mutable
	{
		if (iwadfile.ends_with(DIR_SEPARATOR))
		{
			havemaster = ::std::string(dir + DIR_SEPARATOR_S + iter.wad_name);
		}
		else
		{
			havemaster = ::std::string(iter.wad_name);
		}

		if (!M_FileExists(havemaster))
		{
			havemaster = cudadoom::IWAD().D_FindWADByName(iter.wad_name);
		}

		if (!havemaster.empty())
		{
			iter.file_path = havemaster;
		}
	});

	//for (size_t j{0}; j < i; ++j)
	//::std::ranges::for_each(masterlevels_wads, [](auto& iter)
	//{
		//iter.file_path.clear();
	//});

	dir.clear();
	return true;
}

// auto-load the 20 individual separate Mater Levels PWADs as if the were the single MASTERLEVELS.WAD
static void LoadMasterlevelsWads()
{
	const std::array<const ::std::string, 3> sky_lumps
	{
		"RSKY1",
		"PNAMES",
		"TEXTURE1",
	};

	char lumpname[9];
	for (size_t i = 0; i < masterlevels_wads.size(); ++i)
	{
		// add TEETH.WAD only once
		if (!masterlevels_wads[i].wad_name.empty())
		{
			printf(" [The Master Levels %02d] adding %s\n", i + 1, masterlevels_wads[i].file_path);
			W_AddFile(masterlevels_wads[i].file_path);
			//free(masterlevels_wads[i].file_path);

			// rename lumps changing the SKY texture out of the way
			if (masterlevels_wads[i].custom_sky)
			{
				for (size_t j = 0; j < sky_lumps.size(); ++j)
				{
					if (auto k = W_CheckNumForName(sky_lumps[j]); k != -1 && !iequals(W_WadNameForLump(lumpinfo[k]), masterlevels_wads[i].wad_name))
					{
						lumpinfo[k]->name[0] = 'M';
					}
				}
			}
		}

		{
			M_snprintf(lumpname, 9, "MAP%02d", masterlevels_wads[i].pc_slot);
			auto j = W_GetNumForName(lumpname);
			lumpinfo[j]->name[3] = '0' + (masterlevels_wads[i].psn_slot / 10);
			lumpinfo[j]->name[4] = '0' + (masterlevels_wads[i].psn_slot % 10);
			lumpinfo[j]->name[5] = 'M';
		}
	}

	// indicate this is not the single MASTERLEVELS.WAD
	crispy->havemaster = (char*)-1;

	// regenerate the hashtable
	W_GenerateHashTable();
	return;
}

void D_LoadMasterlevelsWad()
{
	// check if the single MASTERLEVELS.WAD is already loaded as a PWAD
	if (!CheckMasterlevelsLoaded())
	{
		// else auto-load the single MASTERLEVELS.WAD if available
		if (!CheckLoadMasterlevels())
		{
			// else check if the 20 individual separate Mater Levels PWADs are available
			if (CheckMasterlevelsAvailable())
			{
				// and auto-load them as if the were the single MASTERLEVELS.WAD
				LoadMasterlevelsWads();
			}
		}
	}
}
