/**********************************************************************************************************************************************\
	Copyright(C) 1993-1996 Id Software, Inc.
	Copyright(C) 2005-2014 Simon Howard

	This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License
	as published by the Free Software Foundation; either version 2 of the License, or (at your option) any later version.

	This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

	DESCRIPTION:
		Zone Memory Allocation. Neat.
\**********************************************************************************************************************************************/
#include "../derma/stdafx.h"

/*
#include "i_system.h"
#include "m_argv.h"

#include "z_zone.h"

// ZONE MEMORY ALLOCATION
// There is never any space between memblocks, and there will never be two contiguous free memblocks. The rover can be left pointing at a non-empty block.
// It is of no value to free a cachable block, because it will get overwritten automatically if needed.

constexpr size_t MEM_ALIGN{sizeof(void*)};
constexpr size_t ZONEID{0x1d4a11};

//struct memblock_t
//{
//	int size;	// including the header and possibly tiny fragments
//	void** user;
//	pu_tag_t tag;	// pu_tags_t::PU_FREE if this is free
//	int id;	// should be ZONEID
//	memblock_t* next;
//	memblock_t* prev;
//};

//struct memzone_t
//{
//	// total bytes malloced, including header
//	int size;
//
//	// start / end cap for linked list
//	memblock_t blocklist;
//
//	memblock_t* rover;
//};

static memzone_t* mainzone;
static bool zero_on_free;
static bool scan_on_free;

void Z_ClearZone(memzone_t* zone)
{
	memblock_t* block;

	// set the entire zone to one free block
	block = (memblock_t*)((byte*)zone + sizeof(memzone_t));
	zone->blocklist.next = block;
	zone->blocklist.prev = block;

	// TODO fix this void* to void** issue
	zone->blocklist.user = (void**)zone;
	zone->blocklist.tag = pu_tags_t::PU_STATIC;
	zone->rover = block;

	block->prev = block->next = &zone->blocklist;

	// a free block.
	block->tag = pu_tags_t::PU_FREE;

	block->size = zone->size - sizeof(memzone_t);
}

void Z_Init()
{
	memblock_t* block;
	int size;

	mainzone = (memzone_t*)I_ZoneBase(&size);
	mainzone->size = size;

	// set the entire zone to one free block
	block = (memblock_t*)((byte*)mainzone + sizeof(memzone_t));
	mainzone->blocklist.next = block;
	mainzone->blocklist.prev = block;

	// TODO fix this void* to void** issue
	mainzone->blocklist.user = (void**)mainzone;
	mainzone->blocklist.tag = pu_tags_t::PU_STATIC;
	mainzone->rover = block;

	block->next = &mainzone->blocklist;
	block->prev = block->next;

	// free block
	block->tag = pu_tags_t::PU_FREE;

	block->size = mainzone->size - sizeof(memzone_t);

	// [Deliberately undocumented]
	// Zone memory debugging flag. If set, memory is zeroed after it is freed to deliberately break any code that attempts to use it after free.
	zero_on_free = M_ParmExists("-zonezero");

	// [Deliberately undocumented]
	// Zone memory debugging flag. If set, each time memory is freed, the zone heap is scanned to look for remaining pointers to the freed block.
	scan_on_free = M_ParmExists("-zonescan");
}

// Scan the zone heap for pointers within the specified range, and warn about any remaining pointers.
static void ScanForBlock(void* start, void* end)
{
	memblock_t* block;
	void** mem;
	int i;
	int len;
	pu_tags_t tag;

	block = mainzone->blocklist.next;

	while (block->next != &mainzone->blocklist)
	{
		tag = block->tag;

		if (tag == pu_tags_t::PU_STATIC || tag == pu_tags_t::PU_LEVEL || tag == pu_tags_t::PU_LEVSPEC)
		{
			// Scan for pointers on the assumption that pointers are aligned on word boundaries (word size depending on pointer size):
			mem = (void**)((byte*)block + sizeof(memblock_t));
			len = (block->size - sizeof(memblock_t)) / sizeof(void*);

			for (i = 0; i < len; ++i)
			{
				if (start <= mem[i] && mem[i] <= end)
				{
					fprintf(stderr, "%p has dangling pointer into freed block %p (%p -> %p)\n", mem, start, &mem[i], mem[i]);
				}
			}
		}

		block = block->next;
	}
}

void Z_Free(void* ptr)
{
	memblock_t* block;
	memblock_t* other;

	block = (memblock_t*)((byte*)ptr - sizeof(memblock_t));

	if (block->id != ZONEID)
		I_Error("Z_Free: freed a pointer without ZONEID");

	if (block->tag != pu_tags_t::PU_FREE && block->user != NULL)
	{
		// clear the user's mark
		*block->user = 0;
	}

	// mark as free
	block->tag = pu_tags_t::PU_FREE;
	block->user = nullptr;
	block->id = 0;

	// If the -zonezero flag is provided, we zero out the block on free to break code that depends on reading freed memory.
	if (zero_on_free)
	{
		memset(ptr, 0, block->size - sizeof(memblock_t));
	}
	if (scan_on_free)
	{
		ScanForBlock(ptr, (byte*)ptr + block->size - sizeof(memblock_t));
	}

	other = block->prev;

	if (other->tag == pu_tags_t::PU_FREE)
	{
		// merge with previous free block
		other->size += block->size;
		other->next = block->next;
		other->next->prev = other;

		if (block == mainzone->rover)
			mainzone->rover = other;

		block = other;
	}

	other = block->next;
	if (other->tag == pu_tags_t::PU_FREE)
	{
		// merge the next free block onto the end
		block->size += other->size;
		block->next = other->next;
		block->next->prev = block;

		if (other == mainzone->rover)
			mainzone->rover = block;
	}
}

// You can pass a NULL user if the tag is < pu_tags_t::PU_PURGELEVEL.
constexpr size_t MINFRAGMENT{64};

void* Z_Malloc(int size, pu_tags_t tag, void* user)
{
	int extra;
	memblock_t* start;
	memblock_t* rover;
	memblock_t* newblock;
	memblock_t* base;
	void* result;

	size = (size + MEM_ALIGN - 1) & ~(MEM_ALIGN - 1);

	// scan through the block list, looking for the first free block of sufficient size, throwing out any purgable blocks along the way.

	// account for size of block header
	size += sizeof(memblock_t);

	// if there is a free block behind the rover, back up over them
	base = mainzone->rover;

	if (base->prev->tag == pu_tags_t::PU_FREE)
		base = base->prev;

	rover = base;
	start = base->prev;

	do
	{
		if (rover == start)
		{
			// scanned all the way around the list
			//I_Error("Z_Malloc: failed on allocation of %i bytes", size);

			// allocate another zone twice as big
			Z_Init();

			base = mainzone->rover;
			rover = base;
			start = base->prev;
		}

		if (rover->tag != pu_tags_t::PU_FREE)
		{
			if (rover->tag < pu_tags_t::PU_PURGELEVEL)
			{
				// hit a block that can't be purged, so move base past it
				base = rover = rover->next;
			}
			else
			{
				// free the rover block (adding the size to base); the rover can be the base block
				base = base->prev;
				Z_Free((byte*)rover + sizeof(memblock_t));
				base = base->next;
				rover = base->next;
			}
		}
		else
		{
			rover = rover->next;
		}

	} while (base->tag != pu_tags_t::PU_FREE || base->size < size);

	// found a block big enough
	extra = base->size - size;

	if (extra > MINFRAGMENT)
	{
		// there will be a free fragment after the allocated block
		newblock = (memblock_t*)((byte*)base + size);
		newblock->size = extra;

		newblock->tag = pu_tags_t::PU_FREE;
		newblock->user = NULL;
		newblock->prev = base;
		newblock->next = base->next;
		newblock->next->prev = newblock;

		base->next = newblock;
		base->size = size;
	}

	if (user == NULL && tag >= pu_tags_t::PU_PURGELEVEL)
		I_Error("Z_Malloc: an owner is required for purgable blocks");

	// TODO fix more void** to void* issues
	base->user = &user;
	base->tag = tag;

	result = (void*)((byte*)base + sizeof(memblock_t));

	if (base->user)
	{
		*base->user = result;
	}

	// next allocation will start looking here
	mainzone->rover = base->next;

	base->id = ZONEID;

	return result;
}

void Z_FreeTags(pu_tags_t lowtag, pu_tags_t hightag)
{
	memblock_t* block;
	memblock_t* next;

	for (block = mainzone->blocklist.next; block != &mainzone->blocklist; block = next)
	{
		// get link before freeing
		next = block->next;

		// free block?
		if (block->tag == pu_tags_t::PU_FREE)
			continue;

		if (block->tag >= lowtag && block->tag <= hightag)
			Z_Free((byte*)block + sizeof(memblock_t));
	}
}

// Note: TFileDumpHeap( stdout ) ?
void Z_DumpHeap(pu_tags_t lowtag, pu_tags_t hightag)
{
	memblock_t* block;

	printf("zone size: %i location: %p\n", mainzone->size, mainzone);

	printf("tag range: %i to %i\n", lowtag, hightag);

	for (block = mainzone->blocklist.next; ; block = block->next)
	{
		if (block->tag >= lowtag && block->tag <= hightag)
			printf("block:%p	size:%7i	user:%p	tag:%3i\n", block, block->size, block->user, block->tag);

		if (block->next == &mainzone->blocklist)
		{
			// all blocks have been hit
			break;
		}

		if ((byte*)block + block->size != (byte*)block->next)
			printf("ERROR: block size does not touch the next block\n");

		if (block->next->prev != block)
			printf("ERROR: next block doesn't have proper back link\n");

		if (block->tag == pu_tags_t::PU_FREE && block->next->tag == pu_tags_t::PU_FREE)
			printf("ERROR: two consecutive free blocks\n");
	}
}

void Z_FileDumpHeap(FILE* f)
{
	memblock_t* block;

	fprintf(f, "zone size: %i location: %p\n", mainzone->size, mainzone);

	for (block = mainzone->blocklist.next; ; block = block->next)
	{
		fprintf(f, "block:%p	size:%7i	user:%p	tag:%3i\n", block, block->size, block->user, block->tag);

		if (block->next == &mainzone->blocklist)
		{
			// all blocks have been hit
			break;
		}

		if ((byte*)block + block->size != (byte*)block->next)
			fprintf(f, "ERROR: block size does not touch the next block\n");

		if (block->next->prev != block)
			fprintf(f, "ERROR: next block doesn't have proper back link\n");

		if (block->tag == pu_tags_t::PU_FREE && block->next->tag == pu_tags_t::PU_FREE)
			fprintf(f, "ERROR: two consecutive free blocks\n");
	}
}

void Z_CheckHeap()
{
	memblock_t* block;

	for (block = mainzone->blocklist.next; ; block = block->next)
	{
		if (block->next == &mainzone->blocklist)
		{
			// all blocks have been hit
			break;
		}

		if ((byte*)block + block->size != (byte*)block->next)
			I_Error("Z_CheckHeap: block size does not touch the next block\n");

		if (block->next->prev != block)
			I_Error("Z_CheckHeap: next block doesn't have proper back link\n");

		if (block->tag == pu_tags_t::PU_FREE && block->next->tag == pu_tags_t::PU_FREE)
			I_Error("Z_CheckHeap: two consecutive free blocks\n");
	}
}

void Z_ChangeTag2(void* ptr, pu_tags_t tag, ::std::string file, int line)
{
	memblock_t* block;

	block = (memblock_t*)((byte*)ptr - sizeof(memblock_t));

	if (block->id != ZONEID)
		I_Error("%s:%i: Z_ChangeTag: block without a ZONEID!", file, line);

	if (tag >= pu_tags_t::PU_PURGELEVEL && block->user == NULL)
		I_Error("%s:%i: Z_ChangeTag: an owner is required for purgable blocks", file, line);

	block->tag = tag;
}

void Z_ChangeUser(void* ptr, void** user)
{
	memblock_t* block;

	block = (memblock_t*)((byte*)ptr - sizeof(memblock_t));

	if (block->id != ZONEID)
	{
		I_Error("Z_ChangeUser: Tried to change user for invalid block!");
	}

	block->user = user;
	*user = ptr;
}

auto Z_FreeMemory()
{
	memblock_t* block;
	int free;

	free = 0;

	for (block = mainzone->blocklist.next; block != &mainzone->blocklist; block = block->next)
	{
		if (block->tag == pu_tags_t::PU_FREE || block->tag >= pu_tags_t::PU_PURGELEVEL)
			free += block->size;
	}

	return free;
}

auto Z_ZoneSize()
{
	return mainzone->size;
}
*/
