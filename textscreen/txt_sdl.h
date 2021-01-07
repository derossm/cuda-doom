/**********************************************************************************************************************************************\
	Copyright(C) 2005-2014 Simon Howard

	This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License
	as published by the Free Software Foundation; either version 2 of the License, or (at your option) any later version.

	This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

	DESCRIPTION:
		Text mode emulation in SDL
\**********************************************************************************************************************************************/
#pragma once
// DECOUPLE
//#include "../derma/common.h"
//////////

#include "SDL.h"

#include <array>

namespace cudadoom::txt
{

// The textscreen API itself doesn't need SDL; however, SDL needs its headers included where main() is defined.
struct FontType
{
	// 32 BYTE struct
	std::array<const char, 16> name;
	const uint8_t* __data;
	unsigned w;
	unsigned h;
};
static_assert(sizeof(FontType) == 32);

// Event callback function type: a function of this type can be used to intercept events in the textscreen event processing loop.
// Returning 1 will cause the event to be eaten; the textscreen code will not see it.
typedef int (*TxtSDLEventCallbackFunc)(SDL_Event* event, void* user_data);

// Set a callback function to call in the SDL event loop. Useful for intercepting events. Pass callback=NULL to clear an existing
// callback function. user_data is a void pointer to be passed to the callback function.
void SDL_SetEventCallback(TxtSDLEventCallbackFunc callback, void* user_data);

} // END NAMESPACE cudadoom::txt
