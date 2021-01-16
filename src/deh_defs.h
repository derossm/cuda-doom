/**********************************************************************************************************************************************\
	Copyright(C) 2005-2014 Simon Howard

	This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License
	as published by the Free Software Foundation; either version 2 of the License, or (at your option) any later version.

	This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

	DESCRIPTION:
		Definitions for use in the dehacked code
\**********************************************************************************************************************************************/
#pragma once

#include "../derma/stdafx.h"

//#include <string>

#include "../derma/common.h"

#include "sha1.h"

enum class deh_input_type_t
{
	DEH_INPUT_FILE,
	DEH_INPUT_LUMP
};

struct deh_context_t
{
	// If the input comes from a file, the file stream for reading data.
	FILE* stream{nullptr};

	// pointer to start of current line
	long linestart{-1};

	unsigned input_buffer_pos;

	int lumpnum;

	// Current line number that we have reached:
	int linenum{0};

	::std::string filename;
	::std::string input_buffer;
	::std::string readbuffer;

	// Used by DEH_ReadLine:
	bool last_was_newline{true};

	// Error handling.
	bool had_error{false};

	deh_input_type_t type;
};

typedef void (*deh_section_init_t)();
typedef void* (*deh_section_start_t)(deh_context_t* context, ::std::string line);
typedef void (*deh_section_end_t)(deh_context_t* context, void* tag);
typedef void (*deh_line_parser_t)(deh_context_t* context, ::std::string line, void* tag);
typedef void (*deh_sha1_hash_t)(sha1_context_t* context);

struct deh_section_t
{
	::std::string name;

	// Called on startup to initialize code
	deh_section_init_t init;

	// This is called when a new section is started. The pointer returned is used as a tag for the following calls.
	deh_section_start_t start;

	// This is called for each line in the section
	[[deprecated]]
	deh_line_parser_t line_parser;

	// This is called at the end of the section for any cleanup
	deh_section_end_t end;

	// Called when generating an SHA1 sum of the dehacked state
	deh_sha1_hash_t sha1_hash;
};
