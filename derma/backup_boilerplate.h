
#include "stdafx.h"

/*
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
*/