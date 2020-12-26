/**********************************************************************************************************************************************\
	Copyright(C) 1993-1996 Id Software, Inc.
	Copyright(C) 1993-2008 Raven Software
	Copyright(C) 2005-2014 Simon Howard

	This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License
	as published by the Free Software Foundation; either version 2 of the License, or (at your option) any later version.

	This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
\**********************************************************************************************************************************************/

#include "h2def.h"
#include "s_sound.h"
#include "i_system.h"
#include "i_video.h"
#include "m_misc.h"
#include "p_local.h"
#include "v_video.h"
#include "i_swap.h"

// MACROS ------------------------------------------------------------------

#define	TEXTSPEED 3
#define	TEXTWAIT 140

// TYPES -------------------------------------------------------------------

enum class gametype_t
{
	SINGLE,
	COOPERATIVE,
	DEATHMATCH
};

// EXTERNAL FUNCTION PROTOTYPES --------------------------------------------

// PUBLIC FUNCTION PROTOTYPES ----------------------------------------------

// PRIVATE FUNCTION PROTOTYPES ---------------------------------------------

static void WaitStop();
static void Stop();
static void LoadPics();
static void UnloadPics();
static void CheckForSkip();
static void InitStats();
static void DrDeathTally();
static void DrNumber(int val, int x, int y, int wrapThresh);
static void DrNumberBold(int val, int x, int y, int wrapThresh);
static void DrawHubText();

// EXTERNAL DATA DECLARATIONS ----------------------------------------------

// PUBLIC DATA DECLARATIONS ------------------------------------------------

bool intermission;
char ClusterMessage[MAX_INTRMSN_MESSAGE_SIZE];

// PRIVATE DATA DEFINITIONS ------------------------------------------------

static bool skipintermission;
static int interstate = 0;
static int intertime = -1;
static gametype_t gametype;
static int cnt;
static int slaughterboy;		// in DM, the player with the most kills
static patch_t *patchINTERPIC;
static patch_t *FontBNumbers[10];
static patch_t *FontBNegative;
static patch_t *FontBSlash;
static patch_t *FontBPercent;
static int FontABaseLump;
static int FontBLump;
static int FontBLumpBase;

static signed int totalFrags[MAX_PLAYERS];

static int HubCount;
static char *HubText;

// CODE --------------------------------------------------------------------

extern void AM_Stop();

void IN_Start()
{
	int i;
	I_SetPalette(W_CacheLumpName("PLAYPAL", pu_tags_t::PU_CACHE));
	InitStats();
	LoadPics();
	intermission = true;
	interstate = 0;
	skipintermission = false;
	intertime = 0;
	AM_Stop();
	for (i = 0; i < maxplayers; i++)
	{
		players[i].messageTics = 0;
		players[i].message[0] = 0;
	}
	SN_StopAllSequences();
}

void WaitStop()
{
	if (!--cnt)
	{
		Stop();
//				gamestate = GameState_t::GS_LEVEL;
//				G_DoLoadLevel();
		gameaction = GameAction_t::ga_leavemap;
//				G_WorldDone();
	}
}

static void Stop()
{
	intermission = false;
	UnloadPics();
	SB_state = -1;
	BorderNeedRefresh = true;
}

//========================================================================
// InitStats
//		Initializes the stats for single player mode
//========================================================================
static const char* ClusMsgLumpNames[] = {
	"clus1msg",
	"clus2msg",
	"clus3msg",
	"clus4msg",
	"clus5msg"
};

static void InitStats()
{
	extern int LeaveMap;

	if (!deathmatch)
	{
		gametype = gametype_t::SINGLE;
		HubCount = 0;
		auto oldCluster = P_GetMapCluster(gamemap);
		if (oldCluster != P_GetMapCluster(LeaveMap))
		{
			if (oldCluster >= 1 && oldCluster <= 5)
			{
				const char* msgLumpName = ClusMsgLumpNames[oldCluster - 1];
				auto msgLump = W_GetNumForName(msgLumpName);
				auto msgSize = W_LumpLength(msgLump);
				if (msgSize >= MAX_INTRMSN_MESSAGE_SIZE)
				{
					I_Error("Cluster message too long (%s)", msgLumpName);
				}
				W_ReadLump(msgLump, ClusterMessage);
				ClusterMessage[msgSize] = 0;	// Append terminator
				HubText = ClusterMessage;
				HubCount = strlen(HubText) * TEXTSPEED + TEXTWAIT;
				S_StartSongName("hub", true);
			}
		}
	}
	else
	{
		gametype = gametype_t::DEATHMATCH;
		slaughterboy = 0;
		auto slaughterfrags = -9999;
		auto posnum = 0;
		auto playercount = 0;
		auto slaughtercount = 0;
		for (auto i{0}; i < maxplayers; ++i)
		{
			totalFrags[i] = 0;
			if (playeringame[i])
			{
				++playercount;
				for (auto j{0}; j < maxplayers; ++j)
				{
					if (playeringame[j])
					{
						totalFrags[i] += players[i].frags[j];
					}
				}
				++posnum;
			}
			if (totalFrags[i] > slaughterfrags)
			{
				slaughterboy = 1 << i;
				slaughterfrags = totalFrags[i];
				slaughtercount = 1;
			}
			else if (totalFrags[i] == slaughterfrags)
			{
				slaughterboy |= 1 << i;
				++slaughtercount;
			}
		}
		if (playercount == slaughtercount)
		{
			// don't do the slaughter stuff if everyone is equal
			slaughterboy = 0;
		}
		S_StartSongName("hub", true);
	}
}

static void LoadPics()
{
	if (HubCount || gametype == DEATHMATCH)
	{
		patchINTERPIC = W_CacheLumpName("INTERPIC", pu_tags_t::PU_STATIC);
		FontBLumpBase = W_GetNumForName("FONTB16");
		for (auto i{0}; i < 10; ++i)
		{
			FontBNumbers[i] = W_CacheLumpNum(FontBLumpBase + i, pu_tags_t::PU_STATIC);
		}
		FontBLump = W_GetNumForName("FONTB_S") + 1;
		FontBNegative = W_CacheLumpName("FONTB13", pu_tags_t::PU_STATIC);
		FontABaseLump = W_GetNumForName("FONTA_S") + 1;

		FontBSlash = W_CacheLumpName("FONTB15", pu_tags_t::PU_STATIC);
		FontBPercent = W_CacheLumpName("FONTB05", pu_tags_t::PU_STATIC);
	}
}

static void UnloadPics()
{
	if (HubCount || gametype == gametype_t::DEATHMATCH)
	{
		W_ReleaseLumpName("INTERPIC");

		patchINTERPIC = W_CacheLumpName("INTERPIC", pu_tags_t::PU_STATIC);
		FontBLumpBase = W_GetNumForName("FONTB16");
		for (auto i{0}; i < 10; ++i)
		{
			W_ReleaseLumpNum(FontBLumpBase + i);
		}
		W_ReleaseLumpName("FONTB13");
		W_ReleaseLumpName("FONTB15");
		W_ReleaseLumpName("FONTB05");
	}
}

void IN_Ticker()
{
	if (!intermission)
	{
		return;
	}
	if (interstate)
	{
		WaitStop();
		return;
	}
	skipintermission = false;
	CheckForSkip();
	++intertime;
	if (skipintermission || (gametype == gametype_t::SINGLE && !HubCount))
	{
		interstate = 1;
		cnt = 10;
		skipintermission = false;
		//S_StartSound(NULL, sfx_dorcls);
	}
}

//========================================================================
// CheckForSkip
//		Check to see if any player hit a key
//========================================================================
static void CheckForSkip()
{
	static bool triedToSkip;

	for (auto i{0}; i < maxplayers; ++i)
	{
		auto& player = players[i];
		if (playeringame[i])
		{
			if (player.cmd.buttons & BT_ATTACK)
			{
				if (!player.attackdown)
				{
					skipintermission = 1;
				}
				player.attackdown = true;
			}
			else
			{
				player.attackdown = false;
			}
			if (player.cmd.buttons & BT_USE)
			{
				if (!player.usedown)
				{
					skipintermission = 1;
				}
				player.usedown = true;
			}
			else
			{
				player.usedown = false;
			}
		}
	}

	if (deathmatch && intertime < 140)
	{
		// wait for 4 seconds before allowing a skip
		if (skipintermission == 1)
		{
			triedToSkip = true;
			skipintermission = 0;
		}
	}
	else
	{
		if (triedToSkip)
		{
			skipintermission = 1;
			triedToSkip = false;
		}
	}
}

void IN_Drawer()
{
	if (!intermission)
	{
		return;
	}

	if (interstate)
	{
		return;
	}

	UpdateState |= I_FULLSCRN;
	V_CopyScaledBuffer(I_VideoBuffer, (byte*) patchINTERPIC, ORIGWIDTH * ORIGHEIGHT);

	if (gametype == gametype_t::SINGLE)
	{
		if (HubCount)
		{
			DrawHubText();
		}
	}
	else
	{
		DrDeathTally();
	}
}

#define TALLY_EFFECT_TICKS 20
#define TALLY_FINAL_X_DELTA (23*FRACUNIT)
#define TALLY_FINAL_Y_DELTA (13*FRACUNIT)
#define TALLY_START_XPOS (178*FRACUNIT)
#define TALLY_STOP_XPOS (90*FRACUNIT)
#define TALLY_START_YPOS (132*FRACUNIT)
#define TALLY_STOP_YPOS (83*FRACUNIT)
#define TALLY_TOP_X 85
#define TALLY_TOP_Y 9
#define TALLY_LEFT_X 7
#define TALLY_LEFT_Y 71
#define TALLY_TOTALS_X 291

static void DrDeathTally()
{
	fixed_t yPos;
	fixed_t xDelta;
	fixed_t yDelta;
	fixed_t xStart;
	fixed_t scale;
	static bool showTotals;

	V_DrawPatch(TALLY_TOP_X, TALLY_TOP_Y, W_CacheLumpName("tallytop", pu_tags_t::PU_CACHE));
	V_DrawPatch(TALLY_LEFT_X, TALLY_LEFT_Y, W_CacheLumpName("tallylft", pu_tags_t::PU_CACHE));
	if (intertime < TALLY_EFFECT_TICKS)
	{
		showTotals = false;
		scale = (intertime * FRACUNIT) / TALLY_EFFECT_TICKS;
		xDelta = FixedMul(scale, TALLY_FINAL_X_DELTA);
		yDelta = FixedMul(scale, TALLY_FINAL_Y_DELTA);
		xStart = TALLY_START_XPOS - FixedMul(scale, TALLY_START_XPOS - TALLY_STOP_XPOS);
		yPos = TALLY_START_YPOS - FixedMul(scale, TALLY_START_YPOS - TALLY_STOP_YPOS);
	}
	else
	{
		xDelta = TALLY_FINAL_X_DELTA;
		yDelta = TALLY_FINAL_Y_DELTA;
		xStart = TALLY_STOP_XPOS;
		yPos = TALLY_STOP_YPOS;
	}
	if (intertime >= TALLY_EFFECT_TICKS && showTotals == false)
	{
		showTotals = true;
		S_StartSound(NULL, sfxenum_t::SFX_PLATFORM_STOP);
	}

	int y = yPos >> FRACBITS;
	for (auto i{0}; i < maxplayers; ++i)
	{
		fixed_t xPos = xStart;
		for (auto j{0}; j < maxplayers; ++j, xPos += xDelta)
		{
			int x = xPos >> FRACBITS;
			bool bold = (i == consoleplayer || j == consoleplayer);
			if (playeringame[i] && playeringame[j])
			{
				if (bold)
				{
					DrNumberBold(players[i].frags[j], x, y, 100);
				}
				else
				{
					DrNumber(players[i].frags[j], x, y, 100);
				}
			}
			else
			{
				int temp = MN_TextAWidth("--") / 2;
				if (bold)
				{
					MN_DrTextAYellow("--", x - temp, y);
				}
				else
				{
					MN_DrTextA("--", x - temp, y);
				}
			}
		}
		if (showTotals && playeringame[i] && !((slaughterboy & (1 << i)) && !(intertime & 16)))
		{
			DrNumber(totalFrags[i], TALLY_TOTALS_X, y, 1000);
		}
		yPos += yDelta;
		y = yPos >> FRACBITS;
	}
}

static void DrNumber(int val, int x, int y, int wrapThresh)
{
	char buff[8] = "XX";

	if (!(val < -9 && wrapThresh < 1000))
	{
		M_snprintf(buff, sizeof(buff), "%d", val >= wrapThresh ? val % wrapThresh : val);
	}
	MN_DrTextA(buff, x - MN_TextAWidth(buff) / 2, y);
}

static void DrNumberBold(int val, int x, int y, int wrapThresh)
{
	char buff[8] = "XX";

	if (!(val < -9 && wrapThresh < 1000))
	{
		M_snprintf(buff, sizeof(buff), "%d", val >= wrapThresh ? val % wrapThresh : val);
	}
	MN_DrTextAYellow(buff, x - MN_TextAWidth(buff) / 2, y);
}

static void DrawHubText()
{
	auto cy = 5;
	auto cx = 10;
	auto ch = HubText;

	int count{[]()
	{
		if (intertime <= 10)
		{
			return 0;
		}
		else
		{
			return (intertime - 10) / TEXTSPEED;
		}
	}()};

	for (; count; --count)
	{
		int c = *ch++;
		if (!c)
		{
			break;
		}

		if (c == '\n')
		{
			cx = 10;
			cy += 9;
			continue;
		}

		if (c < 32)
		{
			continue;
		}

		c = toupper(c);
		if (c == 32)
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
