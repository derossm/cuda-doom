/**********************************************************************************************************************************************\
	Copyright(C) 1993-1996 Id Software, Inc.
	Copyright(C) 2005-2014 Simon Howard

	This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License
	as published by the Free Software Foundation; either version 2 of the License, or (at your option) any later version.

	This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

	DESCRIPTION:
		Main program, simply calls D_DoomMain high level loop.
\**********************************************************************************************************************************************/
#include "../derma/stdafx.h"

#include "crispy.h"
#include "i_system.h"
#include "m_argv.h"

// Not a globally visible function, just included for source reference, calls all startup code, parses command line options.
void D_DoomMain();

int main(int argc, char** argv)
{
	// save arguments
	myargc = argc;
	myargv = argv;

	// Print the program version and exit.
	if (M_ParmExists("-version") || M_ParmExists("--version"))
	{
		puts(PACKAGE_STRING);
		exit(0);
	}

	{
		crispy->sdlversion = []() {
			SDL_version version;
			SDL_GetVersion(&version);
			return ::std::string(version.major + "." + version.minor + "." + version.patch);
		}();
		crispy->platform = ::std::string(SDL_GetPlatform());
	}

#if defined(_WIN32)
	// compose a proper command line from loose file paths passed as arguments to allow for loading WADs and DEHACKED patches by drag-and-drop
	M_AddLooseFiles();
#endif

	M_FindResponseFile();

#ifdef SDL_HINT_NO_SIGNAL_HANDLERS
	SDL_SetHint(SDL_HINT_NO_SIGNAL_HANDLERS, "1");
#endif

	// start doom
	D_DoomMain();

	return 0;
}
