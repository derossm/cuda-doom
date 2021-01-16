/**********************************************************************************************************************************************\
	Copyright(C) 2017 Alex Mayfield

	This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License
	as published by the Free Software Foundation; either version 2 of the License, or (at your option) any later version.

	This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

	DESCRIPTION:
		A simple buffer and reader implementation.
	
	NOT THREAD SAFE
\**********************************************************************************************************************************************/
#pragma once

#include "../derma/stdafx.h"

//#include <memory>
//#include <string_view>
//#include <array>
//#include <ranges>
//#include <algorithm>
//#include <numeric>

#include "../derma/common.h"
#include "../derma/enumbitset.h"

//#include "doomtype.h"

namespace cudadoom::midi
{

enum class PacketType
{
	error = -1,
	REGISTER_SONG,
	SET_VOLUME,
	PLAY_SONG,
	STOP_SONG,
	SHUTDOWN,
	UNREGISTER_SONG,
	ACK
};

constexpr uint32_t DEFAULT_SAMPLE_RATE{44100};

constexpr size_t BUFFER_SIZE{8192};

class Buffer
{
	::std::array<uint8_t, BUFFER_SIZE> buffer; // Buffer.
	size_t data_moved{0};	// total bytes of active buffer parsed, updated WHEN `Buffer_Data()` is called
	size_t data_size{0};	// bytes of most recently parsed data, updated AFTER `Buffer_Data()` processes buffer

	// `data_moved` advances when new data is requested, adding `data_size` before `data_size` updates to new size being processed.
	// In other words, [Start, End) for the current request is found between [data_moved, data_moved + data_size).
	// Note that buffer shift/buffer reset will both reset these values to zero, since nothing active will have been processed yet.

public:
	Buffer() {}

	void Clear()
	{
		data_size = 0;
		data_moved = 0;
	}

	template<typename T>
	auto Data()
	{
		data_moved += data_size; // add the previously parsed data size to determine the new start position
		if constexpr (::std::is_integral_v<T>)
		{
			data_size = sizeof(T);
			// FIXME -- will accumulate skip bad bytes? or throw?
			return ::std::accumulate(&buffer.at(data_moved), &buffer.at(data_moved+data_size), T(0));
		}
		else
		{
			auto start = buffer.cbegin() + data_moved;
			auto end = ::std::find(start, buffer.cend(), '\0');
			data_size = ::std::size_t(::std::distance(start, end));
			
			return T(start, end);
		}
	}

	// Push `length` bytes of new data from `input` onto the end of the `buffer`.
	// NOTE: this ALWAYS returns false in the original version, with `len` > `space_begin+space_end` at init, so is midiproc even being used by DOOM?
	// -- because the internal buffer was only 1024 and the incoming buffer was 8192, unless somehow length is being clamped somewhere
	// NEED MORE RESEARCH TODO FIXME are the files being read by ReadFile smaller than 1024 bytes? is ReadFile limited to 1024 bytes?
	PacketType Push(decltype(buffer) input, size_t length)
	{
		if (length && input.size() && input.size() <= length)
		{
			// check for good input first, so we can continue without doing the rest of the branch checks
		}
		else if (length == 0 && input.size() == 0)
		{
			// Do nothing, successfully. This might be worth calling an error, for wasting time, but is semantically correct.
			return PacketType::ACK;
		}
		else if (buffer.size() == 0)
		{
			// `length` > `0`, but input is empty is a semantic error
			return PacketType::error;
		}
		else
		{
			// `length` > `input.size()` is a semantic error
			return PacketType::error;
		}

		size_t data_end{data_moved + data_size};
		ptrdiff_t space_available{ptrdiff_t(buffer.size() - data_end)};

		if (space_available < 0)
		{
			// corrupted buffer
			return PacketType::error;
		}

		if (length > std::size_t(space_available))
		{
			if (length > std::size_t(data_moved + space_available))
			{
				// this would overflow the buffer
				return PacketType::error;
			}

			// move remaining unprocessed data to the front of the buffer.
			std::move(buffer.begin()+data_end, buffer.end(), buffer.begin());
		}

		// FIXME prove we should be starting at `data_moved`, not `data_moved + 1`, in all other functions
		// Append to the buffer.
		std::move(buffer.begin()+data_moved, buffer.end(), input.begin());

		return PacketType::ACK;
	}

	// Shift length bytes off of the front of the buffer.
	// Not actually shifting bytes, just changing indicies.
	void Shift(size_t length)
	{
		if (length == 0)
		{
			// Do nothing.
			return;
		}
		else if (length == buffer.size())
		{
			Clear();
			//buffer.fill(0); // FIXME is this necessary or can we otherwise invalidate the data and avoid this?
		}

		size_t data_end{data_moved + data_size};
		// error checking on buffer state before shifting
		if (buffer.size() < data_moved || buffer.size() < data_size)
		{
			// something is wrong with the buffer
			return;
		}
		else if (buffer.size() < data_end)
		{
			// error max_shift would be < 0
			return;
		}

		size_t max_shift{buffer.size() - data_end};
		if (length >= max_shift)
		{
			// If the operation would clear the buffer, just zero everything.
			Clear();
		}
		else
		{
			//std::move(buffer.begin() + length, buffer.end(), buffer.begin());
			//data_size = 0;

			//if (data_moved > length)
			//{
			//	data_moved -= length;
			//}
			//else
			//{
			//	data_moved = 0;
			//}
			data_moved = data_end + length;
			data_size = 0;
		}
	}

	size_t BytesRead() const
	{
		return data_moved + data_size;
	}
};

class BufferReader
{
	Buffer* buffer; // non-owning ptr
public:
	// WARNING: This reader will invalidate if the underlying buffer changes.
	// Do not use more than one reader simultaneously on any unique buffer, or anything else that modifies this buffer.
	// We can add locks if we want to thead these, but for now leave lock-free until further development -- TODO UPDATE
	BufferReader(Buffer* buffer) : buffer(buffer)
	{
	}

	void Reset(Buffer* _buffer)
	{
		buffer = _buffer;
	}

	auto BytesRead() const
	{
		return buffer->BytesRead();
	}

	auto ReadInt8() const
	{
		return buffer->Data<int8_t>();
	}

	auto ReadInt16() const
	{
		return buffer->Data<int16_t>();
	}

	auto ReadInt32() const
	{
		return buffer->Data<int32_t>();
	}

	auto ReadString() const
	{
		return buffer->Data<::std::string>();
	}
};

}
