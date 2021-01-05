/**********************************************************************************************************************************************\
	Copyright(C) 2005-2014 Simon Howard

	This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License
	as published by the Free Software Foundation; either version 2 of the License, or (at your option) any later version.

	This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
\**********************************************************************************************************************************************/
#pragma once

#include "../../derma/common.h"

#include "../../textscreen/txt_widget.h"

// A mouse input is like an input box. When selected, a box pops up allowing a mouse to be selected.
struct txt_mouse_input_t
{
	cudadoom::txt::Widget widget;
	int* variable;
	int check_conflicts;
};

txt_mouse_input_t* NewMouseInput(int* variable);
