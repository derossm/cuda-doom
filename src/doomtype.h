/**********************************************************************************************************************************************\
	Copyright(C) 1993-1996 Id Software, Inc.
	Copyright(C) 2005-2014 Simon Howard

	This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License
	as published by the Free Software Foundation; either version 2 of the License, or (at your option) any later version.

	This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

	DESCRIPTION:
		Simple basic typedefs, isolated here to make it easier separating modules.
\**********************************************************************************************************************************************/
#pragma once

#include "../derma/common.h"

#include "config.h"

#ifndef CRISPY_TRUECOLOR
using pixel_t = uint8_t;
using dpixel_t = int16_t;
#else
using pixel_t = uint32_t;
using dpixel_t = int64_t;
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

#define arrlen(array) (sizeof(array) / sizeof(*array))

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
#endif

#ifdef __WATCOMC__
#define PACKEDPREFIX _Packed
#elif defined(_MSC_VER)
#define PACKEDPREFIX __pragma(pack(push,1))
#else
#define PACKEDPREFIX
#endif

#define PACKED_STRUCT(...) PACKEDPREFIX struct __VA_ARGS__ PACKEDATTR
