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
	T get() { return value; }

	explicit inline constexpr StrongType(const StrongType<T, U>& other) noexcept : value{other.value} {}
	StrongType<T, U>& operator=(const StrongType<T, U>& other)
	{
		if (this != &other)
		{
			value = other.value;
		}

		return *this;
	}

	explicit inline constexpr StrongType(StrongType<T, U>&& other) noexcept : value{other.val} {}
	StrongType<T, U>& operator=(StrongType<T, U>&& other)
	{
		if (this != &other)
		{
			value = other.value;
		}

		return *this;
	}

	explicit inline constexpr StrongType(const T& other) noexcept : value{other} {}
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

	//explicit inline constexpr StrongType(T&& other) noexcept : value{std::move(other)} {}
	explicit inline constexpr StrongType(T&& other) noexcept : value{other} {}
	StrongType<T, U>& operator=(T&& other)
	{
		if constexpr (::std::is_class<T>::value == true)
		{	if constexpr (::std::is_base_of_v<StrongType<T,U>, T>::value == true)
			{
				if (this != &other)
				{
					//return StrongType<T, U>(std::forward(other));
					//*this = &other; // since this doesn't work afaik, return move constructed new object
					value = other.value;
				}

				return *this;
			}
		}

		//return StrongType<T, U>(std::forward(other));
		value = other;

		return *this;
	}

	inline constexpr bool operator==(const StrongType<T, U>& rhs) const noexcept { return (value == rhs.value); }
	inline constexpr bool operator!=(const StrongType<T, U>& rhs) const noexcept { return (value != rhs.value); }
	inline constexpr bool operator< (const StrongType<T, U>& rhs) const noexcept { return (value <  rhs.value); }
	inline constexpr bool operator> (const StrongType<T, U>& rhs) const noexcept { return (value >  rhs.value); }
	inline constexpr bool operator<=(const StrongType<T, U>& rhs) const noexcept { return (value <= rhs.value); }
	inline constexpr bool operator>=(const StrongType<T, U>& rhs) const noexcept { return (value >= rhs.value); }
	inline constexpr auto operator<=>(const StrongType<T, U>& rhs) const noexcept { return (value <=> rhs.value); };

	inline constexpr bool operator==(StrongType<T, U>&& rhs) const noexcept { return (value == rhs.value); }
	inline constexpr bool operator!=(StrongType<T, U>&& rhs) const noexcept { return (value != rhs.value); }
	inline constexpr bool operator< (StrongType<T, U>&& rhs) const noexcept { return (value <  rhs.value); }
	inline constexpr bool operator> (StrongType<T, U>&& rhs) const noexcept { return (value >  rhs.value); }
	inline constexpr bool operator<=(StrongType<T, U>&& rhs) const noexcept { return (value <= rhs.value); }
	inline constexpr bool operator>=(StrongType<T, U>&& rhs) const noexcept { return (value >= rhs.value); }
	inline constexpr auto operator<=>(StrongType<T, U>&& rhs) const noexcept { return (value <=> rhs.value); };

	inline constexpr bool operator==(const T& rhs) const noexcept { return (value == rhs); }
	inline constexpr bool operator!=(const T& rhs) const noexcept { return (value != rhs); }
	inline constexpr bool operator< (const T& rhs) const noexcept { return (value <  rhs); }
	inline constexpr bool operator> (const T& rhs) const noexcept { return (value >  rhs); }
	inline constexpr bool operator<=(const T& rhs) const noexcept { return (value <= rhs); }
	inline constexpr bool operator>=(const T& rhs) const noexcept { return (value >= rhs); }
	inline constexpr auto operator<=>(const T& rhs) const noexcept { return (value <=> rhs); }

	inline constexpr bool operator==(T&& rhs) const noexcept { return (value == rhs); }
	inline constexpr bool operator!=(T&& rhs) const noexcept { return (value != rhs); }
	inline constexpr bool operator< (T&& rhs) const noexcept { return (value <  rhs); }
	inline constexpr bool operator> (T&& rhs) const noexcept { return (value >  rhs); }
	inline constexpr bool operator<=(T&& rhs) const noexcept { return (value <= rhs); }
	inline constexpr bool operator>=(T&& rhs) const noexcept { return (value >= rhs); }
	inline constexpr auto operator<=>(T&& rhs) const noexcept { return (value <=> rhs); }

	friend inline constexpr bool operator==(const T& lhs, const StrongType<T, U>& rhs) noexcept;
	friend inline constexpr bool operator!=(const T& lhs, const StrongType<T, U>& rhs) noexcept;
	friend inline constexpr bool operator< (const T& lhs, const StrongType<T, U>& rhs) noexcept;
	friend inline constexpr bool operator> (const T& lhs, const StrongType<T, U>& rhs) noexcept;
	friend inline constexpr bool operator<=(const T& lhs, const StrongType<T, U>& rhs) noexcept;
	friend inline constexpr bool operator>=(const T& lhs, const StrongType<T, U>& rhs) noexcept;
	friend inline constexpr auto operator<=>(const T& lhs, const StrongType<T, U>& rhs) noexcept;

	friend inline constexpr bool operator==(T&& lhs, StrongType<T, U>&& rhs) noexcept;
	friend inline constexpr bool operator!=(T&& lhs, StrongType<T, U>&& rhs) noexcept;
	friend inline constexpr bool operator< (T&& lhs, StrongType<T, U>&& rhs) noexcept;
	friend inline constexpr bool operator> (T&& lhs, StrongType<T, U>&& rhs) noexcept;
	friend inline constexpr bool operator<=(T&& lhs, StrongType<T, U>&& rhs) noexcept;
	friend inline constexpr bool operator>=(T&& lhs, StrongType<T, U>&& rhs) noexcept;
	friend inline constexpr auto operator<=>(T&& lhs, StrongType<T, U>&& rhs) noexcept;
};

template<typename T, typename U>
inline constexpr bool operator==(const T& lhs, const StrongType<T, U>& rhs) noexcept { return (lhs == rhs.value); }
template<typename T, typename U>
inline constexpr bool operator!=(const T& lhs, const StrongType<T, U>& rhs) noexcept { return (lhs != rhs.value); }
template<typename T, typename U>
inline constexpr bool operator< (const T& lhs, const StrongType<T, U>& rhs) noexcept { return (lhs <  rhs.value); }
template<typename T, typename U>
inline constexpr bool operator> (const T& lhs, const StrongType<T, U>& rhs) noexcept { return (lhs >  rhs.value); }
template<typename T, typename U>
inline constexpr bool operator<=(const T& lhs, const StrongType<T, U>& rhs) noexcept { return (lhs <= rhs.value); }
template<typename T, typename U>
inline constexpr bool operator>=(const T& lhs, const StrongType<T, U>& rhs) noexcept { return (lhs >= rhs.value); }
template<typename T, typename U>
inline constexpr auto operator<=>(const T& lhs, const StrongType<T, U>& rhs) noexcept { return (lhs <=> rhs.value); }

template<typename T, typename U>
inline constexpr bool operator==(T&& lhs, StrongType<T, U>&& rhs) noexcept { return (lhs == rhs.value); }
template<typename T, typename U>
inline constexpr bool operator!=(T&& lhs, StrongType<T, U>&& rhs) noexcept { return (lhs != rhs.value); }
template<typename T, typename U>
inline constexpr bool operator< (T&& lhs, StrongType<T, U>&& rhs) noexcept { return (lhs <  rhs.value); }
template<typename T, typename U>
inline constexpr bool operator> (T&& lhs, StrongType<T, U>&& rhs) noexcept { return (lhs >  rhs.value); }
template<typename T, typename U>
inline constexpr bool operator<=(T&& lhs, StrongType<T, U>&& rhs) noexcept { return (lhs <= rhs.value); }
template<typename T, typename U>
inline constexpr bool operator>=(T&& lhs, StrongType<T, U>&& rhs) noexcept { return (lhs >= rhs.value); }
template<typename T, typename U>
inline constexpr auto operator<=>(T&& lhs, StrongType<T, U>&& rhs) noexcept { return (lhs <=> rhs.value); }
