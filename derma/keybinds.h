/**********************************************************************************************************************************************\
	Copyright© 2020-2021 Mason DeRoss

	Released under the GNU All-permissive License

	Copying and distribution of this file, with or without modification, are permitted in any medium without royalty,
	provided the copyright notice and this notice are preserved. This file is offered as-is, without any warranty.

	DESCRIPTION:

\**********************************************************************************************************************************************/
#pragma once

#include <array>
#include <map>

#include <algorithm>
#include <functional>

#include <any>

// WINDOWS DEFINES FROM FILES WE DON'T EVEN INCLUDE
#ifdef DELETE // WinNT.h
#undef DELETE
#endif
#ifdef HELP_KEY // WinUser.h
#undef HELP_KEY
#endif

enum class SCANCODES : uint16_t
{
	SDL_SCANCODE_UNKNOWN = 0,
	PADDING_0 = 1,
	PADDING_1 = 2,
	PADDING_2 = 3,
	SDL_SCANCODE_A = 4,
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
	SDL_SCANCODE_INSERT = 73,
	SDL_SCANCODE_HOME = 74,
	SDL_SCANCODE_PAGEUP = 75,
	SDL_SCANCODE_DELETE = 76,
	SDL_SCANCODE_END = 77,
	SDL_SCANCODE_PAGEDOWN = 78,
	SDL_SCANCODE_RIGHT = 79,
	SDL_SCANCODE_LEFT = 80,
	SDL_SCANCODE_DOWN = 81,
	SDL_SCANCODE_UP = 82,
	SDL_SCANCODE_NUMLOCKCLEAR = 83,
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
	SDL_SCANCODE_APPLICATION = 101, 
	SDL_SCANCODE_POWER = 102,
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
	SDL_SCANCODE_AGAIN = 121,
	SDL_SCANCODE_UNDO = 122,
	SDL_SCANCODE_CUT = 123,
	SDL_SCANCODE_COPY = 124,
	SDL_SCANCODE_PASTE = 125,
	SDL_SCANCODE_FIND = 126,
	SDL_SCANCODE_MUTE = 127,
	SDL_SCANCODE_VOLUMEUP = 128,
	SDL_SCANCODE_VOLUMEDOWN = 129,
	SDL_SCANCODE_LOCKINGCAPSLOCK = 130,
	SDL_SCANCODE_LOCKINGNUMLOCK = 131,
	SDL_SCANCODE_LOCKINGSCROLLLOCK = 132,
	SDL_SCANCODE_KP_COMMA = 133,
	SDL_SCANCODE_KP_EQUALSAS400 = 134,
	SDL_SCANCODE_INTERNATIONAL1 = 135,
	SDL_SCANCODE_INTERNATIONAL2 = 136,
	SDL_SCANCODE_INTERNATIONAL3 = 137,
	SDL_SCANCODE_INTERNATIONAL4 = 138,
	SDL_SCANCODE_INTERNATIONAL5 = 139,
	SDL_SCANCODE_INTERNATIONAL6 = 140,
	SDL_SCANCODE_INTERNATIONAL7 = 141,
	SDL_SCANCODE_INTERNATIONAL8 = 142,
	SDL_SCANCODE_INTERNATIONAL9 = 143,
	SDL_SCANCODE_LANG1 = 144,
	SDL_SCANCODE_LANG2 = 145,
	SDL_SCANCODE_LANG3 = 146,
	SDL_SCANCODE_LANG4 = 147,
	SDL_SCANCODE_LANG5 = 148,
	SDL_SCANCODE_LANG6 = 149,
	SDL_SCANCODE_LANG7 = 150,
	SDL_SCANCODE_LANG8 = 151,
	SDL_SCANCODE_LANG9 = 152,
	SDL_SCANCODE_ALTERASE = 153,
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
	SDL_SCANCODE_MODE = 257,
	SDL_SCANCODE_AUDIONEXT = 258,
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
	SDL_SCANCODE_BRIGHTNESSDOWN = 275,
	SDL_SCANCODE_BRIGHTNESSUP = 276,
	SDL_SCANCODE_DISPLAYSWITCH = 277,
	SDL_SCANCODE_KBDILLUMTOGGLE = 278,
	SDL_SCANCODE_KBDILLUMDOWN = 279,
	SDL_SCANCODE_KBDILLUMUP = 280,
	SDL_SCANCODE_EJECT = 281,
	SDL_SCANCODE_SLEEP = 282,
	SDL_SCANCODE_APP1 = 283,
	SDL_SCANCODE_APP2 = 284,
	SDL_SCANCODE_AUDIOREWIND = 285,
	SDL_SCANCODE_AUDIOFASTFORWARD = 286,
	SDL_SCANCODE_COUNT,

	SDL_NUM_SCANCODES = 512 // not a key, just marks the number of scancodes for array bounds
};

enum class Keys : uint16_t
{
	// SCANCODE 4-49
	KEY_A,
	KEY_B,
	KEY_C,
	KEY_D,
	KEY_E,
	KEY_F,
	KEY_G,
	KEY_H,
	KEY_I,
	KEY_J,
	KEY_K,
	KEY_L,
	KEY_M,
	KEY_N,
	KEY_O,
	KEY_P,
	KEY_Q,
	KEY_R,
	KEY_S,
	KEY_T,
	KEY_U,
	KEY_V,
	KEY_W,
	KEY_X,
	KEY_Y,
	KEY_Z,
	KEY_1,
	KEY_2,
	KEY_3,
	KEY_4,
	KEY_5,
	KEY_6,
	KEY_7,
	KEY_8,
	KEY_9,
	KEY_0,
	ENTER,
	ESCAPE,
	BACKSPACE,
	TAB,
	SPACE,
	MINUS,
	EQUALS,
	LEFTBRACKET,
	RIGHTBRACKET,
	BACKSLASH,

	// SCANCODE 51-99
	SEMICOLON,
	APOSTROPHE,
	GRAVE,
	COMMA,
	PERIOD,
	SLASH,
	CAPSLOCK,
	F1,
	F2,
	F3,
	F4,
	F5,
	F6,
	F7,
	F8,
	F9,
	F10,
	F11,
	F12,
	PRINTSCREEN,
	SCROLLLOCK,
	PAUSE,
	INSERT,
	HOME,
	PAGEUP,
	DELETE,
	END,
	PAGEDOWN,
	RIGHT,
	LEFT,
	DOWN,
	UP,
	NUMLOCK,
	KP_DIVIDE,
	KP_MULTIPLY,
	KP_MINUS,
	KP_PLUS,
	KP_ENTER,
	KP_1,
	KP_2,
	KP_3,
	KP_4,
	KP_5,
	KP_6,
	KP_7,
	KP_8,
	KP_9,
	KP_0,
	KP_PERIOD,

	// SCANCODE 224-226
	LCTRL,
	LSHIFT,
	LALT,

	// SCANCODE 228-229
	RCTRL,
	RSHIFT,
	RALT,

	count,

	// ALIAS
	//NUMLOCKCLEAR = NUMLOCK,
};

constexpr uint8_t SCANCODE_OFFSET{4};
//constexpr uint16_t SEMICOLON_OFFSET{KEY_A_OFFSET + 1};
//constexpr uint16_t LMOD_OFFSET{224};
//constexpr uint16_t RMOD_OFFSET{226};

class Keybind
{
	::std::map<Keys, ::std::any> Keybinds;
public:

	auto bind(Keys i)
	{
		//return [](){ return do_action; };
		//return actions[i];
	}

	Keybind()
	{
		// MAP Keys to Functions that perform Actions
		// Start with compile time defaults, then support runtime user defined saved changes
		//::std::array<Keys, 80> KYS;
		//for(Keys i{0}; i < Keys::count;)
		//{
			//Keybinds.emplace(i, Keybind(i));
		//}
	}
};

struct KeyNames
{
	Keys key;
	// array.size = 16 - sizeof(Keys)
	::std::array<const char, 14> name;
};

constexpr ::std::array<KeyNames, ::std::size_t(Keys::count)> key_names{
	// 4-49
	KeyNames{ .key{Keys::KEY_A},		.name{"a"}				},		KeyNames{ .key{Keys::KEY_B},		.name{"b"}				},
	KeyNames{ .key{Keys::KEY_C}, 		.name{"c"}				},		KeyNames{ .key{Keys::KEY_D},		.name{"d"}				},
	KeyNames{ .key{Keys::KEY_E},		.name{"e"}				},		KeyNames{ .key{Keys::KEY_F},		.name{"f"}				},
	KeyNames{ .key{Keys::KEY_G},		.name{"g"}				},		KeyNames{ .key{Keys::KEY_H},		.name{"h"}				},
	KeyNames{ .key{Keys::KEY_I},		.name{"i"}				},		KeyNames{ .key{Keys::KEY_J},		.name{"j"}				},
	KeyNames{ .key{Keys::KEY_K},		.name{"k"}				},		KeyNames{ .key{Keys::KEY_L},		.name{"l"}				},
	KeyNames{ .key{Keys::KEY_M},		.name{"m"}				},		KeyNames{ .key{Keys::KEY_N},		.name{"n"}				},
	KeyNames{ .key{Keys::KEY_O},		.name{"o"}				},		KeyNames{ .key{Keys::KEY_P},		.name{"p"}				},
	KeyNames{ .key{Keys::KEY_Q},		.name{"q"}				},		KeyNames{ .key{Keys::KEY_R},		.name{"r"}				},
	KeyNames{ .key{Keys::KEY_S},		.name{"s"}				},		KeyNames{ .key{Keys::KEY_T},		.name{"t"}				},
	KeyNames{ .key{Keys::KEY_U},		.name{"u"}				},		KeyNames{ .key{Keys::KEY_V},		.name{"v"}				},
	KeyNames{ .key{Keys::KEY_W},		.name{"w"}				},		KeyNames{ .key{Keys::KEY_X},		.name{"x"}				},
	KeyNames{ .key{Keys::KEY_Y},		.name{"y"}				},		KeyNames{ .key{Keys::KEY_Z},		.name{"z"}				},
	KeyNames{ .key{Keys::KEY_1},		.name{"1"}				},		KeyNames{ .key{Keys::KEY_2},		.name{"2"}				},
	KeyNames{ .key{Keys::KEY_3},		.name{"3"}				},		KeyNames{ .key{Keys::KEY_4},		.name{"4"}				},
	KeyNames{ .key{Keys::KEY_5},		.name{"5"}				},		KeyNames{ .key{Keys::KEY_6},		.name{"6"}				},
	KeyNames{ .key{Keys::KEY_7},		.name{"7"}				},		KeyNames{ .key{Keys::KEY_8},		.name{"8"}				},
	KeyNames{ .key{Keys::KEY_9},		.name{"9"}				},		KeyNames{ .key{Keys::KEY_0},		.name{"0"}				},
	KeyNames{ .key{Keys::ENTER},		.name{"ENTER"}			},		KeyNames{ .key{Keys::ESCAPE},		.name{"ESCAPE"}			},
	KeyNames{ .key{Keys::BACKSPACE},	.name{"BACKSPACE"}		},		KeyNames{ .key{Keys::TAB},			.name{"TAB"}			},
	KeyNames{ .key{Keys::SPACE},		.name{"SPACE"}			},		KeyNames{ .key{Keys::MINUS},		.name{"-"}				},
	KeyNames{ .key{Keys::EQUALS},		.name{"=" }				},		KeyNames{ .key{Keys::LEFTBRACKET},	.name{"["}				},
	KeyNames{ .key{Keys::RIGHTBRACKET},	.name{"]"}				},		KeyNames{ .key{Keys::BACKSLASH},	.name{"\\"}				},
	// 51-99
	KeyNames{ .key{Keys::SEMICOLON},	.name{";"}				},		KeyNames{ .key{Keys::APOSTROPHE},	.name{"\'"}				},
	KeyNames{ .key{Keys::GRAVE},		.name{"`"}				},		KeyNames{ .key{Keys::COMMA},		.name{","}				},
	KeyNames{ .key{Keys::PERIOD},		.name{"."}				},		KeyNames{ .key{Keys::SLASH},		.name{"/"}				},
	KeyNames{ .key{Keys::CAPSLOCK},		.name{"CAPSLOCK"}		},		KeyNames{ .key{Keys::F1},			.name{"F1"}				},
	KeyNames{ .key{Keys::F2},			.name{"F2"}				},		KeyNames{ .key{Keys::F3},			.name{"F3"}				},
	KeyNames{ .key{Keys::F4},			.name{"F4"}				},		KeyNames{ .key{Keys::F5},			.name{"F5"}				},
	KeyNames{ .key{Keys::F6},			.name{"F6"}				},		KeyNames{ .key{Keys::F7},			.name{"F7"}				},
	KeyNames{ .key{Keys::F8},			.name{"F8"}				},		KeyNames{ .key{Keys::F9},			.name{"F9"}				},
	KeyNames{ .key{Keys::F10},			.name{"F10"}			},		KeyNames{ .key{Keys::F11},			.name{"F11"}			},
	KeyNames{ .key{Keys::F12},			.name{"F12"}			},		KeyNames{ .key{Keys::PRINTSCREEN},	.name{"PRINTSCREEN"}	},
	KeyNames{ .key{Keys::SCROLLLOCK},	.name{"SCROLLLOCK"}		},		KeyNames{ .key{Keys::PAUSE},		.name{"PAUSE"}			},
	KeyNames{ .key{Keys::INSERT},		.name{"INSERT"}			},		KeyNames{ .key{Keys::HOME},			.name{"HOME"}			},
	KeyNames{ .key{Keys::PAGEUP},		.name{"PAGEUP"}			},		KeyNames{ .key{Keys::DELETE},		.name{"DELETE"}			},
	KeyNames{ .key{Keys::END},			.name{"END"}			},		KeyNames{ .key{Keys::PAGEDOWN},		.name{"PAGEDOWN"}		},
	KeyNames{ .key{Keys::RIGHT},		.name{"RIGHT"}			},		KeyNames{ .key{Keys::LEFT},			.name{"LEFT"}			},
	KeyNames{ .key{Keys::DOWN},			.name{"DOWN"}			},		KeyNames{ .key{Keys::UP},			.name{"UP"}				},
	KeyNames{ .key{Keys::NUMLOCK},		.name{"NUMLOCK"}		},		KeyNames{ .key{Keys::KP_DIVIDE},	.name{"NUM /"}			},
	KeyNames{ .key{Keys::KP_MULTIPLY},	.name{"NUM *"}			},		KeyNames{ .key{Keys::KP_MINUS},		.name{"NUM -"}			},
	KeyNames{ .key{Keys::KP_PLUS},		.name{"NUM +"}			},		KeyNames{ .key{Keys::KP_ENTER},		.name{"NUM ENTER"}		},
	KeyNames{ .key{Keys::KP_1},			.name{"NUM 1"}			},		KeyNames{ .key{Keys::KP_2},			.name{"NUM 2"}			},
	KeyNames{ .key{Keys::KP_3},			.name{"NUM 3"}			},		KeyNames{ .key{Keys::KP_4},			.name{"NUM 4"}			},
	KeyNames{ .key{Keys::KP_5},			.name{"NUM 5"}			},		KeyNames{ .key{Keys::KP_6},			.name{"NUM 6"}			},
	KeyNames{ .key{Keys::KP_7},			.name{"NUM 7"}			},		KeyNames{ .key{Keys::KP_8},			.name{"NUM 8"}			},
	KeyNames{ .key{Keys::KP_9},			.name{"NUM 9"}			},		KeyNames{ .key{Keys::KP_0},			.name{"NUM 0"}			},
	KeyNames{ .key{Keys::KP_PERIOD},	.name{"NUM ."}			},
	// SCANCODE 224-226
	KeyNames{ .key{Keys::LCTRL},		.name{"LCTRL"}			},
	KeyNames{ .key{Keys::LSHIFT},		.name{"LSHIFT"}			},
	KeyNames{ .key{Keys::LALT},			.name{"LALT"}			},
	// SCANCODE 228-229
	KeyNames{ .key{Keys::RCTRL},		.name{"RCTRL"}			},
	KeyNames{ .key{Keys::RSHIFT},		.name{"RSHIFT"}			},
	KeyNames{ .key{Keys::RALT},			.name{"RALT"}			},
};

constexpr ::std::array<KeyNames, ::std::size_t(Keys::count)> s_key_names{
	// 4-49
	KeyNames{ .key{Keys::KEY_A},		.name{"A"}				},		KeyNames{ .key{Keys::KEY_B},		.name{"B"}				},
	KeyNames{ .key{Keys::KEY_C}, 		.name{"C"}				},		KeyNames{ .key{Keys::KEY_D},		.name{"D"}				},
	KeyNames{ .key{Keys::KEY_E},		.name{"E"}				},		KeyNames{ .key{Keys::KEY_F},		.name{"F"}				},
	KeyNames{ .key{Keys::KEY_G},		.name{"G"}				},		KeyNames{ .key{Keys::KEY_H},		.name{"H"}				},
	KeyNames{ .key{Keys::KEY_I},		.name{"I"}				},		KeyNames{ .key{Keys::KEY_J},		.name{"J"}				},
	KeyNames{ .key{Keys::KEY_K},		.name{"K"}				},		KeyNames{ .key{Keys::KEY_L},		.name{"L"}				},
	KeyNames{ .key{Keys::KEY_M},		.name{"M"}				},		KeyNames{ .key{Keys::KEY_N},		.name{"N"}				},
	KeyNames{ .key{Keys::KEY_O},		.name{"O"}				},		KeyNames{ .key{Keys::KEY_P},		.name{"P"}				},
	KeyNames{ .key{Keys::KEY_Q},		.name{"Q"}				},		KeyNames{ .key{Keys::KEY_R},		.name{"R"}				},
	KeyNames{ .key{Keys::KEY_S},		.name{"S"}				},		KeyNames{ .key{Keys::KEY_T},		.name{"T"}				},
	KeyNames{ .key{Keys::KEY_U},		.name{"U"}				},		KeyNames{ .key{Keys::KEY_V},		.name{"V"}				},
	KeyNames{ .key{Keys::KEY_W},		.name{"W"}				},		KeyNames{ .key{Keys::KEY_X},		.name{"X"}				},
	KeyNames{ .key{Keys::KEY_Y},		.name{"Y"}				},		KeyNames{ .key{Keys::KEY_Z},		.name{"Z"}				},
	KeyNames{ .key{Keys::KEY_1},		.name{"!"}				},		KeyNames{ .key{Keys::KEY_2},		.name{"@"}				},
	KeyNames{ .key{Keys::KEY_3},		.name{"#"}				},		KeyNames{ .key{Keys::KEY_4},		.name{"$"}				},
	KeyNames{ .key{Keys::KEY_5},		.name{"%"}				},		KeyNames{ .key{Keys::KEY_6},		.name{"^"}				},
	KeyNames{ .key{Keys::KEY_7},		.name{"&"}				},		KeyNames{ .key{Keys::KEY_8},		.name{"*"}				},
	KeyNames{ .key{Keys::KEY_9},		.name{"("}				},		KeyNames{ .key{Keys::KEY_0},		.name{")"}				},
	//KeyNames{ .key{Keys::ENTER},		.name{"ENTER"}			},		KeyNames{ .key{Keys::ESCAPE},		.name{"ESCAPE"}			},
	//KeyNames{ .key{Keys::BACKSPACE},	.name{"BACKSPACE"}		},		KeyNames{ .key{Keys::TAB},			.name{"TAB"}			},
	//KeyNames{ .key{Keys::SPACE},		.name{"SPACE"}			},
	KeyNames{ .key{Keys::MINUS},		.name{"_"}				},
	KeyNames{ .key{Keys::EQUALS},		.name{"+" }				},		KeyNames{ .key{Keys::LEFTBRACKET},	.name{"{"}				},
	KeyNames{ .key{Keys::RIGHTBRACKET},	.name{"}"}				},		KeyNames{ .key{Keys::BACKSLASH},	.name{"|"}				},
	// 51-99
	KeyNames{ .key{Keys::SEMICOLON},	.name{":"}				},		KeyNames{ .key{Keys::APOSTROPHE},	.name{"\""}				},
	KeyNames{ .key{Keys::GRAVE},		.name{"~"}				},		KeyNames{ .key{Keys::COMMA},		.name{"<"}				},
	KeyNames{ .key{Keys::PERIOD},		.name{">"}				},		KeyNames{ .key{Keys::SLASH},		.name{"?"}				},
	//KeyNames{ .key{Keys::CAPSLOCK},		.name{"CAPSLOCK"}		},		KeyNames{ .key{Keys::F1},			.name{"F1"}				},
	//KeyNames{ .key{Keys::F2},			.name{"F2"}				},		KeyNames{ .key{Keys::F3},			.name{"F3"}				},
	//KeyNames{ .key{Keys::F4},			.name{"F4"}				},		KeyNames{ .key{Keys::F5},			.name{"F5"}				},
	//KeyNames{ .key{Keys::F6},			.name{"F6"}				},		KeyNames{ .key{Keys::F7},			.name{"F7"}				},
	//KeyNames{ .key{Keys::F8},			.name{"F8"}				},		KeyNames{ .key{Keys::F9},			.name{"F9"}				},
	//KeyNames{ .key{Keys::F10},			.name{"F10"}			},		KeyNames{ .key{Keys::F11},			.name{"F11"}			},
	//KeyNames{ .key{Keys::F12},			.name{"F12"}			},
	KeyNames{ .key{Keys::PRINTSCREEN},	.name{"SYSREQ"}			}, // SDL_SCANCODE_SYSREQ = 154,
	//KeyNames{ .key{Keys::SCROLLLOCK},	.name{"SCROLLLOCK"}		},		KeyNames{ .key{Keys::PAUSE},		.name{"PAUSE"}			},
	//KeyNames{ .key{Keys::INSERT},		.name{"INSERT"}			},		KeyNames{ .key{Keys::HOME},			.name{"HOME"}			},
	//KeyNames{ .key{Keys::PAGEUP},		.name{"PAGEUP"}			},		KeyNames{ .key{Keys::DELETE},		.name{"DELETE"}			},
	//KeyNames{ .key{Keys::END},			.name{"END"}			},		KeyNames{ .key{Keys::PAGEDOWN},		.name{"PAGEDOWN"}		},
	//KeyNames{ .key{Keys::RIGHT},		.name{"RIGHT"}			},		KeyNames{ .key{Keys::LEFT},			.name{"LEFT"}			},
	//KeyNames{ .key{Keys::DOWN},			.name{"DOWN"}			},		KeyNames{ .key{Keys::UP},			.name{"UP"}				},
	//KeyNames{ .key{Keys::NUMLOCK},		.name{"NUMLOCK"}		},
	//KeyNames{ .key{Keys::KP_DIVIDE},	.name{"NUM /"}			},
	//KeyNames{ .key{Keys::KP_MULTIPLY},	.name{"NUM *"}			},		KeyNames{ .key{Keys::KP_MINUS},		.name{"NUM -"}			},
	//KeyNames{ .key{Keys::KP_PLUS},		.name{"NUM +"}			},		KeyNames{ .key{Keys::KP_ENTER},		.name{"NUM ENTER"}		},
	//KeyNames{ .key{Keys::KP_1},			.name{"NUM 1"}			},		KeyNames{ .key{Keys::KP_2},			.name{"NUM 2"}			},
	//KeyNames{ .key{Keys::KP_3},			.name{"NUM 3"}			},		KeyNames{ .key{Keys::KP_4},			.name{"NUM 4"}			},
	//KeyNames{ .key{Keys::KP_5},			.name{"NUM 5"}			},		KeyNames{ .key{Keys::KP_6},			.name{"NUM 6"}			},
	//KeyNames{ .key{Keys::KP_7},			.name{"NUM 7"}			},		KeyNames{ .key{Keys::KP_8},			.name{"NUM 8"}			},
	//KeyNames{ .key{Keys::KP_9},			.name{"NUM 9"}			},		KeyNames{ .key{Keys::KP_0},			.name{"NUM 0"}			},
	//KeyNames{ .key{Keys::KP_PERIOD},	.name{"NUM ."}			},
	// SCANCODE 224-226
	//KeyNames{ .key{Keys::LCTRL},		.name{"LCTRL"}			},
	//KeyNames{ .key{Keys::LSHIFT},		.name{"LSHIFT"}			},
	//KeyNames{ .key{Keys::LALT},			.name{"LALT"}			},
	// SCANCODE 228-229
	//KeyNames{ .key{Keys::RCTRL},		.name{"RCTRL"}			},
	//KeyNames{ .key{Keys::RSHIFT},		.name{"RSHIFT"}			},
	//KeyNames{ .key{Keys::RALT},			.name{"RALT"}			},
};

// Scancode notes:
// https://www.win.tue.nl/~aeb/linux/kbd/scancodes-1.html https://www.win.tue.nl/~aeb/linux/kbd/scancodes.html
// https://en.wikipedia.org/wiki/Scancode
// https://wiki.libsdl.org/SDL_Scancode
//
//

// old keys
/*
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
/**/
