/**********************************************************************************************************************************************\
	Copyright(C) 1993-1996 Id Software, Inc.
	Copyright(C) 2005-2014 Simon Howard

	This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License
	as published by the Free Software Foundation; either version 2 of the License, or (at your option) any later version.

	This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

	DESCRIPTION:
		Handles WAD file header, directory, lump I/O.
\**********************************************************************************************************************************************/
#include "../derma/stdafx.h"

#include "../i_swap.h"
#include "../i_system.h"
#include "../i_video.h"
#include "../m_misc.h"
#include "v_diskicon.h"

#include "w_wad.h"

// TODO research PACKED_STRUCT nonsense
struct wadinfo_t
{
	// Should be "IWAD" or "PWAD".
	char identification[4];
	int numlumps;
	int infotableofs;
};

struct filelump_t
{
	int filepos;
	int size;
	char name[8];
};

// Location of each lump on disk.
lumpinfo_t** lumpinfo;
unsigned numlumps = 0;

// Hash table for fast lookups
static lumpindex_t* lumphash;

// Variables for the reload hack: filename of the PWAD to reload, and the
// lumps from WADs before the reload file, so we can resent numlumps and
// load the file again.
static wad_file_t* reloadhandle = nullptr;
static lumpinfo_t* reloadlumps = nullptr;
::std::string reloadname = nullptr;
static int reloadlump = -1;

// Hash function used for lump names.
unsigned W_LumpNameHash(::std::string s)
{
	// This is the djb2 string hash function, modded to work on strings that have a maximum length of 8.
	unsigned result = 5381;

	for (size_t i{0}; i < 8 && s[i] != '\0'; ++i)
	{
		result = ((result << 5) ^ result) ^ toupper(s[i]);
	}

	return result;
}

// All files are optional, but at least one file must be found (PWAD, if all required lumps are present).
// Files with a .wad extension are wadlink files with multiple lumps.
// Other files are single lumps with the base filename for the lump name.
wad_file_t* W_AddFile(::std::filesystem::path filename)
{
	wadinfo_t header;
	lumpindex_t i;
	int length;
	int startlump;
	filelump_t* fileinfo;
	filelump_t* filerover;
	lumpinfo_t* filelumps;
	int numfilelumps;

	// If the filename begins with a ~, it indicates that we should use the reload hack.
	if (filename.string().starts_with() == '~')
	{
		if (!reloadname.empty())
		{
			I_Error("Prefixing a WAD filename with '~' indicates that the WAD should be reloaded\n"
				"on each level restart, for use by level authors for rapid development. You\n"
				"can only reload one WAD file, and it must be the last file in the -file list.");
		}

		reloadname = strdup(filename);
		reloadlump = numlumps;
		++filename;
	}

	// Open the file and add to directory
	auto wad_file = W_OpenFile(filename);

	if (wad_file == NULL)
	{
		printf(" couldn't open %s\n", filename);
		return nullptr;
	}

	if (iequals(filename + strlen(filename) - 3, "wad"))
	{
		// single lump file

		// fraggle: Swap the filepos and size here. The WAD directory
		// parsing code expects a little-endian directory, so will swap
		// them back. Effectively we're constructing a "fake WAD directory"
		// here, as it would appear on disk.
		fileinfo = Z_Malloc<filelump_t*>(sizeof(filelump_t), pu_tags_t::PU_STATIC, 0);
		fileinfo->filepos = LONG(0);
		fileinfo->size = LONG(wad_file->length);

		// Name the lump after the base of the filename (without the extension).
		M_ExtractFileBase(filename, fileinfo->name);
		numfilelumps = 1;
	}
	else
	{
		// WAD file
		W_Read(wad_file, 0, &header, sizeof(header));

		if (strncmp(header.identification, "IWAD", 4))
		{
			// Homebrew levels?
			if (strncmp(header.identification, "PWAD", 4))
			{
				W_CloseFile(wad_file);
				I_Error("Wad file %s doesn't have IWAD or PWAD id\n", filename);
			}
			// ???modifiedgame = true;
		}

		header.numlumps = LONG(header.numlumps);

		// Vanilla Doom doesn't like WADs with more than 4046 lumps
		// https://www.doomworld.com/vb/post/1010985
		// disable PWAD lump number limit
		if (!strncmp(header.identification, "PWAD", 4) && header.numlumps > 4046 && false)
		{
			W_CloseFile(wad_file);
			I_Error("Error: Vanilla limit for lumps in a WAD is 4046, PWAD %s has %d", filename, header.numlumps);
		}

		header.infotableofs = LONG(header.infotableofs);
		length = header.numlumps * sizeof(filelump_t);
		fileinfo = Z_Malloc<decltype(fileinfo)>(length, pu_tags_t::PU_STATIC, 0);

		W_Read(wad_file, header.infotableofs, fileinfo, length);
		numfilelumps = header.numlumps;
	}

	// Increase size of numlumps array to accomodate the new file.
	filelumps = static_cast<decltype(filelumps)>(calloc(numfilelumps, sizeof(lumpinfo_t)));
	if (filelumps == NULL)
	{
		W_CloseFile(wad_file);
		I_Error("Failed to allocate array for lumps from new file.");
	}

	startlump = numlumps;
	numlumps += numfilelumps;
	lumpinfo = static_cast<decltype(lumpinfo)>(I_Realloc(lumpinfo, numlumps * sizeof(lumpinfo_t*)));
	filerover = fileinfo;

	for (i = startlump; i < numlumps; ++i)
	{
		lumpinfo_t* lump_p = &filelumps[i - startlump];
		lump_p->wad_file = wad_file;
		lump_p->position = LONG(filerover->filepos);
		lump_p->size = LONG(filerover->size);
		lump_p->cache = NULL;
		strncpy(lump_p->name, filerover->name, 8);
		lumpinfo[i] = lump_p;

		++filerover;
	}

	Z_Free(fileinfo);

	if (lumphash != NULL)
	{
		Z_Free(lumphash);
		lumphash = NULL;
	}

	// If this is the reload file, we need to save some details about the
	// file so that we can close it later on when we do a reload.
	if (reloadname)
	{
		reloadhandle = wad_file;
		reloadlumps = filelumps;
	}

	return wad_file;
}

int W_NumLumps()
{
	return numlumps;
}

// Returns -1 if name not found.
lumpindex_t W_CheckNumForName(::std::string name)
{
	// Do we have a hash table yet?
	if (lumphash != NULL)
	{
		// We do! Excellent.
		auto hash = W_LumpNameHash(name) % numlumps;

		for (lumpindex_t i = lumphash[hash]; i != -1; i = lumpinfo[i]->next)
		{
			if (!strncasecmp(lumpinfo[i]->name, name, 8))
			{
				return i;
			}
		}
	}
	else
	{
		// We don't have a hash table generate yet. Linear search :-(
		// scan backwards so patch lump files take precedence
		for (lumpindex_t i = numlumps - 1; i >= 0; --i)
		{
			if (!strncasecmp(lumpinfo[i]->name, name, 8))
			{
				return i;
			}
		}
	}

	// TFB. Not found.
	return -1;
}

// Calls W_CheckNumForName, but bombs out if not found.
lumpindex_t W_GetNumForName(::std::string name)
{
	lumpindex_t i = W_CheckNumForName(name);

	if (i < 0)
	{
		I_Error("W_GetNumForName: %s not found!", name);
	}

	return i;
}

lumpindex_t W_CheckNumForNameFromTo(::std::string name, int from, int to)
{
	for (lumpindex_t i = from; i >= to; --i)
	{
		if (!strncasecmp(lumpinfo[i]->name, name, 8))
		{
			return i;
		}
	}

	return -1;
}

// Returns the buffer size needed to load the given lump.
int W_LumpLength(lumpindex_t lump)
{
	if (lump >= numlumps)
	{
		I_Error("W_LumpLength: %i >= numlumps", lump);
	}

	return lumpinfo[lump]->size;
}

// Loads the lump into the given buffer, which must be >= W_LumpLength().
void W_ReadLump(lumpindex_t lump, void* dest)
{
	if (lump >= numlumps)
	{
		I_Error("W_ReadLump: %i >= numlumps", lump);
	}

	auto l = lumpinfo[lump];

	V_BeginRead(l->size);

	auto c = W_Read(l->wad_file, l->position, dest, l->size);

	if (c < l->size)
	{
		I_Error("W_ReadLump: only read %i of %i on lump %i", c, l->size, lump);
	}
}

// Release a lump back to the cache, so that it can be reused later
// without having to read from disk again, or alternatively, discarded
// if we run out of memory.
//
// Back in Vanilla Doom, this was just done using Z_ChangeTag
// directly, but now that we have WAD mmap, things are a bit more
// complicated ...
void W_ReleaseLumpNum(lumpindex_t lumpnum)
{
	lumpinfo_t* lump;

	if ((unsigned)lumpnum >= numlumps)
	{
		I_Error("W_ReleaseLumpNum: %i >= numlumps", lumpnum);
	}

	lump = lumpinfo[lumpnum];

	if (lump->wad_file->mapped != NULL)
	{
		// Memory-mapped file, so nothing needs to be done here.
	}
	else
	{
		Z_ChangeTag(lump->cache, pu_tags_t::PU_CACHE);
	}
}

void W_ReleaseLumpName(::std::string name)
{
	W_ReleaseLumpNum(W_GetNumForName(name));
}

#if 0

int info[2500][10];
int profilecount;

void W_Profile()
{
	for (auto i{0}; i < numlumps; ++i)
	{
		char ch;

		auto ptr = lumpinfo[i].cache;
		if (!ptr)
		{
			ch = ' ';
			continue;
		}
		else
		{
			auto block = (memblock_t*)((byte*)ptr - sizeof(memblock_t));
			if (block->tag < pu_tags_t::PU_PURGELEVEL)
			{
				ch = 'S';
			}
			else
			{
				ch = 'P';
			}
		}
		info[i][profilecount] = ch;
	}
	++profilecount;

	auto f{fopen("waddump.txt", "w")};

	char name[9];
	name[8] = 0;

	for (auto i{0}; i < numlumps; ++i)
	{
		memcpy(name, lumpinfo[i].name, 8);

		auto j{0};
		for (; j < 8; ++j)
		{
			if (!name[j])
			{
				break;
			}
		}

		for (; j < 8; ++j)
		{
			name[j] = ' ';
		}

		fprintf(f, "%s ", name);

		for (auto k{0}; k < profilecount; ++k)
		{
			fprintf(f, "	%c", info[i][k]);
		}

		fprintf(f, "\n");
	}
	fclose(f);
}

#endif

// Generate a hash table for fast lookups
void W_GenerateHashTable()
{
	// Free the old hash table, if there is one:
	if (lumphash)
	{
		Z_Free(lumphash);
	}

	// Generate hash table
	if (numlumps > 0)
	{
		lumphash = Z_Malloc<lumpindex_t>(sizeof(lumpindex_t) * numlumps, pu_tags_t::PU_STATIC, NULL);

		for (lumpindex_t i = 0; i < numlumps; ++i)
		{
			lumphash[i] = -1;
		}

		for (lumpindex_t i = 0; i < numlumps; ++i)
		{
			auto hash = W_LumpNameHash(lumpinfo[i]->name) % numlumps;

			// Hook into the hash table
			lumpinfo[i]->next = lumphash[hash];
			lumphash[hash] = i;
		}
	}

	// All done!
}

// The Doom reload hack. The idea here is that if you give a WAD file to -file
// prefixed with the ~ hack, that WAD file will be reloaded each time a new
// level is loaded. This lets you use a level editor in parallel and make
// incremental changes to the level you're working on without having to restart
// the game after every change.
// But: the reload feature is a fragile hack...
void W_Reload()
{
	if (!reloadname)
	{
		return;
	}

	// We must free any lumps being cached from the PWAD we're about to reload:
	for (lumpindex_t i = reloadlump; i < numlumps; ++i)
	{
		if (lumpinfo[i]->cache)
		{
			Z_Free(lumpinfo[i]->cache);
		}
	}

	// Reset numlumps to remove the reload WAD file:
	numlumps = reloadlump;

	// Now reload the WAD file.
	auto filename = reloadname;

	W_CloseFile(reloadhandle);
	free(reloadlumps);

	reloadname = nullptr;
	reloadlump = -1;
	reloadhandle = nullptr;
	W_AddFile(filename);
	free(filename);

	// The WAD directory has changed, so we have to regenerate the fast lookup hashtable:
	W_GenerateHashTable();
}

::std::string W_WadNameForLump(const lumpinfo_t* lump)
{
	return M_BaseName(lump->wad_file->path);
}

bool W_IsIWADLump(const lumpinfo_t* lump)
{
	return lump->wad_file == lumpinfo[0]->wad_file;
}

// dump lump data into a new LMP file
lumpindex_t W_LumpDump(::std::string lumpname)
{
	auto i = W_CheckNumForName(lumpname);

	if (i < 0 || !lumpinfo[i]->size)
	{
		return -1;
	}

	// open file for writing
	auto filename = ::std::string(lumpname + ".lmp");
	//M_ForceLowercase(filename); TODO workaround
	auto fp = fopen(filename, "wb");
	if (!fp)
	{
		I_Error("W_LumpDump: Failed writing to file '%s'!", filename);
	}

	auto lump_p = (char*)malloc(lumpinfo[i]->size);
	W_ReadLump(i, lump_p);
	fwrite(lump_p, 1, lumpinfo[i]->size, fp);
	free(lump_p);

	fclose(fp);

	return i;
}
