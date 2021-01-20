
/**********************************************************************************************************************************************\
	Copyright© 2020-2021 Mason DeRoss

	Released under the GNU All-permissive License

	Copying and distribution of this file, with or without modification, are permitted in any medium without royalty,
	provided the copyright notice and this notice are preserved. This file is offered as-is, without any warranty.

	DESCRIPTION:

\**********************************************************************************************************************************************/
#pragma once

#ifdef _WIN32
#include "os_windows.h"
#else
#include "os_unix.h"
#endif

#ifdef _WIN32
	#define DIR_SEPARATOR '\\'
	#define DIR_SEPARATOR_S "\\"
	#define PATH_SEPARATOR ';'
#else
	#define DIR_SEPARATOR '/'
	#define DIR_SEPARATOR_S "/"
	#define PATH_SEPARATOR ':'
#endif

// The packed attribute forces structures to be packed into the minimum space necessary. If this is not done, the compiler may align structure
// fields differently to optimize memory access, inflating the overall structure size. It is important to use the packed attribute on certain
// structures where alignment is important, particularly data read/written to disk.
#ifdef __GNUC__
	#if defined(_WIN32) && !defined(__clang__)
		#define PACKEDATTR __attribute__((packed,gcc_struct))
	#else
		#define PACKEDATTR __attribute__((packed))
	#endif

	#define PRINTF_ATTR(fmt, first) __attribute__((format(printf, fmt, first)))
	#define PRINTF_ARG_ATTR(x) __attribute__((format_arg(x)))
	#define NORETURN __attribute__((noreturn))
#else
	#if defined(_MSC_VER)
		#define PACKEDATTR __pragma(pack(pop))
	#else
		#define PACKEDATTR
	#endif

	#define PRINTF_ATTR(fmt, first)
	#define PRINTF_ARG_ATTR(x)
	#define NORETURN
#endif // __GNUC__

#ifdef _MSC_VER
	#define PACKEDPREFIX __pragma(pack(push,1))
#else
	#define PACKEDPREFIX
#endif

#define PACKED_STRUCT(...) PACKEDPREFIX struct __VA_ARGS__ PACKEDATTR
