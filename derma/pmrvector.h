/**********************************************************************************************************************************************\
	Copyright© 2020-2021 Mason DeRoss

	Released under the GNU All-permissive License

	Copying and distribution of this file, with or without modification, are permitted in any medium without royalty,
	provided the copyright notice and this notice are preserved. This file is offered as-is, without any warranty.

	DESCRIPTION:

\**********************************************************************************************************************************************/
#pragma once

#include "common.h"

namespace cudadoom
{

	class PMRVector
	{
		::std::array<::std::byte, 8192> stackBuf;
		::std::pmr::monotonic_buffer_resource rsrc;

		PMRVector() : rsrc(stackBuf.data(), sizeof stackBuf)
		{

		}
	};

}