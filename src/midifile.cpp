/**********************************************************************************************************************************************\
	Copyright(C) 2005-2014 Simon Howard

	This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License
	as published by the Free Software Foundation; either version 2 of the License, or (at your option) any later version.

	This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

	DESCRIPTION:
		Reading of MIDI files.
\**********************************************************************************************************************************************/
#include "../derma/stdafx.h"

#include "midifile.h"

namespace cudadoom::midi
{

// Check the header of a chunk:
static bool CheckChunkHeader(chunk_header_t* chunk, ::std::string expected_id)
{
	bool result;

	result = (memcmp((char*)chunk->chunk_id, expected_id, 4) == 0);

	if (!result)
	{
		fprintf(stderr, "CheckChunkHeader: Expected '%s' chunk header, got '%c%c%c%c'\n",
			expected_id, chunk->chunk_id[0], chunk->chunk_id[1], chunk->chunk_id[2], chunk->chunk_id[3]);
	}

	return result;
}

// Read a single byte. Returns false on error.
static bool ReadByte(byte* result, FILE* stream)
{
	if (int c = fgetc(stream); c == EOF)
	{
		fprintf(stderr, "ReadByte: Unexpected end of file\n");
		return false;
	}
	else
	{
		*result = (byte)c;

		return true;
	}
}

// Read a variable-length value.
static bool ReadVariableLength(unsigned* result, FILE* stream)
{
	byte b = 0;

	*result = 0;

	for (size_t i{0}; i < 4; ++i)
	{
		if (!ReadByte(&b, stream))
		{
			fprintf(stderr, "ReadVariableLength: Error while reading variable-length value\n");
			return false;
		}

		// Insert the bottom seven bits from this byte.
		*result <<= 7;
		*result |= b & 0x7f;

		// If the top bit is not set, this is the end.
		if ((b & 0x80) == 0)
		{
			return true;
		}
	}

	fprintf(stderr, "ReadVariableLength: Variable-length value too long: maximum of four bytes\n");
	return false;
}

// Read a byte sequence into the data buffer.
static void* ReadByteSequence(unsigned num_bytes, FILE* stream)
{
	// Allocate a buffer. Allocate one extra byte, as malloc(0) is non-portable.
	byte* result = static_cast<decltype(result)>(malloc(num_bytes + 1));

	if (result == NULL)
	{
		fprintf(stderr, "ReadByteSequence: Failed to allocate buffer\n");
		return nullptr;
	}

	// Read the data:
	for (size_t i{0}; i < num_bytes; ++i)
	{
		if (!ReadByte(&result[i], stream))
		{
			fprintf(stderr, "ReadByteSequence: Error while reading byte %u\n", i);
			free(result);
			return nullptr;
		}
	}

	return result;
}

// Read a MIDI channel event.
// two_param indicates that the event type takes two parameters (three byte) otherwise it is single parameter (two byte)
static bool ReadChannelEvent(EventType* event, byte eventType, bool two_param, FILE* stream)
{
	byte b = 0;

	// Set basics:
	event->eventType = static_cast<EventType_t>(eventType & 0xf0);
	event->data.channel.channel = eventType & 0x0f;

	// Read parameters:
	if (!ReadByte(&b, stream))
	{
		fprintf(stderr, "ReadChannelEvent: Error while reading channel event parameters\n");
		return false;
	}

	event->data.channel.param1 = b;

	// Second parameter:
	if (two_param)
	{
		if (!ReadByte(&b, stream))
		{
			fprintf(stderr, "ReadChannelEvent: Error while reading channel event parameters\n");
			return false;
		}

		event->data.channel.param2 = b;
	}

	return true;
}

// Read sysex event:
static bool ReadSysExEvent(EventType* event, EventType_t eventType, FILE* stream)
{
	event->eventType = eventType;

	if (!ReadVariableLength(&event->data.sysex.length, stream))
	{
		fprintf(stderr, "ReadSysExEvent: Failed to read length of SysEx block\n");
		return false;
	}

	// Read the byte sequence:
	event->data.sysex.data = (byte*)ReadByteSequence(event->data.sysex.length, stream);

	if (event->data.sysex.data == NULL)
	{
		fprintf(stderr, "ReadSysExEvent: Failed while reading SysEx event\n");
		return false;
	}

	return true;
}

// Read meta event:
static bool ReadMetaEvent(EventType* event, FILE* stream)
{
	byte b = 0;

	event->eventType = EventType_t::EVENT_META;

	// Read meta event type:
	if (!ReadByte(&b, stream))
	{
		fprintf(stderr, "ReadMetaEvent: Failed to read meta event type\n");
		return false;
	}

	event->data.meta.type = b;

	// Read length of meta event data:
	if (!ReadVariableLength(&event->data.meta.length, stream))
	{
		fprintf(stderr, "ReadSysExEvent: Failed to read length of SysEx block\n");
		return false;
	}

	// Read the byte sequence:
	event->data.meta.data = static_cast<decltype(event->data.meta.data)>(ReadByteSequence(event->data.meta.length, stream));

	if (event->data.meta.data == NULL)
	{
		fprintf(stderr, "ReadSysExEvent: Failed while reading SysEx event\n");
		return false;
	}

	return true;
}

static bool ReadEvent(EventType* event, unsigned* last_EventType, FILE* stream)
{
	byte eventType = 0;

	if (!ReadVariableLength(&event->delta_time, stream))
	{
		fprintf(stderr, "ReadEvent: Failed to read event timestamp\n");
		return false;
	}

	if (!ReadByte(&eventType, stream))
	{
		fprintf(stderr, "ReadEvent: Failed to read event type\n");
		return false;
	}

	// All event types have their top bit set. Therefore, if
	// the top bit is not set, it is because we are using the "same
	// as previous event type" shortcut to save a byte. Skip back
	// a byte so that we read this byte again.
	if ((eventType & 0x80) == 0)
	{
		eventType = *last_EventType;

		if (fseek(stream, -1, SEEK_CUR) < 0)
		{
			fprintf(stderr, "ReadEvent: Unable to seek in stream\n");
			return false;
		}
	}
	else
	{
		*last_EventType = eventType;
	}

	// Check event type:
	switch (eventType & 0xf0)
	{
		// Two parameter channel events:
	case EventType_t::EVENT_NOTE_OFF:
	case EventType_t::EVENT_NOTE_ON:
	case EventType_t::EVENT_AFTERTOUCH:
	case EventType_t::EVENT_CONTROLLER:
	case EventType_t::EVENT_PITCH_BEND:
		return ReadChannelEvent(event, eventType, true, stream);

		// Single parameter channel events:
	case EventType_t::EVENT_PROGRAM_CHANGE:
	case EventType_t::EVENT_CHAN_AFTERTOUCH:
		return ReadChannelEvent(event, eventType, false, stream);

	default:
		break;
	}

	// Specific value?
	switch (eventType)
	{
	case EventType_t::EVENT_SYSEX:
	case EventType_t::EVENT_SYSEX_SPLIT:
		return ReadSysExEvent(event, static_cast<EventType_t>(eventType), stream);

	case EventType_t::EVENT_META:
		return ReadMetaEvent(event, stream);

	default:
		break;
	}

	fprintf(stderr, "ReadEvent: Unknown MIDI event type: 0x%x\n", eventType);
	return false;
}

// Free an event:
static void FreeEvent(EventType* event)
{
	// Some event types have dynamically allocated buffers assigned to them that must be freed.
	switch (event->eventType)
	{
	case EventType_t::EVENT_SYSEX:
	case EventType_t::EVENT_SYSEX_SPLIT:
		free(event->data.sysex.data);
		break;

	case EventType_t::EVENT_META:
		free(event->data.meta.data);
		break;

	default:
		// Nothing to do.
		break;
	}
}

// Read and check the track chunk header
static bool ReadTrackHeader(track_t* track, FILE* stream)
{
	chunk_header_t chunk_header;

	size_t records_read = fread(&chunk_header, sizeof(chunk_header_t), 1, stream);

	if (records_read < 1)
	{
		return false;
	}

	if (!CheckChunkHeader(&chunk_header, TRACK_CHUNK_ID))
	{
		return false;
	}

	track->data_len = SDL_SwapBE32(chunk_header.chunk_size);

	return true;
}

static bool ReadTrack(track_t* track, FILE* stream)
{
	track->num_events = 0;
	track->events = nullptr;

	// Read the header:
	if (!ReadTrackHeader(track, stream))
	{
		return false;
	}

	// Then the events:
	unsigned last_EventType = 0;

	for (;;)
	{
		// Resize the track slightly larger to hold another event:
		EventType* new_events = (EventType*)I_Realloc(track->events, sizeof(EventType) * (track->num_events + 1));
		track->events = new_events;

		// Read the next event:
		EventType* event = &track->events[track->num_events];
		if (!ReadEvent(event, &last_EventType, stream))
		{
			return false;
		}

		++track->num_events;

		// End of track?
		if (event->eventType == EventType_t::EVENT_META && event->data.meta.type == meta_EventType_t::META_END_OF_TRACK)
		{
			break;
		}
	}

	return true;
}

// Free a track:
static void FreeTrack(track_t* track)
{
	for (size_t i{0}; i < track->num_events; ++i)
	{
		FreeEvent(&track->events[i]);
	}

	free(track->events);
}

static bool ReadAllTracks(file_t* file, FILE* stream)
{
	// Allocate list of tracks and read each track:
	file->tracks = static_cast<decltype(file->tracks)>(malloc(sizeof(track_t) * file->num_tracks));

	if (file->tracks == NULL)
	{
		return false;
	}

	memset(file->tracks, 0, sizeof(track_t) * file->num_tracks);

	// Read each track:
	for (size_t i{0}; i < file->num_tracks; ++i)
	{
		if (!ReadTrack(&file->tracks[i], stream))
		{
			return false;
		}
	}

	return true;
}

// Read and check the header chunk.
static bool ReadFileHeader(file_t* file, FILE* stream)
{
	size_t records_read = fread(&file->header, sizeof(header_t), 1, stream);

	if (records_read < 1)
	{
		return false;
	}

	if (!CheckChunkHeader(&file->header.chunk_header, HEADER_CHUNK_ID) || SDL_SwapBE32(file->header.chunk_header.chunk_size) != 6)
	{
		fprintf(stderr, "ReadFileHeader: Invalid MIDI chunk header! chunk_size=%i\n", SDL_SwapBE32(file->header.chunk_header.chunk_size));
		return false;
	}

	unsigned format_type = SDL_SwapBE16(file->header.format_type);
	file->num_tracks = SDL_SwapBE16(file->header.num_tracks);

	if ((format_type != 0 && format_type != 1) || file->num_tracks < 1)
	{
		fprintf(stderr, "ReadFileHeader: Only type 0/1 MIDI files supported!\n");
		return false;
	}

	return true;
}

void FreeFile(file_t* file)
{
	int i;

	if (file->tracks != nullptr)
	{
		for (i = 0; i < file->num_tracks; ++i)
		{
			FreeTrack(&file->tracks[i]);
		}

		free(file->tracks);
	}

	free(file);
}

file_t* LoadFile(::std::string filename)
{
	file_t* file;
	FILE* stream;

	file = static_cast<decltype(file)>(malloc(sizeof(file_t)));

	if (file == NULL)
	{
		return nullptr;
	}

	file->tracks = nullptr;
	file->num_tracks = 0;
	file->buffer = nullptr;
	file->buffer_size = 0;

	// Open file
	stream = fopen(filename, "rb");

	if (stream == NULL)
	{
		fprintf(stderr, "LoadFile: Failed to open '%s'\n", filename);
		FreeFile(file);
		return nullptr;
	}

	// Read MIDI file header
	if (!ReadFileHeader(file, stream))
	{
		fclose(stream);
		FreeFile(file);
		return nullptr;
	}

	// Read all tracks:
	if (!ReadAllTracks(file, stream))
	{
		fclose(stream);
		FreeFile(file);
		return nullptr;
	}

	fclose(stream);

	return file;
}

// Get the number of tracks in a MIDI file.
unsigned NumTracks(file_t* file)
{
	return file->num_tracks;
}

// Start iterating over the events in a track.
track_iter_t IterateTrack(file_t& file, size_t pos)
{
	//assert(track < file->num_tracks);

	//track_iter_t* iter = static_cast<decltype(iter)>(malloc(sizeof(*iter)));
	track_iter_t iter;
	//iter.track = &file.tracks.emplace_back(track_t());
	iter.track = &file.tracks.at(pos);
	//iter.track = &item;
	//iter->position = 0;

	return iter;
}

void FreeIterator(track_iter_t* iter)
{
	free(iter);
}

// Get the time until the next MIDI event in a track.
unsigned GetDeltaTime(track_iter_t* iter)
{
	if (iter->position < iter->track->num_events)
	{
		EventType* next_event;

		next_event = &iter->track->events[iter->position];

		return next_event->delta_time;
	}
	else
	{
		return 0;
	}
}

// Get a pointer to the next MIDI event.
int GetNextEvent(track_iter_t* iter, EventType** event)
{
	if (iter->position < iter->track->num_events)
	{
		*event = &iter->track->events[iter->position];
		++iter->position;

		return 1;
	}
	else
	{
		return 0;
	}
}

unsigned GetFileTimeDivision(file_t* file)
{
	short result = SDL_SwapBE16(file->header.time_division);

	// Negative time division indicates SMPTE time and must be handled differently.
	if (result < 0)
	{
		return (int)(-(result / 256)) * (int)(result & 0xFF);
	}
	else
	{
		return result;
	}
}

void RestartIterator(track_iter_t* iter)
{
	iter->position = 0;
}

#ifdef TEST

::std::string EventTypeToString(EventType_t eventType)
{
	switch (eventType)
	{
	case EventType_t::EVENT_NOTE_OFF:
		return "EVENT_NOTE_OFF";
	case EventType_t::EVENT_NOTE_ON:
		return "EVENT_NOTE_ON";
	case EventType_t::EVENT_AFTERTOUCH:
		return "EVENT_AFTERTOUCH";
	case EventType_t::EVENT_CONTROLLER:
		return "EVENT_CONTROLLER";
	case EventType_t::EVENT_PROGRAM_CHANGE:
		return "EVENT_PROGRAM_CHANGE";
	case EventType_t::EVENT_CHAN_AFTERTOUCH:
		return "EVENT_CHAN_AFTERTOUCH";
	case EventType_t::EVENT_PITCH_BEND:
		return "EVENT_PITCH_BEND";
	case EventType_t::EVENT_SYSEX:
		return "EVENT_SYSEX";
	case EventType_t::EVENT_SYSEX_SPLIT:
		return "EVENT_SYSEX_SPLIT";
	case EventType_t::EVENT_META:
		return "EVENT_META";

	default:
		return "(unknown)";
	}
}

void PrintTrack(track_t* track)
{
	for (size_t i{0}; i < track->num_events; ++i)
	{
		EventType* event = &track->events[i];

		if (event->delta_time > 0)
		{
			printf("Delay: %u ticks\n", event->delta_time);
		}

		printf("Event type: %s (%i)\n", EventTypeToString(event->eventType), event->eventType);

		switch (event->eventType)
		{
		case EventType_t::EVENT_NOTE_OFF:
		case EventType_t::EVENT_NOTE_ON:
		case EventType_t::EVENT_AFTERTOUCH:
		case EventType_t::EVENT_CONTROLLER:
		case EventType_t::EVENT_PROGRAM_CHANGE:
		case EventType_t::EVENT_CHAN_AFTERTOUCH:
		case EventType_t::EVENT_PITCH_BEND:
			printf("\tChannel: %u\n", event->data.channel.channel);
			printf("\tParameter 1: %u\n", event->data.channel.param1);
			printf("\tParameter 2: %u\n", event->data.channel.param2);
			break;

		case EventType_t::EVENT_SYSEX:
		case EventType_t::EVENT_SYSEX_SPLIT:
			printf("\tLength: %u\n", event->data.sysex.length);
			break;

		case EventType_t::EVENT_META:
			printf("\tMeta type: %u\n", event->data.meta.type);
			printf("\tLength: %u\n", event->data.meta.length);
			break;
		}
	}
}

int main(int argc, char* argv[])
{
	if (argc < 2)
	{
		printf("Usage: %s <filename>\n", argv[0]);
		exit(1);
	}

	file_t* file = LoadFile(argv[1]);

	if (file == NULL)
	{
		fprintf(stderr, "Failed to open %s\n", argv[1]);
		exit(1);
	}

	for (size_t i{0}; i < file->num_tracks; ++i)
	{
		printf("\n== Track %u ==\n\n", i);

		PrintTrack(&file->tracks[i]);
	}

	return 0;
}

#endif

} // end namespace cudadoom::midi
