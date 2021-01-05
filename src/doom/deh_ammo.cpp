/**********************************************************************************************************************************************\
	Copyright(C) 2005-2014 Simon Howard

	This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License
	as published by the Free Software Foundation; either version 2 of the License, or (at your option) any later version.

	This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

	DESCRIPTION:
		Parses "Ammo" sections in dehacked files
\**********************************************************************************************************************************************/

#include "doomdef.h"
#include "doomtype.h"
#include "deh_defs.h"
#include "deh_io.h"
#include "deh_main.h"
#include "p_local.h"

static void* DEH_AmmoStart(deh_context_t* context, std::string line)
{
	int ammo_number = 0;

	if (sscanf(line, "Ammo %i", &ammo_number) != 1)
	{
		DEH_Warning(context, "Parse error on section start");
		return nullptr;
	}

	if (ammo_number < 0 || ammo_number >= (int)AmmoType::NUMAMMO)
	{
		DEH_Warning(context, "Invalid ammo number: %i", ammo_number);
		return nullptr;
	}

	return &maxammo[ammo_number];
}

static void DEH_AmmoParseLine(deh_context_t* context, std::string line, void* tag)
{
	std::string variable_name;
	char* value;
	int ivalue;
	int ammo_number;

	if (tag == NULL)
		return;

	ammo_number = ((int*)tag) - maxammo;

	// Parse the assignment
	if (!DEH_ParseAssignment(line, &variable_name, &value))
	{
		// Failed to parse
		DEH_Warning(context, "Failed to parse assignment");
		return;
	}

	ivalue = atoi(value);

	// maxammo
	if (!iequals(variable_name, "Per ammo"))
	{
		clipammo[ammo_number] = ivalue;
	}
	else if (!iequals(variable_name, "Max ammo"))
	{
		maxammo[ammo_number] = ivalue;
	}
	else
	{
		DEH_Warning(context, "Field named '%s' not found", variable_name);
	}
}

static void DEH_AmmoSHA1Hash(sha1_context_t* context)
{
	for (size_t i{0}; i < std::size_t(AmmoType::NUMAMMO); ++i)
	{
		SHA1_UpdateInt32(context, clipammo[i]);
		SHA1_UpdateInt32(context, maxammo[i]);
	}
}

deh_section_t deh_section_ammo =
{
	"Ammo",
	NULL,
	DEH_AmmoStart,
	DEH_AmmoParseLine,
	NULL,
	DEH_AmmoSHA1Hash,
};
