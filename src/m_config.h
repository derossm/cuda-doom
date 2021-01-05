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

#include "../derma/common.h"

#include "doomtype.h"

void M_LoadDefaults();
void M_SaveDefaults();
void M_SaveDefaultsAlternate(std::string main, std::string extra);
void M_SetConfigDir(std::string dir);
void M_SetMusicPackDir();

template<typename T>
requires std::integral<typename std::remove_pointer<T>::type>
void M_BindIntVariable(std::string name, T location)
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
requires std::floating_point<typename std::remove_pointer<T>::type>
void M_BindFloatVariable(std::string name, T location)
{
	default_t* variable;

	variable = GetDefaultForName(name);
	assert(variable->type == default_type_t::FLOAT);

	variable->location.f = location;
	variable->bound = true;
}

bool M_SetVariable(std::string name, std::string value);
int M_GetIntVariable(std::string name);
std::string M_GetStringVariable(std::string name);
float M_GetFloatVariable(std::string name);
void M_SetConfigFilenames(std::string main_config, std::string extra_config);
std::string M_GetSaveGameDir(std::string iwadname);
std::string M_GetAutoloadDir(std::string iwadname);

extern std::string configdir;
