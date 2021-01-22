/**********************************************************************************************************************************************\
	Copyright(C) 1993-1996 Id Software, Inc.
	Copyright(C) 2005-2014 Simon Howard

	This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License
	as published by the Free Software Foundation; either version 2 of the License, or (at your option) any later version.

	This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

	DESCRIPTION:
		Zone Memory Allocation. Neat.
		This is an implementation of the zone memory API which uses native calls to malloc() and free().
\**********************************************************************************************************************************************/
#include "../derma/stdafx.h"

/*
#include "z_zone.h"
#include "i_system.h"

constexpr size_t ZONEID{0x1d4a11};

struct memblock_t
{
	int id; // = ZONEID
	pu_tags_t tag;
	int size;
	void** user;
	memblock_t* prev;
	memblock_t* next;
};

// Linked list of allocated blocks for each tag type
static memblock_t* allocated_blocks[::std::size_t(pu_tags_t::PU_NUM_TAGS)];

#ifdef TESTING

static int test_malloced = 0;

void* test_malloc(size_t size)
{
	int* result;

	if (test_malloced + size > 2 * 1024 * 1024)
	{
		return nullptr;
	}

	test_malloced += size;

	result = static_cast<decltype(result)>(malloc(size + sizeof(int)));

	*result = size;

	return result + 1;
}

void test_free(void* data)
{
	int* i;

	i = ((int*)data) - 1;

	test_malloced -= *i;

	free(i);
}

#define malloc test_malloc
#define free test_free

#endif // #ifdef TESTING

// Add a block into the linked list for its type.
static void Z_InsertBlock(memblock_t* block)
{
	block->prev = NULL;
	block->next = allocated_blocks[::std::size_t(block->tag)];
	allocated_blocks[::std::size_t(block->tag)] = block;

	if (block->next != NULL)
	{
		block->next->prev = block;
	}
}

// Remove a block from its linked list.
static void Z_RemoveBlock(memblock_t* block)
{
	// Unlink from list
	if (block->prev == NULL)
	{
		// Start of list
		allocated_blocks[::std::size_t(block->tag)] = block->next;
	}
	else
	{
		block->prev->next = block->next;
	}

	if (block->next != NULL)
	{
		block->next->prev = block->prev;
	}
}

void Z_Init()
{
	memset(allocated_blocks, 0, sizeof(allocated_blocks));
	printf("zone memory: Using native C allocator.\n");
}

void Z_Free(void* ptr)
{
	memblock_t* block;

	block = (memblock_t*)((byte*)ptr - sizeof(memblock_t));

	if (block->id != ZONEID)
	{
		I_Error("Z_Free: freed a pointer without ZONEID");
	}

	if (block->tag != pu_tags_t::PU_FREE && block->user != NULL)
	{
		// clear the user's mark
		*block->user = NULL;
	}

	Z_RemoveBlock(block);

	// Free back to system
	free(block);
}

// Empty data from the cache list to allocate enough data of the size required. Returns true if any blocks were freed.
static bool ClearCache(int size)
{
	memblock_t* block;
	memblock_t* next_block;
	int remaining;

	block = allocated_blocks[::std::size_t(pu_tags_t::PU_CACHE)];

	if (block == NULL)
	{
		// Cache is already empty.
		return false;
	}

	// Search to the end of the pu_tags_t::PU_CACHE list.
	// The blocks at the end of the list are the ones that have been free for longer and are more likely to be unneeded now.
	while (block->next != NULL)
	{
		block = block->next;
	}

	//printf("out of memory; cleaning out the cache: %i\n", test_malloced);

	// Search backwards through the list freeing blocks until we have freed the amount of memory required.
	remaining = size;

	while (remaining > 0)
	{
		if (block == NULL)
		{
			// No blocks left to free; we've done our best.
			break;
		}

		next_block = block->prev;

		Z_RemoveBlock(block);

		remaining -= block->size;

		if (block->user)
		{
			*block->user = NULL;
		}

		free(block);
		block = next_block;
	}

	return true;
}

// You can pass a NULL user if the tag is < pu_tags_t::PU_PURGELEVEL.
void* Z_Malloc(int size, pu_tags_t tag, void* user)
{
	memblock_t* newblock;
	unsigned char* data;
	void* result;

	if (tag < pu_tags_t(0) || tag >= pu_tags_t::PU_NUM_TAGS || tag == pu_tags_t::PU_FREE)
	{
		I_Error("Z_Malloc: attempted to allocate a block with an invalid tag: %i", tag);
	}

	if (user == NULL && tag >= pu_tags_t::PU_PURGELEVEL)
	{
		I_Error("Z_Malloc: an owner is required for purgable blocks");
	}

	// Malloc a block of the required size
	newblock = NULL;

	while (newblock == NULL)
	{
		newblock = static_cast<decltype(newblock)>(malloc(sizeof(memblock_t) + size));

		if (newblock == NULL)
		{
			if (!ClearCache(sizeof(memblock_t) + size))
			{
				I_Error("Z_Malloc: failed on allocation of %i bytes", size);
			}
		}
	}

	newblock->tag = tag;

	// Hook into the linked list for this tag type
	newblock->id = ZONEID;
	// TODO unsure
	newblock->user = &user;
	newblock->size = size;

	Z_InsertBlock(newblock);

	data = (unsigned char*)newblock;
	result = data + sizeof(memblock_t);

	if (user != NULL)
	{
		*newblock->user = result;
	}

	return result;
}

void Z_FreeTags(int lowtag, int hightag)
{
	int i;

	for (i = lowtag; i <= hightag; ++i)
	{
		memblock_t* block;
		memblock_t* next;

		// Free all in this chain
		for (block = allocated_blocks[i]; block != NULL; )
		{
			next = block->next;

			// Free this block
			if (block->user != NULL)
			{
				*block->user = NULL;
			}

			free(block);

			// Jump to the next in the chain
			block = next;
		}

		// This chain is empty now
		allocated_blocks[i] = NULL;
	}
}

void Z_DumpHeap(int lowtag, int hightag)
{
	// broken
#if 0
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
#endif
}

void Z_FileDumpHeap(FILE* f)
{
	// broken
#if 0
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
#endif
}

void Z_CheckHeap()
{
	memblock_t* block;
	memblock_t* prev;

	// Check all chains
	for (size_t i{0}; i < ::std::size_t(pu_tags_t::PU_NUM_TAGS); ++i)
	{
		prev = NULL;

		for (block = allocated_blocks[i]; block != NULL; block = block->next)
		{
			if (block->id != ZONEID)
			{
				I_Error("Z_CheckHeap: Block without a ZONEID!");
			}

			if (block->prev != prev)
			{
				I_Error("Z_CheckHeap: Doubly-linked list corrupted!");
			}

			prev = block;
		}
	}
}

void Z_ChangeTag2(void* ptr, pu_tags_t tag, ::std::string file, int line)
{
	memblock_t* block;

	block = (memblock_t*)((byte*)ptr - sizeof(memblock_t));

	if (block->id != ZONEID)
	{
		I_Error("%s:%i: Z_ChangeTag: block without a ZONEID!", file, line);
	}

	if (tag >= pu_tags_t::PU_PURGELEVEL && block->user == NULL)
	{
		I_Error("%s:%i: Z_ChangeTag: an owner is required for purgable blocks", file, line);
	}

	// Remove the block from its current list, and rehook it into its new list.
	Z_RemoveBlock(block);
	block->tag = tag;
	Z_InsertBlock(block);
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

int Z_FreeMemory()
{
	// Limited by the system??
	return -1;
}

unsigned Z_ZoneSize()
{
	return 0;
}
*/
