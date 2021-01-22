/**********************************************************************************************************************************************\
	Copyright(C) 2005-2014 Simon Howard
	Copyright(C) 2014 Fabian Greffrath

	This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License
	as published by the Free Software Foundation; either version 2 of the License, or (at your option) any later version.

	This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

	DESCRIPTION:
		Parses INCLUDE directives in BEX files
\**********************************************************************************************************************************************/
#include "../../derma/stdafx.h"

#include "../m_misc.h"

#include "deh_io.h"
#include "deh_main.h"

static bool bex_nested = false;

static void DEH_BEXInclStart(deh_context_t* context, ::std::string line)
{
	extern bool bex_notext;

	if (DEH_FileName(context).empty())
	{
		DEH_Warning(context, "DEHACKED lumps may not include files");
		return;
	}

	::std::string deh_file = DEH_FileName(context);

	if (bex_nested)
	{
		DEH_Warning(context, "Included files may not include other files");
		return;
	}

	::std::string inc_file = static_cast<decltype(inc_file)>(malloc(strlen(line) + 1));

	if (sscanf(line, "INCLUDE NOTEXT %32s", inc_file) == 1)
	{
		bex_notext = true;
	}
	else if (sscanf(line, "INCLUDE %32s", inc_file) == 1)
	{
		// well, fine - VERY HELPFUL COMMENT TO BE REMOVED NEXT COMMIT TODO
	}
	else
	{
		DEH_Warning(context, "Parse error on section start");
		return;
	}

	// first, try loading the file right away
	::std::string try_path = inc_file;

	if (!M_FileExists(try_path))
	{
		// second, try loading the file in the directory of the current file
		::std::string dir = M_DirName(deh_file);
		try_path = ::std::string(dir + DIR_SEPARATOR_S + inc_file);
	}

	bex_nested = true;

	if (!M_FileExists(try_path) || !DEH_LoadFile(try_path))
	{
		DEH_Warning(context, "Could not include \"%s\"", inc_file);
	}

	bex_nested = false;
	bex_notext = false;

	return;
}

[[deprecated]]
static void DEH_BEXInclParseLine(deh_context_t* context, ::std::string line, void* tag)
{
	// not used
}

deh_section_t deh_section_bexincl =
{
	"INCLUDE",
	NULL,
	DEH_BEXInclStart,
	DEH_BEXInclParseLine,
	NULL,
	NULL,
};
