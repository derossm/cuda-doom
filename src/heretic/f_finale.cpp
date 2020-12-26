/**********************************************************************************************************************************************\
	Copyright(C) 1993-1996 Id Software, Inc.
	Copyright(C) 1993-2008 Raven Software
	Copyright(C) 2005-2014 Simon Howard

	This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License
	as published by the Free Software Foundation; either version 2 of the License, or (at your option) any later version.

	This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
\**********************************************************************************************************************************************/


#include "doomdef.h"
#include "deh_str.h"
#include "i_swap.h"
#include "i_video.h"
#include "s_sound.h"
#include "v_video.h"

static int finalestage;				// 0 = text, 1 = art screen
static int finalecount;

#define TEXTSPEED		3
#define TEXTWAIT		250

static const char *finaletext;
static const char *finaleflat;

static int FontABaseLump;

extern bool automapactive;
extern bool viewactive;

extern void D_StartTitle();

void F_StartFinale()
{
	gameaction = ga_nothing;
	gamestate = GameState_t::GS_FINALE;
	viewactive = false;
	automapactive = false;
	players[consoleplayer].messageTics = 1;
	players[consoleplayer].message = NULL;
	players[consoleplayer].centerMessage = NULL;

	switch (gameepisode)
	{
		case 1:
			finaleflat = DEH_String("FLOOR25");
			finaletext = DEH_String(E1TEXT);
			break;
		case 2:
			finaleflat = DEH_String("FLATHUH1");
			finaletext = DEH_String(E2TEXT);
			break;
		case 3:
			finaleflat = DEH_String("FLTWAWA2");
			finaletext = DEH_String(E3TEXT);
			break;
		case 4:
			finaleflat = DEH_String("FLOOR28");
			finaletext = DEH_String(E4TEXT);
			break;
		case 5:
			finaleflat = DEH_String("FLOOR08");
			finaletext = DEH_String(E5TEXT);
			break;
	}

	finalestage = 0;
	finalecount = 0;
	FontABaseLump = W_GetNumForName(DEH_String("FONTA_S")) + 1;

	S_StartSong(mus_cptd, true);
}

bool F_Responder(event_t * event)
{
	if (event->type != ev_keydown)
	{
		return false;
	}
	if (finalestage == 1 && gameepisode == 2)
	{
		// we're showing the water pic, make any key kick to demo mode
		finalestage++;

		return true;
	}
	return false;
}

void F_Ticker()
{
	finalecount++;
	if (!finalestage
		&& finalecount > strlen(finaletext) * TEXTSPEED + TEXTWAIT)
	{
		finalecount = 0;
		if (!finalestage)
		{
			finalestage = 1;
		}
	}
}

void F_TextWrite()
{
	// erase the entire screen to a tiled background
	byte* src{W_CacheLumpName(finaleflat, pu_tags_t::PU_CACHE)};
	auto dest{I_VideoBuffer};
	for (size_t y{0}; y < SCREENHEIGHT; ++y)
	{
		for (size_t x{0}; x < SCREENWIDTH / 64; ++x)
		{
			memcpy(dest, src + ((y & 63) << 6), 64);
			dest += 64;
		}
		if (SCREENWIDTH & 63)
		{
			memcpy(dest, src + ((y & 63) << 6), SCREENWIDTH & 63);
			dest += (SCREENWIDTH & 63);
		}
	}

	if (finalecount <= 10)
	{
		return;
	}
	else
	{
		// draw some of the text onto the screen
		auto cx = 20;
		auto cy = 5;
		auto ch{finaletext};
		for (auto count{(finalecount - 10) / TEXTSPEED}; count > 0; --count)
		{
			auto c{*ch++};
			if (!c)
			{
				break;
			}
			else if (c == '\n')
			{
				cx = 20;
				cy += 9;
				continue;
			}

			c = toupper(c);
			if (c < 33)
			{
				cx += 5;
				continue;
			}

			patch_t* w = W_CacheLumpNum(FontABaseLump + c - 33, pu_tags_t::PU_CACHE);

			if (cx + SHORT(w->width) > SCREENWIDTH)
			{
				break;
			}

			V_DrawPatch(cx, cy, w);
			cx += SHORT(w->width);
		}
	}
}


void F_DrawPatchCol(int x, patch_t * patch, int col)
{
	column_t *column;
	byte *source, *dest, *desttop;
	int count;

	column = (column_t *) ((byte *) patch + LONG(patch->columnofs[col]));
	desttop = I_VideoBuffer + x;

// step through the posts in a column

	while (column->topdelta != 0xff)
	{
		source = (byte *) column + 3;
		dest = desttop + column->topdelta * SCREENWIDTH;
		count = column->length;

		while (count--)
		{
			*dest = *source++;
			dest += SCREENWIDTH;
		}
		column = (column_t *) ((byte *) column + column->length + 4);
	}
}

void F_DemonScroll()
{
	byte *p1, *p2;
	static int yval = 0;
	static int nextscroll = 0;

	if (finalecount < nextscroll)
	{
		return;
	}
	p1 = W_CacheLumpName(DEH_String("FINAL1"), pu_tags_t::PU_LEVEL);
	p2 = W_CacheLumpName(DEH_String("FINAL2"), pu_tags_t::PU_LEVEL);
	if (finalecount < 70)
	{
		V_CopyScaledBuffer(I_VideoBuffer, p1, ORIGHEIGHT * ORIGWIDTH);
		nextscroll = finalecount;
		return;
	}
	if (yval < 64000)
	{
		V_CopyScaledBuffer(I_VideoBuffer, p2 + ORIGHEIGHT * ORIGWIDTH - yval, yval);
		V_CopyScaledBuffer(I_VideoBuffer + (yval << (2 * crispy->hires)), p1, ORIGHEIGHT * ORIGWIDTH - yval);
		yval += ORIGWIDTH;
		nextscroll = finalecount + 3;
	}
	else
	{							//else, we'll just sit here and wait, for now
		V_CopyScaledBuffer(I_VideoBuffer, p2, ORIGWIDTH * ORIGHEIGHT);
	}
}

void F_DrawUnderwater()
{
	static bool underwawa = false;
	extern bool askforquit;
	const char *lumpname;
	byte *palette;

	// The underwater screen has its own palette, which is rather annoying.
	// The palette doesn't correspond to the normal palette. Because of
	// this, we must regenerate the lookup tables used in the video scaling
	// code.

	switch (finalestage)
	{
		case 1:
			if (!underwawa)
			{
				underwawa = true;
				V_DrawFilledBox(0, 0, SCREENWIDTH, SCREENHEIGHT, 0);
				lumpname = DEH_String("E2PAL");
				palette = W_CacheLumpName(lumpname, pu_tags_t::PU_STATIC);
				I_SetPalette(palette);
				W_ReleaseLumpName(lumpname);
				V_DrawRawScreen(W_CacheLumpName(DEH_String("E2END"), pu_tags_t::PU_CACHE));
			}
			paused = false;
			MenuActive = false;
			askforquit = false;

			break;
		case 2:
			if (underwawa)
			{
				lumpname = DEH_String("PLAYPAL");
				palette = W_CacheLumpName(lumpname, pu_tags_t::PU_STATIC);
				I_SetPalette(palette);
				W_ReleaseLumpName(lumpname);
				underwawa = false;
			}
			V_DrawRawScreen(W_CacheLumpName(DEH_String("TITLE"), pu_tags_t::PU_CACHE));
			//D_StartTitle(); // go to intro/demo mode.
	}
}


#if 0
void F_BunnyScroll()
{
	int scrolled, x;
	patch_t *p1, *p2;
	char name[10];
	int stage;
	static int laststage;

	p1 = W_CacheLumpName("PFUB2", pu_tags_t::PU_LEVEL);
	p2 = W_CacheLumpName("PFUB1", pu_tags_t::PU_LEVEL);

	V_MarkRect(0, 0, SCREENWIDTH, SCREENHEIGHT);

	scrolled = 320 - (finalecount - 230) / 2;
	if (scrolled > 320)
		scrolled = 320;
	if (scrolled < 0)
		scrolled = 0;

	for (x = 0; x < SCREENWIDTH; x++)
	{
		if (x + scrolled < 320)
			F_DrawPatchCol(x, p1, x + scrolled);
		else
			F_DrawPatchCol(x, p2, x + scrolled - 320);
	}

	if (finalecount < 1130)
		return;
	if (finalecount < 1180)
	{
		V_DrawPatch((SCREENWIDTH - 13 * 8) / 2, (SCREENHEIGHT - 8 * 8) / 2, 0,
					W_CacheLumpName("END0", pu_tags_t::PU_CACHE));
		laststage = 0;
		return;
	}

	stage = (finalecount - 1180) / 5;
	if (stage > 6)
		stage = 6;
	if (stage > laststage)
	{
		S_StartSound(NULL, sfx_pistol);
		laststage = stage;
	}

	M_snprintf(name, sizeof(name), "END%i", stage);
	V_DrawPatch((SCREENWIDTH - 13 * 8) / 2, (SCREENHEIGHT - 8 * 8) / 2,
				W_CacheLumpName(name, pu_tags_t::PU_CACHE));
}
#endif

void F_Drawer()
{
	UpdateState |= I_FULLSCRN;
	if (!finalestage)
		F_TextWrite();
	else
	{
		switch (gameepisode)
		{
			case 1:
				if (gamemode == GameMode_t::shareware)
				{
					V_DrawRawScreen(W_CacheLumpName("ORDER", pu_tags_t::PU_CACHE));
				}
				else
				{
					V_DrawRawScreen(W_CacheLumpName("CREDIT", pu_tags_t::PU_CACHE));
				}
				break;
			case 2:
				F_DrawUnderwater();
				break;
			case 3:
				F_DemonScroll();
				break;
			case 4:			// Just show credits screen for extended episodes
			case 5:
				V_DrawRawScreen(W_CacheLumpName("CREDIT", pu_tags_t::PU_CACHE));
				break;
		}
	}
}
