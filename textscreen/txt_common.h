/**********************************************************************************************************************************************\
	[Doom]
	Copyright(C) 1993-1996 Id Software, Inc.

	[textscreen]
	Copyright(C) 2005-2014 Simon Howard

	This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License
	as published by the Free Software Foundation; either version 2 of the License, or (at your option) any later version.

	This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

	------------------------------------------------------------
	Any new code or derivations not subject to prior copyrights:
		Copyright(C) 2021 Mason DeRoss

		Released under the GNU All-permissive License

		Copying and distribution of this file, with or without modification, are permitted in any medium without royalty,
		provided the copyright notice and this notice are preserved. This file is offered as-is, without any warranty.

	DESCRIPTION:

\**********************************************************************************************************************************************/
#pragma once
// DECOUPLE
#include "../derma/common.h"
//////////

namespace cudadoom::txt
{

template<class T>
class WidgetClass;

template<class T>
class Widget;

template<class T>
class Table;

template<typename T>
class InputBox;

class Window;

class Button;
class CheckBox;
class Conditional;
class DropDownList;
class FileSelect;

class Label;
class RadioButton;
class ScrollPane;
class Separator;
class SpinControl;
class Strut;
class WindowAction;

using point_t = long double;
using UserData = void*;

class DepType {};

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

	//inline bool operator==(const StrongType<T, U>& lhs, const T& rhs){ return cmp(lhs.value,rhs) == 0; }
	//inline bool operator!=(const StrongType<T, U>& lhs, const T& rhs){ return cmp(lhs.value,rhs) != 0; }
	//inline bool operator< (const StrongType<T, U>& lhs, const T& rhs){ return cmp(lhs.value,rhs) < 0; }
	//inline bool operator> (const StrongType<T, U>& lhs, const T& rhs){ return cmp(lhs.value,rhs) > 0; }
	//inline bool operator<=(const StrongType<T, U>& lhs, const T& rhs){ return cmp(lhs.value,rhs) <= 0; }
	//inline bool operator>=(const StrongType<T, U>& lhs, const T& rhs){ return cmp(lhs.value,rhs) >= 0; }

	//inline bool operator==(const T& lhs, const StrongType<T, U>& rhs){ return cmp(lhs,rhs.value) == 0; }
	//inline bool operator!=(const T& lhs, const StrongType<T, U>& rhs){ return cmp(lhs,rhs.value) != 0; }
	//inline bool operator< (const T& lhs, const StrongType<T, U>& rhs){ return cmp(lhs,rhs.value) < 0; }
	//inline bool operator> (const T& lhs, const StrongType<T, U>& rhs){ return cmp(lhs,rhs.value) > 0; }
	//inline bool operator<=(const T& lhs, const StrongType<T, U>& rhs){ return cmp(lhs,rhs.value) <= 0; }
	//inline bool operator>=(const T& lhs, const StrongType<T, U>& rhs){ return cmp(lhs,rhs.value) >= 0; }

	StrongType<T, U>& operator=(const T& other)
	{
		if constexpr (std::is_class<T>::value == true)
		{	if constexpr (std::is_base_of_v<StrongType<T,U>, T>::value == true)
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

	//T operator()() { return value; }
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

struct KeyType {};
struct MouseType {};

using Key = StrongType<int, KeyType>;
using MouseButton = StrongType<int, MouseType>;

struct Vec2
{
	point_t x;
	point_t y;
};

struct Vec3
{
	point_t x;
	point_t y;
	point_t z;
};

struct KeyEvent
{
	Key key;
};

struct MouseEvent
{
	int button;
	float x;
	float y;
};

class CallbackEntry
{
private:
	//UserData user;
	//std::function<void(UserData)> handle;
	std::string signal;

	//CallbackEntry() = delete;

	// do not copy
	//CallbackEntry(const CallbackEntry& rhs) = delete;
	//CallbackEntry& operator=(const CallbackEntry& rhs) = delete;

public:
	//CallbackEntry(std::string _signal, std::function<void(UserData)> _handle, UserData _user) noexcept
		//: signal(std::move(_signal)), handle(std::move(_handle)), user(std::move(_user))
	//{}

	//CallbackEntry(CallbackEntry&& rhs) noexcept : signal(std::move(rhs.signal)), handle(std::move(rhs.handle)), user(std::move(rhs.user))
	//{}
/*
	std::function<void(UserData)> findByName(std::string& _signal)
	{
		if (signal.compare(_signal))
		{
			return handle;
		}
		else
		{
			return nullptr;
		}
	}
*/
	//CallbackEntry& operator=(CallbackEntry&& rhs) noexcept
	//{
		//signal = std::move(rhs.signal);
		//handle = std::move(rhs.handle);
		//user = std::move(rhs.user);
		//return *this;
	//}

	//~CallbackEntry() = default;

};

class CallbackTable
{
private:
	//std::vector<CallbackEntry> callbacks;

	//CallbackTable(CallbackTable&&) = delete;
	//CallbackTable& operator=(CallbackTable&&) = delete;

	//CallbackTable(const CallbackTable&) = delete;
	//CallbackTable& operator=(const CallbackTable&) = delete;

public:
	// reserve callbacks in contiguous memory
	//CallbackTable() noexcept //: callbacks(32)
	//{}

	//~CallbackTable() = default;

	//void connect(std::string&& signal, std::function<void(UserData)>&& handle, UserData&& user)
	//{
		//callbacks.emplace_back(CallbackEntry(std::move(signal), std::move(handle), std::move(user)));
	//}

	/*std::function<void(UserData)> get(std::string& signal)
	{
		for (auto& iter : callbacks)
		{
			if (auto ptr = iter.findByName(signal); ptr)
			{
				return ptr;
			}
		}

		return nullptr;
	}*/

	size_t size()
	{
		//return callbacks.size();
	}
};

template<typename T>
class WidgetClass
{
public:
	//std::function<bool(void)> Selectable;
	//std::function<void(void)> CalculateSize;
	//std::function<void(void)> Draw;
	//std::function<bool(KeyType)> KeyPress;
	//std::function<bool(MouseEvent)> MousePress;
	//std::function<void(void)> SetLayout;
	//std::function<void(bool)> SetFocus;
	//std::function<void(void)> Destroy;

	using Type = T;
};

// static WidgetClass txt_widget_class
//{
//	Selectable,
//	CalculateSize,
//	Draw,
//	KeyPress,
//	MousePress,
//	SetLayout,
//	SetFocus,
//	Destroy
//};

}
