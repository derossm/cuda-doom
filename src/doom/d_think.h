/**********************************************************************************************************************************************\
	Copyright(C) 1993-1996 Id Software, Inc.
	Copyright(C) 2005-2014 Simon Howard

	This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License
	as published by the Free Software Foundation; either version 2 of the License, or (at your option) any later version.

	This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

	DESCRIPTION:
		MapObj data. Map Objects or mobjs are actors, entities, thinker, take-your-pick...
		anything that moves, acts, or suffers state changes of more or less violent nature.
\**********************************************************************************************************************************************/
#pragma once

#include "../../derma/common.h"

// Experimental stuff. To compile this as "ANSI C with classes" we will need to handle the various action functions cleanly.
typedef void (*actionf_v)();
typedef void (*actionf_p1)(void*);
typedef void (*actionf_p2)(void*, void*);
typedef void (*actionf_p3)(void*, void*, void*); // let pspr action pointers get called from mobj states

union actionf_t
{
	actionf_v acv;
	actionf_p1 acp1;
	actionf_p2 acp2;
	actionf_p3 acp3; // let pspr action pointers get called from mobj states
};

// Historically, "think_t" is yet another function pointer to a routine to handle an actor.
using think_t = actionf_t;

// Doubly linked list of actors.
struct thinker_t
{
	thinker_t* prev;
	thinker_t* next;
	think_t function;
};
