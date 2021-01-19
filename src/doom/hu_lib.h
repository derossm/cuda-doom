/**********************************************************************************************************************************************\
	Copyright(C) 1993-1996 Id Software, Inc.
	Copyright(C) 2005-2014 Simon Howard

	This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License
	as published by the Free Software Foundation; either version 2 of the License, or (at your option) any later version.

	This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
\**********************************************************************************************************************************************/
#pragma once

#include "../../derma/common.h"

// We are referring to patches.
#include "r_defs.h"

// font stuff
constexpr size_t HU_CHARERASE{KEY_BACKSPACE};

constexpr size_t HU_MAXLINES{4};
constexpr size_t HU_MAXLINELENGTH{80};

// Text Line widget (parent of Scrolling Text and Input Text widgets)
struct hu_textline_t
{
	// left-justified position of scrolling text window
	int x;
	int y;

	patch_t** f;				// font
	int sc;						// start character
	char l[HU_MAXLINELENGTH + 1];	// line of text
	int len;					// current line length

	// whether this line needs to be udpated
	int needsupdate;
};

// Scrolling Text window widget
// (child of Text Line widget)
struct hu_stext_t
{
	hu_textline_t l[HU_MAXLINES];	// text lines to draw
	int h;							// height in lines
	int cl;							// current line number

	// pointer to bool stating whether to update window
	bool* on;
	bool laston;					// last value of *->on.
};

// Input Text Line widget (child of Text Line widget)
struct hu_itext_t
{
	hu_textline_t l;	// text line to input on

	// left margin past which I am not to delete characters
	int lm;

	// pointer to bool stating whether to update window
	bool* on;
	bool laston;		// last value of *->on;
};

// initializes heads-up widget library
void HUlib_init();

void HUlib_clearTextLine(hu_textline_t* t);

void HUlib_initTextLine(hu_textline_t* t, int x, int y, patch_t** f, int sc);

// returns success
bool HUlib_addCharToTextLine(hu_textline_t* t, char ch);

// returns success
bool HUlib_delCharFromTextLine(hu_textline_t* t);

void HUlib_drawTextLine(hu_textline_t* l, bool drawcursor);

void HUlib_eraseTextLine(hu_textline_t* l);

// ?
// TODO REMOVE THIS bool POINTER NONSENSE
void HUlib_initSText(hu_stext_t* s, int x, int y, int h, patch_t** font, int startchar, bool* on);

void HUlib_addLineToSText(hu_stext_t* s);

// ?
void HUlib_addMessageToSText(hu_stext_t* s, ::std::string prefix, ::std::string msg);

void HUlib_drawSText(hu_stext_t* s);

// erases all stext lines
void HUlib_eraseSText(hu_stext_t* s);

// Input Text Line widget routines
void HUlib_initIText(hu_itext_t* it, int x, int y, patch_t** font, int startchar, bool* on);

// enforces left margin
void HUlib_delCharFromIText(hu_itext_t* it);

// enforces left margin
void HUlib_eraseLineFromIText(hu_itext_t* it);

// resets line and left margin
void HUlib_resetIText(hu_itext_t* it);

// left of left-margin
void HUlib_addPrefixToIText(hu_itext_t* it, ::std::string str);

// whether eaten
bool HUlib_keyInIText(hu_itext_t* it, unsigned char ch);

void HUlib_drawIText(hu_itext_t* it);

// erases all itext lines
void HUlib_eraseIText(hu_itext_t* it);
