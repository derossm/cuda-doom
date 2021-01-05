/**********************************************************************************************************************************************\
	Copyright(C) 1993-1996 Id Software, Inc.
	Copyright(C) 1999 id Software, Chi Hoang, Lee Killough, Jim Flynn, Rand Phares, Ty Halderman
	Copyright(C) 2005-2014 Simon Howard
	Copyright(C) 2017 Fabian Greffrath

	This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License
	as published by the Free Software Foundation; either version 2 of the License, or (at your option) any later version.

	This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

	DESCRIPTION:
		Create Blockmap taken from mbfsrc/P_SETUP.C:547-707, slightly adapted
\**********************************************************************************************************************************************/

#include "i_system.h"
#include "p_local.h"
#include "z_zone.h"

// blocklist structure
struct Blocklist
{
	int n;
	int nalloc;
	int* list;
};

void P_CreateBlockMap()
{
	if (numvertexes <= 0)
	{
		return;
	}

	fixed_t minx = INT_MAX;
	fixed_t miny = INT_MAX;
	fixed_t maxx = INT_MIN;
	fixed_t maxy = INT_MIN;

	// First find limits of map
	for (size_t i{0}; i < numvertexes; ++i)
	{
		if (vertexes[i].x >> FRACBITS < minx)
		{
			minx = vertexes[i].x >> FRACBITS;
		}
		else if (vertexes[i].x >> FRACBITS > maxx)
		{
			maxx = vertexes[i].x >> FRACBITS;
		}

		if (vertexes[i].y >> FRACBITS < miny)
		{
			miny = vertexes[i].y >> FRACBITS;
		}
		else if (vertexes[i].y >> FRACBITS > maxy)
		{
			maxy = vertexes[i].y >> FRACBITS;
		}
	}

	// [crispy] doombsp/DRAWING.M:175-178
	minx -= 8; miny -= 8;
	maxx += 8; maxy += 8;

	// Save blockmap parameters
	bmaporgx = minx << FRACBITS;
	bmaporgy = miny << FRACBITS;
	bmapwidth = ((maxx - minx) >> MAPBTOFRAC) + 1;
	bmapheight = ((maxy - miny) >> MAPBTOFRAC) + 1;

	// Compute blockmap, which is stored as a 2d array of variable-sized lists.
	//
	// Pseudocode:
	//
	// For each linedef:
	//
	//	Map the starting and ending vertices to blocks.
	//
	//	Starting in the starting vertex's block, do:
	//
	//		Add linedef to current block's list, dynamically resizing it.
	//
	//		If current block is the same as the ending vertex's block, exit loop.
	//
	//		Move to an adjacent block by moving towards the ending block in
	//		either the x or y direction, to the block which contains the linedef.

	if (numlines > 0)
	{

		unsigned tot = bmapwidth * bmapheight;						// size of blockmap
		Blocklist* blockmap = (Blocklist*)calloc(sizeof * blockmap, tot);			// array of blocklists
		//int x, y, adx, ady, bend;

		for (size_t i{0}; i < numlines; ++i)
		{
			//int dx, dy, diff, b;

			// starting coordinates
			int x = (lines[i].v1->x >> FRACBITS) - minx;
			int y = (lines[i].v1->y >> FRACBITS) - miny;

			// x-y deltas
			int adx = lines[i].dx >> FRACBITS;
			int dx = adx < 0 ? -1 : 1;
			int ady = lines[i].dy >> FRACBITS;
			int dy = ady < 0 ? -1 : 1;

			// difference in preferring to move across y (>0) instead of x (<0)
			int diff = !adx ? 1 : !ady ? -1 :
				(((x >> MAPBTOFRAC) << MAPBTOFRAC) + (dx > 0 ? MAPBLOCKUNITS - 1 : 0) - x) * (ady = abs(ady)) * dx
				- (((y >> MAPBTOFRAC) << MAPBTOFRAC) + (dy > 0 ? MAPBLOCKUNITS - 1 : 0) - y) * (adx = abs(adx)) * dy;

			// starting block, and pointer to its blocklist structure
			int b = (y >> MAPBTOFRAC) * bmapwidth + (x >> MAPBTOFRAC);

			// ending block
			int bend = (((lines[i].v2->y >> FRACBITS) - miny) >> MAPBTOFRAC) * bmapwidth + (((lines[i].v2->x >> FRACBITS) - minx) >> MAPBTOFRAC);

			// delta for pointer when moving across y
			dy *= bmapwidth;

			// deltas for diff inside the loop
			adx <<= MAPBTOFRAC;
			ady <<= MAPBTOFRAC;

			// Now we simply iterate block-by-block until we reach the end block.
			while ((unsigned)b < tot)	// failsafe -- should ALWAYS be true
			{
				// Increase size of allocated list if necessary
				if (blockmap[b].n >= blockmap[b].nalloc)
				{
					blockmap[b].list = (int*)I_Realloc(blockmap[b].list, (blockmap[b].nalloc = blockmap[b].nalloc ? blockmap[b].nalloc * 2 : 8) * sizeof * blockmap->list);
				}

				// Add linedef to end of list
				blockmap[b].list[blockmap[b].n++] = i;

				// If we have reached the last block, exit
				if (b == bend)
				{
					break;
				}

				// Move in either the x or y direction to the next block
				if (diff < 0)
				{
					diff += ady, b += dx;
				}
				else
				{
					diff -= adx, b += dy;
				}
			}
		}

		// Compute the total size of the blockmap.
		//
		// Compression of empty blocks is performed by reserving two offset words
		// at tot and tot+1.
		//
		// 4 words, unused if this routine is called, are reserved at the start.
		{
			int count = tot + 6; // we need at least 1 word per block, plus reserved's

			for (size_t i{0}; i < tot; ++i)
			{
				if (blockmap[i].n)
				{
					count += blockmap[i].n + 2; // 1 header word + 1 trailer word + blocklist
				}
			}

			// Allocate blockmap lump with computed count
			blockmaplump = Z_Malloc<decltype(blockmaplump)>(sizeof(*blockmaplump) * count, pu_tags_t::PU_LEVEL, 0);
		}

		// Now compress the blockmap.
		{
			int ndx = tot += 4;			// Advance index to start of linedef lists
			Blocklist* uncompressedBlockmap = blockmap;			// Start of uncompressed blockmap

			blockmaplump[ndx++] = 0;	// Store an empty blockmap list at start
			blockmaplump[ndx++] = -1;	// (Used for compression)

			for (size_t i{4}; i < tot; ++i, ++uncompressedBlockmap)
			{
				if (uncompressedBlockmap->n)										// Non-empty blocklist
				{
					blockmaplump[blockmaplump[i] = ndx++] = 0; // Store index & header

					do
					{
						blockmaplump[ndx++] = uncompressedBlockmap->list[--uncompressedBlockmap->n]; // Copy linedef list
					} while (uncompressedBlockmap->n);

					blockmaplump[ndx++] = -1;					// Store trailer
					free(uncompressedBlockmap->list);								// Free linedef list
				}
				else			// Empty blocklist: point to reserved empty blocklist
				{
					blockmaplump[i] = tot;
				}
			}

			free(blockmap);	// Free uncompressed blockmap
		}
	}

	// [crispy] copied over from P_LoadBlockMap()
	{
		int count = sizeof(*blocklinks) * bmapwidth * bmapheight;
		blocklinks = Z_Malloc<decltype(blocklinks)>(count, pu_tags_t::PU_LEVEL, 0);
		memset(blocklinks, 0, count);
		blockmap = blockmaplump + 4;
	}

	fprintf(stderr, "+BLOCKMAP)\n");
}
