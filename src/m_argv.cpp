/**********************************************************************************************************************************************\
	Copyright(C) 1993-1996 Id Software, Inc.
	Copyright(C) 2005-2014 Simon Howard

	This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License
	as published by the Free Software Foundation; either version 2 of the License, or (at your option) any later version.

	This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
\**********************************************************************************************************************************************/

#include <string>

#include "SDL_stdinc.h"

#include "doomtype.h"

#include "d_iwad.h"
#include "i_system.h"
#include "m_misc.h"
#include "m_argv.h" // haleyjd 20110212: warning fix

int myargc;
char** myargv;

// Checks for the given parameter in the program's command line arguments.
// Returns the argument number (1 to argc-1) or 0 if not present
int M_CheckParmWithArgs(::std::string check, int num_args)
{
	for (size_t i{1}; i < myargc - num_args; ++i)
	{
		if (!iequals(check, myargv[i]))
		{
			return i;
		}
	}

	return 0;
}

// Returns true if the given parameter exists in the program's command line arguments, false if not.
bool M_ParmExists(::std::string check)
{
	return M_CheckParm(check) != 0;
}

int M_CheckParm(::std::string check)
{
	return M_CheckParmWithArgs(check, 0);
}

constexpr size_t MAXARGVS{100};

static void LoadResponseFile(int argv_index, ::std::string filename)
{
	// Read the response file into memory
	FILE* handle = fopen(filename, "rb");

	if (handle == NULL)
	{
		printf ("\nNo such response file!");
		exit(1);
	}

	printf("Found response file %s!\n", filename);

	auto size = M_FileLength(handle);

	// Read in the entire file
	// Allocate one byte extra - this is in case there is an argument at the end of the response file, in which case a '\0' will be needed.
	::std::string file = static_cast<decltype(file)>(malloc(size + 1));

	for (size_t i{0}, k; i < size; i += k)
	{
		k = fread(file + i, 1, size - i, handle);		
	}

	fclose(handle);

	// Create new arguments list array
	char** newargv = static_cast<decltype(newargv)>(malloc(sizeof(char*) * MAXARGVS));
	int newargc = 0;
	memset(newargv, 0, sizeof(char*) * MAXARGVS);

	// Copy all the arguments in the list up to the response file
	for (size_t i{0}; i < argv_index; ++i)
	{
		newargv[i] = myargv[i];
		++newargc;
	}

	::std::string infile = file;
	for(size_t k{0}; k < size;)
	{
		// Skip past space characters to the next argument
		while(k < size && isspace(infile[k]))
		{
			++k;
		}

		if (k >= size)
		{
			break;
		}

		// If the next argument is enclosed in quote marks, treat the contents as a single argument. This allows long filenames to be specified.
		if (infile[k] == '\"')
		{
			// Skip the first character(")
			++k;

			newargv[newargc++] = &infile[k];

			// Read all characters between quotes
			while (k < size && infile[k] != '\"' && infile[k] != '\n')
			{
				++k;
			}

			if (k >= size || infile[k] == '\n')
			{
				I_Error("Quotes unclosed in response file '%s'", filename);
			}

			// Cut off the string at the closing quote
			infile[k] = '\0';
			++k;
		}
		else
		{
			// Read in the next argument until a space is reached
			newargv[newargc++] = &infile[k];

			while(k < size && !isspace(infile[k]))
			{
				++k;
			}

			// Cut off the end of the argument at the first space

			infile[k] = '\0';

			++k;
		}
	}

	// Add arguments following the response file argument
	for (size_t i{argv_index + 1}; i < myargc; ++i)
	{
		newargv[newargc] = myargv[i];
		++newargc;
	}

	myargv = newargv;
	myargc = newargc;

#if 0
	// Disabled - Vanilla Doom does not do this. Display arguments
	printf("%d command-line args:\n", myargc);

	for (size_t k{1}; k < myargc; ++k)
	{
		printf("'%s'\n", myargv[k]);
	}
#endif
}

void M_FindResponseFile()
{
	for (size_t i{1}; i < myargc; ++i)
	{
		if (myargv[i][0] == '@')
		{
			LoadResponseFile(i, myargv[i] + 1);
		}
	}

	for (;;)
	{
		// Load extra command line arguments from the given response file.
		// Arguments read from the file will be inserted into the command
		// line replacing this argument. A response file can also be loaded
		// using the abbreviated syntax '@filename.rsp'.
		auto i = M_CheckParmWithArgs("-response", 1);
		if (i <= 0)
		{
			break;
		}
		// Replace the -response argument so that the next time through
		// the loop we'll ignore it. Since some parameters stop reading when
		// an argument beginning with a '-' is encountered, we keep something
		// that starts with a '-'.
		myargv[i] = "-_";
		LoadResponseFile(i + 1, myargv[i + 1]);
	}
}

#if defined(_WIN32)
enum
{
	FILETYPE_UNKNOWN = 0x0,
	FILETYPE_IWAD = 0x2,
	FILETYPE_PWAD = 0x4,
	FILETYPE_DEH = 0x8,
};

static int GuessFileType(::std::string name)
{
	int ret = FILETYPE_UNKNOWN;

	::std::string base = M_BaseName(name);
	::std::string lower = ::std::string(base);
	M_ForceLowercase(lower);

	static bool iwad_found{false};
	// only ever add one argument to the -iwad parameter
	if (iwad_found == false && D_IsIWADName(lower))
	{
		ret = FILETYPE_IWAD;
		iwad_found = true;
	}
	else if (M_StringEndsWith(lower, ".wad") || M_StringEndsWith(lower, ".lmp"))
	{
		ret = FILETYPE_PWAD;
	}
	else if (M_StringEndsWith(lower, ".deh") || M_StringEndsWith(lower, ".bex") || M_StringEndsWith(lower, ".hhe") || M_StringEndsWith(lower, ".seh"))
	{
		ret = FILETYPE_DEH;
	}

	return ret;
}

struct argument_t
{
	::std::string str;
	int type;
	int stable;
};

static int CompareByFileType(const void* a, const void* b)
{
	const argument_t* arg_a = (const argument_t*) a;
	const argument_t* arg_b = (const argument_t*) b;

	const int ret = arg_a->type - arg_b->type;

	return ret ? ret : (arg_a->stable - arg_b->stable);
}

void M_AddLooseFiles()
{
	if (myargc < 2)
	{
		return;
	}

	// allocate space for up to three additional regular parameters
	argument_t* arguments = static_cast<decltype(arguments)>(malloc((myargc + 3) * sizeof(*arguments)));
	memset(arguments, 0, (myargc + 3) * sizeof(*arguments));

	int types{0};
	// check the command line and make sure it does not already contain any regular parameters or response files
	// but only fully-qualified LFS or UNC file paths
	for (size_t i{1}; i < myargc; ++i)
	{
		::std::string arg = myargv[i];

		if (strlen(arg) < 3 || arg[0] == '-' || arg[0] == '@' || ((!isalpha(arg[0]) || arg[1] != ':' || arg[2] != '\\')
			&& (arg[0] != '\\' || arg[1] != '\\')))
		{
			free(arguments);
			return;
		}

		auto type = GuessFileType(arg);
		arguments[i].str = arg;
		arguments[i].type = type;
		arguments[i].stable = i;
		types |= type;
	}

	// add space for one additional regular parameter for each discovered file type in the new argument list
	// and sort parameters right before their corresponding file paths
	if (types & FILETYPE_IWAD)
	{
		arguments[myargc].str = ::std::string("-iwad");
		arguments[myargc].type = FILETYPE_IWAD - 1;
		++myargc;
	}
	if (types & FILETYPE_PWAD)
	{
		arguments[myargc].str = ::std::string("-merge");
		arguments[myargc].type = FILETYPE_PWAD - 1;
		++myargc;
	}
	if (types & FILETYPE_DEH)
	{
		arguments[myargc].str = ::std::string("-deh");
		arguments[myargc].type = FILETYPE_DEH - 1;
		++myargc;
	}

	char** newargv = static_cast<decltype(newargv)>(malloc(myargc * sizeof(*newargv)));

	// sort the argument list by file type, except for the zeroth argument which is the executable invocation itself
	SDL_qsort(arguments + 1, myargc - 1, sizeof(*arguments), CompareByFileType);

	newargv[0] = myargv[0];

	for (size_t i{1}; i < myargc; ++i)
	{
		newargv[i] = arguments[i].str;
	}

	free(arguments);

	myargv = newargv;
}
#endif

// Return the name of the executable used to start the program:
::std::string M_GetExecutableName()
{
	return M_BaseName(myargv[0]);
}
