/**********************************************************************************************************************************************\
	Copyright(C) 2017 Alex Mayfield

	This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License
	as published by the Free Software Foundation; either version 2 of the License, or (at your option) any later version.

	This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

	DESCRIPTION:
		A simple buffer and reader implementation.
\**********************************************************************************************************************************************/
#pragma once

#include "../derma/common.h"

#ifdef _WIN32

#include <doomtype.h>

constexpr size_t BUFFER_SIZE{1024};

struct buffer_t
{
	byte buffer[BUFFER_SIZE];	// Buffer.
	byte* buffer_end;			// End of Buffer.
	byte* data;					// Start of actual data.
	byte* data_end;				// End of actual data.
	size_t data_len;			// Length of actual data.
};

struct buffer_reader_t
{
	buffer_t* buffer;
	byte* pos;
};

void Buffer_Clear(buffer_t* buf)
{
	buf->data = buf->buffer;
	buf->data_end = buf->buffer;
	buf->data_len = 0;
}

auto NewBuffer()
{
	auto buf{std::make_unique<buffer_t>()};
	//buffer_t* buf = static_cast<decltype(buf)>(malloc(sizeof(buffer_t)));

	buf->buffer_end = buf->buffer + BUFFER_SIZE;
	Buffer_Clear(buf.get());

	return buf;
}

void DeleteBuffer(buffer_t* buf)
{
}

// return the length of the buffer passed out in the 2nd param, GROSS TODO FIX
auto Buffer_Data(buffer_t* /*IN*/ buf, byte** /*OUT*/ data)
{
	*data = buf->data;
	return buf->data_len;
}

// Push data onto the end of the buffer.
auto Buffer_Push(buffer_t* buf, const void* data, size_t len)
{
	//if (len <= 0)
	//{
	//	// Do nothing, successfully.
	//	return true;
	//}

	ptrdiff_t space_begin{buf->data - buf->buffer};
	ptrdiff_t space_end{buf->buffer_end - buf->data_end};

	if (len > space_end)
	{
		if (len > space_begin + space_end)
		{
			// Don't overflow the buffer.
			return false;
		}

		// Move our data to the front of the buffer.
		memmove(buf->buffer, buf->data, buf->data_len);
		buf->data = buf->buffer;
		buf->data_end = buf->buffer + buf->data_len;
	}

	// Append to the buffer.
	memcpy(buf->data_end, data, len);
	buf->data_len += len;
	buf->data_end = buf->data + buf->data_len;

	return true;
}

// Shift len bytes off of the front of the buffer.
void Buffer_Shift(buffer_t* buf, size_t len)
{
	if (len <= 0)
	{
		// Do nothing.
		return;
	}

	ptrdiff_t max_shift{buf->data_end - buf->data};
	if (len >= max_shift)
	{
		// If the operation would clear the buffer, just zero everything.
		Buffer_Clear(buf);
	}
	else
	{
		buf->data += len;
		buf->data_len -= len;
	}
}

// WARNING: This reader will invalidate if the underlying buffer changes. Use it, then delete it before you touch the underlying buffer again.
auto NewReader(buffer_t* buffer)
{
	//buffer_reader_t *reader = static_cast<decltype(reader)>(malloc(sizeof(buffer_reader_t)));
	auto reader{std::make_unique<buffer_reader_t>()};

	reader->buffer = buffer;
	reader->pos = buffer->data;

	return reader;
}

void DeleteReader(buffer_reader_t* reader)
{
	//free(reader);
}

auto Reader_BytesRead(buffer_reader_t* reader)
{
	return reader->pos - reader->buffer->data;
}

auto Reader_ReadInt8(buffer_reader_t* reader, uint8_t* out)
{
	byte* data;
	size_t len{Buffer_Data(reader->buffer, &data)};

	byte* data_end{data + len};

	if (data_end - reader->pos < 1)
	{
		return false;
	}

	*out = (uint8_t)*reader->pos;
	reader->pos += 1;

	return true;
}

auto Reader_ReadInt16(buffer_reader_t* reader, uint16_t* out)
{
	byte* data;
	size_t len{Buffer_Data(reader->buffer, &data)};

	byte* data_end{data + len};
	byte* dp{reader->pos};

	if (data_end - reader->pos < 2)
	{
		return false;
	}

	*out = (uint16_t)((dp[0] << 8) | dp[1]);
	reader->pos += 2;

	return true;
}

auto Reader_ReadInt32(buffer_reader_t* reader)
{
	byte* data;
	size_t len{Buffer_Data(reader->buffer, &data)};

	byte* data_end{data + len};
	byte* dp{reader->pos};

	if (data_end - reader->pos < 4)
	{
		return 0u;
	}

	reader->pos += 4;

	return (uint32_t)((dp[0] << 24) | (dp[1] << 16) | (dp[2] << 8) | dp[3]);
}

auto Reader_ReadString(buffer_reader_t* reader)
{
	byte* data;
	size_t len{Buffer_Data(reader->buffer, &data)};

	byte* data_start{reader->pos};
	byte* data_end{data + len};
	byte* dp{reader->pos};

	while (dp < data_end && *dp != '\0')
	{
		++dp;
	}

	if (dp >= data_end)
	{
		// Didn't see a null terminator, not a complete string.
		return (char*)nullptr;
	}

	reader->pos = dp + 1;
	return (char*)data_start;
}

#endif
