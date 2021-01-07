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
	requires std::is_enum_v<std::remove_reference_t<E>>
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

	template<typename... Es>
	requires (std::is_enum_v<std::remove_reference_t<Es>> && ...)
			|| (std::is_integral<std::remove_reference_t<Es>> && ...)
	inline BitSet<E, N>& set(bool value, Es... es) :
	{
		auto ev = {es...};
		std::for_each(ev.begin(), ev.end(), [&](auto& iter){ bitset::set(static_cast<size_t>(iter), value); });
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

} // end namespace cudadoom
