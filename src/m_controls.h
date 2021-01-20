/**********************************************************************************************************************************************\
	Copyright(C) 1993-1996 Id Software, Inc.
	Copyright(C) 1993-2008 Raven Software
	Copyright(C) 2005-2014 Simon Howard

	This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License
	as published by the Free Software Foundation; either version 2 of the License, or (at your option) any later version.

	This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
\**********************************************************************************************************************************************/
#pragma once

#include "../derma/common.h"

namespace cudadoom
{

Keys key_right = Keys::RIGHT;
Keys key_left = Keys::LEFT;
Keys key_reverse = Keys::null;

Keys key_up = Keys::KEY_W;
Keys key_alt_up = Keys::UP;
Keys key_down = Keys::KEY_S;
Keys key_alt_down = Keys::DOWN;
Keys key_strafeleft = Keys::KEY_A;
Keys key_alt_strafeleft = Keys::LEFT;
Keys key_straferight = Keys::KEY_D;
Keys key_alt_straferight = Keys::RIGHT;

Keys key_fire = Keys::RCTRL;
Keys key_use = Keys::KEY_E;
Keys key_strafe = Keys::RALT;
Keys key_speed = Keys::RSHIFT;
Keys key_toggleautorun = Keys::NUMLOCK;
Keys key_togglenovert = Keys::null;

//========================================
// Heretic keyboard controls
Keys key_flyup = Keys::PAGEUP;
Keys key_flydown = Keys::INSERT;
Keys key_flycenter = Keys::HOME;

Keys key_lookup = Keys::PAGEDOWN;
Keys key_lookdown = Keys::DEL;
Keys key_lookcenter = Keys::END;

Keys key_invleft = Keys::LEFTBRACKET;
Keys key_invright = Keys::RIGHTBRACKET;
Keys key_useartifact = Keys::KEY_Q;

Keys key_arti_quartz = Keys::null;
Keys key_arti_urn = Keys::null;
Keys key_arti_bomb = Keys::null;
Keys key_arti_tome = Keys::null; // was 127
Keys key_arti_ring = Keys::null;
Keys key_arti_chaosdevice = Keys::null;
Keys key_arti_shadowsphere = Keys::null;
Keys key_arti_wings = Keys::null;
Keys key_arti_torch = Keys::null;
Keys key_arti_morph = Keys::null;

//========================================
// Hexen key controls
Keys key_jump = Keys::SPACE;

Keys key_arti_all = Keys::BACKSPACE;
Keys key_arti_health = Keys::BACKSLASH;
Keys key_arti_poisonbag = Keys::KEY_0;
Keys key_arti_blastradius = Keys::KEY_9;
Keys key_arti_teleport = Keys::KEY_8;
Keys key_arti_teleportother = Keys::KEY_7;
Keys key_arti_egg = Keys::KEY_6;
Keys key_arti_invulnerability = Keys::KEY_5;

//========================================
// Strife key controls
// Note: Strife also uses key_invleft, key_invright, key_jump, key_lookup, and key_lookdown, but with different default values.
Keys key_usehealth = Keys::KEY_H;
Keys key_invquery = Keys::KEY_I;
Keys key_mission = Keys::KEY_L;
Keys key_invpop = Keys::KEY_Z;
Keys key_invkey = Keys::KEY_K;
Keys key_invhome = Keys::HOME;
Keys key_invend = Keys::END;
Keys key_invuse = Keys::ENTER;
Keys key_invdrop = Keys::BACKSPACE;

//========================================
// misc controls
Keys key_message_refresh = Keys::ENTER;
Keys key_pause = Keys::PAUSE;
Keys key_demo_quit = Keys::ESCAPE;
Keys key_spy = Keys::F12;

//========================================
// Multiplayer chat keys:
Keys key_multi_msg = Keys::KEY_T;
::std::array<Keys, 8> key_multi_msgplayer;

//========================================
// Weapon selection keys:
Keys key_weapon1 = Keys::KEY_1;
Keys key_weapon2 = Keys::KEY_2;
Keys key_weapon3 = Keys::KEY_3;
Keys key_weapon4 = Keys::KEY_4;
Keys key_weapon5 = Keys::KEY_5;
Keys key_weapon6 = Keys::KEY_6;
Keys key_weapon7 = Keys::KEY_7;
Keys key_weapon8 = Keys::KEY_8;
Keys key_prevweapon = Keys::null;
Keys key_nextweapon = Keys::null;

//========================================
// Map control keys:
Keys key_map_north = Keys::UP;
Keys key_map_south = Keys::DOWN;
Keys key_map_east = Keys::RIGHT;
Keys key_map_west = Keys::LEFT;
Keys key_map_zoomin = Keys::EQUALS;
Keys key_map_zoomout = Keys::MINUS;
Keys key_map_toggle = Keys::TAB;
Keys key_map_maxzoom = Keys::KEY_0;
Keys key_map_follow = Keys::KEY_F;
Keys key_map_grid = Keys::KEY_G;
Keys key_map_mark = Keys::KEY_M;
Keys key_map_clearmark = Keys::KEY_C;
Keys key_map_overlay = Keys::KEY_O;
Keys key_map_rotate = Keys::KEY_R;

//========================================
// menu keys:
Keys key_menu_activate = Keys::ESCAPE;
Keys key_menu_up = Keys::UP;
Keys key_menu_down = Keys::DOWN;
Keys key_menu_left = Keys::LEFT;
Keys key_menu_right = Keys::RIGHT;
Keys key_menu_back = Keys::BACKSPACE;
Keys key_menu_forward = Keys::ENTER;
Keys key_menu_confirm = Keys::KEY_Y;
Keys key_menu_abort = Keys::KEY_N;

Keys key_menu_help = Keys::F1;
Keys key_menu_save = Keys::F2;
Keys key_menu_load = Keys::F3;
Keys key_menu_volume = Keys::F4;
Keys key_menu_detail = Keys::F5;
Keys key_menu_qsave = Keys::F6;
Keys key_menu_endgame = Keys::F7;
Keys key_menu_messages = Keys::F8;
Keys key_menu_qload = Keys::F9;
Keys key_menu_quit = Keys::F10;
Keys key_menu_gamma = Keys::F11;

Keys key_menu_incscreen = Keys::EQUALS;
Keys key_menu_decscreen = Keys::MINUS;
Keys key_menu_screenshot = Keys::PRINTSCREEN;
Keys key_menu_cleanscreenshot = Keys::null;
Keys key_menu_del = Keys::DEL;
Keys key_menu_nextlevel = Keys::null;
Keys key_menu_reloadlevel = Keys::null;

//========================================
// Mouse controls
int mousebfire = 0;
int mousebstrafe = 1;
int mousebforward = 2;

int mousebjump = -1;

int mousebstrafeleft = -1;
int mousebstraferight = -1;
int mousebbackward = -1;
int mousebuse = -1;
int mousebmouselook = -1;
int mousebreverse = -1;

int mousebprevweapon = 4;
int mousebnextweapon = 3;
int mousebinvleft = -1;
int mousebinvright = -1;

// Control whether if a mouse button is double clicked, it acts like "use" has been pressed
bool dclick_use = true;

//========================================
// Joystick controls
int joybfire = 0;
int joybstrafe = 1;
int joybuse = 3;
int joybspeed = 2;

int joybstrafeleft = -1;
int joybstraferight = -1;

int joybjump = -1;

int joybprevweapon = -1;
int joybnextweapon = -1;

int joybmenu = -1;
int joybautomap = -1;

//========================================
void M_BindBaseControls();
void M_BindHereticControls();
void M_BindHexenControls();
void M_BindStrifeControls();
void M_BindWeaponControls();
void M_BindMapControls();
void M_BindMenuControls();
void M_BindChatControls(unsigned num_players);

void M_ApplyPlatformDefaults();

} // end namespace cudadoom
