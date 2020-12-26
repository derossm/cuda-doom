/**********************************************************************************************************************************************\
	Copyright(C) 1993-1996 Id Software, Inc.
	Copyright(C) 2005-2014 Simon Howard

	This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License
	as published by the Free Software Foundation; either version 2 of the License, or (at your option) any later version.

	This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
\**********************************************************************************************************************************************/
#pragma once

#include "../derma/common.h"

enum class memfile_mode_t
{
	MODE_READ,
	MODE_WRITE
};

enum class mem_rel_t
{
	MEM_SEEK_SET,
	MEM_SEEK_CUR,
	MEM_SEEK_END
};

using BufferType = unsigned char;
using BufferType_Void = unsigned char;

struct MEMFILE
{
	BufferType* buf;
	size_t buflen;
	size_t alloced;
	size_t position;
	memfile_mode_t mode;
};

auto mem_fopen_read(BufferType_Void* buf, size_t buflen);
size_t mem_fread(BufferType_Void* buf, size_t size, size_t nmemb, MEMFILE* stream);

auto mem_fopen_write();
auto mem_fwrite(const BufferType_Void* ptr, size_t size, size_t nmemb, MEMFILE* stream);

void mem_get_buf(MEMFILE* stream, BufferType_Void** buf, size_t* buflen);
void mem_fclose(MEMFILE* stream);
auto mem_ftell(MEMFILE* stream);
auto mem_fseek(MEMFILE* stream, signed long offset, mem_rel_t whence);
