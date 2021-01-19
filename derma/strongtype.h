/**********************************************************************************************************************************************\
	Copyright© 2020-2021 Mason DeRoss

	Released under the GNU All-permissive License

	Copying and distribution of this file, with or without modification, are permitted in any medium without royalty,
	provided the copyright notice and this notice are preserved. This file is offered as-is, without any warranty.

	DESCRIPTION:

\**********************************************************************************************************************************************/
#pragma once

#include "stdafx.h"

template<typename T, typename U>
class StrongType
{
	T value;
public:
	explicit StrongType(const T& _val) noexcept : value{_val} {}
	T get() { return value; }

	auto operator<=>(const StrongType<T, U>&) const = default;

	StrongType<T, U>& operator=(const StrongType<T, U>& other)
	{
		if (this != &other)
		{
			value = other.value;
		}

		return *this;
	}

	inline bool operator==(const T& rhs) const noexcept { return (value == rhs) == 0; }
	inline bool operator!=(const T& rhs) const noexcept { return (value != rhs) != 0; }
	inline bool operator< (const T& rhs) const noexcept { return (value <  rhs) <  0; }
	inline bool operator> (const T& rhs) const noexcept { return (value >  rhs) >  0; }
	inline bool operator<=(const T& rhs) const noexcept { return (value <= rhs) <= 0; }
	inline bool operator>=(const T& rhs) const noexcept { return (value >= rhs) >= 0; }

	friend inline bool operator==(const T& lhs, const StrongType<T, U>& rhs) noexcept;
	friend inline bool operator!=(const T& lhs, const StrongType<T, U>& rhs) noexcept;
	friend inline bool operator< (const T& lhs, const StrongType<T, U>& rhs) noexcept;
	friend inline bool operator> (const T& lhs, const StrongType<T, U>& rhs) noexcept;
	friend inline bool operator<=(const T& lhs, const StrongType<T, U>& rhs) noexcept;
	friend inline bool operator>=(const T& lhs, const StrongType<T, U>& rhs) noexcept;

	StrongType<T, U>& operator=(const T& other)
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
};

template<typename T, typename U>
inline bool operator==(const T& lhs, const StrongType<T, U>& rhs) noexcept { return (lhs == rhs.value) == 0; }
template<typename T, typename U>
inline bool operator!=(const T& lhs, const StrongType<T, U>& rhs) noexcept { return (lhs != rhs.value) != 0; }
template<typename T, typename U>
inline bool operator< (const T& lhs, const StrongType<T, U>& rhs) noexcept { return (lhs <  rhs.value) <  0; }
template<typename T, typename U>
inline bool operator> (const T& lhs, const StrongType<T, U>& rhs) noexcept { return (lhs >  rhs.value) >  0; }
template<typename T, typename U>
inline bool operator<=(const T& lhs, const StrongType<T, U>& rhs) noexcept { return (lhs <= rhs.value) <= 0; }
template<typename T, typename U>
inline bool operator>=(const T& lhs, const StrongType<T, U>& rhs) noexcept { return (lhs >= rhs.value) >= 0; }
