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

#include "../derma/common.h"

#include <SDL_mutex.h>

struct delay_data_t
{
	bool finished;

	SDL_mutex* mutex;
	SDL_cond* cond;
};

typedef void (*opl_callback_t)(delay_data_t* data);

// Result from OPL_Init(), indicating what type of OPL chip was detected, if any.
enum class opl_init_result_t
{
	OPL_INIT_NONE,
	OPL_INIT_OPL2,
	OPL_INIT_OPL3
};

enum class opl_port_t
{
	OPL_REGISTER_PORT = 0,
	OPL_DATA_PORT = 1,
	OPL_REGISTER_PORT_OPL3 = 2
};

constexpr size_t OPL_NUM_OPERATORS{21};
constexpr size_t OPL_NUM_VOICES{9};

constexpr size_t OPL_REG_WAVEFORM_ENABLE{0x01};
constexpr size_t OPL_REG_TIMER1{0x02};
constexpr size_t OPL_REG_TIMER2{0x03};
constexpr size_t OPL_REG_TIMER_CTRL{0x04};
constexpr size_t OPL_REG_FM_MODE{0x08};
constexpr size_t OPL_REG_NEW{0x105};

// Operator registers (21 of each):
constexpr size_t OPL_REGS_TREMOLO{0x20};
constexpr size_t OPL_REGS_LEVEL{0x40};
constexpr size_t OPL_REGS_ATTACK{0x60};
constexpr size_t OPL_REGS_SUSTAIN{0x80};
constexpr size_t OPL_REGS_WAVEFORM{0xE0};

// Voice registers (9 of each):
constexpr size_t OPL_REGS_FREQ_1{0xA0};
constexpr size_t OPL_REGS_FREQ_2{0xB0};
constexpr size_t OPL_REGS_FEEDBACK{0xC0};

// Times
constexpr uint64_t OPL_SECOND{	1'000'000};
constexpr uint64_t OPL_MS{		1'000};
constexpr uint64_t OPL_US{		1};

// ======================================== //
// Low-level functions.
// ======================================== //

// Initialize the OPL subsystem.
opl_init_result_t OPL_Init(unsigned port_base);

// Shut down the OPL subsystem.
void OPL_Shutdown();

// Set the sample rate used for software emulation.
void OPL_SetSampleRate(unsigned rate);

// Write to one of the OPL I/O ports:
void OPL_WritePort(opl_port_t port, unsigned value);

// Read from one of the OPL I/O ports:
unsigned OPL_ReadPort(opl_port_t port);

// ======================================== //
// Higher-level functions.
// ======================================== //

// Read the cuurrent status byte of the OPL chip.
unsigned OPL_ReadStatus();

// Write to an OPL register.
void OPL_WriteRegister(int reg, int value);

// Perform a detection sequence to determine that an OPL chip is present.
opl_init_result_t OPL_Detect();

// Initialize all registers, performed on startup.
void OPL_InitRegisters(int opl3);

// ======================================== //
// Timer callback functions.
// ======================================== //

// Set a timer callback. After the specified number of microseconds have elapsed, the callback will be invoked.
void OPL_SetCallback(uint64_t us, opl_callback_t callback, delay_data_t* data);

// Adjust callback times by the specified factor. For example, a value of 0.5 will halve all remaining times.
void OPL_AdjustCallbacks(float factor);

// Clear all OPL callbacks that have been set.
void OPL_ClearCallbacks();

// Begin critical section, during which, OPL callbacks will not be invoked.
void OPL_Lock();

// End critical section.
void OPL_Unlock();

// Block until the specified number of microseconds have elapsed.
void OPL_Delay(uint64_t us);

// Pause the OPL callbacks.
void OPL_SetPaused(bool paused);
