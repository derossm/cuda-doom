/**********************************************************************************************************************************************\
	Copyright(C) 1993-1996 Id Software, Inc.
	Copyright(C) 2005-2014 Simon Howard

	This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License
	as published by the Free Software Foundation; either version 2 of the License, or (at your option) any later version.

	This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

	DESCRIPTION:
		Configuration file interface.
\**********************************************************************************************************************************************/
#pragma once

#include "../build/config.h"

#include <SDL_filesystem.h>

#include "../derma/common.h"
#include "../derma/keybinds.h"

#include "doomtype.h"
#include "i_system.h"
#include "m_argv.h"
#include "m_misc.h"

#include "z_zone.h"

void M_LoadDefaults();
void M_SaveDefaults();
void M_SaveDefaultsAlternate(::std::string main, ::std::string extra);
void M_SetConfigDir(::std::string dir);
void M_SetMusicPackDir();

// Location where all configuration data is stored - default.cfg, savegames, etc.
::std::string configdir;

::std::string autoload_path = "";

// Default filenames for configuration files.
static ::std::filesystem::path default_main_config;
static ::std::filesystem::path default_extra_config;

enum class
[[deprecated("OBSOLETE")]] default_type_t
{
	INT,
	INT_HEX,
	STRING,
	FLOAT,
	KEY
};

template<typename T>
struct default_t
{
	using type = T;

	T location;

	// Name of the variable
	::std::string name;

	// If true, this config variable has been bound to a variable and is being used.
	bool bound;
};

template<typename T>
struct default_collection_t
{
	std::vector<default_t<T>> defaults;
	::std::fielsystem::path configFile;
};

template<typename T>
//requires ::std::integral<typename ::std::remove_pointer<T>::type>
void M_BindVariable(::std::string name, T* location)
{
	default_t* variable;

	variable = GetDefaultForName(name);
	//assert(variable->type == default_type_t::INT || variable->type == default_type_t::INT_HEX || variable->type == default_type_t::KEY);

	variable->location.i = location;
	variable->bound = true;
}

/*template<typename T>
requires ::std::floating_point<typename ::std::remove_pointer<T>::type>
void M_BindFloatVariable(::std::string name, T location)
{
	default_t* variable;

	variable = GetDefaultForName(name);
	assert(variable->type == default_type_t::FLOAT);

	variable->location.f = location;
	variable->bound = true;
}*/

//bool M_SetVariable(::std::string name, ::std::string value);
//int M_GetIntVariable(::std::string name);
//::std::string M_GetStringVariable(::std::string name);
//float M_GetFloatVariable(::std::string name);
//void M_SetConfigFilenames(::std::string main_config, ::std::string extra_config);
//::std::string M_GetSaveGameDir(::std::string iwadname);
//::std::string M_GetAutoloadDir(::std::string iwadname);

//::std::string configdir;

// Search a collection for a variable
default_t* SearchCollection(default_collection_t* collection, ::std::string name)
{
	int i;

	for (i = 0; i < collection->numdefaults; ++i)
	{
		if (!name.compare(collection->defaults[i].name))
		{
			return &collection->defaults[i];
		}
	}

	return nullptr;
}

// Mapping from DOS keyboard scan code to internal key code (as defined
// in doomkey.h). I think I (fraggle) reused this from somewhere else
// but I can't find where. Anyway, notes:
// * KEY_PAUSE is wrong - it's in the KEY_NUMLOCK spot. This shouldn't
//	matter in terms of Vanilla compatibility because neither of
//	those were valid for key bindings.
// * There is no proper scan code for PrintScreen (on DOS machines it
//	sends an interrupt). So I added a fake scan code of 126 for it.
//	The presence of this is important so we can bind PrintScreen as
//	a screenshot key.
/*static const Keys scantokey[128] =
{
	Keys::null,			Keys::null,		Keys::KEY_1,		Keys::KEY_2,		Keys::KEY_3,		Keys::KEY_4,		Keys::KEY_5,		Keys::KEY_6,
	Keys::KEY_7,		Keys::KEY_8,	Keys::KEY_9,		Keys::KEY_0,		Keys::MINUS,		Keys::EQUALS,		Keys::BACKSPACE,	Keys::null,
	Keys::KEY_Q,		Keys::KEY_W,	Keys::KEY_E,		Keys::KEY_R,		Keys::KEY_T,		Keys::KEY_Y,		Keys::KEY_U,		Keys::KEY_I,
	Keys::KEY_O,		Keys::KEY_P,	Keys::LEFTBRACKET,	Keys::RIGHTBRACKET,	Keys::null,			Keys::RCTRL,		Keys::KEY_A,		Keys::KEY_S,
	Keys::KEY_D,		Keys::KEY_F,	Keys::KEY_G,		Keys::KEY_H,		Keys::KEY_J,		Keys::KEY_K,		Keys::KEY_L,		Keys::SEMICOLON,
	Keys::APOSTROPHE,	Keys::GRAVE,	Keys::RSHIFT,		Keys::BACKSLASH,	Keys::KEY_Z,		Keys::KEY_X,		Keys::KEY_C,		Keys::KEY_V,
	Keys::KEY_B,		Keys::KEY_N,	Keys::KEY_M,		Keys::COMMA,		Keys::PERIOD,		Keys::SLASH,		Keys::RSHIFT,		Keys::KP_MULTIPLY,
	Keys::RALT,			Keys::SPACE,	Keys::CAPSLOCK,		Keys::F1, 			Keys::F2,			Keys::F3,			Keys::F4,			Keys::F5,
	Keys::F6,			Keys::F7,		Keys::F8,			Keys::F9,			Keys::F10,			Keys::PAUSE,		Keys::SCROLLLOCK,	Keys::HOME,
	Keys::UP,			Keys::PAGEUP,	Keys::KP_MINUS,		Keys::LEFT,			Keys::KP_5,			Keys::RIGHT,		Keys::KP_PLUS,		Keys::END,
	Keys::DOWN,			Keys::PAGEDOWN,	Keys::INSERT,		Keys::DEL,			Keys::null,			Keys::null,			Keys::null,			Keys::F11,
	Keys::F12,			Keys::null,		Keys::null,			Keys::null,			Keys::null,			Keys::null,			Keys::null,			Keys::null,
	Keys::null,			Keys::null,		Keys::null,			Keys::null,			Keys::null,			Keys::null,			Keys::null,			Keys::null,
	Keys::null,			Keys::null,		Keys::null,			Keys::null,			Keys::null,			Keys::null,			Keys::null,			Keys::null,
	Keys::null,			Keys::null,		Keys::null,			Keys::null,			Keys::null,			Keys::null,			Keys::null,			Keys::null,
	Keys::null,			Keys::null,		Keys::null,			Keys::null,			Keys::null,			Keys::null,			Keys::PRINTSCREEN,	Keys::null
};*/
// WHY BOTHER SAVING IN A NON-STANDARD FORMAT? JUST STANDARDIZE EVERYWHERE
/*
void SaveDefaultCollection(default_collection_t* collection)
{
	default_t* defaults;
	int i;
	int v;
	FILE* f;

	f = fopen(collection->filename.c_str(), "w");
	if (!f)
	{
		return; // can't write the file, but don't complain
	}
	defaults = collection->defaults;

	for (i = 0; i < collection->numdefaults; ++i)
	{
		int chars_written;

		// Ignore unbound variables
		if (!defaults[i].bound)
		{
			continue;
		}

		// Print the name and line up all values at 30 characters
		chars_written = fprintf(f, "%s ", defaults[i].name.c_str());

		for (; chars_written < 30; ++chars_written)
		{
			fprintf(f, " ");
		}

		// Print the value
		switch (defaults[i].type)
		{
		case default_type_t::KEY:

			// use the untranslated version if we can, to reduce the possibility of screwing up the user's config file
			v = *defaults[i].location.i;

			if (v == Keys::RSHIFT)
			{
				// Special case: for shift, force scan code for
				// right shift, as this is what Vanilla uses.
				// This overrides the change check below, to fix
				// configuration files made by old versions that
				// mistakenly used the scan code for left shift.

				v = 54;
			}
			else if (defaults[i].untranslated && v == defaults[i].original_translated)
			{
				// Has not been changed since the last time we read the config file.
				v = defaults[i].untranslated;
			}
			else
			{
				// search for a reverse mapping back to a scancode in the scantokey table
				int s;
				for (s = 0; s < 128; ++s)
				{
					if (scantokey[s] == v)
					{
						v = s;
						break;
					}
				}
			}

			fprintf(f, "%i", v);
			break;

		case default_type_t::INT:
			fprintf(f, "%i", *defaults[i].location.i);
			break;

		case default_type_t::INT_HEX:
			fprintf(f, "0x%x", *defaults[i].location.i);
			break;

		case default_type_t::FLOAT:
			fprintf(f, "%f", *defaults[i].location.f);
			break;

		case default_type_t::STRING:
			fprintf(f, "\"%s\"", *defaults[i].location.s);
			break;
		}

		fprintf(f, "\n");
	}

	fclose(f);
}
*/
// Parses integer values in the configuration file
int ParseIntParameter(::std::string strparm)
{
	int parm;

	if (strparm[0] == '0' && strparm[1] == 'x')
	{
		// FIXME
		//sscanf(strparm+2, "%x", (unsigned*) &parm);
	}
	else
	{
		// FIXME
		//sscanf(strparm, "%i", &parm);
	}

	return parm;
}

void SetVariable(default_t* def, ::std::string value)
{
	int intparm;

	// parameter found
	switch (def->type)
	{
	case default_type_t::STRING:
		*def->location.s = ::std::string(value);
		break;

	case default_type_t::INT:
	case default_type_t::INT_HEX:
		*def->location.i = ParseIntParameter(value);
		break;

	case default_type_t::KEY:

		// translate scancodes read from config file (save the old value in untranslated)
		intparm = ParseIntParameter(value);
		def->untranslated = intparm;
		if (intparm >= 0 && intparm < 128)
		{
			intparm = scantokey[intparm];
		}
		else
		{
			intparm = 0;
		}

		def->original_translated = intparm;
		*def->location.i = intparm;
		break;

	case default_type_t::FLOAT:
		*def->location.f = (float)atof(value.c_str());
		break;
	}
}

void LoadDefaultCollection(default_collection_t* collection)
{
	FILE* f;
	default_t* def;
	char defname[80];
	char strparm[100];

	// read the file in, overriding any set defaults
	f = fopen(collection->filename.c_str(), "r");

	if (f == NULL)
	{
		// File not opened, but don't complain. It's probably just the first time they ran the game.
		return;
	}

	while (!feof(f))
	{
		if (fscanf(f, "%79s %99[^\n]\n", defname, strparm) != 2)
		{
			// This line doesn't match
			continue;
		}

		// Find the setting in the list
		def = SearchCollection(collection, defname);

		if (def == NULL || !def->bound)
		{
			// Unknown variable? Unbound variables are also treated as unknown.
			continue;
		}

		// Strip off trailing non-printable characters (\r characters from DOS text files)
		while (strlen(strparm) > 0 && !isprint(strparm[strlen(strparm) - 1]))
		{
			strparm[strlen(strparm) - 1] = '\0';
		}

		// Surrounded by quotes? If so, remove them.
		if (strlen(strparm) >= 2 && strparm[0] == '"' && strparm[strlen(strparm) - 1] == '"')
		{
			strparm[strlen(strparm) - 1] = '\0';
			memmove(strparm, strparm + 1, sizeof(strparm) - 1);
		}

		SetVariable(def, strparm);
	}

	fclose(f);
}

void M_SetConfigFilenames(::std::string main_config, ::std::string extra_config)
{
	default_main_config = main_config;
	default_extra_config = extra_config;
}

void M_SaveDefaults()
{
	SaveDefaultCollection(&doom_defaults);
	SaveDefaultCollection(&extra_defaults);
}

void M_SaveDefaultsAlternate(::std::string main, ::std::string extra)
{
	::std::string orig_main;
	::std::string orig_extra;

	// Temporarily change the filenames
	orig_main = doom_defaults.filename;
	orig_extra = extra_defaults.filename;

	doom_defaults.filename = main;
	extra_defaults.filename = extra;

	M_SaveDefaults();

	// Restore normal filenames
	doom_defaults.filename = orig_main;
	extra_defaults.filename = orig_extra;
}

void M_LoadDefaults()
{
	int i;

	// This variable is a special snowflake for no good reason.
	M_BindStringVariable("autoload_path", &autoload_path);

	// check for a custom default file

	// Load main configuration from the specified file, instead of the default.
	i = M_CheckParmWithArgs("-config", 1);

	if (i)
	{
		doom_defaults.filename = myargv[i + 1];
		printf("\tdefault file: %s\n", doom_defaults.filename);
	}
	else
	{
		doom_defaults.filename = ::std::string(configdir + default_main_config);
	}

	printf("saving config in %s\n", doom_defaults.filename);

	// Load additional configuration from the specified file, instead of the default.
	i = M_CheckParmWithArgs("-extraconfig", 1);

	if (i)
	{
		extra_defaults.filename = myargv[i + 1];
		printf("\t\textra configuration file: %s\n", extra_defaults.filename);
	}
	else
	{
		extra_defaults.filename = ::std::string(configdir + default_extra_config);
	}

	LoadDefaultCollection(&doom_defaults);
	LoadDefaultCollection(&extra_defaults);
}

// Get a configuration file variable by its name
default_t* GetDefaultForName(::std::string name)
{
	default_t* result;

	// Try the main list and the extras
	result = SearchCollection(&doom_defaults, name);

	if (result == NULL)
	{
		result = SearchCollection(&extra_defaults, name);
	}

	// Not found? Internal error.
	if (result == NULL)
	{
		I_Error("Unknown configuration variable: '%s'", name);
	}

	return result;
}

// Bind a variable to a given configuration file variable, by name.
/* template<typename T>
requires ::std::integral<typename ::std::remove_pointer<T>::type>
void M_BindVariable<int>(::std::string name, T location)
{
	default_t* variable;

	variable = GetDefaultForName(name);
	//assert(variable->type == default_type_t::INT
	//	|| variable->type == default_type_t::INT_HEX
	//	|| variable->type == default_type_t::KEY);

	variable->location.i = location;
	variable->bound = true;
}

template<typename T>
requires ::std::floating_point<typename ::std::remove_pointer<T>::type>
void M_BindFloatVariable(::std::string name, T location)
{
	default_t* variable;

	variable = GetDefaultForName(name);
	//assert(variable->type == default_type_t::FLOAT);

	variable->location.f = location;
	variable->bound = true;
} */

void M_BindStringVariable(::std::string& name, ::std::string& location)
{
	default_t* variable = GetDefaultForName(name);
	assert(variable->type == default_type_t::STRING);

	variable->location.s = &location;
	variable->bound = true;
}

// Set the value of a particular variable; an API function for other parts of the program to assign values to config variables by name.
bool M_SetVariable(::std::string name, ::std::string value)
{
	default_t* variable = GetDefaultForName(name);

	if (variable == NULL || !variable->bound)
	{
		return false;
	}

	SetVariable(variable, value);

	return true;
}

// Get the value of a variable.
int M_GetIntVariable(::std::string name)
{
	default_t* variable = GetDefaultForName(name);

	if (variable == NULL || !variable->bound
		|| (variable->type != default_type_t::INT && variable->type != default_type_t::INT_HEX))
	{
		return 0;
	}

	return *variable->location.i;
}

::std::string M_GetStringVariable(::std::string name)
{
	default_t* variable = GetDefaultForName(name);

	if (variable == NULL || !variable->bound || variable->type != default_type_t::STRING)
	{
		return nullptr;
	}

	return *variable->location.s;
}

float M_GetFloatVariable(::std::string name)
{
	default_t* variable = GetDefaultForName(name);

	if (variable == NULL || !variable->bound || variable->type != default_type_t::FLOAT)
	{
		return 0;
	}

	return *variable->location.f;
}

// Get the path to the default configuration dir to use, if NULL is passed to M_SetConfigDir.
::std::string GetDefaultConfigDir()
{
#if !defined(_WIN32) || defined(_WIN32_WCE)

	// Configuration settings are stored in an OS-appropriate path
	// determined by SDL. On typical Unix systems, this might be
	// ~/.local/share/chocolate-doom. On Windows, we behave like
	// Vanilla Doom and save in the current directory.
	::std::string result;
	::std::string copy;

	result = SDL_GetPrefPath("", PACKAGE_TARNAME);
	if (result != NULL)
	{
		copy = ::std::string(result);
		SDL_free(result);
		return copy;
	}
#endif /* #ifndef _WIN32 */
	return ::std::string("");
}

// Sets the location of the configuration directory, where configuration files are stored - default.cfg, chocolate-doom.cfg, savegames, etc.
void M_SetConfigDir(::std::string dir)
{
	// Use the directory that was passed, or find the default.
	if (!dir.empty())
	{
		configdir = dir;
	}
	else
	{
		configdir = GetDefaultConfigDir();
	}

	if (configdir.compare(""))
	{
		printf("Using %s for configuration and saves\n", configdir);
	}

	// Make the directory if it doesn't already exist:
	M_MakeDirectory(configdir);
}

#define MUSIC_PACK_README \
"Extract music packs into this directory in .flac or .ogg format;\n"	\
"they will be automatically loaded based on filename to replace the\n" \
"in-game music with high quality versions.\n\n" \
"For more information check here:\n\n" \
" <https://www.chocolate-doom.org/wiki/index.php/Digital_music_packs>\n\n"

// Set the value of music_pack_path if it is currently empty, and create the directory if necessary.
void M_SetMusicPackDir()
{
	::std::string current_path;
	::std::string prefdir;
	::std::string music_pack_path;
	::std::string readme_path;

	current_path = M_GetStringVariable("music_pack_path");

	if (!current_path.empty())
	{
		return;
	}

	prefdir = SDL_GetPrefPath("", PACKAGE_TARNAME);
	music_pack_path = ::std::string(prefdir + "music-packs");

	M_MakeDirectory(prefdir);
	M_MakeDirectory(music_pack_path);
	M_SetVariable("music_pack_path", music_pack_path);

	// We write a README file with some basic instructions on how to use the directory.
	readme_path = ::std::string(music_pack_path + DIR_SEPARATOR_S + "README.txt");
	M_WriteFile(readme_path, MUSIC_PACK_README, strlen(MUSIC_PACK_README));

	//free(readme_path);
	//free(music_pack_path);
	// FIXME ???
	//SDL_free(prefdir);
}

// Calculate the path to the directory to use to store save games. Creates the directory as necessary.
char* M_GetSaveGameDir(::std::string iwadname)
{
/* DIRECTORY INTERACTION SHOULD NOT BE DONE HERE - ASK FILESYSTEM FOR DIRECTORY
	::std::string savegamedir;
	::std::string topdir;
	int p;

	// Specify a path from which to load and save games. If the directory does not exist then it will automatically be created.
	p = M_CheckParmWithArgs("-savedir", 1);
	if (p)
	{
		savegamedir = myargv[p + 1];
		if (!M_FileExists(savegamedir))
		{
			M_MakeDirectory(savegamedir);
		}

		// add separator at end just in case
		savegamedir = ::std::string(savegamedir + DIR_SEPARATOR_S);

		printf("Save directory changed to %s.\n", savegamedir);
	}
#ifdef _WIN32
	// In -cdrom mode, we write savegames to a specific directory in addition to configs.
	else if (M_ParmExists("-cdrom"))
	{
		savegamedir = ::std::string(configdir);
	}
#endif
	// If not "doing" a configuration directory (Windows), don't "do" a savegame directory, either.
	else if (!configdir.compare(""))
	{
		savegamedir = ::std::string("");
	}
	else
	{
		// ~/.local/share/chocolate-doom/savegames
		topdir = ::std::string(configdir + "savegames");
		M_MakeDirectory(topdir);

		// eg. ~/.local/share/chocolate-doom/savegames/doom2.wad/
		savegamedir = ::std::string(topdir + DIR_SEPARATOR_S + iwadname + DIR_SEPARATOR_S);

		M_MakeDirectory(savegamedir);

		//free(topdir);
	}
*/
	return savegamedir;
}

// Calculate the path to the directory for autoloaded WADs/DEHs. Creates the directory as necessary.
::std::string M_GetAutoloadDir(::std::string iwadname)
{
	::std::string result;

/* DIRECTORY INTERACTION SHOULD NOT BE DONE HERE - ASK FILESYSTEM FOR DIRECTORY
	if (autoload_path.empty())
	{
		::std::string prefdir;
		prefdir = SDL_GetPrefPath("", PACKAGE_TARNAME);
		autoload_path = ::std::string(prefdir + "autoload");
		// FIXME ???
		//SDL_free(prefdir);
	}

	M_MakeDirectory(autoload_path);

	result = ::std::string(autoload_path + DIR_SEPARATOR_S + iwadname);
	M_MakeDirectory(result);

	// TODO: Add README file
*/
	return result;
}
