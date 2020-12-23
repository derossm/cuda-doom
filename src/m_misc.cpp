/**********************************************************************************************************************************************\
	Copyright(C) 1993-1996 Id Software, Inc.
	Copyright(C) 1993-2008 Raven Software
	Copyright(C) 2005-2014 Simon Howard

	This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License
	as published by the Free Software Foundation; either version 2 of the License, or (at your option) any later version.

	This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

	DESCRIPTION:
		Miscellaneous.
\**********************************************************************************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>

#ifdef _WIN32
	#ifndef WIN32_LEAN_AND_MEAN
		#define WIN32_LEAN_AND_MEAN
	#endif

	#ifndef NOMINMAX
		#define NOMINMAX
	#endif

	#include <windows.h>
	#include <io.h>

	#ifdef _MSC_VER
		#include <direct.h>
	#endif
#else	// #ifndef _WIN32
	#include <sys/stat.h>
	#include <sys/types.h>
#endif	// _WIN32

#include "doomtype.h"

#include "deh_str.h"

#include "i_swap.h"
#include "i_system.h"
#include "i_video.h"
#include "m_misc.h"
#include "v_video.h"
#include "w_wad.h"
#include "z_zone.h"

// Safe string copy function that works like OpenBSD's strlcpy().
// Returns true if the string was not truncated.
auto M_StringCopy(char* dest, const char* src, size_t dest_size)
{
	if (dest_size >= 1)
	{
		dest[dest_size - 1] = '\0';
		strncpy(dest, src, dest_size - 1);
	}
	else
	{
		return false;
	}

	auto len{strlen(dest)};
	return src[len] == '\0';
}

// Safe string concat function that works like OpenBSD's strlcat().
// Returns true if string not truncated.
auto M_StringConcat(char* dest, const char* src, size_t dest_size)
{
	auto offset{strlen(dest)};
	if (offset > dest_size)
	{
		offset = dest_size;
	}

	return M_StringCopy(dest + offset, src, dest_size - offset);
}

// Returns true if 's' begins with the specified prefix.
auto M_StringStartsWith(const char* s, const char* prefix)
{
	return strlen(s) >= strlen(prefix) && strncmp(s, prefix, strlen(prefix)) == 0;
}

// Returns true if 's' ends with the specified suffix.
auto M_StringEndsWith(const char* s, const char* suffix)
{
	return strlen(s) >= strlen(suffix) && strcmp(s + strlen(s) - strlen(suffix), suffix) == 0;
}

auto M_FileExists(const char* filename)
{
	auto fstream{fopen(filename, "r")};

	if (fstream != NULL)
	{
		fclose(fstream);
		return true;
	}
	else
	{
		// If we can't open because the file is a directory, the "file" exists at least!
		return errno == EISDIR;
	}
}

auto M_FileLength(FILE* handle)
{
	// save the current position in the file
	auto savedpos{ftell(handle)};

	// jump to the end and find the length
	fseek(handle, 0, SEEK_END);
	auto length{ftell(handle)};

	// go back to the old location
	fseek(handle, savedpos, SEEK_SET);

	// TODO: find out of ftell uses negative values for errors, and IGNORE THEM instead of returning a huge number... or maybe handle them
	if (length < 0)
	{
		return (size_t)0;
	}

	return (size_t)length;
}

auto M_WriteFile(const char* name, const void* source, size_t length)
{
	auto handle{fopen(name, "wb")};
	if (handle == NULL)
	{
		return false;
	}

	auto count{fwrite(source, 1, length, handle)};
	fclose(handle);

	if (count < length)
	{
		return false;
	}

	return true;
}

auto M_ReadFile(const char* name, byte** buffer)
{
	auto handle{fopen(name, "rb")};
	if (handle == NULL)
	I_Error("Couldn't read file %s", name);

	// find the size of the file by seeking to the end and
	// reading the current position

	auto length{M_FileLength(handle)};

	//byte* buf = Z_Malloc(length + 1, PU_STATIC, NULL);
	// NOTE: I have no idea what Z_Malloc is, so I need to research this later
	auto buf{std::make_unique<byte*>(length + 1)};
	auto count{fread(*buf, 1, length, handle)};
	fclose (handle);

	if (count < length)
	I_Error("Couldn't read file %s", name);

	(*buf)[length] = '\0';
	*buffer = *buf;
	return length;
}

auto M_StrToInt(const char* str, int* result)
{
	return sscanf(str, " 0x%x", (unsigned int *) result) == 1
		|| sscanf(str, " 0X%x", (unsigned int *) result) == 1
		|| sscanf(str, " 0%o", (unsigned int *) result) == 1
		|| sscanf(str, " %d", result) == 1;
}

// Safe, portable vsnprintf().
auto M_vsnprintf(char* buf, size_t buf_len, const char* s, va_list args)
{
	if (buf_len == 0)
	{
		return 0;
	}

	// Windows (and other OSes?) has a vsnprintf() that doesn't always
	// append a trailing \0. So we must do it, and write into a buffer
	// that is one byte shorter; otherwise this function is unsafe.
	auto result{vsnprintf(buf, buf_len, s, args)};

	// If truncated, change the final char in the buffer to a \0.
	// A negative result indicates a truncated buffer on Windows.
	if (result < 0 || result >= buf_len)
	{
		buf[buf_len - 1] = '\0';
		result = buf_len - 1;
	}

	// TODO: find out if vsnprintf returns negative values, otherwise cast to size_t if this is an index or size
	return result;
}

// Safe, portable snprintf().
auto M_snprintf(char* buf, size_t buf_len, const char* s, ...)
{
	va_list args;
	va_start(args, s);
	auto result{M_vsnprintf(buf, buf_len, s, args)};
	va_end(args);
	return result;
}

// Returns the base filename described by the given path (without the directory name).
// The result points inside path and nothing new is allocated.
const char* M_BaseName(const char* path)
{
	auto p{strrchr(path, DIR_SEPARATOR)};
	if (p == NULL)
	{
		return path;
	}
	else
	{
		return p + 1;
	}
}

// M_StrCaseStr
//
// Case-insensitive version of strstr()
const char* M_StrCaseStr(const char* haystack, const char* needle)
{
	auto haystack_len{strlen(haystack)};
	auto needle_len{strlen(needle)};

	if (haystack_len < needle_len)
	{
		return nullptr;
	}

	auto len{haystack_len - needle_len};

	for (auto i{0}; i <= len; ++i)
	{
		if (!strncasecmp(haystack + i, needle, needle_len))
		{
			return haystack + i;
		}
	}

	return nullptr;
}

// Check if a file exists by probing for common case variation of its filename.
// Returns a newly allocated string that the caller is responsible for freeing.
std::unique_ptr<const char*> M_FileCaseExists(const char* path)
{
	auto path_dup{M_StringDuplicate(path)};

	// 0: actual path
	if (M_FileExists(*path_dup))
	{
		return path_dup;
	}

	auto filename = strrchr(*path_dup, DIR_SEPARATOR);
	if (filename != NULL)
	{
		filename++;
	}
	else
	{
		filename = *path_dup;
	}

	// 1: lowercase filename, e.g. doom2.wad
	M_ForceLowercase(filename);

	if (M_FileExists(*path_dup))
	{
		return path_dup;
	}

	// 2: uppercase filename, e.g. DOOM2.WAD
	M_ForceUppercase(filename);

	if (M_FileExists(*path_dup))
	{
		return path_dup;
	}

	// 3. uppercase basename with lowercase extension, e.g. DOOM2.wad
	auto ext = strrchr(*path_dup, '.');
	if (ext != NULL && ext > filename)
	{
		M_ForceLowercase(ext + 1);

		if (M_FileExists(*path_dup))
		{
			return path_dup;
		}
	}

	// 4. lowercase filename with uppercase first letter, e.g. Doom2.wad
	if (strlen(filename) > 1)
	{
		M_ForceLowercase(filename + 1);

		if (M_FileExists(*path_dup))
		{
			return path_dup;
		}
	}

	// 5. no luck
	return nullptr;
}

// Returns the path to a temporary file of the given name, stored inside the system temporary directory.
//
// The returned value must be freed with Z_Free after use.
std::unique_ptr<const char*> M_TempFile(const char* s)
{
	const char* tempdir;

#ifdef _WIN32
	// Check the TEMP environment variable to find the location.
	tempdir = getenv("TEMP");

	if (tempdir == NULL)
	{
		tempdir = ".";
	}
#else
	// In Unix, just use /tmp.
	tempdir = "/tmp";
#endif	// _WIN32

	// NOTE: what is getenv("TEMP") returning? heap or stack memory?
	return M_StringJoin(tempdir, DIR_SEPARATOR_S, s, NULL);
}

// Returns the directory portion of the given path, without the trailing slash separator character. If no directory is described in the path,
// the string "." is returned. In either case, the result is newly allocated and must be freed by the caller after use.
std::unique_ptr<const char*> M_DirName(const char* path)
{
	auto p{strrchr(path, DIR_SEPARATOR)};
	if (p == NULL)
	{
		return M_StringDuplicate(".");
	}
	else
	{
		auto result = M_StringDuplicate(path);
		(*result)[p - path] = '\0';
		return result;
	}
}

// Safe version of strdup() that checks the string was successfully allocated.
std::unique_ptr<const char*> M_StringDuplicate(const char* orig)
{
	/*
	auto result{strdup(orig)};

	if (result == NULL)
	{
		I_Error("Failed to duplicate string (length %" PRIuPTR ")\n", strlen(orig));
	}

	return result;
	*/

	return std::make_unique<const char*>(orig);
}

std::unique_ptr<const char*> M_StringReplace(const char* haystack, const char* needle, const char* replacement)
{
	auto needle_len{strlen(needle)};

	// Iterate through occurrences of 'needle' and calculate the size of the new string.
	auto result_len{strlen(haystack) + 1};
	auto repacement_len{strlen(replacement)};

	for (auto p{haystack}; ; )
	{
		p = strstr(p, needle);
		if (p == NULL)
		{
			break;
		}

		p += needle_len;
		result_len += repacement_len - needle_len;
	}

	// Construct new string.
	auto result{std::make_unique<char*>(result_len)};

	auto dst{*result};
	auto dst_len{result_len};

	for (auto p{haystack}; *p != '\0'; )
	{
		if (!strncmp(p, needle, needle_len))
		{
			M_StringCopy(dst, replacement, dst_len);
			p += needle_len;
			dst += repacement_len;
			dst_len -= repacement_len;
		}
		else
		{
			*dst = *p;
			++p;
			++dst;
			--dst_len;
		}
	}

	*dst = '\0';

	return result;
}

// Return a newly-malloced string with all the strings given as arguments concatenated together.
std::unique_ptr<const char*> M_StringJoin(const char* s, ...)
{
	va_list args;

	auto result_len{strlen(s) + (size_t)1};

	va_start(args, s);
	for (;;)
	{
		auto v{va_arg(args, const char*)};
		if (v == NULL)
		{
			break;
		}

		result_len += strlen(v);
	}
	va_end(args);

	auto result{std::make_unique<char*>(result_len)};

	M_StringCopy(*result, s, result_len);

	va_start(args, s);
	for (;;)
	{
		auto v{va_arg(args, const char*)};
		if (v == NULL)
		{
			break;
		}

		M_StringConcat(*result, v, result_len);
	}
	va_end(args);

	return result;
}

#ifdef _WIN32
std::unique_ptr<const char*> M_OEMToUTF8(const char* oem)
{
	auto size{strlen(oem)};
	if (size == std::numeric_limits<size_t>::max())
	{
		// oem is too big to convert
		return nullptr;
	}

	auto len{size + 1};

	//wchar_t* tmp = malloc(len * sizeof(wchar_t));
	auto tmp{std::make_unique<wchar_t*>(len)};
	MultiByteToWideChar(CP_OEMCP, 0, oem, len, *tmp, len);
	//char* result = malloc(len * 4);
	auto result{std::make_unique<char*>(len)};
	WideCharToMultiByte(CP_UTF8, 0, *tmp, len, *result, len * 4, NULL, NULL);

	return result;
}
#endif	// _WIN32

void M_MakeDirectory(const char* path)
{
#ifdef _WIN32
	mkdir(path);
#else
	mkdir(path, 0755);
#endif
}

void M_ExtractFileBase(const char* path, char* dest)
{
	auto size{strlen(path)};
	if (size == 0)
	{
		return;
	}

	auto src{path + size - 1};

	// back up until a \ or the start
	while (src != path && *(src - 1) != DIR_SEPARATOR)
	{
		--src;
	}

	auto filename{src};

	// Copy up to eight characters
	// Note: Vanilla Doom exits with an error if a filename is specified
	// with a base of more than eight characters. To remove the 8.3
	// filename limit, instead we simply truncate the name.

	size_t length{0};
	memset(dest, 0, 8);

	while (*src != '\0' && *src != '.')
	{
		if (length >= 8)
		{
			printf("Warning: Truncated '%s' lump name to '%.8s'.\n", filename, dest);
			break;
		}

		dest[length++] = toupper((int)*src++);
	}
}

void M_ForceUppercase(char* text)
{
	for (auto p{text}; *p != '\0'; ++p)
	{
		*p = toupper(*p);
	}
}

void M_ForceLowercase(char* text)
{
	for (auto p{text}; *p != '\0'; ++p)
	{
		*p = tolower(*p);
	}
}

// M_NormalizeSlashes
//
// Remove trailing slashes, translate backslashes to slashes
// The string to normalize is passed and returned in str
//
// killough 11/98: rewritten
//
// [STRIFE] - haleyjd 20110210: Borrowed from Eternity and adapted to respect
// the DIR_SEPARATOR define used by Choco Doom. This routine originated in
// BOOM.
void M_NormalizeSlashes(char* str)
{
	auto size{strlen(str)};
	if (size == 0)
	{
		return;
	}

	// Convert all slashes/backslashes to DIR_SEPARATOR
	if constexpr (DIR_SEPARATOR == '/')
	{
		for (auto i{0}; i < size; ++i)
		{
			if (str[i] == '\\')
			{
				str[i] = DIR_SEPARATOR;
			}
		}
	}
	else
	{
		for (auto i{0}; i < size; ++i)
		{
			if (str[i] == '/')
			{
				str[i] = DIR_SEPARATOR;
			}
		}
	}

	if (size == 1)
	{
		return;
	}

	// Remove trailing slashes
	for (auto i{size-1}; i > 0 && str[i] == DIR_SEPARATOR; --i)
	{
		str[i] = '\0';
	}

	// Collapse multiple slashes
	for (auto p{str}; *str++ = *p; )
	{
		if (*p++ == DIR_SEPARATOR)
		{
			while (*p == DIR_SEPARATOR)
			{
				++p;
			}
		}
	}
}
