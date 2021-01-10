/**********************************************************************************************************************************************\
	Copyright© 2020-2021 Mason DeRoss

	Released under the GNU All-permissive License

	Copying and distribution of this file, with or without modification, are permitted in any medium without royalty,
	provided the copyright notice and this notice are preserved. This file is offered as-is, without any warranty.

	DESCRIPTION:

\**********************************************************************************************************************************************/
#pragma once

#include <cstddef>

template<typename T, size_t N>
class Byte
{
public:
	::std::byte bits;

	using type = T;
	Byte() {}
	Byte(T bits) : bits{bits} {}
	Byte(::std::byte byte) : bits{byte} {}

	template<typename U>
	inline Byte<T, N>& operator=(const U& other) noexcept
	{
		if constexpr (::std::is_same_v(Byte<T, N>, T))
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

	template<typename U>
	inline Byte<T, N>& operator+=(const U& other) noexcept
	{
		if constexpr (::std::is_same_v(Byte<T, N>, U))
		{

			static_cast<uint8_t>(bits) += static_cast<uint8_t>(other.bits);
			return *this;
		}

		static_cast<U>(bits) += other;
		return *this;
	}

	template<typename U>
	inline Byte<T, N>& operator-=(const T& other) noexcept
	{
		if constexpr (::std::is_same_v(Byte<T, N>, U))
		{

			static_cast<uint8_t>(bits) -= static_cast<uint8_t>(other.bits);
			return *this;
		}

		static_cast<U>(bits) -= other;
		return *this;
	}

	template<typename U>
	inline Byte<T, N>& operator*=(const U& other) noexcept
	{
		if constexpr (::std::is_same_v(Byte<T, N>, U))
		{

			static_cast<uint8_t>(bits) *= static_cast<uint8_t>(other.bits);
			return *this;
		}

		static_cast<U>(bits) *= other;
		return *this;
	}

	template<typename U>
	inline Byte<T, N>& operator/=(const U& other) noexcept
	{
		if constexpr (::std::is_same_v(Byte<T, N>, U))
		{

			static_cast<uint8_t>(bits) /= static_cast<uint8_t>(other.bits);
			return *this;
		}

		static_cast<U>(bits) /= other;
		return *this;
	}

	template<typename U>
	inline Byte<T, N>& operator&=(const U& other) noexcept
	{
		if constexpr (::std::is_same_v(Byte<T, N>, U))
		{
			bits &= other.bits;
			return *this;
		}

		bits &= ::std::byte(other);
		return *this;
	}

	template<typename U>
	inline Byte<T, N>& operator|=(const U& other) noexcept
	{
		if constexpr (::std::is_same_v(Byte<T, N>, U))
		{
			bits |= other.bits;
			return *this;
		}

		bits |= ::std::byte(other);
		return *this;
	}

	template<typename U>
	inline Byte<T, N>& operator^=(const U& other) noexcept
	{
		if constexpr (::std::is_same_v(Byte<T, N>, U))
		{
			bits ^= other.bits;
			return *this;
		}

		bits ^= ::std::byte(other);
		return *this;
	}

	template<typename U>
	inline Byte<T, N>& operator~() noexcept
	{
		bits = ~bits;
		return *this;
	}

	template<typename U>
	inline U& operator<<(const U& other) noexcept
	{
		if constexpr (::std::is_same_v(Byte<T, N>, U))
		{

			return static_cast<U>(bits) << static_cast<U>(other.bits);
		}

		return static_cast<U>(bits) << other;
	}

	template<typename U>
	inline U& operator>>(const U& other) noexcept
	{
		if constexpr (::std::is_same_v(Byte<T, N>, U))
		{

			return static_cast<U>(bits) >> static_cast<U>(other.bits);
		}

		return static_cast<U>(bits) >> other;
	}

	template<typename U>
	inline U& operator&(const U& other) noexcept
	{
		if constexpr (::std::is_same_v(Byte<T, N>, U))
		{

			return static_cast<U>(bits) & static_cast<U>(other.bits);
		}

		return static_cast<U>(bits) & other;
	}

	template<typename U>
	inline U& operator|(const U& other) noexcept
	{
		if constexpr (::std::is_same_v(Byte<T, N>, U))
		{

			return static_cast<U>(bits) | static_cast<U>(other.bits);
		}

		return static_cast<U>(bits) | other;
	}

	template<typename U>
	inline U& operator^(const U& other) noexcept
	{
		if constexpr (::std::is_same_v(Byte<T, N>, U))
		{

			return static_cast<U>(bits) ^ static_cast<U>(other.bits);
		}

		return static_cast<U>(bits) ^ other;
	}

	template<typename U>
	inline Byte<T, N>& operator+(const U& other) noexcept
	{
		if constexpr (::std::is_same_v(Byte<T, N>, U))
		{

			return static_cast<uint8_t>(bits) + static_cast<uint8_t>(other.bits);
		}

		return static_cast<U>(bits) + other;
	}

	template<typename U>
	inline Byte<T, N>& operator-(const T& other) noexcept
	{
		if constexpr (::std::is_same_v(Byte<T, N>, U))
		{

			return static_cast<uint8_t>(bits) - static_cast<uint8_t>(other.bits);
		}

		return static_cast<U>(bits) - other;
	}

	template<typename U>
	inline Byte<T, N>& operator*(const U& other) noexcept
	{
		if constexpr (::std::is_same_v(Byte<T, N>, U))
		{

			return static_cast<uint8_t>(bits) * static_cast<uint8_t>(other.bits);
		}

		return static_cast<U>(bits) * other;
	}

	template<typename U>
	inline Byte<T, N>& operator/(const U& other) noexcept
	{
		if constexpr (::std::is_same_v(Byte<T, N>, U))
		{

			return static_cast<uint8_t>(bits) / static_cast<uint8_t>(other.bits);
		}

		return static_cast<U>(bits) / other;
	}

	template<typename T, size_t N, typename U>
	friend inline U& operator+(const U& lhs, const Byte<T, N>& rhs) noexcept;
	template<typename T, size_t N, typename U>
	friend inline U& operator-(const T& lhs, const Byte<T, N>& rhs) noexcept;
	template<typename T, size_t N, typename U>
	friend inline U& operator*(const U& lhs, const Byte<T, N>& rhs) noexcept;
	template<typename T, size_t N, typename U>
	friend inline U& operator/(const U& lhs, const Byte<T, N>& rhs) noexcept;
	template<typename T, size_t N, typename U>
	friend inline U& operator&=(const U& lhs, const Byte<T, N>& rhs) noexcept;
	template<typename T, size_t N, typename U>
	friend inline U& operator|=(const U& lhs, const Byte<T, N>& rhs) noexcept;
	template<typename T, size_t N, typename U>
	friend inline U& operator^=(const U& lhs, const Byte<T, N>& rhs) noexcept;

	template<typename T, size_t N, typename U>
	friend inline U& operator&(const U& lhs, const Byte<T, N>& rhs) noexcept;
	template<typename T, size_t N, typename U>
	friend inline U& operator|(const U& lhs, const Byte<T, N>& rhs) noexcept;
	template<typename T, size_t N, typename U>
	friend inline U& operator^(const U& lhs, const Byte<T, N>& rhs) noexcept;

	template<typename U>
	inline bool operator==(const U& rhs) const noexcept { return (bits == rhs) == 0; }
	template<typename U>
	inline bool operator!=(const U& rhs) const noexcept { return (bits != rhs) != 0; }
	template<typename U>
	inline bool operator< (const U& rhs) const noexcept { return (bits <  rhs) <  0; }
	template<typename U>
	inline bool operator> (const U& rhs) const noexcept { return (bits >  rhs) >  0; }
	template<typename U>
	inline bool operator<=(const U& rhs) const noexcept { return (bits <= rhs) <= 0; }
	template<typename U>
	inline bool operator>=(const U& rhs) const noexcept { return (bits >= rhs) >= 0; }

	template<typename T, size_t N, typename U>
	friend inline bool operator==(const U& lhs, const Byte<T, N>& rhs) noexcept;
	template<typename T, size_t N, typename U>
	friend inline bool operator!=(const U& lhs, const Byte<T, N>& rhs) noexcept;
	template<typename T, size_t N, typename U>
	friend inline bool operator< (const U& lhs, const Byte<T, N>& rhs) noexcept;
	template<typename T, size_t N, typename U>
	friend inline bool operator> (const U& lhs, const Byte<T, N>& rhs) noexcept;
	template<typename T, size_t N, typename U>
	friend inline bool operator<=(const U& lhs, const Byte<T, N>& rhs) noexcept;
	template<typename T, size_t N, typename U>
	friend inline bool operator>=(const U& lhs, const Byte<T, N>& rhs) noexcept;
};

template<typename T, size_t N, typename U>
inline U& operator+(const U& lhs, const Byte<T, N>& rhs) noexcept
{
	if constexpr (::std::is_same_v(Byte<T, N>, U))
	{
		return lhs.bits + static_cast<U>(rhs.bits);
	}

	return lhs + static_cast<U>(rhs.bits);
}

template<typename T, size_t N, typename U>
inline U& operator-(const U& lhs, const Byte<T, N>& rhs) noexcept
{
	if constexpr (::std::is_same_v(Byte<T, N>, U))
	{
		return lhs.bits - static_cast<U>(rhs.bits);
	}

	return lhs - static_cast<U>(rhs.bits);
}

template<typename T, size_t N, typename U>
inline U& operator*(const U& lhs, const Byte<T, N>& rhs) noexcept
{
	if constexpr (::std::is_same_v(Byte<T, N>, U))
	{
		return lhs.bits * static_cast<U>(rhs.bits);
	}

	return lhs * static_cast<U>(rhs.bits);
}

template<typename T, size_t N, typename U>
inline U& operator/(const U& lhs, const Byte<T, N>& rhs) noexcept
{
	if constexpr (::std::is_same_v(Byte<T, N>, U))
	{
		return lhs.bits / static_cast<U>(rhs.bits);
	}

	return lhs / static_cast<U>(rhs.bits);
}

template<typename T, size_t N, typename U>
inline U& operator&=(const U& lhs, const Byte<T, N>& rhs) noexcept
{
	if constexpr (::std::is_same_v(Byte<T, N>, U))
	{
		return lhs.bits &= static_cast<U>(rhs.bits);
	}

	return lhs &= static_cast<U>(rhs.bits);
}

template<typename T, size_t N, typename U>
inline U& operator|=(const U& lhs, const Byte<T, N>& rhs) noexcept
{
	if constexpr (::std::is_same_v(Byte<T, N>, U))
	{
		return lhs.bits |= static_cast<U>(rhs.bits);
	}

	return lhs |= static_cast<U>(rhs.bits);
}

template<typename T, size_t N, typename U>
inline U& operator^=(const U& lhs, const Byte<T, N>& rhs) noexcept
{
	if constexpr (::std::is_same_v(Byte<T, N>, U))
	{
		return lhs.bits ^= static_cast<U>(rhs.bits);
	}

	return lhs ^= static_cast<U>(rhs.bits);
}

template<typename T, size_t N, typename U>
inline U& operator&(const U& lhs, const Byte<T, N>& rhs) noexcept
{
	if constexpr (::std::is_same_v(Byte<T, N>, U))
	{
		return lhs.bits & static_cast<U>(rhs.bits);
	}

	return lhs & static_cast<U>(rhs.bits);
}
template<typename T, size_t N, typename U>
inline U& operator|(const U& lhs, const Byte<T, N>& rhs) noexcept
{
	if constexpr (::std::is_same_v(Byte<T, N>, U))
	{
		return lhs.bits | static_cast<U>(rhs.bits);
	}

	return lhs | static_cast<U>(rhs.bits);
}
template<typename T, size_t N, typename U>
inline U& operator^(const U& lhs, const Byte<T, N>& rhs) noexcept
{
	if constexpr (::std::is_same_v(Byte<T, N>, U))
	{
		return lhs.bits ^ static_cast<U>(rhs.bits);
	}

	return lhs ^ static_cast<U>(rhs.bits);
}

template<typename T, size_t N, typename U>
inline bool operator==(const U& lhs, const Byte<T, N>& rhs) noexcept { return (lhs == rhs.bits) == 0; }
template<typename T, size_t N, typename U>
inline bool operator!=(const U& lhs, const Byte<T, N>& rhs) noexcept { return (lhs != rhs.bits) != 0; }
template<typename T, size_t N, typename U>
inline bool operator< (const U& lhs, const Byte<T, N>& rhs) noexcept { return (lhs <  rhs.bits) <  0; }
template<typename T, size_t N, typename U>
inline bool operator> (const U& lhs, const Byte<T, N>& rhs) noexcept { return (lhs >  rhs.bits) >  0; }
template<typename T, size_t N, typename U>
inline bool operator<=(const U& lhs, const Byte<T, N>& rhs) noexcept { return (lhs <= rhs.bits) <= 0; }
template<typename T, size_t N, typename U>
inline bool operator>=(const U& lhs, const Byte<T, N>& rhs) noexcept { return (lhs >= rhs.bits) >= 0; }
