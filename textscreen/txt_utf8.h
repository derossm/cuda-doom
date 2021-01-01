/**********************************************************************************************************************************************\
	Copyright(C) 2005-2014 Simon Howard

	This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License
	as published by the Free Software Foundation; either version 2 of the License, or (at your option) any later version.

	This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
\**********************************************************************************************************************************************/
#pragma once

#include "../derma/common.h"

namespace cudadoom::txt
{

// Encode a Unicode character as UTF-8, storing it in the buffer 'p' and returning the new, incremented position.
char* EncodeUTF8(char* p, unsigned c)
{
	if (c < 0x80)								// 1 character (ASCII):
	{
		p[0] = c;
		return p + 1;
	}
	else if (c < 0x800)							// 2 character:
	{
		p[0] = 0xc0 | (c >> 6);
		p[1] = 0x80 | (c & 0x3f);
		return p + 2;
	}
	else if (c < 0x10000)						// 3 chacater:
	{
		p[0] = 0xe0 | (c >> 12);
		p[1] = 0x80 | ((c >> 6) & 0x3f);
		p[2] = 0x80 | (c & 0x3f);
		return p + 3;
	}
	else if (c < 0x200000)						// 4 character:
	{
		p[0] = 0xf0 | (c >> 18);
		p[1] = 0x80 | ((c >> 12) & 0x3f);
		p[2] = 0x80 | ((c >> 6) & 0x3f);
		p[3] = 0x80 | (c & 0x3f);
		return p + 4;
	}
	else
	{
		// Too big!
		return p;
	}
}

// Decode UTF-8 character, incrementing *ptr over the decoded bytes.
unsigned DecodeUTF8(const char** ptr)
{
	unsigned c;

	// UTF-8 decode.
	if (const char* p{*ptr};
				(*p & 0x80) == 0)				// 1 character (ASCII):
	{
		c = *p;
		*ptr += 1;
	}
	else if ((p[0] & 0xe0) == 0xc0
			&& (p[1] & 0xc0) == 0x80)			// 2 character:
	{
		c = ((p[0] & 0x1f) << 6)
			| (p[1] & 0x3f);
		*ptr += 2;
	}
	else if ((p[0] & 0xf0) == 0xe0
			&& (p[1] & 0xc0) == 0x80
			&& (p[2] & 0xc0) == 0x80)			// 3 character:
	{
		c = ((p[0] & 0x0f) << 12)
			| ((p[1] & 0x3f) << 6)
			| (p[2] & 0x3f);
		*ptr += 3;
	}
	else if ((p[0] & 0xf8) == 0xf0
			&& (p[1] & 0xc0) == 0x80
			&& (p[2] & 0xc0) == 0x80
			&& (p[3] & 0xc0) == 0x80)			// 4 character:
	{
		c = ((p[0] & 0x07) << 18)
			| ((p[1] & 0x3f) << 12)
			| ((p[2] & 0x3f) << 6)
			| (p[3] & 0x3f);
		*ptr += 4;
	}
	else
	{
		// Decode failure. Don't bother with 5/6 byte sequences.
		c = 0;
	}

	return c;
}

// Count the number of characters in a UTF-8 string.
unsigned UTF8_Strlen(const char* s)
{
	unsigned result{0};

	for (const char* p{s}; *p != '\0';)
	{
		unsigned c{DecodeUTF8(&p)};

		if (c == 0)
		{
			break;
		}

		++result;
	}

	return result;
}

// Skip past the first n characters in a UTF-8 string.
char* UTF8_SkipChars(const char* s, unsigned n)
{
	const char* p{s};

	for (size_t i{0}; i < n; ++i)
	{
		if (DecodeUTF8(&p) == 0)
		{
			break;
		}
	}

	return (char*)p;
}

} /* END NAMESPACE cudadoom::txt */
