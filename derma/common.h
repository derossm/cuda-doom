/**********************************************************************************************************************************************\
	Copyright(C) 2020 Mason DeRoss

	Released under the GNU All-permissive License

	Copying and distribution of this file, with or without modification, are permitted in any medium without royalty,
	provided the copyright notice and this notice are preserved. This file is offered as-is, without any warranty.

	DESCRIPTION:

\**********************************************************************************************************************************************/
#pragma once

#include <limits>

#include <memory>

#include <string>
#include <string_view>
#include <charconv>

#include <vector>
#include <array>

#include <variant>
#include <any>

#include <algorithm>
#include <numeric>

#include <cassert>

class DepType;

namespace CUDADOOM
{
	namespace TXT
	{
		template<typename T>
		class InputBox;
	}
}

using txt_inputbox_t = CUDADOOM::TXT::InputBox<DepType>;

#include <cstdlib>
#include <cstdio>
#include <cctype>
#include <cstring>

#include <cinttypes>
#include <climits>
#include <cstdarg>

using byte = uint8_t;
