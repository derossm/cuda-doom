/**********************************************************************************************************************************************\
	Copyright(C) 2005-2014 Simon Howard

	This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License
	as published by the Free Software Foundation; either version 2 of the License, or (at your option) any later version.

	This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
\**********************************************************************************************************************************************/
#pragma once

#include "../../derma/common.h"

#include "../i_sound.h"
#include "../m_config.h"
#include "../m_misc.h"

#include "execute.h"
#include "mode.h"

#define WINDOW_HELP_URL "https://www.chocolate-doom.org/setup-sound"

void ConfigSound(void* widget, void* user_data);
void BindSoundVariables();

extern ::std::string snd_dmxoption;

enum class oplmode_t
{
	OPLMODE_OPL2,
	OPLMODE_OPL3,
	NUM_OPLMODES,
};

static ::std::string opltype_strings[] =
{
	"OPL2",
	"OPL3"
};

static ::std::string cfg_extension[] = {"cfg", NULL};

// Config file variables:
int snd_sfxdevice = snddevice_t::SB;
int snd_musicdevice = snddevice_t::SB;
int snd_samplerate = 44100;
int opl_io_port = 0x388;
int snd_cachesize = 64 * 1024 * 1024;
int snd_maxslicetime_ms = 28;
::std::string snd_musiccmd = "";
int snd_pitchshift = 0;
::std::string snd_dmxoption = "-opl3"; // default to OPL3 emulation

static int numChannels = 8;
static int sfxVolume = 8;
static int musicVolume = 8;
static int voiceVolume = 15;
static int show_talk = 0;
// values 3 and higher might reproduce DOOM.EXE more accurately,
// but 1 is closer to "use_libsamplerate = 0" which is the default in Choco
// and causes only a short delay at startup
static int use_libsamplerate = 1;
static float libsamplerate_scale = 0.65;

::std::string music_pack_path = NULL;
::std::string timidity_cfg_path = NULL;
::std::string gus_patch_path = NULL;
static int gus_ram_kb = 1024;

// DOS specific variables: these are unused but should be maintained
// so that the config file can be shared between chocolate
// doom and doom.exe
static int snd_sbport = 0;
static int snd_sbirq = 0;
static int snd_sbdma = 0;
static int snd_mport = 0;

static int snd_oplmode;

static void UpdateSndDevices(txt::UNCAST_ARG(widget), txt::UNCAST_ARG(data))
{
	switch (snd_oplmode)
	{
	default:
	case OPLMODE_OPL2:
		snd_dmxoption = "";
		break;

	case OPLMODE_OPL3:
		snd_dmxoption = "-opl3";
		break;
	}
}

static txt::txt_dropdown_list_t* OPLTypeSelector()
{
	txt::txt_dropdown_list_t* result;

	if (snd_dmxoption != NULL && strstr(snd_dmxoption, "-opl3") != NULL)
	{
		snd_oplmode = OPLMODE_OPL3;
	}
	else
	{
		snd_oplmode = OPLMODE_OPL2;
	}

	result = txt::NewDropdownList(&snd_oplmode, opltype_strings, 2);

	txt::SignalConnect(result, "changed", UpdateSndDevices, NULL);

	return result;
}

static void OpenMusicPackDir(txt::UNCAST_ARG(widget), txt::UNCAST_ARG(unused))
{
	if (!OpenFolder(music_pack_path))
	{
		txt::MessageBox("Error", "Failed to open music pack directory.");
	}
}

void ConfigSound(txt::UNCAST_ARG(widget), void* user_data)
{
	txt::Window* window;
	txt::WindowAction* music_action;

	// Build the window
	window = txt::NewWindow("Sound configuration");
	txt::SetWindowHelpURL(window, WINDOW_HELP_URL);

	txt::SetColumnWidths(window, 40);
	txt::SetWindowPosition(window, txt::HORIZ_CENTER, txt::VERT_TOP,
		txt::SCREEN_W / 2, 3);

	music_action = txt::NewWindowAction('m', "Music Packs");
	txt::SetWindowAction(window, txt::HORIZ_CENTER, music_action);
	txt::SignalConnect(music_action, "pressed", OpenMusicPackDir, NULL);

	txt::AddWidgets(window,
		txt::NewSeparator("Sound effects"),
		txt::NewRadioButton("Disabled", &snd_sfxdevice, snddevice_t::NONE),
		txt::If(gamemission == doom,
			txt::NewRadioButton("PC speaker effects", &snd_sfxdevice,
				snddevice_t::PCSPEAKER)),
		txt::NewRadioButton("Digital sound effects",
			&snd_sfxdevice,
			snddevice_t::SB),
		txt::If(gamemission == doom || gamemission == heretic
			|| gamemission == hexen,
			txt::NewConditional(&snd_sfxdevice, snddevice_t::SB,
				txt::MakeHorizontalTable(
					txt::NewStrut(4, 0),
					txt::NewCheckBox("Pitch-shifted sounds", &snd_pitchshift),
					NULL))),
		txt::If(gamemission == strife,
			txt::NewConditional(&snd_sfxdevice, snddevice_t::SB,
				txt::MakeHorizontalTable(
					txt::NewStrut(4, 0),
					txt::NewCheckBox("Show text with voices", &show_talk),
					NULL))),

		txt::NewSeparator("Music"),
		txt::NewRadioButton("Disabled", &snd_musicdevice, snddevice_t::NONE),

		txt::NewRadioButton("OPL (Adlib/Soundblaster)", &snd_musicdevice,
			snddevice_t::SB),
		txt::NewConditional(&snd_musicdevice, snddevice_t::SB,
			txt::MakeHorizontalTable(
				txt::NewStrut(4, 0),
				txt::NewLabel("Chip type: "),
				OPLTypeSelector(),
				NULL)),

		txt::NewRadioButton("GUS (emulated)", &snd_musicdevice, snddevice_t::GUS),
		txt::NewConditional(&snd_musicdevice, snddevice_t::GUS,
			txt::MakeTable(2,
				txt::NewStrut(4, 0),
				txt::NewLabel("Path to patch files: "),
				txt::NewStrut(4, 0),
				txt::NewFileSelector(&gus_patch_path, 34,
					"Select directory containing GUS patches",
					txt::DIRECTORY),
				NULL)),

		txt::NewRadioButton("MIDI/MP3/OGG/FLAC", &snd_musicdevice, snddevice_t::GENMIDI), // improve ambigious music backend name
		txt::NewConditional(&snd_musicdevice, snddevice_t::GENMIDI,
			txt::MakeTable(2,
				txt::NewStrut(4, 0),
				txt::NewLabel("Timidity configuration file: "),
				txt::NewStrut(4, 0),
				txt::NewFileSelector(&timidity_cfg_path, 34,
					"Select Timidity config file",
					cfg_extension),
				NULL)),
		NULL);
}

void BindSoundVariables()
{
	M_BindVariable<int>("snd_sfxdevice", &snd_sfxdevice);
	M_BindVariable<int>("snd_musicdevice", &snd_musicdevice);
	M_BindVariable<int>("snd_channels", &numChannels);
	M_BindVariable<int>("snd_samplerate", &snd_samplerate);
	M_BindVariable<int>("sfx_volume", &sfxVolume);
	M_BindVariable<int>("music_volume", &musicVolume);

	M_BindVariable<int>("use_libsamplerate", &use_libsamplerate);
	M_BindVariable<float>("libsamplerate_scale", &libsamplerate_scale);

	M_BindVariable<int>("gus_ram_kb", &gus_ram_kb);
	M_BindVariable<::std::string>("gus_patch_path", &gus_patch_path);
	M_BindVariable<::std::string>("music_pack_path", &music_pack_path);
	M_BindVariable<::std::string>("timidity_cfg_path", &timidity_cfg_path);

	M_BindVariable<int>("snd_sbport", &snd_sbport);
	M_BindVariable<int>("snd_sbirq", &snd_sbirq);
	M_BindVariable<int>("snd_sbdma", &snd_sbdma);
	M_BindVariable<int>("snd_mport", &snd_mport);
	M_BindVariable<int>("snd_maxslicetime_ms", &snd_maxslicetime_ms);
	M_BindVariable<::std::string>("snd_musiccmd", &snd_musiccmd);
	M_BindVariable<::std::string>("snd_dmxoption", &snd_dmxoption);

	M_BindVariable<int>("snd_cachesize", &snd_cachesize);
	M_BindVariable<int>("opl_io_port", &opl_io_port);

	M_BindVariable<int>("snd_pitchshift", &snd_pitchshift);

	if (gamemission == GameMission::strife)
	{
		M_BindVariable<int>("voice_volume", &voiceVolume);
		M_BindVariable<int>("show_talk", &show_talk);
	}

	music_pack_path = ::std::string("");
	timidity_cfg_path = ::std::string("");
	gus_patch_path = ::std::string("");

	// All versions of Heretic and Hexen did pitch-shifting.
	// Most versions of Doom did not and Strife never did.
	snd_pitchshift = (gamemission == GameMission::heretic || gamemission == GameMission::hexen);

	// Default sound volumes - different games use different values.
	switch (gamemission)
	{
	case GameMission::doom:
	default:
		sfxVolume = 8;
		musicVolume = 8;
		break;
	case GameMission::heretic:
	case GameMission::hexen:
		sfxVolume = 10;
		musicVolume = 10;
		break;
	case GameMission::strife:
		sfxVolume = 8;
		musicVolume = 13;
		break;
	}
}
