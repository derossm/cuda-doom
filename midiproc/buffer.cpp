//
// Copyright(C) 2017 Alex Mayfield
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU General Public License for more details.
//
// DESCRIPTION:
//		A simple buffer and reader implementation.
//

#ifdef _WIN32

#include "buffer.h"

#include <stdlib.h>
#include <stddef.h>

#include <memory>

//
// Create a new buffer.
//
auto NewBuffer()
{
	auto buf{std::make_unique<buffer_t>()};
	//buffer_t* buf = malloc(sizeof(buffer_t));

	buf->buffer_end = buf->buffer + BUFFER_SIZE;
	Buffer_Clear(buf->get());

	return buf;
}

//
// Free a buffer.
//
// {NOTE: this is NEVER called ANYWHERE in the solution, so this was a memory leak lol}
void DeleteBuffer(buffer_t* buf)
{
	//free(buf);
}

//
// Return the data in the buffer.
//
auto Buffer_Data(buffer_t* buf, byte** data)
{
	*data = buf->data;
	return buf->data_len;
}

//
// Push data onto the end of the buffer.
//
auto Buffer_Push(buffer_t* buf, const void* data, size_t len)
{
	if (len <= 0)
	{
		// Do nothing, successfully.
		return true;
	}

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


//
// Shift len bytes off of the front of the buffer.
//
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

//
// Clear the buffer.
//
void Buffer_Clear(buffer_t* buf)
{
	buf->data = buf->buffer;
	buf->data_end = buf->buffer;
	buf->data_len = 0;
}

//
// Create a new buffer reader.
//
// WARNING: This reader will invalidate if the underlying buffer changes.
//			Use it, then delete it before you touch the underlying buffer again.
//
auto NewReader(buffer_t* buffer)
{
	//buffer_reader_t *reader = malloc(sizeof(buffer_reader_t));
	auto reader{std::make_unique<buffer_reader_t>()};

	reader->buffer = buffer;
	reader->pos = buffer->data;

	return reader;
}

//
// Delete a buffer reader.
//
void DeleteReader(buffer_reader_t* reader)
{
	//free(reader);
}

//
// Count the number of bytes read thus far.
//
auto Reader_BytesRead(buffer_reader_t* reader)
{
	return reader->pos - reader->buffer->data;
}

//
// Read an unsigned byte from a buffer.
//
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

//
// Read an unsigned short from a buffer.
//
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

//
// Read an unsigned int from a buffer.
//
auto Reader_ReadInt32(buffer_reader_t* reader, uint32_t* out)
{
	byte* data;
	size_t len{Buffer_Data(reader->buffer, &data)};

	byte* data_end{data + len};
	byte* dp{reader->pos};

	if (data_end - reader->pos < 4)
	{
		return false;
	}

	*out = (uint32_t)((dp[0] << 24) | (dp[1] << 16) | (dp[2] << 8) | dp[3]);
	reader->pos += 4;

	return true;
}

//
// Read a string from a buffer.
//
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
		return nullptr;
	}

	reader->pos = dp + 1;
	return (char*)data_start;
}

#endif // #ifdef _WIN32
