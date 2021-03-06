/**********************************************************************************************************************************************\
	Copyright(C) 2018 Simon Howard

	This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License
	as published by the Free Software Foundation; either version 2 of the License, or (at your option) any later version.

	This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

	DESCRIPTION:
		File globbing API. This allows the contents of the filesystem to be interrogated.
\**********************************************************************************************************************************************/
#include "../derma/stdafx.h"

#include "i_glob.h"
#include "m_misc.h"

// Only the fields d_name and (as an XSI extension) d_ino are specified in POSIX.1.
// Other than Linux, the d_type field is available mainly only on BSD systems. The remaining fields are available on many, but not all systems.
bool IsDirectory(::std::string dir)
{
	return ::std::filesystem::directory_iterator{::std::filesystem::path{dir}}->is_directory();
}

// CALLED EXTERN
glob_t* I_StartMultiGlob(::std::string directory, int flags, ::std::string glob, ...)
{
	char** globs;
	int num_globs;
	glob_t* result;
	va_list args;

	globs = static_cast<decltype(globs)>(malloc(sizeof(char*)));
	if (globs == NULL)
	{
		return nullptr;
	}
	globs[0] = ::std::string(glob);
	num_globs = 1;

	va_start(args, glob);
	for (;;)
	{
		::std::string arg = va_arg(args, ::std::string);
		char** new_globs;

		if (arg == NULL)
		{
			break;
		}

		new_globs = realloc(globs, sizeof(char*) * (num_globs + 1));
		if (new_globs == NULL)
		{
			FreeStringList(globs, num_globs);
		}
		globs = new_globs;
		globs[num_globs] = ::std::string(arg);
		++num_globs;
	}
	va_end(args);

	result = static_cast<decltype(result)>(malloc(sizeof(glob_t)));
	if (result == NULL)
	{
		FreeStringList(globs, num_globs);
		return nullptr;
	}

	result->dir = opendir(directory);
	if (result->dir == NULL)
	{
		FreeStringList(globs, num_globs);
		free(result);
		return nullptr;
	}

	result->directory = ::std::string(directory);
	result->globs = globs;
	result->num_globs = num_globs;
	result->flags = flags;
	result->last_filename = NULL;
	result->filenames = NULL;
	result->filenames_len = 0;
	result->next_index = -1;
	return result;
}

// CALLED EXTERN
glob_t* I_StartGlob(::std::string directory, ::std::string glob, int flags)
{
	return I_StartMultiGlob(directory, flags, glob, NULL);
}

// CALLED EXTERN
void I_EndGlob(glob_t* glob)
{
	if (glob == NULL)
	{
		return;
	}

	FreeStringList(glob->globs, glob->num_globs);
	FreeStringList(glob->filenames, glob->filenames_len);

	free(glob->directory);
	free(glob->last_filename);
	() closedir(glob->dir);
	free(glob);
}

// CALLED EXTERN
::std::string NextGlob(glob_t* glob)
{
	dirent* de;

	do
	{
		de = readdir(glob->dir);
		if (de == NULL)
		{
			return nullptr;
		}
	} while (IsDirectory(glob->directory, de)
		|| !MatchesAnyGlob(de->d_name, glob));

	// Return the fully-qualified path, not just the bare filename.
	return ::std::string(glob->directory + DIR_SEPARATOR_S + de->d_name);
}

// CALLED EXTERN
::std::string I_NextGlob(glob_t* glob)
{
	::std::string result;

	if (glob == NULL)
	{
		return nullptr;
	}

	// In unsorted mode we just return the filenames as we read
	// them back from the system API.
	if ((glob->flags & GLOB_FLAG_SORTED) == 0)
	{
		free(glob->last_filename);
		glob->last_filename = NextGlob(glob);
		return glob->last_filename;
	}

	// In sorted mode we read the whole list of filenames into memory,
	// sort them and return them one at a time.
	if (glob->next_index < 0)
	{
		ReadAllFilenames(glob);
		SortFilenames(glob->filenames, glob->filenames_len, glob->flags);
	}
	if (glob->next_index >= glob->filenames_len)
	{
		return nullptr;
	}
	result = glob->filenames[glob->next_index];
	++glob->next_index;
	return result;
}

static bool MatchesGlob(::std::string name, ::std::string glob, int flags)
{
	int n;
	int g;

	while (*glob != '\0')
	{
		n = *name;
		g = *glob;

		if ((flags & GLOB_FLAG_NOCASE) != 0)
		{
			n = tolower(n);
			g = tolower(g);
		}

		if (g == '*')
		{
			// To handle *-matching we skip past the * and recurse
			// to check each subsequent character in turn. If none
			// match then the whole match is a failure.
			while (*name != '\0')
			{
				if (MatchesGlob(name, glob + 1, flags))
				{
					return true;
				}
				++name;
			}
			return glob[1] == '\0';
		}
		else if (g != '?' && n != g)
		{
			// For normal characters the name must match the glob,
			// but for ? we don't care what the character is.
			return false;
		}

		++name;
		++glob;
	}

	// Match successful when glob and name end at the same time.
	return *name == '\0';
}

static bool MatchesAnyGlob(::std::string name, glob_t* glob)
{
	int i;

	for (i = 0; i < glob->num_globs; ++i)
	{
		if (MatchesGlob(name, glob->globs[i], glob->flags))
		{
			return true;
		}
	}
	return false;
}

static void ReadAllFilenames(glob_t* glob)
{
	::std::string name;

	glob->filenames = NULL;
	glob->filenames_len = 0;
	glob->next_index = 0;

	for (;;)
	{
		name = NextGlob(glob);
		if (name == NULL)
		{
			break;
		}
		glob->filenames = realloc(glob->filenames, (glob->filenames_len + 1) * sizeof(char*));
		glob->filenames[glob->filenames_len] = name;
		++glob->filenames_len;
	}
}

static void SortFilenames(char** filenames, int len, int flags)
{
	::std::string pivot;
	::std::string tmp;
	int i;
	int left_len;
	int cmp;

	if (len <= 1)
	{
		return;
	}
	pivot = filenames[len - 1];
	left_len = 0;
	for (i = 0; i < len - 1; ++i)
	{
		if ((flags & GLOB_FLAG_NOCASE) != 0)
		{
			cmp = iequals(filenames[i], pivot);
		}
		else
		{
			cmp = strcmp(filenames[i], pivot);
		}

		if (cmp < 0)
		{
			tmp = filenames[i];
			filenames[i] = filenames[left_len];
			filenames[left_len] = tmp;
			++left_len;
		}
	}
	filenames[len - 1] = filenames[left_len];
	filenames[left_len] = pivot;

	SortFilenames(filenames, left_len, flags);
	SortFilenames(&filenames[left_len + 1], len - left_len - 1, flags);
}
