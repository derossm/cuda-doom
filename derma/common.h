/**********************************************************************************************************************************************\
	Copyright© 2020-2021 Mason DeRoss

	Released under the GNU All-permissive License

	Copying and distribution of this file, with or without modification, are permitted in any medium without royalty,
	provided the copyright notice and this notice are preserved. This file is offered as-is, without any warranty.

	DESCRIPTION:

\**********************************************************************************************************************************************/
#pragma once

#include "stdafx.h"

using CHAR_PTR = const char*;
using TimeType = uint64_t;

namespace cudadoom
{

using byte = ::std::byte;

// Times
//enum class Seconds : TimeType
//{
	//ATTO	= 1,			// attoseconds
	//FEMTO	= ATTO*1'000,	// femtoseconds
	//PICO	= FEMTO*1'000,	// picoseconds
	//NANO	= PICO*1'000,	// nanoseconds
	//MICRO	= NANO*1'000,	// microseconds
	//MILLI	= MICRO*1'000,	// milliseconds
	//SECOND	= MILLI*1'000,	// seconds
//};

bool iequals(const ::std::string& lhs, const ::std::string& rhs)
{
	return ::std::equal(lhs.begin(), lhs.end(), rhs.begin(),
		[](char l, char r)
		{
			return tolower(l) == tolower(r);
		});
}

} // end namespace cudadoom
