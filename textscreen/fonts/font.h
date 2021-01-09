/**********************************************************************************************************************************************\
	Copyright(C) 2020 Mason DeRoss

	Released under the GNU All-permissive License

	Copying and distribution of this file, with or without modification, are permitted in any medium without royalty,
	provided the copyright notice and this notice are preserved. This file is offered as-is, without any warranty.

	DESCRIPTION:

\**********************************************************************************************************************************************/
#pragma once

#include <array>
#include <type_traits>
#include <concepts>

#include "large.h"
#include "normal.h"
#include "small.h"

enum class FontData
{
	none,
	large,
	normal,
	small,
	other
};

template<typename T, T v>
	requires std::is_array_v<T>
struct FontType
{
	using pixel = int;

	std::array<const char, 16> name;
	const pixel width;
	const pixel height;
	const T& data{std::reference_wrapper(v)};
};

static constexpr FontType<decltype(large_font_data), large_font_data> large_font{		.name{"large"},		.width{16},	.height{32} };
static constexpr FontType<decltype(normal_font_data), normal_font_data> normal_font{	.name{"normal"},	.width{8},	.height{16} };
static constexpr FontType<decltype(small_font_data), small_font_data> small_font{		.name{"small"},		.width{4},	.height{8} };
