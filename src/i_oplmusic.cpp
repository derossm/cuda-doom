/**********************************************************************************************************************************************\
	Copyright(C) 1993-1996 Id Software, Inc.
	Copyright(C) 2005-2014 Simon Howard

	This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License
	as published by the Free Software Foundation; either version 2 of the License, or (at your option) any later version.

	This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

	DESCRIPTION:
		System interface for music.
\**********************************************************************************************************************************************/
#include "../derma/stdafx.h"

#include "../opl/opl.h"
#include "disk_io/deh_main.h"
#include "disk_io/w_wad.h"

#include "memio.h"
#include "mus2mid.h"

#include "midifile.h"

#include "i_sound.h"
#include "i_swap.h"
#include "m_misc.h"
#include "z_zone.h"

// #define OPL_MIDI_DEBUG

#define GENMIDI_HEADER "#OPL_II#"

#include "i_oplmusic.cc"

namespace cudadoom
{

constexpr size_t MAXMIDLENGTH{96 * 1024};
constexpr size_t GENMIDI_NUM_INSTRS{128};
constexpr size_t GENMIDI_NUM_PERCUSSION{47};

constexpr int GENMIDI_FLAG_FIXED{0x0001}; // fixed pitch
constexpr int GENMIDI_FLAG_2VOICE{0x0004}; // double voice (OPL3)

constexpr size_t PERCUSSION_LOG_LEN{16};

struct genmidi_op_t
{
	byte tremolo;
	byte attack;
	byte sustain;
	byte waveform;
	byte scale;
	byte level;
};

struct genmidi_voice_t
{
	genmidi_op_t modulator;
	byte feedback;
	genmidi_op_t carrier;
	byte unused;
	short base_note_offset;
};

struct genmidi_instr_t
{
	unsigned short flags;
	byte fine_tuning;
	byte fixed_note;

	genmidi_voice_t voices[2];
};

// Data associated with a channel of a track that is currently playing.
struct channel_data_t
{
	// The instrument currently used for this track.
	genmidi_instr_t* instrument;

	// Volume level
	int volume;
	int volume_base;

	// Pan
	int pan;

	// Pitch bend value:
	int bend;
};

// Data associated with a track that is currently playing.
struct track_data_t
{
	// Track iterator used to read new events.
	midi::track_iter_t iter;
};

struct voice_t
{
	// Index of this voice:
	int index;

	// The operators used by this voice:
	int op1;
	int op2;

	// Array used by voice:
	int array;

	// Currently-loaded instrument data
	genmidi_instr_t* current_instr;

	// The voice number in the instrument to use. This is normally set to zero; if this is a double voice instrument, it may be one.
	unsigned current_instr_voice;

	// The channel currently using this voice.
	channel_data_t* channel;

	// The midi key that this voice is playing.
	unsigned key;

	// The note being played. This is normally the same as the key, but if the instrument is a fixed pitch instrument, it is different.
	unsigned note;

	// The frequency value being used.
	unsigned freq;

	// The volume of the note being played on this channel.
	unsigned note_volume;

	// The current volume (register value) that has been set for this channel.
	unsigned car_volume;
	unsigned mod_volume;

	// Pan.
	unsigned reg_pan;

	// Priority.
	unsigned priority;
};

static driver_ver_t drv_ver{driver_ver_t::doom_1_9};
static bool music_initialized{false};

//static bool musicpaused{false};
static int start_music_volume;
static int current_music_volume;

// GENMIDI lump instrument data:
static ::std::vector<genmidi_instr_t> main_instrs;
static ::std::vector<genmidi_instr_t> percussion_instrs;

// ptr to array of char
static char (*main_instr_names)[32];
static char (*percussion_names)[32];

// Voices:
static ::std::array<voice_t, opl::NUM_VOICES * 2> voices;
static ::std::array<voice_t*, opl::NUM_VOICES * 2> voice_free_list;
static ::std::array<voice_t*, opl::NUM_VOICES * 2> voice_alloced_list;
static int voice_free_num;
static int voice_alloced_num;
static int opl3mode;
static int num_opl_voices;

// Data for each channel.
static ::std::array<channel_data_t, midi::CHANNELS_PER_TRACK> channels;

// Track data for playing tracks:
static ::std::vector<track_data_t> tracks;
//static unsigned num_tracks{0};
static unsigned running_tracks{0};
static bool song_looping;

// Tempo control variables
static unsigned ticks_per_beat;
static unsigned us_per_beat;

// Mini-log of recently played percussion instruments:
static ::std::array<uint8_t, PERCUSSION_LOG_LEN> last_perc;
static unsigned last_perc_count;

// Configuration file variable, containing the port number for the adlib chip.
::std::string snd_dmxoption{"-opl3"}; // default to OPL3 emulation
int io_port{0x388};

// If true, OPL sound channels are reversed to their correct arrangement
// (as intended by the MIDI standard) rather than the backwards one used by DMX due to a bug.
static bool stereo_correct{false};

// Load instrument table from GENMIDI lump:
static bool LoadInstrumentTable()
{
	byte* lump{W_CacheLumpName<byte>(DEH_String("genmidi"), pu_tags_t::PU_STATIC)};

	// DMX does not check header
	main_instrs = (genmidi_instr_t*)(lump + strlen(GENMIDI_HEADER));
	percussion_instrs = main_instrs + GENMIDI_NUM_INSTRS;
	// ptr to array of chars with 32 elements
	main_instr_names = (char(*)[32])(percussion_instrs + GENMIDI_NUM_PERCUSSION);
	percussion_names = main_instr_names + GENMIDI_NUM_INSTRS;

	return true;
}

// Get the next available voice from the freelist.
static voice_t* GetFreeVoice()
{
	// None available?
	if (voice_free_num == 0)
	{
		return nullptr;
	}

	// Remove from free list
	auto result{voice_free_list[0]};

	--voice_free_num;

	for (size_t i{0}; i < voice_free_num; ++i)
	{
		voice_free_list[i] = voice_free_list[i + 1];
	}

	// Add to allocated list
	voice_alloced_list[voice_alloced_num++] = result;

	return result;
}

// Release a voice back to the freelist.
static void VoiceKeyOff(voice_t* voice);

static void ReleaseVoice(int index)
{
	// Doom 2 1.666 OPL crash emulation.
	if (index >= voice_alloced_num)
	{
		voice_alloced_num = 0;
		voice_free_num = 0;
		return;
	}

	auto voice{voice_alloced_list[index]};

	VoiceKeyOff(voice);

	voice->channel = nullptr;
	voice->note = 0;

	bool double_voice{voice->current_instr_voice != 0};

	// Remove from alloced list.
	--voice_alloced_num;

	for (size_t i{index}; i < voice_alloced_num; ++i)
	{
		voice_alloced_list[i] = voice_alloced_list[i + 1];
	}

	// Search to the end of the freelist (This is how Doom behaves!)
	voice_free_list[voice_free_num++] = voice;

	if (double_voice && drv_ver < driver_ver_t::doom_1_9)
	{
		ReleaseVoice(index);
	}
}

// Load data to the specified operator
static void LoadOperatorData(int op, genmidi_op_t* data, bool max_level, unsigned* volume)
{
	// The scale and level fields must be combined for the level register. For the carrier wave we always set the maximum level.
	auto level{data->scale};

	if (max_level)
	{
		level |= 0x3f;
	}
	else
	{
		level |= data->level;
	}

	*volume = level;

	opl::WriteRegister(opl::REGS_LEVEL + op, level);
	opl::WriteRegister(opl::REGS_TREMOLO + op, data->tremolo);
	opl::WriteRegister(opl::REGS_ATTACK + op, data->attack);
	opl::WriteRegister(opl::REGS_SUSTAIN + op, data->sustain);
	opl::WriteRegister(opl::REGS_WAVEFORM + op, data->waveform);
}

// Set the instrument for a particular voice.
static void SetVoiceInstrument(voice_t* voice, genmidi_instr_t* instr, unsigned instr_voice)
{
	// Instrument already set for this channel?
	if (voice->current_instr == instr && voice->current_instr_voice == instr_voice)
	{
		return;
	}

	voice->current_instr = instr;
	voice->current_instr_voice = instr_voice;

	auto data{&instr->voices[instr_voice]};

	// Are we usind modulated feedback mode?
	bool modulating{(data->feedback & 0x01) == 0};

	// Doom loads the second operator first, then the first. The carrier is set to minimum volume until the voice volume is set in
	// SetVoiceVolume (below). If we are not using modulating mode, we must set both to minimum volume.
	LoadOperatorData(voice->op2 | voice->array, &data->carrier, true, &voice->car_volume);
	LoadOperatorData(voice->op1 | voice->array, &data->modulator, !modulating, &voice->mod_volume);

	// Set feedback register that control the connection between the two operators. Turn on bits in the upper nybble;
	// I think this is for OPL3, where it turns on channel A/B.
	opl::WriteRegister((opl::REGS_FEEDBACK + voice->index) | voice->array, data->feedback | voice->reg_pan);

	// Calculate voice priority.
	voice->priority = 0x0f - (data->carrier.attack >> 4) + 0x0f - (data->carrier.sustain & 0x0f);
}

// FIXME this is fucked up, a ptr to a voice_t is pointed to a genmidi_voice_t object,
// and then members from both non-overlapping structures are accessed... how could this possibly have worked?
// in C, would this mean the first 8 bytes are treated as the "modulator" in a genmidi, and the 9th byte treated as feedback?
// or is it aligned 6 byte with 7th byte squeezed in? this would be midway into op1, or else op2 is "feedback"
// WTF were they thinking? This is such a gross fucking hack
// voice->car_volume is going to be way off out of bounds when pointed at genmidi object
// same with note_volume and channel
// voice->array will be the final byte of the 2nd genmidi_op_t + 1 unused byte + 2bytes of short at the end and subject to invalidation if the
// structure if ever altered... so they have to be manually aligning the bytes of multiple data structures to overlap or appear in specific
// order somewhere, which is insane to do, or they are just grabbing whatever garbage is in memory they didn't mean to access
// I have to redo the entire sound system, because it's all garbage someone has to have made intentionally cryptic for misguided job security
// so I may as well get rid of OPL and find something modern
static void SetVoiceVolume(voice_t* voice, unsigned volume)
{
	voice->note_volume = volume;

	voice = &voice->current_instr->voices[voice->current_instr_voice];

	// Multiply note volume and channel volume to get the actual volume.
	unsigned midi_volume{2 * (volume_mapping_table[voice->channel->volume] + 1)};

	unsigned full_volume{(volume_mapping_table[voice->note_volume] * midi_volume) >> 9};

	// The volume value to use in the register:
	unsigned car_volume{0x3f - full_volume};

	// Update the volume register(s) if necessary.
	if (car_volume != (voice->car_volume & 0x3f))
	{
		voice->car_volume = car_volume | (voice->car_volume & 0xc0);

		opl::WriteRegister((opl::REGS_LEVEL + voice->op2) | voice->array, voice->car_volume);

		// If we are using non-modulated feedback mode, we must set the volume for both voices.
		if ((voice->feedback & 0x01) != 0 && voice->modulator.level != 0x3f)
		{
			unsigned mod_volume{voice->modulator.level};
			if (mod_volume < car_volume)
			{
				mod_volume = car_volume;
			}

			mod_volume |= voice->mod_volume & 0xc0;

			if(mod_volume != voice->mod_volume)
			{
				voice->mod_volume = mod_volume;
				opl::WriteRegister((opl::REGS_LEVEL + voice->op1) | voice->array, mod_volume | (voice->modulator.scale & 0xc0));
			}
		}
	}
}

static void SetVoicePan(voice_t* voice, unsigned pan)
{
	voice->reg_pan = pan;
	auto voice{&voice->current_instr->voices[voice->current_instr_voice]};

	opl::WriteRegister((opl::REGS_FEEDBACK + voice->index) | voice->array, voice->feedback | pan);
}

// Initialize the voice table and freelist
static void InitVoices()
{
	// Start with an empty free list.
	voice_free_num = num_opl_voices;
	voice_alloced_num = 0;

	// Initialize each voice.
	for (size_t i{0}; i < num_opl_voices; ++i)
	{
		voices[i].index = i % opl::NUM_VOICES;
		voices[i].op1 = voice_operators[0][i % opl::NUM_VOICES];
		voices[i].op2 = voice_operators[1][i % opl::NUM_VOICES];
		voices[i].array = (i / opl::NUM_VOICES) << 8;
		voices[i].current_instr = NULL;

		// Add this voice to the freelist.
		voice_free_list[i] = &voices[i];
	}
}

static void SetChannelVolume(channel_data_t* channel, unsigned volume, bool clip_start);

// Set music volume (0 - 127)
static void I_OPL_SetMusicVolume(int volume)
{
	if (current_music_volume == volume)
	{
		return;
	}

	// Internal state variable.
	current_music_volume = volume;

	// Update the volume of all voices.
	for (size_t i{0}; i < midi::CHANNELS_PER_TRACK; ++i)
	{
		if (i == 15)
		{
			SetChannelVolume(&channels[i], volume, false);
		}
		else
		{
			SetChannelVolume(&channels[i], channels[i].volume_base, false);
		}
	}
}

static void VoiceKeyOff(voice_t* voice)
{
	opl::WriteRegister((opl::REGS_FREQ_2 + voice->index) | voice->array, voice->freq >> 8);
}

static channel_data_t* TrackChannelForEvent(track_data_t* track, midi::EventType* event)
{
	unsigned channel_num{event->data.channel.channel};

	// MIDI uses track #9 for percussion, but for MUS it's track #15 instead. Because DMX works on MUS data internally,
	// we need to swap back to the MUS version of the channel number.
	if (channel_num == 9)
	{
		channel_num = 15;
	}
	else if (channel_num == 15)
	{
		channel_num = 9;
	}

	return &channels[channel_num];
}

// Get the frequency that we should be using for a voice.
static void KeyOffEvent(track_data_t* track, midi::EventType* event)
{
	//printf("note off: channel %i, %i, %i\n", event->data.channel.channel, event->data.channel.param1, event->data.channel.param2);

	auto channel{TrackChannelForEvent(track, event)};
	auto key{event->data.channel.param1};

	// Turn off voices being used to play this key. If it is a double voice instrument there will be two.
	for (size_t i{0}; i < voice_alloced_num; ++i)
	{
		if (voice_alloced_list[i]->channel == channel && voice_alloced_list[i]->key == key)
		{
			// Finished with this voice now.
			ReleaseVoice(i);

			--i;
		}
	}
}

// When all voices are in use, we must discard an existing voice to play a new note. Find and free an existing voice.
// The channel passed to the function is the channel for the new note to be played.
static void ReplaceExistingVoice()
{
	// Check the allocated voices, if we find an instrument that is of a lower priority to the new instrument, discard it.
	// If a voice is being used to play the second voice of an instrument, use that, as second voices are non-essential.
	// Lower numbered MIDI channels implicitly have a higher priority than higher-numbered channels, eg. MIDI channel 1 is never
	// discarded for MIDI channel 2.
	auto result{0};

	for (size_t i{0}; i < voice_alloced_num; ++i)
	{
		if (voice_alloced_list[i]->current_instr_voice != 0 || voice_alloced_list[i]->channel >= voice_alloced_list[result]->channel)
		{
			result = i;
		}
	}

	ReleaseVoice(result);
}

// Alternate versions of ReplaceExistingVoice() used when emulating old versions of the DMX library used in Doom 1.666, Heretic and Hexen.
static void ReplaceExistingVoiceDoom1()
{
	auto result{0};

	for (size_t i{0}; i < voice_alloced_num; ++i)
	{
		if (voice_alloced_list[i]->channel > voice_alloced_list[result]->channel)
		{
			result = i;
		}
	}

	ReleaseVoice(result);
}

static void ReplaceExistingVoiceDoom2(channel_data_t* channel)
{
	auto result{0};

	for (size_t i{0}, priority{0x8000}; i < voice_alloced_num - 3; ++i)
	{
		if (voice_alloced_list[i]->priority < priority && voice_alloced_list[i]->channel >= channel)
		{
			priority = voice_alloced_list[i]->priority;
			result = i;
		}
	}

	ReleaseVoice(result);
}

static unsigned FrequencyForVoice(voice_t* voice)
{
	// Apply note offset. Don't apply offset if the instrument is a fixed note instrument.
	auto gm_voice{&voice->current_instr->voices[voice->current_instr_voice]};

	int note{voice->note};
	if ((SHORT(voice->current_instr->flags) & GENMIDI_FLAG_FIXED) == 0)
	{
		note += (short) SHORT(gm_voice->base_note_offset);
	}

	// Avoid possible overflow due to base note offset:
	while (note < 0)
	{
		note += 12;
	}

	while (note > 95)
	{
		note -= 12;
	}

	int freq_index{64 + 32 * note + voice->channel->bend};

	// If this is the second voice of a double voice instrument, the frequency index can be adjusted by the fine tuning field.
	if (voice->current_instr_voice != 0)
	{
		freq_index += (voice->current_instr->fine_tuning / 2) - 64;
	}

	if (freq_index < 0)
	{
		freq_index = 0;
	}

	// The first 7 notes use the start of the table, while consecutive notes loop around the latter part.
	if (freq_index < 284)
	{
		return frequency_curve[freq_index];
	}

	unsigned sub_index{(freq_index - 284) % (12 * 32)};
	unsigned octave{(freq_index - 284) / (12 * 32)};

	// Once the seventh octave is reached, things break down. We can only go up to octave 7 as a maximum anyway (the OPL
	// register only has three bits for octave number), but for the notes in octave 7, the first five bits have octave=7, the
	// following notes have octave=6. This 7/6 pattern repeats in following octaves (which are technically impossible to represent anyway).
	if (octave >= 7)
	{
		octave = 7;
	}

	// Calculate the resulting register value to use for the frequency.
	return frequency_curve[sub_index + 284] | (octave << 10);
}

// Update the frequency that a voice is programmed to use.
static void UpdateVoiceFrequency(voice_t* voice)
{
	// Calculate the frequency to use for this voice and update it if neccessary.
	auto freq{FrequencyForVoice(voice)};

	if (voice->freq != freq)
	{
		opl::WriteRegister((opl::REGS_FREQ_1 + voice->index) | voice->array, freq & 0xff);
		opl::WriteRegister((opl::REGS_FREQ_2 + voice->index) | voice->array, (freq >> 8) | 0x20);

		voice->freq = freq;
	}
}

// Program a single voice for an instrument. For a double voice instrument (GENMIDI_FLAG_2VOICE), this is called twice for each key on event.
static void VoiceKeyOn(channel_data_t* channel, genmidi_instr_t* instrument, unsigned instrument_voice, unsigned note, unsigned key, unsigned volume)
{
	if (!opl3mode && drv_ver == driver_ver_t::doom1_1_666)
	{
		instrument_voice = 0;
	}

	// Find a voice to use for this new note.
	auto voice{GetFreeVoice()};

	if (!voice)
	{
		return;
	}

	voice->channel = channel;
	voice->key = key;

	// Work out the note to use. This is normally the same as the key, unless it is a fixed pitch instrument.
	if ((SHORT(instrument->flags) & GENMIDI_FLAG_FIXED) != 0)
	{
		voice->note = instrument->fixed_note;
	}
	else
	{
		voice->note = note;
	}

	voice->reg_pan = channel->pan;

	// Program the voice with the instrument data:
	SetVoiceInstrument(voice, instrument, instrument_voice);

	// Set the volume level.
	SetVoiceVolume(voice, volume);

	// Write the frequency value to turn the note on.
	voice->freq = 0;
	UpdateVoiceFrequency(voice);
}

static void KeyOnEvent(track_data_t* track, midi::EventType* event)
{
	genmidi_instr_t* instrument;

	//printf("note on: channel %i, %i, %i\n", event->data.channel.channel, event->data.channel.param1, event->data.channel.param2);

	auto note{event->data.channel.param1};
	auto key{event->data.channel.param1};
	auto volume{event->data.channel.param2};

	// A volume of zero means key off. Some MIDI tracks, eg. the ones in AV.wad, use a second key on with a volume of zero to mean key off.
	if (volume <= 0)
	{
		KeyOffEvent(track, event);
		return;
	}

	auto channel{TrackChannelForEvent(track, event)};

	// Percussion channel is treated differently.
	if (event->data.channel.channel == 9)
	{
		if (key < 35 || key > 81)
		{
			return;
		}

		instrument = &percussion_instrs[key - 35];

		last_perc[last_perc_count] = key;
		last_perc_count = (last_perc_count + 1) % PERCUSSION_LOG_LEN;
		note = 60;
	}
	else
	{
		instrument = channel->instrument;
	}

	auto double_voice{(SHORT(instrument->flags) & GENMIDI_FLAG_2VOICE) != 0};

	switch (drv_ver)
	{
		case driver_ver_t::doom1_1_666:
			auto voicenum{double_voice + 1};
			if (!opl3mode)
			{
				voicenum = 1;
			}
			while (voice_alloced_num > num_opl_voices - voicenum)
			{
				ReplaceExistingVoiceDoom1();
			}

			// Find and program a voice for this instrument. If this is a double voice instrument, we must do this twice.
			if (double_voice)
			{
				VoiceKeyOn(channel, instrument, 1, note, key, volume);
			}

			VoiceKeyOn(channel, instrument, 0, note, key, volume);
			break;
		case driver_ver_t::doom2_1_666:
			if (voice_alloced_num == num_opl_voices)
			{
				ReplaceExistingVoiceDoom2(channel);
			}
			if (voice_alloced_num == num_opl_voices - 1 && double_voice)
			{
				ReplaceExistingVoiceDoom2(channel);
			}

			// Find and program a voice for this instrument. If this is a double voice instrument, we must do this twice.
			if (double_voice)
			{
				VoiceKeyOn(channel, instrument, 1, note, key, volume);
			}

			VoiceKeyOn(channel, instrument, 0, note, key, volume);
			break;
		default:
		case driver_ver_t::doom_1_9:
			if (voice_free_num == 0)
			{
				ReplaceExistingVoice();
			}

			// Find and program a voice for this instrument. If this is a double voice instrument, we must do this twice.
			VoiceKeyOn(channel, instrument, 0, note, key, volume);

			if (double_voice)
			{
				VoiceKeyOn(channel, instrument, 1, note, key, volume);
			}
			break;
	}
}

static void ProgramChangeEvent(track_data_t* track, midi::EventType* event)
{
	// Set the instrument used on this channel.
	auto channel{TrackChannelForEvent(track, event)};
	auto instrument{event->data.channel.param1};
	channel->instrument = &main_instrs[instrument];

	// TODO: Look through existing voices that are turned on on this channel, and change the instrument.
}

static void SetChannelVolume(channel_data_t* channel, unsigned volume, bool clip_start)
{
	channel->volume_base = volume;

	if (volume > current_music_volume)
	{
		volume = current_music_volume;
	}

	if (clip_start && volume > start_music_volume)
	{
		volume = start_music_volume;
	}

	channel->volume = volume;

	// Update all voices that this channel is using.
	for (size_t i{0}; i < num_opl_voices; ++i)
	{
		if (voices[i].channel == channel)
		{
			SetVoiceVolume(&voices[i], voices[i].note_volume);
		}
	}
}

static void SetChannelPan(channel_data_t* channel, unsigned pan)
{
	// The DMX library has the stereo channels backwards, maybe because Paul Radek had a Soundblaster card with the channels reversed, or
	// perhaps it was just a bug in the OPL3 support that was never finished. By default we preserve this bug, but we also provide a
	// secret DMXOPTION to fix it.
	if (stereo_correct)
	{
		pan = 144 - pan;
	}

	if (opl3mode)
	{
		unsigned reg_pan;
		if (pan >= 96)
		{
			reg_pan = 0x10;
		}
		else if (pan <= 48)
		{
			reg_pan = 0x20;
		}
		else
		{
			reg_pan = 0x30;
		}
		if (channel->pan != reg_pan)
		{
			channel->pan = reg_pan;
			for (size_t i{0}; i < num_opl_voices; ++i)
			{
				if (voices[i].channel == channel)
				{
					SetVoicePan(&voices[i], reg_pan);
				}
			}
		}
	}
}

// Handler for the MIDI_CONTROLLER_ALL_NOTES_OFF channel event.
static void AllNotesOff(channel_data_t* channel, unsigned param)
{
	for (size_t i{0}; i < voice_alloced_num; ++i)
	{
		if (voice_alloced_list[i]->channel == channel)
		{
			// Finished with this voice now.
			ReleaseVoice(i);

			--i;
		}
	}
}

static void ControllerEvent(track_data_t* track, midi::EventType* event)
{
	//printf("change controller: channel %i, %i, %i\n", event->data.channel.channel, event->data.channel.param1, event->data.channel.param2);

	auto channel{TrackChannelForEvent(track, event)};
	auto controller{event->data.channel.param1};
	auto param{event->data.channel.param2};

	switch (controller)
	{
		case midi::controller_t::CONTROLLER_MAIN_VOLUME:
			SetChannelVolume(channel, param, true);
			break;

		case midi::controller_t::CONTROLLER_PAN:
			SetChannelPan(channel, param);
			break;

		case midi::controller_t::CONTROLLER_ALL_NOTES_OFF:
			AllNotesOff(channel, param);
			break;

		default:
#ifdef OPL_MIDI_DEBUG
			fprintf(stderr, "Unknown MIDI controller type: %u\n", controller);
#endif
			break;
	}
}

// Process a pitch bend event.
static void PitchBendEvent(track_data_t* track, midi::EventType* event)
{
	voice_t* voice_updated_list[opl::NUM_VOICES * 2];
	unsigned voice_updated_num = 0;
	voice_t* voice_not_updated_list[opl::NUM_VOICES * 2];
	unsigned voice_not_updated_num = 0;

	// Update the channel bend value. Only the MSB of the pitch bend value is considered: this is what Doom does.
	auto channel{TrackChannelForEvent(track, event)};
	channel->bend = event->data.channel.param2 - 64;

	// Update all voices for this channel.
	for (size_t i{0}; i < voice_alloced_num; ++i)
	{
		if (voice_alloced_list[i]->channel == channel)
		{
			UpdateVoiceFrequency(voice_alloced_list[i]);
			voice_updated_list[voice_updated_num++] = voice_alloced_list[i];
		}
		else
		{
			voice_not_updated_list[voice_not_updated_num++] = voice_alloced_list[i];
		}
	}

	for (size_t i{0}; i < voice_not_updated_num; ++i)
	{
		voice_alloced_list[i] = voice_not_updated_list[i];
	}

	for (size_t i{0}; i < voice_updated_num; ++i)
	{
		voice_alloced_list[i + voice_not_updated_num] = voice_updated_list[i];
	}
}

static void MetaSetTempo(unsigned tempo)
{
	opl::AdjustCallbacks((float) us_per_beat / tempo);
	us_per_beat = tempo;
}

// Process a meta event.
static void MetaEvent(track_data_t* track, midi::EventType* event)
{
	auto data{event->data.meta.data};
	auto data_len{event->data.meta.length};

	switch (event->data.meta.type)
	{
		// Things we can just ignore.
		case midi::meta_EventType_t::META_SEQUENCE_NUMBER:
		case midi::meta_EventType_t::META_TEXT:
		case midi::meta_EventType_t::META_COPYRIGHT:
		case midi::meta_EventType_t::META_TRACK_NAME:
		case midi::meta_EventType_t::META_INSTR_NAME:
		case midi::meta_EventType_t::META_LYRICS:
		case midi::meta_EventType_t::META_MARKER:
		case midi::meta_EventType_t::META_CUE_POINT:
		case midi::meta_EventType_t::META_SEQUENCER_SPECIFIC:
			break;

		case midi::meta_EventType_t::META_SET_TEMPO:
			if (data_len == 3)
			{
				MetaSetTempo((data[0] << 16) | (data[1] << 8) | data[2]);
			}
			break;

		// End of track - actually handled when we run out of events in the track, see below.
		case midi::meta_EventType_t::META_END_OF_TRACK:
			break;

		default:
#ifdef OPL_MIDI_DEBUG
			fprintf(stderr, "Unknown MIDI meta event type: %u\n", event->data.meta.type);
#endif
			break;
	}
}

// Process a MIDI event from a track.
static void ProcessEvent(track_data_t* track, midi::EventType* event)
{
	switch (event->eventType)
	{
		case midi::EventType_t::EVENT_NOTE_OFF:
			KeyOffEvent(track, event);
			break;

		case midi::EventType_t::EVENT_NOTE_ON:
			KeyOnEvent(track, event);
			break;

		case midi::EventType_t::EVENT_CONTROLLER:
			ControllerEvent(track, event);
			break;

		case midi::EventType_t::EVENT_PROGRAM_CHANGE:
			ProgramChangeEvent(track, event);
			break;

		case midi::EventType_t::EVENT_PITCH_BEND:
			PitchBendEvent(track, event);
			break;

		case midi::EventType_t::EVENT_META:
			MetaEvent(track, event);
			break;

		// SysEx events can be ignored.

		case midi::EventType_t::EVENT_SYSEX:
		case midi::EventType_t::EVENT_SYSEX_SPLIT:
			break;

		default:
#ifdef OPL_MIDI_DEBUG
			fprintf(stderr, "Unknown MIDI event type %i\n", event->EventType);
#endif
			break;
	}
}

static void ScheduleTrack(track_data_t* track);
static void InitChannel(channel_data_t* channel);

// FIXME remove unused param
// Restart a song from the beginning.
static void RestartSong(void* unused)
{
	//running_tracks = num_tracks;
	running_tracks = tracks.size();
	start_music_volume = current_music_volume;

	for (auto& iter : tracks)
	//for (size_t i{0}; i < num_tracks; ++i)
	{
		midi::RestartIterator(&iter.iter);
		ScheduleTrack(&iter); // FIXME make sure this isn't a temp address
	}

	for (auto& iter : channels)
	//for (size_t i{0}; i < midi::CHANNELS_PER_TRACK; ++i)
	{
		InitChannel(&iter);
	}
}

// Callback function invoked when another event needs to be read from a track.
static void TrackTimerCallback(track_data_t* arg)
{
	track_data_t* track{arg};
	midi::EventType* event;

	// Get the next event and process it.
	if (!midi::GetNextEvent(&track->iter, &event))
	{
		return;
	}

	ProcessEvent(track, event);

	// End of track?
	if (event->eventType == midi::EventType_t::EVENT_META && event->data.meta.type == midi::meta_EventType_t::META_END_OF_TRACK)
	{
		--running_tracks;

		// When all tracks have finished, restart the song.
		// Don't restart the song immediately, but wait for 5ms before triggering a restart.
		// Otherwise it is possible to construct an empty MIDI file that causes the game to lock up in an infinite loop.
		// (5ms should be short enough not to be noticeable by the listener).
		if (running_tracks <= 0 && song_looping)
		{
			opl::SetCallback(5000, RestartSong, nullptr);
		}

		return;
	}

	// Reschedule the callback for the next event in the track.
	ScheduleTrack(track);
}

static void ScheduleTrack(track_data_t* track)
{
	// Get the number of microseconds until the next event.
	auto nticks{midi::GetDeltaTime(&track->iter)};
	uint64_t us{((uint64_t) nticks * us_per_beat) / ticks_per_beat};

	// Set a timer to be invoked when the next event is ready to play.
	opl::SetCallback(us, TrackTimerCallback, track);
}

// Initialize a channel.
static void InitChannel(channel_data_t* channel)
{
	// TODO: Work out sensible defaults?
	channel->instrument = &main_instrs[0];
	channel->volume = current_music_volume;
	channel->volume_base = 100;
	if (channel->volume > channel->volume_base)
	{
		channel->volume = channel->volume_base;
	}
	channel->pan = 0x30;
	channel->bend = 0;
}

// Start a MIDI track playing:
static void StartTrack(midi::file_t& file, unsigned track_num)
{
	auto track{&tracks.at(track_num)};
	track->iter = midi::IterateTrack(file, track_num);

	// Schedule the first event.
	ScheduleTrack(track);
}

// Start playing a mid
static void I_OPL_PlaySong(midi::file_t& file, bool looping)
{
	if (!music_initialized)
	{
		return;
	}

	//auto file{handle};

	// Allocate track data.
	//tracks = static_cast<decltype(tracks)>(malloc(midi::NumTracks(file) * sizeof(track_data_t)));
	tracks.reserve(file.tracks.size());

	//num_tracks = midi::NumTracks(file);
	running_tracks = file.tracks.size();
	song_looping = looping;

	ticks_per_beat = midi::GetFileTimeDivision(&file);

	// Default is 120 bpm.
	// TODO: this is wrong
	us_per_beat = 500 * 1000;

	start_music_volume = current_music_volume;

	for (size_t i{0}; i < running_tracks; ++i)
	{
		StartTrack(file, i);
	}

	//for (size_t i{0}; i < midi::CHANNELS_PER_TRACK; ++i)
	for (auto& iter : channels)
	{
		InitChannel(&iter);
	}

	// If the music was previously paused, it needs to be unpaused; playing a new song implies that we turn off pause. This matches vanilla
	// behavior of the DMX library, and some of the higher-level code in s_sound.cpp relies on this.
	opl::SetPaused(false);
}

static void I_OPL_PauseSong()
{
	if (!music_initialized)
	{
		return;
	}

	// Pause OPL callbacks.
	opl::SetPaused(true);

	// Turn off all main instrument voices (not percussion). This is what Vanilla does.
	for (size_t i{0}; i < num_opl_voices; ++i)
	{
		if (voices[i].channel && voices[i].current_instr < percussion_instrs)
		{
			VoiceKeyOff(&voices[i]);
		}
	}
}

static void I_OPL_ResumeSong()
{
	if (!music_initialized)
	{
		return;
	}

	opl::SetPaused(false);
}

static void I_OPL_StopSong()
{
	if (!music_initialized)
	{
		return;
	}

	//OPL_Lock();

	// Stop all playback.
	//opl::ClearCallbacks();

	// Free all voices.
	/*for (size_t i{0}; i < midi::CHANNELS_PER_TRACK; ++i)
	{
		AllNotesOff(&channels[i], 0);
	}*/

	// Free all track data.
	/*for (size_t i{0}; i < num_tracks; ++i)
	{
		midi::FreeIterator(tracks[i].iter);
	}*/

	//free(tracks);

	tracks.reset();
	//tracks = nullptr;
	//num_tracks = 0;

	//OPL_Unlock();
}

static void I_OPL_UnRegisterSong(midi::file_t* handle)
{
	if (!music_initialized)
	{
		return;
	}

	if (handle)
	{
		midi::FreeFile(handle);
	}
}

// Determine whether memory block is a .mid file
static bool IsMid(byte* mem, int len)
{
	return len > 4 && !memcmp(mem, "MThd", 4);
}

static bool ConvertMus(byte* musdata, int len, ::std::string filename)
{
	auto instream{mem_fopen_read(musdata, len)};
	auto outstream{mem_fopen_write()};

	auto result{mus2mid(instream, outstream)};

	if (result == 0)
	{
		// pointless variables required by these function calls
		BufferType_Void* outbuf;
		size_t outbuf_len;

		mem_get_buf(outstream, &outbuf, &outbuf_len);
		M_WriteFile(filename, outbuf, outbuf_len);
	}

	mem_fclose(instream);
	mem_fclose(outstream);

	return result;
}

static midi::file_t* I_OPL_RegisterSong(byte* data, int len)
{
	if (!music_initialized)
	{
		return (midi::file_t*)nullptr;
	}

	// MUS files begin with "MUS"; reject anything which doesnt have this signature
	auto filename{M_TempFile("doom.mid")};

	// remove MID file size limit
	if (IsMid(data, len) /* && len < MAXMIDLENGTH */)
	{
		M_WriteFile(*filename, data, len);
	}
	else
	{
		// assume a MUS file and try to convert
		ConvertMus(data, len, *filename);
	}

	auto result{midi::LoadFile(*filename)};

	if (!result)
	{
		fprintf(stderr, "I_OPL_RegisterSong: Failed to load MID.\n");
	}

	// remove file now
	remove(*filename);
	//free(filename);

	return result;
}

static bool I_OPL_MusicIsPlaying()
{
	if (!music_initialized)
	{
		return false;
	}

	return num_tracks > 0;
}

static void I_OPL_ShutdownMusic()
{
	if (music_initialized)
	{
		// Stop currently-playing track, if there is one:
		I_OPL_StopSong();

		opl::Shutdown();

		// Release GENMIDI lump
		W_ReleaseLumpName(DEH_String("genmidi"));

		music_initialized = false;
	}
}

static bool I_OPL_InitMusic()
{
	opl::SetSampleRate(snd_samplerate);

	auto chip_type{opl::Init(io_port)};
	if (chip_type == opl::init_result_t::NONE)
	{
		printf("Dude. The Adlib isn't responding.\n");
		return false;
	}

	// The DMXOPTION variable must be set to enable OPL3 support. As an extension, we also allow it to be set from the config file.
	::std::string dmxoption = getenv("DMXOPTION");
	if (!dmxoption)
	{
		dmxoption = snd_dmxoption != nullptr ? snd_dmxoption : "";
	}

	if (chip_type == opl::init_result_t::OPL3 && strstr(dmxoption, "-opl3") != NULL)
	{
		opl3mode = 1;
		num_opl_voices = opl::NUM_VOICES * 2;
	}
	else
	{
		opl3mode = 0;
		num_opl_voices = opl::NUM_VOICES;
	}

	// Secret, undocumented DMXOPTION that reverses the stereo channels into their correct orientation.
	stereo_correct = strstr(dmxoption, "-reverse") != NULL;

	// Initialize all registers.
	opl::InitRegisters(opl3mode);

	// Load instruments from GENMIDI lump:
	if (!LoadInstrumentTable())
	{
		opl::Shutdown();
		return false;
	}

	InitVoices();

	tracks = nullptr;
	num_tracks = 0;
	music_initialized = true;

	return true;
}

static snddevice_t music_opl_devices[] =
{
	snddevice_t::ADLIB,
	snddevice_t::SB
};

music_module_t music_opl_module =
{
	music_opl_devices,
	arrlen(music_opl_devices),
	I_OPL_InitMusic,
	I_OPL_ShutdownMusic,
	I_OPL_SetMusicVolume,
	I_OPL_PauseSong,
	I_OPL_ResumeSong,
	I_OPL_RegisterSong,
	I_OPL_UnRegisterSong,
	I_OPL_PlaySong,
	I_OPL_StopSong,
	I_OPL_MusicIsPlaying,
	nullptr
};

void I_SetOPLDriverVer(driver_ver_t ver)
{
	drv_ver = ver;
}

//----------------------------------------------------------------------
// Development / debug message generation, to help developing GENMIDI lumps.
//----------------------------------------------------------------------

static int NumActiveChannels()
{
	for (size_t i{midi::CHANNELS_PER_TRACK - 1}; i >= 0; --i)
	{
		if (channels[i].instrument != &main_instrs[0])
		{
			return i + 1;
		}
	}

	return 0;
}

static bool ChannelInUse(channel_data_t* channel)
{
	for (size_t i{0}; i < voice_alloced_num; ++i)
	{
		if (voice_alloced_list[i]->channel == channel)
		{
			return true;
		}
	}

	return false;
}

void I_OPL_DevMessages(::std::string result, size_t result_len)
{
	if (num_tracks == 0)
	{
		M_snprintf(result, result_len, "No OPL track!");
		return;
	}

	M_snprintf(result, result_len, "Tracks:\n");
	auto lines{1};

	char tmp[80];
	for (size_t i{0}; i < NumActiveChannels(); ++i)
	{
		if (channels[i].instrument == NULL)
		{
			continue;
		}

		auto instr_num{channels[i].instrument - main_instrs};

		// FIXME remove this ternery argument
		M_snprintf(tmp, sizeof(tmp), "chan %i: %c i#%i (%s)\n", i, ChannelInUse(&channels[i]) ? '\'' : ' ', instr_num + 1, main_instr_names[instr_num]);
		M_StringConcat(result, tmp, result_len);

		++lines;
	}

	M_snprintf(tmp, sizeof(tmp), "\nLast percussion:\n");
	M_StringConcat(result, tmp, result_len);
	lines += 2;

	{
		auto i{(last_perc_count + PERCUSSION_LOG_LEN - 1) % PERCUSSION_LOG_LEN};
		do {
			if (last_perc[i] == 0)
			{
				break;
			}

			// FIXME remove this ternery argument
			M_snprintf(tmp, sizeof(tmp), "%cp#%i (%s)\n", i == 0 ? '\'' : ' ', last_perc[i], percussion_names[last_perc[i] - 35]);
			M_StringConcat(result, tmp, result_len);
			++lines;

			i = (i + PERCUSSION_LOG_LEN - 1) % PERCUSSION_LOG_LEN;
		} while (lines < 25 && i != last_perc_count);
	}
}

}
