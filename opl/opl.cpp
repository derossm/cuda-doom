/**********************************************************************************************************************************************\
	Copyright(C) 2005-2014 Simon Howard

	This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License
	as published by the Free Software Foundation; either version 2 of the License, or (at your option) any later version.

	This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

	DESCRIPTION:
		OPL interface.
\**********************************************************************************************************************************************/

#include <string>

#include "config.h"
#include "../derma/d_native.h"

#include "SDL.h"

#include "opl.h"
#include "opl_internal.h"

//#define OPL_DEBUG_TRACE

#if (defined(__i386__) || defined(__x86_64__)) && defined(HAVE_IOPERM)
extern opl_driver_t opl_linux_driver;
#endif
#if defined(HAVE_LIBI386) || defined(HAVE_LIBAMD64)
extern opl_driver_t opl_openbsd_driver;
#endif
#ifdef _WIN32
extern opl_driver_t opl_win32_driver;
#endif

extern opl_driver_t opl_sdl_driver;

static opl_driver_t* drivers[] =
{
#if (defined(__i386__) || defined(__x86_64__)) && defined(HAVE_IOPERM)
	&opl_linux_driver,
#endif
#if defined(HAVE_LIBI386) || defined(HAVE_LIBAMD64)
	&opl_openbsd_driver,
#endif
#ifdef _WIN32
	& opl_win32_driver,
#endif
	& opl_sdl_driver,
	nullptr
};

static opl_driver_t* driver{nullptr};
static int init_stage_reg_writes = 1;

// Initialize the specified driver and detect an OPL chip. Returns true if an OPL is detected.
static opl_init_result_t InitDriver(opl_driver_t* _driver, unsigned port_base)
{
	// Initialize the driver.
	if (!_driver->init_func(port_base))
	{
		return opl_init_result_t::OPL_INIT_NONE;
	}

	// The driver was initialized okay, so we now have somewhere to write to. It doesn't mean there's an OPL chip there,
	// though. Perform the detection sequence to make sure. (it's done twice, like how Doom does it).
	driver = _driver;
	init_stage_reg_writes = 1;

	auto result1{OPL_Detect()};
	auto result2{OPL_Detect()};

	if (result1 == opl_init_result_t::OPL_INIT_NONE || result2 == opl_init_result_t::OPL_INIT_NONE)
	{
		printf("OPL_Init: No OPL detected using '%s' driver.\n", _driver->name.c_str());
		_driver->shutdown_func();
		driver = nullptr;
		return opl_init_result_t::OPL_INIT_NONE;
	}

	init_stage_reg_writes = 0;

	printf("OPL_Init: Using driver '%s'.\n", driver->name.c_str());

	return result2;
}

// Find a driver automatically by trying each in the list.
static opl_init_result_t AutoSelectDriver(unsigned port_base)
{
	for (size_t i{0}; drivers[i]; ++i)
	{
		auto result = InitDriver(drivers[i], port_base);
		if (result != opl_init_result_t::OPL_INIT_NONE)
		{
			return result;
		}
	}

	printf("OPL_Init: Failed to find a working driver.\n");

	return opl_init_result_t::OPL_INIT_NONE;
}

// Initialize the OPL library. Return value indicates type of OPL chip detected, if any.
opl_init_result_t OPL_Init(unsigned port_base)
{
	::std::string driver_name{::std::getenv("OPL_DRIVER")};

	if (!driver_name.empty())
	{
		// Search the list until we find the driver with this name.
		for (size_t i{0}; drivers[i]; ++i)
		{
			if (!driver_name.compare(drivers[i]->name))
			{
				auto result{InitDriver(drivers[i], port_base)};
				if (result == opl_init_result_t::OPL_INIT_NONE)
				{
					printf("OPL_Init: Failed to initialize driver: '%s'.\n", driver_name.c_str());
				}

				return result;
			}
		}

		printf("OPL_Init: unknown driver: '%s'.\n", driver_name.c_str());

		return opl_init_result_t::OPL_INIT_NONE;
	}
	else
	{
		return AutoSelectDriver(port_base);
	}
}

// Shut down the OPL library.
void OPL_Shutdown()
{
	if (driver)
	{
		driver->shutdown_func();
		driver = nullptr;
	}
}

// Set the sample rate used for software OPL emulation.
void OPL_SetSampleRate(unsigned rate)
{
	opl_sample_rate = rate;
}

void OPL_WritePort(opl_port_t port, unsigned value)
{
	if (driver)
	{
#ifdef OPL_DEBUG_TRACE
		printf("OPL_write: %i, %x\n", port, value);
		fflush(stdout);
#endif
		driver->write_port_func(port, value);
	}
}

unsigned OPL_ReadPort(opl_port_t port)
{
	if (driver)
	{
#ifdef OPL_DEBUG_TRACE
		printf("OPL_read: %i...\n", port);
		fflush(stdout);
#endif

		auto result{driver->read_port_func(port)};

#ifdef OPL_DEBUG_TRACE
		printf("OPL_read: %i -> %x\n", port, result);
		fflush(stdout);
#endif

		return result;
	}
	else
	{
		return 0;
	}
}

// Higher-level functions, based on the lower-level functions above (register write, etc).
unsigned OPL_ReadStatus()
{
	return OPL_ReadPort(opl_port_t::OPL_REGISTER_PORT);
}

// Write an OPL register value
void OPL_WriteRegister(unsigned reg, unsigned value)
{
	if (reg & 0x100)
	{
		OPL_WritePort(opl_port_t::OPL_REGISTER_PORT_OPL3, reg);
	}
	else
	{
		OPL_WritePort(opl_port_t::OPL_REGISTER_PORT, reg);
	}

	// For timing, read the register port six times after writing the register number to cause the appropriate delay
	for (size_t i{0}; i < 6; ++i)
	{
		// An oddity of the Doom OPL code: at startup initialization, the spacing here is performed by
		// reading from the register port; after initialization, the data port is read, instead.
		if (init_stage_reg_writes)
		{
			OPL_ReadPort(opl_port_t::OPL_REGISTER_PORT);
		}
		else
		{
			OPL_ReadPort(opl_port_t::OPL_DATA_PORT);
		}
	}

	OPL_WritePort(opl_port_t::OPL_DATA_PORT, value);

	// Read the register port 24 times after writing the value to cause the appropriate delay
	for (size_t i{0}; i < 24; ++i)
	{
		OPL_ReadStatus();
	}
}

// Detect the presence of an OPL chip
opl_init_result_t OPL_Detect()
{
	// Reset both timers:
	OPL_WriteRegister(OPL_REG_TIMER_CTRL, 0x60);

	// Enable interrupts:
	OPL_WriteRegister(OPL_REG_TIMER_CTRL, 0x80);

	// Read status
	auto result1{OPL_ReadStatus()};

	// Set timer:
	OPL_WriteRegister(OPL_REG_TIMER1, 0xff);

	// Start timer 1:
	OPL_WriteRegister(OPL_REG_TIMER_CTRL, 0x21);

	// Wait for 80 microseconds. This is how Doom does it:
	for (size_t i{0}; i < 200; ++i)
	{
		OPL_ReadStatus();
	}

	OPL_Delay(1 * OPL_MS);

	// Read status
	auto result2{OPL_ReadStatus()};

	// Reset both timers:
	OPL_WriteRegister(OPL_REG_TIMER_CTRL, 0x60);

	// Enable interrupts:
	OPL_WriteRegister(OPL_REG_TIMER_CTRL, 0x80);

	if ((result1 & 0xe0) == 0x00 && (result2 & 0xe0) == 0xc0)
	{
		result1 = OPL_ReadPort(opl_port_t::OPL_REGISTER_PORT);
		result2 = OPL_ReadPort(opl_port_t::OPL_REGISTER_PORT_OPL3);
		if (result1 == 0x00)
		{
			return opl_init_result_t::OPL_INIT_OPL3;
		}
		else
		{
			return opl_init_result_t::OPL_INIT_OPL2;
		}
	}
	else
	{
		return opl_init_result_t::OPL_INIT_NONE;
	}
}

// Initialize registers on startup
void OPL_InitRegisters(unsigned opl3)
{
	// Initialize level registers
	for (size_t r{OPL_REGS_LEVEL}; r <= OPL_REGS_LEVEL + OPL_NUM_OPERATORS; ++r)
	{
		OPL_WriteRegister(r, 0x3f);
	}

	// Initialize other registers
	// These two loops write to registers that actually don't exist, but this is what Doom does ...
	// Similarly, the <= is also intenational.
	for (size_t r{OPL_REGS_ATTACK}; r <= OPL_REGS_WAVEFORM + OPL_NUM_OPERATORS; ++r)
	{
		OPL_WriteRegister(r, 0x00);
	}

	// More registers ...
	for (size_t r{1}; r < OPL_REGS_LEVEL; ++r)
	{
		OPL_WriteRegister(r, 0x00);
	}

	// Re-initialize the low registers:

	// Reset both timers and enable interrupts:
	OPL_WriteRegister(OPL_REG_TIMER_CTRL, 0x60);
	OPL_WriteRegister(OPL_REG_TIMER_CTRL, 0x80);

	// "Allow FM chips to control the waveform of each operator":
	OPL_WriteRegister(OPL_REG_WAVEFORM_ENABLE, 0x20);

	if (opl3)
	{
		OPL_WriteRegister(OPL_REG_NEW, 0x01);

		// Initialize level registers
		for (size_t r{OPL_REGS_LEVEL}; r <= OPL_REGS_LEVEL + OPL_NUM_OPERATORS; ++r)
		{
			OPL_WriteRegister(r | 0x100, 0x3f);
		}

		// Initialize other registers; These two loops write to registers that actually don't exist,
		// but this is what Doom does ... Similarly, the <= is also intenational.
		for (size_t r{OPL_REGS_ATTACK}; r <= OPL_REGS_WAVEFORM + OPL_NUM_OPERATORS; ++r)
		{
			OPL_WriteRegister(r | 0x100, 0x00);
		}

		// More registers ...
		for (size_t r{1}; r < OPL_REGS_LEVEL; ++r)
		{
			OPL_WriteRegister(r | 0x100, 0x00);
		}
	}

	// Keyboard split point on (?)
	OPL_WriteRegister(OPL_REG_FM_MODE, 0x40);

	if (opl3)
	{
		OPL_WriteRegister(OPL_REG_NEW, 0x01);
	}
}

// Timer functions.
// FIXME FIND A COMMON TYPE FOR THIS VOID
void OPL_SetCallback(uint64_t us, opl_callback_t callback, delay_data_t* data)
{
	if (driver)
	{
		driver->set_callback_func(us, callback, data);
	}
}

void OPL_ClearCallbacks()
{
	if (driver)
	{
		driver->clear_callbacks_func();
	}
}

void OPL_Lock()
{
	if (driver)
	{
		driver->lock_func();
	}
}

void OPL_Unlock()
{
	if (driver)
	{
		driver->unlock_func();
	}
}

static void DelayCallback(delay_data_t* _delay_data)
{
	auto delay_data{_delay_data};

	SDL_LockMutex(delay_data->mutex);
	delay_data->finished = true;

	SDL_CondSignal(delay_data->cond);

	SDL_UnlockMutex(delay_data->mutex);
}

void OPL_Delay(uint64_t us)
{
	if (driver)
	{
		return;
	}

	// Create a callback that will signal this thread after the specified time.
	delay_data_t delay_data{
		.finished = false,
		.mutex = SDL_CreateMutex(),
		.cond = SDL_CreateCond()
	};

	OPL_SetCallback(us, DelayCallback, &delay_data);

	// Wait until the callback is invoked.
	SDL_LockMutex(delay_data.mutex);

	while (!delay_data.finished)
	{
		SDL_CondWait(delay_data.cond, delay_data.mutex);
	}

	SDL_UnlockMutex(delay_data.mutex);

	// Clean up.
	SDL_DestroyMutex(delay_data.mutex);
	SDL_DestroyCond(delay_data.cond);
}

void OPL_SetPaused(bool paused)
{
	if (driver)
	{
		driver->set_paused_func(paused);
	}
}

void OPL_AdjustCallbacks(float value)
{
	if (driver)
	{
		driver->adjust_callbacks_func(value);
	}
}
