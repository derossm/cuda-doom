/**********************************************************************************************************************************************\
	Copyright(C) 1993-1996 Id Software, Inc.
	Copyright(C) 2005-2014 Simon Howard

	This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License
	as published by the Free Software Foundation; either version 2 of the License, or (at your option) any later version.

	This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

	DESCRIPTION:
		The not so system specific sound interface.
\**********************************************************************************************************************************************/
#pragma once

#include <string>

#include "../derma/common.h"

#include "doomtype.h"

// so that the individual game logic and sound driver code agree
constexpr size_t NORM_PITCH{127};

struct sfxinfo_t
{
	// tag name, used for hexen.
	std::string tagname;

	// lump name. If we are running with use_sfx_prefix=true, a
	// 'DS' (or 'DP' for PC speaker sounds) is prepended to this.

	char name[9];

	// Sfx priority
	int priority;

	// referenced sound if a link
	sfxinfo_t* link;

	// pitch if a link (Doom), whether to pitch-shift (Hexen)
	int pitch;

	// volume if a link
	int volume;

	// this is checked every second to see if sound
	// can be thrown out (if 0, then decrement, if -1,
	// then throw out, if > 0, then it is in use)
	int usefulness;

	// lump number of sfx
	int lumpnum;

	// Maximum number of channels that the sound can be played on
	// (Heretic)
	int numchannels;

	// data used by the low level code
	void* driver_data;
};

// MusicInfo struct.
struct musicinfo_t
{
	// up to 6-character name
	std::string name;

	// lump number of music
	int lumpnum;

	// music data
	void* data;

	// music handle once registered
	void* handle;

};

enum class snddevice_t
{
	NONE = 0,
	PCSPEAKER,
	ADLIB,
	SB,
	PAS,
	GUS,
	WAVEBLASTER,
	SOUNDCANVAS,
	GENMIDI,
	AWE32,
	CD
};

// Interface for sound modules
struct sound_module_t
{
	// List of sound devices that this sound module is used for.
	snddevice_t* sound_devices;
	int num_sound_devices;

	// Initialize sound module. Returns true if successfully initialized.
	bool (*Init)(bool use_sfx_prefix);

	// Shutdown sound module
	void (*Shutdown)();

	// Returns the lump index of the given sound.
	int (*GetSfxLumpNum)(sfxinfo_t* sfxinfo);

	// Called periodically to update the subsystem.
	void (*Update)();

	// Update the sound settings on the given channel.
	void (*UpdateSoundParams)(int channel, int vol, int sep);

	// Start a sound on a given channel. Returns the channel id or -1 on failure.
	int (*StartSound)(sfxinfo_t* sfxinfo, int channel, int vol, int sep, int pitch);

	// Stop the sound playing on the given channel.
	void (*StopSound)(int channel);

	// Query if a sound is playing on the given channel
	bool (*SoundIsPlaying)(int channel);

	// Called on startup to precache sound effects (if necessary)
	void (*CacheSounds)(sfxinfo_t* sounds, int num_sounds);

};

void I_InitSound(bool use_sfx_prefix);
void I_ShutdownSound();
int I_GetSfxLumpNum(sfxinfo_t* sfxinfo);
void I_UpdateSound();
void I_UpdateSoundParams(int channel, int vol, int sep);
int I_StartSound(sfxinfo_t* sfxinfo, int channel, int vol, int sep, int pitch);
void I_StopSound(int channel);
bool I_SoundIsPlaying(int channel);
void I_PrecacheSounds(sfxinfo_t* sounds, int num_sounds);

// Interface for music modules
struct music_module_t
{
	// List of sound devices that this music module is used for.
	snddevice_t* sound_devices;
	int num_sound_devices;

	// Initialise the music subsystem
	bool (*Init)();

	// Shutdown the music subsystem
	void (*Shutdown)();

	// Set music volume - range 0-127
	void (*SetMusicVolume)(int volume);

	// Pause music
	void (*PauseMusic)();

	// Un-pause music
	void (*ResumeMusic)();

	// Register a song handle from data. Returns a handle that can be used to play the song
	void* (*RegisterSong)(void* data, int len);

	// Un-register (free) song data
	void (*UnRegisterSong)(void* handle);

	// Play the song
	void (*PlaySong)(void* handle, bool looping);

	// Stop playing the current song.
	void (*StopSong)();

	// Query if music is playing.
	bool (*MusicIsPlaying)();

	// Invoked periodically to poll.
	void (*Poll)();
};

void I_InitMusic();
void I_ShutdownMusic();
void I_SetMusicVolume(int volume);
void I_PauseSong();
void I_ResumeSong();
void* I_RegisterSong(void* data, int len);
void I_UnRegisterSong(void* handle);
void I_PlaySong(void* handle, bool looping);
void I_StopSong();
bool I_MusicIsPlaying();

extern int snd_sfxdevice;
extern int snd_musicdevice;
extern int snd_samplerate;
extern int snd_cachesize;
extern int snd_maxslicetime_ms;
extern std::string snd_musiccmd;
extern int snd_pitchshift;

void I_BindSoundVariables();

// DMX version to emulate for OPL emulation:
enum class opl_driver_ver_t
{
	opl_doom1_1_666,	// Doom 1 v1.666
	opl_doom2_1_666,	// Doom 2 v1.666, Hexen, Heretic
	opl_doom_1_9		// Doom v1.9, Strife
};

void I_SetOPLDriverVer(opl_driver_ver_t ver);
