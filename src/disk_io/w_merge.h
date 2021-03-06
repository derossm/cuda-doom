/**********************************************************************************************************************************************\
	Copyright(C) 2005-2014 Simon Howard

	This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License
	as published by the Free Software Foundation; either version 2 of the License, or (at your option) any later version.

	This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

	DESCRIPTION:
		Handles merging of PWADs, similar to deutex's -merge option
		Ideally this should work exactly the same as in deutex, but trying to read the deutex source code made my brain hurt.
\**********************************************************************************************************************************************/
#pragma once

#include "../../derma/common.h"

constexpr size_t W_NWT_MERGE_SPRITES{0x1};
constexpr size_t W_NWT_MERGE_FLATS{0x2};

// Add a new WAD and merge it into the main directory
void W_MergeFile(::std::string filename);

// NWT-style merging
void W_NWTMergeFile(::std::string filename, int flags);

inline constexpr void W_NWTMergeFileSprites(::std::string filename) noexcept { W_NWTMergeFile(filename, W_NWT_MERGE_SPRITES); }
inline constexpr void W_NWTMergeFileFlats(::std::string filename) noexcept { W_NWTMergeFile(filename, W_NWT_MERGE_FLATS); }
inline constexpr void W_NWTMergeFileBoth(::std::string filename) noexcept { W_NWTMergeFile(filename, W_NWT_MERGE_SPRITES | W_NWT_MERGE_FLATS); }

// Acts the same as NWT's "-merge" option.
void W_NWTDashMerge(::std::string filename);

// Debug function that prints the WAD directory.
void W_PrintDirectory();
