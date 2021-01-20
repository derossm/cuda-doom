/**********************************************************************************************************************************************\
	Copyright© 2020-2021 Mason DeRoss

	Released under the GNU All-permissive License

	Copying and distribution of this file, with or without modification, are permitted in any medium without royalty,
	provided the copyright notice and this notice are preserved. This file is offered as-is, without any warranty.

	DESCRIPTION:

\**********************************************************************************************************************************************/
#pragma once

#include "stdafx.h"

namespace cudadoom
{

	class PMRVector
	{
		static constexpr size_t _PMR_BUFFER_SIZE{8192};
		::std::array<::std::byte, _PMR_BUFFER_SIZE> stackBuf;
		::std::pmr::monotonic_buffer_resource rsrc;

		PMRVector() : rsrc(stackBuf.data(), sizeof stackBuf)
		{

		}
	};

}