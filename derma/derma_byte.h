/**********************************************************************************************************************************************\
	Copyright© 2020-2021 Mason DeRoss

	Released under the GNU All-permissive License

	Copying and distribution of this file, with or without modification, are permitted in any medium without royalty,
	provided the copyright notice and this notice are preserved. This file is offered as-is, without any warranty.

	DESCRIPTION:

\**********************************************************************************************************************************************/
#pragma once

#include "stdafx.h"

template<typename T, size_t N = 1>
	requires std::is_integral_v<T>
class Byte
{
public:
	T bits;
	//::std::array<::std::byte, N> byteArray;

	using type = T;
	Byte() : bits{0} {}
	Byte(T bits) : bits{bits} {}
	//Byte(::std::byte byte) : bits{byte} {}

	// add logging
	// add overflow/underflow guard/checking
	// add support for narrowing to the smallest required size
	// add support for narrowing to a fixed size with intentional data loss

	// BOILER PLATE FROM HERE TO END
	/* #region BoilerPlate */

	//==========================================================================
	// user-defined conversion functions
	constexpr explicit operator bool() const noexcept
	{
		return static_cast<bool>(bits) != 0;
	}

	constexpr explicit operator uint64_t() const noexcept
	{
		return static_cast<uint64_t>(bits);
	}

	constexpr explicit operator int64_t() const noexcept
	{
		return static_cast<int64_t>(bits);
	}

	constexpr explicit operator uint32_t() const noexcept
	{
		return static_cast<uint32_t>(bits);
	}

	constexpr explicit operator int32_t() const noexcept
	{
		return static_cast<int32_t>(bits);
	}

	constexpr explicit operator uint16_t() const noexcept
	{
		return static_cast<uint16_t>(bits);
	}

	constexpr explicit operator int16_t() const noexcept
	{
		return static_cast<int16_t>(bits);
	}

	constexpr explicit operator uint8_t() const noexcept
	{
		return static_cast<uint8_t>(bits);
	}

	constexpr explicit operator int8_t() const noexcept
	{
		return static_cast<int8_t>(bits);
	}

	constexpr explicit operator wchar_t() const noexcept
	{
		return static_cast<wchar_t>(bits);
	}

	constexpr explicit operator char32_t() const noexcept
	{
		return static_cast<char32_t>(bits);
	}

	constexpr explicit operator char16_t() const noexcept
	{
		return static_cast<char16_t>(bits);
	}

	constexpr explicit operator char8_t() const noexcept
	{
		return static_cast<char8_t>(bits);
	}

	//constexpr explicit operator ptrdiff_t() const noexcept
	//{
	//	return static_cast<ptrdiff_t>(bits);
	//}

	//constexpr explicit operator size_t() const noexcept
	//{
	//	return static_cast<size_t>(bits);
	//}

	// this might shadow all the explicit overloads,
	// but seems necessary to work as integral array index i.e varArray[this] == varArray[this.bits]
	constexpr operator auto() const noexcept
	{
		return bits;
	}

	//==========================================================================
	// assignment by const-ref
	template<typename U>
		requires (std::is_integral_v<U> || ::std::is_same_v<U, Byte<T, N>> || ::std::is_class_v<U>)
	constexpr inline Byte<T, N>& operator=(const U& other) noexcept
	{
		if constexpr (::std::is_same_v<U, Byte<T, N>>)
		{
			if (this == &other)
			{
				return *this;
			}

			bits = other.bits;
			return *this;
		}
		else if constexpr (::std::is_class_v<U>)
		{

			bits = other.bits;
			return *this;
		}

		bits = static_cast<T>(other);
		return *this;
	}

	// assignment by forwarding-ref
	template<typename U>
		requires (std::is_integral_v<U> || ::std::is_same_v<U, Byte<T, N>> || ::std::is_class_v<U>)
	constexpr inline Byte<T, N>& operator=(U&& other) noexcept
	{
		if constexpr (::std::is_same_v<U, Byte<T, N>>)
		{
			if (this == &other)
			{
				return *this;
			}

			bits = other.bits;
			return *this;
		}
		else if constexpr (::std::is_class_v<U>)
		{

			bits = other.bits;
			return *this;
		}

		bits = static_cast<T>(other);
		return *this;
	}

	//==========================================================================
	// unary
	constexpr inline bool operator!() const noexcept
	{
		return static_cast<bool>(bits) == 0;
	}

	// arithmetic negation; user responsibility for the negation to be in bounds of T
	constexpr inline T operator-() const noexcept
	{
		return static_cast<T>(0ll - static_cast<int64_t>(bits));
	}

	// bit-wise negation
	constexpr inline Byte<T, N>& operator~() noexcept
	{
		bits = static_cast<T>(~bits);
		//bits = ~bits;
		return *this;
	}

	constexpr inline Byte<T, N>& operator++() noexcept
	{
		//bits = static_cast<T>(bits + 1);
		++bits;
		return *this;
	}

	constexpr inline Byte<T, N>& operator--() noexcept
	{
		//bits = static_cast<T>(bits - 1);
		--bits;
		return *this;
	}

private:
	// post-fix operators deleted until proven useful (so forever)
	constexpr inline Byte<T, N>& operator++(int) noexcept = delete;
	constexpr inline Byte<T, N>& operator--(int) noexcept = delete;

public:

	//==========================================================================
	// special by constant-ref
	//template<typename U>
	//	requires (std::is_integral_v<U> || ::std::is_same_v<U, Byte<T, N>> || ::std::is_class_v<U>)
	//constexpr inline auto operator->() const noexcept {}

	//template<typename U>
	//	requires (std::is_integral_v<U> || ::std::is_same_v<U, Byte<T, N>> || ::std::is_class_v<U>)
	//constexpr inline auto operator->*() const noexcept {}

	//template<typename U>
	//	requires (std::is_integral_v<U> || ::std::is_same_v<U, Byte<T, N>> || ::std::is_class_v<U>)
	//constexpr inline auto operator()() const noexcept {}

	//template<typename U>
	//	requires (std::is_integral_v<U> || ::std::is_same_v<U, Byte<T, N>> || ::std::is_class_v<U>)
	//constexpr inline auto operator[]() const noexcept {}

	//template<typename U>
	//	requires (std::is_integral_v<U> || ::std::is_same_v<U, Byte<T, N>> || ::std::is_class_v<U>)
	//constexpr inline auto operator, () const noexcept {}

	// special by forwarding-ref
	//template<typename U>
	//	requires (std::is_integral_v<U> || ::std::is_same_v<U, Byte<T, N>> || ::std::is_class_v<U>)
	//constexpr inline auto operator->() const noexcept {}

	//template<typename U>
	//	requires (std::is_integral_v<U> || ::std::is_same_v<U, Byte<T, N>> || ::std::is_class_v<U>)
	//constexpr inline auto operator->*() const noexcept {}

	//template<typename U>
	//	requires (std::is_integral_v<U> || ::std::is_same_v<U, Byte<T, N>> || ::std::is_class_v<U>)
	//constexpr inline auto operator()() const noexcept {}

	//template<typename U>
	//	requires (std::is_integral_v<U> || ::std::is_same_v<U, Byte<T, N>> || ::std::is_class_v<U>)
	//constexpr inline auto operator[]() const noexcept {}

	//template<typename U>
	//	requires (std::is_integral_v<U> || ::std::is_same_v<U, Byte<T, N>> || ::std::is_class_v<U>)
	//constexpr inline auto operator, () const noexcept {}

	//==========================================================================
	// shift (assignment) by const-ref
	template<typename U>
		requires (std::is_integral_v<U> || ::std::is_same_v<U, Byte<T, N>> || ::std::is_class_v<U>)
	constexpr inline Byte<T, N>& operator>>=(const U& other) noexcept
	{
		if constexpr (::std::is_same_v<U, Byte<T, N>> || ::std::is_class_v<U>)
		{
			bits = static_cast<T>(bits >> other.bits);
			return *this;
		}

		bits = static_cast<T>(bits >> other);
		return *this;
	}

	template<typename U>
		requires (std::is_integral_v<U> || ::std::is_same_v<U, Byte<T, N>> || ::std::is_class_v<U>)
	constexpr inline Byte<T, N>& operator<<=(const U& other) noexcept
	{
		if constexpr (::std::is_same_v<U, Byte<T, N>> || ::std::is_class_v<U>)
		{
			bits = static_cast<T>(bits << other.bits);
			return *this;
		}

		bits = static_cast<T>(bits << other);
		return *this;
	}

	// shift (assignment) by forwarding-ref
	template<typename U>
		requires (std::is_integral_v<U> || ::std::is_same_v<U, Byte<T, N>> || ::std::is_class_v<U>)
	constexpr inline Byte<T, N>& operator<<=(U&& other) noexcept
	{
		if constexpr (::std::is_same_v<U, Byte<T, N>> || ::std::is_class_v<U>)
		{
			bits = static_cast<T>(bits << other.bits);
			return *this;
		}

		bits = static_cast<T>(bits << other);
		return *this;
	}

	template<typename U>
		requires (std::is_integral_v<U> || ::std::is_same_v<U, Byte<T, N>> || ::std::is_class_v<U>)
	constexpr inline Byte<T, N>& operator>>=(U&& other) noexcept
	{
		if constexpr (::std::is_same_v<U, Byte<T, N>> || ::std::is_class_v<U>)
		{
			bits = static_cast<T>(bits >> other.bits);
			return *this;
		}

		bits = static_cast<T>(bits >> other);
		return *this;
	}

	//==========================================================================
	// shift (non-assignment) by const-ref
	template<typename U>
		requires (std::is_integral_v<U> || ::std::is_same_v<U, Byte<T, N>> || ::std::is_class_v<U>)
	constexpr inline T operator<<(const U& other) const noexcept
	{
		if constexpr (::std::is_same_v<U, Byte<T, N>> || ::std::is_class_v<U>)
		{
			return static_cast<T>(bits << other.bits);
		}

		return static_cast<T>(bits << other);
	}

	template<typename U>
		requires (std::is_integral_v<U> || ::std::is_same_v<U, Byte<T, N>> || ::std::is_class_v<U>)
	constexpr inline T operator>>(const U& other) const noexcept
	{
		if constexpr (::std::is_same_v<U, Byte<T, N>> || ::std::is_class_v<U>)
		{
			return static_cast<T>(bits >> other.bits);
		}

		return static_cast<T>(bits >> other);
	}

	// shift (non-assignment) by forwarding-ref
	template<typename U>
		requires (std::is_integral_v<U> || ::std::is_same_v<U, Byte<T, N>> || ::std::is_class_v<U>)
	constexpr inline T operator<<(U&& other) const noexcept
	{
		if constexpr (::std::is_same_v<U, Byte<T, N>> || ::std::is_class_v<U>)
		{
			return static_cast<T>(bits << other.bits);
		}

		return static_cast<T>(bits << other);
	}

	template<typename U>
		requires (std::is_integral_v<U> || ::std::is_same_v<U, Byte<T, N>> || ::std::is_class_v<U>)
	constexpr inline T operator>>(U&& other) const noexcept
	{
		if constexpr (::std::is_same_v<U, Byte<T, N>> || ::std::is_class_v<U>)
		{
			return static_cast<T>(bits >> other.bits);
		}

		return static_cast<T>(bits >> other);
	}

	//==========================================================================
	// logical comparison by const-ref
	template<typename U>
		requires (std::is_integral_v<U> || ::std::is_same_v<U, Byte<T, N>> || ::std::is_class_v<U>)
	constexpr inline bool operator||(const U& other) const noexcept
	{
		if constexpr (::std::is_same_v<U, Byte<T, N>> || ::std::is_class_v<U>)
		{
			return (bits || other.bits); // || 0;
		}

		return (static_cast<U>(bits) || other); // || 0;
	}

	template<typename U>
		requires (std::is_integral_v<U> || ::std::is_same_v<U, Byte<T, N>> || ::std::is_class_v<U>)
	constexpr inline bool operator&&(const U& other) const noexcept
	{
		if constexpr (::std::is_same_v<U, Byte<T, N>> || ::std::is_class_v<U>)
		{
			return (bits && other.bits); // && 0;
		}

		return (static_cast<U>(bits) && other); // && 0;
	}

	// logical comparison by forwarding-ref
	template<typename U>
		requires (std::is_integral_v<U> || ::std::is_same_v<U, Byte<T, N>> || ::std::is_class_v<U>)
	constexpr inline bool operator||(U&& other) const noexcept
	{
		if constexpr (::std::is_same_v<U, Byte<T, N>> || ::std::is_class_v<U>)
		{
			return (bits || other.bits); // || 0;
		}

		return (static_cast<U>(bits) || other); // || 0;
	}

	template<typename U>
		requires (std::is_integral_v<U> || ::std::is_same_v<U, Byte<T, N>> || ::std::is_class_v<U>)
	constexpr inline bool operator&&(U&& other) const noexcept
	{
		if constexpr (::std::is_same_v<U, Byte<T, N>> || ::std::is_class_v<U>)
		{
			return (bits && other.bits); // && 0;
		}

		return (static_cast<U>(bits) && other); // && 0;
	}

	//==========================================================================
	// comparison by const-ref
	template<typename U>
		requires (std::is_integral_v<U> || ::std::is_same_v<U, Byte<T, N>> || ::std::is_class_v<U>)
	constexpr inline bool operator==(const U& other) const noexcept
	{
		if constexpr (::std::is_same_v<U, Byte<T, N>> || ::std::is_class_v<U>)
		{
			return (bits == other.bits); // == 0;
		}

		return (static_cast<U>(bits) == other); // == 0;
	}

	template<typename U>
		requires (std::is_integral_v<U> || ::std::is_same_v<U, Byte<T, N>> || ::std::is_class_v<U>)
	constexpr inline bool operator!=(const U& other) const noexcept
	{
		if constexpr (::std::is_same_v<U, Byte<T, N>> || ::std::is_class_v<U>)
		{
			return (bits != other.bits); // != 0;
		}

		return (static_cast<U>(bits) != other); // != 0;
	}

	template<typename U>
		requires (std::is_integral_v<U> || ::std::is_same_v<U, Byte<T, N>> || ::std::is_class_v<U>)
	constexpr inline bool operator< (const U& other) const noexcept
	{
		if constexpr (::std::is_same_v<U, Byte<T, N>> || ::std::is_class_v<U>)
		{
			return (bits < other.bits); // < 0;
		}

		return (static_cast<U>(bits) <  other); // <  0;
	}

	template<typename U>
		requires (std::is_integral_v<U> || ::std::is_same_v<U, Byte<T, N>> || ::std::is_class_v<U>)
	constexpr inline bool operator<=(const U& other) const noexcept
	{
		if constexpr (::std::is_same_v<U, Byte<T, N>> || ::std::is_class_v<U>)
		{
			return (bits <= other.bits); // <= 0;
		}

		return (static_cast<U>(bits) <= other); // <= 0;
	}

	template<typename U>
		requires (std::is_integral_v<U> || ::std::is_same_v<U, Byte<T, N>> || ::std::is_class_v<U>)
	constexpr inline bool operator> (const U& other) const noexcept
	{
		if constexpr (::std::is_same_v<U, Byte<T, N>> || ::std::is_class_v<U>)
		{
			return (bits > other.bits); // > 0;
		}

		return (static_cast<U>(bits) >  other); // >  0;
	}

	template<typename U>
		requires (std::is_integral_v<U> || ::std::is_same_v<U, Byte<T, N>> || ::std::is_class_v<U>)
	constexpr inline bool operator>=(const U& other) const noexcept
	{
		if constexpr (::std::is_same_v<U, Byte<T, N>> || ::std::is_class_v<U>)
		{
			return (bits >= other.bits); // >= 0;
		}

		return (static_cast<U>(bits) >= other); // >= 0;
	}

	template<typename U>
		requires (std::is_integral_v<U> || ::std::is_same_v<U, Byte<T, N>> || ::std::is_class_v<U>)
	constexpr inline auto operator<=>(const U& other) const noexcept
	{
		if constexpr (::std::is_same_v<U, Byte<T, N>> || ::std::is_class_v<U>)
		{
			return bits <=> other.bits;
		}

		return static_cast<U>(bits) <=> other;
	}

	// comparison by forwarding-ref
	template<typename U>
		requires (std::is_integral_v<U> || ::std::is_same_v<U, Byte<T, N>> || ::std::is_class_v<U>)
	constexpr inline bool operator==(U&& other) const noexcept
	{
		if constexpr (::std::is_same_v<U, Byte<T, N>> || ::std::is_class_v<U>)
		{
			return (bits == other.bits); // == 0;
		}

		return (static_cast<U>(bits) == other); // == 0;
	}

	template<typename U>
		requires (std::is_integral_v<U> || ::std::is_same_v<U, Byte<T, N>> || ::std::is_class_v<U>)
	constexpr inline bool operator!=(U&& other) const noexcept
	{
		if constexpr (::std::is_same_v<U, Byte<T, N>> || ::std::is_class_v<U>)
		{
			return (bits != other.bits); // != 0;
		}

		return (static_cast<U>(bits) != other); // != 0;
	}

	template<typename U>
		requires (std::is_integral_v<U> || ::std::is_same_v<U, Byte<T, N>> || ::std::is_class_v<U>)
	constexpr inline bool operator< (U&& other) const noexcept
	{
		if constexpr (::std::is_same_v<U, Byte<T, N>> || ::std::is_class_v<U>)
		{
			return (bits < other.bits); // < 0;
		}

		return (static_cast<U>(bits) <  other); // <  0;
	}

	template<typename U>
		requires (std::is_integral_v<U> || ::std::is_same_v<U, Byte<T, N>> || ::std::is_class_v<U>)
	constexpr inline bool operator<=(U&& other) const noexcept
	{
		if constexpr (::std::is_same_v<U, Byte<T, N>> || ::std::is_class_v<U>)
		{
			return (bits <= other.bits); // <= 0;
		}

		return (static_cast<U>(bits) <= other); // <= 0;
	}

	template<typename U>
		requires (std::is_integral_v<U> || ::std::is_same_v<U, Byte<T, N>> || ::std::is_class_v<U>)
	constexpr inline bool operator> (U&& other) const noexcept
	{
		if constexpr (::std::is_same_v<U, Byte<T, N>> || ::std::is_class_v<U>)
		{
			return (bits > other.bits); // > 0;
		}

		return (static_cast<U>(bits) >  other); // >  0;
	}

	template<typename U>
		requires (std::is_integral_v<U> || ::std::is_same_v<U, Byte<T, N>> || ::std::is_class_v<U>)
	constexpr inline bool operator>=(U&& other) const noexcept
	{
		if constexpr (::std::is_same_v<U, Byte<T, N>> || ::std::is_class_v<U>)
		{
			return (bits >= other.bits); // >= 0;
		}

		return (static_cast<U>(bits) >= other); // >= 0;
	}

	template<typename U>
		requires (std::is_integral_v<U> || ::std::is_same_v<U, Byte<T, N>> || ::std::is_class_v<U>)
	constexpr inline auto operator<=>(U&& other) const noexcept
	{
		if constexpr (::std::is_same_v<U, Byte<T, N>> || ::std::is_class_v<U>)
		{
			return bits <=> other.bits;
		}

		return static_cast<U>(bits) <=> other;
	}

	//==========================================================================
	// bitwise (assignment) by const-ref
	template<typename U>
		requires (std::is_integral_v<U> || ::std::is_same_v<U, Byte<T, N>> || ::std::is_class_v<U>)
	constexpr inline Byte<T, N>& operator|=(const U& other) noexcept
	{
		if constexpr (::std::is_same_v<U, Byte<T, N>> || ::std::is_class_v<U>)
		{
			bits |= other.bits;
			return *this;
		}

		bits |= static_cast<T>(other);
		return *this;
	}

	template<typename U>
		requires (std::is_integral_v<U> || ::std::is_same_v<U, Byte<T, N>> || ::std::is_class_v<U>)
	constexpr inline Byte<T, N>& operator&=(const U& other) noexcept
	{
		if constexpr (::std::is_same_v<U, Byte<T, N>> || ::std::is_class_v<U>)
		{
			bits &= other.bits;
			return *this;
		}

		bits &= static_cast<T>(other);
		return *this;
	}

	template<typename U>
		requires (std::is_integral_v<U> || ::std::is_same_v<U, Byte<T, N>> || ::std::is_class_v<U>)
	constexpr inline Byte<T, N>& operator^=(const U& other) noexcept
	{
		if constexpr (::std::is_same_v<U, Byte<T, N>> || ::std::is_class_v<U>)
		{
			bits ^= other.bits;
			return *this;
		}

		bits ^= static_cast<T>(other);
		return *this;
	}

	// bitwise (assignment) by forwarding-ref
	template<typename U>
		requires (std::is_integral_v<U> || ::std::is_same_v<U, Byte<T, N>> || ::std::is_class_v<U>)
	constexpr inline Byte<T, N>& operator|=(U&& other) noexcept
	{
		if constexpr (::std::is_same_v<U, Byte<T, N>> || ::std::is_class_v<U>)
		{
			bits |= other.bits;
			return *this;
		}

		bits |= static_cast<T>(other);
		return *this;
	}

	template<typename U>
		requires (std::is_integral_v<U> || ::std::is_same_v<U, Byte<T, N>> || ::std::is_class_v<U>)
	constexpr inline Byte<T, N>& operator&=(U&& other) noexcept
	{
		if constexpr (::std::is_same_v<U, Byte<T, N>> || ::std::is_class_v<U>)
		{
			bits &= other.bits;
			return *this;
		}

		bits &= static_cast<T>(other);
		return *this;
	}

	template<typename U>
		requires (std::is_integral_v<U> || ::std::is_same_v<U, Byte<T, N>> || ::std::is_class_v<U>)
	constexpr inline Byte<T, N>& operator^=(U&& other) noexcept
	{
		if constexpr (::std::is_same_v<U, Byte<T, N>> || ::std::is_class_v<U>)
		{
			bits ^= other.bits;
			return *this;
		}

		bits ^= static_cast<T>(other);
		return *this;
	}

	//==========================================================================
	// arithmetic (assignment) by const-ref
	template<typename U>
		requires (std::is_integral_v<U> || ::std::is_same_v<U, Byte<T, N>> || ::std::is_class_v<U>)
	constexpr inline Byte<T, N>& operator+=(const U& other) noexcept
	{
		if constexpr (::std::is_same_v<U, Byte<T, N>> || ::std::is_class_v<U>)
		{
			bits = static_cast<T>(bits + other.bits);
			return *this;
		}

		bits = static_cast<T>(static_cast<U>(bits) + other);
		return *this;
	}

	template<typename U>
		requires (std::is_integral_v<U> || ::std::is_same_v<U, Byte<T, N>> || ::std::is_class_v<U>)
	constexpr inline Byte<T, N>& operator-=(const U& other) noexcept
	{
		if constexpr (::std::is_same_v<U, Byte<T, N>> || ::std::is_class_v<U>)
		{
			bits = static_cast<T>(bits - other.bits);
			return *this;
		}

		bits = static_cast<T>(static_cast<U>(bits) - other);
		return *this;
	}

	template<typename U>
		requires (std::is_integral_v<U> || ::std::is_same_v<U, Byte<T, N>> || ::std::is_class_v<U>)
	constexpr inline Byte<T, N>& operator*=(const U& other) noexcept
	{
		if constexpr (::std::is_same_v<U, Byte<T, N>> || ::std::is_class_v<U>)
		{
			bits = static_cast<T>(bits * other.bits);
			return *this;
		}

		bits = static_cast<T>(static_cast<U>(bits) * other);
		return *this;
	}

	template<typename U>
		requires (std::is_integral_v<U> || ::std::is_same_v<U, Byte<T, N>> || ::std::is_class_v<U>)
	constexpr inline Byte<T, N>& operator/=(const U& other) noexcept
	{
		if constexpr (::std::is_same_v<U, Byte<T, N>> || ::std::is_class_v<U>)
		{
			bits = static_cast<T>(bits / other.bits);
			return *this;
		}

		bits = static_cast<T>(static_cast<U>(bits) / other);
		return *this;
	}

	template<typename U>
		requires (std::is_integral_v<U> || ::std::is_same_v<U, Byte<T, N>> || ::std::is_class_v<U>)
	constexpr inline Byte<T, N>& operator%=(const U& other) noexcept
	{
		if constexpr (::std::is_same_v<U, Byte<T, N>> || ::std::is_class_v<U>)
		{
			bits = static_cast<T>(bits % other.bits);
			return *this;
		}

		bits = static_cast<T>(static_cast<U>(bits) % other);
		return *this;
	}

	// arithmetic (assignment) by forwarding-ref
	template<typename U>
		requires (std::is_integral_v<U> || ::std::is_same_v<U, Byte<T, N>> || ::std::is_class_v<U>)
	constexpr inline Byte<T, N>& operator+=(U&& other) noexcept
	{
		if constexpr (::std::is_same_v<U, Byte<T, N>> || ::std::is_class_v<U>)
		{
			bits = static_cast<T>(bits + other.bits);
			return *this;
		}

		bits = static_cast<T>(static_cast<U>(bits) + other);
		return *this;
	}

	template<typename U>
		requires (std::is_integral_v<U> || ::std::is_same_v<U, Byte<T, N>> || ::std::is_class_v<U>)
	constexpr inline Byte<T, N>& operator-=(U&& other) noexcept
	{
		if constexpr (::std::is_same_v<U, Byte<T, N>> || ::std::is_class_v<U>)
		{
			bits = static_cast<T>(bits - other.bits);
			return *this;
		}

		bits = static_cast<T>(static_cast<U>(bits) - other);
		return *this;
	}

	template<typename U>
		requires (std::is_integral_v<U> || ::std::is_same_v<U, Byte<T, N>> || ::std::is_class_v<U>)
	constexpr inline Byte<T, N>& operator*=(U&& other) noexcept
	{
		if constexpr (::std::is_same_v<U, Byte<T, N>> || ::std::is_class_v<U>)
		{
			bits = static_cast<T>(bits * other.bits);
			return *this;
		}

		bits = static_cast<T>(static_cast<U>(bits) * other);
		return *this;
	}

	template<typename U>
		requires (std::is_integral_v<U> || ::std::is_same_v<U, Byte<T, N>> || ::std::is_class_v<U>)
	constexpr inline Byte<T, N>& operator/=(U&& other) noexcept
	{
		if constexpr (::std::is_same_v<U, Byte<T, N>> || ::std::is_class_v<U>)
		{
			bits = static_cast<T>(bits / other.bits);
			return *this;
		}

		bits = static_cast<T>(static_cast<U>(bits) / other);
		return *this;
	}

	template<typename U>
		requires (std::is_integral_v<U> || ::std::is_same_v<U, Byte<T, N>> || ::std::is_class_v<U>)
	constexpr inline Byte<T, N>& operator%=(U&& other) noexcept
	{
		if constexpr (::std::is_same_v<U, Byte<T, N>> || ::std::is_class_v<U>)
		{
			bits = static_cast<T>(bits % other.bits);
			return *this;
		}

		bits = static_cast<T>(static_cast<U>(bits) % other);
		return *this;
	}

	//==========================================================================
	// bitwise (assignment) by const-ref
	template<typename U>
		requires (std::is_integral_v<U> || ::std::is_same_v<U, Byte<T, N>> || ::std::is_class_v<U>)
	constexpr inline T operator|(const U& other) const noexcept
	{
		if constexpr (::std::is_same_v<U, Byte<T, N>> || ::std::is_class_v<U>)
		{
			return bits | other.bits;
		}

		return static_cast<T>(static_cast<U>(bits) | other);
	}

	template<typename U>
		requires (std::is_integral_v<U> || ::std::is_same_v<U, Byte<T, N>> || ::std::is_class_v<U>)
	constexpr inline T operator&(const U& other) const noexcept
	{
		if constexpr (::std::is_same_v<U, Byte<T, N>> || ::std::is_class_v<U>)
		{
			return bits & other.bits;
		}

		return static_cast<T>(static_cast<U>(bits) & other);
	}

	template<typename U>
		requires (std::is_integral_v<U> || ::std::is_same_v<U, Byte<T, N>> || ::std::is_class_v<U>)
	constexpr inline T operator^(const U& other) const noexcept
	{
		if constexpr (::std::is_same_v<U, Byte<T, N>> || ::std::is_class_v<U>)
		{
			return bits ^ other.bits;
		}

		return static_cast<T>(static_cast<U>(bits) ^ other);
	}

	// bitwise (non-assignment) by forwarding-ref
	template<typename U>
		requires (std::is_integral_v<U> || ::std::is_same_v<U, Byte<T, N>> || ::std::is_class_v<U>)
	constexpr inline T operator|(U&& other) const noexcept
	{
		if constexpr (::std::is_same_v<U, Byte<T, N>> || ::std::is_class_v<U>)
		{
			return bits | other.bits;
		}

		return static_cast<T>(static_cast<U>(bits) | other);
	}

	template<typename U>
		requires (std::is_integral_v<U> || ::std::is_same_v<U, Byte<T, N>> || ::std::is_class_v<U>)
	constexpr inline T operator&(U&& other) const noexcept
	{
		if constexpr (::std::is_same_v<U, Byte<T, N>> || ::std::is_class_v<U>)
		{
			return bits & other.bits;
		}

		return static_cast<T>(static_cast<U>(bits) & other);
	}

	template<typename U>
		requires (std::is_integral_v<U> || ::std::is_same_v<U, Byte<T, N>> || ::std::is_class_v<U>)
	constexpr inline T operator^(U&& other) const noexcept
	{
		if constexpr (::std::is_same_v<U, Byte<T, N>> || ::std::is_class_v<U>)
		{
			return bits ^ other.bits;
		}

		return static_cast<T>(static_cast<U>(bits) ^ other);
	}

	//==========================================================================
	// arithmetic (non-assignment) by const-ref
	template<typename U>
		requires (std::is_integral_v<U> || ::std::is_same_v<U, Byte<T, N>> || ::std::is_class_v<U>)
	constexpr inline T operator+(const U& other) const noexcept
	{
		if constexpr (::std::is_same_v<U, Byte<T, N>> || ::std::is_class_v<U>)
		{
			return bits + other.bits;
		}

		return static_cast<T>(static_cast<U>(bits) + other);
	}

	template<typename U>
		requires (std::is_integral_v<U> || ::std::is_same_v<U, Byte<T, N>> || ::std::is_class_v<U>)
	constexpr inline T operator-(const U& other) const noexcept
	{
		if constexpr (::std::is_same_v<U, Byte<T, N>> || ::std::is_class_v<U>)
		{
			return bits - other.bits;
		}

		return static_cast<T>(static_cast<U>(bits) - other);
	}

	template<typename U>
		requires (std::is_integral_v<U> || ::std::is_same_v<U, Byte<T, N>> || ::std::is_class_v<U>)
	constexpr inline T operator*(const U& other) const noexcept
	{
		if constexpr (::std::is_same_v<U, Byte<T, N>> || ::std::is_class_v<U>)
		{
			return bits * other.bits;
		}

		return static_cast<T>(static_cast<U>(bits) * other);
	}

	template<typename U>
		requires (std::is_integral_v<U> || ::std::is_same_v<U, Byte<T, N>> || ::std::is_class_v<U>)
	constexpr inline T operator/(const U& other) const noexcept
	{
		if constexpr (::std::is_same_v<U, Byte<T, N>> || ::std::is_class_v<U>)
		{
			return bits / other.bits;
		}

		return static_cast<T>(static_cast<U>(bits) / other);
	}

	template<typename U>
		requires (std::is_integral_v<U> || ::std::is_same_v<U, Byte<T, N>> || ::std::is_class_v<U>)
	constexpr inline T operator%(const U& other) const noexcept
	{
		if constexpr (::std::is_same_v<U, Byte<T, N>> || ::std::is_class_v<U>)
		{
			return bits / other.bits;
		}

		return static_cast<T>(static_cast<U>(bits) % other);
	}

	// arithmetic (non-assignment) by forwarding-ref
	template<typename U>
		requires (std::is_integral_v<U> || ::std::is_same_v<U, Byte<T, N>> || ::std::is_class_v<U>)
	constexpr inline T operator+(U&& other) const noexcept
	{
		if constexpr (::std::is_same_v<U, Byte<T, N>> || ::std::is_class_v<U>)
		{
			return bits + other.bits;
		}

		return static_cast<T>(static_cast<U>(bits) + other);
	}

	template<typename U>
		requires (std::is_integral_v<U> || ::std::is_same_v<U, Byte<T, N>> || ::std::is_class_v<U>)
	constexpr inline T operator-(U&& other) const noexcept
	{
		if constexpr (::std::is_same_v<U, Byte<T, N>> || ::std::is_class_v<U>)
		{
			return bits - other.bits;
		}

		return static_cast<T>(static_cast<U>(bits) - other);
	}

	template<typename U>
		requires (std::is_integral_v<U> || ::std::is_same_v<U, Byte<T, N>> || ::std::is_class_v<U>)
	constexpr inline T operator*(U&& other) const noexcept
	{
		if constexpr (::std::is_same_v<U, Byte<T, N>> || ::std::is_class_v<U>)
		{
			return bits * other.bits;
		}

		return static_cast<T>(static_cast<U>(bits) * other);
	}

	template<typename U>
		requires (std::is_integral_v<U> || ::std::is_same_v<U, Byte<T, N>> || ::std::is_class_v<U>)
	constexpr inline T operator/(U&& other) const noexcept
	{
		if constexpr (::std::is_same_v<U, Byte<T, N>> || ::std::is_class_v<U>)
		{
			return bits / other.bits;
		}

		return static_cast<T>(static_cast<U>(bits) / other);
	}

	template<typename U>
		requires (std::is_integral_v<U> || ::std::is_same_v<U, Byte<T, N>> || ::std::is_class_v<U>)
	constexpr inline T operator%(U&& other) const noexcept
	{
		if constexpr (::std::is_same_v<U, Byte<T, N>> || ::std::is_class_v<U>)
		{
			return bits / other.bits;
		}

		return static_cast<T>(static_cast<U>(bits) % other);
	}

//==============================================================================
// friends

	//==========================================================================
	// logical comparison by const-ref
	template<typename T, size_t N, typename U>
		requires (std::is_integral_v<U> && !::std::is_same_v<U, Byte<T, N>>)
	friend constexpr inline bool operator||(const U& lhs, const Byte<T, N>& rhs) noexcept;
	template<typename T, size_t N, typename U>
		requires (std::is_integral_v<U> && !::std::is_same_v<U, Byte<T, N>>)
	friend constexpr inline bool operator&&(const U& lhs, const Byte<T, N>& rhs) noexcept;

	// logical comparison by forwarding-ref
	template<typename T, size_t N, typename U>
		requires (std::is_integral_v<U> && !::std::is_same_v<U, Byte<T, N>>)
	friend constexpr inline bool operator||(U&& lhs, Byte<T, N>&& rhs) noexcept;
	template<typename T, size_t N, typename U>
		requires (std::is_integral_v<U> && !::std::is_same_v<U, Byte<T, N>>)
	friend constexpr inline bool operator&&(U&& lhs, Byte<T, N>&& rhs) noexcept;

	//==========================================================================
	// comparison by const-ref
	template<typename T, size_t N, typename U>
		requires (std::is_integral_v<U> && !::std::is_same_v<U, Byte<T, N>>)
	friend constexpr inline bool operator==(const U& lhs, const Byte<T, N>& rhs) noexcept;
	template<typename T, size_t N, typename U>
		requires (std::is_integral_v<U> && !::std::is_same_v<U, Byte<T, N>>)
	friend constexpr inline bool operator!=(const U& lhs, const Byte<T, N>& rhs) noexcept;
	template<typename T, size_t N, typename U>
		requires (std::is_integral_v<U> && !::std::is_same_v<U, Byte<T, N>>)
	friend constexpr inline bool operator< (const U& lhs, const Byte<T, N>& rhs) noexcept;
	template<typename T, size_t N, typename U>
		requires (std::is_integral_v<U> && !::std::is_same_v<U, Byte<T, N>>)
	friend constexpr inline bool operator<=(const U& lhs, const Byte<T, N>& rhs) noexcept;
	template<typename T, size_t N, typename U>
		requires (std::is_integral_v<U> && !::std::is_same_v<U, Byte<T, N>>)
	friend constexpr inline bool operator> (const U& lhs, const Byte<T, N>& rhs) noexcept;
	template<typename T, size_t N, typename U>
		requires (std::is_integral_v<U> && !::std::is_same_v<U, Byte<T, N>>)
	friend constexpr inline bool operator>=(const U& lhs, const Byte<T, N>& rhs) noexcept;
	template<typename T, size_t N, typename U>
		requires (std::is_integral_v<U> && !::std::is_same_v<U, Byte<T, N>>)
	friend constexpr inline auto operator<=>(const U& lhs, const Byte<T, N>& rhs) noexcept;

	// comparison by forwarding-ref
	template<typename T, size_t N, typename U>
		requires (std::is_integral_v<U> && !::std::is_same_v<U, Byte<T, N>>)
	friend constexpr inline bool operator==(U&& lhs, Byte<T, N>&& rhs) noexcept;
	template<typename T, size_t N, typename U>
		requires (std::is_integral_v<U> && !::std::is_same_v<U, Byte<T, N>>)
	friend constexpr inline bool operator!=(U&& lhs, Byte<T, N>&& rhs) noexcept;
	template<typename T, size_t N, typename U>
		requires (std::is_integral_v<U> && !::std::is_same_v<U, Byte<T, N>>)
	friend constexpr inline bool operator< (U&& lhs, Byte<T, N>&& rhs) noexcept;
	template<typename T, size_t N, typename U>
		requires (std::is_integral_v<U> && !::std::is_same_v<U, Byte<T, N>>)
	friend constexpr inline bool operator<=(U&& lhs, Byte<T, N>&& rhs) noexcept;
	template<typename T, size_t N, typename U>
		requires (std::is_integral_v<U> && !::std::is_same_v<U, Byte<T, N>>)
	friend constexpr inline bool operator> (U&& lhs, Byte<T, N>&& rhs) noexcept;
	template<typename T, size_t N, typename U>
		requires (std::is_integral_v<U> && !::std::is_same_v<U, Byte<T, N>>)
	friend constexpr inline bool operator>=(U&& lhs, Byte<T, N>&& rhs) noexcept;
	template<typename T, size_t N, typename U>
		requires (std::is_integral_v<U> && !::std::is_same_v<U, Byte<T, N>>)
	friend constexpr inline auto operator<=>(U&& lhs, Byte<T, N>&& rhs) noexcept;

	//==========================================================================
	// bitwise (assignment) by const-ref
	template<typename T, size_t N, typename U>
		requires (std::is_integral_v<U> && !::std::is_same_v<U, Byte<T, N>>)
	friend constexpr inline U& operator|=(U& lhs, const Byte<T, N>& rhs) noexcept;
	template<typename T, size_t N, typename U>
		requires (std::is_integral_v<U> && !::std::is_same_v<U, Byte<T, N>>)
	friend constexpr inline U& operator&=(U& lhs, const Byte<T, N>& rhs) noexcept;
	template<typename T, size_t N, typename U>
		requires (std::is_integral_v<U> && !::std::is_same_v<U, Byte<T, N>>)
	friend constexpr inline U& operator^=(U& lhs, const Byte<T, N>& rhs) noexcept;

	// bitwise (assignment) by forwarding-ref
	template<typename T, size_t N, typename U>
		requires (std::is_integral_v<U> && !::std::is_same_v<U, Byte<T, N>>)
	friend constexpr inline U& operator|=(U&& lhs, Byte<T, N>&& rhs) noexcept;
	template<typename T, size_t N, typename U>
		requires (std::is_integral_v<U> && !::std::is_same_v<U, Byte<T, N>>)
	friend constexpr inline U& operator&=(U&& lhs, Byte<T, N>&& rhs) noexcept;
	template<typename T, size_t N, typename U>
		requires (std::is_integral_v<U> && !::std::is_same_v<U, Byte<T, N>>)
	friend constexpr inline U& operator^=(U&& lhs, Byte<T, N>&& rhs) noexcept;

	//==========================================================================
	// arithmetic (assignment) by const-ref
	template<typename T, size_t N, typename U>
		requires (std::is_integral_v<U> && !::std::is_same_v<U, Byte<T, N>>)
	friend constexpr inline U& operator+=(U& lhs, const Byte<T, N>& rhs) noexcept;
	template<typename T, size_t N, typename U>
		requires (std::is_integral_v<U> && !::std::is_same_v<U, Byte<T, N>>)
	friend constexpr inline U& operator-=(U& lhs, const Byte<T, N>& rhs) noexcept;
	template<typename T, size_t N, typename U>
		requires (std::is_integral_v<U> && !::std::is_same_v<U, Byte<T, N>>)
	friend constexpr inline U& operator*=(U& lhs, const Byte<T, N>& rhs) noexcept;
	template<typename T, size_t N, typename U>
		requires (std::is_integral_v<U> && !::std::is_same_v<U, Byte<T, N>>)
	friend constexpr inline U& operator/=(U& lhs, const Byte<T, N>& rhs) noexcept;
	template<typename T, size_t N, typename U>
		requires (std::is_integral_v<U> && !::std::is_same_v<U, Byte<T, N>>)
	friend constexpr inline U& operator%=(U& lhs, const Byte<T, N>& rhs) noexcept;

	// arithmetic (assignment) by forwarding-ref
	template<typename T, size_t N, typename U>
		requires (std::is_integral_v<U> && !::std::is_same_v<U, Byte<T, N>>)
	friend constexpr inline U& operator+=(U&& lhs, Byte<T, N>&& rhs) noexcept;
	template<typename T, size_t N, typename U>
		requires (std::is_integral_v<U> && !::std::is_same_v<U, Byte<T, N>>)
	friend constexpr inline U& operator-=(U&& lhs, Byte<T, N>&& rhs) noexcept;
	template<typename T, size_t N, typename U>
		requires (std::is_integral_v<U> && !::std::is_same_v<U, Byte<T, N>>)
	friend constexpr inline U& operator*=(U&& lhs, Byte<T, N>&& rhs) noexcept;
	template<typename T, size_t N, typename U>
		requires (std::is_integral_v<U> && !::std::is_same_v<U, Byte<T, N>>)
	friend constexpr inline U& operator/=(U&& lhs, Byte<T, N>&& rhs) noexcept;
	template<typename T, size_t N, typename U>
		requires (std::is_integral_v<U> && !::std::is_same_v<U, Byte<T, N>>)
	friend constexpr inline U& operator%=(U&& lhs, Byte<T, N>&& rhs) noexcept;

	//==========================================================================
	// bitwise (non-assignment) by const-ref
	template<typename T, size_t N, typename U>
		requires (std::is_integral_v<U> && !::std::is_same_v<U, Byte<T, N>>)
	friend constexpr inline U operator|(const U& lhs, const Byte<T, N>& rhs) noexcept;
	template<typename T, size_t N, typename U>
		requires (std::is_integral_v<U> && !::std::is_same_v<U, Byte<T, N>>)
	friend constexpr inline U operator&(const U& lhs, const Byte<T, N>& rhs) noexcept;
	template<typename T, size_t N, typename U>
		requires (std::is_integral_v<U> && !::std::is_same_v<U, Byte<T, N>>)
	friend constexpr inline U operator^(const U& lhs, const Byte<T, N>& rhs) noexcept;

	// bitwise (non-assignment) by forwarding-ref
	template<typename T, size_t N, typename U>
		requires (std::is_integral_v<U> && !::std::is_same_v<U, Byte<T, N>>)
	friend constexpr inline U operator|(U&& lhs, Byte<T, N>&& rhs) noexcept;
	template<typename T, size_t N, typename U>
		requires (std::is_integral_v<U> && !::std::is_same_v<U, Byte<T, N>>)
	friend constexpr inline U operator&(U&& lhs, Byte<T, N>&& rhs) noexcept;
	template<typename T, size_t N, typename U>
		requires (std::is_integral_v<U> && !::std::is_same_v<U, Byte<T, N>>)
	friend constexpr inline U operator^(U&& lhs, Byte<T, N>&& rhs) noexcept;

	//==========================================================================
	// arithmetic (non-assignment) by const-ref
	template<typename T, size_t N, typename U>
		requires (std::is_integral_v<U> && !::std::is_same_v<U, Byte<T, N>>)
	friend constexpr inline U operator+(const U& lhs, const Byte<T, N>& rhs) noexcept;
	template<typename T, size_t N, typename U>
		requires (std::is_integral_v<U> && !::std::is_same_v<U, Byte<T, N>>)
	friend constexpr inline U operator-(const U& lhs, const Byte<T, N>& rhs) noexcept;
	template<typename T, size_t N, typename U>
		requires (std::is_integral_v<U> && !::std::is_same_v<U, Byte<T, N>>)
	friend constexpr inline U operator*(const U& lhs, const Byte<T, N>& rhs) noexcept;
	template<typename T, size_t N, typename U>
		requires (std::is_integral_v<U> && !::std::is_same_v<U, Byte<T, N>>)
	friend constexpr inline U operator/(const U& lhs, const Byte<T, N>& rhs) noexcept;
	template<typename T, size_t N, typename U>
		requires (std::is_integral_v<U> && !::std::is_same_v<U, Byte<T, N>>)
	friend constexpr inline U operator%(const U& lhs, const Byte<T, N>& rhs) noexcept;

	// arithmetic (non-assignment) by forwarding-ref
	template<typename T, size_t N, typename U>
		requires (std::is_integral_v<U> && !::std::is_same_v<U, Byte<T, N>>)
	friend constexpr inline U operator+(U&& lhs, Byte<T, N>&& rhs) noexcept;
	template<typename T, size_t N, typename U>
		requires (std::is_integral_v<U> && !::std::is_same_v<U, Byte<T, N>>)
	friend constexpr inline U operator-(U&& lhs, Byte<T, N>&& rhs) noexcept;
	template<typename T, size_t N, typename U>
		requires (std::is_integral_v<U> && !::std::is_same_v<U, Byte<T, N>>)
	friend constexpr inline U operator*(U&& lhs, Byte<T, N>&& rhs) noexcept;
	template<typename T, size_t N, typename U>
		requires (std::is_integral_v<U> && !::std::is_same_v<U, Byte<T, N>>)
	friend constexpr inline U operator/(U&& lhs, Byte<T, N>&& rhs) noexcept;
	template<typename T, size_t N, typename U>
		requires (std::is_integral_v<U> && !::std::is_same_v<U, Byte<T, N>>)
	friend constexpr inline U operator%(U&& lhs, Byte<T, N>&& rhs) noexcept;
};

//==============================================================================
// logical comparison by forwarding-ref
template<typename T, size_t N, typename U>
	requires (std::is_integral_v<U> && !::std::is_same_v<U, Byte<T, N>>)
constexpr inline bool operator||(const U& lhs, const Byte<T, N>& rhs) noexcept
{
	return (lhs || static_cast<U>(rhs.bits)); // || 0;
}

template<typename T, size_t N, typename U>
	requires (std::is_integral_v<U> && !::std::is_same_v<U, Byte<T, N>>)
constexpr inline bool operator&&(const U& lhs, const Byte<T, N>& rhs) noexcept
{
	return (lhs && static_cast<U>(rhs.bits)); // && 0;
}

// logical comparison by const-ref
template<typename T, size_t N, typename U>
	requires (std::is_integral_v<U> && !::std::is_same_v<U, Byte<T, N>>)
constexpr inline bool operator||(U&& lhs, Byte<T, N>&& rhs) noexcept
{
	return (lhs || static_cast<U>(rhs.bits)); // || 0;
}

template<typename T, size_t N, typename U>
	requires (std::is_integral_v<U> && !::std::is_same_v<U, Byte<T, N>>)
constexpr inline bool operator&&(U&& lhs, Byte<T, N>&& rhs) noexcept
{
	return (lhs && static_cast<U>(rhs.bits)); // && 0;
}

//==============================================================================
// comparison by const-ref
template<typename T, size_t N, typename U>
	requires (std::is_integral_v<U> && !::std::is_same_v<U, Byte<T, N>>)
constexpr inline bool operator==(const U& lhs, const Byte<T, N>& rhs) noexcept
{
	return (lhs == static_cast<U>(rhs.bits)); // == 0;
}

template<typename T, size_t N, typename U>
	requires (std::is_integral_v<U> && !::std::is_same_v<U, Byte<T, N>>)
constexpr inline bool operator!=(const U& lhs, const Byte<T, N>& rhs) noexcept
{
	return (lhs != static_cast<U>(rhs.bits)); // != 0;
}

template<typename T, size_t N, typename U>
	requires (std::is_integral_v<U> && !::std::is_same_v<U, Byte<T, N>>)
constexpr inline bool operator< (const U& lhs, const Byte<T, N>& rhs) noexcept
{
	return (lhs < static_cast<U>(rhs.bits)); // < 0;
}

template<typename T, size_t N, typename U>
	requires (std::is_integral_v<U> && !::std::is_same_v<U, Byte<T, N>>)
constexpr inline bool operator<=(const U& lhs, const Byte<T, N>& rhs) noexcept
{
	return (lhs <= static_cast<U>(rhs.bits)); // <= 0;
}

template<typename T, size_t N, typename U>
	requires (std::is_integral_v<U> && !::std::is_same_v<U, Byte<T, N>>)
constexpr inline bool operator> (const U& lhs, const Byte<T, N>& rhs) noexcept
{
	return (lhs > static_cast<U>(rhs.bits)); // > 0;
}

template<typename T, size_t N, typename U>
	requires (std::is_integral_v<U> && !::std::is_same_v<U, Byte<T, N>>)
constexpr inline bool operator>=(const U& lhs, const Byte<T, N>& rhs) noexcept
{
	return (lhs >= static_cast<U>(rhs.bits)); // >= 0;
}

template<typename T, size_t N, typename U>
	requires (std::is_integral_v<U> && !::std::is_same_v<U, Byte<T, N>>)
constexpr inline auto operator<=>(const U& lhs, const Byte<T, N>& rhs) noexcept
{
	return lhs <=> static_cast<U>(rhs.bits);
}

// comparison by forwarding-ref
template<typename T, size_t N, typename U>
	requires (std::is_integral_v<U> && !::std::is_same_v<U, Byte<T, N>>)
constexpr inline bool operator==(U&& lhs, Byte<T, N>&& rhs) noexcept
{
	return (lhs == static_cast<U>(rhs.bits)); // == 0;
}

template<typename T, size_t N, typename U>
	requires (std::is_integral_v<U> && !::std::is_same_v<U, Byte<T, N>>)
constexpr inline bool operator!=(U&& lhs, Byte<T, N>&& rhs) noexcept
{
	return (lhs != static_cast<U>(rhs.bits)); // != 0;
}

template<typename T, size_t N, typename U>
	requires (std::is_integral_v<U> && !::std::is_same_v<U, Byte<T, N>>)
constexpr inline bool operator< (U&& lhs, Byte<T, N>&& rhs) noexcept
{
	return (lhs < static_cast<U>(rhs.bits)); // < 0;
}

template<typename T, size_t N, typename U>
	requires (std::is_integral_v<U> && !::std::is_same_v<U, Byte<T, N>>)
constexpr inline bool operator<=(U&& lhs, Byte<T, N>&& rhs) noexcept
{
	return (lhs <= static_cast<U>(rhs.bits)); // <= 0;
}

template<typename T, size_t N, typename U>
	requires (std::is_integral_v<U> && !::std::is_same_v<U, Byte<T, N>>)
constexpr inline bool operator> (U&& lhs, Byte<T, N>&& rhs) noexcept
{
	return (lhs > static_cast<U>(rhs.bits)); // > 0;
}

template<typename T, size_t N, typename U>
	requires (std::is_integral_v<U> && !::std::is_same_v<U, Byte<T, N>>)
constexpr inline bool operator>=(U&& lhs, Byte<T, N>&& rhs) noexcept
{
	return (lhs >= static_cast<U>(rhs.bits)); // >= 0;
}

template<typename T, size_t N, typename U>
	requires (std::is_integral_v<U> && !::std::is_same_v<U, Byte<T, N>>)
constexpr inline auto operator<=>(U&& lhs, Byte<T, N>&& rhs) noexcept
{
	return lhs <=> static_cast<U>(rhs.bits);
}

//==============================================================================
// bitwise (assignment) by const-ref
template<typename T, size_t N, typename U>
	requires (std::is_integral_v<U> && !::std::is_same_v<U, Byte<T, N>>)
constexpr inline U& operator|=(U& lhs, const Byte<T, N>& rhs) noexcept
{
	return lhs |= static_cast<U>(rhs.bits);
}

template<typename T, size_t N, typename U>
	requires (std::is_integral_v<U> && !::std::is_same_v<U, Byte<T, N>>)
constexpr inline U& operator&=(U& lhs, const Byte<T, N>& rhs) noexcept
{
	return lhs &= static_cast<U>(rhs.bits);
}

template<typename T, size_t N, typename U>
	requires (std::is_integral_v<U> && !::std::is_same_v<U, Byte<T, N>>)
constexpr inline U& operator^=(U& lhs, const Byte<T, N>& rhs) noexcept
{
	return lhs ^= static_cast<U>(rhs.bits);
}

// bitwise (assignment) by forwarding-ref
template<typename T, size_t N, typename U>
	requires (std::is_integral_v<U> && !::std::is_same_v<U, Byte<T, N>>)
constexpr inline U& operator|=(U&& lhs, Byte<T, N>&& rhs) noexcept
{
	return lhs |= static_cast<U>(rhs.bits);
}

template<typename T, size_t N, typename U>
	requires (std::is_integral_v<U> && !::std::is_same_v<U, Byte<T, N>>)
constexpr inline U& operator&=(U&& lhs, Byte<T, N>&& rhs) noexcept
{
	return lhs &= static_cast<U>(rhs.bits);
}

template<typename T, size_t N, typename U>
	requires (std::is_integral_v<U> && !::std::is_same_v<U, Byte<T, N>>)
constexpr inline U& operator^=(U&& lhs, Byte<T, N>&& rhs) noexcept
{
	return lhs ^= static_cast<U>(rhs.bits);
}

//==============================================================================
// arithmetic (assignment) by const-ref
template<typename T, size_t N, typename U>
	requires (std::is_integral_v<U> && !::std::is_same_v<U, Byte<T, N>>)
constexpr inline U& operator+=( U& lhs, const Byte<T, N>& rhs) noexcept
{
	return lhs += static_cast<U>(rhs.bits);
}

template<typename T, size_t N, typename U>
	requires (std::is_integral_v<U> && !::std::is_same_v<U, Byte<T, N>>)
constexpr inline U& operator-=( U& lhs, const Byte<T, N>& rhs) noexcept
{
	return lhs -= static_cast<U>(rhs.bits);
}

template<typename T, size_t N, typename U>
	requires (std::is_integral_v<U> && !::std::is_same_v<U, Byte<T, N>>)
constexpr inline U& operator*=( U& lhs, const Byte<T, N>& rhs) noexcept
{
	return lhs *= static_cast<U>(rhs.bits);
}

template<typename T, size_t N, typename U>
	requires (std::is_integral_v<U> && !::std::is_same_v<U, Byte<T, N>>)
constexpr inline U& operator/=( U& lhs, const Byte<T, N>& rhs) noexcept
{
	return lhs /= static_cast<U>(rhs.bits);
}

template<typename T, size_t N, typename U>
	requires (std::is_integral_v<U> && !::std::is_same_v<U, Byte<T, N>>)
constexpr inline U& operator%=( U& lhs, const Byte<T, N>& rhs) noexcept
{
	return lhs %= static_cast<U>(rhs.bits);
}

// arithmetic (assignment) by forwarding-ref
template<typename T, size_t N, typename U>
	requires (std::is_integral_v<U> && !::std::is_same_v<U, Byte<T, N>>)
constexpr inline U& operator+=(U& lhs, Byte<T, N>&& rhs) noexcept
{
	return lhs += static_cast<U>(rhs.bits);
}

template<typename T, size_t N, typename U>
	requires (std::is_integral_v<U> && !::std::is_same_v<U, Byte<T, N>>)
constexpr inline U& operator-=(U& lhs, Byte<T, N>&& rhs) noexcept
{
	return lhs -= static_cast<U>(rhs.bits);
}

template<typename T, size_t N, typename U>
	requires (std::is_integral_v<U> && !::std::is_same_v<U, Byte<T, N>>)
constexpr inline U& operator*=(U& lhs, Byte<T, N>&& rhs) noexcept
{
	return lhs *= static_cast<U>(rhs.bits);
}

template<typename T, size_t N, typename U>
	requires (std::is_integral_v<U> && !::std::is_same_v<U, Byte<T, N>>)
constexpr inline U& operator/=(U& lhs, Byte<T, N>&& rhs) noexcept
{
	return lhs /= static_cast<U>(rhs.bits);
}

template<typename T, size_t N, typename U>
	requires (std::is_integral_v<U> && !::std::is_same_v<U, Byte<T, N>>)
constexpr inline U& operator%=(U& lhs, Byte<T, N>&& rhs) noexcept
{
	return lhs %= static_cast<U>(rhs.bits);
}

//==============================================================================
// bitwise (non-assignment) by const-ref
template<typename T, size_t N, typename U>
	requires (std::is_integral_v<U> && !::std::is_same_v<U, Byte<T, N>>)
constexpr inline U operator|(const U& lhs, const Byte<T, N>& rhs) noexcept
{
	return lhs | static_cast<U>(rhs.bits);
}

template<typename T, size_t N, typename U>
	requires (std::is_integral_v<U> && !::std::is_same_v<U, Byte<T, N>>)
constexpr inline U operator&(const U& lhs, const Byte<T, N>& rhs) noexcept
{
	return lhs & static_cast<U>(rhs.bits);
}

template<typename T, size_t N, typename U>
	requires (std::is_integral_v<U> && !::std::is_same_v<U, Byte<T, N>>)
constexpr inline U operator^(const U& lhs, const Byte<T, N>& rhs) noexcept
{
	return lhs ^ static_cast<U>(rhs.bits);
}

// bitwise (non-assignment) by forwarding-ref
template<typename T, size_t N, typename U>
	requires (std::is_integral_v<U> && !::std::is_same_v<U, Byte<T, N>>)
constexpr inline U operator|(U&& lhs, Byte<T, N>&& rhs) noexcept
{
	return lhs | static_cast<U>(rhs.bits);
}

template<typename T, size_t N, typename U>
	requires (std::is_integral_v<U> && !::std::is_same_v<U, Byte<T, N>>)
constexpr inline U operator&(U&& lhs, Byte<T, N>&& rhs) noexcept
{
	return lhs & static_cast<U>(rhs.bits);
}

template<typename T, size_t N, typename U>
	requires (std::is_integral_v<U> && !::std::is_same_v<U, Byte<T, N>>)
constexpr inline U operator^(U&& lhs, Byte<T, N>&& rhs) noexcept
{
	return lhs ^ static_cast<U>(rhs.bits);
}

//==============================================================================
// arithmetic (non-assignment) by const-ref
template<typename T, size_t N, typename U>
	requires (std::is_integral_v<U> && !::std::is_same_v<U, Byte<T, N>>)
constexpr inline U operator+(const U& lhs, const Byte<T, N>& rhs) noexcept
{
	return lhs + static_cast<U>(rhs.bits);
}

template<typename T, size_t N, typename U>
	requires (std::is_integral_v<U> && !::std::is_same_v<U, Byte<T, N>>)
constexpr inline U operator-(const U& lhs, const Byte<T, N>& rhs) noexcept
{
	return lhs - static_cast<U>(rhs.bits);
}

template<typename T, size_t N, typename U>
	requires (std::is_integral_v<U> && !::std::is_same_v<U, Byte<T, N>>)
constexpr inline U operator*(const U& lhs, const Byte<T, N>& rhs) noexcept
{
	return lhs * static_cast<U>(rhs.bits);
}

template<typename T, size_t N, typename U>
	requires (std::is_integral_v<U> && !::std::is_same_v<U, Byte<T, N>>)
constexpr inline U operator/(const U& lhs, const Byte<T, N>& rhs) noexcept
{
	return lhs / static_cast<U>(rhs.bits);
}

template<typename T, size_t N, typename U>
	requires (std::is_integral_v<U> && !::std::is_same_v<U, Byte<T, N>>)
constexpr inline U operator%(const U& lhs, const Byte<T, N>& rhs) noexcept
{
	return lhs % static_cast<U>(rhs.bits);
}

// arithmetic (non-assignment) by forwarding-ref
template<typename T, size_t N, typename U>
	requires (std::is_integral_v<U> && !::std::is_same_v<U, Byte<T, N>>)
constexpr inline U operator+(U&& lhs, Byte<T, N>&& rhs) noexcept
{
	return lhs + static_cast<U>(rhs.bits);
}

template<typename T, size_t N, typename U>
	requires (std::is_integral_v<U> && !::std::is_same_v<U, Byte<T, N>>)
constexpr inline U operator-(U&& lhs, Byte<T, N>&& rhs) noexcept
{
	return lhs - static_cast<U>(rhs.bits);
}

template<typename T, size_t N, typename U>
	requires (std::is_integral_v<U> && !::std::is_same_v<U, Byte<T, N>>)
constexpr inline U operator*(U&& lhs, Byte<T, N>&& rhs) noexcept
{
	return lhs * static_cast<U>(rhs.bits);
}

template<typename T, size_t N, typename U>
	requires (std::is_integral_v<U> && !::std::is_same_v<U, Byte<T, N>>)
constexpr inline U operator/(U&& lhs, Byte<T, N>&& rhs) noexcept
{
	return lhs / static_cast<U>(rhs.bits);
}

template<typename T, size_t N, typename U>
	requires (std::is_integral_v<U> && !::std::is_same_v<U, Byte<T, N>>)
constexpr inline U operator%(U&& lhs, Byte<T, N>&& rhs) noexcept
{
	return lhs % static_cast<U>(rhs.bits);
}
//==============================================================================
	/* #endregion BoilerPlate */
