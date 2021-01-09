/**********************************************************************************************************************************************\
	Copyright(C) 2020 Mason DeRoss

	Released under the GNU All-permissive License

	Copying and distribution of this file, with or without modification, are permitted in any medium without royalty,
	provided the copyright notice and this notice are preserved. This file is offered as-is, without any warranty.

	DESCRIPTION:

\**********************************************************************************************************************************************/
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

#include <array>

enum class SCANCODES : short
{
	SDL_SCANCODE_UNKNOWN = 0,
	PADDING_0 = 1,
	PADDING_1 = 2,
	PADDING_2 = 3,
	SDL_SCANCODE_A = 4,	// These values are from usage page 0x07 (USB keyboard page).
	SDL_SCANCODE_B = 5,
	SDL_SCANCODE_C = 6,
	SDL_SCANCODE_D = 7,
	SDL_SCANCODE_E = 8,
	SDL_SCANCODE_F = 9,
	SDL_SCANCODE_G = 10,
	SDL_SCANCODE_H = 11,
	SDL_SCANCODE_I = 12,
	SDL_SCANCODE_J = 13,
	SDL_SCANCODE_K = 14,
	SDL_SCANCODE_L = 15,
	SDL_SCANCODE_M = 16,
	SDL_SCANCODE_N = 17,
	SDL_SCANCODE_O = 18,
	SDL_SCANCODE_P = 19,
	SDL_SCANCODE_Q = 20,
	SDL_SCANCODE_R = 21,
	SDL_SCANCODE_S = 22,
	SDL_SCANCODE_T = 23,
	SDL_SCANCODE_U = 24,
	SDL_SCANCODE_V = 25,
	SDL_SCANCODE_W = 26,
	SDL_SCANCODE_X = 27,
	SDL_SCANCODE_Y = 28,
	SDL_SCANCODE_Z = 29,
	SDL_SCANCODE_1 = 30,
	SDL_SCANCODE_2 = 31,
	SDL_SCANCODE_3 = 32,
	SDL_SCANCODE_4 = 33,
	SDL_SCANCODE_5 = 34,
	SDL_SCANCODE_6 = 35,
	SDL_SCANCODE_7 = 36,
	SDL_SCANCODE_8 = 37,
	SDL_SCANCODE_9 = 38,
	SDL_SCANCODE_0 = 39,
	SDL_SCANCODE_RETURN = 40,
	SDL_SCANCODE_ESCAPE = 41,
	SDL_SCANCODE_BACKSPACE = 42,
	SDL_SCANCODE_TAB = 43,
	SDL_SCANCODE_SPACE = 44,
	SDL_SCANCODE_MINUS = 45,
	SDL_SCANCODE_EQUALS = 46,
	SDL_SCANCODE_LEFTBRACKET = 47,
	SDL_SCANCODE_RIGHTBRACKET = 48,
	SDL_SCANCODE_BACKSLASH = 49,
	SDL_SCANCODE_NONUSHASH = 50,
	SDL_SCANCODE_SEMICOLON = 51,
	SDL_SCANCODE_APOSTROPHE = 52,
	SDL_SCANCODE_GRAVE = 53,
	SDL_SCANCODE_COMMA = 54,
	SDL_SCANCODE_PERIOD = 55,
	SDL_SCANCODE_SLASH = 56,
	SDL_SCANCODE_CAPSLOCK = 57,
	SDL_SCANCODE_F1 = 58,
	SDL_SCANCODE_F2 = 59,
	SDL_SCANCODE_F3 = 60,
	SDL_SCANCODE_F4 = 61,
	SDL_SCANCODE_F5 = 62,
	SDL_SCANCODE_F6 = 63,
	SDL_SCANCODE_F7 = 64,
	SDL_SCANCODE_F8 = 65,
	SDL_SCANCODE_F9 = 66,
	SDL_SCANCODE_F10 = 67,
	SDL_SCANCODE_F11 = 68,
	SDL_SCANCODE_F12 = 69,
	SDL_SCANCODE_PRINTSCREEN = 70,
	SDL_SCANCODE_SCROLLLOCK = 71,
	SDL_SCANCODE_PAUSE = 72,
	SDL_SCANCODE_INSERT = 73,				// insert on PC, help on some Mac keyboards (but does send code 73, not 117)
	SDL_SCANCODE_HOME = 74,
	SDL_SCANCODE_PAGEUP = 75,
	SDL_SCANCODE_DELETE = 76,
	SDL_SCANCODE_END = 77,
	SDL_SCANCODE_PAGEDOWN = 78,
	SDL_SCANCODE_RIGHT = 79,
	SDL_SCANCODE_LEFT = 80,
	SDL_SCANCODE_DOWN = 81,
	SDL_SCANCODE_UP = 82,
	SDL_SCANCODE_NUMLOCKCLEAR = 83,			// num lock on PC, clear on Mac keyboards
	SDL_SCANCODE_KP_DIVIDE = 84,
	SDL_SCANCODE_KP_MULTIPLY = 85,
	SDL_SCANCODE_KP_MINUS = 86,
	SDL_SCANCODE_KP_PLUS = 87,
	SDL_SCANCODE_KP_ENTER = 88,
	SDL_SCANCODE_KP_1 = 89,
	SDL_SCANCODE_KP_2 = 90,
	SDL_SCANCODE_KP_3 = 91,
	SDL_SCANCODE_KP_4 = 92,
	SDL_SCANCODE_KP_5 = 93,
	SDL_SCANCODE_KP_6 = 94,
	SDL_SCANCODE_KP_7 = 95,
	SDL_SCANCODE_KP_8 = 96,
	SDL_SCANCODE_KP_9 = 97,
	SDL_SCANCODE_KP_0 = 98,
	SDL_SCANCODE_KP_PERIOD = 99,
	SDL_SCANCODE_NONUSBACKSLASH = 100,
	SDL_SCANCODE_APPLICATION = 101, 		// windows contextual menu, compose
	SDL_SCANCODE_POWER = 102,				// The USB document says this is a status flag, not a physical key - but some Mac keyboards do have a power key.
	SDL_SCANCODE_KP_EQUALS = 103,
	SDL_SCANCODE_F13 = 104,
	SDL_SCANCODE_F14 = 105,
	SDL_SCANCODE_F15 = 106,
	SDL_SCANCODE_F16 = 107,
	SDL_SCANCODE_F17 = 108,
	SDL_SCANCODE_F18 = 109,
	SDL_SCANCODE_F19 = 110,
	SDL_SCANCODE_F20 = 111,
	SDL_SCANCODE_F21 = 112,
	SDL_SCANCODE_F22 = 113,
	SDL_SCANCODE_F23 = 114,
	SDL_SCANCODE_F24 = 115,
	SDL_SCANCODE_EXECUTE = 116,
	SDL_SCANCODE_HELP = 117,
	SDL_SCANCODE_MENU = 118,
	SDL_SCANCODE_SELECT = 119,
	SDL_SCANCODE_STOP = 120,
	SDL_SCANCODE_AGAIN = 121, // redo
	SDL_SCANCODE_UNDO = 122,
	SDL_SCANCODE_CUT = 123,
	SDL_SCANCODE_COPY = 124,
	SDL_SCANCODE_PASTE = 125,
	SDL_SCANCODE_FIND = 126,
	SDL_SCANCODE_MUTE = 127,
	SDL_SCANCODE_VOLUMEUP = 128,
	SDL_SCANCODE_VOLUMEDOWN = 129,
	SDL_SCANCODE_LOCKINGCAPSLOCK = 130,	// not sure whether there's a reason to enable these
	SDL_SCANCODE_LOCKINGNUMLOCK = 131,	//
	SDL_SCANCODE_LOCKINGSCROLLLOCK = 132,	//
	SDL_SCANCODE_KP_COMMA = 133,
	SDL_SCANCODE_KP_EQUALSAS400 = 134,
	SDL_SCANCODE_INTERNATIONAL1 = 135,		// used on Asian keyboards, see footnotes in USB doc
	SDL_SCANCODE_INTERNATIONAL2 = 136,
	SDL_SCANCODE_INTERNATIONAL3 = 137,		// Yen
	SDL_SCANCODE_INTERNATIONAL4 = 138,
	SDL_SCANCODE_INTERNATIONAL5 = 139,
	SDL_SCANCODE_INTERNATIONAL6 = 140,
	SDL_SCANCODE_INTERNATIONAL7 = 141,
	SDL_SCANCODE_INTERNATIONAL8 = 142,
	SDL_SCANCODE_INTERNATIONAL9 = 143,
	SDL_SCANCODE_LANG1 = 144,				// Hangul/English toggle
	SDL_SCANCODE_LANG2 = 145,				// Hanja conversion
	SDL_SCANCODE_LANG3 = 146,				// Katakana
	SDL_SCANCODE_LANG4 = 147,				// Hiragana
	SDL_SCANCODE_LANG5 = 148,				// Zenkaku/Hankaku
	SDL_SCANCODE_LANG6 = 149,				// reserved
	SDL_SCANCODE_LANG7 = 150,				// reserved
	SDL_SCANCODE_LANG8 = 151,				// reserved
	SDL_SCANCODE_LANG9 = 152,				// reserved
	SDL_SCANCODE_ALTERASE = 153,			// Erase-Eaze
	SDL_SCANCODE_SYSREQ = 154,
	SDL_SCANCODE_CANCEL = 155,
	SDL_SCANCODE_CLEAR = 156,
	SDL_SCANCODE_PRIOR = 157,
	SDL_SCANCODE_RETURN2 = 158,
	SDL_SCANCODE_SEPARATOR = 159,
	SDL_SCANCODE_OUT = 160,
	SDL_SCANCODE_OPER = 161,
	SDL_SCANCODE_CLEARAGAIN = 162,
	SDL_SCANCODE_CRSEL = 163,
	SDL_SCANCODE_EXSEL = 164,
	PADDING_3 = 165,
	PADDING_4 = 166,
	PADDING_5 = 167,
	PADDING_6 = 168,
	PADDING_7 = 169,
	PADDING_8 = 170,
	PADDING_9 = 171,
	PADDING_A = 172,
	PADDING_B = 173,
	PADDING_C = 174,
	PADDING_D = 175,
	SDL_SCANCODE_KP_00 = 176,
	SDL_SCANCODE_KP_000 = 177,
	SDL_SCANCODE_THOUSANDSSEPARATOR = 178,
	SDL_SCANCODE_DECIMALSEPARATOR = 179,
	SDL_SCANCODE_CURRENCYUNIT = 180,
	SDL_SCANCODE_CURRENCYSUBUNIT = 181,
	SDL_SCANCODE_KP_LEFTPAREN = 182,
	SDL_SCANCODE_KP_RIGHTPAREN = 183,
	SDL_SCANCODE_KP_LEFTBRACE = 184,
	SDL_SCANCODE_KP_RIGHTBRACE = 185,
	SDL_SCANCODE_KP_TAB = 186,
	SDL_SCANCODE_KP_BACKSPACE = 187,
	SDL_SCANCODE_KP_A = 188,
	SDL_SCANCODE_KP_B = 189,
	SDL_SCANCODE_KP_C = 190,
	SDL_SCANCODE_KP_D = 191,
	SDL_SCANCODE_KP_E = 192,
	SDL_SCANCODE_KP_F = 193,
	SDL_SCANCODE_KP_XOR = 194,
	SDL_SCANCODE_KP_POWER = 195,
	SDL_SCANCODE_KP_PERCENT = 196,
	SDL_SCANCODE_KP_LESS = 197,
	SDL_SCANCODE_KP_GREATER = 198,
	SDL_SCANCODE_KP_AMPERSAND = 199,
	SDL_SCANCODE_KP_DBLAMPERSAND = 200,
	SDL_SCANCODE_KP_VERTICALBAR = 201,
	SDL_SCANCODE_KP_DBLVERTICALBAR = 202,
	SDL_SCANCODE_KP_COLON = 203,
	SDL_SCANCODE_KP_HASH = 204,
	SDL_SCANCODE_KP_SPACE = 205,
	SDL_SCANCODE_KP_AT = 206,
	SDL_SCANCODE_KP_EXCLAM = 207,
	SDL_SCANCODE_KP_MEMSTORE = 208,
	SDL_SCANCODE_KP_MEMRECALL = 209,
	SDL_SCANCODE_KP_MEMCLEAR = 210,
	SDL_SCANCODE_KP_MEMADD = 211,
	SDL_SCANCODE_KP_MEMSUBTRACT = 212,
	SDL_SCANCODE_KP_MEMMULTIPLY = 213,
	SDL_SCANCODE_KP_MEMDIVIDE = 214,
	SDL_SCANCODE_KP_PLUSMINUS = 215,
	SDL_SCANCODE_KP_CLEAR = 216,
	SDL_SCANCODE_KP_CLEARENTRY = 217,
	SDL_SCANCODE_KP_BINARY = 218,
	SDL_SCANCODE_KP_OCTAL = 219,
	SDL_SCANCODE_KP_DECIMAL = 220,
	SDL_SCANCODE_KP_HEXADECIMAL = 221,
	PADDING_E = 222,
	PADDING_F = 223,
	SDL_SCANCODE_LCTRL = 224,
	SDL_SCANCODE_LSHIFT = 225,
	SDL_SCANCODE_LALT = 226,
	SDL_SCANCODE_LGUI = 227,
	SDL_SCANCODE_RCTRL = 228,
	SDL_SCANCODE_RSHIFT = 229,
	SDL_SCANCODE_RALT = 230,
	SDL_SCANCODE_RGUI = 231,
	PADDING_G = 232,
	PADDING_H = 233,
	PADDING_I = 234,
	PADDING_J = 235,
	PADDING_K = 236,
	PADDING_L = 237,
	PADDING_M = 238,
	PADDING_N = 239,
	PADDING_O = 240,
	PADDING_P = 241,
	PADDING_Q = 242,
	PADDING_R = 243,
	PADDING_S = 244,
	PADDING_T = 245,
	PADDING_U = 246,
	PADDING_V = 247,
	PADDING_W = 248,
	PADDING_X = 249,
	PADDING_Y = 250,
	PADDING_Z = 251,
	PADDING_00 = 252,
	PADDING_01 = 253,
	PADDING_02 = 254,
	PADDING_03 = 255,
	PADDING_04 = 256,
	SDL_SCANCODE_MODE = 257,				// Usage page 0x07
	SDL_SCANCODE_AUDIONEXT = 258,			// These values are mapped from usage page 0x0C (USB consumer page).
	SDL_SCANCODE_AUDIOPREV = 259,
	SDL_SCANCODE_AUDIOSTOP = 260,
	SDL_SCANCODE_AUDIOPLAY = 261,
	SDL_SCANCODE_AUDIOMUTE = 262,
	SDL_SCANCODE_MEDIASELECT = 263,
	SDL_SCANCODE_WWW = 264,
	SDL_SCANCODE_MAIL = 265,
	SDL_SCANCODE_CALCULATOR = 266,
	SDL_SCANCODE_COMPUTER = 267,
	SDL_SCANCODE_AC_SEARCH = 268,
	SDL_SCANCODE_AC_HOME = 269,
	SDL_SCANCODE_AC_BACK = 270,
	SDL_SCANCODE_AC_FORWARD = 271,
	SDL_SCANCODE_AC_STOP = 272,
	SDL_SCANCODE_AC_REFRESH = 273,
	SDL_SCANCODE_AC_BOOKMARKS = 274,
	SDL_SCANCODE_BRIGHTNESSDOWN = 275,		// Usage page 0x0C
	SDL_SCANCODE_BRIGHTNESSUP = 276,
	SDL_SCANCODE_DISPLAYSWITCH = 277,		// display mirroring/dual display switch, video mode switch
	SDL_SCANCODE_KBDILLUMTOGGLE = 278,
	SDL_SCANCODE_KBDILLUMDOWN = 279,
	SDL_SCANCODE_KBDILLUMUP = 280,
	SDL_SCANCODE_EJECT = 281,
	SDL_SCANCODE_SLEEP = 282,
	SDL_SCANCODE_APP1 = 283,
	SDL_SCANCODE_APP2 = 284,				// These values are mapped from usage page 0x0C (USB consumer page).
	SDL_SCANCODE_AUDIOREWIND = 285,
	SDL_SCANCODE_AUDIOFASTFORWARD = 286,	// Usage page 0x0C (additional media keys)
	SDL_SCANCODE_COUNT,

	SDL_NUM_SCANCODES = 512				// not a key, just marks the number of scancodes for array bounds
};

	//SDL_SCANCODE_BACKSLASH = 49,	// Located at the lower left of the return key on ISO keyboards and at the right end of the QWERTY row on
									// ANSI keyboards. Produces REVERSE SOLIDUS (backslash) and VERTICAL LINE in a US layout, REVERSE SOLIDUS
									// and VERTICAL LINE in a UK Mac layout, NUMBER SIGN and TILDE in a UK Windows layout, DOLLAR SIGN and
									// POUND SIGN in a Swiss German layout, NUMBER SIGN and APOSTROPHE in a German layout, GRAVE ACCENT and
									// POUND SIGN in a French Mac layout, and ASTERISK and MICRO SIGN in a French Windows layout.

	//SDL_SCANCODE_NONUSHASH = 50,	// ISO USB keyboards actually use this code instead of 49 for the same key, but all OSes I've seen treat the
									// two codes identically. So, as an implementor, unless your keyboard generates both of those codes and your
									// OS treats them differently, you should generate SDL_SCANCODE_BACKSLASH instead of this code. As a user,
									// you should not rely on this code because SDL will never generate it with most (all?) keyboards.

	//SDL_SCANCODE_GRAVE = 53,		// Located in the top left corner (on both ANSI and ISO keyboards). Produces GRAVE ACCENT and TILDE in a
									// US Windows layout and in US and UK Mac layouts on ANSI keyboards, GRAVE ACCENT and NOT SIGN in a
									// UK Windows layout, SECTION SIGN and PLUS-MINUS SIGN in US and UK Mac layouts on ISO keyboards,
									// SECTION SIGN and DEGREE SIGN in a Swiss German layout (Mac: only on ISO keyboards), CIRCUMFLEX ACCENT
									// and DEGREE SIGN in a German layout (Mac: only on ISO keyboards), SUPERSCRIPT TWO and TILDE in a
									// French Windows layout, COMMERCIAL AT and NUMBER SIGN in a French Mac layout on ISO keyboards, and
									// LESS-THAN SIGN and GREATER-THAN SIGN in a Swiss German, German, or French Mac layout on ANSI keyboards.

	//SDL_SCANCODE_NONUSBACKSLASH = 100,// This is the additional key that ISO keyboards have over ANSI ones, located between left shift and Y.
										// Produces GRAVE ACCENT and TILDE in a US or UK Mac layout, REVERSE SOLIDUS (backslash) and VERTICAL LINE in a
										// US or UK Windows layout, and LESS-THAN SIGN and GREATER-THAN SIGN in a Swiss German, German, or French layout.

	//SDL_SCANCODE_MODE = 257,			// I'm not sure if this is really not covered by any of the above,
										// but since there's a special KMOD_MODE for it I'm adding it here

	//SDL_SCANCODE_LALT = 226,			// alt, option
	//SDL_SCANCODE_LGUI = 227,			// windows, command (apple), meta
	//SDL_SCANCODE_RCTRL = 228,
	//SDL_SCANCODE_RSHIFT = 229,
	//SDL_SCANCODE_RALT = 230,			// alt gr, option
	//SDL_SCANCODE_RGUI = 231,			// windows, command (apple), meta

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

// DOOM keyboard definition. This is the stuff configured by Setup.Exe. Most key data are simple ascii (uppercased).

static constexpr short KEY_RIGHTARROW{0xae};
static constexpr short KEY_LEFTARROW{0xac};
static constexpr short KEY_UPARROW{0xad};
static constexpr short KEY_DOWNARROW{0xaf};
static constexpr short KEY_ESCAPE{27};
static constexpr short KEY_ENTER{13};
static constexpr short KEY_TAB{9};
static constexpr short KEY_F1{(0x80 + 0x3b)};
static constexpr short KEY_F2{(0x80 + 0x3c)};
static constexpr short KEY_F3{(0x80 + 0x3d)};
static constexpr short KEY_F4{(0x80 + 0x3e)};
static constexpr short KEY_F5{(0x80 + 0x3f)};
static constexpr short KEY_F6{(0x80 + 0x40)};
static constexpr short KEY_F7{(0x80 + 0x41)};
static constexpr short KEY_F8{(0x80 + 0x42)};
static constexpr short KEY_F9{(0x80 + 0x43)};
static constexpr short KEY_F10{(0x80 + 0x44)};
static constexpr short KEY_F11{(0x80 + 0x57)};
static constexpr short KEY_F12{(0x80 + 0x58)};
static constexpr short KEY_BACKSPACE{0x7f};
static constexpr short KEY_PAUSE{0xff};
static constexpr short KEY_EQUALS{0x3d};
static constexpr short KEY_MINUS{0x2d};
static constexpr short KEY_RSHIFT{(0x80 + 0x36)};
static constexpr short KEY_RCTRL{(0x80 + 0x1d)};
static constexpr short KEY_RALT{(0x80 + 0x38)};
static constexpr short KEY_LALT{KEY_RALT};
static constexpr short KEY_CAPSLOCK{(0x80 + 0x3a)};
static constexpr short KEY_NUMLOCK{(0x80 + 0x45)};
static constexpr short KEY_SCRLCK{(0x80 + 0x46)};
static constexpr short KEY_PRTSCR{(0x80 + 0x59)};
static constexpr short KEY_HOME{(0x80 + 0x47)};
static constexpr short KEY_END{(0x80 + 0x4f)};
static constexpr short KEY_PGUP{(0x80 + 0x49)};
static constexpr short KEY_PGDN{(0x80 + 0x51)};
static constexpr short KEY_INS{(0x80 + 0x52)};
static constexpr short KEY_DEL{(0x80 + 0x53)};
static constexpr short KEYP_0{KEY_INS};
static constexpr short KEYP_1{KEY_END};
static constexpr short KEYP_2{KEY_DOWNARROW};
static constexpr short KEYP_3{KEY_PGDN};
static constexpr short KEYP_4{KEY_LEFTARROW};
static constexpr short KEYP_5{(0x80 + 0x4c)};
static constexpr short KEYP_6{KEY_RIGHTARROW};
static constexpr short KEYP_7{KEY_HOME};
static constexpr short KEYP_8{KEY_UPARROW};
static constexpr short KEYP_9{KEY_PGUP};
static constexpr short KEYP_DIVIDE{'/'};
static constexpr short KEYP_PLUS{'+'};
static constexpr short KEYP_MINUS{'-'};
static constexpr short KEYP_MULTIPLY{'*'};
static constexpr short KEYP_PERIOD{0};
static constexpr short KEYP_EQUALS{KEY_EQUALS};
static constexpr short KEYP_ENTER{KEY_ENTER};

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
	const unsigned char key;
	std::array<const char, 7> name;
};

//constexpr int k = sizeof(KeyNames);
constexpr std::array<KeyNames, 84> key_names{
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
