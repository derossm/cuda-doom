/**********************************************************************************************************************************************\
	Doom
	Copyright(C) 1993-1996 Id Software, Inc.

	textscreen
	Copyright(C) 2005-2014 Simon Howard

	This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License
	as published by the Free Software Foundation; either version 2 of the License, or (at your option) any later version.

	This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

	------------------------------------------------------------
	Any new code or derivations not subject to prior copyrights:
		Copyright© 2020-2021 Mason DeRoss

		Released under the GNU All-permissive License

		Copying and distribution of this file, with or without modification, are permitted in any medium without royalty,
		provided the copyright notice and this notice are preserved. This file is offered as-is, without any warranty.

	DESCRIPTION:

\**********************************************************************************************************************************************/
#pragma once

namespace cudadoom::txt
{

class CallbackEntry
{
private:
	//UserData user;
	//::std::function<void(UserData)> handle;
	//::std::string signal;

	//CallbackEntry() = delete;

	// do not copy
	//CallbackEntry(const CallbackEntry& rhs) = delete;
	//CallbackEntry& operator=(const CallbackEntry& rhs) = delete;

public:
	//CallbackEntry(::std::string _signal, ::std::function<void(UserData)> _handle, UserData _user) noexcept
		//: signal(::std::move(_signal)), handle(::std::move(_handle)), user(::std::move(_user))
	//{}

	//CallbackEntry(CallbackEntry&& rhs) noexcept : signal(::std::move(rhs.signal)), handle(::std::move(rhs.handle)), user(::std::move(rhs.user))
	//{}
/*
	::std::function<void(UserData)> findByName(::std::string& _signal)
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
		//signal = ::std::move(rhs.signal);
		//handle = ::std::move(rhs.handle);
		//user = ::std::move(rhs.user);
		//return *this;
	//}

	//~CallbackEntry() = default;

};

class CallbackTable
{
private:
	//::std::vector<CallbackEntry> callbacks;

	//CallbackTable(CallbackTable&&) = delete;
	//CallbackTable& operator=(CallbackTable&&) = delete;

	//CallbackTable(const CallbackTable&) = delete;
	//CallbackTable& operator=(const CallbackTable&) = delete;

public:
	// reserve callbacks in contiguous memory
	//CallbackTable() noexcept //: callbacks(32)
	//{}

	//~CallbackTable() = default;

	//void connect(::std::string&& signal, ::std::function<void(UserData)>&& handle, UserData&& user)
	//{
		//callbacks.emplace_back(CallbackEntry(::std::move(signal), ::std::move(handle), ::std::move(user)));
	//}

	/*::std::function<void(UserData)> get(::std::string& signal)
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
	//::std::function<bool(void)> Selectable;
	//::std::function<void(void)> CalculateSize;
	//::std::function<void(void)> Draw;
	//::std::function<bool(KeyType)> KeyPress;
	//::std::function<bool(MouseEvent)> MousePress;
	//::std::function<void(void)> SetLayout;
	//::std::function<void(bool)> SetFocus;
	//::std::function<void(void)> Destroy;

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

} // end namespace cudadoom::txt
