/**********************************************************************************************************************************************\
	Copyright(C) 2012 James Haley
	Copyright(C) 2017 Alex Mayfield

	This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License
	as published by the Free Software Foundation; either version 2 of the License, or (at your option) any later version.

	This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

	DESCRIPTION:
		Win32/SDL_mixer MIDI Server
		Uses pipes to communicate with Doom. This allows this separate process to have its own independent volume control even under
		Windows Vista and later where the mixer model doesn't support separate volumes to different devices for the same process.
\**********************************************************************************************************************************************/
#include "../derma/stdafx.h"

#include "buffer.h"

namespace cudadoom::midi
{

static HANDLE midi_process_in;		// Standard In.
static HANDLE midi_process_out;		// Standard Out.

// Sound sample rate to use for digital output (Hz)
static int snd_samplerate{0};

// Currently playing music track.
static Mix_Music* music{nullptr};

// Write an unsignedeger into a simple CHAR buffer.
static bool WriteInt16(CHAR* out, size_t osize, cudadoom::midi::PacketType in)
{
	if (osize < 2)
	{
		return false;
	}

	// FIXME replace shift math
	out[0] = ((int)in >> 8) & 0xff;
	out[1] = (int)in & 0xff;

	return true;
}

// Cleanly close our in-use pipes.
static void FreePipes()
{
	if (midi_process_in)
	{
		CloseHandle(midi_process_in);
		midi_process_in = nullptr;
	}
	if (midi_process_out)
	{
		CloseHandle(midi_process_out);
		midi_process_out = nullptr;
	}
}

// Unregisters the currently playing song. This is never called from the protocol, we simply do this before playing a new song.
static void UnregisterSong()
{
	if (!music)
	{
		return;
	}

	Mix_FreeMusic(music);
	music = nullptr;
}

static void ShutdownSDL()
{
	UnregisterSong();
	Mix_CloseAudio();
	SDL_Quit();
}

//=============================================================================
// SDL_mixer Interface
static bool RegisterSong(::std::string filename)
{
	music = Mix_LoadMUS(filename.c_str());

	// Remove the temporary MIDI file
	remove(filename.c_str());

	if (!music)
	{
		return false;
	}

	return true;
}

static void SetVolume(int vol)
{
	Mix_VolumeMusic(vol);
}

static void PlaySong(int loops)
{
	Mix_PlayMusic(music, loops);

	// BUG: In my testing, setting the volume of a MIDI track while there is no song playing appears to be a no-op. This can happen when you're
	// mixing midiproc with vanilla SDL_Mixer, such as when you are alternating between a digital music pack (in the parent process) and MIDI
	// (in this process). To work around this bug, we set the volume to itself after the MIDI has started playing.
	Mix_VolumeMusic(Mix_VolumeMusic(-1));
}

static void StopSong()
{
	Mix_HaltMusic();
}

//=============================================================================
// Pipe Server Interface
bool MidiPipe_RegisterSong(BufferReader* reader)
{
	auto filename{reader->ReadString()};
	if (!filename.empty())
	{
		return false;
	}

	return RegisterSong(filename);
}

bool MidiPipe_UnregisterSong()
{
	UnregisterSong();
	return true;
}

bool MidiPipe_SetVolume(BufferReader* reader)
{
	if (auto volume{reader->ReadInt32()}; volume)
	{
		SetVolume(volume);
		return true;
	}
	else
	{
		// TODO why not SetVolume(0);
		return false;
	}
}

// FIXME - I NEED MY OWN BUFFER SO I CAN BE A SEPARATE THREAD - MAYBE?
bool MidiPipe_PlaySong(BufferReader* reader)
{
	if (auto loops{reader->ReadInt32()}; loops)
	{
		PlaySong(loops);
		return true;
	}
	else
	{
		return false;
	}
}

bool MidiPipe_StopSong()
{
	StopSong();

	return true;
}

bool MidiPipe_Shutdown()
{
	exit(EXIT_SUCCESS);
}

//=============================================================================
// Server Implementation
// Parses a command and directs to the proper read function.
bool ParseCommand(BufferReader* reader, PacketType command)
{
	switch (command)
	{
	case cudadoom::midi::PacketType::REGISTER_SONG:
		return MidiPipe_RegisterSong(reader);
	case cudadoom::midi::PacketType::UNREGISTER_SONG:
		return MidiPipe_UnregisterSong();
	case cudadoom::midi::PacketType::SET_VOLUME:
		return MidiPipe_SetVolume(reader);
	case cudadoom::midi::PacketType::PLAY_SONG:
		return MidiPipe_PlaySong(reader);
	case cudadoom::midi::PacketType::STOP_SONG:
		return MidiPipe_StopSong();
	case cudadoom::midi::PacketType::SHUTDOWN:
		return MidiPipe_Shutdown();
	default:
		return false;
	}
}

// Server packet parser
bool ParseMessage(Buffer* buf)
{
	uint16_t command;
	auto reader{std::make_unique<BufferReader>(buf)};

	// Attempt to read a command out of the buffer.
	// FIXME consider what is error now
	if (command = reader->ReadInt16(); true)
	{
		// Attempt to parse a complete message.
		if (ParseCommand(reader.get(), static_cast<cudadoom::midi::PacketType>(command)))
		{
			// We parsed a complete message! We can now safely shift the prior message off the front of the buffer.
			buf->Shift(reader->BytesRead());

			CHAR buffer[2];
			DWORD bytes_written;
			// Send acknowledgement back that the command has completed.
			if (WriteInt16(buffer, sizeof(buffer), cudadoom::midi::PacketType::ACK))
			{
				WriteFile(midi_process_out, buffer, sizeof(buffer), &bytes_written, NULL);
				return true;
			}
		}
	}

	// We did not read a complete packet. Delete our reader and try again with more data.
	return false;
}

// The main pipe "listening" loop
bool ListenForever()
{
	//auto buffer{NewBuffer()};
	auto buffer{std::make_unique<Buffer>()};
	DWORD pipe_buffer_read{0};
	std::array<uint8_t, BUFFER_SIZE> pipe_buffer;

	for (;;)
	{
		// Wait until we see some data on the pipe.
		if (auto ok{bool(PeekNamedPipe(midi_process_in, NULL, 0, NULL, &pipe_buffer_read, NULL))};
			!ok)
		{
			break;
		}
		else if (pipe_buffer_read == 0)
		{
			SDL_Delay(1);
			continue;
		}

		// Read data off the pipe and add it to the buffer.
		if (auto ok{bool(ReadFile(midi_process_in, LPVOID(pipe_buffer.data()), sizeof(pipe_buffer), &pipe_buffer_read, NULL))};
			!ok)
		{
			break;
		}

		if (auto ok{buffer->Push(pipe_buffer, pipe_buffer_read)};
			ok == PacketType::error)
		{
			break;
		}

		for (bool ok{true};
			ok; )
		{
			// Read messages off the buffer until we can't anymore.
			ok = ParseMessage(buffer.get());
		}
	}

	return false;
}

//=============================================================================
// Main Program

// Start up SDL and SDL_mixer.
bool InitSDL()
{
	if (SDL_Init(SDL_INIT_AUDIO) < 0)
	{
		return false;
	}

	if (Mix_OpenAudioDevice(snd_samplerate, MIX_DEFAULT_FORMAT, 2, 2048, NULL, SDL_AUDIO_ALLOW_FREQUENCY_CHANGE) < 0)
	{
		return false;
	}

	atexit(ShutdownSDL);

	return true;
}

// Ensure that we can communicate.
void InitPipes(HANDLE in, HANDLE out)
{
	midi_process_in = in;
	midi_process_out = out;

	atexit(FreePipes);
}

extern "C"
{
int main(int argc, char* argv[])
{
	// Make sure we're not launching this process by itself.
	if (argc < 5)
	{
		MessageBox(NULL, TEXT("This program is tasked with playing Native MIDI music, and is intended to be launched by ")
			TEXT(PACKAGE_NAME) TEXT("."), TEXT(PACKAGE_STRING), MB_OK | MB_ICONASTERISK);

		return EXIT_FAILURE;
	}

	// Make sure our Choccolate Doom and midiproc version are lined up.
	if (strcmp(PACKAGE_STRING, argv[1]) != 0)
	{
		std::stringstream message;
		message << "It appears that the version of " << PACKAGE_NAME << " and " << PROGRAM_PREFIX << "midiproc are out of sync. Please reinstall " << PACKAGE_NAME << ".\n\nServer Version: " << PACKAGE_STRING << "\nClient Version: " << argv[1];

		MessageBox(NULL, TEXT(message.str().c_str()), TEXT(PACKAGE_STRING), MB_OK | MB_ICONASTERISK);

		return EXIT_FAILURE;
	}

	// Parse out the sample rate - if we can't, default to `DEFAULT_SAMPLE_RATE` defined in buffer.h
	::std::from_chars(argv[2], nullptr, snd_samplerate);
	if (snd_samplerate == LONG_MAX || snd_samplerate == LONG_MIN || snd_samplerate == 0)
	{
		snd_samplerate = DEFAULT_SAMPLE_RATE;
	}

	// Parse out our handle ids.
	uintptr_t tmp;

	::std::from_chars(argv[3], nullptr, tmp);
	HANDLE in{(void*)tmp};
	if (in == 0)
	{
		return EXIT_FAILURE;
	}

	::std::from_chars(argv[4], nullptr, tmp);
	HANDLE out{(void*)tmp};
	if (out == 0)
	{
		return EXIT_FAILURE;
	}

	InitPipes(in, out);

	if (!InitSDL())
	{
		return EXIT_FAILURE;
	}

	if (!ListenForever())
	{
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}
}

}
