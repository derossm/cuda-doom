/**********************************************************************************************************************************************\
	Copyright(C) 1993-1996 Id Software, Inc.
	Copyright(C) 1993-2008 Raven Software
	Copyright(C) 2005-2014 Simon Howard

	This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License
	as published by the Free Software Foundation; either version 2 of the License, or (at your option) any later version.

	This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
\**********************************************************************************************************************************************/
#include "../derma/stdafx.h"

#include "m_config.h"
#include "m_misc.h"

#include "m_controls.h"

namespace cudadoom
{

//========================================
// Bind all of the common controls used by Doom and all other games.
void M_BindBaseControls()
{
	M_BindVariable("key_right", &key_right);
	M_BindVariable("key_left", &key_left);
	M_BindVariable("key_up", &key_up);
	M_BindVariable("key_alt_up", &key_alt_up);
	M_BindVariable("key_down", &key_down);
	M_BindVariable("key_alt_down", &key_alt_down);
	M_BindVariable("key_strafeleft", &key_strafeleft);
	M_BindVariable("key_alt_strafeleft", &key_alt_strafeleft);
	M_BindVariable("key_straferight", &key_straferight);
	M_BindVariable("key_alt_straferight", &key_alt_straferight);
	M_BindVariable("key_fire", &key_fire);
	M_BindVariable("key_use", &key_use);
	M_BindVariable("key_strafe", &key_strafe);
	M_BindVariable("key_speed", &key_speed);

	M_BindVariable("mouseb_fire", &mousebfire);
	M_BindVariable("mouseb_strafe", &mousebstrafe);
	M_BindVariable("mouseb_forward", &mousebforward);

	M_BindVariable("joyb_fire", &joybfire);
	M_BindVariable("joyb_strafe", &joybstrafe);
	M_BindVariable("joyb_use", &joybuse);
	M_BindVariable("joyb_speed", &joybspeed);

	M_BindVariable("joyb_menu_activate", &joybmenu);
	M_BindVariable("joyb_toggle_automap", &joybautomap);

	// Extra controls that are not in the Vanilla versions:

	M_BindVariable("joyb_strafeleft", &joybstrafeleft);
	M_BindVariable("joyb_straferight", &joybstraferight);
	M_BindVariable("mouseb_strafeleft", &mousebstrafeleft);
	M_BindVariable("mouseb_straferight", &mousebstraferight);
	M_BindVariable("mouseb_use", &mousebuse);
	M_BindVariable("mouseb_backward", &mousebbackward);
	M_BindVariable("dclick_use", &dclick_use);
	M_BindVariable("key_pause", &key_pause);
	M_BindVariable("key_message_refresh", &key_message_refresh);

	M_BindVariable("key_lookup", &key_lookup);
	M_BindVariable("key_lookdown", &key_lookdown);
	M_BindVariable("key_lookcenter", &key_lookcenter);

	M_BindVariable("key_jump", &key_jump);
	M_BindVariable("mouseb_jump", &mousebjump);
	M_BindVariable("joyb_jump", &joybjump);

	M_BindVariable("mouseb_mouselook", &mousebmouselook);
	M_BindVariable("mouseb_reverse", &mousebreverse);
	M_BindVariable("key_reverse", &key_reverse);
	M_BindVariable("key_toggleautorun", &key_toggleautorun);
	M_BindVariable("key_togglenovert", &key_togglenovert);
}

void M_BindHereticControls()
{
	M_BindVariable("key_flyup", &key_flyup);
	M_BindVariable("key_flydown", &key_flydown);
	M_BindVariable("key_flycenter", &key_flycenter);

	// key_look* moved to M_BindBaseControls()

	M_BindVariable("key_invleft", &key_invleft);
	M_BindVariable("key_invright", &key_invright);
	M_BindVariable("key_useartifact", &key_useartifact);

	M_BindVariable("mouseb_invleft", &mousebinvleft);
	M_BindVariable("mouseb_invright", &mousebinvright);

	M_BindVariable("key_arti_quartz", &key_arti_quartz);
	M_BindVariable("key_arti_urn", &key_arti_urn);
	M_BindVariable("key_arti_bomb", &key_arti_bomb);
	M_BindVariable("key_arti_tome", &key_arti_tome);
	M_BindVariable("key_arti_ring", &key_arti_ring);
	M_BindVariable("key_arti_chaosdevice", &key_arti_chaosdevice);
	M_BindVariable("key_arti_shadowsphere", &key_arti_shadowsphere);
	M_BindVariable("key_arti_wings", &key_arti_wings);
	M_BindVariable("key_arti_torch", &key_arti_torch);
	M_BindVariable("key_arti_morph", &key_arti_morph);
}

void M_BindHexenControls()
{
	// *_jump moved to M_BindBaseControls()

	M_BindVariable("key_arti_all", &key_arti_all);
	M_BindVariable("key_arti_health", &key_arti_health);
	M_BindVariable("key_arti_poisonbag", &key_arti_poisonbag);
	M_BindVariable("key_arti_blastradius", &key_arti_blastradius);
	M_BindVariable("key_arti_teleport", &key_arti_teleport);
	M_BindVariable("key_arti_teleportother", &key_arti_teleportother);
	M_BindVariable("key_arti_egg", &key_arti_egg);
	M_BindVariable("key_arti_invulnerability", &key_arti_invulnerability);
}

void M_BindStrifeControls()
{
	// These are shared with all games, but have different defaults:
	key_message_refresh = Keys::SLASH;

	// These keys are shared with Heretic/Hexen but have different defaults:
	key_jump = Keys::SPACE;
	key_lookup = Keys::PAGEUP;
	key_lookdown = Keys::PAGEDOWN;
	key_invleft = Keys::INSERT;
	key_invright = Keys::DEL;

	M_BindVariable("key_jump", &key_jump);
	M_BindVariable("key_lookUp", &key_lookup);
	M_BindVariable("key_lookDown", &key_lookdown);
	M_BindVariable("key_invLeft", &key_invleft);
	M_BindVariable("key_invRight", &key_invright);

	// Custom Strife-only Keys:
	M_BindVariable("key_useHealth", &key_usehealth);
	M_BindVariable("key_invquery", &key_invquery);
	M_BindVariable("key_mission", &key_mission);
	M_BindVariable("key_invPop", &key_invpop);
	M_BindVariable("key_invKey", &key_invkey);
	M_BindVariable("key_invHome", &key_invhome);
	M_BindVariable("key_invEnd", &key_invend);
	M_BindVariable("key_invUse", &key_invuse);
	M_BindVariable("key_invDrop", &key_invdrop);

	// Strife also supports jump on mouse and joystick, and in the exact same
	// manner as Hexen!
	M_BindVariable("mouseb_jump", &mousebjump);
	M_BindVariable("joyb_jump", &joybjump);
}

void M_BindWeaponControls()
{
	M_BindVariable("key_weapon1", &key_weapon1);
	M_BindVariable("key_weapon2", &key_weapon2);
	M_BindVariable("key_weapon3", &key_weapon3);
	M_BindVariable("key_weapon4", &key_weapon4);
	M_BindVariable("key_weapon5", &key_weapon5);
	M_BindVariable("key_weapon6", &key_weapon6);
	M_BindVariable("key_weapon7", &key_weapon7);
	M_BindVariable("key_weapon8", &key_weapon8);

	M_BindVariable("key_prevweapon", &key_prevweapon);
	M_BindVariable("key_nextweapon", &key_nextweapon);

	M_BindVariable("joyb_prevweapon", &joybprevweapon);
	M_BindVariable("joyb_nextweapon", &joybnextweapon);

	M_BindVariable("mouseb_prevweapon", &mousebprevweapon);
	M_BindVariable("mouseb_nextweapon", &mousebnextweapon);
}

void M_BindMapControls()
{
	M_BindVariable("key_map_north", &key_map_north);
	M_BindVariable("key_map_south", &key_map_south);
	M_BindVariable("key_map_east", &key_map_east);
	M_BindVariable("key_map_west", &key_map_west);
	M_BindVariable("key_map_zoomin", &key_map_zoomin);
	M_BindVariable("key_map_zoomout", &key_map_zoomout);
	M_BindVariable("key_map_toggle", &key_map_toggle);
	M_BindVariable("key_map_maxzoom", &key_map_maxzoom);
	M_BindVariable("key_map_follow", &key_map_follow);
	M_BindVariable("key_map_grid", &key_map_grid);
	M_BindVariable("key_map_mark", &key_map_mark);
	M_BindVariable("key_map_clearmark", &key_map_clearmark);
	M_BindVariable("key_map_overlay", &key_map_overlay);
	M_BindVariable("key_map_rotate", &key_map_rotate);
}

void M_BindMenuControls()
{
	M_BindVariable("key_menu_activate", &key_menu_activate);
	M_BindVariable("key_menu_up", &key_menu_up);
	M_BindVariable("key_menu_down", &key_menu_down);
	M_BindVariable("key_menu_left", &key_menu_left);
	M_BindVariable("key_menu_right", &key_menu_right);
	M_BindVariable("key_menu_back", &key_menu_back);
	M_BindVariable("key_menu_forward", &key_menu_forward);
	M_BindVariable("key_menu_confirm", &key_menu_confirm);
	M_BindVariable("key_menu_abort", &key_menu_abort);

	M_BindVariable("key_menu_help", &key_menu_help);
	M_BindVariable("key_menu_save", &key_menu_save);
	M_BindVariable("key_menu_load", &key_menu_load);
	M_BindVariable("key_menu_volume", &key_menu_volume);
	M_BindVariable("key_menu_detail", &key_menu_detail);
	M_BindVariable("key_menu_qsave", &key_menu_qsave);
	M_BindVariable("key_menu_endgame", &key_menu_endgame);
	M_BindVariable("key_menu_messages", &key_menu_messages);
	M_BindVariable("key_menu_qload", &key_menu_qload);
	M_BindVariable("key_menu_quit", &key_menu_quit);
	M_BindVariable("key_menu_gamma", &key_menu_gamma);

	M_BindVariable("key_menu_incscreen", &key_menu_incscreen);
	M_BindVariable("key_menu_decscreen", &key_menu_decscreen);
	M_BindVariable("key_menu_screenshot", &key_menu_screenshot);
	M_BindVariable("key_menu_cleanscreenshot", &key_menu_cleanscreenshot);
	M_BindVariable("key_menu_del", &key_menu_del);
	M_BindVariable("key_demo_quit", &key_demo_quit);
	M_BindVariable("key_spy", &key_spy);
	M_BindVariable("key_menu_nextlevel", &key_menu_nextlevel);
	M_BindVariable("key_menu_reloadlevel", &key_menu_reloadlevel);
}

void M_BindChatControls(unsigned num_players)
{
	::std::string name;

	M_BindVariable("key_multi_msg", &key_multi_msg);

	for (size_t i{0}; i < num_players; ++i)
	{
		M_snprintf(name, "key_multi_msgplayer%i", i + 1);
		M_BindVariable(name, &key_multi_msgplayer[i]);
	}
}

// Apply custom patches to the default values depending on the platform we are running on.
void M_ApplyPlatformDefaults()
{
	// no-op. Add your platform-specific patches here.
}

} // end namespace cudadoom
