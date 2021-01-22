/**********************************************************************************************************************************************\
	Copyright(C) 2005-2014 Simon Howard

	This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License
	as published by the Free Software Foundation; either version 2 of the License, or (at your option) any later version.

	This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

	DESCRIPTION:
		MIDI file parsing.
\**********************************************************************************************************************************************/
#pragma once

#include "../derma/common.h"

#include "i_swap.h"
#include "i_system.h"
#include "midifile.h"

#define HEADER_CHUNK_ID "MThd"
#define TRACK_CHUNK_ID "MTrk"

namespace cudadoom::midi
{

constexpr size_t MAX_BUFFER_SIZE{0x10000};

constexpr size_t CHANNELS_PER_TRACK{16};

enum class EventType_t
{
	EVENT_NOTE_OFF = 0x80,
	EVENT_NOTE_ON = 0x90,
	EVENT_AFTERTOUCH = 0xa0,
	EVENT_CONTROLLER = 0xb0,
	EVENT_PROGRAM_CHANGE = 0xc0,
	EVENT_CHAN_AFTERTOUCH = 0xd0,
	EVENT_PITCH_BEND = 0xe0,

	EVENT_SYSEX = 0xf0,
	EVENT_SYSEX_SPLIT = 0xf7,
	EVENT_META = 0xff,
};

enum class controller_t
{
	CONTROLLER_BANK_SELECT = 0x0,
	CONTROLLER_MODULATION = 0x1,
	CONTROLLER_BREATH_CONTROL = 0x2,
	CONTROLLER_FOOT_CONTROL = 0x3,
	CONTROLLER_PORTAMENTO = 0x4,
	CONTROLLER_DATA_ENTRY = 0x5,

	CONTROLLER_MAIN_VOLUME = 0x7,
	CONTROLLER_PAN = 0xa,

	CONTROLLER_ALL_NOTES_OFF = 0x7b,
};

enum class meta_EventType_t
{
	META_SEQUENCE_NUMBER = 0x0,

	META_TEXT = 0x1,
	META_COPYRIGHT = 0x2,
	META_TRACK_NAME = 0x3,
	META_INSTR_NAME = 0x4,
	META_LYRICS = 0x5,
	META_MARKER = 0x6,
	META_CUE_POINT = 0x7,

	META_CHANNEL_PREFIX = 0x20,
	META_END_OF_TRACK = 0x2f,

	META_SET_TEMPO = 0x51,
	META_SMPTE_OFFSET = 0x54,
	META_TIME_SIGNATURE = 0x58,
	META_KEY_SIGNATURE = 0x59,
	META_SEQUENCER_SPECIFIC = 0x7f,
};

// haleyjd 09/09/10: packing required
#ifdef _MSC_VER
#pragma pack(push, 1)
#endif

struct chunk_header_t
{
	byte chunk_id[4];
	unsigned chunk_size;
};

struct header_t
{
	chunk_header_t chunk_header;
	unsigned short format_type;
	unsigned short num_tracks;
	unsigned short time_division;
};

// haleyjd 09/09/10: packing off.
#ifdef _MSC_VER
#pragma pack(pop)
#endif

struct meta_event_data_t
{
	// Meta event type:
	meta_EventType_t type;

	unsigned length;

	// Meta event data:
	byte* data;
};

struct sysex_event_data_t
{
	unsigned length;

	// Event data:
	byte* data;
};

struct channel_event_data_t
{
	// The channel number to which this applies:
	unsigned channel;

	// Extra parameters:
	controller_t param1;
	unsigned param2;
};

struct EventType
{
	// Time between the previous event and this event.
	TimeType delta_time;

	// Type of event:
	EventType_t eventType;

	union
	{
		channel_event_data_t channel;
		meta_event_data_t meta;
		sysex_event_data_t sysex;
	} data;
};

struct track_t
{
	// Length in bytes:
	unsigned data_len;

	// Events in this track:
	EventType* events;
	int num_events;
};

struct track_iter_t
{
	track_t* track;
	unsigned position;
};

struct file_t
{
	header_t header;

	// All tracks in this file:
	::std::vector<track_t> tracks;
	unsigned num_tracks;

	// Data buffer used to store data read for SysEx or meta events:
	byte* buffer;
	unsigned buffer_size;
};

file_t* LoadFile(::std::string filename);

void FreeFile(file_t* file);

// Get the time division value from the MIDI header.
unsigned GetFileTimeDivision(file_t* file);

unsigned NumTracks(file_t* file);

// Start iterating over the events in a track.
track_iter_t IterateTrack(file_t& file, size_t pos);

void FreeIterator(track_iter_t* iter);

// Get the time until the next MIDI event in a track.
unsigned GetDeltaTime(track_iter_t* iter);

// Get a pointer to the next MIDI event.
int GetNextEvent(track_iter_t* iter, EventType** event);

// Reset an iterator to the beginning of a track.
void RestartIterator(track_iter_t* iter);

}
