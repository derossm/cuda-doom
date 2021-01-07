/**********************************************************************************************************************************************\
	Copyright(C) 2005-2014 Simon Howard

	This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License
	as published by the Free Software Foundation; either version 2 of the License, or (at your option) any later version.

	This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
\**********************************************************************************************************************************************/
#pragma once
// DECOUPLE
#include "../derma/common.h"
//////////

namespace cudadoom::txt
{

// Encode a Unicode character as UTF-8, storing it in the buffer 'p' and returning the new, incremented position.
std::string EncodeUTF8(std::string str, unsigned c)
{
	return std::string{};
}

// Decode UTF-8 character, incrementing *ptr over the decoded bytes.
unsigned DecodeUTF8(CHAR_PTR ptr)
{
	return 0;
}

// Count the number of characters in a UTF-8 string.
unsigned UTF8_Strlen(std::string str)
{
	return 0;
}

// Skip past the first n characters in a UTF-8 string.
std::string UTF8_SkipChars(std::string str, unsigned n)
{
	return std::string{};
}

} // END NAMESPACE cudadoom::txt
