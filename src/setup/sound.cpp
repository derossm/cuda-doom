/**********************************************************************************************************************************************\
	Copyright(C) 2005-2014 Simon Howard

	This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License
	as published by the Free Software Foundation; either version 2 of the License, or (at your option) any later version.

	This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
\**********************************************************************************************************************************************/

#include "SDL_mixer.h"

#include "textscreen.h"
#include "m_config.h"
#include "m_misc.h"

#include "execute.h"
#include "mode.h"
#include "sound.h"

#define WINDOW_HELP_URL "https://www.chocolate-doom.org/setup-sound"

typedef enum
{
	OPLMODE_OPL2,
	OPLMODE_OPL3,
	NUM_OPLMODES,
} oplmode_t;

static const char *opltype_strings[] =
{
	"OPL2",
	"OPL3"
};

static const char *cfg_extension[] = { "cfg", NULL };

// Config file variables:
int snd_sfxdevice = SNDDEVICE_SB;
int snd_musicdevice = SNDDEVICE_SB;
int snd_samplerate = 44100;
int opl_io_port = 0x388;
int snd_cachesize = 64 * 1024 * 1024;
int snd_maxslicetime_ms = 28;
char *snd_musiccmd = "";
int snd_pitchshift = 0;
char *snd_dmxoption = "-opl3"; // [crispy] default to OPL3 emulation

static int numChannels = 8;
static int sfxVolume = 8;
static int musicVolume = 8;
static int voiceVolume = 15;
static int show_talk = 0;
// [crispy] values 3 and higher might reproduce DOOM.EXE more accurately,
// but 1 is closer to "use_libsamplerate = 0" which is the default in Choco
// and causes only a short delay at startup
static int use_libsamplerate = 1;
static float libsamplerate_scale = 0.65;

static char *music_pack_path = NULL;
static char *timidity_cfg_path = NULL;
static char *gus_patch_path = NULL;
static int gus_ram_kb = 1024;

// DOS specific variables: these are unused but should be maintained
// so that the config file can be shared between chocolate
// doom and doom.exe
static int snd_sbport = 0;
static int snd_sbirq = 0;
static int snd_sbdma = 0;
static int snd_mport = 0;

static int snd_oplmode;

static void UpdateSndDevices(cudadoom::txt::TXT_UNCAST_ARG(widget), cudadoom::txt::TXT_UNCAST_ARG(data))
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

static cudadoom::txt::txt_dropdown_list_t *OPLTypeSelector()
{
	cudadoom::txt::txt_dropdown_list_t *result;

	if (snd_dmxoption != NULL && strstr(snd_dmxoption, "-opl3") != NULL)
	{
		snd_oplmode = OPLMODE_OPL3;
	}
	else
	{
		snd_oplmode = OPLMODE_OPL2;
	}

	result = cudadoom::txt::TXT_NewDropdownList(&snd_oplmode, opltype_strings, 2);

	cudadoom::txt::TXT_SignalConnect(result, "changed", UpdateSndDevices, NULL);

	return result;
}

static void OpenMusicPackDir(cudadoom::txt::TXT_UNCAST_ARG(widget), cudadoom::txt::TXT_UNCAST_ARG(unused))
{
	if (!OpenFolder(music_pack_path))
	{
		cudadoom::txt::TXT_MessageBox("Error", "Failed to open music pack directory.");
	}
}

void ConfigSound(cudadoom::txt::TXT_UNCAST_ARG(widget), void *user_data)
{
	cudadoom::txt::txt_window_t *window;
	cudadoom::txt::txt_window_action_t *music_action;

	// Build the window
	window = cudadoom::txt::TXT_NewWindow("Sound configuration");
	cudadoom::txt::TXT_SetWindowHelpURL(window, WINDOW_HELP_URL);

	cudadoom::txt::TXT_SetColumnWidths(window, 40);
	cudadoom::txt::TXT_SetWindowPosition(window, cudadoom::txt::TXT_HORIZ_CENTER, cudadoom::txt::TXT_VERT_TOP,
									cudadoom::txt::TXT_SCREEN_W / 2, 3);

	music_action = cudadoom::txt::TXT_NewWindowAction('m', "Music Packs");
	cudadoom::txt::TXT_SetWindowAction(window, cudadoom::txt::TXT_HORIZ_CENTER, music_action);
	cudadoom::txt::TXT_SignalConnect(music_action, "pressed", OpenMusicPackDir, NULL);

	cudadoom::txt::TXT_AddWidgets(window,
		cudadoom::txt::TXT_NewSeparator("Sound effects"),
		cudadoom::txt::TXT_NewRadioButton("Disabled", &snd_sfxdevice, SNDDEVICE_NONE),
		cudadoom::txt::TXT_If(gamemission == doom,
			cudadoom::txt::TXT_NewRadioButton("PC speaker effects", &snd_sfxdevice,
								SNDDEVICE_PCSPEAKER)),
		cudadoom::txt::TXT_NewRadioButton("Digital sound effects",
							&snd_sfxdevice,
							SNDDEVICE_SB),
		cudadoom::txt::TXT_If(gamemission == doom || gamemission == heretic
			|| gamemission == hexen,
			cudadoom::txt::TXT_NewConditional(&snd_sfxdevice, SNDDEVICE_SB,
				cudadoom::txt::TXT_NewHorizBox(
					cudadoom::txt::TXT_NewStrut(4, 0),
					cudadoom::txt::TXT_NewCheckBox("Pitch-shifted sounds", &snd_pitchshift),
					NULL))),
		cudadoom::txt::TXT_If(gamemission == strife,
			cudadoom::txt::TXT_NewConditional(&snd_sfxdevice, SNDDEVICE_SB,
				cudadoom::txt::TXT_NewHorizBox(
					cudadoom::txt::TXT_NewStrut(4, 0),
					cudadoom::txt::TXT_NewCheckBox("Show text with voices", &show_talk),
					NULL))),

		cudadoom::txt::TXT_NewSeparator("Music"),
		cudadoom::txt::TXT_NewRadioButton("Disabled", &snd_musicdevice, SNDDEVICE_NONE),

		cudadoom::txt::TXT_NewRadioButton("OPL (Adlib/Soundblaster)", &snd_musicdevice,
							SNDDEVICE_SB),
		cudadoom::txt::TXT_NewConditional(&snd_musicdevice, SNDDEVICE_SB,
			cudadoom::txt::TXT_NewHorizBox(
				cudadoom::txt::TXT_NewStrut(4, 0),
				cudadoom::txt::TXT_NewLabel("Chip type: "),
				OPLTypeSelector(),
				NULL)),

		cudadoom::txt::TXT_NewRadioButton("GUS (emulated)", &snd_musicdevice, SNDDEVICE_GUS),
		cudadoom::txt::TXT_NewConditional(&snd_musicdevice, SNDDEVICE_GUS,
			cudadoom::txt::TXT_MakeTable(2,
				cudadoom::txt::TXT_NewStrut(4, 0),
				cudadoom::txt::TXT_NewLabel("Path to patch files: "),
				cudadoom::txt::TXT_NewStrut(4, 0),
				cudadoom::txt::TXT_NewFileSelector(&gus_patch_path, 34,
									"Select directory containing GUS patches",
									cudadoom::txt::TXT_DIRECTORY),
				NULL)),

		cudadoom::txt::TXT_NewRadioButton("MIDI/MP3/OGG/FLAC", &snd_musicdevice, SNDDEVICE_GENMIDI), // [crispy] improve ambigious music backend name
		cudadoom::txt::TXT_NewConditional(&snd_musicdevice, SNDDEVICE_GENMIDI,
			cudadoom::txt::TXT_MakeTable(2,
				cudadoom::txt::TXT_NewStrut(4, 0),
				cudadoom::txt::TXT_NewLabel("Timidity configuration file: "),
				cudadoom::txt::TXT_NewStrut(4, 0),
				cudadoom::txt::TXT_NewFileSelector(&timidity_cfg_path, 34,
									"Select Timidity config file",
									cfg_extension),
				NULL)),
		NULL);
}

void BindSoundVariables()
{
	M_BindIntVariable("snd_sfxdevice",			&snd_sfxdevice);
	M_BindIntVariable("snd_musicdevice",			&snd_musicdevice);
	M_BindIntVariable("snd_channels",				&numChannels);
	M_BindIntVariable("snd_samplerate",			&snd_samplerate);
	M_BindIntVariable("sfx_volume",				&sfxVolume);
	M_BindIntVariable("music_volume",				&musicVolume);

	M_BindIntVariable("use_libsamplerate",		&use_libsamplerate);
	M_BindFloatVariable("libsamplerate_scale",	&libsamplerate_scale);

	M_BindIntVariable("gus_ram_kb",				&gus_ram_kb);
	M_BindStringVariable("gus_patch_path",		&gus_patch_path);
	M_BindStringVariable("music_pack_path",		&music_pack_path);
	M_BindStringVariable("timidity_cfg_path",		&timidity_cfg_path);

	M_BindIntVariable("snd_sbport",				&snd_sbport);
	M_BindIntVariable("snd_sbirq",				&snd_sbirq);
	M_BindIntVariable("snd_sbdma",				&snd_sbdma);
	M_BindIntVariable("snd_mport",				&snd_mport);
	M_BindIntVariable("snd_maxslicetime_ms",		&snd_maxslicetime_ms);
	M_BindStringVariable("snd_musiccmd",			&snd_musiccmd);
	M_BindStringVariable("snd_dmxoption",			&snd_dmxoption);

	M_BindIntVariable("snd_cachesize",			&snd_cachesize);
	M_BindIntVariable("opl_io_port",				&opl_io_port);

	M_BindIntVariable("snd_pitchshift",			&snd_pitchshift);

	if (gamemission == strife)
	{
		M_BindIntVariable("voice_volume",			&voiceVolume);
		M_BindIntVariable("show_talk",			&show_talk);
	}

	music_pack_path = M_StringDuplicate("");
	timidity_cfg_path = M_StringDuplicate("");
	gus_patch_path = M_StringDuplicate("");

	// All versions of Heretic and Hexen did pitch-shifting.
	// Most versions of Doom did not and Strife never did.
	snd_pitchshift = gamemission == heretic || gamemission == hexen;

	// Default sound volumes - different games use different values.
	switch (gamemission)
	{
		case doom:
		default:
			sfxVolume = 8; musicVolume = 8;
			break;
		case heretic:
		case hexen:
			sfxVolume = 10; musicVolume = 10;
			break;
		case strife:
			sfxVolume = 8; musicVolume = 13;
			break;
	}
}
