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

template<typename T, typename E>
	//requires ::std::is_enum_v<E>
class BitManipulator
{
private:
	E value;

public:
	using Type = T;
	using Enum = E;

	BitManipulator()
	{
	}

	~BitManipulator()
	{
	}

	inline bool operator==(const E& rhs) const noexcept { return (value == rhs) == 0; }
	inline bool operator!=(const E& rhs) const noexcept { return (value != rhs) != 0; }
	inline bool operator< (const E& rhs) const noexcept { return (value <  rhs) <  0; }
	inline bool operator> (const E& rhs) const noexcept { return (value >  rhs) >  0; }
	inline bool operator<=(const E& rhs) const noexcept { return (value <= rhs) <= 0; }
	inline bool operator>=(const E& rhs) const noexcept { return (value >= rhs) >= 0; }

	friend inline bool operator==(const E& lhs, const BitManipulator<T, E>& rhs) noexcept;
	friend inline bool operator!=(const E& lhs, const BitManipulator<T, E>& rhs) noexcept;
	friend inline bool operator< (const E& lhs, const BitManipulator<T, E>& rhs) noexcept;
	friend inline bool operator> (const E& lhs, const BitManipulator<T, E>& rhs) noexcept;
	friend inline bool operator<=(const E& lhs, const BitManipulator<T, E>& rhs) noexcept;
	friend inline bool operator>=(const E& lhs, const BitManipulator<T, E>& rhs) noexcept;

	BitManipulator<T, E>& operator=(const T& other)
	{
		if constexpr (::std::is_class<T>::value == true)
		{	if constexpr (::std::is_base_of_v<StrongType<T,U>, T>::value == true)
			{
				if (this != &other)
				{
					value = other.value;
				}
				
				return *this;
			}
		}

		value = other;

		return *this;
	}

	BitManipulator<T, E>& operator=(const E& other)
	{
		value = other;

		return *this;
	}

	//T operator()() { return value; }
};

template<typename T, typename E>
inline bool operator==(const E& lhs, const BitManipulator<T, E>& rhs) noexcept { return (lhs == rhs.value) == 0; }
template<typename T, typename E>
inline bool operator!=(const E& lhs, const BitManipulator<T, E>& rhs) noexcept { return (lhs != rhs.value) != 0; }
template<typename T, typename E>
inline bool operator< (const E& lhs, const BitManipulator<T, E>& rhs) noexcept { return (lhs <  rhs.value) <  0; }
template<typename T, typename E>
inline bool operator> (const E& lhs, const BitManipulator<T, E>& rhs) noexcept { return (lhs >  rhs.value) >  0; }
template<typename T, typename E>
inline bool operator<=(const E& lhs, const BitManipulator<T, E>& rhs) noexcept { return (lhs <= rhs.value) <= 0; }
template<typename T, typename E>
inline bool operator>=(const E& lhs, const BitManipulator<T, E>& rhs) noexcept { return (lhs >= rhs.value) >= 0; }

/*
template<typename T, typename E>
requires ::std::is_enum_v<E>
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

	T& operator[](::std::ptrdiff_t rhs) { return static_cast<T>(value); }
	T& operator[](::std::ptrdiff_t lhs, Index<T, E>* rhs) { return static_cast<T>(this->value); }

	friend inline bool operator==(const E& lhs, const Index<T, E>& rhs) noexcept;
	friend inline bool operator!=(const E& lhs, const Index<T, E>& rhs) noexcept;
	friend inline bool operator< (const E& lhs, const Index<T, E>& rhs) noexcept;
	friend inline bool operator> (const E& lhs, const Index<T, E>& rhs) noexcept;
	friend inline bool operator<=(const E& lhs, const Index<T, E>& rhs) noexcept;
	friend inline bool operator>=(const E& lhs, const Index<T, E>& rhs) noexcept;

	Index<T, E>& operator=(const T& other)
	{
		if constexpr (::std::is_class<T>::value == true)
		{	//if constexpr (::std::is_base_of_v<Index<T,E>, T>::value == true)
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
