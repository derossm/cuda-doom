/**********************************************************************************************************************************************\
	Copyright(C) 2005-2014 Simon Howard

	This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License
	as published by the Free Software Foundation; either version 2 of the License, or (at your option) any later version.

	This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
\**********************************************************************************************************************************************/
#pragma once

#include <string>

#include "../../derma/common.h"

#include "d_mode.h"
#include "d_iwad.h"

typedef void (*GameSelectCallback)();
extern GameMission gamemission;

void SetupMission(GameSelectCallback callback);
void InitBindings();
std::string GetExecutableName();
std::string GetGameTitle();
const iwad_t** GetIwads();
