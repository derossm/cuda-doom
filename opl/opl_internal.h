/**********************************************************************************************************************************************\
	Copyright(C) 2005-2014 Simon Howard

	This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License
	as published by the Free Software Foundation; either version 2 of the License, or (at your option) any later version.

	This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

	DESCRIPTION:
		OPL internal interface.
\**********************************************************************************************************************************************/
#pragma once

#include "../derma/common.h"

#include "opl.h"

typedef int (*opl_init_func)(unsigned int port_base);
typedef void (*opl_shutdown_func)();
typedef unsigned int (*opl_read_port_func)(opl_port_t port);
typedef void (*opl_write_port_func)(opl_port_t port, unsigned int value);
typedef void (*opl_set_callback_func)(uint64_t us, opl_callback_t callback, delay_data_t* data);
typedef void (*opl_clear_callbacks_func)();
typedef void (*opl_lock_func)();
typedef void (*opl_unlock_func)();
typedef void (*opl_set_paused_func)(int paused);
typedef void (*opl_adjust_callbacks_func)(float value);

struct opl_driver_t
{
	const char* name;

	opl_init_func init_func;
	opl_shutdown_func shutdown_func;
	opl_read_port_func read_port_func;
	opl_write_port_func write_port_func;
	opl_set_callback_func set_callback_func;
	opl_clear_callbacks_func clear_callbacks_func;
	opl_lock_func lock_func;
	opl_unlock_func unlock_func;
	opl_set_paused_func set_paused_func;
	opl_adjust_callbacks_func adjust_callbacks_func;
};

// Sample rate to use when doing software emulation.
unsigned int opl_sample_rate = 22050;
