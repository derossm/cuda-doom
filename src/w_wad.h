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

#include "doomtype.h"
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
extern unsigned int numlumps;

wad_file_t* W_AddFile(const char* filename);
void W_Reload();

lumpindex_t W_CheckNumForName(const char* name);
lumpindex_t W_GetNumForName(const char* name);
lumpindex_t W_CheckNumForNameFromTo(const char* name, int from, int to);

int W_LumpLength(lumpindex_t lump);
void W_ReadLump(lumpindex_t lump, void* dest);

void* W_CacheLumpNum(lumpindex_t lump, pu_tags_t tag);
void* W_CacheLumpName(const char* name, pu_tags_t tag);

void W_GenerateHashTable();

extern unsigned int W_LumpNameHash(const char* s);

void W_ReleaseLumpNum(lumpindex_t lump);
void W_ReleaseLumpName(const char* name);

const char* W_WadNameForLump(const lumpinfo_t* lump);
bool W_IsIWADLump(const lumpinfo_t* lump);
