/**********************************************************************************************************************************************\
	Copyright(C) 2005-2014 Simon Howard

	This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License
	as published by the Free Software Foundation; either version 2 of the License, or (at your option) any later version.

	This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

	DESCRIPTION:
		Pseudo-random number generator for secure demos.
\**********************************************************************************************************************************************/
#pragma once

#include "../derma/common.h"

#include "doomtype.h"

using prng_seed_t = byte[16];

// Initialize Pseudo-RNG using the specified 128-bit key.
void PRNG_Start(prng_seed_t key);
void PRNG_Stop();
// Read a random 32-bit integer from the PRNG.
unsigned PRNG_Random();
