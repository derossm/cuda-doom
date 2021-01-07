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
	// a-z
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

//#define SCANCODE_TO_KEYS_ARRAY
constexpr std::array<int, 104> scancode_translate_table
{
	int{0x0},			int{0x0},			int{0x0},			int{0x0},			int{'a'},				// 000-009
	int{'b'},			int{'c'},			int{'d'},			int{'e'},			int{'f'},
	int{'g'},			int{'h'},			int{'i'},			int{'j'},			int{'k'},				// 010-019
	int{'l'},			int{'m'},			int{'n'},			int{'o'},			int{'p'},
	int{'q'},			int{'r'},			int{'s'},			int{'t'},			int{'u'},				// 020-029
	int{'v'},			int{'w'},			int{'x'},			int{'y'},			int{'z'},
	int{'1'},			int{'2'},			int{'3'},			int{'4'},			int{'5'},				// 030-039
	int{'6'},			int{'7'},			int{'8'},			int{'9'},			int{'0'},
	int{KEY_ENTER},		int{KEY_ESCAPE},	int{KEY_BACKSPACE},	int{KEY_TAB},		int{' '},				// 040-049
	int{KEY_MINUS},		int{KEY_EQUALS},	int{'['},			int{']'},			int{'\\'},
	int{0x0},			int{';'},			int{'\''},			int{'`'},			int{','},				// 050-059
	int{'.'},			int{'/'},			int{KEY_CAPSLOCK},	int{KEY_F1},		int{KEY_F2},
	int{KEY_F3},		int{KEY_F4},		int{KEY_F5},		int{KEY_F6},		int{KEY_F7},			// 060-069
	int{KEY_F8},		int{KEY_F9},		int{KEY_F10},		int{KEY_F11},		int{KEY_F12},
	int{KEY_PRTSCR},	int{KEY_SCRLCK},	int{KEY_PAUSE},		int{KEY_INS},		int{KEY_HOME},			// 070-079
	int{KEY_PGUP},		int{KEY_DEL},		int{KEY_END},		int{KEY_PGDN},		int{KEY_RIGHTARROW},
	int{KEY_LEFTARROW},	int{KEY_DOWNARROW},	int{KEY_UPARROW},	int{KEY_NUMLOCK},	int{KEYP_DIVIDE},		// 080-089
	int{KEYP_MULTIPLY},	int{KEYP_MINUS},	int{KEYP_PLUS},		int{KEYP_ENTER},	int{KEYP_1},
	int{KEYP_2},		int{KEYP_3},		int{KEYP_4},		int{KEYP_5},		int{KEYP_6},			// 090-099
	int{KEYP_7},		int{KEYP_8},		int{KEYP_9},		int{KEYP_0},		int{KEYP_PERIOD},
	int{0x0},			int{0x0},			int{0x0},			int{KEYP_EQUALS}							// 100-103
};

// Default names for keys, to use in English or as fallback.
//#define KEY_NAMES_ARRAY
struct KeyNames
{
	// 8 byte size total per key name pair
	const UCHAR key;
	const char name[7];
};

//constexpr int k = sizeof(KeyNames);
constexpr std::array<KeyNames, 84> key_names{
	const KeyNames{ const UCHAR{KEY_BACKSPACE},	{"BACKSP"}	},	const KeyNames{ const UCHAR{KEY_TAB},			{"TAB"}		},
	const KeyNames{ const UCHAR{KEY_INS},		{"INS"}		},	const KeyNames{ const UCHAR{KEY_DEL},			{"DEL"}		},
	const KeyNames{ const UCHAR{KEY_PGUP},		{"PGUP"}	},	const KeyNames{ const UCHAR{KEY_PGDN},			{"PGDN"}	},
	const KeyNames{ const UCHAR{KEY_ENTER},		{"ENTER"}	},	const KeyNames{ const UCHAR{KEY_ESCAPE},		{"ESC"}		},
	const KeyNames{ const UCHAR{KEY_F1},		{"F1"}		},	const KeyNames{ const UCHAR{KEY_F2},			{"F2"}		},
	const KeyNames{ const UCHAR{KEY_F3},		{"F3"}		},	const KeyNames{ const UCHAR{KEY_F4},			{"F4"}		},
	const KeyNames{ const UCHAR{KEY_F5},		{"F5"}		},	const KeyNames{ const UCHAR{KEY_F6},			{"F6"}		},
	const KeyNames{ const UCHAR{KEY_F7},		{"F7"}		},	const KeyNames{ const UCHAR{KEY_F8},			{"F8"}		},
	const KeyNames{ const UCHAR{KEY_F9},		{"F9"}		},	const KeyNames{ const UCHAR{KEY_F10},			{"F10"}		},
	const KeyNames{ const UCHAR{KEY_F11},		{"F11"}		},	const KeyNames{ const UCHAR{KEY_F12},			{"F12"}		},
	const KeyNames{ const UCHAR{KEY_HOME},		{"HOME"}	},	const KeyNames{ const UCHAR{KEY_END},			{"END"}		},
	const KeyNames{ const UCHAR{KEY_MINUS},		{"-"}		},	const KeyNames{ const UCHAR{KEY_EQUALS},		{"=" }		},
	const KeyNames{ const UCHAR{KEY_NUMLOCK},	{"NUMLCK"}	},	const KeyNames{ const UCHAR{KEY_SCRLCK},		{"SCRLCK"}	},
	const KeyNames{ const UCHAR{KEY_PAUSE},		{"PAUSE"}	},	const KeyNames{ const UCHAR{KEY_PRTSCR},		{"PRTSC"}	},
	const KeyNames{ const UCHAR{KEY_UPARROW},	{"UP"}		},	const KeyNames{ const UCHAR{KEY_DOWNARROW},		{"DOWN"}	},
	const KeyNames{ const UCHAR{KEY_LEFTARROW},	{"LEFT"}	},	const KeyNames{ const UCHAR{KEY_RIGHTARROW},	{"RIGHT"}	},
	const KeyNames{ const UCHAR{KEY_RALT},		{"ALT"}		},	const KeyNames{ const UCHAR{KEY_LALT},			{"ALT"}		},
	const KeyNames{ const UCHAR{KEY_RSHIFT},	{"SHIFT"}	},	const KeyNames{ const UCHAR{KEY_CAPSLOCK},		{"CAPS"}	},
	const KeyNames{ const UCHAR{KEY_RCTRL},		{"CTRL"}	},	const KeyNames{ const UCHAR{KEYP_5},			{"NUM5"}	},

	const KeyNames{ const UCHAR{' '},			{"SPACE"}	},
	const KeyNames{ const UCHAR{'a'},			{"A"}		},	const KeyNames{ const UCHAR{'b'},				{"B"}		},
	const KeyNames{ const UCHAR{'c'}, 			{"C"}		},	const KeyNames{ const UCHAR{'d'},				{"D"}		},
	const KeyNames{ const UCHAR{'e'},			{"E"}		},	const KeyNames{ const UCHAR{'f'},				{"F"}		},
	const KeyNames{ const UCHAR{'g'},			{"G"}		},	const KeyNames{ const UCHAR{'h'},				{"H"}		},
	const KeyNames{ const UCHAR{'i'},			{"I"}		},	const KeyNames{ const UCHAR{'j'},				{"J"}		},
	const KeyNames{ const UCHAR{'k'},			{"K"}		},	const KeyNames{ const UCHAR{'l'},				{"L"}		},
	const KeyNames{ const UCHAR{'m'},			{"M"}		},	const KeyNames{ const UCHAR{'n'},				{"N"}		},
	const KeyNames{ const UCHAR{'o'},			{"O"}		},	const KeyNames{ const UCHAR{'p'},				{"P"}		},
	const KeyNames{ const UCHAR{'q'},			{"Q"}		},	const KeyNames{ const UCHAR{'r'},				{"R"}		},
	const KeyNames{ const UCHAR{'s'},			{"S"}		},	const KeyNames{ const UCHAR{'t'},				{"T"}		},
	const KeyNames{ const UCHAR{'u'},			{"U"}		},	const KeyNames{ const UCHAR{'v'},				{"V"}		},
	const KeyNames{ const UCHAR{'w'},			{"W"}		},	const KeyNames{ const UCHAR{'x'},				{"X"}		},
	const KeyNames{ const UCHAR{'y'},			{"Y"}		},	const KeyNames{ const UCHAR{'z'},				{"Z"}		},
	const KeyNames{ const UCHAR{'0'},			{"0"}		},	const KeyNames{ const UCHAR{'1'},				{"1"}		},
	const KeyNames{ const UCHAR{'2'},			{"2"}		},	const KeyNames{ const UCHAR{'3'},				{"3"}		},
	const KeyNames{ const UCHAR{'4'},			{"4"}		},	const KeyNames{ const UCHAR{'5'},				{"5"}		},
	const KeyNames{ const UCHAR{'6'},			{"6"}		},	const KeyNames{ const UCHAR{'7'},				{"7"}		},
	const KeyNames{ const UCHAR{'8'},			{"8"}		},	const KeyNames{ const UCHAR{'9'},				{"9"}		},
	const KeyNames{ const UCHAR{'['},			{"["}		},	const KeyNames{ const UCHAR{']'},				{"]"}		},
	const KeyNames{ const UCHAR{';'},			{";"}		},	const KeyNames{ const UCHAR{'`'},				{"`"}		},
	const KeyNames{ const UCHAR{','},			{","}		},	const KeyNames{ const UCHAR{'.'},				{"."}		},
	const KeyNames{ const UCHAR{'/'},			{"/"}		},	const KeyNames{ const UCHAR{'\\'},				{"\\"}		},
	const KeyNames{ const UCHAR{'\''},			{"\'"}		}
};
