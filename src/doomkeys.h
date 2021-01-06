/**********************************************************************************************************************************************\
	Copyright(C) 1993-1996 Id Software, Inc.
	Copyright(C) 2005-2014 Simon Howard

	This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License
	as published by the Free Software Foundation; either version 2 of the License, or (at your option) any later version.

	This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

	DESCRIPTION:
		Key definitions
\**********************************************************************************************************************************************/
#pragma once

#include "../derma/common.h"

// DOOM keyboard definition. This is the stuff configured by Setup.Exe. Most key data are simple ascii (uppercased).
enum class Keybinds
{
	KEY_RIGHTARROW = 0xae,
	KEY_LEFTARROW = 0xac,
	KEY_UPARROW = 0xad,
	KEY_DOWNARROW = 0xaf,
	KEY_ESCAPE = 27,
	KEY_ENTER = 13,
	KEY_TAB = 9,
	KEY_F1 = (0x80 + 0x3b),
	KEY_F2 = (0x80 + 0x3c),
	KEY_F3 = (0x80 + 0x3d),
	KEY_F4 = (0x80 + 0x3e),
	KEY_F5 = (0x80 + 0x3f),
	KEY_F6 = (0x80 + 0x40),
	KEY_F7 = (0x80 + 0x41),
	KEY_F8 = (0x80 + 0x42),
	KEY_F9 = (0x80 + 0x43),
	KEY_F10 = (0x80 + 0x44),
	KEY_F11 = (0x80 + 0x57),
	KEY_F12 = (0x80 + 0x58),
	KEY_BACKSPACE = 0x7f,
	KEY_PAUSE = 0xff,
	KEY_EQUALS = 0x3d,
	KEY_MINUS = 0x2d,
	KEY_RSHIFT = (0x80 + 0x36),
	KEY_RCTRL = (0x80 + 0x1d),
	KEY_RALT = (0x80 + 0x38),
	KEY_LALT = KEY_RALT,
	// new keys:
	KEY_CAPSLOCK = (0x80 + 0x3a),
	KEY_NUMLOCK = (0x80 + 0x45),
	KEY_SCRLCK = (0x80 + 0x46),
	KEY_PRTSCR = (0x80 + 0x59),
	KEY_HOME = (0x80 + 0x47),
	KEY_END = (0x80 + 0x4f),
	KEY_PGUP = (0x80 + 0x49),
	KEY_PGDN = (0x80 + 0x51),
	KEY_INS = (0x80 + 0x52),
	KEY_DEL = (0x80 + 0x53),
	KEYP_0 = KEY_INS,
	KEYP_1 = KEY_END,
	KEYP_2 = KEY_DOWNARROW,
	KEYP_3 = KEY_PGDN,
	KEYP_4 = KEY_LEFTARROW,
	KEYP_5 = (0x80 + 0x4c),
	KEYP_6 = KEY_RIGHTARROW,
	KEYP_7 = KEY_HOME,
	KEYP_8 = KEY_UPARROW,
	KEYP_9 = KEY_PGUP,
	KEYP_DIVIDE = '/',
	KEYP_PLUS = '+',
	KEYP_MINUS = '-',
	KEYP_MULTIPLY = '*',
	KEYP_PERIOD = 0,
	KEYP_EQUALS = KEY_EQUALS,
	KEYP_ENTER = KEY_ENTER,
	//
	KEY_A = 'a',
	KEY_B = 'b',
	KEY_C = 'c',
	KEY_D = 'd',
	KEY_E = 'e',
	KEY_F = 'f',
	KEY_G = 'g',
	KEY_H = 'h',
	KEY_I = 'i',
	KEY_J = 'j',
	KEY_K = 'k',
	KEY_L = 'l',
	KEY_M = 'm',
	KEY_N = 'n',
	KEY_O = 'o',
	KEY_P = 'p',
	KEY_Q = 'q',
	KEY_R = 'r',
	KEY_S = 's',
	KEY_T = 't',
	KEY_U = 'u',
	KEY_V = 'v',
	KEY_W = 'w',
	KEY_X = 'x',
	KEY_Y = 'y',
	KEY_Z = 'z'
};

constexpr size_t KEY_RIGHTARROW{0xae};
constexpr size_t KEY_LEFTARROW{0xac};
constexpr size_t KEY_UPARROW{0xad};
constexpr size_t KEY_DOWNARROW{0xaf};
constexpr size_t KEY_ESCAPE{27};
constexpr size_t KEY_ENTER{13};
constexpr size_t KEY_TAB{9};
constexpr size_t KEY_F1{(0x80 + 0x3b)};
constexpr size_t KEY_F2{(0x80 + 0x3c)};
constexpr size_t KEY_F3{(0x80 + 0x3d)};
constexpr size_t KEY_F4{(0x80 + 0x3e)};
constexpr size_t KEY_F5{(0x80 + 0x3f)};
constexpr size_t KEY_F6{(0x80 + 0x40)};
constexpr size_t KEY_F7{(0x80 + 0x41)};
constexpr size_t KEY_F8{(0x80 + 0x42)};
constexpr size_t KEY_F9{(0x80 + 0x43)};
constexpr size_t KEY_F10{(0x80 + 0x44)};
constexpr size_t KEY_F11{(0x80 + 0x57)};
constexpr size_t KEY_F12{(0x80 + 0x58)};

constexpr size_t KEY_BACKSPACE{0x7f};
constexpr size_t KEY_PAUSE{0xff};

constexpr size_t KEY_EQUALS{0x3d};
constexpr size_t KEY_MINUS{0x2d};

constexpr size_t KEY_RSHIFT{(0x80 + 0x36)};
constexpr size_t KEY_RCTRL{(0x80 + 0x1d)};
constexpr size_t KEY_RALT{(0x80 + 0x38)};

constexpr size_t KEY_LALT{KEY_RALT};

// new keys:

constexpr size_t KEY_CAPSLOCK{(0x80 + 0x3a)};
constexpr size_t KEY_NUMLOCK{(0x80 + 0x45)};
constexpr size_t KEY_SCRLCK{(0x80 + 0x46)};
constexpr size_t KEY_PRTSCR{(0x80 + 0x59)};

constexpr size_t KEY_HOME{(0x80 + 0x47)};
constexpr size_t KEY_END{(0x80 + 0x4f)};
constexpr size_t KEY_PGUP{(0x80 + 0x49)};
constexpr size_t KEY_PGDN{(0x80 + 0x51)};
constexpr size_t KEY_INS{(0x80 + 0x52)};
constexpr size_t KEY_DEL{(0x80 + 0x53)};

constexpr size_t KEYP_0{KEY_INS};
constexpr size_t KEYP_1{KEY_END};
constexpr size_t KEYP_2{KEY_DOWNARROW};
constexpr size_t KEYP_3{KEY_PGDN};
constexpr size_t KEYP_4{KEY_LEFTARROW};
constexpr size_t KEYP_5{(0x80 + 0x4c)};
constexpr size_t KEYP_6{KEY_RIGHTARROW};
constexpr size_t KEYP_7{KEY_HOME};
constexpr size_t KEYP_8{KEY_UPARROW};
constexpr size_t KEYP_9{KEY_PGUP};

constexpr size_t KEYP_DIVIDE{'/'};
constexpr size_t KEYP_PLUS{'+'};
constexpr size_t KEYP_MINUS{'-'};
constexpr size_t KEYP_MULTIPLY{'*'};
constexpr size_t KEYP_PERIOD{0};
constexpr size_t KEYP_EQUALS{KEY_EQUALS};
constexpr size_t KEYP_ENTER{KEY_ENTER};

#define SCANCODE_TO_KEYS_ARRAY													\
	0x0, 0x0, 0x0, 0x0, 'a',									/* 000-009 */	\
	'b', 'c', 'd', 'e', 'f',													\
	'g', 'h', 'i', 'j', 'k',									/* 010-019 */	\
	'l', 'm', 'n', 'o', 'p',													\
	'q', 'r', 's', 't', 'u',									/* 020-029 */	\
	'v', 'w', 'x', 'y', 'z',													\
	'1', '2', '3', '4', '5',									/* 030-039 */	\
	'6', '7', '8', '9', '0',													\
	KEY_ENTER, KEY_ESCAPE, KEY_BACKSPACE, KEY_TAB, ' ',			/* 040-049 */	\
	KEY_MINUS, KEY_EQUALS, '[', ']', '\\',										\
	0x0, ';', '\'', '`', ',',									/* 050-059 */	\
	'.', '/', KEY_CAPSLOCK, KEY_F1, KEY_F2,										\
	KEY_F3, KEY_F4, KEY_F5, KEY_F6, KEY_F7,						/* 060-069 */	\
	KEY_F8, KEY_F9, KEY_F10, KEY_F11, KEY_F12,									\
	KEY_PRTSCR, KEY_SCRLCK, KEY_PAUSE, KEY_INS, KEY_HOME,		/* 070-079 */	\
	KEY_PGUP, KEY_DEL, KEY_END, KEY_PGDN, KEY_RIGHTARROW,						\
	KEY_LEFTARROW, KEY_DOWNARROW, KEY_UPARROW,					/* 080-089 */	\
	KEY_NUMLOCK, KEYP_DIVIDE,													\
	KEYP_MULTIPLY, KEYP_MINUS, KEYP_PLUS, KEYP_ENTER, KEYP_1,					\
	KEYP_2, KEYP_3, KEYP_4, KEYP_5, KEYP_6,						/* 090-099 */	\
	KEYP_7, KEYP_8, KEYP_9, KEYP_0, KEYP_PERIOD,								\
	0x0, 0x0, 0x0, KEYP_EQUALS									/* 100-103 */

// Default names for keys, to use in English or as fallback.
#define KEY_NAMES_ARRAY													\
	{ KEY_BACKSPACE,	"BACKSP" },	{ KEY_TAB,			"TAB" },		\
	{ KEY_INS,			"INS" },	{ KEY_DEL,			"DEL" },		\
	{ KEY_PGUP,			"PGUP" },	{ KEY_PGDN,			"PGDN" },		\
	{ KEY_ENTER,		"ENTER" },	{ KEY_ESCAPE,		"ESC" },		\
	{ KEY_F1,			"F1" },		{ KEY_F2,			"F2" },			\
	{ KEY_F3,			"F3" },		{ KEY_F4,			"F4" },			\
	{ KEY_F5,			"F5" },		{ KEY_F6,			"F6" },			\
	{ KEY_F7,			"F7" },		{ KEY_F8,			"F8" },			\
	{ KEY_F9,			"F9" },		{ KEY_F10,			"F10" },		\
	{ KEY_F11,			"F11" },	{ KEY_F12,			"F12" },		\
	{ KEY_HOME,			"HOME" },	{ KEY_END,			"END" },		\
	{ KEY_MINUS,		"-" },		{ KEY_EQUALS,		"=" },			\
	{ KEY_NUMLOCK,		"NUMLCK" },	{ KEY_SCRLCK,		"SCRLCK" },		\
	{ KEY_PAUSE,		"PAUSE" },	{ KEY_PRTSCR,		"PRTSC" },		\
	{ KEY_UPARROW,		"UP" },		{ KEY_DOWNARROW,	"DOWN" },		\
	{ KEY_LEFTARROW,	"LEFT" }	{ KEY_RIGHTARROW,	"RIGHT" },		\
	{ KEY_RALT,			"ALT" },	{ KEY_LALT,			"ALT" },		\
	{ KEY_RSHIFT,		"SHIFT" },	{ KEY_CAPSLOCK,		"CAPS" },		\
	{ KEY_RCTRL,		"CTRL" },	{ KEYP_5,			"NUM5" },		\
	{ ' ',				"SPACE" },										\
	{ 'a', "A" },	{ 'b', "B" },	{ 'c', "C" },	{ 'd', "D" },		\
	{ 'e', "E" },	{ 'f', "F" },	{ 'g', "G" },	{ 'h', "H" },		\
	{ 'i', "I" },	{ 'j', "J" },	{ 'k', "K" },	{ 'l', "L" },		\
	{ 'm', "M" },	{ 'n', "N" },	{ 'o', "O" },	{ 'p', "P" },		\
	{ 'q', "Q" },	{ 'r', "R" },	{ 's', "S" },	{ 't', "T" },		\
	{ 'u', "U" },	{ 'v', "V" },	{ 'w', "W" },	{ 'x', "X" },		\
	{ 'y', "Y" },	{ 'z', "Z" },	{ '0', "0" },	{ '1', "1" },		\
	{ '2', "2" },	{ '3', "3" },	{ '4', "4" },	{ '5', "5" },		\
	{ '6', "6" },	{ '7', "7" },	{ '8', "8" },	{ '9', "9" },		\
	{ '[', "[" },	{ ']', "]" },	{ ';', ";" },	{ '`', "`" },		\
	{ ',', "," },	{ '.', "." },	{ '/', "/" },	{ '\\', "\\" },		\
	{ '\'', "\'" }
