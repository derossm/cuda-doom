/**********************************************************************************************************************************************\
	Copyright(C) 2020 Mason DeRoss

	Released under the GNU All-permissive License

	Copying and distribution of this file, with or without modification, are permitted in any medium without royalty,
	provided the copyright notice and this notice are preserved. This file is offered as-is, without any warranty.

	DESCRIPTION:

\**********************************************************************************************************************************************/
#pragma once

#include <string>
#include <cctype>

using byte = uint8_t;

using TimeType = uint64_t;

using CHAR_PTR = const char*;

bool iequals(const std::string& lhs, const std::string& rhs)
{
	return std::equal(lhs.begin(), lhs.end(), rhs.begin(),
		[](char l, char r)
		{
			return tolower(l) == tolower(r);
		});
}
