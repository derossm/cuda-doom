/**********************************************************************************************************************************************\
	Copyright(C) 2005-2014 Simon Howard

	This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License
	as published by the Free Software Foundation; either version 2 of the License, or (at your option) any later version.

	This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

	DESCRIPTION:
		OPL interface.
\**********************************************************************************************************************************************/
#pragma once

#include "../build/config.h"

#include <SDL.h>
//#include <SDL_mutex.h>

#include "../derma/common.h"
#include "../derma/d_native.h"

//#define DEBUG_TRACE

namespace cudadoom::opl
{

// Result from Init(), indicating what type of OPL chip was detected, if any.
enum class opl_init_result_t
{
	NONE,
	OPL2,
	OPL3
};

enum class opl_port_t
{
	REGISTER_PORT = 0,
	DATA_PORT = 1,
	REGISTER_PORT_OPL3 = 2
};

struct delay_data_t
{
	bool finished;

	::std::mutex mutex;
	SDL_cond* cond;
};

typedef void (*opl_callback_t)(delay_data_t* data);

typedef bool (*opl_init_func)(uint64_t port_base);
typedef void (*opl_shutdown_func)();
typedef byte (*opl_read_port_func)(opl_port_t port);
typedef void (*opl_write_port_func)(opl_port_t port, byte value);
typedef void (*opl_set_callback_func)(TimeType us, opl_callback_t callback, delay_data_t* data);
typedef void (*opl_clear_callbacks_func)();
typedef void (*opl_set_paused_func)(bool paused);
typedef void (*opl_adjust_callbacks_func)(double value);

struct opl_driver_t
{
	::std::string name;

	opl_init_func init_func;
	opl_shutdown_func shutdown_func;
	opl_read_port_func read_port_func;
	opl_write_port_func write_port_func;
	opl_set_callback_func set_callback_func;
	opl_clear_callbacks_func clear_callbacks_func;
	opl_set_paused_func set_paused_func;
	opl_adjust_callbacks_func adjust_callbacks_func;
};

constexpr size_t MAX_OPL_QUEUE{64};

enum class thread_state_t
{
	stopped,
	running,
	stopping
};

struct QueueEntry
{
	delay_data_t* data{nullptr};
	opl_callback_t callback;
	TimeType time;
};

constexpr uint8_t NUM_OPERATORS{21};
constexpr uint8_t NUM_VOICES{9};

constexpr uint8_t REG_WAVEFORM_ENABLE{0x01};
constexpr uint8_t REG_TIMER1{0x02};
constexpr uint8_t REG_TIMER2{0x03};
constexpr uint8_t REG_TIMER_CTRL{0x04};
constexpr uint8_t REG_FM_MODE{0x08};
constexpr uint16_t REG_NEW{0x105}; // FIXME

// Operator registers (21 of each):
constexpr uint8_t REGS_TREMOLO{0x20};
constexpr uint8_t REGS_LEVEL{0x40};
constexpr uint8_t REGS_ATTACK{0x60};
constexpr uint8_t REGS_SUSTAIN{0x80};
constexpr uint8_t REGS_WAVEFORM{0xE0};

// Voice registers (9 of each):
constexpr uint8_t REGS_FREQ_1{0xA0};
constexpr uint8_t REGS_FREQ_2{0xB0};
constexpr uint8_t REGS_FEEDBACK{0xC0};

// Times
static constexpr TimeType		US{1}; // microseconds
static constexpr TimeType		MS{US*1'000}; // milliseconds
static constexpr TimeType	SECOND{MS*1'000}; // seconds

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

static opl_driver_t* drivers[]{
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

// Sample rate to use when doing software emulation.
uint64_t opl_sample_rate = 22050;

static opl_driver_t* driver{nullptr};

static bool init_stage_reg_writes = true;

// Shut down the OPL library.
void Shutdown()
{
	if (driver)
	{
		driver->shutdown_func();
		driver = nullptr;
	}
}

// Set the sample rate used for software OPL emulation.
void SetSampleRate(uint64_t rate)
{
	opl_sample_rate = rate;
}

void WritePort(opl_port_t port, byte value)
{
	if (driver)
	{
#ifdef DEBUG_TRACE
		printf("write: %i, %x\n", port, value);
		fflush(stdout);
#endif
		driver->write_port_func(port, value);
	}
}

auto ReadPort(opl_port_t port)
{
	if (driver)
	{
#ifdef DEBUG_TRACE
		printf("read: %i...\n", port);
		fflush(stdout);
#endif

		auto result{driver->read_port_func(port)};

#ifdef DEBUG_TRACE
		printf("read: %i -> %x\n", port, result);
		fflush(stdout);
#endif

		return result;
	}
	else
	{
		return ::std::byte(0x00);
	}
}

// Higher-level functions, based on the lower-level functions above (register write, etc).
auto ReadStatus()
{
	return ReadPort(opl_port_t::REGISTER_PORT);
}

// Write an OPL register value
void WriteRegister(uint64_t reg, byte value)
{
	// TODO INVESTIGATE FIXME
	if (reg & 0x100)
	{
		WritePort(opl_port_t::REGISTER_PORT_OPL3, std::byte(reg));
	}
	else
	{
		WritePort(opl_port_t::REGISTER_PORT, std::byte(reg));
	}

	// For timing, read the register port six times after writing the register number to cause the appropriate delay
	for (size_t i{0}; i < 6; ++i)
	{
		// An oddity of the Doom OPL code: at startup initialization, the spacing here is performed by
		// reading from the register port; after initialization, the data port is read, instead.
		if (init_stage_reg_writes)
		{
			ReadPort(opl_port_t::REGISTER_PORT);
		}
		else
		{
			ReadPort(opl_port_t::DATA_PORT);
		}
	}

	WritePort(opl_port_t::DATA_PORT, value);

	// Read the register port 24 times after writing the value to cause the appropriate delay
	for (size_t i{0}; i < 24; ++i)
	{
		ReadStatus();
	}
}

// Initialize registers on startup
void InitRegisters(bool opl3)
{
	// Initialize level registers
	for (size_t r{REGS_LEVEL}; r <= REGS_LEVEL + NUM_OPERATORS; ++r)
	{
		WriteRegister(r, ::std::byte(0x3f));
	}

	// Initialize other registers
	// These two loops write to registers that actually don't exist, but this is what Doom does ...
	// Similarly, the <= is also intenational.
	for (size_t r{REGS_ATTACK}; r <= REGS_WAVEFORM + NUM_OPERATORS; ++r)
	{
		WriteRegister(r, ::std::byte(0x00));
	}

	// More registers ...
	for (size_t r{1}; r < REGS_LEVEL; ++r)
	{
		WriteRegister(r, ::std::byte(0x00));
	}

	// Re-initialize the low registers:

	// Reset both timers and enable interrupts:
	WriteRegister(REG_TIMER_CTRL, ::std::byte(0x60));
	WriteRegister(REG_TIMER_CTRL, ::std::byte(0x80));

	// "Allow FM chips to control the waveform of each operator":
	WriteRegister(REG_WAVEFORM_ENABLE, ::std::byte(0x20));

	if (opl3)
	{
		WriteRegister(REG_NEW, ::std::byte(0x01));

		// Initialize level registers
		for (size_t r{REGS_LEVEL}; r <= REGS_LEVEL + NUM_OPERATORS; ++r)
		{
			WriteRegister(r | 0x100, ::std::byte(0x3f));
		}

		// Initialize other registers; These two loops write to registers that actually don't exist,
		// but this is what Doom does ... Similarly, the <= is also intenational.
		for (size_t r{REGS_ATTACK}; r <= REGS_WAVEFORM + NUM_OPERATORS; ++r)
		{
			WriteRegister(r | 0x100, ::std::byte(0x00));
		}

		// More registers ...
		for (size_t r{1}; r < REGS_LEVEL; ++r)
		{
			WriteRegister(r | 0x100, ::std::byte(0x00));
		}
	}

	// Keyboard split point on (?)
	WriteRegister(REG_FM_MODE, ::std::byte(0x40));

	if (opl3)
	{
		WriteRegister(REG_NEW, ::std::byte(0x01));
	}
}

// Timer functions.
// FIXME FIND A COMMON TYPE FOR THIS VOID
void SetCallback(TimeType us, opl_callback_t callback, delay_data_t* data)
{
	if (driver)
	{
		driver->set_callback_func(us, callback, data);
	}
}

void ClearCallbacks()
{
	if (driver)
	{
		driver->clear_callbacks_func();
	}
}

void DelayCallback(delay_data_t* _delay_data)
{
	auto delay_data{_delay_data};

	::std::scoped_lock lock(delay_data->mutex);
	delay_data->finished = true;

	SDL_CondSignal(delay_data->cond);
}

void Delay(TimeType us)
{
	if (driver)
	{
		return;
	}

	// Create a callback that will signal this thread after the specified time.
	delay_data_t delay_data{
		.finished = false,
		.mutex = ::std::mutex{},
		.cond = SDL_CreateCond()
	};

	SetCallback(us, DelayCallback, &delay_data);

	// Wait until the callback is invoked.

	//for (::std::scoped_lock lock(delay_data.mutex); !delay_data.finished; )
	{
		//SDL_CondWait(delay_data.cond, delay_data.mutex);
	}

	SDL_DestroyCond(delay_data.cond);
}

void SetPaused(bool paused)
{
	if (driver)
	{
		driver->set_paused_func(paused);
	}
}

void AdjustCallbacks(double value)
{
	if (driver)
	{
		driver->adjust_callbacks_func(value);
	}
}

// Detect the presence of an OPL chip
auto Detect()
{
	// Reset both timers:
	WriteRegister(REG_TIMER_CTRL, std::byte(0x60));

	// Enable interrupts:
	WriteRegister(REG_TIMER_CTRL, std::byte(0x80));

	// Read status
	auto result1{ReadStatus()};

	// Set timer:
	WriteRegister(REG_TIMER1, std::byte(0xff));

	// Start timer 1:
	WriteRegister(REG_TIMER_CTRL, std::byte(0x21));

	// Wait for 80 microseconds. This is how Doom does it:
	for (size_t i{0}; i < 200; ++i)
	{
		ReadStatus();
	}

	Delay(1 * MS);

	// Read status
	auto result2{ReadStatus()};

	// Reset both timers:
	WriteRegister(REG_TIMER_CTRL, std::byte(0x60));

	// Enable interrupts:
	WriteRegister(REG_TIMER_CTRL, std::byte(0x80));

	if ((result1 & std::byte(0xe0)) == std::byte(0x00) && (result2 & std::byte(0xe0)) == std::byte(0xc0))
	{
		result1 = ReadPort(opl_port_t::REGISTER_PORT);
		result2 = ReadPort(opl_port_t::REGISTER_PORT_OPL3);
		if (result1 == std::byte(0x00))
		{
			return opl_init_result_t::OPL3;
		}
		else
		{
			return opl_init_result_t::OPL2;
		}
	}
	else
	{
		return opl_init_result_t::NONE;
	}
}

// Initialize the specified driver and detect an OPL chip. Returns true if an OPL is detected.
static auto InitDriver(opl_driver_t* _driver, uint64_t port_base)
{
	// Initialize the driver.
	if (!_driver->init_func(port_base))
	{
		return opl_init_result_t::NONE;
	}

	// The driver was initialized okay, so we now have somewhere to write to. It doesn't mean there's an OPL chip there,
	// though. Perform the detection sequence to make sure. (it's done twice, like how Doom does it).
	driver = _driver;
	init_stage_reg_writes = true;

	auto result1{Detect()};
	auto result2{Detect()};

	if (result1 == opl_init_result_t::NONE || result2 == opl_init_result_t::NONE)
	{
		printf("Init: No OPL detected using '%s' driver.\n", _driver->name.c_str());
		_driver->shutdown_func();
		driver = nullptr;
		return opl_init_result_t::NONE;
	}

	init_stage_reg_writes = false;

	printf("Init: Using driver '%s'.\n", driver->name.c_str());

	return result2;
}

// Find a driver automatically by trying each in the list.
static auto AutoSelectDriver(uint64_t port_base)
{
	for (size_t i{0}; drivers[i]; ++i)
	{
		auto result = InitDriver(drivers[i], port_base);
		if (result != opl_init_result_t::NONE)
		{
			return result;
		}
	}

	printf("Init: Failed to find a working driver.\n");

	return opl_init_result_t::NONE;
}

// Initialize the OPL library. Return value indicates type of OPL chip detected, if any.
auto Init(uint64_t port_base)
{
	char** buffer;
	size_t* numberOfElements;

	// ANNOYING RETURN BY PARAMATER WINDOWS FUNCTION
	// https://docs.microsoft.com/en-us/cpp/c-runtime-library/reference/dupenv-s-wdupenv-s?view=msvc-160
	if (auto error{::_dupenv_s(buffer, numberOfElements, "DRIVER")};
			error)
	{
		// _dupenv_s: error if return is non-zero, could handle error code here if you want
		return opl_init_result_t::NONE;
	}

	::std::string driver_name((const char*)*buffer, *numberOfElements);
	if (!driver_name.empty())
	{
		// Search the list until we find the driver with this name.
		for (size_t i{0}; drivers[i]; ++i)
		{
			if (!driver_name.compare(drivers[i]->name))
			{
				auto result{InitDriver(drivers[i], port_base)};
				if (result == opl_init_result_t::NONE)
				{
					printf("Init: Failed to initialize driver: '%s'.\n", driver_name.c_str());
				}

				return result;
			}
		}

		printf("Init: unknown driver: '%s'.\n", driver_name.c_str());

		return opl_init_result_t::NONE;
	}
	else
	{
		return AutoSelectDriver(port_base);
	}
}

} // end namespace cudadoom::opl
