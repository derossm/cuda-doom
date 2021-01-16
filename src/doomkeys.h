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

#include "../derma/stdafx.h"

#include "../derma/keybinds.h"
/*
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
constexpr ::std::array<int, 104> scancode_translate_table
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
	const unsigned char key;
	::std::array<const char, 7> name;
};

//constexpr int k = sizeof(KeyNames);
constexpr ::std::array<KeyNames, 84> key_names{
	KeyNames{ .key{KEY_BACKSPACE},	.name{"BACKSP"}	},	KeyNames{ .key{KEY_TAB},		.name{"TAB"}	},
	KeyNames{ .key{KEY_INS},		.name{"INS"}	},	KeyNames{ .key{KEY_DEL},		.name{"DEL"}	},
	KeyNames{ .key{KEY_PGUP},		.name{"PGUP"}	},	KeyNames{ .key{KEY_PGDN},		.name{"PGDN"}	},
	KeyNames{ .key{KEY_ENTER},		.name{"ENTER"}	},	KeyNames{ .key{KEY_ESCAPE},		.name{"ESC"}	},
	KeyNames{ .key{KEY_F1},			.name{"F1"}		},	KeyNames{ .key{KEY_F2},			.name{"F2"}		},
	KeyNames{ .key{KEY_F3},			.name{"F3"}		},	KeyNames{ .key{KEY_F4},			.name{"F4"}		},
	KeyNames{ .key{KEY_F5},			.name{"F5"}		},	KeyNames{ .key{KEY_F6},			.name{"F6"}		},
	KeyNames{ .key{KEY_F7},			.name{"F7"}		},	KeyNames{ .key{KEY_F8},			.name{"F8"}		},
	KeyNames{ .key{KEY_F9},			.name{"F9"}		},	KeyNames{ .key{KEY_F10},		.name{"F10"}	},
	KeyNames{ .key{KEY_F11},		.name{"F11"}	},	KeyNames{ .key{KEY_F12},		.name{"F12"}	},
	KeyNames{ .key{KEY_HOME},		.name{"HOME"}	},	KeyNames{ .key{KEY_END},		.name{"END"}	},
	KeyNames{ .key{KEY_MINUS},		.name{"-"}		},	KeyNames{ .key{KEY_EQUALS},		.name{"=" }		},
	KeyNames{ .key{KEY_NUMLOCK},	.name{"NUMLCK"}	},	KeyNames{ .key{KEY_SCRLCK},		.name{"SCRLCK"}	},
	KeyNames{ .key{KEY_PAUSE},		.name{"PAUSE"}	},	KeyNames{ .key{KEY_PRTSCR},		.name{"PRTSC"}	},
	KeyNames{ .key{KEY_UPARROW},	.name{"UP"}		},	KeyNames{ .key{KEY_DOWNARROW},	.name{"DOWN"}	},
	KeyNames{ .key{KEY_LEFTARROW},	.name{"LEFT"}	},	KeyNames{ .key{KEY_RIGHTARROW},	.name{"RIGHT"}	},
	KeyNames{ .key{KEY_RALT},		.name{"ALT"}	},	KeyNames{ .key{KEY_LALT},		.name{"ALT"}	},
	KeyNames{ .key{KEY_RSHIFT},		.name{"SHIFT"}	},	KeyNames{ .key{KEY_CAPSLOCK},	.name{"CAPS"}	},
	KeyNames{ .key{KEY_RCTRL},		.name{"CTRL"}	},	KeyNames{ .key{KEYP_5},			.name{"NUM5"}	},

	KeyNames{ .key{' '},			.name{"SPACE"}	},
	KeyNames{ .key{'a'},			.name{"A"}		},	KeyNames{ .key{'b'},			.name{"B"}		},
	KeyNames{ .key{'c'}, 			.name{"C"}		},	KeyNames{ .key{'d'},			.name{"D"}		},
	KeyNames{ .key{'e'},			.name{"E"}		},	KeyNames{ .key{'f'},			.name{"F"}		},
	KeyNames{ .key{'g'},			.name{"G"}		},	KeyNames{ .key{'h'},			.name{"H"}		},
	KeyNames{ .key{'i'},			.name{"I"}		},	KeyNames{ .key{'j'},			.name{"J"}		},
	KeyNames{ .key{'k'},			.name{"K"}		},	KeyNames{ .key{'l'},			.name{"L"}		},
	KeyNames{ .key{'m'},			.name{"M"}		},	KeyNames{ .key{'n'},			.name{"N"}		},
	KeyNames{ .key{'o'},			.name{"O"}		},	KeyNames{ .key{'p'},			.name{"P"}		},
	KeyNames{ .key{'q'},			.name{"Q"}		},	KeyNames{ .key{'r'},			.name{"R"}		},
	KeyNames{ .key{'s'},			.name{"S"}		},	KeyNames{ .key{'t'},			.name{"T"}		},
	KeyNames{ .key{'u'},			.name{"U"}		},	KeyNames{ .key{'v'},			.name{"V"}		},
	KeyNames{ .key{'w'},			.name{"W"}		},	KeyNames{ .key{'x'},			.name{"X"}		},
	KeyNames{ .key{'y'},			.name{"Y"}		},	KeyNames{ .key{'z'},			.name{"Z"}		},
	KeyNames{ .key{'0'},			.name{"0"}		},	KeyNames{ .key{'1'},			.name{"1"}		},
	KeyNames{ .key{'2'},			.name{"2"}		},	KeyNames{ .key{'3'},			.name{"3"}		},
	KeyNames{ .key{'4'},			.name{"4"}		},	KeyNames{ .key{'5'},			.name{"5"}		},
	KeyNames{ .key{'6'},			.name{"6"}		},	KeyNames{ .key{'7'},			.name{"7"}		},
	KeyNames{ .key{'8'},			.name{"8"}		},	KeyNames{ .key{'9'},			.name{"9"}		},
	KeyNames{ .key{'['},			.name{"["}		},	KeyNames{ .key{']'},			.name{"]"}		},
	KeyNames{ .key{';'},			.name{";"}		},	KeyNames{ .key{'`'},			.name{"`"}		},
	KeyNames{ .key{','},			.name{","}		},	KeyNames{ .key{'.'},			.name{"."}		},
	KeyNames{ .key{'/'},			.name{"/"}		},	KeyNames{ .key{'\\'},			.name{"\\"}		},
	KeyNames{ .key{'\''},			.name{"\'"}		}
};
*/
