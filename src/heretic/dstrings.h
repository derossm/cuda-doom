/**********************************************************************************************************************************************\
 	filename: dstrings.h

	Copyright(C) 1993-1996 Id Software, Inc.
	Copyright(C) 1993-2008 Raven Software
	Copyright(C) 2005-2014 Simon Howard

	This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License
	as published by the Free Software Foundation; either version 2 of the License, or (at your option) any later version.

	This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
\**********************************************************************************************************************************************/
#pragma once

#include "../../derma/common.h"

//---------------------------------------------------------------------------
// p_inter.cpp
//---------------------------------------------------------------------------

// Keys
#define TXT_GOTBLUEKEY			"BLUE KEY"
#define TXT_GOTYELLOWKEY		"YELLOW KEY"
#define TXT_GOTGREENKEY			"GREEN KEY"

// Artifacts
#define TXT_ARTIHEALTH			"QUARTZ FLASK"
#define TXT_ARTIFLY				"WINGS OF WRATH"
#define TXT_ARTIINVULNERABILITY	"RING OF INVINCIBILITY"
#define TXT_ARTITOMEOFPOWER		"TOME OF POWER"
#define TXT_ARTIINVISIBILITY	"SHADOWSPHERE"
#define TXT_ARTIEGG				"MORPH OVUM"
#define TXT_ARTISUPERHEALTH		"MYSTIC URN"
#define TXT_ARTITORCH			"TORCH"
#define TXT_ARTIFIREBOMB		"TIME BOMB OF THE ANCIENTS"
#define TXT_ARTITELEPORT		"CHAOS DEVICE"

// Items
#define TXT_ITEMHEALTH			"CRYSTAL VIAL"
#define TXT_ITEMBAGOFHOLDING	"BAG OF HOLDING"
#define TXT_ITEMSHIELD1			"SILVER SHIELD"
#define TXT_ITEMSHIELD2			"ENCHANTED SHIELD"
#define TXT_ITEMSUPERMAP		"MAP SCROLL"

// Ammo
#define TXT_AMMOGOLDWAND1		"WAND CRYSTAL"
#define TXT_AMMOGOLDWAND2		"CRYSTAL GEODE"
#define TXT_AMMOMACE1			"MACE SPHERES"
#define TXT_AMMOMACE2			"PILE OF MACE SPHERES"
#define TXT_AMMOCROSSBOW1		"ETHEREAL ARROWS"
#define TXT_AMMOCROSSBOW2		"QUIVER OF ETHEREAL ARROWS"
#define TXT_AMMOBLASTER1		"CLAW ORB"
#define TXT_AMMOBLASTER2		"ENERGY ORB"
#define TXT_AMMOSKULLROD1		"LESSER RUNES"
#define TXT_AMMOSKULLROD2		"GREATER RUNES"
#define TXT_AMMOPHOENIXROD1		"FLAME ORB"
#define TXT_AMMOPHOENIXROD2		"INFERNO ORB"

// Weapons
#define TXT_WPNMACE				"FIREMACE"
#define TXT_WPNCROSSBOW			"ETHEREAL CROSSBOW"
#define TXT_WPNBLASTER			"DRAGON CLAW"
#define TXT_WPNSKULLROD			"HELLSTAFF"
#define TXT_WPNPHOENIXROD		"PHOENIX ROD"
#define TXT_WPNGAUNTLETS		"GAUNTLETS OF THE NECROMANCER"

//---------------------------------------------------------------------------
// sb_bar.cpp
//---------------------------------------------------------------------------

#define TXT_CHEATGODON			"GOD MODE ON"
#define TXT_CHEATGODOFF			"GOD MODE OFF"
#define TXT_CHEATNOCLIPON		"NO CLIPPING ON"
#define TXT_CHEATNOCLIPOFF		"NO CLIPPING OFF"
#define TXT_CHEATWEAPONS		"ALL WEAPONS"
#define TXT_CHEATFLIGHTON		"FLIGHT ON"
#define TXT_CHEATFLIGHTOFF		"FLIGHT OFF"
#define TXT_CHEATPOWERON		"POWER ON"
#define TXT_CHEATPOWEROFF		"POWER OFF"
#define TXT_CHEATHEALTH			"FULL HEALTH"
#define TXT_CHEATKEYS			"ALL KEYS"
#define TXT_CHEATSOUNDON		"SOUND DEBUG ON"
#define TXT_CHEATSOUNDOFF		"SOUND DEBUG OFF"
#define TXT_CHEATTICKERON		"TICKER ON"
#define TXT_CHEATTICKEROFF		"TICKER OFF"
#define TXT_CHEATARTIFACTS1		"CHOOSE AN ARTIFACT ( A - J )"
#define TXT_CHEATARTIFACTS2		"HOW MANY ( 1 - 9 )"
#define TXT_CHEATARTIFACTS3		"YOU GOT IT"
#define TXT_CHEATARTIFACTSFAIL	"BAD INPUT"
#define TXT_CHEATWARP			"LEVEL WARP"
#define TXT_CHEATSCREENSHOT		"SCREENSHOT"
#define TXT_CHEATCHICKENON		"CHICKEN ON"
#define TXT_CHEATCHICKENOFF		"CHICKEN OFF"
#define TXT_SHOWFPSON			"SHOW FPS ON"	// [crispy]
#define TXT_SHOWFPSOFF			"SHOW FPS OFF"	// [crispy]
#define TXT_CHEATMASSACRE		"MASSACRE"
#define TXT_CHEATIDDQD			"TRYING TO CHEAT, EH? NOW YOU DIE!"
#define TXT_CHEATIDKFA			"CHEATER - YOU DON'T DESERVE WEAPONS"

//---------------------------------------------------------------------------
// p_doors.cpp
//---------------------------------------------------------------------------

#define TXT_NEEDBLUEKEY			"YOU NEED A BLUE KEY TO OPEN THIS DOOR"
#define TXT_NEEDGREENKEY		"YOU NEED A GREEN KEY TO OPEN THIS DOOR"
#define TXT_NEEDYELLOWKEY		"YOU NEED A YELLOW KEY TO OPEN THIS DOOR"

//---------------------------------------------------------------------------
// g_game.cpp
//---------------------------------------------------------------------------

#define TXT_GAMESAVED			"GAME SAVED"

//---------------------------------------------------------------------------
// am_map.cpp
//---------------------------------------------------------------------------

#define AMSTR_FOLLOWON			"FOLLOW MODE ON"
#define AMSTR_FOLLOWOFF			"FOLLOW MODE OFF"

#define AMSTR_GRIDON			"Grid ON"
#define AMSTR_GRIDOFF			"Grid OFF"

#define AMSTR_MARKEDSPOT		"Marked Spot"
#define AMSTR_MARKSCLEARED		"All Marks Cleared"

//---------------------------------------------------------------------------
// f_finale.cpp
//---------------------------------------------------------------------------

#define E1TEXT	"With the destruction of the iron\nliches and their minions, the last\nof the undead are cleared from this\n"						\
				"plane of existence.\n\nThose creatures had to come from\nsomewhere, though, and you have the\nsneaky suspicion that the fiery\n"	\
				"portal of hell's maw opens onto\ntheir home dimension.\n\nTo make sure that more undead\n(or even worse things) don't come\n"		\
				"through, you'll have to seal hell's\nmaw from the other side. Of course\nthis means you may get stuck in a\n"						\
				"very unfriendly world, but no one\never said being a Heretic was easy!"

#define E2TEXT	"the mighty maulotaurs have proved\nto be no match for you, and as\ntheir steaming corpses slide to the\n"							\
				"ground you feel a sense of grim\nsatisfaction that they have been\ndestroyed.\n\nthe gateways which they guarded\n"				\
				"have opened, revealing what you\nhope is the way home. but as you\nstep through, mocking laughter\nrings in your ears.\n\n"		\
				"was some other force controlling\nthe maulotaurs? could there be even\nmore horrific beings through this\n"						\
				"gate? the sweep of a crystal dome\noverhead where the sky should be is\ncertainly not a good sign...."

#define E3TEXT	"the death of d'sparil has loosed\nthe magical bonds holding his\ncreatures on this plane, their\n"									\
				"dying screams overwhelming his own\ncries of agony.\n\nyour oath of vengeance fulfilled,\nyou enter the portal to your own\n"		\
				"world, mere moments before the dome\nshatters into a million pieces.\n\nbut if d'sparil's power is broken\n"						\
				"forever, why don't you feel safe?\nwas it that last shout just before\nhis death, the one that sounded\n"							\
				"like a curse? or a summoning? you\ncan't really be sure, but it might\njust have been a scream.\n\n"								\
				"then again, what about the other\nserpent riders?"

#define E4TEXT	"you thought you would return to your\nown world after d'sparil died, but\nhis final act banished you to his\n"						\
				"own plane. here you entered the\nshattered remnants of lands\nconquered by d'sparil. you defeated\n"								\
				"the last guardians of these lands,\nbut now you stand before the gates\nto d'sparil's stronghold. until this\n"					\
				"moment you had no doubts about your\nability to face anything you might\nencounter, but beyond this portal\n"						\
				"lies the very heart of the evil\nwhich invaded your world. d'sparil\nmight be dead, but the pit where he\n"						\
				"was spawned remains. now you must\nenter that pit in the hopes of\nfinding a way out. and somewhere,\n"							\
				"in the darkest corner of d'sparil's\ndemesne, his personal bodyguards\nawait your arrival ..."

#define E5TEXT	"as the final maulotaur bellows his\ndeath-agony, you realize that you\nhave never come so close to your own\n"						\
				"destruction. not even the fight with\nd'sparil and his disciples had been\nthis desperate. grimly you stare at\n"					\
				"the gates which open before you,\nwondering if they lead home, or if\nthey open onto some undreamed-of\n"							\
				"horror. you find yourself wondering\nif you have the strength to go on,\nif nothing but death and pain await\n"					\
				"you. but what else can you do, if\nthe will to fight is gone? can you\nforce yourself to continue in the\n"						\
				"face of such despair? do you have\nthe courage? you find, in the end,\nthat it is not within you to\n"								\
				"surrender without a fight. eyes\nwide, you go to meet your fate."
