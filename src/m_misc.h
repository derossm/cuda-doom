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

#include "i_swap.h"
#include "i_system.h"
#include "i_video.h"
#include "v_video.h"
#include "disk_io/w_wad.h"
#include "z_zone.h"

#include "disk_io/deh_str.h"

int M_StringCopy(...) { return 0; }
int M_StringConcat(...) { return 0; }

// Returns true if 's' begins with the specified prefix.
auto M_StringStartsWith(::std::string s, ::std::string prefix)
{
	// OBSOLETE
}

// Returns true if 's' ends with the specified suffix.
auto M_StringEndsWith(::std::string s, ::std::string suffix)
{
	// OBSOLETE
}

auto M_FileExists(::std::string filename)
{
	auto fstream{fopen(filename.c_str(), "r")};

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
		return ::std::size_t(0);
	}

	return ::std::size_t(length);
}

auto M_WriteFile(::std::string name, ::std::string source)
{
	auto handle{fopen(name.c_str(), "wb")};
	if (handle == NULL)
	{
		return false;
	}

	auto count{fwrite(source.c_str(), 1, source.length(), handle)};
	fclose(handle);

	if (count < source.length())
	{
		return false;
	}

	return true;
}

auto M_ReadFile(::std::string name, byte** buffer)
{
	auto handle{fopen(name.c_str(), "rb")};
	if (handle == NULL)
	{
		I_Error("Couldn't read file %s", name.c_str());
	}

	// find the size of the file by seeking to the end and reading the current position
	auto length{M_FileLength(handle)};

	//byte* buf = Z_Malloc<decltype(//byte* buf)>(length + 1, pu_tags_t::PU_STATIC, NULL);
	auto buf{::std::make_unique<byte*>(length + 1)};
	auto count{fread(*buf, 1, length, handle)};
	fclose(handle);

	if (count < length)
	{
		I_Error("Couldn't read file %s", name.c_str());
	}

	(*buf)[length] = '\0';
	*buffer = *buf;
	return length;
}

auto M_StrToInt(::std::string str, int* result)
{
	/* return sscanf(str.c_str(), " 0x%x", (unsigned*) result) == 1
		|| sscanf(str.c_str(), " 0X%x", (unsigned*) result) == 1
		|| sscanf(str.c_str(), " 0%o", (unsigned*) result) == 1
		|| sscanf(str.c_str(), " %d", result) == 1; */

	return 0;
}

// Safe, portable vsnprintf().
int M_vsnprintf(...)
{
	// Windows (and other OSes?) has a vsnprintf() that doesn't always
	// append a trailing \0. So we must do it, and write into a buffer
	// that is one byte shorter; otherwise this function is unsafe.
	//auto result{vsnprintf(buf.c_str(), buf.length(), s.c_str(), args)};

	// If truncated, change the final char in the buffer to a \0.
	// A negative result indicates a truncated buffer on Windows.
	//if (result < 0 || result >= buf_len)
	//{
		//buf[buf_len - 1] = '\0';
		//result = buf_len - 1;
	//}

	// TODO: find out if vsnprintf returns negative values, otherwise cast to size_t if this is an index or size
	//return result;
	return 0;
}

// Safe, portable snprintf().
template<typename... Ts>
::std::string M_snprintf(Ts ...ts)
{
	auto is = {ts...};

	::std::string buf;
	for (auto& iter : is)
	{
		buf += ::std::string(iter);
	}

	return buf;
}

// Returns the base filename described by the given path (without the directory name).
// The result points inside path and nothing new is allocated.
::std::string M_BaseName(::std::string path)
{
	//auto p{strrchr(path.c_str(), DIR_SEPARATOR)};
	//if (p == NULL)
	//{
		//return path;
	//}
	//else
	//{
		//return p + 1;
	//}
	return ::std::string{};
}

// M_StrCaseStr
// Case-insensitive version of strstr()
::std::string M_StrCaseStr(::std::string haystack, ::std::string needle)
{
	/*	auto haystack_len{strlen(haystack)};
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
		} */

	return ::std::string{};
}

// Check if a file exists by probing for common case variation of its filename.
// Returns a newly allocated string that the caller is responsible for freeing.
::std::string M_FileCaseExists(::std::string path)
{
	/*	auto path_dup{::std::string(path)};

		// 0: actual path
		if (M_FileExists(path_dup))
		{
			return path_dup;
		}

		auto filename = strrchr(path_dup.c_str(), DIR_SEPARATOR);
		if (filename != NULL)
		{
			++filename;
		}
		else
		{
			filename = path_dup.c_str();
		}

		// 1: lowercase filename, e.g. doom2.wad
		//M_ForceLowercase(filename);

		if (M_FileExists(path_dup))
		{
			return path_dup;
		}

		// 2: uppercase filename, e.g. DOOM2.WAD
		//M_ForceUppercase(filename);

		if (M_FileExists(path_dup))
		{
			return path_dup;
		}

		// 3. uppercase basename with lowercase extension, e.g. DOOM2.wad
		auto ext = strrchr(path_dup.c_str(), '.');
		if (ext != NULL && ext > filename)
		{
			//M_ForceLowercase(ext + 1);

			if (M_FileExists(path_dup))
			{
				return path_dup;
			}
		}

		// 4. lowercase filename with uppercase first letter, e.g. Doom2.wad
		if (strlen(filename) > 1)
		{
			//M_ForceLowercase(filename + 1);

			if (M_FileExists(path_dup))
			{
				return path_dup;
			}
		} */

		// 5. no luck
	return ::std::string{};
}

// Returns the path to a temporary file of the given name, stored inside the system temporary directory.
//
// The returned value must be freed with Z_Free after use.
::std::string M_TempFile(::std::string s)
{
	/* ::std::string tempdir;

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
	return ::std::string(tempdir + DIR_SEPARATOR_S + s); */
	return ::std::string{};
}

// Returns the directory portion of the given path, without the trailing slash separator character. If no directory is described in the path,
// the string "." is returned. In either case, the result is newly allocated and must be freed by the caller after use.
::std::string M_DirName(::std::string path)
{
	auto p{strrchr(path.c_str(), DIR_SEPARATOR)};
	if (p == NULL)
	{
		return ::std::string(".");
	}
	else
	{
		auto result = ::std::string(path);
		//(result)[p - path] = '\0';
		return result;
	}
}

::std::string M_StringReplace(::std::string haystack, ::std::string needle, ::std::string replacement)
{
	//auto needle_len{needle.length()};

	// Iterate through occurrences of 'needle' and calculate the size of the new string.
	//auto result_len{haystack.length() + 1};
	//auto repacement_len{replacement.length()};

	// FIXME
/*	for (auto p{haystack}; ; )
	{
		p = ::std::string(strstr(p.c_str(), needle.c_str()));
		if (p == NULL)
		{
			break;
		}

		p += needle_len;
		result_len += repacement_len - needle_len;
	}

	// Construct new string.
	auto result{::std::string(result_len)};

	auto dst{result};
	auto dst_len{result_len};

	for (auto p{haystack}; *p != '\0'; )
	{
		if (!strncmp(p.c_str(), needle.c_str(), needle_len))
		{
			//dst = ::std::string(replacement);
			//p += needle_len;
			//dst += repacement_len;
			//dst_len -= repacement_len;
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

	return result; */
	return ::std::string{};
}

// Return a newly-malloced string with all the strings given as arguments concatenated together.
::std::string [[deprecated]] M_StringJoin(::std::string s, ...)
{
	/* va_list args;

	auto result_len{strlen(s) + 1};

	va_start(args, s);
	for (;;)
	{
		auto v{va_arg(args, ::std::string)};
		if (v == NULL)
		{
			break;
		}

		result_len += strlen(v);
	}
	va_end(args);

	auto result{::std::make_unique<char*>(result_len)};

	M_StringCopy(*result, s, result_len);

	va_start(args, s);
	for (;;)
	{
		auto v{va_arg(args, ::std::string)};
		if (v == NULL)
		{
			break;
		}

		M_StringConcat(*result, v, result_len);
	}
	va_end(args);

	return result; */
	return ::std::string{};
}

#ifdef _WIN32
::std::string M_OEMToUTF8(::std::string oem)
{
	/*	auto size{strlen(oem)};
		if (size == ::std::numeric_limits<size_t>::max())
		{
			// oem is too big to convert
			return nullptr;
		}

		auto len{size + 1};

		//wchar_t* tmp = static_cast<decltype(tmp)>(malloc(len * sizeof(wchar_t)));
		auto tmp{::std::make_unique<wchar_t*>(len)};
		MultiByteToWideChar(CP_OEMCP, 0, oem, len, *tmp, len);
		//char* result = static_cast<decltype(result)>(malloc(len * 4));
		auto result{::std::make_unique<char*>(len)};
		WideCharToMultiByte(CP_UTF8, 0, *tmp, len, *result, len * 4, NULL, NULL);

		return result; */
	return ::std::string{};
}
#endif	// _WIN32

void M_MakeDirectory(::std::string path)
{
#ifdef _WIN32
	mkdir(path);
#else
	mkdir(path, 0755);
#endif
}

void M_ExtractFileBase(::std::string path, ::std::string dest)
{
	/*	auto size{strlen(path)};
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

			dest[length] = toupper((int)*src);
			++length;
			++src;
		} */
}

void M_ForceUppercase(::std::string text)
{
	/*	for (auto p{text}; *p != '\0'; ++p)
		{
			*p = toupper(*p);
		} */
}

void M_ForceLowercase(::std::string text)
{
	/*	for (auto p{text}; *p != '\0'; ++p)
		{
			*p = tolower(*p);
		} */
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
void M_NormalizeSlashes(::std::string str)
{
	/*	auto size{strlen(str)};
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
		for (auto p{str};* (str++) = *p; )
		{
			if (*p == DIR_SEPARATOR)
			{
				++p;
				while (*p == DIR_SEPARATOR)
				{
					++p;
				}
			}
			else
			{
				++p;
			}
		} */
}
