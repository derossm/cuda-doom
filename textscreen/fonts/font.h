/**********************************************************************************************************************************************\
	Copyright© 2020-2021 Mason DeRoss

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
	//none,
	//large,
	//normal,
	//small,
	//other
};

class FontBase
{
public:
	using pixel = int;

	const ::std::array<const char, 16> name;
	const pixel width;
	const pixel height;

	virtual ~FontBase() = default;
};

/*template<typename T>
	requires (!::std::is_same_v<T, decltype(large_font_data)>
			&& !::std::is_same_v<T, decltype(normal_font_data)>
			&& !::std::is_same_v<T, decltype(small_font_data)>)
class FontType : public FontBase
{
public:
	const T& data{nullptr};

	FontType(::std::string name, pixel width, pixel height) : name{name.c_str()}, width{width}, height{height}
	{
	}
};

template<typename T>
	requires ::std::same_as<T, decltype(large_font_data)>
class FontType : public FontBase
{
public:
	const T& data{::std::reference_wrapper(large_font_data)};

	FontType() : name{"large"}, width{16}, height{32}
	{
	}
};

template<typename T>
	requires ::std::same_as<T, decltype(normal_font_data)>
class FontType : public FontBase
{
public:
	const T& data{::std::reference_wrapper(normal_font_data)};

	FontType() : name{"normal"}, width{8}, height{16}
	{
	}
};

template<typename T>
	requires ::std::same_as<T, decltype(small_font_data)>
class FontType : public FontBase
{
public:
	const T& data{::std::reference_wrapper(small_font_data)};

	FontType() : name{"small"}, width{4}, height{8}
	{
	}
};*/

template<typename T>
class FontType : public FontBase
{
public:
	//const T& data{::std::remove_reference_t<v>};
	const T& data{v};
	FontType(T v, const char* name, pixel width, pixel height) : data{v}, name{name}, width{width}, height{height}
	{}

};

static constexpr FontType large_font{large_font_data, "large", 16, 32};
static constexpr FontType normal_font{normal_font_data, "normal", 8, 16};
static constexpr FontType small_font{small_font_data, "small", 4, 8};
