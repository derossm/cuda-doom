/**********************************************************************************************************************************************\
	Copyright(C) 2020 Mason DeRoss

	Released under the GNU All-permissive License

	Copying and distribution of this file, with or without modification, are permitted in any medium without royalty,
	provided the copyright notice and this notice are preserved. This file is offered as-is, without any warranty.

	DESCRIPTION:

\**********************************************************************************************************************************************/
#pragma once

#include <cassert>

#include <cstdlib>
#include <cstdio>
#include <cctype>
#include <cstring>

#include <cinttypes>
#include <climits>
#include <cstdarg>

#include <cstddef>

#include <cerrno>

#include <cmath>

#include <ctime>
#include <cstdint>

#include <sys/types.h>

#if !defined(HAVE_LIBI386) && !defined(HAVE_LIBAMD64) && !defined(NO_OBSD_DRIVER)
	#define NO_OBSD_DRIVER
#endif
