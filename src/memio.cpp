/**********************************************************************************************************************************************\
	Copyright(C) 1993-1996 Id Software, Inc.
	Copyright(C) 2005-2014 Simon Howard

	This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License
	as published by the Free Software Foundation; either version 2 of the License, or (at your option) any later version.

	This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

// Emulates the IO functions in C stdio.h reading and writing to
// memory.
\**********************************************************************************************************************************************/


#include "memio.h"

#include "z_zone.h"

// Open a memory area for reading
auto mem_fopen_read(BufferType_Void* buf, size_t buflen)
{
	MEMFILE* file = Z_Malloc<MEMFILE*>(sizeof(MEMFILE), pu_tags_t::PU_STATIC, 0);

	file->buf = (unsigned char*) buf;
	file->buflen = buflen;
	file->position = 0;
	file->mode = memfile_mode_t::MODE_READ;

	return file;
}

// Read bytes
size_t mem_fread(BufferType_Void* buf, size_t size, size_t nmemb, MEMFILE* stream)
{
	size_t items;

	if (stream->mode != memfile_mode_t::MODE_READ)
	{
		printf("not a read stream\n");
		return (size_t)0;
	}

	// Trying to read more bytes than we have left?
	items = nmemb;

	if (items * size > stream->buflen - stream->position)
	{
		items = (stream->buflen - stream->position) / size;
	}

	// Copy bytes to buffer
	memcpy(buf, stream->buf + stream->position, items * size);

	// Update position
	stream->position += items * size;

	return items;
}

// Open a memory area for writing
auto mem_fopen_write()
{
	MEMFILE* file = Z_Malloc<MEMFILE*>(sizeof(MEMFILE), pu_tags_t::PU_STATIC, 0);

	file->alloced = 1024;
	file->buf = Z_Malloc<decltype(file->buf)>(file->alloced, pu_tags_t::PU_STATIC, 0);
	file->buflen = 0;
	file->position = 0;
	file->mode = memfile_mode_t::MODE_WRITE;

	return file;
}

// Write bytes to stream
auto mem_fwrite(const BufferType_Void* ptr, size_t size, size_t nmemb, MEMFILE* stream)
{
	size_t bytes;

	if (stream->mode != memfile_mode_t::MODE_WRITE)
	{
		return (size_t)0;
	}

	// More bytes than can fit in the buffer? If so, reallocate bigger.
	bytes = size * nmemb;

	while (bytes > stream->alloced - stream->position)
	{
		unsigned char* newbuf;

		newbuf = Z_Malloc<decltype(newbuf)>(stream->alloced * 2, pu_tags_t::PU_STATIC, 0);
		memcpy(newbuf, stream->buf, stream->alloced);
		Z_Free(stream->buf);
		stream->buf = newbuf;
		stream->alloced *= 2;
	}

	// Copy into buffer
	memcpy(stream->buf + stream->position, ptr, bytes);
	stream->position += bytes;

	if (stream->position > stream->buflen)
		stream->buflen = stream->position;

	return nmemb;
}

void mem_get_buf(MEMFILE* stream, BufferType_Void** buf, size_t* buflen)
{
	*buf = stream->buf;
	*buflen = stream->buflen;
}

void mem_fclose(MEMFILE* stream)
{
	if (stream->mode == memfile_mode_t::MODE_WRITE)
	{
		Z_Free(stream->buf);
	}

	Z_Free(stream);
}

auto mem_ftell(MEMFILE* stream)
{
	return stream->position;
}

auto mem_fseek(MEMFILE* stream, size_t position, mem_rel_t whence)
{
	size_t newpos;

	switch (whence)
	{
		case mem_rel_t::MEM_SEEK_SET:
			newpos = position;
			break;

		case mem_rel_t::MEM_SEEK_CUR:
			newpos = stream->position + position;
			break;

		case mem_rel_t::MEM_SEEK_END:
			newpos = stream->buflen + position;
			break;

		default:
			return false;
	}

	if (newpos < stream->buflen)
	{
		stream->position = newpos;
		return true;
	}
	else
	{
		printf("Error seeking to %u\n", newpos);
		return false;
	}
}
