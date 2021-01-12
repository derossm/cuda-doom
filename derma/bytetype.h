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

template<typename T, size_t N = 1>
	requires std::is_integral_v<T>
class Byte
{
public:
	T bits;
	//::std::array<::std::byte, 8> byteArray;

	using type = T;
	Byte() : bits{0x00} {}
	Byte(T bits) : bits{bits} {}
	//Byte(::std::byte byte) : bits{byte} {}

	// add logging
	// add overflow/underflow guard/checking
	// add support for narrowing to the smallest required size
	// add support for narrowing to a fixed size with intentional data loss

	// BOILER PLATE FROM HERE TO END
	/* #region BoilerPlate */
	constexpr operator size_t() const noexcept
	{
		return static_cast<size_t>(bits);
	}

	//=====
	// unary
	constexpr inline bool operator!() const noexcept
	{
		return static_cast<bool>(bits) != 0;
	}

	constexpr inline Byte<T, N>& operator~() noexcept
	{
		bits = ~static_cast<T>(bits);
		//bits = ~bits;
		return *this;
	}

	constexpr inline Byte<T, N>& operator++() noexcept
	{
		bits = static_cast<T>(bits) + 1;
		//++bits;
		return *this;
	}

	constexpr inline Byte<T, N>& operator--() noexcept
	{
		bits = static_cast<T>(bits) - 1;
		//--bits;
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

	//=====
	// shift (assignment) by const-ref
	template<typename U>
		requires (std::is_integral_v<U> || ::std::is_same_v<U, Byte<T, N>> || ::std::is_class_v<U>)
	constexpr inline Byte<T, N>& operator>>=(const U& other) noexcept
	{
		if constexpr (::std::is_same_v<U, Byte<T, N>> || ::std::is_class_v<U>)
		{
			bits = static_cast<T>(static_cast<T>(bits) >> static_cast<T>(other.bits));
			return *this;
		}

		bits = static_cast<T>(static_cast<U>(bits) >> other);
		return *this;
	}

	template<typename U>
		requires (std::is_integral_v<U> || ::std::is_same_v<U, Byte<T, N>> || ::std::is_class_v<U>)
	constexpr inline Byte<T, N>& operator<<=(const U& other) noexcept
	{
		if constexpr (::std::is_same_v<U, Byte<T, N>> || ::std::is_class_v<U>)
		{
			bits = static_cast<T>(static_cast<T>(bits) << static_cast<T>(other.bits));
			return *this;
		}

		bits = static_cast<T>(static_cast<U>(bits) << other);
		return *this;
	}

	// shift (assignment) by forwarding-ref
	template<typename U>
		requires (std::is_integral_v<U> || ::std::is_same_v<U, Byte<T, N>> || ::std::is_class_v<U>)
	constexpr inline Byte<T, N>& operator<<=(U&& other) noexcept
	{
		if constexpr (::std::is_same_v<U, Byte<T, N>> || ::std::is_class_v<U>)
		{
			bits = static_cast<T>(static_cast<T>(bits) << static_cast<T>(other.bits));
			return *this;
		}

		bits = static_cast<T>(static_cast<U>(bits) << other);
		return *this;
	}

	template<typename U>
		requires (std::is_integral_v<U> || ::std::is_same_v<U, Byte<T, N>> || ::std::is_class_v<U>)
	constexpr inline Byte<T, N>& operator>>=(U&& other) noexcept
	{
		if constexpr (::std::is_same_v<U, Byte<T, N>> || ::std::is_class_v<U>)
		{
			bits = static_cast<T>(static_cast<T>(bits) >> static_cast<T>(other.bits));
			return *this;
		}

		bits = static_cast<T>(static_cast<U>(bits) >> other);
		return *this;
	}

	// shift (non-assignment) by const-ref
	template<typename U>
		requires (std::is_integral_v<U> || ::std::is_same_v<U, Byte<T, N>> || ::std::is_class_v<U>)
	constexpr inline U operator<<(const U& other) const noexcept
	{
		if constexpr (::std::is_same_v<U, Byte<T, N>> || ::std::is_class_v<U>)
		{
			return static_cast<T>(bits) << static_cast<T>(other.bits);
		}

		return static_cast<U>(bits) << other;
	}

	template<typename U>
		requires (std::is_integral_v<U> || ::std::is_same_v<U, Byte<T, N>> || ::std::is_class_v<U>)
	constexpr inline U operator>>(const U& other) const noexcept
	{
		if constexpr (::std::is_same_v<U, Byte<T, N>> || ::std::is_class_v<U>)
		{
			return static_cast<T>(bits) >> static_cast<T>(other.bits);
		}

		return static_cast<U>(bits) >> other;
	}

	// shift (non-assignment) by forwarding-ref
	template<typename U>
		requires (std::is_integral_v<U> || ::std::is_same_v<U, Byte<T, N>> || ::std::is_class_v<U>)
	constexpr inline U operator<<(U&& other) const noexcept
	{
		if constexpr (::std::is_same_v<U, Byte<T, N>> || ::std::is_class_v<U>)
		{
			return static_cast<T>(bits) << static_cast<T>(other.bits);
		}

		return static_cast<U>(bits) << other;
	}

	template<typename U>
		requires (std::is_integral_v<U> || ::std::is_same_v<U, Byte<T, N>> || ::std::is_class_v<U>)
	constexpr inline U operator>>(U&& other) const noexcept
	{
		if constexpr (::std::is_same_v<U, Byte<T, N>> || ::std::is_class_v<U>)
		{
			return static_cast<T>(bits) >> static_cast<T>(other.bits);
		}

		return static_cast<U>(bits) >> other;
	}

	//=====
	// comparison by const-ref
	template<typename U>
		requires (std::is_integral_v<U> || ::std::is_same_v<U, Byte<T, N>> || ::std::is_class_v<U>)
	constexpr inline bool operator==(const U& rhs) const noexcept
	{
		if constexpr (::std::is_same_v<U, Byte<T, N>> || ::std::is_class_v<U>)
		{
			return (static_cast<T>(bits) == static_cast<T>(rhs.bits)) == 0;
		}

		return (static_cast<U>(bits) == rhs) == 0;
	}

	template<typename U>
		requires (std::is_integral_v<U> || ::std::is_same_v<U, Byte<T, N>> || ::std::is_class_v<U>)
	constexpr inline bool operator!=(const U& rhs) const noexcept
	{
		if constexpr (::std::is_same_v<U, Byte<T, N>> || ::std::is_class_v<U>)
		{
			return (static_cast<T>(bits) != static_cast<T>(rhs.bits)) != 0;
		}

		return (static_cast<U>(bits) != rhs) != 0;
	}

	template<typename U>
		requires (std::is_integral_v<U> || ::std::is_same_v<U, Byte<T, N>> || ::std::is_class_v<U>)
	constexpr inline bool operator< (const U& rhs) const noexcept
	{
		if constexpr (::std::is_same_v<U, Byte<T, N>> || ::std::is_class_v<U>)
		{
			return (static_cast<T>(bits) < static_cast<T>(rhs.bits)) < 0;
		}

		return (static_cast<U>(bits) <  rhs) <  0;
	}

	template<typename U>
		requires (std::is_integral_v<U> || ::std::is_same_v<U, Byte<T, N>> || ::std::is_class_v<U>)
	constexpr inline bool operator<=(const U& rhs) const noexcept
	{
		if constexpr (::std::is_same_v<U, Byte<T, N>> || ::std::is_class_v<U>)
		{
			return (static_cast<T>(bits) <= static_cast<T>(rhs.bits)) <= 0;
		}

		return (static_cast<U>(bits) <= rhs) <= 0;
	}

	template<typename U>
		requires (std::is_integral_v<U> || ::std::is_same_v<U, Byte<T, N>> || ::std::is_class_v<U>)
	constexpr inline bool operator> (const U& rhs) const noexcept
	{
		if constexpr (::std::is_same_v<U, Byte<T, N>> || ::std::is_class_v<U>)
		{
			return (static_cast<T>(bits) > static_cast<T>(rhs.bits)) > 0;
		}

		return (static_cast<U>(bits) >  rhs) >  0;
	}

	template<typename U>
		requires (std::is_integral_v<U> || ::std::is_same_v<U, Byte<T, N>> || ::std::is_class_v<U>)
	constexpr inline bool operator>=(const U& rhs) const noexcept
	{
		if constexpr (::std::is_same_v<U, Byte<T, N>> || ::std::is_class_v<U>)
		{
			return (static_cast<T>(bits) >= static_cast<T>(rhs.bits)) >= 0;
		}

		return (static_cast<U>(bits) >= rhs) >= 0;
	}

	template<typename U>
		requires (std::is_integral_v<U> || ::std::is_same_v<U, Byte<T, N>> || ::std::is_class_v<U>)
	constexpr inline auto operator<=>(const U& rhs) const noexcept
	{
		if constexpr (::std::is_same_v<U, Byte<T, N>> || ::std::is_class_v<U>)
		{
			return static_cast<T>(bits) <=> static_cast<T>(rhs.bits);
		}

		return static_cast<U>(bits) <=> rhs;
	}

	template<typename U>
		requires (std::is_integral_v<U> || ::std::is_same_v<U, Byte<T, N>> || ::std::is_class_v<U>)
	constexpr inline bool operator||(const U& rhs) const noexcept
	{
		if constexpr (::std::is_same_v<U, Byte<T, N>> || ::std::is_class_v<U>)
		{
			return (static_cast<T>(bits) || static_cast<T>(rhs.bits));
		}

		return (static_cast<U>(bits) || rhs);
	}

	template<typename U>
		requires (std::is_integral_v<U> || ::std::is_same_v<U, Byte<T, N>> || ::std::is_class_v<U>)
	constexpr inline bool operator&&(const U& rhs) const noexcept
	{
		if constexpr (::std::is_same_v<U, Byte<T, N>> || ::std::is_class_v<U>)
		{
			return (static_cast<T>(bits) && static_cast<T>(rhs.bits));
		}

		return (static_cast<U>(bits) && rhs);
	}

	// comparison by forwarding-ref
	template<typename U>
		requires (std::is_integral_v<U> || ::std::is_same_v<U, Byte<T, N>> || ::std::is_class_v<U>)
	constexpr inline bool operator==(U&& rhs) const noexcept
	{
		if constexpr (::std::is_same_v<U, Byte<T, N>> || ::std::is_class_v<U>)
		{
			return (static_cast<T>(bits) == static_cast<T>(rhs.bits)) == 0;
		}

		return (static_cast<U>(bits) == rhs) == 0;
	}

	template<typename U>
		requires (std::is_integral_v<U> || ::std::is_same_v<U, Byte<T, N>> || ::std::is_class_v<U>)
	constexpr inline bool operator!=(U&& rhs) const noexcept
	{
		if constexpr (::std::is_same_v<U, Byte<T, N>> || ::std::is_class_v<U>)
		{
			return (static_cast<T>(bits) != static_cast<T>(rhs.bits)) != 0;
		}

		return (static_cast<U>(bits) != rhs) != 0;
	}

	template<typename U>
		requires (std::is_integral_v<U> || ::std::is_same_v<U, Byte<T, N>> || ::std::is_class_v<U>)
	constexpr inline bool operator< (U&& rhs) const noexcept
	{
		if constexpr (::std::is_same_v<U, Byte<T, N>> || ::std::is_class_v<U>)
		{
			return (static_cast<T>(bits) < static_cast<T>(rhs.bits)) < 0;
		}

		return (static_cast<U>(bits) <  rhs) <  0;
	}

	template<typename U>
		requires (std::is_integral_v<U> || ::std::is_same_v<U, Byte<T, N>> || ::std::is_class_v<U>)
	constexpr inline bool operator<=(U&& rhs) const noexcept
	{
		if constexpr (::std::is_same_v<U, Byte<T, N>> || ::std::is_class_v<U>)
		{
			return (static_cast<T>(bits) <= static_cast<T>(rhs.bits)) <= 0;
		}

		return (static_cast<U>(bits) <= rhs) <= 0;
	}

	template<typename U>
		requires (std::is_integral_v<U> || ::std::is_same_v<U, Byte<T, N>> || ::std::is_class_v<U>)
	constexpr inline bool operator> (U&& rhs) const noexcept
	{
		if constexpr (::std::is_same_v<U, Byte<T, N>> || ::std::is_class_v<U>)
		{
			return (static_cast<T>(bits) > static_cast<T>(rhs.bits)) > 0;
		}

		return (static_cast<U>(bits) >  rhs) >  0;
	}

	template<typename U>
		requires (std::is_integral_v<U> || ::std::is_same_v<U, Byte<T, N>> || ::std::is_class_v<U>)
	constexpr inline bool operator>=(U&& rhs) const noexcept
	{
		if constexpr (::std::is_same_v<U, Byte<T, N>> || ::std::is_class_v<U>)
		{
			return (static_cast<T>(bits) >= static_cast<T>(rhs.bits)) >= 0;
		}

		return (static_cast<U>(bits) >= rhs) >= 0;
	}

	template<typename U>
		requires (std::is_integral_v<U> || ::std::is_same_v<U, Byte<T, N>> || ::std::is_class_v<U>)
	constexpr inline auto operator<=>(U&& rhs) const noexcept
	{
		if constexpr (::std::is_same_v<U, Byte<T, N>> || ::std::is_class_v<U>)
		{
			return static_cast<T>(bits) <=> static_cast<T>(rhs.bits);
		}

		return static_cast<U>(bits) <=> rhs;
	}

	template<typename U>
		requires (std::is_integral_v<U> || ::std::is_same_v<U, Byte<T, N>> || ::std::is_class_v<U>)
	constexpr inline bool operator||(U&& rhs) const noexcept
	{
		if constexpr (::std::is_same_v<U, Byte<T, N>> || ::std::is_class_v<U>)
		{
			return (static_cast<T>(bits) || static_cast<T>(rhs.bits));
		}

		return (static_cast<U>(bits) || rhs);
	}

	template<typename U>
		requires (std::is_integral_v<U> || ::std::is_same_v<U, Byte<T, N>> || ::std::is_class_v<U>)
	constexpr inline bool operator&&(U&& rhs) const noexcept
	{
		if constexpr (::std::is_same_v<U, Byte<T, N>> || ::std::is_class_v<U>)
		{
			return (static_cast<T>(bits) && static_cast<T>(rhs.bits));
		}

		return (static_cast<U>(bits) && rhs);
	}

	//=====
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

			bits = static_cast<T>(other.bits);
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

			bits = static_cast<T>(other.bits);
			return *this;
		}

		bits = static_cast<T>(other);
		return *this;
	}

	//=====
	// bitwise (assignment) by const-ref
	template<typename U>
		requires (std::is_integral_v<U> || ::std::is_same_v<U, Byte<T, N>> || ::std::is_class_v<U>)
	constexpr inline U operator|(const U& other) const noexcept
	{
		if constexpr (::std::is_same_v<U, Byte<T, N>> || ::std::is_class_v<U>)
		{
			return static_cast<T>(bits) | static_cast<T>(other.bits);
		}

		return static_cast<U>(bits) | other;
	}

	template<typename U>
		requires (std::is_integral_v<U> || ::std::is_same_v<U, Byte<T, N>> || ::std::is_class_v<U>)
	constexpr inline U operator&(const U& other) const noexcept
	{
		if constexpr (::std::is_same_v<U, Byte<T, N>> || ::std::is_class_v<U>)
		{
			return static_cast<T>(bits) & static_cast<T>(other.bits);
		}

		return static_cast<U>(bits) & other;
	}

	template<typename U>
		requires (std::is_integral_v<U> || ::std::is_same_v<U, Byte<T, N>> || ::std::is_class_v<U>)
	constexpr inline U operator^(const U& other) const noexcept
	{
		if constexpr (::std::is_same_v<U, Byte<T, N>> || ::std::is_class_v<U>)
		{
			return static_cast<T>(bits) ^ static_cast<T>(other.bits);
		}

		return static_cast<U>(bits) ^ other;
	}

	// bitwise (non-assignment) by forwarding-ref
	template<typename U>
		requires (std::is_integral_v<U> || ::std::is_same_v<U, Byte<T, N>> || ::std::is_class_v<U>)
	constexpr inline U operator|(U&& other) const noexcept
	{
		if constexpr (::std::is_same_v<U, Byte<T, N>> || ::std::is_class_v<U>)
		{
			return static_cast<T>(bits) | static_cast<T>(other.bits);
		}

		return static_cast<U>(bits) | other;
	}

	template<typename U>
		requires (std::is_integral_v<U> || ::std::is_same_v<U, Byte<T, N>> || ::std::is_class_v<U>)
	constexpr inline U operator&(U&& other) const noexcept
	{
		if constexpr (::std::is_same_v<U, Byte<T, N>> || ::std::is_class_v<U>)
		{
			return static_cast<T>(bits) & static_cast<T>(other.bits);
		}

		return static_cast<U>(bits) & other;
	}

	template<typename U>
		requires (std::is_integral_v<U> || ::std::is_same_v<U, Byte<T, N>> || ::std::is_class_v<U>)
	constexpr inline U operator^(U&& other) const noexcept
	{
		if constexpr (::std::is_same_v<U, Byte<T, N>> || ::std::is_class_v<U>)
		{
			return static_cast<T>(bits) ^ static_cast<T>(other.bits);
		}

		return static_cast<U>(bits) ^ other;
	}

	//=====
	// arithmetic (non-assignment) by const-ref
	template<typename U>
		requires (std::is_integral_v<U> || ::std::is_same_v<U, Byte<T, N>> || ::std::is_class_v<U>)
	constexpr inline U operator+(const U& other) const noexcept
	{
		if constexpr (::std::is_same_v<U, Byte<T, N>> || ::std::is_class_v<U>)
		{
			return static_cast<T>(bits) + static_cast<T>(other.bits);
		}

		return static_cast<U>(bits) + other;
	}

	template<typename U>
		requires (std::is_integral_v<U> || ::std::is_same_v<U, Byte<T, N>> || ::std::is_class_v<U>)
	constexpr inline U operator-(const U& other) const noexcept
	{
		if constexpr (::std::is_same_v<U, Byte<T, N>> || ::std::is_class_v<U>)
		{
			return static_cast<T>(bits) - static_cast<T>(other.bits);
		}

		return static_cast<U>(bits) - other;
	}

	template<typename U>
		requires (std::is_integral_v<U> || ::std::is_same_v<U, Byte<T, N>> || ::std::is_class_v<U>)
	constexpr inline U operator*(const U& other) const noexcept
	{
		if constexpr (::std::is_same_v<U, Byte<T, N>> || ::std::is_class_v<U>)
		{
			return static_cast<T>(bits) * static_cast<T>(other.bits);
		}

		return static_cast<U>(bits) * other;
	}

	template<typename U>
		requires (std::is_integral_v<U> || ::std::is_same_v<U, Byte<T, N>> || ::std::is_class_v<U>)
	constexpr inline U operator/(const U& other) const noexcept
	{
		if constexpr (::std::is_same_v<U, Byte<T, N>> || ::std::is_class_v<U>)
		{
			return static_cast<T>(bits) / static_cast<T>(other.bits);
		}

		return static_cast<U>(bits) / other;
	}

	template<typename U>
		requires (std::is_integral_v<U> || ::std::is_same_v<U, Byte<T, N>> || ::std::is_class_v<U>)
	constexpr inline U operator%(const U& other) const noexcept
	{
		if constexpr (::std::is_same_v<U, Byte<T, N>> || ::std::is_class_v<U>)
		{
			return static_cast<T>(bits) / static_cast<T>(other.bits);
		}

		return static_cast<U>(bits) % other;
	}

	// arithmetic (non-assignment) by forwarding-ref
	template<typename U>
		requires (std::is_integral_v<U> || ::std::is_same_v<U, Byte<T, N>> || ::std::is_class_v<U>)
	constexpr inline U operator+(U&& other) const noexcept
	{
		if constexpr (::std::is_same_v<U, Byte<T, N>> || ::std::is_class_v<U>)
		{
			return static_cast<T>(bits) + static_cast<T>(other.bits);
		}

		return static_cast<U>(bits) + other;
	}

	template<typename U>
		requires (std::is_integral_v<U> || ::std::is_same_v<U, Byte<T, N>> || ::std::is_class_v<U>)
	constexpr inline U operator-(U&& other) const noexcept
	{
		if constexpr (::std::is_same_v<U, Byte<T, N>> || ::std::is_class_v<U>)
		{
			return static_cast<T>(bits) - static_cast<T>(other.bits);
		}

		return static_cast<U>(bits) - other;
	}

	template<typename U>
		requires (std::is_integral_v<U> || ::std::is_same_v<U, Byte<T, N>> || ::std::is_class_v<U>)
	constexpr inline U operator*(U&& other) const noexcept
	{
		if constexpr (::std::is_same_v<U, Byte<T, N>> || ::std::is_class_v<U>)
		{
			return static_cast<T>(bits) * static_cast<T>(other.bits);
		}

		return static_cast<U>(bits) * other;
	}

	template<typename U>
		requires (std::is_integral_v<U> || ::std::is_same_v<U, Byte<T, N>> || ::std::is_class_v<U>)
	constexpr inline U operator/(U&& other) const noexcept
	{
		if constexpr (::std::is_same_v<U, Byte<T, N>> || ::std::is_class_v<U>)
		{
			return static_cast<T>(bits) / static_cast<T>(other.bits);
		}

		return static_cast<U>(bits) / other;
	}

	template<typename U>
		requires (std::is_integral_v<U> || ::std::is_same_v<U, Byte<T, N>> || ::std::is_class_v<U>)
	constexpr inline U operator%(U&& other) const noexcept
	{
		if constexpr (::std::is_same_v<U, Byte<T, N>> || ::std::is_class_v<U>)
		{
			return static_cast<T>(bits) / static_cast<T>(other.bits);
		}

		return static_cast<U>(bits) % other;
	}

	//=====
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

	// arithmetic (assignment) by const-ref
	template<typename U>
		requires (std::is_integral_v<U> || ::std::is_same_v<U, Byte<T, N>> || ::std::is_class_v<U>)
	constexpr inline Byte<T, N>& operator+=(const U& other) noexcept
	{
		if constexpr (::std::is_same_v<U, Byte<T, N>> || ::std::is_class_v<U>)
		{
			bits = static_cast<T>(static_cast<T>(bits) + static_cast<T>(other.bits));
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
			bits = static_cast<T>(static_cast<T>(bits) - static_cast<T>(other.bits));
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
			bits = static_cast<T>(static_cast<T>(bits) * static_cast<T>(other.bits));
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
			bits = static_cast<T>(static_cast<T>(bits) / static_cast<T>(other.bits));
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
			bits = static_cast<T>(static_cast<T>(bits) % static_cast<T>(other.bits));
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
			bits = static_cast<T>(static_cast<T>(bits) + static_cast<T>(other.bits));
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
			bits = static_cast<T>(static_cast<T>(bits) - static_cast<T>(other.bits));
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
			bits = static_cast<T>(static_cast<T>(bits) * static_cast<T>(other.bits));
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
			bits = static_cast<T>(static_cast<T>(bits) / static_cast<T>(other.bits));
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
			bits = static_cast<T>(static_cast<T>(bits) % static_cast<T>(other.bits));
			return *this;
		}

		bits = static_cast<T>(static_cast<U>(bits) % other);
		return *this;
	}
//==============================================================================
// friends

	//=====
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
	template<typename T, size_t N, typename U>
		requires (std::is_integral_v<U> && !::std::is_same_v<U, Byte<T, N>>)
	friend constexpr inline bool operator||(const U& lhs, const Byte<T, N>& rhs) noexcept;
	template<typename T, size_t N, typename U>
		requires (std::is_integral_v<U> && !::std::is_same_v<U, Byte<T, N>>)
	friend constexpr inline bool operator&&(const U& lhs, const Byte<T, N>& rhs) noexcept;

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
	template<typename T, size_t N, typename U>
		requires (std::is_integral_v<U> && !::std::is_same_v<U, Byte<T, N>>)
	friend constexpr inline bool operator||(U&& lhs, Byte<T, N>&& rhs) noexcept;
	template<typename T, size_t N, typename U>
		requires (std::is_integral_v<U> && !::std::is_same_v<U, Byte<T, N>>)
	friend constexpr inline bool operator&&(U&& lhs, Byte<T, N>&& rhs) noexcept;

	//=====
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

	//=====
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

	//=====
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

	//=====
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
};

//=====
// comparison by const-ref
template<typename T, size_t N, typename U>
	requires (std::is_integral_v<U> && !::std::is_same_v<U, Byte<T, N>>)
constexpr inline bool operator==(const U& lhs, const Byte<T, N>& rhs) noexcept
{
	return (lhs == static_cast<U>(rhs.bits)) == 0;
}

template<typename T, size_t N, typename U>
	requires (std::is_integral_v<U> && !::std::is_same_v<U, Byte<T, N>>)
constexpr inline bool operator!=(const U& lhs, const Byte<T, N>& rhs) noexcept
{
	return (lhs != static_cast<U>(rhs.bits)) != 0;
}

template<typename T, size_t N, typename U>
	requires (std::is_integral_v<U> && !::std::is_same_v<U, Byte<T, N>>)
constexpr inline bool operator< (const U& lhs, const Byte<T, N>& rhs) noexcept
{
	return (lhs < static_cast<U>(rhs.bits)) < 0;
}

template<typename T, size_t N, typename U>
	requires (std::is_integral_v<U> && !::std::is_same_v<U, Byte<T, N>>)
constexpr inline bool operator<=(const U& lhs, const Byte<T, N>& rhs) noexcept
{
	return (lhs <= static_cast<U>(rhs.bits)) <= 0;
}

template<typename T, size_t N, typename U>
	requires (std::is_integral_v<U> && !::std::is_same_v<U, Byte<T, N>>)
constexpr inline bool operator> (const U& lhs, const Byte<T, N>& rhs) noexcept
{
	return (lhs > static_cast<U>(rhs.bits)) > 0;
}

template<typename T, size_t N, typename U>
	requires (std::is_integral_v<U> && !::std::is_same_v<U, Byte<T, N>>)
constexpr inline bool operator>=(const U& lhs, const Byte<T, N>& rhs) noexcept
{
	return (lhs >= static_cast<U>(rhs.bits)) >= 0;
}

template<typename T, size_t N, typename U>
	requires (std::is_integral_v<U> && !::std::is_same_v<U, Byte<T, N>>)
constexpr inline auto operator<=>(const U& lhs, const Byte<T, N>& rhs) noexcept
{
	return lhs <=> static_cast<U>(rhs.bits);
}

template<typename T, size_t N, typename U>
	requires (std::is_integral_v<U> && !::std::is_same_v<U, Byte<T, N>>)
constexpr inline bool operator||(const U& lhs, const Byte<T, N>& rhs) noexcept
{
	return (lhs || static_cast<U>(rhs.bits));
}

template<typename T, size_t N, typename U>
	requires (std::is_integral_v<U> && !::std::is_same_v<U, Byte<T, N>>)
constexpr inline bool operator&&(const U& lhs, const Byte<T, N>& rhs) noexcept
{
	return (lhs && static_cast<U>(rhs.bits));
}

// comparison by forwarding-ref
template<typename T, size_t N, typename U>
	requires (std::is_integral_v<U> && !::std::is_same_v<U, Byte<T, N>>)
constexpr inline bool operator==(U&& lhs, Byte<T, N>&& rhs) noexcept
{
	return (lhs == static_cast<U>(rhs.bits)) == 0;
}

template<typename T, size_t N, typename U>
	requires (std::is_integral_v<U> && !::std::is_same_v<U, Byte<T, N>>)
constexpr inline bool operator!=(U&& lhs, Byte<T, N>&& rhs) noexcept
{
	return (lhs != static_cast<U>(rhs.bits)) != 0;
}

template<typename T, size_t N, typename U>
	requires (std::is_integral_v<U> && !::std::is_same_v<U, Byte<T, N>>)
constexpr inline bool operator< (U&& lhs, Byte<T, N>&& rhs) noexcept
{
	return (lhs < static_cast<U>(rhs.bits)) < 0;
}

template<typename T, size_t N, typename U>
	requires (std::is_integral_v<U> && !::std::is_same_v<U, Byte<T, N>>)
constexpr inline bool operator<=(U&& lhs, Byte<T, N>&& rhs) noexcept
{
	return (lhs <= static_cast<U>(rhs.bits)) <= 0;
}

template<typename T, size_t N, typename U>
	requires (std::is_integral_v<U> && !::std::is_same_v<U, Byte<T, N>>)
constexpr inline bool operator> (U&& lhs, Byte<T, N>&& rhs) noexcept
{
	return (lhs > static_cast<U>(rhs.bits)) > 0;
}

template<typename T, size_t N, typename U>
	requires (std::is_integral_v<U> && !::std::is_same_v<U, Byte<T, N>>)
constexpr inline bool operator>=(U&& lhs, Byte<T, N>&& rhs) noexcept
{
	return (lhs >= static_cast<U>(rhs.bits)) >= 0;
}

template<typename T, size_t N, typename U>
	requires (std::is_integral_v<U> && !::std::is_same_v<U, Byte<T, N>>)
constexpr inline auto operator<=>(U&& lhs, Byte<T, N>&& rhs) noexcept
{
	return lhs <=> static_cast<U>(rhs.bits);
}

template<typename T, size_t N, typename U>
	requires (std::is_integral_v<U> && !::std::is_same_v<U, Byte<T, N>>)
constexpr inline bool operator||(U&& lhs, Byte<T, N>&& rhs) noexcept
{
	return (lhs || static_cast<U>(rhs.bits));
}

template<typename T, size_t N, typename U>
	requires (std::is_integral_v<U> && !::std::is_same_v<U, Byte<T, N>>)
constexpr inline bool operator&&(U&& lhs, Byte<T, N>&& rhs) noexcept
{
	return (lhs && static_cast<U>(rhs.bits));
}

//=====
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

//=====
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

//=====
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

//=====
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
	/* #endregion BoilerPlate */
