/**********************************************************************************************************************************************\
	Copyright(C) 2005-2014 Simon Howard

	This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License
	as published by the Free Software Foundation; either version 2 of the License, or (at your option) any later version.

	This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
\**********************************************************************************************************************************************/
#pragma once

#include "../../build/config.h"

#include "../../derma/common.h"

#include "../../textscreen/txt_widget.h"

#include "../m_config.h"
#include "../m_misc.h"

#include "mode.h"

#define WINDOW_HELP_URL "https://www.chocolate-doom.org/setup-display"

extern void RestartTextscreen();

struct window_size_t
{
	int w;
	int h;
};

// List of aspect ratio-uncorrected window sizes:
static window_size_t window_sizes_unscaled[] =
{
// { 320, 200 }, // hires
	{ 640, 400 },
	{ 960, 600 },
	{ 1280, 800 },
	{ 1600, 1000 },
	{ 1920, 1200 }, // hires * 3
	{ 2560, 1600 }, // hires * 4
	{ 3200, 2000 }, // hires * 5
	{ 0, 0},
};

// List of aspect ratio-corrected window sizes:
static window_size_t window_sizes_scaled[] =
{
// { 320, 240 }, // hires
// { 512, 400 }, // hires
	{ 640, 480 },
	{ 800, 600 },
	{ 960, 720 },
	{ 1024, 800 },
	{ 1280, 960 },
	{ 1600, 1200 },
	{ 1920, 1440 },
	{ 2560, 1920 }, // hires * 4
	{ 3200, 2400 }, // hires * 5
	{ 0, 0},
};

::std::string video_driver = "";
::std::string window_position = "";
static int aspect_ratio_correct = 1;
static int integer_scaling = 0;
static int vga_porch_flash = 0;
static int force_software_renderer = 0;
static int fullscreen = 1;
static int fullscreen_width = 0, fullscreen_height = 0;
static int window_width = 800, window_height = 600;
static int startup_delay = 1000;
static int max_scaling_buffer_pixels = 16000000;
static int usegamma = 0;

int graphical_startup = 0;
int show_endoom = 0;
int show_diskicon = 1;
int png_screenshots = 1;

static int system_video_env_set;

// Set the SDL_VIDEODRIVER environment variable
void SetDisplayDriver()
{
	static TimeType first_time = 1;

	if (first_time)
	{
		system_video_env_set = getenv("SDL_VIDEODRIVER") != NULL;

		first_time = 0;
	}

	// Don't override the command line environment, if it has been set.
	if (system_video_env_set)
	{
		return;
	}

	// Use the value from the configuration file, if it has been set.
	if (video_driver.compare(""))
	{
		::std::string env_string("SDL_VIDEODRIVER=" + video_driver);
		putenv(env_string.c_str());
	}
}

static void WindowSizeSelected(txt::Widget* widget, window_size_t* size)
{
	window_width = size->w;
	window_height = size->h;
}

static txt::RadioButton* SizeSelectButton(window_size_t* size)
{
	char buf[15];
	txt::RadioButton* result;

	M_snprintf(buf, sizeof(buf), "%ix%i", size->w, size->h);
	result = txt::NewRadioButton(buf, &window_width, size->w);
	txt::SignalConnect(result, "selected", WindowSizeSelected, size);

	return result;
}

static void GenerateSizesTable(txt::Widget* widget, txt::Table* sizes_table)
{
	window_size_t* sizes;
	bool have_size;
	int i;

	// Pick which window sizes list to use
	if (aspect_ratio_correct == 1)
	{
		sizes = window_sizes_scaled;
	}
	else
	{
		sizes = window_sizes_unscaled;
	}

	// Build the table
	txt::ClearTable(sizes_table);
	txt::SetColumnWidths(sizes_table, 14, 14, 14);

	txt::AddWidget(sizes_table, txt::NewSeparator("Window size"));

	have_size = false;

	for (i = 0; sizes[i].w != 0; ++i)
	{
		txt::AddWidget(sizes_table, SizeSelectButton(&sizes[i]));
		have_size = have_size || window_width == sizes[i].w;
	}

	// Windows can be any arbitrary size. We key off the width of the
	// window in pixels. If the current size is not in the list of
	// standard (integer multiply) sizes, create a special button to
	// mean "the current window size".
	if (!have_size)
	{
		static window_size_t current_size;
		current_size.w = window_width;
		current_size.h = window_height;
		txt::AddWidget(sizes_table, SizeSelectButton(&current_size));
	}
}

static void AdvancedDisplayConfig(txt::Widget widget, txt::Table* sizes_table)
{
	txt::Window* window;
	txt::CheckBox* ar_checkbox;

	window = txt::NewWindow("Advanced display options");

	txt::SetWindowHelpURL(window, WINDOW_HELP_URL);

	txt::SetColumnWidths(window, 40);

	txt::AddWidgets(window, ar_checkbox = txt::NewCheckBox("Force correct aspect ratio", &aspect_ratio_correct),
		(gamemission == GameMission::heretic || gamemission == GameMission::hexen || gamemission == GameMission::strife,
		txt::NewCheckBox("Graphical startup", &graphical_startup)),
		(gamemission == GameMission::doom || gamemission == GameMission::heretic || gamemission == GameMission::strife,
		txt::NewCheckBox("Show ENDOOM screen on exit", &show_endoom)),
#ifdef HAVE_LIBPNG
		txt::NewCheckBox("Save screenshots in PNG format", &png_screenshots),
#endif
		NULL);

	txt::SignalConnect(ar_checkbox, "changed", GenerateSizesTable, sizes_table);
}

void ConfigDisplay(txt::UNCAST_ARG(widget), void* user_data)
{
	txt::Window* window;
	txt::Table* sizes_table;
	txt::WindowAction* advanced_button;

	// Open the window
	window = txt::NewWindow("Display Configuration");
	txt::SetWindowHelpURL(window, WINDOW_HELP_URL);

	// Build window:
	txt::AddWidgets(window,
		txt::NewCheckBox("Full screen", &fullscreen),
		txt::NewConditional(&fullscreen, 0, sizes_table = txt::NewTable(3)), NULL);

	txt::SetColumnWidths(window, 42);

	// The window is set at a fixed vertical position. This keeps the top of the window stationary when switching between
	// fullscreen and windowed mode (which causes the window's height to change).
	txt::SetWindowPosition(window, txt::HORIZ_CENTER, txt::VERT_TOP, txt::SCREEN_W / 2, 6);

	GenerateSizesTable(NULL, sizes_table);

	// Button to open "advanced" window.
	// Need to pass a pointer to the window sizes table, as some of the options in there trigger a rebuild of it.
	advanced_button = txt::NewWindowAction('a', "Advanced");
	txt::SetWindowAction(window, txt::AlignHorizontal::center, advanced_button);
	txt::SignalConnect(advanced_button, "pressed", AdvancedDisplayConfig, sizes_table);
}

void BindDisplayVariables()
{
	M_BindVariable<int>("aspect_ratio_correct",		&aspect_ratio_correct);
	M_BindVariable<int>("integer_scaling",			&integer_scaling);
	M_BindVariable<int>("fullscreen",					&fullscreen);
	M_BindVariable<int>("fullscreen_width",			&fullscreen_width);
	M_BindVariable<int>("fullscreen_height",			&fullscreen_height);
	M_BindVariable<int>("window_width",				&window_width);
	M_BindVariable<int>("window_height",				&window_height);
	M_BindVariable<int>("startup_delay",				&startup_delay);
	M_BindStringVariable("video_driver",			&video_driver);
	M_BindStringVariable("window_position",			&window_position);
	M_BindVariable<int>("usegamma",					&usegamma);
	M_BindVariable<int>("png_screenshots",			&png_screenshots);
	M_BindVariable<int>("vga_porch_flash",			&vga_porch_flash);
	M_BindVariable<int>("force_software_renderer",	&force_software_renderer);
	M_BindVariable<int>("max_scaling_buffer_pixels",	&max_scaling_buffer_pixels);

	if (gamemission == doom || gamemission == heretic || gamemission == strife)
	{
		M_BindVariable<int>("show_endoom", &show_endoom);
	}

	if (gamemission == doom || gamemission == strife)
	{
		M_BindVariable<int>("show_diskicon", &show_diskicon);
	}

	if (gamemission == heretic || gamemission == hexen || gamemission == strife)
	{
		M_BindVariable<int>("graphical_startup", &graphical_startup);
	}
}
