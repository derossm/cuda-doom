/**********************************************************************************************************************************************\
	Copyright(C) 2005-2014 Simon Howard

	This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License
	as published by the Free Software Foundation; either version 2 of the License, or (at your option) any later version.

	This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

	DESCRIPTION:
		Dehacked I/O code (does all reads from dehacked files)
\**********************************************************************************************************************************************/
#pragma once

#include "../derma/stdafx.h"

//#include <string>

#include "../derma/common.h"

#include "deh_defs.h"

deh_context_t* DEH_OpenFile(::std::string filename);
deh_context_t* DEH_OpenLump(int lumpnum);
void DEH_CloseFile(deh_context_t* context);
int DEH_GetChar(deh_context_t* context);
::std::string DEH_ReadLine(deh_context_t* context, bool extended);
void DEH_Error(deh_context_t* context, ::std::string msg, ...) PRINTF_ATTR(2, 3);
void DEH_Warning(deh_context_t* context, ::std::string msg, ...) PRINTF_ATTR(2, 3);
bool DEH_HadError(deh_context_t* context);
::std::string DEH_FileName(deh_context_t* context);
