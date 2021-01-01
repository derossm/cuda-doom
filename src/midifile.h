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

#include "doomtype.h"

#include "i_swap.h"
#include "i_system.h"
#include "midifile.h"

#define HEADER_CHUNK_ID "MThd"
#define TRACK_CHUNK_ID "MTrk"
#define MAX_BUFFER_SIZE 0x10000

#define MIDI_CHANNELS_PER_TRACK 16

enum class midi_EventType_t
{
	MIDI_EVENT_NOTE_OFF			= 0x80,
	MIDI_EVENT_NOTE_ON			= 0x90,
	MIDI_EVENT_AFTERTOUCH		= 0xa0,
	MIDI_EVENT_CONTROLLER		= 0xb0,
	MIDI_EVENT_PROGRAM_CHANGE	= 0xc0,
	MIDI_EVENT_CHAN_AFTERTOUCH	= 0xd0,
	MIDI_EVENT_PITCH_BEND		= 0xe0,

	MIDI_EVENT_SYSEX			= 0xf0,
	MIDI_EVENT_SYSEX_SPLIT		= 0xf7,
	MIDI_EVENT_META				= 0xff,
};

enum class midi_controller_t
{
	MIDI_CONTROLLER_BANK_SELECT		= 0x0,
	MIDI_CONTROLLER_MODULATION		= 0x1,
	MIDI_CONTROLLER_BREATH_CONTROL	= 0x2,
	MIDI_CONTROLLER_FOOT_CONTROL	= 0x3,
	MIDI_CONTROLLER_PORTAMENTO		= 0x4,
	MIDI_CONTROLLER_DATA_ENTRY		= 0x5,

	MIDI_CONTROLLER_MAIN_VOLUME		= 0x7,
	MIDI_CONTROLLER_PAN				= 0xa,

	MIDI_CONTROLLER_ALL_NOTES_OFF	= 0x7b,
};

enum class midi_meta_EventType_t
{
	MIDI_META_SEQUENCE_NUMBER		= 0x0,

	MIDI_META_TEXT					= 0x1,
	MIDI_META_COPYRIGHT				= 0x2,
	MIDI_META_TRACK_NAME			= 0x3,
	MIDI_META_INSTR_NAME			= 0x4,
	MIDI_META_LYRICS				= 0x5,
	MIDI_META_MARKER				= 0x6,
	MIDI_META_CUE_POINT				= 0x7,

	MIDI_META_CHANNEL_PREFIX		= 0x20,
	MIDI_META_END_OF_TRACK			= 0x2f,

	MIDI_META_SET_TEMPO				= 0x51,
	MIDI_META_SMPTE_OFFSET			= 0x54,
	MIDI_META_TIME_SIGNATURE		= 0x58,
	MIDI_META_KEY_SIGNATURE			= 0x59,
	MIDI_META_SEQUENCER_SPECIFIC	= 0x7f,
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

struct midi_header_t
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

struct midi_meta_event_data_t
{
	// Meta event type:
	unsigned type;

	unsigned length;

	// Meta event data:
	byte* data;
};

struct midi_sysex_event_data_t
{
	unsigned length;

	// Event data:
	byte* data;
};

struct midi_channel_event_data_t
{
	// The channel number to which this applies:
	unsigned channel;

	// Extra parameters:
	unsigned param1;
	unsigned param2;
};

struct midi_EventType
{
	// Time between the previous event and this event.
	TimeType delta_time;

	// Type of event:
	midi_EventType_t eventType;

	union
	{
		midi_channel_event_data_t channel;
		midi_meta_event_data_t meta;
		midi_sysex_event_data_t sysex;
	} data;
};

struct midi_track_t
{
	// Length in bytes:
	unsigned data_len;

	// Events in this track:
	midi_EventType *events;
	int num_events;
};

struct midi_track_iter_t
{
	midi_track_t* track;
	unsigned position;
};

struct midi_file_t
{
	midi_header_t header;

	// All tracks in this file:
	midi_track_t* tracks;
	unsigned num_tracks;

	// Data buffer used to store data read for SysEx or meta events:
	byte* buffer;
	unsigned buffer_size;
};

midi_file_t* MIDI_LoadFile(char* filename);

void MIDI_FreeFile(midi_file_t* file);

// Get the time division value from the MIDI header.
unsigned MIDI_GetFileTimeDivision(midi_file_t* file);

unsigned MIDI_NumTracks(midi_file_t* file);

// Start iterating over the events in a track.
midi_track_iter_t* MIDI_IterateTrack(midi_file_t* file, unsigned track_num);

void MIDI_FreeIterator(midi_track_iter_t* iter);

// Get the time until the next MIDI event in a track.
unsigned MIDI_GetDeltaTime(midi_track_iter_t* iter);

// Get a pointer to the next MIDI event.
int MIDI_GetNextEvent(midi_track_iter_t* iter, midi_EventType** event);

// Reset an iterator to the beginning of a track.
void MIDI_RestartIterator(midi_track_iter_t* iter);
