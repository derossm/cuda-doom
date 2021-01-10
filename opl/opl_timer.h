/**********************************************************************************************************************************************\
	Copyright(C) 2005-2014 Simon Howard

	This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License
	as published by the Free Software Foundation; either version 2 of the License, or (at your option) any later version.

	This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

	DESCRIPTION:
		OPL timer thread.
		Once started using OPL_Timer_StartThread, the thread sleeps, waking up to invoke callbacks set using OPL_Timer_SetCallback.
\**********************************************************************************************************************************************/
#pragma once

#include "../derma/common.h"

#include "SDL.h"

#include "opl.h"
#include "opl_queue.h"

namespace cudadoom::opl
{

enum class thread_state_t
{
	THREAD_STATE_STOPPED,
	THREAD_STATE_RUNNING,
	THREAD_STATE_STOPPING
};

static SDL_Thread* timer_thread = nullptr;
static thread_state_t timer_thread_state;
static TimeType current_time;

// If non-zero, callbacks are currently paused.
static int opl_timer_paused;

// Offset in microseconds to adjust time due to the fact that playback was paused.
static TimeType pause_offset = 0;

// Queue of callbacks waiting to be invoked. The callback queue mutex is held while the callback queue structure or current_time is being accessed.
static opl_callback_queue_t* callback_queue;
static SDL_mutex* callback_queue_mutex;

// The timer mutex is held while timer callback functions are being invoked, so that the calling code can prevent clashes.
static SDL_mutex* timer_mutex;

// Returns true if there is a callback at the head of the queue ready to be invoked.
// Otherwise, next_time is set to the time when the timer thread must wake up again to check.
static TimeType CallbackWaiting(TimeType* next_time)
{
	// If paused, just wait in 50ms increments until unpaused. Update pause_offset so after we unpause, the callback times will be right.
	if (opl_timer_paused)
	{
		*next_time = current_time + 50 * OPL_MS;
		pause_offset += 50 * OPL_MS;
		return 0;
	}

	// If there are no queued callbacks, sleep for 50ms at a time until a callback is added.
	if (OPL_Queue_IsEmpty(callback_queue))
	{
		*next_time = current_time + 50 * OPL_MS;
		return 0;
	}

	// Read the time of the first callback in the queue. If the time for the callback has not yet arrived, we must sleep until the callback time.
	*next_time = OPL_Queue_Peek(callback_queue) + pause_offset;

	return *next_time <= current_time;
}

static TimeType GetNextTime()
{
	opl_callback_t callback;
	delay_data_t* callback_data;
	TimeType next_time;
	TimeType have_callback;

	// Keep running through callbacks until there are none ready to run. When we run out of callbacks, next_time will be set.
	do
	{
		SDL_LockMutex(callback_queue_mutex);

		// Check if the callback at the head of the list is ready to be invoked. If so, pop from the head of the queue.
		have_callback = CallbackWaiting(&next_time);

		if (have_callback)
		{
			OPL_Queue_Pop(callback_queue, &callback, &callback_data);
		}

		SDL_UnlockMutex(callback_queue_mutex);

		// Now invoke the callback, if we have one. The timer mutex is held while the callback is invoked.
		if (have_callback)
		{
			SDL_LockMutex(timer_mutex);
			callback(callback_data);
			SDL_UnlockMutex(timer_mutex);
		}
	} while (have_callback);

	return next_time;
}

// signature restricted by SDL library
static int ThreadFunction(void* unused)
{
	TimeType next_time;
	TimeType now;

	// Keep running until OPL_Timer_StopThread is called.
	while (timer_thread_state == thread_state_t::THREAD_STATE_RUNNING)
	{
		// Get the next time that we must sleep until, and wait until that time.
		next_time = GetNextTime();
		now = SDL_GetTicks() * OPL_MS;

		if (next_time > now)
		{
			SDL_Delay((next_time - now) / OPL_MS);
		}

		// Update the current time.
		SDL_LockMutex(callback_queue_mutex);
		current_time = next_time;
		SDL_UnlockMutex(callback_queue_mutex);
	}

	timer_thread_state = thread_state_t::THREAD_STATE_STOPPED;

	return 0;
}

static void InitResources()
{
	callback_queue = OPL_Queue_Create();
	timer_mutex = SDL_CreateMutex();
	callback_queue_mutex = SDL_CreateMutex();
}

static void FreeResources()
{
	OPL_Queue_Destroy(callback_queue);
	SDL_DestroyMutex(callback_queue_mutex);
	SDL_DestroyMutex(timer_mutex);
}

bool OPL_Timer_StartThread()
{
	InitResources();

	timer_thread_state = thread_state_t::THREAD_STATE_RUNNING;
	current_time = SDL_GetTicks();
	opl_timer_paused = 0;
	pause_offset = 0;

	timer_thread = SDL_CreateThread(ThreadFunction, "OPL timer thread", NULL);

	if (timer_thread == nullptr)
	{
		timer_thread_state = thread_state_t::THREAD_STATE_STOPPED;
		FreeResources();

		return false;
	}

	return true;
}

void OPL_Timer_StopThread()
{
	timer_thread_state = thread_state_t::THREAD_STATE_STOPPING;

	while (timer_thread_state != thread_state_t::THREAD_STATE_STOPPED)
	{
		SDL_Delay(1);
	}

	FreeResources();
}

void OPL_Timer_SetCallback(TimeType us, opl_callback_t callback, delay_data_t* data)
{
	SDL_LockMutex(callback_queue_mutex);
	OPL_Queue_Push(callback_queue, callback, data, current_time + us - pause_offset);
	SDL_UnlockMutex(callback_queue_mutex);
}

void OPL_Timer_ClearCallbacks()
{
	SDL_LockMutex(callback_queue_mutex);
	OPL_Queue_Clear(callback_queue);
	SDL_UnlockMutex(callback_queue_mutex);
}

void OPL_Timer_AdjustCallbacks(double factor)
{
	SDL_LockMutex(callback_queue_mutex);
	OPL_Queue_AdjustCallbacks(callback_queue, current_time, factor);
	SDL_UnlockMutex(callback_queue_mutex);
}

void OPL_Timer_Lock()
{
	SDL_LockMutex(timer_mutex);
}

void OPL_Timer_Unlock()
{
	SDL_UnlockMutex(timer_mutex);
}

void OPL_Timer_SetPaused(bool paused)
{
	SDL_LockMutex(callback_queue_mutex);
	opl_timer_paused = paused;
	SDL_UnlockMutex(callback_queue_mutex);
}

} // end namespace cudadoom::opl
