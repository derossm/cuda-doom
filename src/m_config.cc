/**********************************************************************************************************************************************\
	Copyright(C) 1993-1996 Id Software, Inc.
	Copyright(C) 1993-2008 Raven Software
	Copyright(C) 2005-2014 Simon Howard

	This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License
	as published by the Free Software Foundation; either version 2 of the License, or (at your option) any later version.

	This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

	DESCRIPTION:
		Configuration file interface.
\**********************************************************************************************************************************************/

#define CONFIG_VARIABLE_GENERIC(name, type) { #name, {NULL}, type, 0, 0, false }

#define CONFIG_VARIABLE_KEY(name) CONFIG_VARIABLE_GENERIC(name, default_type_t::KEY)
#define CONFIG_VARIABLE_INT(name) CONFIG_VARIABLE_GENERIC(name, default_type_t::INT)
#define CONFIG_VARIABLE_INT_HEX(name) CONFIG_VARIABLE_GENERIC(name, default_type_t::INT_HEX)
#define CONFIG_VARIABLE_FLOAT(name) CONFIG_VARIABLE_GENERIC(name, default_type_t::FLOAT)
#define CONFIG_VARIABLE_STRING(name) CONFIG_VARIABLE_GENERIC(name, default_type_t::STRING)

#include "m_config.h"

static default_t doom_defaults_list[] =
{
	// Mouse sensitivity. This value is used to multiply input mouse
	// movement to control the effect of moving the mouse.

	// The "normal" maximum value available for this through the
	// in-game options menu is 9. A value of 31 or greater will cause
	// the game to crash when entering the options menu.
	CONFIG_VARIABLE_INT(mouse_sensitivity),

	// Horizontal mouse sensitivity (strafe)
	CONFIG_VARIABLE_INT(mouse_sensitivity_x2),

	// Vertical mouse sensitivity
	CONFIG_VARIABLE_INT(mouse_sensitivity_y),

	// Volume of sound effects, range 0-15.
	CONFIG_VARIABLE_INT(sfx_volume),

	// Volume of in-game music, range 0-15.
	CONFIG_VARIABLE_INT(music_volume),

	// If non-zero, dialogue text is displayed over characters' pictures when engaging actors who have voices.
	CONFIG_VARIABLE_INT(show_talk),

	// Volume of voice sound effects, range 0-15.
	CONFIG_VARIABLE_INT(voice_volume),

	// If non-zero, messages are displayed on the heads-up display in the game ("picked up a clip", etc). If zero, these messages are not displayed.
	CONFIG_VARIABLE_INT(show_messages),

	// Keyboard key to turn right.
	CONFIG_VARIABLE_KEY(key_right),

	// Keyboard key to turn left.
	CONFIG_VARIABLE_KEY(key_left),

	// Keyboard key to move forward.
	CONFIG_VARIABLE_KEY(key_up),

	// Keyboard key to move backward.
	CONFIG_VARIABLE_KEY(key_down),

	// Keyboard key to move forward (alternative).
	CONFIG_VARIABLE_KEY(key_alt_up),

	// Keyboard key to move backward (alternative).
	CONFIG_VARIABLE_KEY(key_alt_down),

	// Keyboard key to strafe left.
	CONFIG_VARIABLE_KEY(key_strafeleft),

	// Keyboard key to strafe left (alternative).
	CONFIG_VARIABLE_KEY(key_alt_strafeleft),

	// Keyboard key to strafe right.
	CONFIG_VARIABLE_KEY(key_straferight),

	// Keyboard key to strafe right (alternative).
	CONFIG_VARIABLE_KEY(key_alt_straferight),

	// Keyboard key to use health.
	CONFIG_VARIABLE_KEY(key_useHealth),

	// Keyboard key to jump.
	CONFIG_VARIABLE_KEY(key_jump),

	// @game heretic hexen
	//
	// Keyboard key to fly upward.
	CONFIG_VARIABLE_KEY(key_flyup),

	// @game heretic hexen
	//
	// Keyboard key to fly downwards.
	CONFIG_VARIABLE_KEY(key_flydown),

	// @game heretic hexen
	//
	// Keyboard key to center flying.
	CONFIG_VARIABLE_KEY(key_flycenter),

	// @game heretic hexen
	//
	// Keyboard key to look up.
	CONFIG_VARIABLE_KEY(key_lookup),

	// @game heretic hexen
	//
	// Keyboard key to look down.
	CONFIG_VARIABLE_KEY(key_lookdown),

	// @game heretic hexen
	//
	// Keyboard key to center the view.
	CONFIG_VARIABLE_KEY(key_lookcenter),

	// Keyboard key to query inventory.
	CONFIG_VARIABLE_KEY(key_invquery),

	// Keyboard key to display mission objective.
	CONFIG_VARIABLE_KEY(key_mission),

	// Keyboard key to display inventory popup.
	CONFIG_VARIABLE_KEY(key_invPop),

	// Keyboard key to display keys popup.
	CONFIG_VARIABLE_KEY(key_invKey),

	// Keyboard key to jump to start of inventory.
	CONFIG_VARIABLE_KEY(key_invHome),

	// Keyboard key to jump to end of inventory.
	CONFIG_VARIABLE_KEY(key_invEnd),

	// @game heretic hexen
	//
	// Keyboard key to scroll left in the inventory.
	CONFIG_VARIABLE_KEY(key_invleft),

	// @game heretic hexen
	//
	// Keyboard key to scroll right in the inventory.
	CONFIG_VARIABLE_KEY(key_invright),

	// Keyboard key to scroll left in the inventory.
	CONFIG_VARIABLE_KEY(key_invLeft),

	// Keyboard key to scroll right in the inventory.
	CONFIG_VARIABLE_KEY(key_invRight),

	// @game heretic hexen
	//
	// Keyboard key to use the current item in the inventory.
	CONFIG_VARIABLE_KEY(key_useartifact),

	// Keyboard key to use inventory item.
	CONFIG_VARIABLE_KEY(key_invUse),

	// Keyboard key to drop an inventory item.
	CONFIG_VARIABLE_KEY(key_invDrop),

	// Keyboard key to look up.
	CONFIG_VARIABLE_KEY(key_lookUp),

	// Keyboard key to look down.
	CONFIG_VARIABLE_KEY(key_lookDown),

	// Keyboard key to fire the currently selected weapon.
	CONFIG_VARIABLE_KEY(key_fire),

	// Keyboard key to "use" an object, eg. a door or switch.
	CONFIG_VARIABLE_KEY(key_use),

	// Keyboard key to turn on strafing. When held down, pressing the
	// key to turn left or right causes the player to strafe left or
	// right instead.
	CONFIG_VARIABLE_KEY(key_strafe),

	// Keyboard key to make the player run.
	CONFIG_VARIABLE_KEY(key_speed),

	// If non-zero, mouse input is enabled. If zero, mouse input is
	// disabled.
	CONFIG_VARIABLE_INT(use_mouse),

	// Mouse button to fire the currently selected weapon.
	CONFIG_VARIABLE_INT(mouseb_fire),

	// Mouse button to turn on strafing. When held down, the player
	// will strafe left and right instead of turning left and right.
	CONFIG_VARIABLE_INT(mouseb_strafe),

	// Mouse button to move forward.
	CONFIG_VARIABLE_INT(mouseb_forward),

	// @game hexen strife
	//
	// Mouse button to jump.
	CONFIG_VARIABLE_INT(mouseb_jump),

	// Mouse button to enable free looking.
	CONFIG_VARIABLE_INT(mouseb_mouselook),

	// Quick 180° reverse.
	CONFIG_VARIABLE_INT(mouseb_reverse),

	// If non-zero, joystick input is enabled.
	CONFIG_VARIABLE_INT(use_joystick),

	// Joystick virtual button that fires the current weapon.
	CONFIG_VARIABLE_INT(joyb_fire),

	// Joystick virtual button that makes the player strafe while
	// held down.
	CONFIG_VARIABLE_INT(joyb_strafe),

	// Joystick virtual button to "use" an object, eg. a door or switch.
	CONFIG_VARIABLE_INT(joyb_use),

	// Joystick virtual button that makes the player run while held
	// down.
	//
	// If this has a value of 20 or greater, the player will always run,
	// even if use_joystick is 0.
	CONFIG_VARIABLE_INT(joyb_speed),

	// @game hexen strife
	//
	// Joystick virtual button that makes the player jump.
	CONFIG_VARIABLE_INT(joyb_jump),

	// @game doom heretic hexen
	//
	// Screen size, range 3-11.
	//
	// A value of 11 gives a full-screen view with the status bar not
	// displayed. A value of 10 gives a full-screen view with the
	// status bar displayed.
	CONFIG_VARIABLE_INT(screenblocks),

	// Screen size, range 3-11.
	//
	// A value of 11 gives a full-screen view with the status bar not
	// displayed. A value of 10 gives a full-screen view with the
	// status bar displayed.
	CONFIG_VARIABLE_INT(screensize),

	// Screen detail. Zero gives normal "high detail" mode, while
	// a non-zero value gives "low detail" mode.
	CONFIG_VARIABLE_INT(detaillevel),

	// Number of sounds that will be played simultaneously.
	CONFIG_VARIABLE_INT(snd_channels),

	// Music output device. A non-zero value gives MIDI sound output,
	// while a value of zero disables music.
	CONFIG_VARIABLE_INT(snd_musicdevice),

	// Sound effects device. A value of zero disables in-game sound
	// effects, a value of 1 enables PC speaker sound effects, while
	// a value in the range 2-9 enables the "normal" digital sound
	// effects.
	CONFIG_VARIABLE_INT(snd_sfxdevice),

	// SoundBlaster I/O port. Unused.
	CONFIG_VARIABLE_INT(snd_sbport),

	// SoundBlaster IRQ. Unused.
	CONFIG_VARIABLE_INT(snd_sbirq),

	// SoundBlaster DMA channel. Unused.
	CONFIG_VARIABLE_INT(snd_sbdma),

	// Output port to use for OPL MIDI playback. Unused.
	CONFIG_VARIABLE_INT(snd_mport),

	// Gamma correction level. A value of zero disables gamma
	// correction, while a value in the range 1-4 gives increasing
	// levels of gamma correction.
	CONFIG_VARIABLE_INT(usegamma),

	// Directory in which to store savegames.
	CONFIG_VARIABLE_STRING(savedir),

	// Controls whether messages are displayed in the heads-up display.
	// If this has a non-zero value, messages are displayed.
	CONFIG_VARIABLE_INT(messageson),

	// Name of background flat used by view border.
	CONFIG_VARIABLE_STRING(back_flat),

	// Multiplayer nickname (?).
	CONFIG_VARIABLE_STRING(nickname),

	// Multiplayer chat macro: message to send when alt+0 is pressed.
	CONFIG_VARIABLE_STRING(chatmacro0),

	// Multiplayer chat macro: message to send when alt+1 is pressed.
	CONFIG_VARIABLE_STRING(chatmacro1),

	// Multiplayer chat macro: message to send when alt+2 is pressed.
	CONFIG_VARIABLE_STRING(chatmacro2),

	// Multiplayer chat macro: message to send when alt+3 is pressed.
	CONFIG_VARIABLE_STRING(chatmacro3),

	// Multiplayer chat macro: message to send when alt+4 is pressed.
	CONFIG_VARIABLE_STRING(chatmacro4),

	// Multiplayer chat macro: message to send when alt+5 is pressed.
	CONFIG_VARIABLE_STRING(chatmacro5),

	// Multiplayer chat macro: message to send when alt+6 is pressed.
	CONFIG_VARIABLE_STRING(chatmacro6),

	// Multiplayer chat macro: message to send when alt+7 is pressed.
	CONFIG_VARIABLE_STRING(chatmacro7),

	// Multiplayer chat macro: message to send when alt+8 is pressed.
	CONFIG_VARIABLE_STRING(chatmacro8),

	// Multiplayer chat macro: message to send when alt+9 is pressed.
	CONFIG_VARIABLE_STRING(chatmacro9),

	// Serial port number to use for SERSETUP.EXE (unused).
	CONFIG_VARIABLE_INT(comport),
};

//! @begin_config_file extended

static default_t extra_defaults_list[] =
{
	//!
	// Name of the SDL video driver to use. If this is an empty string,
	// the default video driver is used.
	CONFIG_VARIABLE_STRING(video_driver),

	// Position of the window on the screen when running in windowed
	// mode. Accepted values are: "" (empty string) - don't care,
	// "center" - place window at center of screen, "x,y" - place
	// window at the specified coordinates.
	CONFIG_VARIABLE_STRING(window_position),

	// If non-zero, the game will run in full screen mode. If zero,
	// the game will run in a window.
	CONFIG_VARIABLE_INT(fullscreen),

	// Index of the display on which the game should run. This has no
	// effect if running in windowed mode (fullscreen=0) and
	// window_position is not set to "center".
	CONFIG_VARIABLE_INT(video_display),

	// If non-zero, the screen will be stretched vertically to display
	// correctly on a square pixel video mode.
	CONFIG_VARIABLE_INT(aspect_ratio_correct),

	// If non-zero, forces integer scales for resolution-independent rendering.
	CONFIG_VARIABLE_INT(integer_scaling),

	// If non-zero, any pillar/letter boxes drawn around the game area
	// will "flash" when the game palette changes, simulating the VGA
	// "porch"

	CONFIG_VARIABLE_INT(vga_porch_flash),

	// Window width when running in windowed mode.
	CONFIG_VARIABLE_INT(window_width),

	// Window height when running in windowed mode.
	CONFIG_VARIABLE_INT(window_height),

	// Width for screen mode when running fullscreen.
	// If this and fullscreen_height are both set to zero, we run
	// fullscreen as a desktop window that covers the entire screen,
	// rather than ever switching screen modes. It should usually
	// be unnecessary to set this value.
	CONFIG_VARIABLE_INT(fullscreen_width),

	// Height for screen mode when running fullscreen.
	// See documentation for fullscreen_width.
	CONFIG_VARIABLE_INT(fullscreen_height),

	// If non-zero, force the use of a software renderer. For use on
	// systems lacking hardware acceleration.
	CONFIG_VARIABLE_INT(force_software_renderer),

	// Maximum number of pixels to use for intermediate scaling buffer.
	// More pixels mean that the screen can be rendered more precisely,
	// but there are diminishing returns on quality. The default limits to
	// 16,000,000 pixels, which is enough to cover 4K monitor standards.

	CONFIG_VARIABLE_INT(max_scaling_buffer_pixels),

	// Number of milliseconds to wait on startup after the video mode
	// has been set, before the game will start. This allows the
	// screen to settle on some monitors that do not display an image
	// for a brief interval after changing video modes.
	CONFIG_VARIABLE_INT(startup_delay),

	// @game heretic hexen strife
	//
	// If non-zero, display the graphical startup screen.
	CONFIG_VARIABLE_INT(graphical_startup),

	// @game doom heretic strife
	//
	// If non-zero, the ENDOOM text screen is displayed when exiting the
	// game. If zero, the ENDOOM screen is not displayed.
	CONFIG_VARIABLE_INT(show_endoom),

	// @game doom strife
	//
	// If non-zero, a disk activity indicator is displayed when data is read
	// from disk. If zero, the disk activity indicator is not displayed.
	CONFIG_VARIABLE_INT(show_diskicon),

	// If non-zero, save screenshots in PNG format. If zero, screenshots are
	// saved in PCX format, as Vanilla Doom does.
	CONFIG_VARIABLE_INT(png_screenshots),

	// Vertical mouse acceleration factor. When the speed of mouse movement
	// exceeds the threshold value (mouse_threshold), the speed is
	// multiplied by this value.
	CONFIG_VARIABLE_FLOAT(mouse_acceleration_y),

	// Vertical mouse acceleration threshold. When the speed of mouse movement
	// exceeds this threshold value, the speed is multiplied by an
	// acceleration factor (mouse_acceleration).
	CONFIG_VARIABLE_INT(mouse_threshold_y),

	// Sound output sample rate, in Hz. Typical values to use are
	// 11025, 22050, 44100 and 48000.
	CONFIG_VARIABLE_INT(snd_samplerate),

	// Maximum number of bytes to allocate for caching converted sound
	// effects in memory. If set to zero, there is no limit applied.
	CONFIG_VARIABLE_INT(snd_cachesize),

	// Maximum size of the output sound buffer size in milliseconds.
	// Sound output is generated periodically in slices. Higher values
	// might be more efficient but will introduce latency to the
	// sound output. The default is 28ms (one slice per tic with the
	// 35fps timer).
	CONFIG_VARIABLE_INT(snd_maxslicetime_ms),

	// If non-zero, sound effects will have their pitch varied up or
	// down by a random amount during play. If zero, sound effects
	// play back at their default pitch.
	CONFIG_VARIABLE_INT(snd_pitchshift),

	// External command to invoke to perform MIDI playback. If set to
	// the empty string, SDL_mixer's internal MIDI playback is used.
	// This only has any effect when snd_musicdevice is set to General
	// MIDI output.
	CONFIG_VARIABLE_STRING(snd_musiccmd),

	// Value to set for the DMXOPTION environment variable. If this contains
	// "-opl3", output for an OPL3 chip is generated when in OPL MIDI
	// playback mode.
	CONFIG_VARIABLE_STRING(snd_dmxoption),

	// The I/O port to use to access the OPL chip. Only relevant when
	// using native OPL music playback.
	CONFIG_VARIABLE_INT_HEX(opl_io_port),

	// Controls whether libsamplerate support is used for performing
	// sample rate conversions of sound effects. Support for this
	// must be compiled into the program.
	//
	// If zero, libsamplerate support is disabled. If non-zero,
	// libsamplerate is enabled. Increasing values roughly correspond
	// to higher quality conversion; the higher the quality, the
	// slower the conversion process. Linear conversion = 1;
	// Zero order hold = 2; Fast Sinc filter = 3; Medium quality
	// Sinc filter = 4; High quality Sinc filter = 5.
	CONFIG_VARIABLE_INT(use_libsamplerate),

	// Scaling factor used by libsamplerate. This is used when converting
	// sounds internally back into integer form; normally it should not
	// be necessary to change it from the default value. The only time
	// it might be needed is if a PWAD file is loaded that contains very
	// loud sounds, in which case the conversion may cause sound clipping
	// and the scale factor should be reduced. The lower the value, the
	// quieter the sound effects become, so it should be set as high as is
	// possible without clipping occurring.

	CONFIG_VARIABLE_FLOAT(libsamplerate_scale),

	// Full path to a directory in which WAD files and dehacked patches
	// can be placed to be automatically loaded on startup. A subdirectory
	// of this directory matching the IWAD name is checked to find the
	// files to load.

	CONFIG_VARIABLE_STRING(autoload_path),

	// Full path to a directory containing configuration files for
	// substitute music packs. These packs contain high quality renderings
	// of game music to be played instead of using the system's built-in
	// MIDI playback.
	CONFIG_VARIABLE_STRING(music_pack_path),

	// Full path to a Timidity configuration file to use for MIDI
	// playback. The file will be evaluated from the directory where
	// it is evaluated, so there is no need to add "dir" commands
	// into it.
	CONFIG_VARIABLE_STRING(timidity_cfg_path),

	// Path to GUS patch files to use when operating in GUS emulation
	// mode.
	CONFIG_VARIABLE_STRING(gus_patch_path),

	// Number of kilobytes of RAM to use in GUS emulation mode. Valid
	// values are 256, 512, 768 or 1024.
	CONFIG_VARIABLE_INT(gus_ram_kb),

	// @game doom strife
	//
	// If non-zero, the Vanilla savegame limit is enforced; if the
	// savegame exceeds 180224 bytes in size, the game will exit with
	// an error. If this has a value of zero, there is no limit to
	// the size of savegames.
	CONFIG_VARIABLE_INT(vanilla_savegame_limit),

	// @game doom strife
	//
	// If non-zero, the Vanilla demo size limit is enforced; the game
	// exits with an error when a demo exceeds the demo size limit
	// (128KiB by default). If this has a value of zero, there is no
	// limit to the size of demos.
	CONFIG_VARIABLE_INT(vanilla_demo_limit),

	// If non-zero, the game behaves like Vanilla Doom, always assuming
	// an American keyboard mapping. If this has a value of zero, the
	// native keyboard mapping of the keyboard is used.
	CONFIG_VARIABLE_INT(vanilla_keyboard_mapping),

	// Name to use in network games for identification. This is only
	// used on the "waiting" screen while waiting for the game to start.
	CONFIG_VARIABLE_STRING(player_name),

	// If this is non-zero, the mouse will be "grabbed" when running
	// in windowed mode so that it can be used as an input device.
	// When running full screen, this has no effect.
	CONFIG_VARIABLE_INT(grabmouse),

	// If non-zero, all vertical mouse movement is ignored. This
	// emulates the behavior of the "novert" tool available under DOS
	// that performs the same function.
	CONFIG_VARIABLE_INT(novert),

	// Mouse acceleration factor. When the speed of mouse movement
	// exceeds the threshold value (mouse_threshold), the speed is
	// multiplied by this value.
	CONFIG_VARIABLE_FLOAT(mouse_acceleration),

	// Mouse acceleration threshold. When the speed of mouse movement
	// exceeds this threshold value, the speed is multiplied by an
	// acceleration factor (mouse_acceleration).
	CONFIG_VARIABLE_INT(mouse_threshold),

	// Mouse button to strafe left.
	CONFIG_VARIABLE_INT(mouseb_strafeleft),

	// Mouse button to strafe right.
	CONFIG_VARIABLE_INT(mouseb_straferight),

	// Mouse button to "use" an object, eg. a door or switch.
	CONFIG_VARIABLE_INT(mouseb_use),

	// Mouse button to move backwards.
	CONFIG_VARIABLE_INT(mouseb_backward),

	// Mouse button to cycle to the previous weapon.
	CONFIG_VARIABLE_INT(mouseb_prevweapon),

	// Mouse button to cycle to the next weapon.
	CONFIG_VARIABLE_INT(mouseb_nextweapon),

	// Mouse button to move to the left in the inventory.
	CONFIG_VARIABLE_INT(mouseb_invleft),

	// Mouse button to move to the right in the inventory.
	CONFIG_VARIABLE_INT(mouseb_invright),

	// If non-zero, double-clicking a mouse button acts like pressing
	// the "use" key to use an object in-game, eg. a door or switch.
	CONFIG_VARIABLE_INT(dclick_use),

	// SDL GUID string indicating the joystick to use. An empty string
	// indicates that no joystick is configured.
	CONFIG_VARIABLE_STRING(joystick_guid),

	// Index of SDL joystick to use; this is only used in the case where
	// multiple identical joystick devices are connected which have the
	// same GUID, to distinguish between devices.
	CONFIG_VARIABLE_INT(joystick_index),

	// Joystick axis to use to for horizontal (X) movement.
	CONFIG_VARIABLE_INT(joystick_x_axis),

	// If non-zero, movement on the horizontal joystick axis is inverted.
	CONFIG_VARIABLE_INT(joystick_x_invert),

	// Joystick axis to use to for vertical (Y) movement.
	CONFIG_VARIABLE_INT(joystick_y_axis),

	// If non-zero, movement on the vertical joystick axis is inverted.
	CONFIG_VARIABLE_INT(joystick_y_invert),

	// Joystick axis to use to for strafing movement.
	CONFIG_VARIABLE_INT(joystick_strafe_axis),

	// If non-zero, movement on the joystick axis used for strafing
	// is inverted.
	CONFIG_VARIABLE_INT(joystick_strafe_invert),

	// Joystick axis to use to for looking up and down.
	CONFIG_VARIABLE_INT(joystick_look_axis),

	// If non-zero, movement on the joystick axis used for looking
	// is inverted.
	CONFIG_VARIABLE_INT(joystick_look_invert),

	// The physical joystick button that corresponds to joystick
	// virtual button #0.
	CONFIG_VARIABLE_INT(joystick_physical_button0),

	// The physical joystick button that corresponds to joystick
	// virtual button #1.
	CONFIG_VARIABLE_INT(joystick_physical_button1),

	// The physical joystick button that corresponds to joystick
	// virtual button #2.
	CONFIG_VARIABLE_INT(joystick_physical_button2),

	// The physical joystick button that corresponds to joystick
	// virtual button #3.
	CONFIG_VARIABLE_INT(joystick_physical_button3),

	// The physical joystick button that corresponds to joystick
	// virtual button #4.
	CONFIG_VARIABLE_INT(joystick_physical_button4),

	// The physical joystick button that corresponds to joystick
	// virtual button #5.
	CONFIG_VARIABLE_INT(joystick_physical_button5),

	// The physical joystick button that corresponds to joystick
	// virtual button #6.
	CONFIG_VARIABLE_INT(joystick_physical_button6),

	// The physical joystick button that corresponds to joystick
	// virtual button #7.
	CONFIG_VARIABLE_INT(joystick_physical_button7),

	// The physical joystick button that corresponds to joystick
	// virtual button #8.
	CONFIG_VARIABLE_INT(joystick_physical_button8),

	// The physical joystick button that corresponds to joystick
	// virtual button #9.
	CONFIG_VARIABLE_INT(joystick_physical_button9),

	// The physical joystick button that corresponds to joystick
	// virtual button #10.
	CONFIG_VARIABLE_INT(joystick_physical_button10),

	// Joystick virtual button to make the player strafe left.
	CONFIG_VARIABLE_INT(joyb_strafeleft),

	// Joystick virtual button to make the player strafe right.
	CONFIG_VARIABLE_INT(joyb_straferight),

	// Joystick virtual button to activate the menu.
	CONFIG_VARIABLE_INT(joyb_menu_activate),

	// Joystick virtual button to toggle the automap.
	CONFIG_VARIABLE_INT(joyb_toggle_automap),

	// Joystick virtual button that cycles to the previous weapon.
	CONFIG_VARIABLE_INT(joyb_prevweapon),

	// Joystick virtual button that cycles to the next weapon.
	CONFIG_VARIABLE_INT(joyb_nextweapon),

	// Key to pause or unpause the game.
	CONFIG_VARIABLE_KEY(key_pause),

	// Key that activates the menu when pressed.
	CONFIG_VARIABLE_KEY(key_menu_activate),

	// Key that moves the cursor up on the menu.
	CONFIG_VARIABLE_KEY(key_menu_up),

	// Key that moves the cursor down on the menu.
	CONFIG_VARIABLE_KEY(key_menu_down),

	// Key that moves the currently selected slider on the menu left.
	CONFIG_VARIABLE_KEY(key_menu_left),

	// Key that moves the currently selected slider on the menu right.
	CONFIG_VARIABLE_KEY(key_menu_right),

	// Key to go back to the previous menu.
	CONFIG_VARIABLE_KEY(key_menu_back),

	// Key to activate the currently selected menu item.
	CONFIG_VARIABLE_KEY(key_menu_forward),

	// Key to answer 'yes' to a question in the menu.
	CONFIG_VARIABLE_KEY(key_menu_confirm),

	// Key to answer 'no' to a question in the menu.
	CONFIG_VARIABLE_KEY(key_menu_abort),

	// Keyboard shortcut to bring up the help screen.
	CONFIG_VARIABLE_KEY(key_menu_help),

	// Keyboard shortcut to bring up the save game menu.
	CONFIG_VARIABLE_KEY(key_menu_save),

	// Keyboard shortcut to bring up the load game menu.
	CONFIG_VARIABLE_KEY(key_menu_load),

	// Keyboard shortcut to bring up the sound volume menu.
	CONFIG_VARIABLE_KEY(key_menu_volume),

	// Keyboard shortcut to toggle the detail level.
	CONFIG_VARIABLE_KEY(key_menu_detail),

	// Keyboard shortcut to quicksave the current game.
	CONFIG_VARIABLE_KEY(key_menu_qsave),

	// Keyboard shortcut to end the game.
	CONFIG_VARIABLE_KEY(key_menu_endgame),

	// Keyboard shortcut to toggle heads-up messages.
	CONFIG_VARIABLE_KEY(key_menu_messages),

	// Keyboard shortcut to load the last quicksave.
	CONFIG_VARIABLE_KEY(key_menu_qload),

	// Keyboard shortcut to quit the game.
	CONFIG_VARIABLE_KEY(key_menu_quit),

	// Keyboard shortcut to toggle the gamma correction level.
	CONFIG_VARIABLE_KEY(key_menu_gamma),

	// Keyboard shortcut to switch view in multiplayer.
	CONFIG_VARIABLE_KEY(key_spy),

	// Keyboard shortcut to go to next level.
	CONFIG_VARIABLE_KEY(key_menu_nextlevel),

	// Keyboard shortcut to restart current level or demo.
	CONFIG_VARIABLE_KEY(key_menu_reloadlevel),

	// Keyboard shortcut to increase the screen size.
	CONFIG_VARIABLE_KEY(key_menu_incscreen),

	// Keyboard shortcut to decrease the screen size.
	CONFIG_VARIABLE_KEY(key_menu_decscreen),

	// Keyboard shortcut to save a screenshot.
	CONFIG_VARIABLE_KEY(key_menu_screenshot),

	// Keyboard shortcut to save a clean screenshot.
	CONFIG_VARIABLE_KEY(key_menu_cleanscreenshot),

	// Key to delete a savegame.
	CONFIG_VARIABLE_KEY(key_menu_del),

	// Key to toggle the map view.
	CONFIG_VARIABLE_KEY(key_map_toggle),

	// Key to pan north when in the map view.
	CONFIG_VARIABLE_KEY(key_map_north),

	// Key to pan south when in the map view.
	CONFIG_VARIABLE_KEY(key_map_south),

	// Key to pan east when in the map view.
	CONFIG_VARIABLE_KEY(key_map_east),

	// Key to pan west when in the map view.
	CONFIG_VARIABLE_KEY(key_map_west),

	// Key to zoom in when in the map view.
	CONFIG_VARIABLE_KEY(key_map_zoomin),

	// Key to zoom out when in the map view.
	CONFIG_VARIABLE_KEY(key_map_zoomout),

	// Key to zoom out the maximum amount when in the map view.
	CONFIG_VARIABLE_KEY(key_map_maxzoom),

	// Key to toggle follow mode when in the map view.
	CONFIG_VARIABLE_KEY(key_map_follow),

	// Key to toggle the grid display when in the map view.
	CONFIG_VARIABLE_KEY(key_map_grid),

	// Key to set a mark when in the map view.
	CONFIG_VARIABLE_KEY(key_map_mark),

	// Key to clear all marks when in the map view.
	CONFIG_VARIABLE_KEY(key_map_clearmark),

	// Key to toggle the overlay mode when in the map view.
	CONFIG_VARIABLE_KEY(key_map_overlay),

	// Key to toggle the rotate mode when in the map view.
	CONFIG_VARIABLE_KEY(key_map_rotate),

	// Key to select weapon 1.
	CONFIG_VARIABLE_KEY(key_weapon1),

	// Key to select weapon 2.
	CONFIG_VARIABLE_KEY(key_weapon2),

	// Key to select weapon 3.
	CONFIG_VARIABLE_KEY(key_weapon3),

	// Key to select weapon 4.
	CONFIG_VARIABLE_KEY(key_weapon4),

	// Key to select weapon 5.
	CONFIG_VARIABLE_KEY(key_weapon5),

	// Key to select weapon 6.
	CONFIG_VARIABLE_KEY(key_weapon6),

	// Key to select weapon 7.
	CONFIG_VARIABLE_KEY(key_weapon7),

	// Key to select weapon 8.
	CONFIG_VARIABLE_KEY(key_weapon8),

	// Key to cycle to the previous weapon.
	CONFIG_VARIABLE_KEY(key_prevweapon),

	// Key to cycle to the next weapon.
	CONFIG_VARIABLE_KEY(key_nextweapon),

	// Key to use "quartz flask" artifact.
	CONFIG_VARIABLE_KEY(key_arti_quartz),

	// Key to use "mystic urn" artifact.
	CONFIG_VARIABLE_KEY(key_arti_urn),

	// Key to use "timebomb of the ancients" artifact.
	CONFIG_VARIABLE_KEY(key_arti_bomb),

	// Key to use "tome of power" artifact.
	CONFIG_VARIABLE_KEY(key_arti_tome),

	// Key to use "ring of invincibility" artifact.
	CONFIG_VARIABLE_KEY(key_arti_ring),

	// Key to use "chaos device" artifact.
	CONFIG_VARIABLE_KEY(key_arti_chaosdevice),

	// Key to use "shadowsphere" artifact.
	CONFIG_VARIABLE_KEY(key_arti_shadowsphere),

	// Key to use "wings of wrath" artifact.
	CONFIG_VARIABLE_KEY(key_arti_wings),

	// Key to use "torch" artifact.
	CONFIG_VARIABLE_KEY(key_arti_torch),

	// Key to use "morph ovum" artifact.
	CONFIG_VARIABLE_KEY(key_arti_morph),

	// Key to use one of each artifact.
	CONFIG_VARIABLE_KEY(key_arti_all),

	// Key to use "quartz flask" artifact.
	CONFIG_VARIABLE_KEY(key_arti_health),

	// Key to use "flechette" artifact.
	CONFIG_VARIABLE_KEY(key_arti_poisonbag),

	// Key to use "disc of repulsion" artifact.
	CONFIG_VARIABLE_KEY(key_arti_blastradius),

	// Key to use "chaos device" artifact.
	CONFIG_VARIABLE_KEY(key_arti_teleport),

	// Key to use "banishment device" artifact.
	CONFIG_VARIABLE_KEY(key_arti_teleportother),

	// Key to use "porkalator" artifact.
	CONFIG_VARIABLE_KEY(key_arti_egg),

	// Key to use "icon of the defender" artifact.
	CONFIG_VARIABLE_KEY(key_arti_invulnerability),

	// Key to re-display last message.
	CONFIG_VARIABLE_KEY(key_message_refresh),

	// Key to quit the game when recording a demo.
	CONFIG_VARIABLE_KEY(key_demo_quit),

	// Key to send a message during multiplayer games.
	CONFIG_VARIABLE_KEY(key_multi_msg),

	// Key to send a message to player 1 during multiplayer games.
	CONFIG_VARIABLE_KEY(key_multi_msgplayer1),

	// Key to send a message to player 2 during multiplayer games.
	CONFIG_VARIABLE_KEY(key_multi_msgplayer2),

	// Key to send a message to player 3 during multiplayer games.
	CONFIG_VARIABLE_KEY(key_multi_msgplayer3),

	// Key to send a message to player 4 during multiplayer games.
	CONFIG_VARIABLE_KEY(key_multi_msgplayer4),

	// @game hexen strife
	//
	// Key to send a message to player 5 during multiplayer games.
	CONFIG_VARIABLE_KEY(key_multi_msgplayer5),

	// @game hexen strife
	//
	// Key to send a message to player 6 during multiplayer games.
	CONFIG_VARIABLE_KEY(key_multi_msgplayer6),

	// @game hexen strife
	//
	// Key to send a message to player 7 during multiplayer games.
	CONFIG_VARIABLE_KEY(key_multi_msgplayer7),

	// @game hexen strife
	//
	// Key to send a message to player 8 during multiplayer games.
	CONFIG_VARIABLE_KEY(key_multi_msgplayer8),

	// @game doom
	// Quick 180° reverse.
	CONFIG_VARIABLE_KEY(key_reverse),

	// @game doom
	// Toggle always run.
	CONFIG_VARIABLE_KEY(key_toggleautorun),

	// @game doom heretic
	// Toggle vertical mouse movement.
	CONFIG_VARIABLE_KEY(key_togglenovert),

	// @game doom
	// Invert vertical mouse movement.
	CONFIG_VARIABLE_KEY(mouse_y_invert),

	// Automap overlay mode.
	CONFIG_VARIABLE_INT(crispy_automapoverlay),

	// Automap rotate mode.
	CONFIG_VARIABLE_INT(crispy_automaprotate),

	// Show additional level statistics.
	CONFIG_VARIABLE_INT(crispy_automapstats),

	// Variable player view bob.
	CONFIG_VARIABLE_INT(crispy_bobfactor),

	// "Use" button timer.
	CONFIG_VARIABLE_INT(crispy_btusetimer),

	// Apply brightmaps to select textures and sprites.
	CONFIG_VARIABLE_INT(crispy_brightmaps),

	// Center Weapon when Firing.
	CONFIG_VARIABLE_INT(crispy_centerweapon),

	// Enable Colored Blood.
	CONFIG_VARIABLE_INT(crispy_coloredblood),

	// Show colored numbers in the status bar.
	CONFIG_VARIABLE_INT(crispy_coloredhud),

	// Draw a crosshair.
	CONFIG_VARIABLE_INT(crispy_crosshair),

	// Crosshair Color indicates Health.
	CONFIG_VARIABLE_INT(crispy_crosshairhealth),

	// Highlight Crosshair on target.
	CONFIG_VARIABLE_INT(crispy_crosshairtarget),

	// Crosshair type.
	CONFIG_VARIABLE_INT(crispy_crosshairtype),

	// Show a progress bar when playing back a demo.
	CONFIG_VARIABLE_INT(crispy_demobar),

	// Show a timer when recording or playing pack demos.
	CONFIG_VARIABLE_INT(crispy_demotimer),

	// Timer direction whan playing back a demo.
	CONFIG_VARIABLE_INT(crispy_demotimerdir),

	// Extended Automap.
	CONFIG_VARIABLE_INT(crispy_extautomap),

	// Extended Savegames.
	CONFIG_VARIABLE_INT(crispy_extsaveg),

	// Enable Mirrored Corpses.
	CONFIG_VARIABLE_INT(crispy_flipcorpses),

	// Enable vertical aiming.
	CONFIG_VARIABLE_INT(crispy_freeaim),

	// Enable looking up and down.
	CONFIG_VARIABLE_INT(crispy_freelook),

	// High Resolution Rendering.
	CONFIG_VARIABLE_INT(crispy_hires),

	// Enable jumping.
	CONFIG_VARIABLE_INT(crispy_jump),

	// Show level time.
	CONFIG_VARIABLE_INT(crispy_leveltime),

	// Use the mouse to look up and down.
	CONFIG_VARIABLE_INT(crispy_mouselook),

	// Negative player health.
	CONFIG_VARIABLE_INT(crispy_neghealth),

	// Players may walk over and under shootable things.
	CONFIG_VARIABLE_INT(crispy_overunder),

	// Enable weapon recoil pitch.
	CONFIG_VARIABLE_INT(crispy_pitch),

	// Show player coordinates.
	CONFIG_VARIABLE_INT(crispy_playercoords),

	// Enable weapon recoil thrust.
	CONFIG_VARIABLE_INT(crispy_recoil),

	// Show a centered message and play a sound when a secret is found.
	CONFIG_VARIABLE_INT(crispy_secretmessage),

	// Smooth Diminishing Lighting.
	CONFIG_VARIABLE_INT(crispy_smoothlight),

	// Smooth Lines on Automap.
	CONFIG_VARIABLE_INT(crispy_smoothmap),

	// Smooth Scaling.
	CONFIG_VARIABLE_INT(crispy_smoothscaling),

	// Misc. sound fixes.
	CONFIG_VARIABLE_INT(crispy_soundfix),

	// Play sounds in full length.
	CONFIG_VARIABLE_INT(crispy_soundfull),

	// Mono sound effects.
	CONFIG_VARIABLE_INT(crispy_soundmono),

	// Enable translucency.
	CONFIG_VARIABLE_INT(crispy_translucency),

#ifdef CRISPY_TRUECOLOR
	//!
	// Enable true-color rendering (experimental).
	CONFIG_VARIABLE_INT(crispy_truecolor),
#endif

	// Uncapped Framerate.
	CONFIG_VARIABLE_INT(crispy_uncapped),

	// Enable VSync.
	CONFIG_VARIABLE_INT(crispy_vsync),

	// Squat down weapon on impact.
	CONFIG_VARIABLE_INT(crispy_weaponsquat),

	// Widescreen rendering.
	CONFIG_VARIABLE_INT(crispy_widescreen),
};
