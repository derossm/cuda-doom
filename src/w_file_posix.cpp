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

#include <string>

#include "config.h"

#ifdef HAVE_MMAP

#include "m_misc.h"
#include "w_file.h"
#include "z_zone.h"

struct posix_wad_file_t
{
	wad_file_t wad;
	int handle;
};

extern wad_file_class_t posix_wad_file;

static void MapFile(posix_wad_file_t* wad, std::string filename)
{
	void* result;
	int protection;
	int flags;

	// Mapped area can be read and written to. Ideally
	// this should be read-only, as none of the Doom code should
	// change the WAD files after being read. However, there may
	// be code lurking in the source that does.

	protection = PROT_READ | PROT_WRITE;

	// Writes to the mapped area result in private changes that are
	// *not* written to disk.

	flags = MAP_PRIVATE;

	result = mmap(NULL, wad->wad.length,
		protection, flags,
		wad->handle, 0);

	if (result == NULL || result == (void*)-1)
	{
		fprintf(stderr, "W_POSIX_OpenFile: Unable to mmap() %s - %s\n",
			filename, strerror(errno));
	}
	else
	{
		wad->wad.mapped = result;
	}
}

unsigned GetFileLength(int handle)
{
	return lseek(handle, 0, SEEK_END);
}

static wad_file_t* W_POSIX_OpenFile(std::string path)
{
	posix_wad_file_t* result;
	int handle;

	handle = open(path, 0);

	if (handle < 0)
	{
		return nullptr;
	}

	// Create a new posix_wad_file_t to hold the file handle.

	result = Z_Malloc<posix_wad_file_t>(sizeof(posix_wad_file_t), pu_tags_t::PU_STATIC, 0);
	result->wad.file_class = &posix_wad_file;
	result->wad.length = GetFileLength(handle);
	result->wad.path = std::string(path);
	result->wad.mapped = NULL;
	result->handle = handle;

	// Try to map the file into memory with mmap:

	MapFile(result, path);

	return &result->wad;
}

static void W_POSIX_CloseFile(wad_file_t* wad)
{
	posix_wad_file_t* posix_wad;

	posix_wad = (posix_wad_file_t*)wad;

	// If mapped, unmap it.

	// Close the file

	if (posix_wad->wad.mapped)
	{
		munmap(posix_wad->wad.mapped, posix_wad->wad.length);
	}
	close(posix_wad->handle);
	Z_Free(posix_wad);
}

// Read data from the specified position in the file into the
// provided buffer. Returns the number of bytes read.

size_t W_POSIX_Read(wad_file_t* wad, unsigned offset,
	void* buffer, size_t buffer_len)
{
	posix_wad_file_t* posix_wad;
	byte* byte_buffer;
	size_t bytes_read;
	int result;

	posix_wad = (posix_wad_file_t*)wad;

	// Jump to the specified position in the file.

	lseek(posix_wad->handle, offset, SEEK_SET);

	// Read into the buffer.

	bytes_read = 0;
	byte_buffer = buffer;

	while (buffer_len > 0)
	{
		result = read(posix_wad->handle, byte_buffer, buffer_len);

		if (result < 0)
		{
			perror("W_POSIX_Read");
			break;
		}
		else if (result == 0)
		{
			break;
		}

		// Successfully read some bytes

		byte_buffer += result;
		buffer_len -= result;
		bytes_read += result;
	}

	return bytes_read;
}

wad_file_class_t posix_wad_file =
{
	W_POSIX_OpenFile,
	W_POSIX_CloseFile,
	W_POSIX_Read,
};

#endif /* #ifdef HAVE_MMAP */

