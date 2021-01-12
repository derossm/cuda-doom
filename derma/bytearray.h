/**********************************************************************************************************************************************\
	Copyright© 2020-2021 Mason DeRoss

	Released under the GNU All-permissive License

	Copying and distribution of this file, with or without modification, are permitted in any medium without royalty,
	provided the copyright notice and this notice are preserved. This file is offered as-is, without any warranty.

	DESCRIPTION:

\**********************************************************************************************************************************************/
#pragma once

#include <concepts>
#include <type_traits>

#include <cstddef>
#include <cstdint>

#include <array>
#include <bit>

namespace derma::experimental
{

template<typename T, size_t N=1>
	requires std::is_integral_v<T>
class Byte
{
public:
	::std::array<::std::byte, 8> byteArray;
	uint64_t bits = reinterpret_cast<uint64_t>(byteArray.data());

	//using type = T;
	//Byte() : bits{0x00} {}
	//Byte(T bits) : bits{bits} {}
	//Byte(::std::byte byte) : bits{byte} {}

	// add logging
	// add overflow/underflow guard/checking
	// add support for narrowing to the smallest required size
	// add support for narrowing to a fixed size with intentional data loss

	// BOILER PLATE FROM HERE TO END
	/* #region BoilerPlate */
	constexpr operator int() const
	{
		return static_cast<int>(bits);
	}

	//=====
	// unary
	constexpr inline bool operator!() const noexcept
	{
		return static_cast<bool>(bits) != 0;
	}

	constexpr inline Byte<T, N>& operator~() noexcept
	{
		//bits = ~static_cast<T>(bits);
		bits = ~bits;
		return *this;
	}

	constexpr inline Byte<T, N>& operator++() noexcept
	{
		//bits = static_cast<T>(bits) + 1;
		++bits;
		return *this;
	}

	constexpr inline Byte<T, N>& operator--() noexcept
	{
		//bits = static_cast<T>(bits) - 1;
		--bits;
		return *this;
	}

	//=====
	// special by constant-ref
	//template<typename U>
		//requires (std::is_integral_v<U> || ::std::is_same_v<U, Byte<T, N>> || ::std::is_class_v<U>)
	//constexpr inline bool operator->() const noexcept {}

	//template<typename U>
		//requires (std::is_integral_v<U> || ::std::is_same_v<U, Byte<T, N>> || ::std::is_class_v<U>)
	//constexpr inline bool operator->*() const noexcept {}

	//template<typename U>
		//requires (std::is_integral_v<U> || ::std::is_same_v<U, Byte<T, N>> || ::std::is_class_v<U>)
	//constexpr inline bool operator()() const noexcept {}

	//template<typename U>
		//requires (std::is_integral_v<U> || ::std::is_same_v<U, Byte<T, N>> || ::std::is_class_v<U>)
	//constexpr inline bool operator[]() const noexcept {}

	//template<typename U>
		//requires (std::is_integral_v<U> || ::std::is_same_v<U, Byte<T, N>> || ::std::is_class_v<U>)
	//constexpr inline bool operator, () const noexcept = delete;

	// special by forwarding-ref
	//template<typename U>
		//requires (std::is_integral_v<U> || ::std::is_same_v<U, Byte<T, N>> || ::std::is_class_v<U>)
	//constexpr inline bool operator->() const noexcept {}

	//template<typename U>
		//requires (std::is_integral_v<U> || ::std::is_same_v<U, Byte<T, N>> || ::std::is_class_v<U>)
	//constexpr inline bool operator->*() const noexcept {}

	//template<typename U>
		//requires (std::is_integral_v<U> || ::std::is_same_v<U, Byte<T, N>> || ::std::is_class_v<U>)
	//constexpr inline bool operator()() const noexcept {}

	//template<typename U>
		//requires (std::is_integral_v<U> || ::std::is_same_v<U, Byte<T, N>> || ::std::is_class_v<U>)
	//constexpr inline bool operator[]() const noexcept {}

	//template<typename U>
		//requires (std::is_integral_v<U> || ::std::is_same_v<U, Byte<T, N>> || ::std::is_class_v<U>)
	//constexpr inline bool operator, () const noexcept = delete;
};

}
