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

template<typename T, size_t N = 1>
	requires std::is_integral_v<T>
class Byte
{
public:
	//::std::byte bits;
	T bits;

	using type = T;
	Byte() : bits{0x00} {}
	Byte(T bits) : bits{bits} {}
	//Byte(::std::byte byte) : bits{byte} {}

	constexpr inline bool operator!() const noexcept { return static_cast<bool>(bits) != 0; }

	template<typename U>
		requires (std::is_integral_v<U> || ::std::is_same_v<U, Byte<T, N>>)
	constexpr inline bool operator==(const U& rhs) const noexcept
	{
		if constexpr (::std::is_same<U, Byte<T, N>>())
		{
			return (static_cast<T>(bits) == static_cast<T>(rhs.bits)) == 0;
		}

		return (static_cast<U>(bits) == rhs) == 0;
	}

	template<typename U>
		requires (std::is_integral_v<U> || ::std::is_same_v<U, Byte<T, N>>)
	constexpr inline bool operator!=(const U& rhs) const noexcept
	{
		if constexpr (::std::is_same<U, Byte<T, N>>())
		{
			return (static_cast<T>(bits) != static_cast<T>(rhs.bits)) != 0;
		}

		return (static_cast<U>(bits) != rhs) != 0;
	}

	template<typename U>
		requires (std::is_integral_v<U> || ::std::is_same_v<U, Byte<T, N>>)
	constexpr inline bool operator< (const U& rhs) const noexcept
	{
		if constexpr (::std::is_same<U, Byte<T, N>>())
		{
			return (static_cast<T>(bits) < static_cast<T>(rhs.bits)) < 0;
		}

		return (static_cast<U>(bits) <  rhs) <  0;
	}

	template<typename U>
		requires (std::is_integral_v<U> || ::std::is_same_v<U, Byte<T, N>>)
	constexpr inline bool operator<=(const U& rhs) const noexcept
	{
		if constexpr (::std::is_same<U, Byte<T, N>>())
		{
			return (static_cast<T>(bits) <= static_cast<T>(rhs.bits)) <= 0;
		}

		return (static_cast<U>(bits) <= rhs) <= 0;
	}

	template<typename U>
		requires (std::is_integral_v<U> || ::std::is_same_v<U, Byte<T, N>>)
	constexpr inline bool operator> (const U& rhs) const noexcept
	{
		if constexpr (::std::is_same<U, Byte<T, N>>())
		{
			return (static_cast<T>(bits) > static_cast<T>(rhs.bits)) > 0;
		}

		return (static_cast<U>(bits) >  rhs) >  0;
	}

	template<typename U>
		requires (std::is_integral_v<U> || ::std::is_same_v<U, Byte<T, N>>)
	constexpr inline bool operator>=(const U& rhs) const noexcept
	{
		if constexpr (::std::is_same<U, Byte<T, N>>())
		{
			return (static_cast<T>(bits) >= static_cast<T>(rhs.bits)) >= 0;
		}

		return (static_cast<U>(bits) >= rhs) >= 0;
	}

	template<typename U>
		requires (std::is_integral_v<U> || ::std::is_same_v<U, Byte<T, N>>)
	constexpr inline bool operator||(const U& rhs) const noexcept
	{
		if constexpr (::std::is_same<U, Byte<T, N>>())
		{
			return (static_cast<T>(bits) || static_cast<T>(rhs.bits)) || 0;
		}

		return (static_cast<U>(bits) || rhs) || 0;
	}

	template<typename U>
		requires (std::is_integral_v<U> || ::std::is_same_v<U, Byte<T, N>>)
	constexpr inline bool operator&&(const U& rhs) const noexcept
	{
		if constexpr (::std::is_same<U, Byte<T, N>>())
		{
			return (static_cast<T>(bits) && static_cast<T>(rhs.bits)) && 0;
		}

		return (static_cast<U>(bits) && rhs) && 0;
	}

	template<typename U>
		requires (std::is_integral_v<U> || ::std::is_same_v<U, Byte<T, N>>)
	constexpr inline auto operator<=>(const U& rhs) const noexcept
	{
		if constexpr (::std::is_same<U, Byte<T, N>>())
		{
			return static_cast<T>(bits) <=> static_cast<T>(rhs.bits);
		}

		return static_cast<U>(bits) <=> rhs;
	}

	template<typename U>
		requires (std::is_integral_v<U> || ::std::is_same_v<U, Byte<T, N>>)
	constexpr inline bool operator->() const noexcept {}

	//template<typename U>
		//requires (std::is_integral_v<U> || ::std::is_same_v<U, Byte<T, N>>)
	//constexpr inline bool operator->*() const noexcept {}

	template<typename U>
		requires (std::is_integral_v<U> || ::std::is_same_v<U, Byte<T, N>>)
	constexpr inline bool operator()() const noexcept {}

	//template<typename U>
		//requires (std::is_integral_v<U> || ::std::is_same_v<U, Byte<T, N>>)
	//constexpr inline bool operator[]() const noexcept {}

	//template<typename U>
		//requires (std::is_integral_v<U> || ::std::is_same_v<U, Byte<T, N>>)
	//constexpr inline bool operator, () const noexcept = delete;

//=====
// unary
	constexpr inline Byte<T, N>& operator~() noexcept
	{
		bits = static_cast<decltype(bits)>(~static_cast<T>(bits));
		//bits = ::std::byte(~static_cast<T>(bits));
		return *this;
	}

	constexpr inline Byte<T, N>& operator++() noexcept
	{
		bits = static_cast<decltype(bits)>(static_cast<T>(bits)++);
		//bits = ::std::byte(static_cast<T>(bits)++);
		return *this;
	}

	constexpr inline Byte<T, N>& operator--() noexcept
	{
		bits = static_cast<decltype(bits)>(static_cast<T>(bits)--);
		//bits = ::std::byte(static_cast<T>(bits)--);
		return *this;
	}

//=====
// assignment
	template<typename U>
		requires (std::is_integral_v<U> || ::std::is_same_v<U, Byte<T, N>>)
	constexpr inline Byte<T, N>& operator=(const U& other) noexcept
	{
		if constexpr (::std::is_same<U, Byte<T, N>>())
		{
			if (this == &other)
			{
				return *this;
			}

			bits = other.bits;
			return *this;
		}

		bits = std::byte(other);
		return *this;
	}

//=====
// binary (assignment)
	template<typename U>
		requires (std::is_integral_v<U> || ::std::is_same_v<U, Byte<T, N>>)
	constexpr inline Byte<T, N>& operator+=(const U& other) noexcept
	{
		if constexpr (::std::is_same<U, Byte<T, N>>())
		{
			bits = ::std::byte(static_cast<T>(bits) + static_cast<T>(other.bits));
			return *this;
		}

		bits = ::std::byte(static_cast<U>(bits) + other);
		return *this;
	}

	template<typename U>
		requires (std::is_integral_v<U> || ::std::is_same_v<U, Byte<T, N>>)
	constexpr inline Byte<T, N>& operator-=(const T& other) noexcept
	{
		if constexpr (::std::is_same<U, Byte<T, N>>())
		{
			bits = ::std::byte(static_cast<T>(bits) - static_cast<T>(other.bits));
			return *this;
		}

		bits = ::std::byte(static_cast<U>(bits) - other);
		return *this;
	}

	template<typename U>
		requires (std::is_integral_v<U> || ::std::is_same_v<U, Byte<T, N>>)
	constexpr inline Byte<T, N>& operator*=(const U& other) noexcept
	{
		if constexpr (::std::is_same<U, Byte<T, N>>())
		{
			bits = ::std::byte(static_cast<T>(bits) * static_cast<T>(other.bits));
			return *this;
		}

		bits = ::std::byte(static_cast<U>(bits) * other);
		return *this;
	}

	template<typename U>
		requires (std::is_integral_v<U> || ::std::is_same_v<U, Byte<T, N>>)
	constexpr inline Byte<T, N>& operator/=(const U& other) noexcept
	{
		if constexpr (::std::is_same<U, Byte<T, N>>())
		{
			bits = ::std::byte(static_cast<T>(bits) / static_cast<T>(other.bits));
			return *this;
		}

		bits = ::std::byte(static_cast<U>(bits) / other);
		return *this;
	}

	template<typename U>
		requires (std::is_integral_v<U> || ::std::is_same_v<U, Byte<T, N>>)
	constexpr inline Byte<T, N>& operator%=(const U& other) noexcept
	{
		if constexpr (::std::is_same<U, Byte<T, N>>())
		{
			bits = ::std::byte(static_cast<T>(bits) % static_cast<T>(other.bits));
			return *this;
		}

		bits = ::std::byte(static_cast<U>(bits) % other);
		return *this;
	}

	template<typename U>
		requires (std::is_integral_v<U> || ::std::is_same_v<U, Byte<T, N>>)
	constexpr inline Byte<T, N>& operator|=(const U& other) noexcept
	{
		if constexpr (::std::is_same<U, Byte<T, N>>())
		{
			bits |= other.bits;
			return *this;
		}

		bits |= ::std::byte(other);
		return *this;
	}

	template<typename U>
		requires (std::is_integral_v<U> || ::std::is_same_v<U, Byte<T, N>>)
	constexpr inline Byte<T, N>& operator&=(const U& other) noexcept
	{
		if constexpr (::std::is_same<U, Byte<T, N>>())
		{
			bits &= other.bits;
			return *this;
		}

		bits &= ::std::byte(other);
		return *this;
	}

	template<typename U>
		requires (std::is_integral_v<U> || ::std::is_same_v<U, Byte<T, N>>)
	constexpr inline Byte<T, N>& operator^=(const U& other) noexcept
	{
		if constexpr (::std::is_same<U, Byte<T, N>>())
		{
			bits ^= other.bits;
			return *this;
		}

		bits ^= ::std::byte(other);
		return *this;
	}

	template<typename U>
		requires (std::is_integral_v<U> || ::std::is_same_v<U, Byte<T, N>>)
	constexpr inline Byte<T, N>& operator<<=(const U& other) noexcept
	{
		if constexpr (::std::is_same<U, Byte<T, N>>())
		{
			return static_cast<U>(bits) << static_cast<U>(other.bits);
		}

		return static_cast<U>(bits) << other;
	}

	template<typename U>
		requires (std::is_integral_v<U> || ::std::is_same_v<U, Byte<T, N>>)
	constexpr inline Byte<T, N>& operator>>=(const U& other) noexcept
	{
		if constexpr (::std::is_same<U, Byte<T, N>>())
		{
			return static_cast<U>(bits) >> static_cast<U>(other.bits);
		}

		return static_cast<U>(bits) >> other;
	}

//=====
// binary (non-assignment)
	template<typename U>
		requires (std::is_integral_v<U> || ::std::is_same_v<U, Byte<T, N>>)
	constexpr inline U operator+(const U& other) const noexcept
	{
		if constexpr (::std::is_same<U, Byte<T, N>>())
		{
			return static_cast<T>(bits) + static_cast<T>(other.bits);
		}

		return static_cast<U>(bits) + other;
	}

	template<typename U>
		requires (std::is_integral_v<U> || ::std::is_same_v<U, Byte<T, N>>)
	constexpr inline U operator-(const U& other) const noexcept
	{
		if constexpr (::std::is_same<U, Byte<T, N>>())
		{
			return static_cast<T>(bits) - static_cast<T>(other.bits);
		}

		return static_cast<U>(bits) - other;
	}

	template<typename U>
		requires (std::is_integral_v<U> || ::std::is_same_v<U, Byte<T, N>>)
	constexpr inline U operator*(const U& other) const noexcept
	{
		if constexpr (::std::is_same<U, Byte<T, N>>())
		{
			return static_cast<T>(bits) * static_cast<T>(other.bits);
		}

		return static_cast<U>(bits) * other;
	}

	template<typename U>
		requires (std::is_integral_v<U> || ::std::is_same_v<U, Byte<T, N>>)
	constexpr inline U operator/(const U& other) const noexcept
	{
		if constexpr (::std::is_same<U, Byte<T, N>>())
		{
			return static_cast<T>(bits) / static_cast<T>(other.bits);
		}

		return static_cast<U>(bits) / other;
	}

	template<typename U>
		requires (std::is_integral_v<U> || ::std::is_same_v<U, Byte<T, N>>)
	constexpr inline U operator%(const U& other) const noexcept
	{
		if constexpr (::std::is_same<U, Byte<T, N>>())
		{
			return static_cast<T>(bits) / static_cast<T>(other.bits);
		}

		return static_cast<U>(bits) % other;
	}

	template<typename U>
		requires (std::is_integral_v<U> || ::std::is_same_v<U, Byte<T, N>>)
	constexpr inline U operator&(const U& other) const noexcept
	{
		if constexpr (::std::is_same<U, Byte<T, N>>())
		{
			return static_cast<T>(bits) & static_cast<T>(other.bits);
		}

		return static_cast<U>(bits) & other;
	}

	template<typename U>
		requires (std::is_integral_v<U> || ::std::is_same_v<U, Byte<T, N>>)
	constexpr inline U operator|(const U& other) const noexcept
	{
		if constexpr (::std::is_same<U, Byte<T, N>>())
		{
			return static_cast<T>(bits) | static_cast<T>(other.bits);
		}

		return static_cast<U>(bits) | other;
	}

	template<typename U>
		requires (std::is_integral_v<U> || ::std::is_same_v<U, Byte<T, N>>)
	constexpr inline U operator^(const U& other) const noexcept
	{
		if constexpr (::std::is_same<U, Byte<T, N>>())
		{
			return static_cast<T>(bits) ^ static_cast<T>(other.bits);
		}

		return static_cast<U>(bits) ^ other;
	}

	template<typename U>
		requires (std::is_integral_v<U> || ::std::is_same_v<U, Byte<T, N>>)
	constexpr inline U operator<<(const U& other) const noexcept
	{
		if constexpr (::std::is_same<U, Byte<T, N>>())
		{
			return static_cast<U>(bits) << static_cast<U>(other.bits);
		}

		return static_cast<U>(bits) << other;
	}

	template<typename U>
		requires (std::is_integral_v<U> || ::std::is_same_v<U, Byte<T, N>>)
	constexpr inline U operator>>(const U& other) const noexcept
	{
		if constexpr (::std::is_same<U, Byte<T, N>>())
		{
			return static_cast<U>(bits) >> static_cast<U>(other.bits);
		}

		return static_cast<U>(bits) >> other;
	}

//=====
// friends
	template<typename T, size_t N, typename U>
	friend constexpr inline bool operator==(const U& lhs, const Byte<T, N>& rhs) noexcept;
	template<typename T, size_t N, typename U>
	friend constexpr inline bool operator!=(const U& lhs, const Byte<T, N>& rhs) noexcept;
	template<typename T, size_t N, typename U>
	friend constexpr inline bool operator< (const U& lhs, const Byte<T, N>& rhs) noexcept;
	template<typename T, size_t N, typename U>
	friend constexpr inline bool operator<=(const U& lhs, const Byte<T, N>& rhs) noexcept;
	template<typename T, size_t N, typename U>
	friend constexpr inline bool operator> (const U& lhs, const Byte<T, N>& rhs) noexcept;
	template<typename T, size_t N, typename U>
	friend constexpr inline bool operator>=(const U& lhs, const Byte<T, N>& rhs) noexcept;
	template<typename T, size_t N, typename U>
	friend constexpr inline auto operator<=>(const U& lhs, const Byte<T, N>& rhs) noexcept;

	template<typename T, size_t N, typename U>
	friend constexpr inline U operator|(const U& lhs, const Byte<T, N>& rhs) noexcept;
	template<typename T, size_t N, typename U>
	friend constexpr inline U operator&(const U& lhs, const Byte<T, N>& rhs) noexcept;
	template<typename T, size_t N, typename U>
	friend constexpr inline U operator^(const U& lhs, const Byte<T, N>& rhs) noexcept;

	template<typename T, size_t N, typename U>
	friend constexpr inline U& operator|=(U& lhs, const Byte<T, N>& rhs) noexcept;
	template<typename T, size_t N, typename U>
	friend constexpr inline U& operator&=(U& lhs, const Byte<T, N>& rhs) noexcept;
	template<typename T, size_t N, typename U>
	friend constexpr inline U& operator^=(U& lhs, const Byte<T, N>& rhs) noexcept;

	template<typename T, size_t N, typename U>
	friend constexpr inline U operator+(const U& lhs, const Byte<T, N>& rhs) noexcept;
	template<typename T, size_t N, typename U>
	friend constexpr inline U operator-(const U& lhs, const Byte<T, N>& rhs) noexcept;
	template<typename T, size_t N, typename U>
	friend constexpr inline U operator*(const U& lhs, const Byte<T, N>& rhs) noexcept;
	template<typename T, size_t N, typename U>
	friend constexpr inline U operator/(const U& lhs, const Byte<T, N>& rhs) noexcept;
	template<typename T, size_t N, typename U>
	friend constexpr inline U operator%(const U& lhs, const Byte<T, N>& rhs) noexcept;

	template<typename T, size_t N, typename U>
	friend constexpr inline U& operator+=( U& lhs, const Byte<T, N>& rhs) noexcept;
	template<typename T, size_t N, typename U>
	friend constexpr inline U& operator-=( U& lhs, const Byte<T, N>& rhs) noexcept;
	template<typename T, size_t N, typename U>
	friend constexpr inline U& operator*=( U& lhs, const Byte<T, N>& rhs) noexcept;
	template<typename T, size_t N, typename U>
	friend constexpr inline U& operator/=( U& lhs, const Byte<T, N>& rhs) noexcept;
	template<typename T, size_t N, typename U>
	friend constexpr inline U& operator%=( U& lhs, const Byte<T, N>& rhs) noexcept;
};

// comparison
template<typename T, size_t N, typename U>
	requires (std::is_integral_v<U> && !::std::is_same_v<U, Byte<T, N>>)
constexpr inline bool operator==(const U& lhs, const Byte<T, N>& rhs) noexcept
{
	if constexpr (::std::is_same<U, Byte<T, N>>())
	{
		return (static_cast<T>(lhs.bits) == static_cast<T>(rhs.bits)) == 0;
	}

	return (lhs.bits == static_cast<U>(rhs.bits)) == 0;
}

template<typename T, size_t N, typename U>
	requires (std::is_integral_v<U> && !::std::is_same_v<U, Byte<T, N>>)
constexpr inline bool operator!=(const U& lhs, const Byte<T, N>& rhs) noexcept
{
	if constexpr (::std::is_same<U, Byte<T, N>>())
	{
		return (static_cast<T>(lhs.bits) != static_cast<T>(rhs.bits)) != 0;
	}

	return (lhs.bits != static_cast<U>(rhs.bits)) != 0;
}

template<typename T, size_t N, typename U>
	requires (std::is_integral_v<U> && !::std::is_same_v<U, Byte<T, N>>)
constexpr inline bool operator< (const U& lhs, const Byte<T, N>& rhs) noexcept
{
	if constexpr (::std::is_same<U, Byte<T, N>>())
	{
		return (static_cast<T>(lhs.bits) < static_cast<T>(rhs.bits)) < 0;
	}

	return (lhs.bits < static_cast<U>(rhs.bits)) < 0;
}

template<typename T, size_t N, typename U>
	requires (std::is_integral_v<U> && !::std::is_same_v<U, Byte<T, N>>)
constexpr inline bool operator<=(const U& lhs, const Byte<T, N>& rhs) noexcept
{
	if constexpr (::std::is_same<U, Byte<T, N>>())
	{
		return (static_cast<T>(lhs.bits) <= static_cast<T>(rhs.bits)) <= 0;
	}

	return (lhs.bits <= static_cast<U>(rhs.bits)) <= 0;
}

template<typename T, size_t N, typename U>
	requires (std::is_integral_v<U> && !::std::is_same_v<U, Byte<T, N>>)
constexpr inline bool operator> (const U& lhs, const Byte<T, N>& rhs) noexcept
{
	if constexpr (::std::is_same<U, Byte<T, N>>())
	{
		return (static_cast<T>(lhs.bits) > static_cast<T>(rhs.bits)) > 0;
	}

	return (lhs.bits > static_cast<U>(rhs.bits)) > 0;
}

template<typename T, size_t N, typename U>
	requires (std::is_integral_v<U> && !::std::is_same_v<U, Byte<T, N>>)
constexpr inline bool operator>=(const U& lhs, const Byte<T, N>& rhs) noexcept
{
	if constexpr (::std::is_same<U, Byte<T, N>>())
	{
		return (static_cast<T>(lhs.bits) >= static_cast<T>(rhs.bits)) >= 0;
	}

	return (lhs.bits >= static_cast<U>(rhs.bits)) >= 0;
}

template<typename T, size_t N, typename U>
	requires (std::is_integral_v<U> && !::std::is_same_v<U, Byte<T, N>>)
constexpr inline auto operator<=>(const U& lhs, const Byte<T, N>& rhs) noexcept
{
	if constexpr (::std::is_same<U, Byte<T, N>>())
	{
		return (static_cast<T>(lhs.bits) <=> static_cast<T>(rhs.bits));
	}

	return (lhs.bits <=> static_cast<U>(rhs.bits));
}

// logical (assignment)
template<typename T, size_t N, typename U>
	requires (std::is_integral_v<U> && !::std::is_same_v<U, Byte<T, N>>)
constexpr inline U& operator|=(U& lhs, const Byte<T, N>& rhs) noexcept
{
	if constexpr (::std::is_same<U, Byte<T, N>>())
	{
		return static_cast<T>(lhs.bits) |= static_cast<T>(rhs.bits);
	}

	return lhs |= static_cast<U>(rhs.bits);
}

template<typename T, size_t N, typename U>
	requires (std::is_integral_v<U> && !::std::is_same_v<U, Byte<T, N>>)
constexpr inline U& operator&=(U& lhs, const Byte<T, N>& rhs) noexcept
{
	if constexpr (::std::is_same<U, Byte<T, N>>())
	{
		return static_cast<T>(lhs.bits) &= static_cast<T>(rhs.bits);
	}

	return lhs &= static_cast<U>(rhs.bits);
}

template<typename T, size_t N, typename U>
	requires (std::is_integral_v<U> && !::std::is_same_v<U, Byte<T, N>>)
constexpr inline U& operator^=(U& lhs, const Byte<T, N>& rhs) noexcept
{
	if constexpr (::std::is_same<U, Byte<T, N>>())
	{
		return static_cast<T>(lhs.bits) ^= static_cast<T>(rhs.bits);
	}

	return lhs ^= static_cast<U>(rhs.bits);
}

// logical (non-assignment)
template<typename T, size_t N, typename U>
	requires (std::is_integral_v<U> && !::std::is_same_v<U, Byte<T, N>>)
constexpr inline U operator|(const U& lhs, const Byte<T, N>& rhs) noexcept
{
	if constexpr (::std::is_same<U, Byte<T, N>>())
	{
		return static_cast<T>(lhs.bits) | static_cast<T>(rhs.bits);
	}

	return lhs | static_cast<U>(rhs.bits);
}

template<typename T, size_t N, typename U>
	requires (std::is_integral_v<U> && !::std::is_same_v<U, Byte<T, N>>)
constexpr inline U operator&(const U& lhs, const Byte<T, N>& rhs) noexcept
{
	if constexpr (::std::is_same<U, Byte<T, N>>())
	{
		return static_cast<T>(lhs.bits) & static_cast<T>(rhs.bits);
	}

	return lhs & static_cast<U>(rhs.bits);
}

template<typename T, size_t N, typename U>
	requires (std::is_integral_v<U> && !::std::is_same_v<U, Byte<T, N>>)
constexpr inline U operator^(const U& lhs, const Byte<T, N>& rhs) noexcept
{
	if constexpr (::std::is_same<U, Byte<T, N>>())
	{
		return static_cast<T>(lhs.bits) ^ static_cast<T>(rhs.bits);
	}

	return lhs ^ static_cast<U>(rhs.bits);
}

// arithmetic (non-assignment)
template<typename T, size_t N, typename U>
	requires (std::is_integral_v<U> && !::std::is_same_v<U, Byte<T, N>>)
constexpr inline U operator+(const U& lhs, const Byte<T, N>& rhs) noexcept
{
	if constexpr (::std::is_same<U, Byte<T, N>>())
	{
		return static_cast<T>(lhs.bits) + static_cast<T>(rhs.bits);
	}

	return lhs + static_cast<U>(rhs.bits);
}

template<typename T, size_t N, typename U>
	requires (std::is_integral_v<U> && !::std::is_same_v<U, Byte<T, N>>)
constexpr inline U operator-(const U& lhs, const Byte<T, N>& rhs) noexcept
{
	if constexpr (::std::is_same<U, Byte<T, N>>())
	{
		return static_cast<T>(lhs.bits) - static_cast<T>(rhs.bits);
	}

	return lhs - static_cast<U>(rhs.bits);
}

template<typename T, size_t N, typename U>
	requires (std::is_integral_v<U> && !::std::is_same_v<U, Byte<T, N>>)
constexpr inline U operator*(const U& lhs, const Byte<T, N>& rhs) noexcept
{
	if constexpr (::std::is_same<U, Byte<T, N>>())
	{
		return static_cast<T>(lhs.bits) * static_cast<T>(rhs.bits);
	}

	return lhs * static_cast<U>(rhs.bits);
}

template<typename T, size_t N, typename U>
	requires (std::is_integral_v<U> && !::std::is_same_v<U, Byte<T, N>>)
constexpr inline U operator/(const U& lhs, const Byte<T, N>& rhs) noexcept
{
	if constexpr (::std::is_same<U, Byte<T, N>>())
	{
		return static_cast<T>(lhs.bits) / static_cast<T>(rhs.bits);
	}

	return lhs / static_cast<U>(rhs.bits);
}

template<typename T, size_t N, typename U>
	requires (std::is_integral_v<U> && !::std::is_same_v<U, Byte<T, N>>)
constexpr inline U operator%(const U& lhs, const Byte<T, N>& rhs) noexcept
{
	if constexpr (::std::is_same<U, Byte<T, N>>())
	{
		return static_cast<T>(lhs.bits) % static_cast<T>(rhs.bits);
	}

	return lhs % static_cast<U>(rhs.bits);
}

template<typename T, size_t N, typename U>
	requires (std::is_integral_v<U> && !::std::is_same_v<U, Byte<T, N>>)
constexpr inline U& operator+=( U& lhs, const Byte<T, N>& rhs) noexcept
{
	if constexpr (::std::is_same<U, Byte<T, N>>())
	{
		return static_cast<T>(lhs.bits) += static_cast<T>(rhs.bits);
	}

	return lhs += static_cast<U>(rhs.bits);
}

template<typename T, size_t N, typename U>
	requires (std::is_integral_v<U> && !::std::is_same_v<U, Byte<T, N>>)
constexpr inline U& operator-=( U& lhs, const Byte<T, N>& rhs) noexcept
{
	if constexpr (::std::is_same<U, Byte<T, N>>())
	{
		return static_cast<T>(lhs.bits) -= static_cast<T>(rhs.bits);
	}

	return lhs -= static_cast<U>(rhs.bits);
}

template<typename T, size_t N, typename U>
	requires (std::is_integral_v<U> && !::std::is_same_v<U, Byte<T, N>>)
constexpr inline U& operator*=( U& lhs, const Byte<T, N>& rhs) noexcept
{
	if constexpr (::std::is_same<U, Byte<T, N>>())
	{
		return static_cast<T>(lhs.bits) *= static_cast<T>(rhs.bits);
	}

	return lhs *= static_cast<U>(rhs.bits);
}

template<typename T, size_t N, typename U>
	requires (std::is_integral_v<U> && !::std::is_same_v<U, Byte<T, N>>)
constexpr inline U& operator/=( U& lhs, const Byte<T, N>& rhs) noexcept
{
	if constexpr (::std::is_same<U, Byte<T, N>>())
	{
		return static_cast<T>(lhs.bits) /= static_cast<T>(rhs.bits);
	}

	return lhs /= static_cast<U>(rhs.bits);
}

template<typename T, size_t N, typename U>
	requires (std::is_integral_v<U> && !::std::is_same_v<U, Byte<T, N>>)
constexpr inline U& operator%=( U& lhs, const Byte<T, N>& rhs) noexcept
{
	if constexpr (::std::is_same<U, Byte<T, N>>())
	{
		return static_cast<T>(lhs.bits) %= static_cast<T>(rhs.bits);
	}

	return lhs %= static_cast<U>(rhs.bits);
}
