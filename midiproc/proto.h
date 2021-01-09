/**********************************************************************************************************************************************\
	Copyright(C) 2017 Alex Mayfield

	This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License
	as published by the Free Software Foundation; either version 2 of the License, or (at your option) any later version.

	This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

	DESCRIPTION:
		Headers for all types of midipipe messages.
\**********************************************************************************************************************************************/
#pragma once

#include "../derma/common.h"

namespace cudadoom::midi
{

enum class PacketType
{
	REGISTER_SONG,
	SET_VOLUME,
	PLAY_SONG,
	STOP_SONG,
	SHUTDOWN,
	UNREGISTER_SONG,
	ACK
};

} // end namespace cudadoom::midi
