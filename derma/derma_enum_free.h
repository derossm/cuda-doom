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

template<typename T, typename U>
	requires ((std::is_enum_v<std::remove_reference_t<T>> || std::integral<std::remove_reference_t<T>>)
			&& (std::is_enum_v<std::remove_reference_t<U>> || std::integral<std::remove_reference_t<U>>))
T& operator|(const T& lhs, const U& rhs)
{
	return static_cast<T>(static_cast<size_t>(lhs) | static_cast<size_t>(rhs));
}

template<typename T, typename U>
	requires ((std::is_enum_v<std::remove_reference_t<T>> || std::integral<std::remove_reference_t<T>>)
			&& (std::is_enum_v<std::remove_reference_t<U>> || std::integral<std::remove_reference_t<U>>))
T& operator|(T&& lhs, U&& rhs)
{
	return static_cast<T>(static_cast<size_t>(lhs) | static_cast<size_t>(rhs));
}

template<typename T, typename U>
	requires ((std::is_enum_v<std::remove_reference_t<T>> || std::integral<std::remove_reference_t<T>>)
			&& (std::is_enum_v<std::remove_reference_t<U>> || std::integral<std::remove_reference_t<U>>))
T& operator&(const T& lhs, const U& rhs)
{
	return static_cast<T>(static_cast<size_t>(lhs) & static_cast<size_t>(rhs));
}

template<typename T, typename U>
	requires ((std::is_enum_v<std::remove_reference_t<T>> || std::integral<std::remove_reference_t<T>>)
			&& (std::is_enum_v<std::remove_reference_t<U>> || std::integral<std::remove_reference_t<U>>))
T& operator&(T&& lhs, U&& rhs)
{
	return static_cast<T>(static_cast<size_t>(lhs) & static_cast<size_t>(rhs));
}

template<typename T, typename U>
	requires ((std::is_enum_v<T> || std::integral<T>)
			&& (std::is_enum_v<std::remove_const_t<U>> || std::integral<std::remove_const_t<U>>))
T& operator^(const T& lhs, const U& rhs)
{
	return static_cast<T>(static_cast<size_t>(lhs) ^ static_cast<size_t>(rhs));
}

template<typename T, typename U>
	requires ((std::is_enum_v<std::remove_reference_t<T>> || std::integral<std::remove_reference_t<T>>)
			&& (std::is_enum_v<std::remove_reference_t<U>> || std::integral<std::remove_reference_t<U>>))
T& operator^(T&& lhs, U&& rhs)
{
	return static_cast<T>(static_cast<size_t>(lhs) ^ static_cast<size_t>(rhs));
}

template<typename T, typename U>
	requires ((std::is_enum_v<std::remove_reference_t<T>> || std::integral<std::remove_reference_t<T>>)
			&& (std::is_enum_v<std::remove_reference_t<U>> || std::integral<std::remove_reference_t<U>>))
T& operator|=(T& lhs, const U& rhs)
{
	//lhs = static_cast<T>(static_cast<size_t>(lhs) | static_cast<size_t>(rhs));
	lhs = lhs | rhs;
	return lhs;
}

template<typename T, typename U>
	requires ((std::is_enum_v<std::remove_reference_t<T>> || std::integral<std::remove_reference_t<T>>)
			&& (std::is_enum_v<std::remove_reference_t<U>> || std::integral<std::remove_reference_t<U>>))
T& operator|=(T&& lhs, U&& rhs)
{
	//lhs = static_cast<T>(static_cast<size_t>(lhs) | static_cast<size_t>(rhs));
	lhs = lhs | rhs;
	return lhs;
}

template<typename T, typename U>
	requires ((std::is_enum_v<std::remove_reference_t<T>> || std::integral<std::remove_reference_t<T>>)
			&& (std::is_enum_v<std::remove_reference_t<U>> || std::integral<std::remove_reference_t<U>>))
T& operator&=(T& lhs, const U& rhs)
{
	//lhs = static_cast<T>(static_cast<size_t>(lhs) & static_cast<size_t>(rhs));
	lhs = lhs & rhs;
	return lhs;
}

template<typename T, typename U>
	requires ((std::is_enum_v<std::remove_reference_t<T>> || std::integral<std::remove_reference_t<T>>)
			&& (std::is_enum_v<std::remove_reference_t<U>> || std::integral<std::remove_reference_t<U>>))
T& operator&=(T&& lhs, U&& rhs)
{
	//lhs = static_cast<T>(static_cast<size_t>(lhs) & static_cast<size_t>(rhs));
	lhs = lhs & rhs;
	return lhs;
}

template<typename T, typename U>
	requires ((std::is_enum_v<std::remove_reference_t<T>> || std::integral<std::remove_reference_t<T>>)
			&& (std::is_enum_v<std::remove_reference_t<U>> || std::integral<std::remove_reference_t<U>>))
T& operator^=(T& lhs, const U& rhs)
{
	//lhs = static_cast<T>(static_cast<size_t>(lhs) ^ static_cast<size_t>(rhs));
	lhs = lhs ^ rhs;
	return lhs;
}

template<typename T, typename U>
	requires ((std::is_enum_v<std::remove_reference_t<T>> || std::integral<std::remove_reference_t<T>>)
			&& (std::is_enum_v<std::remove_reference_t<U>> || std::integral<std::remove_reference_t<U>>))
T& operator^=(T&& lhs, U&& rhs)
{
	//lhs = static_cast<T>(static_cast<size_t>(lhs) ^ static_cast<size_t>(rhs));
	lhs = lhs ^ rhs;
	return lhs;
}

} // end namespace cudadoom
