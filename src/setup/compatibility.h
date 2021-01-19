/**********************************************************************************************************************************************\
	Copyright(C) 2005-2014 Simon Howard

	This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License
	as published by the Free Software Foundation; either version 2 of the License, or (at your option) any later version.

	This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

	DESCRIPTION:
		Sound control menu
\**********************************************************************************************************************************************/
#pragma once

#include "../../derma/common.h"

#include "../crispy.h"
#include "../m_config.h"

#include "mode.h"

#define WINDOW_HELP_URL "https://www.chocolate-doom.org/setup-compat"

void CompatibilitySettings(void* widget, void* user_data);
void BindCompatibilityVariables();

int vanilla_savegame_limit{0};
int vanilla_demo_limit{0};

void CompatibilitySettings(Widget* widget, void* user_data)
{
	if (gamemission == GameMission::doom)
	{
		txt::ShowMessageBox("", "Please refer to the in-game Crispness menu.");
	}
	else
	{
		txt::Window* window{"Compatibility"};

		window->SetWindowHelpURL(WINDOW_HELP_URL);

		window->AddWidgets( std::make_unique(txt::CheckBox("Vanilla savegame limit", &vanilla_savegame_limit)),
							std::make_unique(txt::CheckBox("Vanilla demo limit", &vanilla_demo_limit)),
							nullptr);
	}
}

void BindCompatibilityVariables()
{
	if (gamemission == GameMission::doom)
	{
		M_BindVariable<int>("crispy_automapoverlay", &crispy->automapoverlay);
		M_BindVariable<int>("crispy_automaprotate", &crispy->automaprotate);
		M_BindVariable<int>("crispy_automapstats", &crispy->automapstats);
		M_BindVariable<int>("crispy_bobfactor", &crispy->bobfactor);
		M_BindVariable<int>("crispy_btusetimer", &crispy->btusetimer);
		M_BindVariable<int>("crispy_brightmaps", &crispy->brightmaps);
		M_BindVariable<int>("crispy_centerweapon", &crispy->centerweapon);
		M_BindVariable<int>("crispy_coloredblood", &crispy->coloredblood);
		M_BindVariable<int>("crispy_coloredhud", &crispy->coloredhud);
		M_BindVariable<int>("crispy_crosshair", &crispy->crosshair);
		M_BindVariable<int>("crispy_crosshairhealth", &crispy->crosshairhealth);
		M_BindVariable<int>("crispy_crosshairtarget", &crispy->crosshairtarget);
		M_BindVariable<int>("crispy_crosshairtype", &crispy->crosshairtype);
		M_BindVariable<int>("crispy_demobar", &crispy->demobar);
		M_BindVariable<int>("crispy_demotimer", &crispy->demotimer);
		M_BindVariable<int>("crispy_demotimerdir", &crispy->demotimerdir);
		M_BindVariable<int>("crispy_extautomap", &crispy->extautomap);
		M_BindVariable<int>("crispy_extsaveg", &crispy->extsaveg);
		M_BindVariable<int>("crispy_flipcorpses", &crispy->flipcorpses);
		M_BindVariable<int>("crispy_freeaim", &crispy->freeaim);
		M_BindVariable<int>("crispy_freelook", &crispy->freelook);
		M_BindVariable<int>("crispy_hires", &crispy->hires);
		M_BindVariable<int>("crispy_jump", &crispy->jump);
		M_BindVariable<int>("crispy_leveltime", &crispy->leveltime);
		M_BindVariable<int>("crispy_mouselook", &crispy->mouselook);
		M_BindVariable<int>("crispy_neghealth", &crispy->neghealth);
		M_BindVariable<int>("crispy_overunder", &crispy->overunder);
		M_BindVariable<int>("crispy_pitch", &crispy->pitch);
		M_BindVariable<int>("crispy_playercoords", &crispy->playercoords);
		M_BindVariable<int>("crispy_recoil", &crispy->recoil);
		M_BindVariable<int>("crispy_secretmessage", &crispy->secretmessage);
		M_BindVariable<int>("crispy_smoothlight", &crispy->smoothlight);
		M_BindVariable<int>("crispy_smoothmap", &crispy->smoothmap);
		M_BindVariable<int>("crispy_smoothscaling", &crispy->smoothscaling);
		M_BindVariable<int>("crispy_soundfix", &crispy->soundfix);
		M_BindVariable<int>("crispy_soundfull", &crispy->soundfull);
		M_BindVariable<int>("crispy_soundmono", &crispy->soundmono);
		M_BindVariable<int>("crispy_translucency", &crispy->translucency);
#ifdef CRISPY_TRUECOLOR
		M_BindVariable<int>("crispy_truecolor", &crispy->truecolor);
#endif
		M_BindVariable<int>("crispy_uncapped", &crispy->uncapped);
		M_BindVariable<int>("crispy_vsync", &crispy->vsync);
		M_BindVariable<int>("crispy_weaponsquat", &crispy->weaponsquat);
		M_BindVariable<int>("crispy_widescreen", &crispy->widescreen);
	}
	else if (gamemission == heretic)
	{
		M_BindVariable<int>("vanilla_savegame_limit", &vanilla_savegame_limit);
		M_BindVariable<int>("vanilla_demo_limit", &vanilla_demo_limit);
		M_BindVariable<int>("crispy_automapstats", &crispy->automapstats);
		M_BindVariable<int>("crispy_hires", &crispy->hires);
		M_BindVariable<int>("crispy_leveltime", &crispy->leveltime);
		M_BindVariable<int>("crispy_playercoords", &crispy->playercoords);
		M_BindVariable<int>("crispy_secretmessage", &crispy->secretmessage);
		M_BindVariable<int>("crispy_smoothscaling", &crispy->smoothscaling);
		M_BindVariable<int>("crispy_uncapped", &crispy->uncapped);
		M_BindVariable<int>("crispy_vsync", &crispy->vsync);
	}
	else
	{
		M_BindVariable<int>("vanilla_savegame_limit", &vanilla_savegame_limit);
		M_BindVariable<int>("vanilla_demo_limit", &vanilla_demo_limit);
	}
}
