/**********************************************************************************************************************************************\
	Copyright(C) 1993-1996 Id Software, Inc.
	Copyright(C) 2005-2014 Simon Howard

	This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License
	as published by the Free Software Foundation; either version 2 of the License, or (at your option) any later version.

	This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

	DESCRIPTION:
		WAD I/O functions.
\**********************************************************************************************************************************************/
#pragma once

#include "../derma/common.h"

#include "w_file.h"
#include "z_zone.h"

using lumpindex_t = int;

struct lumpinfo_t
{
	char name[8];
	wad_file_t* wad_file;
	int position;
	int size;
	void* cache;

	// Used for hash table lookups
	lumpindex_t next;
};

extern lumpinfo_t** lumpinfo;
extern unsigned numlumps;

wad_file_t* W_AddFile(::std::string filename);
void W_Reload();

lumpindex_t W_CheckNumForName(::std::string name);
lumpindex_t W_GetNumForName(::std::string name);
lumpindex_t W_CheckNumForNameFromTo(::std::string name, int from, int to);

int W_LumpLength(lumpindex_t lump);
void W_ReadLump(lumpindex_t lump, void* dest);

// Load a lump into memory and return a pointer to a buffer containing the lump data.
// 'tag' is the type of zone memory buffer to allocate for the lump (usually pu_tags_t::PU_STATIC or pu_tags_t::PU_CACHE).
// If the lump is loaded as pu_tags_t::PU_STATIC, it should be released back using W_ReleaseLumpNum when no longer needed (do not use Z_ChangeTag).
template<typename T>
T* W_CacheLumpNum(lumpindex_t lumpnum, pu_tags_t tag)
{
	if ((unsigned)lumpnum >= numlumps)
	{
		I_Error("W_CacheLumpNum: %i >= numlumps", lumpnum);
	}

	auto lump = lumpinfo[lumpnum];

	T result;
	// Get the pointer to return. If the lump is in a memory-mapped file, we can just return a pointer to within the memory-mapped region.
	// If the lump is in an ordinary file, we may already have it cached; otherwise, load it into memory.
	if (lump->wad_file->mapped != nullptr)
	{
		// Memory mapped file, return from the mmapped region.
		result = static_cast<T*>(lump->wad_file->mapped + lump->position);
	}
	else if (lump->cache != nullptr)
	{
		// Already cached, so just switch the zone tag.
		result = static_cast<T*>(lump->cache);
		Z_ChangeTag(lump->cache, tag);
	}
	else
	{
		// Not yet loaded, so load it now
		lump->cache = Z_Malloc<decltype(lump->cache)>(W_LumpLength(lumpnum), tag, &lump->cache);
		W_ReadLump(lumpnum, lump->cache);
		result = static_cast<T*>(lump->cache);
	}

	return result;
}

template<typename T>
T* W_CacheLumpName(::std::string name, pu_tags_t tag)
{
	return W_CacheLumpNum<T>(W_GetNumForName(name), tag);
}

void W_GenerateHashTable();

extern unsigned W_LumpNameHash(::std::string s);

void W_ReleaseLumpNum(lumpindex_t lump);
void W_ReleaseLumpName(::std::string name);

::std::string W_WadNameForLump(const lumpinfo_t* lump);
bool W_IsIWADLump(const lumpinfo_t* lump);
