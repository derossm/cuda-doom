/**********************************************************************************************************************************************\
	Copyright© 2020-2021 Mason DeRoss

	Released under the GNU All-permissive License

	Copying and distribution of this file, with or without modification, are permitted in any medium without royalty,
	provided the copyright notice and this notice are preserved. This file is offered as-is, without any warranty.

	DESCRIPTION:

\**********************************************************************************************************************************************/
#pragma once

#include "stdafx.h"

enum class Keys : uint16_t
{
	null = 0, // wasteful for indexing, but allows !key to work for null
	// SCANCODE 4-49
	KEY_A, KEY_B, KEY_C, KEY_D, KEY_E, KEY_F, KEY_G, KEY_H, KEY_I, KEY_J, KEY_K, KEY_L, KEY_M,
	KEY_N, KEY_O, KEY_P, KEY_Q, KEY_R, KEY_S, KEY_T, KEY_U, KEY_V, KEY_W, KEY_X, KEY_Y, KEY_Z,
	KEY_1, KEY_2, KEY_3, KEY_4, KEY_5, KEY_6, KEY_7, KEY_8, KEY_9, KEY_0,
	ENTER, ESCAPE, BACKSPACE, TAB, SPACE, MINUS, EQUALS, LEFTBRACKET, RIGHTBRACKET, BACKSLASH,

	// SCANCODE 51-99
	SEMICOLON, APOSTROPHE, GRAVE, COMMA, PERIOD, SLASH, CAPSLOCK,
	F1, F2, F3, F4, F5, F6, F7, F8, F9, F10, F11, F12,
	PRINTSCREEN, SCROLLLOCK, PAUSE, INSERT, HOME, PAGEUP, DEL, END, PAGEDOWN,
	RIGHT, LEFT, DOWN, UP,
	NUMLOCK, KP_DIVIDE, KP_MULTIPLY, KP_MINUS, KP_PLUS, KP_ENTER,
	KP_1, KP_2, KP_3, KP_4, KP_5, KP_6, KP_7, KP_8, KP_9, KP_0,
	KP_PERIOD,

	// SCANCODE 224-226
	LCTRL, LSHIFT, LALT,

	// SCANCODE 228-229
	RCTRL, RSHIFT, RALT,

	count
};

struct KeyNames
{
	Keys key;
	::std::array<const char, 14> name;
};

constexpr ::std::array<KeyNames, ::std::size_t(Keys::count)> key_names{
	KeyNames{ .key{Keys::null},				.name{""}				},
	// 4-49
	KeyNames{ .key{Keys::KEY_A},			.name{"a"}				}, KeyNames{ .key{Keys::KEY_B},			.name{"b"}				},
	KeyNames{ .key{Keys::KEY_C}, 			.name{"c"}				}, KeyNames{ .key{Keys::KEY_D},			.name{"d"}				},
	KeyNames{ .key{Keys::KEY_E},			.name{"e"}				}, KeyNames{ .key{Keys::KEY_F},			.name{"f"}				},
	KeyNames{ .key{Keys::KEY_G},			.name{"g"}				}, KeyNames{ .key{Keys::KEY_H},			.name{"h"}				},
	KeyNames{ .key{Keys::KEY_I},			.name{"i"}				}, KeyNames{ .key{Keys::KEY_J},			.name{"j"}				},
	KeyNames{ .key{Keys::KEY_K},			.name{"k"}				}, KeyNames{ .key{Keys::KEY_L},			.name{"l"}				},
	KeyNames{ .key{Keys::KEY_M},			.name{"m"}				}, KeyNames{ .key{Keys::KEY_N},			.name{"n"}				},
	KeyNames{ .key{Keys::KEY_O},			.name{"o"}				}, KeyNames{ .key{Keys::KEY_P},			.name{"p"}				},
	KeyNames{ .key{Keys::KEY_Q},			.name{"q"}				}, KeyNames{ .key{Keys::KEY_R},			.name{"r"}				},
	KeyNames{ .key{Keys::KEY_S},			.name{"s"}				}, KeyNames{ .key{Keys::KEY_T},			.name{"t"}				},
	KeyNames{ .key{Keys::KEY_U},			.name{"u"}				}, KeyNames{ .key{Keys::KEY_V},			.name{"v"}				},
	KeyNames{ .key{Keys::KEY_W},			.name{"w"}				}, KeyNames{ .key{Keys::KEY_X},			.name{"x"}				},
	KeyNames{ .key{Keys::KEY_Y},			.name{"y"}				}, KeyNames{ .key{Keys::KEY_Z},			.name{"z"}				},
	KeyNames{ .key{Keys::KEY_1},			.name{"1"}				}, KeyNames{ .key{Keys::KEY_2},			.name{"2"}				},
	KeyNames{ .key{Keys::KEY_3},			.name{"3"}				}, KeyNames{ .key{Keys::KEY_4},			.name{"4"}				},
	KeyNames{ .key{Keys::KEY_5},			.name{"5"}				}, KeyNames{ .key{Keys::KEY_6},			.name{"6"}				},
	KeyNames{ .key{Keys::KEY_7},			.name{"7"}				}, KeyNames{ .key{Keys::KEY_8},			.name{"8"}				},
	KeyNames{ .key{Keys::KEY_9},			.name{"9"}				}, KeyNames{ .key{Keys::KEY_0},			.name{"0"}				},
	KeyNames{ .key{Keys::ENTER},			.name{"ENTER"}			}, KeyNames{ .key{Keys::ESCAPE},		.name{"ESCAPE"}			},
	KeyNames{ .key{Keys::BACKSPACE},		.name{"BACKSPACE"}		}, KeyNames{ .key{Keys::TAB},			.name{"TAB"}			},
	KeyNames{ .key{Keys::SPACE},			.name{"SPACE"}			}, KeyNames{ .key{Keys::MINUS},			.name{"-"}				},
	KeyNames{ .key{Keys::EQUALS},			.name{"=" }				}, KeyNames{ .key{Keys::LEFTBRACKET},	.name{"["}				},
	KeyNames{ .key{Keys::RIGHTBRACKET},		.name{"]"}				}, KeyNames{ .key{Keys::BACKSLASH},		.name{"\\"}				},
	// 51-99
	KeyNames{ .key{Keys::SEMICOLON},		.name{";"}				}, KeyNames{ .key{Keys::APOSTROPHE},	.name{"\'"}				},
	KeyNames{ .key{Keys::GRAVE},			.name{"`"}				}, KeyNames{ .key{Keys::COMMA},			.name{","}				},
	KeyNames{ .key{Keys::PERIOD},			.name{"."}				}, KeyNames{ .key{Keys::SLASH},			.name{"/"}				},
	KeyNames{ .key{Keys::CAPSLOCK},			.name{"CAPSLOCK"}		}, KeyNames{ .key{Keys::F1},			.name{"F1"}				},
	KeyNames{ .key{Keys::F2},				.name{"F2"}				}, KeyNames{ .key{Keys::F3},			.name{"F3"}				},
	KeyNames{ .key{Keys::F4},				.name{"F4"}				}, KeyNames{ .key{Keys::F5},			.name{"F5"}				},
	KeyNames{ .key{Keys::F6},				.name{"F6"}				}, KeyNames{ .key{Keys::F7},			.name{"F7"}				},
	KeyNames{ .key{Keys::F8},				.name{"F8"}				}, KeyNames{ .key{Keys::F9},			.name{"F9"}				},
	KeyNames{ .key{Keys::F10},				.name{"F10"}			}, KeyNames{ .key{Keys::F11},			.name{"F11"}			},
	KeyNames{ .key{Keys::F12},				.name{"F12"}			}, KeyNames{ .key{Keys::PRINTSCREEN},	.name{"PRINTSCREEN"}	},
	KeyNames{ .key{Keys::SCROLLLOCK},		.name{"SCROLLLOCK"}		}, KeyNames{ .key{Keys::PAUSE},			.name{"PAUSE"}			},
	KeyNames{ .key{Keys::INSERT},			.name{"INSERT"}			}, KeyNames{ .key{Keys::HOME},			.name{"HOME"}			},
	KeyNames{ .key{Keys::PAGEUP},			.name{"PAGEUP"}			}, KeyNames{ .key{Keys::DEL},			.name{"DELETE"}			},
	KeyNames{ .key{Keys::END},				.name{"END"}			}, KeyNames{ .key{Keys::PAGEDOWN},		.name{"PAGEDOWN"}		},
	KeyNames{ .key{Keys::RIGHT},			.name{"RIGHT"}			}, KeyNames{ .key{Keys::LEFT},			.name{"LEFT"}			},
	KeyNames{ .key{Keys::DOWN},				.name{"DOWN"}			}, KeyNames{ .key{Keys::UP},			.name{"UP"}				},
	KeyNames{ .key{Keys::NUMLOCK},			.name{"NUMLOCK"}		}, KeyNames{ .key{Keys::KP_DIVIDE},		.name{"NUM /"}			},
	KeyNames{ .key{Keys::KP_MULTIPLY},		.name{"NUM *"}			}, KeyNames{ .key{Keys::KP_MINUS},		.name{"NUM -"}			},
	KeyNames{ .key{Keys::KP_PLUS},			.name{"NUM +"}			}, KeyNames{ .key{Keys::KP_ENTER},		.name{"NUM ENTER"}		},
	KeyNames{ .key{Keys::KP_1},				.name{"NUM 1"}			}, KeyNames{ .key{Keys::KP_2},			.name{"NUM 2"}			},
	KeyNames{ .key{Keys::KP_3},				.name{"NUM 3"}			}, KeyNames{ .key{Keys::KP_4},			.name{"NUM 4"}			},
	KeyNames{ .key{Keys::KP_5},				.name{"NUM 5"}			}, KeyNames{ .key{Keys::KP_6},			.name{"NUM 6"}			},
	KeyNames{ .key{Keys::KP_7},				.name{"NUM 7"}			}, KeyNames{ .key{Keys::KP_8},			.name{"NUM 8"}			},
	KeyNames{ .key{Keys::KP_9},				.name{"NUM 9"}			}, KeyNames{ .key{Keys::KP_0},			.name{"NUM 0"}			},
	KeyNames{ .key{Keys::KP_PERIOD},		.name{"NUM ."}			},
	// SCANCODE 224-226
	KeyNames{ .key{Keys::LCTRL},			.name{"LCTRL"}			},
	KeyNames{ .key{Keys::LSHIFT},			.name{"LSHIFT"}			},
	KeyNames{ .key{Keys::LALT},				.name{"LALT"}			},
	// SCANCODE 228-229
	KeyNames{ .key{Keys::RCTRL},			.name{"RCTRL"}			},
	KeyNames{ .key{Keys::RSHIFT},			.name{"RSHIFT"}			},
	KeyNames{ .key{Keys::RALT},				.name{"RALT"}			},
};

constexpr ::std::array<KeyNames, ::std::size_t(Keys::SLASH) + 1> s_key_names{
	KeyNames{ .key{Keys::null},				.name{""}				},
	// 4-49
	KeyNames{ .key{Keys::KEY_A},			.name{"A"}				}, KeyNames{ .key{Keys::KEY_B},			.name{"B"}				},
	KeyNames{ .key{Keys::KEY_C}, 			.name{"C"}				}, KeyNames{ .key{Keys::KEY_D},			.name{"D"}				},
	KeyNames{ .key{Keys::KEY_E},			.name{"E"}				}, KeyNames{ .key{Keys::KEY_F},			.name{"F"}				},
	KeyNames{ .key{Keys::KEY_G},			.name{"G"}				}, KeyNames{ .key{Keys::KEY_H},			.name{"H"}				},
	KeyNames{ .key{Keys::KEY_I},			.name{"I"}				}, KeyNames{ .key{Keys::KEY_J},			.name{"J"}				},
	KeyNames{ .key{Keys::KEY_K},			.name{"K"}				}, KeyNames{ .key{Keys::KEY_L},			.name{"L"}				},
	KeyNames{ .key{Keys::KEY_M},			.name{"M"}				}, KeyNames{ .key{Keys::KEY_N},			.name{"N"}				},
	KeyNames{ .key{Keys::KEY_O},			.name{"O"}				}, KeyNames{ .key{Keys::KEY_P},			.name{"P"}				},
	KeyNames{ .key{Keys::KEY_Q},			.name{"Q"}				}, KeyNames{ .key{Keys::KEY_R},			.name{"R"}				},
	KeyNames{ .key{Keys::KEY_S},			.name{"S"}				}, KeyNames{ .key{Keys::KEY_T},			.name{"T"}				},
	KeyNames{ .key{Keys::KEY_U},			.name{"U"}				}, KeyNames{ .key{Keys::KEY_V},			.name{"V"}				},
	KeyNames{ .key{Keys::KEY_W},			.name{"W"}				}, KeyNames{ .key{Keys::KEY_X},			.name{"X"}				},
	KeyNames{ .key{Keys::KEY_Y},			.name{"Y"}				}, KeyNames{ .key{Keys::KEY_Z},			.name{"Z"}				},
	KeyNames{ .key{Keys::KEY_1},			.name{"!"}				}, KeyNames{ .key{Keys::KEY_2},			.name{"@"}				},
	KeyNames{ .key{Keys::KEY_3},			.name{"#"}				}, KeyNames{ .key{Keys::KEY_4},			.name{"$"}				},
	KeyNames{ .key{Keys::KEY_5},			.name{"%"}				}, KeyNames{ .key{Keys::KEY_6},			.name{"^"}				},
	KeyNames{ .key{Keys::KEY_7},			.name{"&"}				}, KeyNames{ .key{Keys::KEY_8},			.name{"*"}				},
	KeyNames{ .key{Keys::KEY_9},			.name{"("}				}, KeyNames{ .key{Keys::KEY_0},			.name{")"}				},
	// note: no shift change to the next 5 keys, but kept for maintaining index the same as non-shifted
	KeyNames{ .key{Keys::ENTER},			.name{"ENTER"}			}, KeyNames{ .key{Keys::ESCAPE},		.name{"ESCAPE"}			},
	KeyNames{ .key{Keys::BACKSPACE},		.name{"BACKSPACE"}		}, KeyNames{ .key{Keys::TAB},			.name{"TAB"}			},
	KeyNames{ .key{Keys::SPACE},			.name{"SPACE"}			}, KeyNames{ .key{Keys::MINUS},			.name{"_"}				},
	KeyNames{ .key{Keys::EQUALS},			.name{"+" }				}, KeyNames{ .key{Keys::LEFTBRACKET},	.name{"{"}				},
	KeyNames{ .key{Keys::RIGHTBRACKET},		.name{"}"}				}, KeyNames{ .key{Keys::BACKSLASH},		.name{"|"}				},
	// 51-99
	KeyNames{ .key{Keys::SEMICOLON},		.name{":"}				}, KeyNames{ .key{Keys::APOSTROPHE},	.name{"\""}				},
	KeyNames{ .key{Keys::GRAVE},			.name{"~"}				}, KeyNames{ .key{Keys::COMMA},			.name{"<"}				},
	KeyNames{ .key{Keys::PERIOD},			.name{">"}				}, KeyNames{ .key{Keys::SLASH},			.name{"?"}				},
};

// Scancode notes:
// https://www.win.tue.nl/~aeb/linux/kbd/scancodes.html
// https://www.win.tue.nl/~aeb/linux/kbd/scancodes-1.html
// https://wiki.libsdl.org/SDL_Scancode
// https://en.wikipedia.org/wiki/Scancode
