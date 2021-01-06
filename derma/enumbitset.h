/**********************************************************************************************************************************************\
	Copyright(C) 2020 Mason DeRoss

	Released under the GNU All-permissive License

	Copying and distribution of this file, with or without modification, are permitted in any medium without royalty,
	provided the copyright notice and this notice are preserved. This file is offered as-is, without any warranty.

	DESCRIPTION:

\**********************************************************************************************************************************************/
#pragma once

#include "common.h"

namespace cudadoom
{

template<typename E, E N>
class BitSet : public std::bitset<static_cast<size_t>(N)>
{
public:
	inline BitSet<E, N>& set() noexcept
	{
		bitset::set();
		return *this;
	}

	inline BitSet<E, N>& set(E pos, bool value = true)
	{
		bitset::set(static_cast<size_t>(pos), value);
		return *this;
	}

	inline BitSet<E, N>& set(size_t pos, bool value = true)
	{
		bitset::set(pos, value);
		return *this;
	}

	inline BitSet<E, N>& flip() noexcept
	{
		bitset::flip();
		return *this;
	}

	inline BitSet<E, N>& flip(E pos)
	{
		bitset::flip(static_cast<size_t>(pos));
		return *this;
	}

	inline BitSet<E, N>& flip(size_t pos)
	{
		bitset::flip(pos);
		return *this;
	}

	inline BitSet<E, N>& reset() noexcept
	{
		bitset::reset();
		return *this;
	}

	inline BitSet<E, N>& reset(E pos)
	{
		bitset::reset(static_cast<size_t>(pos));
		return *this;
	}

	inline BitSet<E, N>& reset(size_t pos)
	{
		bitset::reset(pos);
		return *this;
	}

	inline bool test(E pos)
	{
		bitset::test(static_cast<size_t>(pos));
		return *this;
	}

	inline bool test(size_t pos)
	{
		bitset::test(pos);
		return *this;
	}

	//inline bool operator<=>(const BitSet<E, N> &b) = default;
};

/*
template<typename T, typename E>
requires std::is_enum_v<E>
class Index
{
private:
	E value;

public:
	using Type = T;
	using Enum = E;

	Index()
	{
	}

	~Index()
	{
	}

	inline bool operator==(const E& rhs) const noexcept { return (value == rhs) == 0; }
	inline bool operator!=(const E& rhs) const noexcept { return (value != rhs) != 0; }
	inline bool operator< (const E& rhs) const noexcept { return (value <  rhs) <  0; }
	inline bool operator> (const E& rhs) const noexcept { return (value >  rhs) >  0; }
	inline bool operator<=(const E& rhs) const noexcept { return (value <= rhs) <= 0; }
	inline bool operator>=(const E& rhs) const noexcept { return (value >= rhs) >= 0; }

	T& operator[](std::ptrdiff_t rhs) { return static_cast<T>(value); }
	T& operator[](std::ptrdiff_t lhs, Index<T, E>* rhs) { return static_cast<T>(this->value); }

	friend inline bool operator==(const E& lhs, const Index<T, E>& rhs) noexcept;
	friend inline bool operator!=(const E& lhs, const Index<T, E>& rhs) noexcept;
	friend inline bool operator< (const E& lhs, const Index<T, E>& rhs) noexcept;
	friend inline bool operator> (const E& lhs, const Index<T, E>& rhs) noexcept;
	friend inline bool operator<=(const E& lhs, const Index<T, E>& rhs) noexcept;
	friend inline bool operator>=(const E& lhs, const Index<T, E>& rhs) noexcept;

	Index<T, E>& operator=(const T& other)
	{
		if constexpr (std::is_class<T>::value == true)
		{	//if constexpr (std::is_base_of_v<Index<T,E>, T>::value == true)
			//{
				if (this != &other)
				{
					value = other.value;
				}
				
				return *this;
			//}
		}

		value = other;

		return *this;
	}

	Index<T, E>& operator=(const E& other)
	{
		value = other;

		return *this;
	}

	//T operator()() { return value; }
};

template<typename T, typename E>
inline bool operator==(const E& lhs, const Index<T, E>& rhs) noexcept { return (lhs == rhs.value) == 0; }
template<typename T, typename E>
inline bool operator!=(const E& lhs, const Index<T, E>& rhs) noexcept { return (lhs != rhs.value) != 0; }
template<typename T, typename E>
inline bool operator< (const E& lhs, const Index<T, E>& rhs) noexcept { return (lhs <  rhs.value) <  0; }
template<typename T, typename E>
inline bool operator> (const E& lhs, const Index<T, E>& rhs) noexcept { return (lhs >  rhs.value) >  0; }
template<typename T, typename E>
inline bool operator<=(const E& lhs, const Index<T, E>& rhs) noexcept { return (lhs <= rhs.value) <= 0; }
template<typename T, typename E>
inline bool operator>=(const E& lhs, const Index<T, E>& rhs) noexcept { return (lhs >= rhs.value) >= 0; }

 */

} // end namespace cudadoom
