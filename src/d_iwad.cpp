/**********************************************************************************************************************************************\
	Copyright(C) 2005-2014 Simon Howard

	This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License
	as published by the Free Software Foundation; either version 2 of the License, or (at your option) any later version.

	This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

	DESCRIPTION:
		Search for and locate an IWAD file, and initialize according to the IWAD type.
\**********************************************************************************************************************************************/

#include "deh_str.h"
#include "doomkeys.h"
#include "d_iwad.h"
#include "i_system.h"
#include "m_argv.h"
#include "m_config.h"
#include "m_misc.h"
#include "w_wad.h"
#include "z_zone.h"

static const iwad_t iwads[]{
	{ std::string("doom2.wad"),		GameMission_t::doom2,		GameMode::commercial,		std::string("Doom II") },
	{ std::string("plutonia.wad"),	GameMission_t::pack_plut,	GameMode::commercial,		std::string("Final Doom: Plutonia Experiment") },
	{ std::string("tnt.wad"),		GameMission_t::pack_tnt,	GameMode::commercial,		std::string("Final Doom: TNT: Evilution") },
	{ std::string("doom.wad"),		GameMission_t::doom,		GameMode::retail,			std::string("Doom") },
	{ std::string("doom1.wad"),		GameMission_t::doom,		GameMode::shareware,		std::string("Doom Shareware") },
	{ std::string("chex.wad"),		GameMission_t::pack_chex,	GameMode::retail,			std::string("Chex Quest") },
	{ std::string("hacx.wad"),		GameMission_t::pack_hacx,	GameMode::commercial,		std::string("Hacx") },
	{ std::string("freedoom2.wad"),	GameMission_t::doom2,		GameMode::commercial,		std::string("Freedoom: Phase 2") },
	{ std::string("freedoom1.wad"),	GameMission_t::doom,		GameMode::retail,			std::string("Freedoom: Phase 1") },
	{ std::string("freedm.wad"),	GameMission_t::doom2,		GameMode::commercial,		std::string("FreeDM") },
	{ std::string("heretic.wad"),	GameMission_t::heretic,		GameMode::retail,			std::string("Heretic") },
	{ std::string("heretic1.wad"),	GameMission_t::heretic,		GameMode::shareware,		std::string("Heretic Shareware") },
	{ std::string("hexen.wad"),		GameMission_t::hexen,		GameMode::commercial,		std::string("Hexen") },
	//{ std::string("strife0.wad"),	GameMission_t::strife,		GameMode::commercial,		std::string("Strife") }, // haleyjd: STRIFE-FIXME
	{ std::string("strife1.wad"),	GameMission_t::strife,		GameMode::commercial,		std::string("Strife") }
};

bool D_IsIWADName(std::string& name)
{
	for (size_t i{0}; i < arrlen(iwads); ++i)
	{
		if (!strcasecmp(name.c_str(), iwads[i].name))
		{
			return true;
		}
	}

	return false;
}

static bool iwad_dirs_built{false};
std::vector<std::string> iwad_dirs;

void AddIWADDir(std::string&& directory)
{
	iwad_dirs.emplace_back(directory);
}

// This is Windows-specific code that automatically finds the location of installed IWAD files. The registry is inspected to find special
// keys installed by the Windows installers for various CD versions of Doom. From these keys we can deduce where to find an IWAD.
#if defined(_WIN32) && !defined(_WIN32_WCE)
#define UNINSTALLER_STRING "\\uninstl.exe /S "

// Keys installed by the various CD editions. These are actually the commands to invoke the uninstaller and look like this:
//		C:\Program Files\Path\uninstl.exe /S C:\Program Files\Path
// With some munging we can find where Doom was installed.

// From the persepctive of a 64-bit executable, 32-bit registry keys are located in a different spot.
#if _WIN64
#define SOFTWARE_KEY "Software\\Wow6432Node"
#else
#define SOFTWARE_KEY "Software"
#endif

static registry_value_t uninstall_values[]{
	// Ultimate Doom, CD version (Depths of Doom trilogy)
	{
		HKEY_LOCAL_MACHINE,
		SOFTWARE_KEY "\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\Ultimate Doom for Windows 95",
		"UninstallString"
	},

	// Doom II, CD version (Depths of Doom trilogy)
	{
		HKEY_LOCAL_MACHINE,
		SOFTWARE_KEY "\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\Doom II for Windows 95",
		"UninstallString"
	},

	// Final Doom
	{
		HKEY_LOCAL_MACHINE,
		SOFTWARE_KEY "\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\Final Doom for Windows 95",
		"UninstallString"
	},

	// Shareware version
	{
		HKEY_LOCAL_MACHINE,
		SOFTWARE_KEY "\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\Doom Shareware for Windows 95",
		"UninstallString"
	}
};

// Values installed by the GOG.com and Collector's Edition versions
static registry_value_t root_path_keys[]{
	// Doom Collector's Edition
	{
		HKEY_LOCAL_MACHINE,
		SOFTWARE_KEY "\\Activision\\DOOM Collector's Edition\\v1.0",
		"INSTALLPATH"
	},

	// Doom II
	{
		HKEY_LOCAL_MACHINE,
		SOFTWARE_KEY "\\GOG.com\\Games\\1435848814",
		"PATH"
	},

	// Doom 3: BFG Edition
	{
		HKEY_LOCAL_MACHINE,
		SOFTWARE_KEY "\\GOG.com\\Games\\1135892318",
		"PATH"
	},

	// Final Doom
	{
		HKEY_LOCAL_MACHINE,
		SOFTWARE_KEY "\\GOG.com\\Games\\1435848742",
		"PATH"
	},

	// Ultimate Doom
	{
		HKEY_LOCAL_MACHINE,
		SOFTWARE_KEY "\\GOG.com\\Games\\1435827232",
		"PATH"
	},

	// Strife: Veteran Edition
	{
		HKEY_LOCAL_MACHINE,
		SOFTWARE_KEY "\\GOG.com\\Games\\1432899949",
		"PATH"
	}
};

// Subdirectories of the above install path, where IWADs are installed.
static const char* root_path_subdirs[]{
	".",
	"Doom2",
	"Final Doom",
	"Ultimate Doom",
	"Plutonia",
	"TNT",
	"base\\wads"
};

// Location where Steam is installed
static registry_value_t steam_install_location{
	HKEY_LOCAL_MACHINE,
	SOFTWARE_KEY "\\Valve\\Steam",
	"InstallPath"
};

// Subdirs of the steam install directory where IWADs are found
static const char* steam_install_subdirs[]{
	"steamapps\\common\\doom 2\\base",
	"steamapps\\common\\final doom\\base",
	"steamapps\\common\\ultimate doom\\base",
	"steamapps\\common\\heretic shadow of the serpent riders\\base",
	"steamapps\\common\\hexen\\base",
	"steamapps\\common\\hexen deathkings of the dark citadel\\base",

	// From Doom 3: BFG Edition:
	"steamapps\\common\\DOOM 3 BFG Edition\\base\\wads",

	// From Strife: Veteran Edition:
	"steamapps\\common\\Strife"
};

#define STEAM_BFG_GUS_PATCHES "steamapps\\common\\DOOM 3 BFG Edition\\base\\classicmusic\\instruments"

std::string GetRegistryString(registry_value_t* reg_val)
{
	HKEY key;

	// Open the key (directory where the value is stored)
	// RegOpenKeyEx(HKEY, LPCSTR, DWORD, REGSAM, PHKEY)
	if (RegOpenKeyEx(reg_val->root, reg_val->path, 0, KEY_READ, &key) != ERROR_SUCCESS)
	{
		return std::string{};
	}

	std::string result;

	// Find the type and length of the string, and only accept strings.
	if (DWORD len, valtype; RegQueryValueEx(key, reg_val->value, nullptr, &valtype, nullptr, &len) == ERROR_SUCCESS && valtype == REG_SZ)
	{
		// Allocate a buffer for the value and read the value
		auto t_result = std::make_unique<char[]>(len + 1);

		if (RegQueryValueEx(key, reg_val->value, nullptr, &valtype, (unsigned char*)t_result.get(), &len) != ERROR_SUCCESS)
		{
			t_result.reset();
		}
		else
		{
			// Ensure the value is null-terminated
			t_result[len] = '\0';
		}

		result = std::string(t_result.get());
	}

	// Close the key
	RegCloseKey(key);

	return result;
}

// Check for the uninstall strings from the CD versions
void CheckUninstallStrings()
{
	for (size_t i{0}, size{arrlen(uninstall_values)}; i < size; ++i)
	{
		auto val{GetRegistryString(&uninstall_values[i])};

		if (!val || *val == '\0')
		{
			continue;
		}

		auto unstr{strstr(*val, UNINSTALLER_STRING)};

		if (!unstr || *unstr == NULL)
		{
			val.reset();
		}
		else
		{
			AddIWADDir(std::string(unstr + UNINSTALLER_STRING));
		}
	}
}

// Check for GOG.com and Doom: Collector's Edition
void CheckInstallRootPaths()
{
	for (size_t i{0}; i < arrlen(root_path_keys); ++i)
	{
		auto install_path{GetRegistryString(&root_path_keys[i])};

		if (!install_path)
		{
			continue;
		}

		for (size_t j{0}; j < arrlen(root_path_subdirs); ++j)
		{
			AddIWADDir(std::string(*install_path + DIR_SEPARATOR_S + root_path_subdirs[j]));
		}
	}
}

// Check for Doom downloaded via Steam
void CheckSteamEdition()
{
	auto install_path{GetRegistryString(&steam_install_location)};

	if (!install_path)
	{
		return;
	}

	for (size_t i{0}; i < arrlen(steam_install_subdirs); ++i)
	{
		AddIWADDir(std::string(*install_path + DIR_SEPARATOR_S + steam_install_subdirs[i]));
	}
}

// The BFG edition ships with a full set of GUS patches. If we find them, we can autoconfigure to use them.
void CheckSteamGUSPatches()
{
	// Already configured? Don't stomp on the user's choices.
	auto current_path{M_GetStringVariable("gus_patch_path")};
	if (current_path && strlen(current_path) > 0)
	{
		return;
	}

	auto install_path{GetRegistryString(&steam_install_location)};
	if (!install_path)
	{
		return;
	}

	auto patch_path{std::string(*install_path + DIR_SEPARATOR_S + STEAM_BFG_GUS_PATCHES)};
	auto test_patch_path{M_StringJoin(*patch_path, "\\ACBASS.PAT")};

	// Does acbass.pat exist? If so, then set gus_patch_path.
	if (M_FileExists(*test_patch_path))
	{
		M_SetVariable("gus_patch_path", *patch_path);
	}
}

// Default install directories for DOS Doom
void CheckDOSDefaults()
{
	// These are the default install directories used by the deice installer program:
	AddIWADDir(std::string("\\doom2"));				// Doom II
	AddIWADDir(std::string("\\plutonia"));			// Final Doom
	AddIWADDir(std::string("\\tnt"));
	AddIWADDir(std::string("\\doom_se"));			// Ultimate Doom
	AddIWADDir(std::string("\\doom"));				// Shareware / Registered Doom
	AddIWADDir(std::string("\\dooms"));				// Shareware versions
	AddIWADDir(std::string("\\doomsw"));

	AddIWADDir(std::string("\\heretic"));			// Heretic
	AddIWADDir(std::string("\\hrtic_se"));			// Heretic Shareware from Quake disc

	AddIWADDir(std::string("\\hexen"));				// Hexen
	AddIWADDir(std::string("\\hexendk"));			// Hexen Deathkings of the Dark Citadel

	AddIWADDir(std::string("\\strife"));			// Strife
}

#endif // defined(_WIN32) && !defined(_WIN32_WCE)

// Returns true if the specified path is a path to a file of the specified name.
bool DirIsFile(std::string& path, std::string& filename)
{
	return strchr(path, DIR_SEPARATOR) != NULL && !strcasecmp(M_BaseName(path), filename);
}

// Check if the specified directory contains the specified IWAD file, returning the full path to the IWAD if found, or NULL if not found.
std::string CheckDirectoryHasIWAD(std::string& directory, std::string& iwadname)
{
	// As a special case, the "directory" may refer directly to an IWAD file if the path comes from DOOMWADDIR or DOOMWADPATH.
	auto probe{M_FileCaseExists(directory)};
	if (DirIsFile(directory, iwadname) && probe)
	{
		return probe;
	}

	// Construct the full path to the IWAD if it is located in this directory, and check if it exists.
	auto filename{[&](){
		if (!strcmp(directory, "."))
		{
			return std::string(iwadname);
		}
		else
		{
			return std::string(directory + DIR_SEPARATOR_S + iwadname);
		}
	}()};

	probe.reset();
	probe = M_FileCaseExists(*filename);

	if (probe && *probe != '\0')
	{
		return probe;
	}

	return std::string{};
}

// Search a directory to try to find an IWAD; returns the location of the IWAD if found, otherwise nullptr
std::string SearchDirectoryForIWAD(std::string& directory, int mask, GameMission_t* mission)
{
	for (size_t i{0}; i < arrlen(iwads); ++i)
	{
		if (((1 << iwads[i].mission) & mask) == 0)
		{
			continue;
		}

		auto filename{CheckDirectoryHasIWAD(directory, DEH_String(iwads[i].name))};

		if (filename)
		{
			*mission = iwads[i].mission;

			return filename;
		}
	}

	return std::string{};
}

// When given an IWAD with the '-iwad' parameter, attempt to identify it by its name.
auto IdentifyIWADByName(std::string& name, int mask)
{
	name = M_BaseName(name);
	auto mission = GameMission_t::none;

	for (size_t i{0}; i < arrlen(iwads); ++i)
	{
		// Check if the filename is this IWAD name.

		// Only use supported missions:
		if (((1 << iwads[i].mission) & mask) == 0)
		{
			continue;
		}

		// Check if it ends in this IWAD name.
		if (!strcasecmp(name, iwads[i].name))
		{
			mission = iwads[i].mission;
			break;
		}
	}

	return mission;
}

// Add IWAD directories parsed from splitting a path string containing paths separated by PATH_SEPARATOR.
// 'suffix' is a string to concatenate to the end of the paths before adding them.
void AddIWADPath(std::string&& path)
{
	//for (;;)
	//{
		//auto p{strchr(left, PATH_SEPARATOR)};
		//if (p)
		//{
			// Break at the separator and use the left hand side as another iwad directory
			// TODO what is this trying to do? modify a std::string for what purpose? INVESTIGATE
			//*p = '\0';
			//AddIWADDir(M_StringJoin(left, suffix));
			//left = p + 1;
		//}
		//else
		//{
			//break;
		//}
	//}

	AddIWADDir(path);
}

#ifndef _WIN32
// Add standard directories where IWADs are located on Unix systems.
// To respect the freedesktop.org specification we support overriding using standard environment variables.
// See the XDG Base Directory Specification:
// <http://standards.freedesktop.org/basedir-spec/basedir-spec-latest.html>
void AddXdgDirs()
{
	// $XDG_DATA_HOME defines the base directory relative to which user specific data files should be stored.
	// If $XDG_DATA_HOME is either not set or empty, a default equal to $HOME/.local/share should be used.
	//auto env{getenv("XDG_DATA_HOME")};
	auto env{getenv("XDG_DATA_HOME")};
	std::string tstr;

	if (!env || *env == '\0')
	{
		auto homedir{getenv("HOME")};
		if (!homedir || *homedir == '\0')
		{
			homedir = "/";
		}

		tstr = std::string(homedir + "/.local/share");
		env = tstr.c_str();
	}

	// We support $XDG_DATA_HOME/games/doom (which will usually be ~/.local/share/games/doom)
	// as a user-writeable extension to the usual /usr/share/games/doom location.
	AddIWADDir(std::string(env + "/games/doom"));

	// $XDG_DATA_DIRS defines the preference-ordered set of base directories to search for data files, in addition to the $XDG_DATA_HOME base directory.
	// The directories in $XDG_DATA_DIRS should be seperated with a colon ':'.
	// If $XDG_DATA_DIRS is either not set or empty, a value equal to /usr/local/share/:/usr/share/ should be used.
	env = getenv("XDG_DATA_DIRS");
	if (!env || *env == '\0')
	{
		// (Trailing / omitted from paths, as it is added below)
		env = "/usr/local/share:/usr/share";
	}

	// The "standard" location for IWADs on Unix that is supported by most source ports is /usr/share/games/doom 
	// we support this through the XDG_DATA_DIRS mechanism, through which it can be overridden.
	AddIWADPath(std::string(env + "/games/doom"));
	AddIWADPath(std::string(env + "/doom"));

	// The convention set by RBDOOM-3-BFG is to install Doom 3: BFG Edition into this directory, under which includes the Doom Classic WADs.
	AddIWADPath(std::string(env + "/games/doom3bfg/base/wads"));
}

#ifndef __MACOSX__
// Steam on Linux allows installing some select Windows games, including the classic Doom series (running DOSBox via Wine). We could
// parse *.vdf files to more accurately detect installation locations, but the defaults are likely to be good enough for just about everyone.
void AddSteamDirs()
{
	auto homedir{getenv("HOME")};
	if (!homedir || *homedir == '\0')
	{
		homedir = "/";
	}

	AddIWADPath(std::string(homedir + "/.steam/root/steamapps/common" + "/Doom 2/base"));
	AddIWADPath(std::string(homedir + "/.steam/root/steamapps/common" + "/Master Levels of Doom/doom2"));
	AddIWADPath(std::string(homedir + "/.steam/root/steamapps/common" + "/Ultimate Doom/base"));
	AddIWADPath(std::string(homedir + "/.steam/root/steamapps/common" + "/Final Doom/base"));
	AddIWADPath(std::string(homedir + "/.steam/root/steamapps/common" + "/DOOM 3 BFG Edition/base/wads"));
	AddIWADPath(std::string(homedir + "/.steam/root/steamapps/common" + "/Heretic Shadow of the Serpent Riders/base"));
	AddIWADPath(std::string(homedir + "/.steam/root/steamapps/common" + "/Hexen/base"));
	AddIWADPath(std::string(homedir + "/.steam/root/steamapps/common" + "/Hexen Deathkings of the Dark Citadel/base"));
	AddIWADPath(std::string(homedir + "/.steam/root/steamapps/common" + "/Strife"));
}
#endif // __MACOSX__
#endif // !_WIN32

// Build a list of IWAD files
void BuildIWADDirList()
{
	if (iwad_dirs_built)
	{
		return;
	}

	// Look in the current directory. Doom always does this.
	AddIWADDir(std::string("."));

	// Next check the directory where the executable is located. This might be different from the current directory.
	AddIWADDir(M_DirName(myargv[0]));

	// Add DOOMWADDIR if it is in the environment
	auto env{getenv("DOOMWADDIR")};
	if (env)
	{
		AddIWADDir(std::string(env));
	}

	// Add dirs from DOOMWADPATH:
	env = getenv("DOOMWADPATH");
	if (env)
	{
		AddIWADPath(std::string(env));
	}

#ifdef _WIN32
	// Search the registry and find where IWADs have been installed.
	CheckUninstallStrings();
	CheckInstallRootPaths();
	CheckSteamEdition();
	CheckDOSDefaults();

	// Check for GUS patches installed with the BFG edition!
	CheckSteamGUSPatches();
#else
	AddXdgDirs();
	#ifndef __MACOSX__
		AddSteamDirs();
	#endif
#endif

	// Don't run this function again.
	iwad_dirs_built = true;
}

// Searches WAD search paths for an WAD with a specific filename.
std::string D_FindWADByName(std::string name)
{
	// Absolute path?
	auto probe{M_FileCaseExists(name)};
	if (probe)
	{
		return probe;
	}

	BuildIWADDirList();

	// Search through all IWAD paths for a file with the given name.
	//for (size_t i{0}; i < num_iwad_dirs; ++i)
	for (auto& iter: iwad_dirs)
	{
		// As a special case, if this is in DOOMWADDIR or DOOMWADPATH, the "directory" may actually refer directly to an IWAD file.
		probe = M_FileCaseExists(iter);
		if (DirIsFile(iter, name) && probe)
		{
			return probe;
		}

		// Construct a string for the full path
		auto path{std::string(iter + DIR_SEPARATOR_S + name))};

		probe = M_FileCaseExists(*path);
		if (probe)
		{
			return probe;
		}
	}

	// File not found
	return static_cast<decltype(probe)>(nullptr);
}

// Searches for a WAD by its filename, or returns a copy of the filename if not found.
std::string D_TryFindWADByName(std::string filename)
{
	auto result{D_FindWADByName(filename)};

	if (!result || *result == '\0')
	{
		result = M_StringDuplicate(filename);
	}

	return result;
}

// Checks availability of IWAD files by name, to determine whether registered/commercial features should be executed (notably loading PWADs).
std::string D_FindIWAD(int mask, GameMission_t* /*OUT*/ mission)
{
	std::string result;

	// Check for the -iwad parameter
	if (size_t iwadparm{static_cast<size_t>(M_CheckParmWithArgs("-iwad", 1))}; iwadparm)
	{
		// Search through IWAD dirs for an IWAD with the given name.
		std::string iwadfile{myargv[iwadparm + 1]};

		result = D_FindWADByName(iwadfile);

		if (!result)
		{
			I_Error("IWAD file '%s' not found!", iwadfile);
		}

		*mission = IdentifyIWADByName(result, mask);
	}
	else
	{
		// Search through the list and look for an IWAD
		BuildIWADDirList();

		//for (size_t i{0}; result == nullptr && i < num_iwad_dirs; ++i)
		for (auto& iter : iwad_dirs)
		{
			// FIXME
			result = SearchDirectoryForIWAD(iter, mask, mission);
			if (result != std::string{})
			{
				break;
			}
		}
	}

	return result;
}

const iwad_t** D_FindAllIWADs(int mask)
{
	const iwad_t** result = static_cast<decltype(result)>(malloc(sizeof(iwad_t*) * (arrlen(iwads) + 1)));
	auto result_len{0};

	// Try to find all IWADs
	for (size_t i{0}; i < arrlen(iwads); ++i)
	{
		//if (((1 << iwads[i].mission) & mask) == 0)
		//{
		//	continue;
		//}

		//FORALL IWAD.MISSION contained in SET of NOT MASK add to result

		auto filename{D_FindWADByName(iwads[i].name)};

		if (filename && *filename != '\0')
		{
			result[result_len] = &iwads[i];
			++result_len;
		}
	}

	// End of list
	result[result_len] = nullptr;

	return result;
}

// Get the IWAD name used for savegames.
const char* D_SaveGameIWADName(GameMission_t gamemission, GameVariant_t gamevariant)
{
	// Determine the IWAD name to use for savegames. This determines the directory the savegame files get put into.
	// Note that we match on gamemission rather than on IWAD name. This ensures that doom1.wad and doom.wad saves are stored in the same place.
	if (gamevariant == GameVariant_t::freedoom)
	{
		if (gamemission == GameMission_t::doom)
		{
			return (const char*)"freedoom1.wad";
		}
		else if (gamemission == GameMission_t::doom2)
		{
			return (const char*)"freedoom2.wad";
		}
	}
	else if (gamevariant == GameVariant_t::freedm && gamemission == GameMission_t::doom2)
	{
		return (const char*)"freedm.wad";
	}

	for (size_t i{0}; i < arrlen(iwads); ++i)
	{
		if (gamemission == iwads[i].mission)
		{
			return iwads[i].name;
		}
	}

	// Default fallback:
	return (const char*)"unknown.wad";
}

const char* D_SuggestIWADName(GameMission_t mission, GameMode mode)
{
	for (size_t i{0}; i < arrlen(iwads); ++i)
	{
		if (iwads[i].mission == mission && iwads[i].mode == mode)
		{
			return iwads[i].name;
		}
	}

	return (const char*)"unknown.wad";
}

const char* D_SuggestGameName(GameMission_t mission, GameMode mode)
{
	for (size_t i{0}; i < arrlen(iwads); ++i)
	{
		if (iwads[i].mission == mission	&& (mode == GameMode::indetermined || iwads[i].mode == mode))
		{
			return iwads[i].description;
		}
	}

	return (const char*)"Unknown game?";
}
