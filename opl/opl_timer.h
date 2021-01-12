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
#include <vector>

#include <mutex>
#include <thread>

#include <ranges>
#include <algorithm>

#include "../derma/common.h"

#include "SDL.h"

#include "opl.h"

namespace cudadoom::opl
{

class Timers
{
public:
	std::vector<QueueEntry> callback_queue;
	SDL_Thread* timer_thread = nullptr;
	thread_state_t timer_thread_state;
	TimeType current_time;

	// If non-zero, callbacks are currently paused.
	bool opl_timer_paused;

	// Offset in microseconds to adjust time due to the fact that playback was paused.
	TimeType pause_offset = 0;

	// Queue of callbacks waiting to be invoked. The callback queue mutex is held while the callback queue structure or current_time is being accessed.
	::std::mutex callback_queue_mutex;

	// The timer mutex is held while timer callback functions are being invoked, so that the calling code can prevent clashes.
	::std::mutex timer_mutex;

	// Returns true if there is a callback at the head of the queue ready to be invoked.
	// Otherwise, next_time is set to the time when the timer thread must wake up again to check.
	TimeType CallbackWaiting(TimeType* next_time)
	{
		// If paused, just wait in 50ms increments until unpaused. Update pause_offset so after we unpause, the callback times will be right.
		if (opl_timer_paused)
		{
			*next_time = current_time + 50 * MS;
			pause_offset += 50 * MS;
			return 0;
		}

		// If there are no queued callbacks, sleep for 50ms at a time until a callback is added.
		if (callback_queue.empty())
		{
			*next_time = current_time + 50 * MS;
			return 0;
		}

		// Read the time of the first callback in the queue. If the time for the callback has not yet arrived, we must sleep until the callback time.
		//*next_time = Queue_Peek(callback_queue) + pause_offset;
		*next_time = callback_queue.front().time + pause_offset;

		return *next_time <= current_time;
	}

	TimeType GetNextTime()
	{
		//opl_callback_t callback;
		//delay_data_t* callback_data;
		TimeType next_time;
		TimeType have_callback;

		::std::unique_lock queue_lock(callback_queue_mutex, ::std::defer_lock);
		// Keep running through callbacks until there are none ready to run. When we run out of callbacks, next_time will be set.
		do
		{
			queue_lock.lock();

			// Check if the callback at the head of the list is ready to be invoked. If so, pop from the head of the queue.
			have_callback = CallbackWaiting(&next_time);

			//QueueEntry entry;
			if (have_callback)
			{
				auto& entry = callback_queue.front();

				{
					// Now invoke the callback, if we have one. The timer mutex is held while the callback is invoked.
					::std::scoped_lock timer_lock(timer_mutex);
					queue_lock.unlock();
					entry.callback(entry.data);
					queue_lock.lock();
				}

				callback_queue.erase(callback_queue.begin());
			}
		} while (have_callback);

		return next_time;
	}

	// signature restricted by SDL library
	int ThreadFunction(void* unused)
	{
		// Keep running until OPL_Timer_StopThread is called.
		while (timer_thread_state == thread_state_t::running)
		{
			// Get the next time that we must sleep until, and wait until that time.
			TimeType next_time = GetNextTime();
			TimeType now = SDL_GetTicks() * MS;

			if (next_time > now)
			{
				// NOTE SDL API call here limiting precision
				SDL_Delay(static_cast<uint32_t>((next_time - now) / MS));
			}

			// Update the current time.
			{
				::std::scoped_lock queue_lock(callback_queue_mutex);
				current_time = next_time;
			}
		}

		timer_thread_state = thread_state_t::stopped;

		return 0;
	}

	void InitResources()
	{
		callback_queue.reserve(MAX_OPL_QUEUE);
	}

	void FreeResources()
	{
		callback_queue.clear();
	}

	bool Timer_StartThread()
	{
		InitResources();

		timer_thread_state = thread_state_t::running;
		current_time = SDL_GetTicks();
		opl_timer_paused = 0;
		pause_offset = 0;

		// make our own thread FIXME
		//timer_thread = SDL_CreateThread(ThreadFunction, "OPL timer thread", NULL);

		if (timer_thread == nullptr)
		{
			timer_thread_state = thread_state_t::stopped;
			FreeResources();

			return false;
		}

		return true;
	}

	void Timer_StopThread()
	{
		timer_thread_state = thread_state_t::stopping;

		while (timer_thread_state != thread_state_t::stopped)
		{
			SDL_Delay(1);
		}

		FreeResources();
	}

	void Timer_SetCallback(TimeType us, opl_callback_t callback, delay_data_t* data)
	{
		::std::scoped_lock queue_lock(callback_queue_mutex);
		callback_queue.emplace_back(QueueEntry{.data=data, .callback=callback, .time=(current_time + us - pause_offset)});
	}

	void Timer_ClearCallbacks()
	{
		::std::scoped_lock queue_lock(callback_queue_mutex);
		callback_queue.clear();
	}

	void Timer_AdjustCallbacks(double factor)
	{
		::std::scoped_lock queue_lock(callback_queue_mutex);
		//callback_queue.Queue_AdjustCallbacks(current_time, factor);
		//size_t offset = queue->entries[i].time - time;
		//queue->entries[i].time = time + size_t(offset / factor);
		std::ranges::for_each(callback_queue, [&](auto &iter)
			{
				TimeType offset = iter.time - current_time;
				iter.time = current_time + TimeType(offset/factor);
			});
	}

	void Timer_SetPaused(bool paused)
	{
		::std::scoped_lock queue_lock(callback_queue_mutex);
		opl_timer_paused = paused;
	}
};

} // end namespace cudadoom::opl
