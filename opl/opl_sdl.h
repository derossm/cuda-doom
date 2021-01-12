/**********************************************************************************************************************************************\
	Copyright(C) 2005-2014 Simon Howard

	This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License
	as published by the Free Software Foundation; either version 2 of the License, or (at your option) any later version.

	This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

	DESCRIPTION:
		OPL callback queue.
\**********************************************************************************************************************************************/
#pragma once

#include "config.h"

#include "../derma/common.h"

//#include <stdio.h>
//#include <stdlib.h>
//#include <string.h>
//#include <errno.h>
#include <cassert>
//#include <inttypes.h>

#include <mutex>
#include <vector>
#include <ranges>
#include <algorithm>

#include <SDL.h>
#include <SDL_mixer.h>

#include "opl3.h"

#include "opl.h"

namespace cudadoom::opl
{

constexpr uint64_t MAX_SOUND_SLICE_TIME = 100; /* ms */

struct opl_timer_t
{
	TimeType expire_time{0ull};	// Calculated time that timer will expire.
	unsigned rate;			// Number of times the timer is advanced per sec.
	unsigned value{0u};			// Last value that was set.
	bool enabled{false};			// Non-zero if timer is enabled.
};

class OPLManager
{
	uint8_t* mix_buffer{nullptr};					// Temporary mixing buffer used by the mixing callback.

	TimeType current_time{0ull};					// Current time, in us since startup:
	TimeType pause_offset{0ull};					// Time offset (in us) due to the fact that callbacks were previously paused.

	::std::vector<QueueEntry> callback_queue;		// Queue of callbacks waiting to be invoked.
	::std::mutex callback_mutex;					// When the callback mutex is locked using OPL_Lock, callback functions are not invoked.
	::std::mutex callback_queue_mutex;				// Mutex used to control access to the callback queue.

	int opl_sdl_paused{0};							// If non-zero, playback is currently paused.
	int opl_opl3mode{0};
	int register_num{0};							// Register number that was written.

	// SDL parameters.
	int mixing_freq{0};
	int mixing_channels{0};
	uint16_t mixing_format{0u};
	bool sdl_was_initialized{false};

	// Timers; DBOPL does not do timer stuff itself.
	opl_timer_t timer1{ .rate=12500u };
	opl_timer_t timer2{ .rate=3125u };

	Chip opl_chip; // OPL software emulator structure.

	int SDLIsInitialized()
	{
		int freq;
		int channels;
		uint16_t format;

		return Mix_QuerySpec(&freq, &format, &channels);
	}

	// Advance time by the specified number of samples, invoking any callback functions as appropriate.
	void AdvanceTime(uint64_t nsamples)
	{
		opl_callback_t callback;
		delay_data_t* callback_data;
		uint64_t us;

		::std::unique_lock queue_lock(callback_queue_mutex, ::std::defer_lock);
		queue_lock.lock();

		// Advance time.
		us = (nsamples * SECOND) / mixing_freq;
		current_time += us;

		if (opl_sdl_paused)
		{
			pause_offset += us;
		}

		// Are there callbacks to invoke now? Keep invoking them until there are no more left.
		while (callback_queue.size() && current_time >= callback_queue.front().time + pause_offset)
		{
			// Pop the callback from the queue to invoke it.
			//if (!callback_queue.Queue_Pop(callback_queue, &callback, &callback_data))
			{
				//break;
			}

			// The mutex stuff here is a bit complicated. We must hold callback_mutex when we invoke the callback (so that
			// the control thread can use OPL_Lock() to prevent callbacks from being invoked), but we must not be holding
			// callback_queue_mutex, as the callback must be able to call OPL_SetCallback to schedule new callbacks.
			::std::scoped_lock lock(callback_mutex);
			queue_lock.unlock();
			callback(callback_data);
			queue_lock.lock();
		}
	}

	// Call the OPL emulator code to fill the specified buffer.
	void FillBuffer(uint8_t* buffer, uint32_t nsamples)
	{
		// This seems like a reasonable assumption. mix_buffer is 1 second long,
		// which should always be much longer than the SDL mix buffer.
		assert(nsamples < mixing_freq);

		// OPL output is generated into temporary buffer and then mixed (to avoid overflows etc.)
		mix_buffer = opl_chip.GenerateStream(nsamples);
		SDL_MixAudioFormat(buffer, mix_buffer, AUDIO_S16SYS, nsamples * 4, SDL_MIX_MAXVOLUME);
	}

	// SDL API NOTE SIGNATURE DO NOT CHANGE
	// Callback function to fill a new sound buffer:
	void Mix_Callback(void* udata, Uint8* buffer, int len)
	{
		unsigned filled;
		unsigned buffer_samples;

		// Repeatedly call the OPL emulator update function until the buffer is full.
		filled = 0;
		buffer_samples = len / 4;

		while (filled < buffer_samples)
		{
			uint64_t next_callback_time;
			uint64_t nsamples;

			// Work out the time until the next callback waiting in the callback queue must be invoked.
			// We can then fill the buffer with this many samples.
			if (::std::scoped_lock lock; opl_sdl_paused || !callback_queue.size())
			{
				nsamples = buffer_samples - filled;
			}
			else
			{
				//next_callback_time = OPL_Queue_Peek(callback_queue) + pause_offset;
				next_callback_time = callback_queue.front().time + pause_offset;

				nsamples = (next_callback_time - current_time) * mixing_freq;
				nsamples = (nsamples + SECOND - 1) / SECOND;

				if (nsamples > buffer_samples - filled)
				{
					nsamples = buffer_samples - filled;
				}
			}

			// Add emulator output to buffer.
			FillBuffer(buffer + filled * 4, nsamples);
			filled += nsamples;

			// Invoke callbacks for this point in time.
			AdvanceTime(nsamples);
		}
	}

	void SDL_Shutdown()
	{
		Mix_HookMusic(NULL, NULL);

		if (sdl_was_initialized)
		{
			Mix_CloseAudio();
			SDL_QuitSubSystem(SDL_INIT_AUDIO);
			//Queue_Destroy(callback_queue);
			callback_queue.clear();
			//free(mix_buffer);
			sdl_was_initialized = false;
		}

		/*
		if (opl_chip != NULL)
		{
			OPLDestroy(opl_chip);
			opl_chip = NULL;
		}
		*/
	}

	unsigned GetSliceSize()
	{
		unsigned limit{static_cast<unsigned>(opl_sample_rate * (MAX_SOUND_SLICE_TIME / 1000u))};

		// Try all powers of two, not exceeding the limit.
		for (int n{0};; ++n)
		{
			// 2^n <= limit < 2^n+1 ?

			if ((1u << (n + 1)) > limit)
			{
				return (1u << n);
			}
		}

		// Should never happen?
		return 1024;
	}

	bool SDL_Init()
	{
		// Check if SDL_mixer has been opened already; If not, we must initialize it now
		if (!SDLIsInitialized())
		{
			// FIXME this is a weird recursion, pointlessly convoluted way to do this
			if (!SDL_Init())
			{
				fprintf(stderr, "Unable to set up sound.\n");
				return false;
			}

			if (Mix_OpenAudioDevice(opl_sample_rate, AUDIO_S16SYS, 2, GetSliceSize(), NULL, SDL_AUDIO_ALLOW_FREQUENCY_CHANGE) < 0)
			{
				fprintf(stderr, "Error initialising SDL_mixer: %s\n", Mix_GetError());

				SDL_QuitSubSystem(SDL_INIT_AUDIO);
				return false;
			}

			SDL_PauseAudio(0);

			// When this module shuts down, it has the responsibility to shut down SDL.
			sdl_was_initialized = true;
		}
		else
		{
			sdl_was_initialized = false;
		}

		opl_sdl_paused = 0;
		pause_offset = 0;

		// Queue structure of callbacks to invoke.
		//callback_queue = OPL_Queue_Create();
		callback_queue.reserve(MAX_OPL_QUEUE);
		current_time = 0;

		// Get the mixer frequency, format and number of channels.
		Mix_QuerySpec(&mixing_freq, &mixing_format, &mixing_channels);

		// Only supports AUDIO_S16SYS
		if (mixing_format != AUDIO_S16SYS || mixing_channels != 2)
		{
			fprintf(stderr, "OPL_SDL only supports native signed 16-bit LSB, stereo format!\n");

			SDL_Shutdown();
			return false;
		}

		// Mix buffer: four bytes per sample (16 bits * 2 channels):
		mix_buffer = static_cast<decltype(mix_buffer)>(malloc(mixing_freq * 4));

		// Create the emulator structure:
		opl_chip.Reset(mixing_freq);
		opl_opl3mode = 0;

		//callback_mutex = SDL_CreateMutex();
		//callback_queue_mutex = SDL_CreateMutex();

		// Set postmix that adds the OPL music. This is deliberately done
		// as a postmix and not using Mix_HookMusic() as the latter disables
		// normal SDL_mixer music mixing.
		//Mix_SetPostMix(Mix_Callback, nullptr); //FIXME

		return true;
	}

	unsigned SDL_PortRead(opl_port_t port)
	{
		unsigned result{0};

		if (port == opl_port_t::REGISTER_PORT_OPL3)
		{
			return 0xff;
		}

		if (timer1.enabled && current_time > timer1.expire_time)
		{
			result |= 0x80;	// Either have expired
			result |= 0x40;	// Timer 1 has expired
		}

		if (timer2.enabled && current_time > timer2.expire_time)
		{
			result |= 0x80;	// Either have expired
			result |= 0x20;	// Timer 2 has expired
		}

		return result;
	}

	void OPLTimer_CalculateEndTime(opl_timer_t* timer)
	{
		// If the timer is enabled, calculate the time when the timer will expire.
		if (timer->enabled)
		{
			uint64_t tics{0x100 - static_cast<uint64_t>(timer->value)};
			// TODO NOTE is timer->rate bigger than opl_second? we risk either rounding to zero or overflow from the multiplication
			// if only floating point arithmetic was a thing...
			timer->expire_time = current_time + (tics * SECOND) / timer->rate;
		}
	}

	void WriteRegister(unsigned reg_num, unsigned value)
	{
		switch (reg_num)
		{
		case REG_TIMER1:
			timer1.value = value;
			OPLTimer_CalculateEndTime(&timer1);
			break;

		case REG_TIMER2:
			timer2.value = value;
			OPLTimer_CalculateEndTime(&timer2);
			break;

		case REG_TIMER_CTRL:
			if (value & 0x80)
			{
				timer1.enabled = 0;
				timer2.enabled = 0;
			}
			else
			{
				if ((value & 0x40) == 0)
				{
					timer1.enabled = (value & 0x01) != 0;
					OPLTimer_CalculateEndTime(&timer1);
				}

				if ((value & 0x20) == 0)
				{
					timer1.enabled = (value & 0x02) != 0;
					OPLTimer_CalculateEndTime(&timer2);
				}
			}

			break;

		case REG_NEW:
			opl_opl3mode = value & 0x01;

		default:
			opl_chip.WriteRegBuffered(reg_num, value);
			break;
		}
	}

	void SDL_PortWrite(opl_port_t port, unsigned value)
	{
		if (port == opl_port_t::REGISTER_PORT)
		{
			register_num = value;
		}
		else if (port == opl_port_t::REGISTER_PORT_OPL3)
		{
			register_num = value | 0x100;
		}
		else if (port == opl_port_t::DATA_PORT)
		{
			WriteRegister(register_num, value);
		}
	}

	void SDL_SetCallback(TimeType us, opl_callback_t callback, delay_data_t* data)
	{
		::std::scoped_lock lock(callback_queue_mutex);
		//callback_queue.Queue_Push(callback, data, current_time - pause_offset + us);
		callback_queue.emplace_back(QueueEntry{.data=data, .callback=callback, .time=(current_time + us - pause_offset)});
	}

	void SDL_ClearCallbacks()
	{
		::std::scoped_lock lock(callback_queue_mutex);
		//callback_queue.Queue_Clear();
		callback_queue.clear();
	}

	void SDL_SetPaused(int paused)
	{
		::std::scoped_lock queue_lock(callback_queue_mutex);
		opl_sdl_paused = paused;
	}

	void SDL_AdjustCallbacks(float factor)
	{
		::std::scoped_lock lock(callback_queue_mutex);
		//callback_queue.Queue_AdjustCallbacks(current_time, factor);
		std::ranges::for_each(callback_queue, [&](auto &iter)
			{
				TimeType offset = iter.time - current_time;
				iter.time = current_time + TimeType(offset/factor);
			});
	}
};
/*
static OPLManager manager;

opl_driver_t opl_sdl_driver =
{
	"SDL",
	&manager.SDL_Init,
	&manager.SDL_Shutdown,
	&manager.SDL_PortRead,
	&manager.SDL_PortWrite,
	&manager.SDL_SetCallback,
	&manager.SDL_ClearCallbacks,
	&manager.SDL_SetPaused,
	&manager.SDL_AdjustCallbacks,
};
*/
} // end namespace cudadoom::opl
