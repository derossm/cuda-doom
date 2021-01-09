/**********************************************************************************************************************************************\
	Copyright(C) 1993-1996 Id Software, Inc.
	Copyright(C) 2000, 2005-2014 Simon Howard
	Copyright(C) 2019 Fabian Greffrath

	This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License
	as published by the Free Software Foundation; either version 2 of the License, or (at your option) any later version.

	This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

	DESCRIPTION:
 add support for SMMU swirling flats
 adapted from smmu/r_ripple.c, by Simon Howard
\**********************************************************************************************************************************************/

#include <string>

#include <tables.h>
#include <i_system.h>
#include <w_wad.h>
#include <z_zone.h>

#include "doomstat.h"

// swirl factors determine the number of waves per flat width

// 1 cycle per 64 units
constexpr size_t swirlfactor{(8192 / 64)};

// 1 cycle per 32 units (2 in 64)
constexpr size_t swirlfactor2{(8192 / 32)};

constexpr size_t SEQUENCE{1024};
constexpr size_t FLATSIZE{(64 * 64)};

static int* offsets;
static int* offset;

constexpr size_t AMP{2};
constexpr size_t AMP2{2};
constexpr size_t SPEED{40};

void R_InitDistortedFlats()
{
	if (!offsets)
	{
		offsets = I_Realloc(NULL, SEQUENCE * FLATSIZE * sizeof(*offsets));
		offset = offsets;

		for (size_t i{0}; i < SEQUENCE; ++i)
		{
			for (size_t x{0}; x < 64; ++x)
			{
				for (size_t y{0}; y < 64; ++y)
				{
					auto sinvalue = (y * swirlfactor + i * SPEED * 5 + 900) & 8191;
					auto sinvalue2 = (x * swirlfactor2 + i * SPEED * 4 + 300) & 8191;
					auto x1 = x + 128
						+ ((finesine[sinvalue] * AMP) >> FRACBITS)
						+ ((finesine[sinvalue2] * AMP2) >> FRACBITS);

					sinvalue = (x * swirlfactor + i * SPEED * 3 + 700) & 8191;
					sinvalue2 = (y * swirlfactor2 + i * SPEED * 4 + 1200) & 8191;
					auto y1 = y + 128
						+ ((finesine[sinvalue] * AMP) >> FRACBITS)
						+ ((finesine[sinvalue2] * AMP2) >> FRACBITS);

					x1 &= 63;
					y1 &= 63;

					offset[(y << 6) + x] = (y1 << 6) + x1;
				}
			}

			offset += FLATSIZE;
		}
	}
}

::std::string R_DistortedFlat(int flatnum)
{
	static int swirltic{-1};
	static int swirlflat{-1};
	static char distortedflat[FLATSIZE];

	if (swirltic != leveltime)
	{
		offset = offsets + ((leveltime & (SEQUENCE - 1)) * FLATSIZE);

		swirltic = leveltime;
		swirlflat = -1;
	}

	if (swirlflat != flatnum)
	{
		auto normalflat = W_CacheLumpNum<char>(flatnum, pu_tags_t::PU_STATIC);

		for (size_t i{0}; i < FLATSIZE; ++i)
		{
			distortedflat[i] = normalflat[offset[i]];
		}

		W_ReleaseLumpNum(flatnum);

		swirlflat = flatnum;
	}

	return distortedflat;
}
