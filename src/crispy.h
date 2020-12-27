/**********************************************************************************************************************************************\
	Copyright(C) 1993-1996 Id Software, Inc.
	Copyright(C) 2005-2014 Simon Howard
	Copyright(C) 2014-2017 Fabian Greffrath

	This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License
	as published by the Free Software Foundation; either version 2 of the License, or (at your option) any later version.

	This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

	DESCRIPTION:
		Crispy Doom specific variables.
\**********************************************************************************************************************************************/
#pragma once

#include "../derma/common.h"

#include "doomtype.h"

// TODO ENFORCE DO NOT USE THESE "HANDY" MACROS, TEMP MEASURE CALL LAMBDA TO STD ALGORITHMS
#ifndef MIN
	//#define MIN(a,b) (((a)<(b))?(a):(b))
#	define MIN(a,b) [](auto x, auto y){return std::min(x, y);}(a, b)
#endif
#ifndef MAX
	//#define MAX(a,b) (((a)>(b))?(a):(b))
	#define MAX(a,b) [](auto x, auto y){return std::max(x, y);}(a, b)
#endif
#ifndef BETWEEN
	//#define BETWEEN(l,u,x) (((l)>(x))?(l):((x)>(u))?(u):(x))
	#define BETWEEN(l,u,x) [](auto v, auto lo, auto hi){return std::clamp(v, lo, hi);}(x, l, u)
#endif

struct crispy_t
{
	// [crispy] "crispness" config variables
	int automapoverlay{0};
	int automaprotate{0};
	int automapstats{0};
	int bobfactor{0};
	int brightmaps{0};
	int btusetimer{0};
	int centerweapon{0};
	int coloredblood{0};
	int coloredhud{0};
	int crosshair{0};
	int crosshairhealth{0};
	int crosshairtarget{0};
	int crosshairtype{0};
	int demotimer{0};
	int demotimerdir{0};
	int demobar{0};
	int extautomap{1};
	int extsaveg{1};
	int flipcorpses{0};
	int freeaim{0};
	int freelook{0};
	int hires{1};
	int jump{0};
	int leveltime{0};
	int mouselook{0};
	int neghealth{0};
	int overunder{0};
	int pitch{0};
	int playercoords{0};
	int recoil{0};
	int secretmessage{0};
	int smoothlight{0};
	int smoothmap{0};
	int smoothscaling{1};
	int soundfix{1};
	int soundfull{0};
	int soundmono{0};
	int translucency{0};
#ifdef CRISPY_TRUECOLOR
	int truecolor{1};
#endif
	int uncapped{0};
	int vsync{1};
	int weaponsquat{0};
	int widescreen{0};

	// [crispy] in-game switches and variables
	int screenshotmsg{0};
	int cleanscreenshot{0};
	int demowarp{0};
	int fps{0};

	bool flashinghom{false};
	bool fliplevels{false};
	bool flipweapons{false};
	bool haved1e5{false};
	bool havee1m10{false};
	bool havemap33{false};
	bool havessg{false};
	bool pistolstart{false};
	bool singleplayer{true};
	bool stretchsky{false};

	char* havenerve{nullptr};
	char* havemaster{nullptr};

	const char* sdlversion{nullptr};
	const char* platform{nullptr};

	void (*post_rendering_hook) ();
};

// [crispy] "regular" config variables
static crispy_t crispy_s{};
crispy_t *const crispy = &crispy_s;

// [crispy] "critical" config variables
static const crispy_t critical_s{0};
const crispy_t* critical = &critical_s;

// [crispy] update the "singleplayer" variable and the "critical" struct
void CheckCrispySingleplayer(bool singleplayer)
{
	if ((crispy->singleplayer = singleplayer))
	{
		critical = &crispy_s;
	}
	else
	{
		critical = &critical_s;
	}
}

enum
{
	REINIT_FRAMEBUFFERS		= 1,
	REINIT_RENDERER			= 2,
	REINIT_TEXTURES			= 4,
	REINIT_ASPECTRATIO		= 8
};

enum
{
	BOBFACTOR_FULL,
	BOBFACTOR_75,
	BOBFACTOR_OFF,
	NUM_BOBFACTORS
};

enum
{
	BRIGHTMAPS_OFF,
	BRIGHTMAPS_TEXTURES,
	BRIGHTMAPS_SPRITES,
	BRIGHTMAPS_BOTH,
	NUM_BRIGHTMAPS
};

enum
{
	CENTERWEAPON_OFF,
	CENTERWEAPON_CENTER,
	CENTERWEAPON_BOB,
	NUM_CENTERWEAPON
};

enum
{
	COLOREDHUD_OFF,
	COLOREDHUD_BAR,
	COLOREDHUD_TEXT,
	COLOREDHUD_BOTH,
	NUM_COLOREDHUD
};

enum
{
	CROSSHAIR_OFF,
	CROSSHAIR_STATIC,
	CROSSHAIR_PROJECTED,
	NUM_CROSSHAIRS,
	CROSSHAIR_INTERCEPT = 0x10
};

enum
{
	DEMOTIMER_OFF,
	DEMOTIMER_RECORD,
	DEMOTIMER_PLAYBACK,
	DEMOTIMER_BOTH,
	NUM_DEMOTIMERS
};

enum
{
	FREEAIM_AUTO,
	FREEAIM_DIRECT,
	FREEAIM_BOTH,
	NUM_FREEAIMS
};

enum
{
	FREELOOK_OFF,
	FREELOOK_SPRING,
	FREELOOK_LOCK,
	NUM_FREELOOKS
};

enum
{
	JUMP_OFF,
	JUMP_LOW,
	JUMP_HIGH,
	NUM_JUMPS
};

enum
{
	TRANSLUCENCY_OFF,
	TRANSLUCENCY_MISSILE,
	TRANSLUCENCY_ITEM,
	TRANSLUCENCY_BOTH,
	NUM_TRANSLUCENCY
};

enum
{
	SECRETMESSAGE_OFF,
	SECRETMESSAGE_ON,
	SECRETMESSAGE_COUNT,
	NUM_SECRETMESSAGE
};

enum
{
	WIDGETS_OFF,
	WIDGETS_AUTOMAP,
	WIDGETS_ALWAYS,
	NUM_WIDGETS
};

enum
{
	RATIO_4_3,
	RATIO_MATCH_SCREEN,
	RATIO_16_10,
	RATIO_16_9,
	RATIO_21_9,
	NUM_RATIOS
};
