/**********************************************************************************************************************************************\
	Copyright(C) 2005-2014 Simon Howard

	This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License
	as published by the Free Software Foundation; either version 2 of the License, or (at your option) any later version.

	This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
\**********************************************************************************************************************************************/
// #pragma once

// #include "../derma/common.h"

// #include "../src/doomkeys.h"

// #include "txt_main.h"
// #include "txt_button.h"
// #include "txt_checkbox.h"
// #include "txt_conditional.h"
// #include "txt_desktop.h"
// #include "txt_dropdown.h"
// #include "txt_fileselect.h"
// #include "txt_inputbox.h"
// #include "txt_label.h"
// #include "txt_radiobutton.h"
// #include "txt_scrollpane.h"
// #include "txt_separator.h"
// #include "txt_spinctrl.h"
// #include "txt_strut.h"
// #include "txt_table.h"
// #include "txt_widget.h"
// #include "txt_window_action.h"
// #include "txt_window.h"

export module textscreen;

import std.core;

export
{

class DepType;
using txt_inputbox_t = cudadoom::txt::InputBox<DepType>;

using byte = uint8_t;

using TimeType = uint64_t;

namespace cudadoom
{
	namespace d
	{
	}

	namespace deh
	{
	}

	namespace f
	{
	}

	namespace g
	{
	}

	namespace hu
	{
	}

	namespace m
	{
	}

	namespace midi
	{
	}

	namespace opl
	{
	}

	namespace pcsnd
	{
	}

	namespace p
	{
	}

	namespace r
	{
	}

	namespace s
	{
	}

	namespace st
	{
	}

	namespace txt
	{
	}
}

namespace cudadoom::midi
{
	enum class PacketType;
	using PacketTypeAlias = unsigned;
}

namespace cudadoom::txt
{
	template<typename T>
	class InputBox;
}

}
