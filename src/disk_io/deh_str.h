/**********************************************************************************************************************************************\
	Copyright(C) 2005-2014 Simon Howard

	This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License
	as published by the Free Software Foundation; either version 2 of the License, or (at your option) any later version.

	This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

	DESCRIPTION:
		Dehacked string replacements
\**********************************************************************************************************************************************/
#pragma once

#include "../../derma/common.h"

struct deh_substitution_t
{
	::std::string from_text;
	::std::string to_text;
};

enum class format_arg_t
{
	FORMAT_ARG_INVALID,
	FORMAT_ARG_INT,
	FORMAT_ARG_FLOAT,
	FORMAT_ARG_CHAR,
	FORMAT_ARG_STRING,
	FORMAT_ARG_PTR,
	FORMAT_ARG_SAVE_POS
};

// Used to do dehacked text substitutions throughout the program
::std::string DEH_String(::std::string s) PRINTF_ARG_ATTR(1);
void DEH_printf(::std::string fmt, ...) PRINTF_ATTR(1, 2);
void DEH_fprintf(FILE* fstream, ::std::string fmt, ...) PRINTF_ATTR(2, 3);
void DEH_snprintf(::std::string buffer, size_t len, ::std::string fmt, ...) PRINTF_ATTR(3, 4);
void DEH_AddStringReplacement(::std::string from_text, ::std::string to_text);
bool DEH_HasStringReplacement(::std::string s);

#if 0
// Static macro versions of the functions above
#define DEH_String(x) (x)
#define DEH_printf printf
#define DEH_fprintf fprintf
#define DEH_snprintf snprintf
#endif
