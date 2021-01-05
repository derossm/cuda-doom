/**********************************************************************************************************************************************\
	Copyright(C) 2005-2014 Simon Howard

	This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License
	as published by the Free Software Foundation; either version 2 of the License, or (at your option) any later version.

	This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

	DESCRIPTION:
		Dehacked I/O code (does all reads from dehacked files)
\**********************************************************************************************************************************************/

#include "deh_io.h"

#include "deh_defs.h"

#include "m_misc.h"
#include "w_wad.h"
#include "z_zone.h"

auto DEH_NewContext()
{
	auto context{std::make_unique<deh_context_t>()};
	return context;
}

// Open a dehacked file for reading; Returns NULL if open failed
auto DEH_OpenFile(std::string filename)
{
	auto fstream{fopen(filename.c_str(), "r")};

	if (!fstream)
	{
		return std::unique_ptr<deh_context_t>(nullptr);
	}

	auto context{DEH_NewContext()};

	context->type = deh_input_type_t::DEH_INPUT_FILE;
	context->stream = fstream;
	context->filename = std::string(filename);

	return context;
}

// Open a WAD lump for reading.
auto DEH_OpenLump(int lumpnum)
{
	auto lump = W_CacheLumpNum<char>(lumpnum, pu_tags_t::PU_STATIC);

	auto context = DEH_NewContext();

	context->type = deh_input_type_t::DEH_INPUT_LUMP;
	context->lumpnum = lumpnum;
	context->input_buffer = std::string((char*)lump);
	context->input_buffer_pos = 0;

	context->filename = std::string(lumpinfo[lumpnum]->name);

	return context;
}

// Close dehacked file
void DEH_CloseFile(deh_context_t* context)
{
	if (context->type == deh_input_type_t::DEH_INPUT_FILE)
	{
		fclose(context->stream);
	}
	else if (context->type == deh_input_type_t::DEH_INPUT_LUMP)
	{
		W_ReleaseLumpNum(context->lumpnum);
	}
}

int DEH_GetCharFile(deh_context_t* context)
{
	if (feof(context->stream))
	{
		// end of file
		return -1;
	}

	return fgetc(context->stream);
}

int DEH_GetCharLump(deh_context_t* context)
{
	int result;

	if (context->input_buffer_pos >= context->input_buffer.size())
	{
		return -1;
	}

	result = context->input_buffer[context->input_buffer_pos];
	++context->input_buffer_pos;

	return result;
}

// Reads a single character from a dehacked file
int DEH_GetChar(deh_context_t* context)
{
	int result = 0;

	// Read characters, but ignore carriage returns; Essentially this is a DOS->Unix conversion
	do
	{
		switch (context->type)
		{
		case deh_input_type_t::DEH_INPUT_FILE:
			result = DEH_GetCharFile(context);
			break;

		case deh_input_type_t::DEH_INPUT_LUMP:
			result = DEH_GetCharLump(context);
			break;
		}
	} while (result == '\r');

	// Track the current line number
	if (context->last_was_newline)
	{
		++context->linenum;
	}

	context->last_was_newline = result == '\n';

	return result;
}

// Increase the read buffer size
static void IncreaseReadBuffer(deh_context_t* context)
{}

// Save pointer to start of current line ...
void DEH_SaveLineStart(deh_context_t* context)
{
	if (context->type == deh_input_type_t::DEH_INPUT_FILE)
	{
		context->linestart = ftell(context->stream);
	}
	else if (context->type == deh_input_type_t::DEH_INPUT_LUMP)
	{
		context->linestart = context->input_buffer_pos;
	}
}

// ... and reset context to start of current line to retry with previous line parser in case of a parsing error
void DEH_RestoreLineStart(deh_context_t* context)
{
	// never point past the start
	if (context->linestart < 0)
	{
		return;
	}

	if (context->type == deh_input_type_t::DEH_INPUT_FILE)
	{
		fseek(context->stream, context->linestart, SEEK_SET);
	}
	else if (context->type == deh_input_type_t::DEH_INPUT_LUMP)
	{
		context->input_buffer_pos = context->linestart;
	}

	// don't count this line twice
	--(context->linenum);
}

// Read a whole line
std::string DEH_ReadLine(deh_context_t* context, bool extended)
{
	bool escaped{false};

	for (size_t pos{0};;)
	{
		auto c{DEH_GetChar(context)};

		if (c < 0 && pos == 0)
		{
			// end of file
			return std::string{};
		}

		// cope with lines of any length: increase the buffer size
		if (pos >= context->readbuffer.size())
		{
			IncreaseReadBuffer(context);
			// FIXME
		}

		// extended string support
		if (extended && c == '\\')
		{
			c = DEH_GetChar(context);

			// "\n" in the middle of a string indicates an internal linefeed
			if (c == 'n')
			{
				context->readbuffer[pos] = '\n';
				++pos;
				continue;
			}

			// values to be assigned may be split onto multiple lines by ending each line that is to be continued with a backslash
			if (c == '\n')
			{
				escaped = true;
				continue;
			}
		}

		// blanks before the backslash are included in the string but indentation after the linefeed is not
		if (escaped && c >= 0 && isspace(c) && c != '\n')
		{
			continue;
		}
		else
		{
			escaped = false;
		}

		if (c == '\n' || c < 0)
		{
			// end of line: a full line has been read
			context->readbuffer[pos] = '\0';
			break;
		}
		else if (c != '\0')
		{
			// normal character; don't allow NUL characters to be added.
			context->readbuffer[pos] = (char)c;
			++pos;
		}
	}

	return context->readbuffer;
}

void DEH_Warning(deh_context_t* context, std::string msg, ...)
{
	va_list args;

	va_start(args, msg.c_str());

	fprintf(stderr, "%s:%i: warning: ", context->filename.c_str(), context->linenum);
	vfprintf(stderr, msg.c_str(), args);
	fprintf(stderr, "\n");

	va_end(args);
}

void DEH_Error(deh_context_t* context, std::string msg, ...)
{
	va_list args;

	va_start(args, msg.c_str());

	fprintf(stderr, "%s:%i: ", context->filename.c_str(), context->linenum);
	vfprintf(stderr, msg.c_str(), args);
	fprintf(stderr, "\n");

	va_end(args);

	context->had_error = true;
}

bool DEH_HadError(deh_context_t* context)
{
	return context->had_error;
}

// return the filename of the DEHACKED file or NULL if it is a DEHACKED lump loaded from a PWAD
std::string DEH_FileName(deh_context_t* context)
{
	if (context->type == deh_input_type_t::DEH_INPUT_FILE)
	{
		return context->filename;
	}

	return std::string{};
}
