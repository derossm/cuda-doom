/**********************************************************************************************************************************************\
	Copyright(C) 1993-1996 Id Software, Inc.
	Copyright(C) 2005-2014 Simon Howard

	This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License
	as published by the Free Software Foundation; either version 2 of the License, or (at your option) any later version.

	This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

	DESCRIPTION:
		Miscellaneous.
\**********************************************************************************************************************************************/
#pragma once

#include "../derma/common.h"

#include "doomtype.h"

auto M_WriteFile(const char* name, const void* source, size_t length);
auto M_ReadFile(const char* name, byte** buffer);
auto M_FileExists(const char* file);
auto M_FileLength(FILE* handle);
auto M_StrToInt(const char* str, int* result);
auto M_StringCopy(char* dest, const char* src, size_t dest_size);
auto M_StringConcat(char* dest, const char* src, size_t dest_size);
auto M_StringStartsWith(const char* s, const char* prefix);
auto M_StringEndsWith(const char* s, const char* suffix);
auto M_vsnprintf(char* buf, size_t buf_len, const char* s, va_list args);
auto M_snprintf(char* buf, size_t buf_len, const char* s, ...) PRINTF_ATTR(3, 4);
const char* M_BaseName(const char* path);
const char* M_StrCaseStr(const char* haystack, const char* needle);
std::unique_ptr<const char*> M_TempFile(const char* s);
std::unique_ptr<const char*> M_FileCaseExists(const char* file);
std::unique_ptr<const char*> M_DirName(const char* path);
std::unique_ptr<const char*> M_StringDuplicate(const char* orig);
std::unique_ptr<const char*> M_StringReplace(const char* haystack, const char* needle, const char* replacement);
std::unique_ptr<const char*> M_StringJoin(const char* s, ...);
std::unique_ptr<const char*> M_OEMToUTF8(const char* ansi);
void M_MakeDirectory(const char* dir);
void M_ExtractFileBase(const char* path, char* dest);
void M_ForceUppercase(char* text);
void M_ForceLowercase(char* text);
void M_NormalizeSlashes(char* str);
