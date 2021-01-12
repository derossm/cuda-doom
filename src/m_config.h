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

#include <string>

#include "../derma/common.h"

#include "doomtype.h"

void M_LoadDefaults();
void M_SaveDefaults();
void M_SaveDefaultsAlternate(::std::string main, ::std::string extra);
void M_SetConfigDir(::std::string dir);
void M_SetMusicPackDir();

// Location where all configuration data is stored - default.cfg, savegames, etc.
::std::string configdir;

::std::string autoload_path = "";

// Default filenames for configuration files.
static ::std::string default_main_config;
static ::std::string default_extra_config;

enum class default_type_t
{
	INT,
	INT_HEX,
	STRING,
	FLOAT,
	KEY
};

struct default_t
{
	// Name of the variable
	::std::string name;

	// Pointer to the location in memory of the variable
	union
	{
		int* i;
		::std::string* s;
		float* f;
	} location;

	// Type of the variable
	default_type_t type;

	// If this is a key value, the original integer scancode we read from
	// the config file before translating it to the internal key value.
	// If zero, we didn't read this value from a config file.
	int untranslated;

	// The value we translated the scancode into when we read the
	// config file on startup. If the variable value is different from
	// this, it has been changed and needs to be converted; otherwise,
	// use the 'untranslated' value.
	int original_translated;

	// If true, this config variable has been bound to a variable and is being used.
	bool bound;
};

struct default_collection_t
{
	default_t* defaults;
	int numdefaults;
	::std::string filename;
};

#define CONFIG_VARIABLE_GENERIC(name, type) { #name, {NULL}, type, 0, 0, false }

#define CONFIG_VARIABLE_KEY(name) CONFIG_VARIABLE_GENERIC(name, default_type_t::KEY)
#define CONFIG_VARIABLE_INT(name) CONFIG_VARIABLE_GENERIC(name, default_type_t::INT)
#define CONFIG_VARIABLE_INT_HEX(name) CONFIG_VARIABLE_GENERIC(name, default_type_t::INT_HEX)
#define CONFIG_VARIABLE_FLOAT(name) CONFIG_VARIABLE_GENERIC(name, default_type_t::FLOAT)
#define CONFIG_VARIABLE_STRING(name) CONFIG_VARIABLE_GENERIC(name, default_type_t::STRING)

template<typename T>
requires ::std::integral<typename ::std::remove_pointer<T>::type>
void M_BindIntVariable(::std::string name, T location)
{
	default_t* variable;

	variable = GetDefaultForName(name);
	assert(variable->type == default_type_t::INT
		|| variable->type == default_type_t::INT_HEX
		|| variable->type == default_type_t::KEY);

	variable->location.i = location;
	variable->bound = true;
}

template<typename T>
requires ::std::floating_point<typename ::std::remove_pointer<T>::type>
void M_BindFloatVariable(::std::string name, T location)
{
	default_t* variable;

	variable = GetDefaultForName(name);
	assert(variable->type == default_type_t::FLOAT);

	variable->location.f = location;
	variable->bound = true;
}

bool M_SetVariable(::std::string name, ::std::string value);
int M_GetIntVariable(::std::string name);
::std::string M_GetStringVariable(::std::string name);
float M_GetFloatVariable(::std::string name);
void M_SetConfigFilenames(::std::string main_config, ::std::string extra_config);
::std::string M_GetSaveGameDir(::std::string iwadname);
::std::string M_GetAutoloadDir(::std::string iwadname);

extern ::std::string configdir;
