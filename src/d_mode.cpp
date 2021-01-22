/**********************************************************************************************************************************************\
	Copyright(C) 2005-2014 Simon Howard

	This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License
	as published by the Free Software Foundation; either version 2 of the License, or (at your option) any later version.

	This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

	DESCRIPTION:
		Functions and definitions relating to the game type and operational mode.
\**********************************************************************************************************************************************/
#include "../derma/stdafx.h"

#include "d_mode.h"

// Valid game mode/mission combinations, with the number of episodes/maps for each.
static struct
{
	GameMission mission;
	GameMode mode;
	int episode;
	int map;
} valid_modes[] = {
	{ GameMission::pack_chex,		GameMode::retail,		1, 5 },
	{ GameMission::doom,			GameMode::shareware,	1, 9 },
	{ GameMission::doom,			GameMode::registered,	3, 9 },
	{ GameMission::doom,			GameMode::retail,		4, 9 },
	{ GameMission::doom2,			GameMode::commercial,	1, 32 },
	{ GameMission::pack_tnt,		GameMode::commercial,	1, 32 },
	{ GameMission::pack_plut,		GameMode::commercial,	1, 32 },
	{ GameMission::pack_hacx,		GameMode::commercial,	1, 32 },
	{ GameMission::pack_nerve,		GameMode::commercial,	1, 9 },
	{ GameMission::pack_master,		GameMode::commercial,	1, 21 },
	{ GameMission::heretic,			GameMode::shareware,	1, 9 },
	{ GameMission::heretic,			GameMode::registered,	3, 9 },
	{ GameMission::heretic,			GameMode::retail,		5, 9 },
	{ GameMission::hexen,			GameMode::commercial,	1, 60 },
	{ GameMission::strife,			GameMode::commercial,	1, 34 },
};

// Check that a gamemode+gamemission received over the network is valid.
bool D_ValidGameMode(GameMission mission, GameMode mode)
{
	for (size_t i{0}; i < arrlen(valid_modes); ++i)
	{
		if (valid_modes[i].mode == mode && valid_modes[i].mission == mission)
		{
			return true;
		}
	}

	return false;
}

bool D_ValidEpisodeMap(GameMission mission, GameMode mode, int episode, int map)
{
	// Hacks for Heretic secret episodes
	if (mission == GameMission::heretic)
	{
		if (mode == GameMode::retail && episode == 6)
		{
			return map >= 1 && map <= 3;
		}
		else if (mode == GameMode::registered && episode == 4)
		{
			return map == 1;
		}
	}

	// Find the table entry for this mission/mode combination.
	for (size_t i{0}; i < arrlen(valid_modes); ++i)
	{
		if (mission == valid_modes[i].mission && mode == valid_modes[i].mode)
		{
			return episode >= 1 && episode <= valid_modes[i].episode && map >= 1 && map <= valid_modes[i].map;
		}
	}

	// Unknown mode/mission combination
	return false;
}

// Get the number of valid episodes for the specified mission/mode.
int D_GetNumEpisodes(GameMission mission, GameMode mode)
{
	auto episode{1};

	while (D_ValidEpisodeMap(mission, mode, episode, 1))
	{
		++episode;
	}

	return episode - 1;
}

// Table of valid versions
static struct {
	GameMission mission;
	GameVersion version;
} valid_versions[] = {
	{ GameMission::doom,		GameVersion::exe_doom_1_2 },
	{ GameMission::doom,		GameVersion::exe_doom_1_666 },
	{ GameMission::doom,		GameVersion::exe_doom_1_7 },
	{ GameMission::doom,		GameVersion::exe_doom_1_8 },
	{ GameMission::doom,		GameVersion::exe_doom_1_9 },
	{ GameMission::doom,		GameVersion::exe_hacx },
	{ GameMission::doom,		GameVersion::exe_ultimate },
	{ GameMission::doom,		GameVersion::exe_final },
	{ GameMission::doom,		GameVersion::exe_final2 },
	{ GameMission::doom,		GameVersion::exe_chex },
	{ GameMission::heretic,		GameVersion::exe_heretic_1_3 },
	{ GameMission::hexen,		GameVersion::exe_hexen_1_1 },
	{ GameMission::strife,		GameVersion::exe_strife_1_2 },
	{ GameMission::strife,		GameVersion::exe_strife_1_31 }
};

bool D_ValidGameVersion(GameMission mission, GameVersion version)
{
	// All Doom variants can use the Doom versions.
	if (mission == GameMission::doom2 || mission == GameMission::pack_plut || mission == GameMission::pack_tnt
		|| mission == GameMission::pack_hacx || mission == GameMission::pack_chex
		|| mission == GameMission::pack_nerve || mission == GameMission::pack_master)
	{
		mission = GameMission::doom;
	}

	for (size_t i{0}; i < arrlen(valid_versions); ++i)
	{
		if (valid_versions[i].mission == mission && valid_versions[i].version == version)
		{
			return true;
		}
	}

	return false;
}

// Does this mission type use ExMy form, rather than MAPxy form?
bool D_IsEpisodeMap(GameMission mission)
{
	switch (mission)
	{
		case GameMission::doom:
		case GameMission::heretic:
		case GameMission::pack_chex:
			return true;

		case GameMission::none:
		case GameMission::hexen:
		case GameMission::doom2:
		case GameMission::pack_hacx:
		case GameMission::pack_tnt:
		case GameMission::pack_plut:
		case GameMission::pack_nerve:
		case GameMission::pack_master:
		case GameMission::strife:
		default:
			return false;
	}
}

::std::string D_GameMissionString(GameMission mission)
{
	switch (mission)
	{
		case GameMission::doom:
			return "doom";
		case GameMission::doom2:
			return "doom2";
		case GameMission::pack_tnt:
			return "tnt";
		case GameMission::pack_plut:
			return "plutonia";
		case GameMission::pack_hacx:
			return "hacx";
		case GameMission::pack_chex:
			return "chex";
		case GameMission::heretic:
			return "heretic";
		case GameMission::hexen:
			return "hexen";
		case GameMission::strife:
			return "strife";
		case GameMission::none:
		default:
			return "none";
	}
}

::std::string D_GameModeString(GameMode mode)
{
	switch (mode)
	{
		case GameMode::shareware:
			return "shareware";
		case GameMode::registered:
			return "registered";
		case GameMode::commercial:
			return "commercial";
		case GameMode::retail:
			return "retail";
		case GameMode::indetermined:
		default:
			return "unknown";
	}
}
