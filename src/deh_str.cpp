/**********************************************************************************************************************************************\
	Copyright(C) 2005-2014 Simon Howard

	This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License
	as published by the Free Software Foundation; either version 2 of the License, or (at your option) any later version.

	This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

	DESCRIPTION:
		Parses Text substitution sections in dehacked files
\**********************************************************************************************************************************************/

#include "doomtype.h"
#include "deh_str.h"
#include "m_misc.h"

#include "z_zone.h"

static deh_substitution_t** hash_table{nullptr};
static int hash_table_entries;
static int hash_table_length = -1;

// This is the algorithm used by glib
unsigned strhash(std::string s)
{
	auto p = s.c_str();
	unsigned h = *p;

	if (h)
	{
		for (p += 1; *p; ++p)
		{
			h = (h << 5) - h + *p;
		}
	}

	return h;
}

deh_substitution_t* SubstitutionForString(std::string s)
{
	// Fallback if we have not initialized the hash table yet
	if (hash_table_length < 0)
	{
		return nullptr;
	}

	auto entry = strhash(s) % hash_table_length;

	while (hash_table[entry])
	{
		if (!s.compare(hash_table[entry]->from_text))
		{
			// substitution found!
			return hash_table[entry];
		}

		entry = (entry + 1) % hash_table_length;
	}

	// no substitution found
	return nullptr;
}

// Look up a string to see if it has been replaced with something else, this will be used throughout the program to substitute text
std::string DEH_String(std::string s)
{
	deh_substitution_t* subst = SubstitutionForString(s);

	if (subst)
	{
		return subst->to_text;
	}
	else
	{
		return s;
	}
}

// [crispy] returns true if a string has been substituted
bool DEH_HasStringReplacement(std::string s)
{
	return DEH_String(s) != s;
}

void InitHashTable()
{
	// init hash table
	hash_table_entries = 0;
	hash_table_length = 16;
	hash_table = Z_Malloc<deh_substitution_t**>(sizeof(deh_substitution_t*) * hash_table_length, pu_tags_t::PU_STATIC, nullptr);
	memset(hash_table, 0, sizeof(deh_substitution_t*) * hash_table_length);
}

void DEH_AddToHashtable(deh_substitution_t* sub);

void IncreaseHashtable()
{
	// save the old table
	auto old_table = hash_table;
	auto old_table_length = hash_table_length;

	// double the size
	hash_table_length *= 2;
	hash_table = Z_Malloc<deh_substitution_t**>(sizeof(deh_substitution_t*) * hash_table_length, pu_tags_t::PU_STATIC, nullptr);
	memset(hash_table, 0, sizeof(deh_substitution_t*) * hash_table_length);

	// go through the old table and insert all the old entries
	for (auto i{0}; i < old_table_length; ++i)
	{
		if (old_table[i] != NULL)
		{
			DEH_AddToHashtable(old_table[i]);
		}
	}

	// free the old table
	Z_Free(old_table);
}

void DEH_AddToHashtable(deh_substitution_t* sub)
{
	// if the hash table is more than 60% full, increase its size
	if ((hash_table_entries * 10) / hash_table_length > 6)
	{
		IncreaseHashtable();
	}

	// find where to insert it
	size_t entry = strhash(sub->from_text) % hash_table_length;

	while (hash_table[entry] != NULL)
	{
		entry = (entry + 1) % hash_table_length;
	}

	hash_table[entry] = sub;
	++hash_table_entries;
}

void DEH_AddStringReplacement(std::string from_text, std::string to_text)
{
	deh_substitution_t* sub;
	size_t len;

	// Initialize the hash table if this is the first time
	if (hash_table_length < 0)
	{
		InitHashTable();
	}

	// Check to see if there is an existing substitution already in place.
	sub = SubstitutionForString(from_text);
	// FIXME
/*
	if (sub != nullptr)
	{
		Z_Free(sub->to_text);

		len = strlen(to_text) + 1;
		sub->to_text = Z_Malloc<decltype(sub->to_text)>(len, pu_tags_t::PU_STATIC, nullptr);
		memcpy(sub->to_text, to_text, len);
	}
	else
	{
		// We need to allocate a new substitution.
		sub = Z_Malloc<decltype(*sub)>(sizeof(*sub), pu_tags_t::PU_STATIC, 0);

		// We need to create our own duplicates of the provided strings.
		len = strlen(from_text) + 1;
		sub->from_text = Z_Malloc<decltype(sub->from_text)>(len, pu_tags_t::PU_STATIC, nullptr);
		memcpy(sub->from_text, from_text, len);

		len = strlen(to_text) + 1;
		sub->to_text = Z_Malloc<decltype(sub->to_text)>(len, pu_tags_t::PU_STATIC, NULL);
		memcpy(sub->to_text, to_text, len);

		DEH_AddToHashtable(sub);
	}
*/
}

// Get the type of a format argument. We can mix-and-match different format arguments as long as they are for the same data type.
format_arg_t FormatArgumentType(char c)
{
	switch (c)
	{
		case 'd': case 'i': case 'o': case 'u': case 'x': case 'X':
			return format_arg_t::FORMAT_ARG_INT;

		case 'e': case 'E': case 'f': case 'F': case 'g': case 'G':
		case 'a': case 'A':
			return format_arg_t::FORMAT_ARG_FLOAT;

		case 'c': case 'C':
			return format_arg_t::FORMAT_ARG_CHAR;

		case 's': case 'S':
			return format_arg_t::FORMAT_ARG_STRING;

		case 'p':
			return format_arg_t::FORMAT_ARG_PTR;

		case 'n':
			return format_arg_t::FORMAT_ARG_SAVE_POS;

		default:
			return format_arg_t::FORMAT_ARG_INVALID;
	}
}

// Given the specified string, get the type of the first format string encountered.
format_arg_t NextFormatArgument(std::string str)
{
	format_arg_t argtype;
	// FIXME
/* 
	// Search for the '%' starting the next string.
	while (**str != '\0')
	{
		if (**str == '%')
		{
			//++*str;
			++(*str);

			// Don't stop for double-%s.
			if (**str != '%')
			{
				break;
			}
		}

		//++*str;
		++(*str);
	}

	// Find the type of the format string.
	while (**str != '\0')
	{
		argtype = FormatArgumentType(**str);

		if (argtype != format_arg_t::FORMAT_ARG_INVALID)
		{
			//++*str;
			++(*str);

			return argtype;
		}

		//++*str;
		++(*str);
	}

	// Stop searching, we have reached the end.
	*str = nullptr;
 */
	return format_arg_t::FORMAT_ARG_INVALID;
}

// Check if the specified argument type is a valid replacement for the original.
bool ValidArgumentReplacement(format_arg_t original, format_arg_t replacement)
{
	// In general, the original and replacement types should be identical.
	// However, there are some cases where the replacement is valid and the types don't match.

	// Characters can be represented as ints.
	if (original == format_arg_t::FORMAT_ARG_CHAR && replacement == format_arg_t::FORMAT_ARG_INT)
	{
		return true;
	}

	// Strings are pointers.
	if (original == format_arg_t::FORMAT_ARG_STRING && replacement == format_arg_t::FORMAT_ARG_PTR)
	{
		return true;
	}

	return original == replacement;
}

// Return true if the specified string contains no format arguments.
bool ValidFormatReplacement(std::string original, std::string replacement)
{
	// Check each argument in turn and compare types.
	//auto rover1 = original;
	//auto rover2 = replacement;

	for (;;)
	{
		auto argtype1 = NextFormatArgument(original);
		auto argtype2 = NextFormatArgument(replacement);

		if (argtype2 == format_arg_t::FORMAT_ARG_INVALID)
		{
			// No more arguments left to read from the replacement string.
			break;
		}
		else if (argtype1 == format_arg_t::FORMAT_ARG_INVALID)
		{
			// Replacement string has more arguments than the original.
			return false;
		}
		else if (!ValidArgumentReplacement(argtype1, argtype2))
		{
			// Not a valid replacement argument.
			return false;
		}
	}

	return true;
}

// Get replacement format string, checking arguments.
std::string FormatStringReplacement(std::string s)
{
	auto repl = DEH_String(s);

	if (!ValidFormatReplacement(s, repl))
	{
		printf("WARNING: Unsafe dehacked replacement provided for printf format string: %s\n", s.c_str());

		return s;
	}

	return repl;
}

// printf(), performing a replacement on the format string.
void DEH_printf(std::string fmt, ...)
{
	va_list args;
	std::string repl;

	repl = FormatStringReplacement(fmt);

	va_start(args, fmt);

	vprintf(repl.c_str(), args);

	va_end(args);
}

// fprintf(), performing a replacement on the format string.
void DEH_fprintf(FILE* fstream, std::string fmt, ...)
{
	va_list args;
	std::string repl;

	repl = FormatStringReplacement(fmt);

	va_start(args, fmt);

	vfprintf(fstream, repl.c_str(), args);

	va_end(args);
}

// snprintf(), performing a replacement on the format string.
void DEH_snprintf(std::string buffer, size_t len, std::string fmt, ...)
{
	va_list args;
	std::string repl;

	repl = FormatStringReplacement(fmt);

	va_start(args, fmt);

	// FIXME
	//M_vsnprintf(buffer, len, repl, args);

	va_end(args);
}
