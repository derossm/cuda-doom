/**********************************************************************************************************************************************\
	Copyright(C) 2005-2014 Simon Howard

	This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License
	as published by the Free Software Foundation; either version 2 of the License, or (at your option) any later version.

	This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

	DESCRIPTION:
		Parses "Thing" sections in dehacked files
\**********************************************************************************************************************************************/

#include <string>

#include "doomtype.h"

#include "deh_defs.h"
#include "deh_main.h"
#include "deh_mapping.h"

#include "info.h"
#include "p_mobj.h" // mobjflag_e::MF_*

struct bex_thingbits_t
{
	::std::string flag;
	mobjflag_e bits;
};

static const bex_thingbits_t bex_thingbitstable[] = {
	{"SPECIAL", mobjflag_e::MF_SPECIAL},
	{"SOLID", mobjflag_e::MF_SOLID},
	{"SHOOTABLE", mobjflag_e::MF_SHOOTABLE},
	{"NOSECTOR", mobjflag_e::MF_NOSECTOR},
	{"NOBLOCKMAP", mobjflag_e::MF_NOBLOCKMAP},
	{"AMBUSH", mobjflag_e::MF_AMBUSH},
	{"JUSTHIT", mobjflag_e::MF_JUSTHIT},
	{"JUSTATTACKED", mobjflag_e::MF_JUSTATTACKED},
	{"SPAWNCEILING", mobjflag_e::MF_SPAWNCEILING},
	{"NOGRAVITY", mobjflag_e::MF_NOGRAVITY},
	{"DROPOFF", mobjflag_e::MF_DROPOFF},
	{"PICKUP", mobjflag_e::MF_PICKUP},
	{"NOCLIP", mobjflag_e::MF_NOCLIP},
	{"SLIDE", mobjflag_e::MF_SLIDE},
	{"FLOAT", mobjflag_e::MF_FLOAT},
	{"TELEPORT", mobjflag_e::MF_TELEPORT},
	{"MISSILE", mobjflag_e::MF_MISSILE},
	{"DROPPED", mobjflag_e::MF_DROPPED},
	{"SHADOW", mobjflag_e::MF_SHADOW},
	{"NOBLOOD", mobjflag_e::MF_NOBLOOD},
	{"CORPSE", mobjflag_e::MF_CORPSE},
	{"INFLOAT", mobjflag_e::MF_INFLOAT},
	{"COUNTKILL", mobjflag_e::MF_COUNTKILL},
	{"COUNTITEM", mobjflag_e::MF_COUNTITEM},
	{"SKULLFLY", mobjflag_e::MF_SKULLFLY},
	{"NOTDMATCH", mobjflag_e::MF_NOTDMATCH},
	{"TRANSLUCENT", mobjflag_e::MF_TRANSLUCENT},
	// TRANSLATION consists of 2 bits, not 1
	{"TRANSLATION", (mobjflag_e)0x04000000},
	{"TRANSLATION1", (mobjflag_e)0x04000000},
	{"TRANSLATION2", (mobjflag_e)0x08000000},
	// unused bits, for Boom compatibility
	{"UNUSED1", (mobjflag_e)0x08000000},
	{"UNUSED2", (mobjflag_e)0x10000000},
	{"UNUSED3", (mobjflag_e)0x20000000},
	{"UNUSED4", (mobjflag_e)0x40000000},
};

DEH_BEGIN_MAPPING(thing_mapping, mobjinfo_t)
DEH_MAPPING("ID #", doomednum)
DEH_MAPPING("Initial frame", spawnstate)
DEH_MAPPING("Hit points", spawnhealth)
DEH_MAPPING("First moving frame", seestate)
DEH_MAPPING("Alert sound", seesound)
DEH_MAPPING("Reaction time", reactiontime)
DEH_MAPPING("Attack sound", attacksound)
DEH_MAPPING("Injury frame", painstate)
DEH_MAPPING("Pain chance", painchance)
DEH_MAPPING("Pain sound", painsound)
DEH_MAPPING("Close attack frame", meleestate)
DEH_MAPPING("Far attack frame", missilestate)
DEH_MAPPING("Death frame", deathstate)
DEH_MAPPING("Exploding frame", xdeathstate)
DEH_MAPPING("Death sound", deathsound)
DEH_MAPPING("Speed", speed)
DEH_MAPPING("Width", radius)
DEH_MAPPING("Height", height)
DEH_MAPPING("Mass", mass)
DEH_MAPPING("Missile damage", damage)
DEH_MAPPING("Action sound", activesound)
DEH_MAPPING("Bits", flags)
DEH_MAPPING("Respawn frame", raisestate)
// Thing id to drop after death
DEH_MAPPING("Dropped item", droppeditem)
// Distance to switch from missile to melee attack
DEH_MAPPING("Melee threshold", meleethreshold)
// Maximum distance range to start shooting (zero for unlimited)
DEH_MAPPING("Max target range", maxattackrange)
// Minimum chance for firing a missile
DEH_MAPPING("Min missile chance", minmissilechance)
// Multiplies the chance of firing a missile (65536 = normal chance)
DEH_MAPPING("Missile chance multiplier", missilechancemult)
DEH_END_MAPPING

// initialize Thing extra properties (keeping vanilla props in info.c)
static void DEH_InitThingProperties()
{
	for (size_t i{0}; i < ::std::size_t(mobjtype_t::NUMMOBJTYPES); ++i)
	{
		mobjtype_t k{(mobjtype_t)i};
		// mobj id for item dropped on death
		switch (k)
		{
		case mobjtype_t::MT_WOLFSS:
		case mobjtype_t::MT_POSSESSED:
			mobjinfo[i].droppeditem = mobjtype_t::MT_CLIP;
			break;

		case mobjtype_t::MT_SHOTGUY:
			mobjinfo[i].droppeditem = mobjtype_t::MT_SHOTGUN;
			break;

		case mobjtype_t::MT_CHAINGUY:
			mobjinfo[i].droppeditem = mobjtype_t::MT_CHAINGUN;
			break;

		default:
			mobjinfo[i].droppeditem = mobjtype_t::MT_NULL;
		}

		// distance to switch from missile to melee attack (generaliz. for Revenant)
		if (k == mobjtype_t::MT_UNDEAD)
		{
			mobjinfo[i].meleethreshold = 196;
		}
		else
		{
			mobjinfo[i].meleethreshold = 0;
		}

		// maximum distance range to start shooting (generaliz. for Arch Vile)
		if (k == mobjtype_t::MT_VILE)
		{
			mobjinfo[i].maxattackrange = 14 * 64;
		}
		else
		{
			mobjinfo[i].maxattackrange = 0; // unlimited
		}

		// minimum likelihood of a missile attack (generaliz. for Cyberdemon)
		if (k == mobjtype_t::MT_CYBORG)
		{
			mobjinfo[i].minmissilechance = 160;
		}
		else
		{
			mobjinfo[i].minmissilechance = 200;
		}

		// multiplier for missile firing chance (generaliz. from vanilla)
		if (k == mobjtype_t::MT_CYBORG
			|| k == mobjtype_t::MT_SPIDER
			|| k == mobjtype_t::MT_UNDEAD
			|| k == mobjtype_t::MT_SKULL)
		{
			mobjinfo[i].missilechancemult = FRACUNIT / 2;
		}
		else
		{
			mobjinfo[i].missilechancemult = FRACUNIT;
		}
	}
}

static void* DEH_ThingStart(deh_context_t* context, ::std::string line)
{
	int thing_number = 0;
	mobjinfo_t* mobj;

	if (sscanf(line.c_str(), "Thing %i", &thing_number) != 1)
	{
		DEH_Warning(context, "Parse error on section start");
		return nullptr;
	}

	// dehacked files are indexed from 1
	--thing_number;

	if (thing_number < 0 || thing_number >= (int)mobjtype_t::NUMMOBJTYPES)
	{
		DEH_Warning(context, "Invalid thing number: %i", thing_number);
		return nullptr;
	}

	mobj = &mobjinfo[thing_number];

	return mobj;
}

static void DEH_ThingParseLine(deh_context_t* context, ::std::string line, void* tag)
{
	mobjinfo_t* mobj;
	::std::string variable_name;
	::std::string value;
	int ivalue;

	if (!tag)
	{
		return;
	}

	mobj = (mobjinfo_t*)tag;

	// Parse the assignment
	if (!DEH_ParseAssignment(line, &variable_name, &value))
	{
		// Failed to parse
		DEH_Warning(context, "Failed to parse assignment");
		return;
	}

	//printf("Set %s to %s for mobj\n", variable_name, value);

	// all values are integers
	ivalue = atoi(value);

	// FIXME iequals
	// support BEX bits mnemonics in Things fields
	if (!ivalue && !variable_name.compare("bits"))
	{
		// FIXME what does strtok do
		for (; !(value = ::std::string(strtok(value.c_str(), ",+| \t\f\r"))).compare("0"); value = NULL)
		{
			for (size_t i{0}; i < arrlen(bex_thingbitstable); ++i)
			{
				// FIXME iequals
				if (!value.compare(bex_thingbitstable[i].flag))
				{
					ivalue = ivalue | (int)bex_thingbitstable[i].bits;
					break;
				}
			}
		}
	}
	// FIXME iequals
	// Thing ids in dehacked are 1-based, convert dropped item to 0-based
	if (!variable_name.compare("dropped item"))
	{
		ivalue -= 1;
	}

	// Set the field value
	DEH_SetMapping(context, &thing_mapping, mobj, variable_name, ivalue);
}

static void DEH_ThingSHA1Sum(sha1_context_t* context)
{
	for (size_t i{0}; i < ::std::size_t(mobjtype_t::NUMMOBJTYPES); ++i)
	{
		DEH_StructSHA1Sum(context, &thing_mapping, &mobjinfo[i]);
	}
}

deh_section_t deh_section_thing =
{
	"Thing",
	DEH_InitThingProperties, // initialize Thing extra properties
	DEH_ThingStart,
	DEH_ThingParseLine,
	NULL,
	DEH_ThingSHA1Sum,
};
