/**********************************************************************************************************************************************\
	Copyright(C) 1993-1996 Id Software, Inc.
	Copyright(C) 2005-2014 Simon Howard

	This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License
	as published by the Free Software Foundation; either version 2 of the License, or (at your option) any later version.

	This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

	DESCRIPTION:
		Printed strings for translation.
		English language support (default).
\**********************************************************************************************************************************************/
#pragma once

#include "../../derma/common.h"

//
// Printed strings for translation
//

//
// D_Main.C
//
#define D_DEVSTR	"Development mode ON.\n"
#define D_CDROM		"CD-ROM Version: default.cfg from c:\\doomdata\n"

//
// M_Menu.C
//
#define PRESSKEY	"press a key."
#define PRESSYN		"press y or n."
#define QUITMSG		"are you sure you want to\nquit this great game?"
#define LOADNET		"you can't do load while in a net game!\n\n"PRESSKEY
#define QLOADNET	"you can't quickload during a netgame!\n\n"PRESSKEY
#define QSAVESPOT	"you haven't picked a quicksave slot yet!\n\n"PRESSKEY
#define SAVEDEAD	"you can't save if you aren't playing!\n\n"PRESSKEY
#define QSPROMPT	"quicksave over your game named\n\n'%s'?\n\n"PRESSYN
#define QLPROMPT	"do you want to quickload the game named\n\n'%s'?\n\n"PRESSYN

#define NEWGAME		"you can't start a new game\nwhile in a network game.\n\n"PRESSKEY

#define NIGHTMARE	"are you sure? this skill level\nisn't even remotely fair.\n\n"PRESSYN

#define SWSTRING	"this is the shareware version of doom.\n\nyou need to order the entire trilogy.\n\n"PRESSKEY

#define MSGOFF		"Messages OFF"
#define MSGON		"Messages ON"
#define NETEND		"you can't end a netgame!\n\n"PRESSKEY
#define ENDGAME		"are you sure you want to end the game?\n\n"PRESSYN

#define DOSY		"(press y to quit.)" // remove " to dos.)"

#define DETAILHI	"High detail"
#define DETAILLO	"Low detail"
#define GAMMALVL0	"Gamma correction OFF"
#define GAMMALVL1	"Gamma correction level 1"
#define GAMMALVL2	"Gamma correction level 2"
#define GAMMALVL3	"Gamma correction level 3"
#define GAMMALVL4	"Gamma correction level 4"
// intermediate gamma levels
#define GAMMALVL05	"Gamma correction level 0.5"
#define GAMMALVL15	"Gamma correction level 1.5"
#define GAMMALVL25	"Gamma correction level 2.5"
#define GAMMALVL35	"Gamma correction level 3.5"
#define EMPTYSTRING	"empty slot"

//
// P_inter.C
//
#define GOTARMOR	"Picked up the armor."
#define GOTMEGA		"Picked up the MegaArmor!"
#define GOTHTHBONUS	"Picked up a health bonus."
#define GOTARMBONUS	"Picked up an armor bonus."
#define GOTSTIM		"Picked up a stimpack."
#define GOTMEDINEED	"Picked up a medikit that you REALLY need!"
#define GOTMEDIKIT	"Picked up a medikit."
#define GOTSUPER	"Supercharge!"

#define GOTBLUECARD	"Picked up a blue keycard."
#define GOTYELWCARD	"Picked up a yellow keycard."
#define GOTREDCARD	"Picked up a red keycard."
#define GOTBLUESKUL	"Picked up a blue skull key."
#define GOTYELWSKUL	"Picked up a yellow skull key."
#define GOTREDSKULL	"Picked up a red skull key."

#define GOTINVUL	"Invulnerability!"
#define GOTBERSERK	"Berserk!"
#define GOTINVIS	"Partial Invisibility"
#define GOTSUIT		"Radiation Shielding Suit"
#define GOTMAP		"Computer Area Map"
#define GOTVISOR	"Light Amplification Visor"
#define GOTMSPHERE	"MegaSphere!"

#define GOTCLIP		"Picked up a clip."
#define GOTCLIPBOX	"Picked up a box of bullets."
#define GOTROCKET	"Picked up a rocket."
#define GOTROCKBOX	"Picked up a box of rockets."
#define GOTCELL		"Picked up an energy cell."
#define GOTCELLBOX	"Picked up an energy cell pack."
#define GOTSHELLS	"Picked up 4 shotgun shells."
#define GOTSHELLBOX	"Picked up a box of shotgun shells."
#define GOTBACKPACK	"Picked up a backpack full of ammo!"

#define GOTBFG9000	"You got the BFG9000! Oh, yes."
#define GOTCHAINGUN	"You got the chaingun!"
#define GOTCHAINSAW	"A chainsaw! Find some meat!"
#define GOTLAUNCHER	"You got the rocket launcher!"
#define GOTPLASMA	"You got the plasma gun!"
#define GOTSHOTGUN	"You got the shotgun!"
#define GOTSHOTGUN2	"You got the super shotgun!"

//
// P_Doors.C
//
#define PD_BLUEO	"You need a blue key to activate this object"
#define PD_REDO		"You need a red key to activate this object"
#define PD_YELLOWO	"You need a yellow key to activate this object"
#define PD_BLUEK	"You need a blue key to open this door"
#define PD_REDK		"You need a red key to open this door"
#define PD_YELLOWK	"You need a yellow key to open this door"

//
// G_game.C
//
#define GGSAVED		"game saved."

//
// HU_stuff.C
//
#define HUSTR_MSGU	"[Message unsent]"

#define HUSTR_E1M1	"E1M1: Hangar"
#define HUSTR_E1M2	"E1M2: Nuclear Plant"
#define HUSTR_E1M3	"E1M3: Toxin Refinery"
#define HUSTR_E1M4	"E1M4: Command Control"
#define HUSTR_E1M5	"E1M5: Phobos Lab"
#define HUSTR_E1M6	"E1M6: Central Processing"
#define HUSTR_E1M7	"E1M7: Computer Station"
#define HUSTR_E1M8	"E1M8: Phobos Anomaly"
#define HUSTR_E1M9	"E1M9: Military Base"
#define HUSTR_E1M10	"E1M10: Sewers"
#define HUSTR_E1M4B	"E1M4B: Phobos Mission Control"
#define HUSTR_E1M8B	"E1M8B: Tech Gone Bad"

#define HUSTR_E2M1	"E2M1: Deimos Anomaly"
#define HUSTR_E2M2	"E2M2: Containment Area"
#define HUSTR_E2M3	"E2M3: Refinery"
#define HUSTR_E2M4	"E2M4: Deimos Lab"
#define HUSTR_E2M5	"E2M5: Command Center"
#define HUSTR_E2M6	"E2M6: Halls of the Damned"
#define HUSTR_E2M7	"E2M7: Spawning Vats"
#define HUSTR_E2M8	"E2M8: Tower of Babel"
#define HUSTR_E2M9	"E2M9: Fortress of Mystery"

#define HUSTR_E3M1	"E3M1: Hell Keep"
#define HUSTR_E3M2	"E3M2: Slough of Despair"
#define HUSTR_E3M3	"E3M3: Pandemonium"
#define HUSTR_E3M4	"E3M4: House of Pain"
#define HUSTR_E3M5	"E3M5: Unholy Cathedral"
#define HUSTR_E3M6	"E3M6: Mt. Erebus"
#define HUSTR_E3M7	"E3M7: Limbo"
#define HUSTR_E3M8	"E3M8: Dis"
#define HUSTR_E3M9	"E3M9: Warrens"

#define HUSTR_E4M1	"E4M1: Hell Beneath"
#define HUSTR_E4M2	"E4M2: Perfect Hatred"
#define HUSTR_E4M3	"E4M3: Sever The Wicked"
#define HUSTR_E4M4	"E4M4: Unruly Evil"
#define HUSTR_E4M5	"E4M5: They Will Repent"
#define HUSTR_E4M6	"E4M6: Against Thee Wickedly"
#define HUSTR_E4M7	"E4M7: And Hell Followed"
#define HUSTR_E4M8	"E4M8: Unto The Cruel"
#define HUSTR_E4M9	"E4M9: Fear"

#define HUSTR_E5M1	"E5M1: Baphomet's Demesne"
#define HUSTR_E5M2	"E5M2: Sheol"
#define HUSTR_E5M3	"E5M3: Cages of the Damned"
#define HUSTR_E5M4	"E5M4: Paths of Wretchedness"
#define HUSTR_E5M5	"E5M5: Abaddon's Void"
#define HUSTR_E5M6	"E5M6: Unspeakable Persecution"
#define HUSTR_E5M7	"E5M7: Nightmare Underworld"
#define HUSTR_E5M8	"E5M8: Halls of Perdition"
#define HUSTR_E5M9	"E5M9: Realm of Iblis"

#define HUSTR_1		"level 1: entryway"
#define HUSTR_2		"level 2: underhalls"
#define HUSTR_3		"level 3: the gantlet"
#define HUSTR_4		"level 4: the focus"
#define HUSTR_5		"level 5: the waste tunnels"
#define HUSTR_6		"level 6: the crusher"
#define HUSTR_7		"level 7: dead simple"
#define HUSTR_8		"level 8: tricks and traps"
#define HUSTR_9		"level 9: the pit"
#define HUSTR_10	"level 10: refueling base"

#define HUSTR_11	"level 11: 'o' of destruction!"
#define HUSTR_12	"level 12: the factory"
#define HUSTR_13	"level 13: downtown"
#define HUSTR_14	"level 14: the inmost dens"
#define HUSTR_15	"level 15: industrial zone"
#define HUSTR_16	"level 16: suburbs"
#define HUSTR_17	"level 17: tenements"
#define HUSTR_18	"level 18: the courtyard"
#define HUSTR_19	"level 19: the citadel"
#define HUSTR_20	"level 20: gotcha!"

#define HUSTR_21	"level 21: nirvana"
#define HUSTR_22	"level 22: the catacombs"
#define HUSTR_23	"level 23: barrels o' fun"
#define HUSTR_24	"level 24: the chasm"
#define HUSTR_25	"level 25: bloodfalls"
#define HUSTR_26	"level 26: the abandoned mines"
#define HUSTR_27	"level 27: monster condo"
#define HUSTR_28	"level 28: the spirit world"
#define HUSTR_29	"level 29: the living end"
#define HUSTR_30	"level 30: icon of sin"

#define HUSTR_31	"level 31: wolfenstein"
#define HUSTR_32	"level 32: grosse"

#define PHUSTR_1	"level 1: congo"
#define PHUSTR_2	"level 2: well of souls"
#define PHUSTR_3	"level 3: aztec"
#define PHUSTR_4	"level 4: caged"
#define PHUSTR_5	"level 5: ghost town"
#define PHUSTR_6	"level 6: baron's lair"
#define PHUSTR_7	"level 7: caughtyard"
#define PHUSTR_8	"level 8: realm"
#define PHUSTR_9	"level 9: abattoire"
#define PHUSTR_10	"level 10: onslaught"

#define PHUSTR_11	"level 11: hunted"
#define PHUSTR_12	"level 12: speed"
#define PHUSTR_13	"level 13: the crypt"
#define PHUSTR_14	"level 14: genesis"
#define PHUSTR_15	"level 15: the twilight"
#define PHUSTR_16	"level 16: the omen"
#define PHUSTR_17	"level 17: compound"
#define PHUSTR_18	"level 18: neurosphere"
#define PHUSTR_19	"level 19: nme"
#define PHUSTR_20	"level 20: the death domain"

#define PHUSTR_21	"level 21: slayer"
#define PHUSTR_22	"level 22: impossible mission"
#define PHUSTR_23	"level 23: tombstone"
#define PHUSTR_24	"level 24: the final frontier"
#define PHUSTR_25	"level 25: the temple of darkness"
#define PHUSTR_26	"level 26: bunker"
#define PHUSTR_27	"level 27: anti-christ"
#define PHUSTR_28	"level 28: the sewers"
#define PHUSTR_29	"level 29: odyssey of noises"
#define PHUSTR_30	"level 30: the gateway of hell"

#define PHUSTR_31	"level 31: cyberden"
#define PHUSTR_32	"level 32: go 2 it"

#define THUSTR_1	"level 1: system control"
#define THUSTR_2	"level 2: human bbq"
#define THUSTR_3	"level 3: power control"
#define THUSTR_4	"level 4: wormhole"
#define THUSTR_5	"level 5: hanger"
#define THUSTR_6	"level 6: open season"
#define THUSTR_7	"level 7: prison"
#define THUSTR_8	"level 8: metal"
#define THUSTR_9	"level 9: stronghold"
#define THUSTR_10	"level 10: redemption"

#define THUSTR_11	"level 11: storage facility"
#define THUSTR_12	"level 12: crater"
#define THUSTR_13	"level 13: nukage processing"
#define THUSTR_14	"level 14: steel works"
#define THUSTR_15	"level 15: dead zone"
#define THUSTR_16	"level 16: deepest reaches"
#define THUSTR_17	"level 17: processing area"
#define THUSTR_18	"level 18: mill"
#define THUSTR_19	"level 19: shipping/respawning"
#define THUSTR_20	"level 20: central processing"

#define THUSTR_21	"level 21: administration center"
#define THUSTR_22	"level 22: habitat"
#define THUSTR_23	"level 23: lunar mining project"
#define THUSTR_24	"level 24: quarry"
#define THUSTR_25	"level 25: baron's den"
#define THUSTR_26	"level 26: ballistyx"
#define THUSTR_27	"level 27: mount pain"
#define THUSTR_28	"level 28: heck"
#define THUSTR_29	"level 29: river styx"
#define THUSTR_30	"level 30: last call"

#define THUSTR_31	"level 31: pharaoh"
#define THUSTR_32	"level 32: caribbean"

#define NHUSTR_1	"level 1: The Earth Base"
#define NHUSTR_2	"level 2: The Pain Labs"
#define NHUSTR_3	"level 3: Canyon of the Dead"
#define NHUSTR_4	"level 4: Hell Mountain"
#define NHUSTR_5	"level 5: Vivisection"
#define NHUSTR_6	"level 6: Inferno of Blood"
#define NHUSTR_7	"level 7: Baron's Banquet"
#define NHUSTR_8	"level 8: Tomb of Malevolence"
#define NHUSTR_9	"level 9: March of the Demons"

#define MHUSTR_1	"level 1: Attack"
#define MHUSTR_2	"level 2: Canyon"
#define MHUSTR_3	"level 3: The Catwalk"
#define MHUSTR_4	"level 4: The Combine"
#define MHUSTR_5	"level 5: The Fistula"
#define MHUSTR_6	"level 6: The Garrison"
#define MHUSTR_7	"level 7: Titan Manor"
#define MHUSTR_8	"level 8: Paradox"
#define MHUSTR_9	"level 9: Subspace"
#define MHUSTR_10	"level 10: Subterra"
#define MHUSTR_11	"level 11: Trapped On Titan"
#define MHUSTR_12	"level 12: Virgil's Lead"
#define MHUSTR_13	"level 13: Minos' Judgement"
#define MHUSTR_14	"level 14: Bloodsea Keep"
#define MHUSTR_15	"level 15: Mephisto's Maosoleum"
#define MHUSTR_16	"level 16: Nessus"
#define MHUSTR_17	"level 17: Geryon"
#define MHUSTR_18	"level 18: Vesperas"
#define MHUSTR_19	"level 19: Black Tower"
#define MHUSTR_20	"level 20: The Express Elevator To Hell"
#define MHUSTR_21	"level 21: Bad Dream"

#define HUSTR_CHATMACRO1	"I'm ready to kick butt!"
#define HUSTR_CHATMACRO2	"I'm OK."
#define HUSTR_CHATMACRO3	"I'm not looking too good!"
#define HUSTR_CHATMACRO4	"Help!"
#define HUSTR_CHATMACRO5	"You suck!"
#define HUSTR_CHATMACRO6	"Next time, scumbag..."
#define HUSTR_CHATMACRO7	"Come here!"
#define HUSTR_CHATMACRO8	"I'll take care of it."
#define HUSTR_CHATMACRO9	"Yes"
#define HUSTR_CHATMACRO0	"No"

#define HUSTR_TALKTOSELF1	"You mumble to yourself"
#define HUSTR_TALKTOSELF2	"Who's there?"
#define HUSTR_TALKTOSELF3	"You scare yourself"
#define HUSTR_TALKTOSELF4	"You start to rave"
#define HUSTR_TALKTOSELF5	"You've lost it..."

#define HUSTR_MESSAGESENT	"[Message Sent]"

// The following should NOT be changed unless it seems just AWFULLY necessary
#define HUSTR_PLRGREEN		"Green: "
#define HUSTR_PLRINDIGO		"Indigo: "
#define HUSTR_PLRBROWN		"Brown: "
#define HUSTR_PLRRED		"Red: "

#define HUSTR_KEYGREEN		'g'
#define HUSTR_KEYINDIGO		'i'
#define HUSTR_KEYBROWN		'b'
#define HUSTR_KEYRED		'r'

//
// AM_map.C
//
#define AMSTR_FOLLOWON		"Follow Mode ON"
#define AMSTR_FOLLOWOFF		"Follow Mode OFF"

#define AMSTR_GRIDON		"Grid ON"
#define AMSTR_GRIDOFF		"Grid OFF"

#define AMSTR_MARKEDSPOT	"Marked Spot"
#define AMSTR_MARKSCLEARED	"All Marks Cleared"

#define AMSTR_OVERLAYON		"Overlay Mode ON"
#define AMSTR_OVERLAYOFF	"Overlay Mode OFF"

#define AMSTR_ROTATEON		"Rotate Mode ON"
#define AMSTR_ROTATEOFF		"Rotate Mode OFF"

//
// ST_stuff.C
//
#define STSTR_MUS			"Music Change"
#define STSTR_NOMUS			"IMPOSSIBLE SELECTION"
#define STSTR_DQDON			"Degreelessness Mode On"
#define STSTR_DQDOFF		"Degreelessness Mode Off"

#define STSTR_KFAADDED		"Very Happy Ammo Added"
#define STSTR_FAADDED		"Ammo (no keys) Added"

#define STSTR_NCON			"No Clipping Mode ON"
#define STSTR_NCOFF			"No Clipping Mode OFF"

#define STSTR_BEHOLD		"inVuln, Str, Inviso, Rad, Allmap, or Lite-amp"
#define STSTR_BEHOLDX		"Power-up Toggled"

#define STSTR_CHOPPERS		"... doesn't suck - GM"
#define STSTR_CLEV			"Changing Level..."

//
// F_Finale.C
//
#define E1TEXT \
"Once you beat the big badasses and\nclean out the moon base you're supposed\nto win, aren't you? Aren't you? Where's\n"							\
"your fat reward and ticket home? What\nthe hell is this? It's not supposed to\nend this way!\n\nIt stinks like rotten meat, but looks\n"			\
"like the lost Deimos base. Looks like\nyou're stuck on The Shores of Hell.\nThe only way out is through.\n\n"										\
"To continue the DOOM experience, play\nThe Shores of Hell and its amazing\nsequel, Inferno!\n"

#define E2TEXT \
"You've done it! The hideous cyber-\ndemon lord that ruled the lost Deimos\nmoon base has been slain and you\nare triumphant! But ... where are\n"	\
"you? You clamber to the edge of the\nmoon and look down to see the awful\ntruth.\n\nDeimos floats above Hell itself!\n"							\
"You've never heard of anyone escaping\nfrom Hell, but you'll make the bastards\nsorry they ever heard of you! Quickly,\n"							\
"you rappel down to the surface of\nHell.\n\nNow, it's on to the final chapter of\nDOOM! -- Inferno."

#define E3TEXT \
"The loathsome spiderdemon that\nmasterminded the invasion of the moon\nbases and caused so much death has had\nits ass kicked for all time.\n"		\
"\nA hidden doorway opens and you enter.\nYou've proven too tough for Hell to\ncontain, and now Hell at last plays\n"								\
"fair -- for you emerge from the door\nto see the green fields of Earth!\nHome at last.\n\nYou wonder what's been happening on\n"					\
"Earth while you were battling evil\nunleashed. It's good that no Hell-\nspawn could have come through that\ndoor with you ..."

#define E4TEXT \
"the spider mastermind must have sent forth\nits legions of hellspawn before your\nfinal confrontation with that terrible\n"						\
"beast from hell. but you stepped forward\nand brought forth eternal damnation and\nsuffering upon the horde as a true hero\n"						\
"would in the face of something so evil.\n\nbesides, someone was gonna pay for what\nhappened to daisy, your pet rabbit.\n\n"						\
"but now, you see spread before you more\npotential pain and gibbitude as a nation\nof demons run amok among our cities.\n\n"						\
"next stop, hell on earth!"

#define E5TEXT \
"Baphomet was only doing Satan's bidding\nby bringing you back to Hell. Somehow they\ndidn't understand that you're the reason\n"					\
"they failed in the first place.\n\nAfter mopping up the place with your\narsenal, you're ready to face the more\n"									\
"advanced demons that were sent to Earth.\n\n\nLock and load. Rip and tear."

// after level 6, put this:
#define C1TEXT \
"YOU HAVE ENTERED DEEPLY INTO THE INFESTED\nSTARPORT. BUT SOMETHING IS WRONG. THE\nMONSTERS HAVE BROUGHT THEIR OWN REALITY\n"						\
"WITH THEM, AND THE STARPORT'S TECHNOLOGY\nIS BEING SUBVERTED BY THEIR PRESENCE.\n\nAHEAD, YOU SEE AN OUTPOST OF HELL, A\n"							\
"FORTIFIED ZONE. IF YOU CAN GET PAST IT,\nYOU CAN PENETRATE INTO THE HAUNTED HEART\nOF THE STARBASE AND FIND THE CONTROLLING\n"						\
"SWITCH WHICH HOLDS EARTH'S POPULATION\nHOSTAGE."

// After level 11, put this:
#define C2TEXT \
"YOU HAVE WON! YOUR VICTORY HAS ENABLED\nHUMANKIND TO EVACUATE EARTH AND ESCAPE\nTHE NIGHTMARE. NOW YOU ARE THE ONLY\n"								\
"HUMAN LEFT ON THE FACE OF THE PLANET.\nCANNIBAL MUTATIONS, CARNIVOROUS ALIENS,\nAND EVIL SPIRITS ARE YOUR ONLY NEIGHBORS.\n"						\
"YOU SIT BACK AND WAIT FOR DEATH, CONTENT\nTHAT YOU HAVE SAVED YOUR SPECIES.\n\nBUT THEN, EARTH CONTROL BEAMS DOWN A\n"								\
"MESSAGE FROM SPACE: \"SENSORS HAVE LOCATED\nTHE SOURCE OF THE ALIEN INVASION. IF YOU\nGO THERE, YOU MAY BE ABLE TO BLOCK THEIR\n"					\
"ENTRY. THE ALIEN BASE IS IN THE HEART OF\nYOUR OWN HOME CITY, NOT FAR FROM THE\nSTARPORT.\" SLOWLY AND PAINFULLY YOU GET\n"						\
"UP AND RETURN TO THE FRAY."

// After level 20, put this:
#define C3TEXT \
"YOU ARE AT THE CORRUPT HEART OF THE CITY,\nSURROUNDED BY THE CORPSES OF YOUR ENEMIES.\nYOU SEE NO WAY TO DESTROY THE CREATURES'\n"					\
"ENTRYWAY ON THIS SIDE, SO YOU CLENCH YOUR\nTEETH AND PLUNGE THROUGH IT.\n\nTHERE MUST BE A WAY TO CLOSE IT ON THE\n"								\
"OTHER SIDE. WHAT DO YOU CARE IF YOU'VE\nGOT TO GO THROUGH HELL TO GET TO IT?"

// After level 29, put this:
#define C4TEXT \
"THE HORRENDOUS VISAGE OF THE BIGGEST\nDEMON YOU'VE EVER SEEN CRUMBLES BEFORE\nYOU, AFTER YOU PUMP YOUR ROCKETS INTO\n"								\
"HIS EXPOSED BRAIN. THE MONSTER SHRIVELS\nUP AND DIES, ITS THRASHING LIMBS\nDEVASTATING UNTOLD MILES OF HELL'S\nSURFACE.\n\n"						\
"YOU'VE DONE IT. THE INVASION IS OVER.\nEARTH IS SAVED. HELL IS A WRECK. YOU\nWONDER WHERE BAD FOLKS WILL GO WHEN THEY\n"							\
"DIE, NOW. WIPING THE SWEAT FROM YOUR\nFOREHEAD YOU BEGIN THE LONG TREK BACK\nHOME. REBUILDING EARTH OUGHT TO BE A\n"								\
"LOT MORE FUN THAN RUINING IT WAS.\n"

// Before level 31, put this:
#define C5TEXT \
"CONGRATULATIONS, YOU'VE FOUND THE SECRET\nLEVEL! LOOKS LIKE IT'S BEEN BUILT BY\nHUMANS, RATHER THAN DEMONS. YOU WONDER\n"							\
"WHO THE INMATES OF THIS CORNER OF HELL\nWILL BE."

// Before level 32, put this:
#define C6TEXT "CONGRATULATIONS, YOU'VE FOUND THE\nSUPER SECRET LEVEL! YOU'D BETTER\nBLAZE THROUGH THIS ONE!\n"

// after map 06
#define P1TEXT \
"You gloat over the steaming carcass of the\nGuardian. With its death, you've wrested\nthe Accelerator from the stinking claws\n"					\
"of Hell. You relax and glance around the\nroom. Damn! There was supposed to be at\nleast one working prototype, but you can't\n"					\
"see it. The demons must have taken it.\n\nYou must find the prototype, or all your\nstruggles will have been wasted. Keep\n"						\
"moving, keep fighting, keep killing.\nOh yes, keep living, too."

// after map 11
#define P2TEXT \
"Even the deadly Arch-Vile labyrinth could\nnot stop you, and you've gotten to the\nprototype Accelerator which is soon\n"							\
"efficiently and permanently deactivated.\n\nYou're good at that kind of thing."

// after map 20
#define P3TEXT \
"You've bashed and battered your way into\nthe heart of the devil-hive. Time for a\nSearch-and-Destroy mission, aimed at the\n"						\
"Gatekeeper, whose foul offspring is\ncascading to Earth. Yeah, he's bad. But\nyou know who's worse!\n\n"											\
"Grinning evilly, you check your gear, and\nget ready to give the bastard a little Hell\nof your own making!"

// after map 30
#define P4TEXT \
"The Gatekeeper's evil face is splattered\nall over the place. As its tattered corpse\ncollapses, an inverted Gate forms and\n"						\
"sucks down the shards of the last\nprototype Accelerator, not to mention the\nfew remaining demons. You're done. Hell\n"							\
"has gone back to pounding bad dead folks \ninstead of good live ones. Remember to\ntell your grandkids to put a rocket\n"							\
"launcher in your coffin. If you go to Hell\nwhen you die, you'll need it for some\nfinal cleaning-up ..."

// before map 31
#define P5TEXT \
"You've found the second-hardest level we\ngot. Hope you have a saved game a level or\ntwo previous. If not, be prepared to die\n"					\
"aplenty. For master marines only."

// before map 32
#define P6TEXT "Betcha wondered just what WAS the hardest\nlevel we had ready for ya? Now you know.\nNo one gets out alive."

#define T1TEXT \
"You've fought your way out of the infested\nexperimental labs.	It seems that UAC has\nonce again gulped it down. With their\n"						\
"high turnover, it must be hard for poor\nold UAC to buy corporate health insurance\nnowadays..\n\nAhead lies the military complex, now\n"			\
"swarming with diseased horrors hot to get\ntheir teeth into you. With luck, the\ncomplex still has some warlike ordnance\nlaying around."

#define T2TEXT \
"You hear the grinding of heavy machinery\nahead. You sure hope they're not stamping\nout new hellspawn, but you're ready to\n"						\
"ream out a whole herd if you have to.\nThey might be planning a blood feast, but\nyou feel about as mean as two thousand\n"						\
"maniacs packed into one mad killer.\n\nYou don't plan to go down easy."

#define T3TEXT \
"The vista opening ahead looks real damn\nfamiliar. Smells familiar, too -- like\nfried excrement. You didn't like this\n"							\
"place before, and you sure as hell ain't\nplanning to like it now. The more you\nbrood on it, the madder you get.\n"								\
"Hefting your gun, an evil grin trickles\nonto your face. Time to take some names."

#define T4TEXT \
"Suddenly, all is silent, from one horizon\nto the other. The agonizing echo of Hell\nfades away, the nightmare sky turns to\n"						\
"blue, the heaps of monster corpses start \nto evaporate along with the evil stench \nthat filled the air. Jeeze, maybe you've\n"					\
"done it. Have you really won?\n\nSomething rumbles in the distance.\nA blue light begins to glow inside the\nruined skull of the demon-spitter."

#define T5TEXT \
"What now? Looks totally different. Kind\nof like King Tut's condo. Well,\nwhatever's here can't be any worse\n"									\
"than usual. Can it? Or maybe it's best\nto let sleeping gods lie.."

#define T6TEXT \
"Time for a vacation. You've burst the\nbowels of hell and by golly you're ready\nfor a break. You mutter to yourself,\n"							\
"Maybe someone else can kick Hell's ass\nnext time around. Ahead lies a quiet town,\nwith peaceful flowing water, quaint\n"							\
"buildings, and presumably no Hellspawn.\n\nAs you step off the transport, you hear\nthe stomp of a cyberdemon's iron shoe."

#define N1TEXT \
"TROUBLE WAS BREWING AGAIN IN YOUR FAVORITE\nVACATION SPOT... HELL. SOME CYBERDEMON\nPUNK THOUGHT HE COULD TURN HELL INTO A\n"						\
"PERSONAL AMUSEMENT PARK, AND MAKE EARTH\nTHE TICKET BOOTH.\n\nWELL THAT HALF-ROBOT FREAK SHOW DIDN'T\nKNOW WHO WAS COMING TO THE FAIR. THERE'S\n"	\
"NOTHING LIKE A SHOOTING GALLERY FULL OF\nHELLSPAWN TO GET THE BLOOD PUMPING...\n\nNOW THE WALLS OF THE DEMON'S LABYRINTH\n"						\
"ECHO WITH THE SOUND OF HIS METALLIC LIMBS\nHITTING THE FLOOR. HIS DEATH MOAN GURGLES\nOUT THROUGH THE MESS YOU LEFT OF HIS FACE.\n\n"				\
"THIS RIDE IS CLOSED."

#define M1TEXT \
"CONGRATULATIONS YOU HAVE FINISHED... \n\nMOST OF THE MASTER LEVELS\n\nYou have ventured through the most\ntwisted levels that hell had to\n"		\
"offer and you have survived. \n\nBut alas the demons laugh at you\nsince you have shown cowardice and didn't\n"									\
"reach the most hideous level\nthey had made for you."

#define M2TEXT \
"CONGRATULATIONS YOU HAVE FINISHED... \n\nALL THE MASTER LEVELS\n\nYou have ventured through all the\ntwisted levels that hell had to\n"			\
"offer and you have survived. \n\nThe Flames of rage flow through\nyour veins, you are ready\nfor more - but you don't know where\n"				\
"to find more when the demons hide\nlike cowards when they see you."

//
// Character cast strings F_FINALE.C
//
#define CC_ZOMBIE	"ZOMBIEMAN"
#define CC_SHOTGUN	"SHOTGUN GUY"
#define CC_HEAVY	"HEAVY WEAPON DUDE"
#define CC_IMP		"IMP"
#define CC_DEMON	"DEMON"
#define CC_LOST		"LOST SOUL"
#define CC_CACO		"CACODEMON"
#define CC_HELL		"HELL KNIGHT"
#define CC_BARON	"BARON OF HELL"
#define CC_ARACH	"ARACHNOTRON"
#define CC_PAIN		"PAIN ELEMENTAL"
#define CC_REVEN	"REVENANT"
#define CC_MANCU	"MANCUBUS"
#define CC_ARCH		"ARCH-VILE"
#define CC_SPIDER	"THE SPIDER MASTERMIND"
#define CC_CYBER	"THE CYBERDEMON"
#define CC_HERO		"OUR HERO"
