/**********************************************************************************************************************************************\
	Copyright(C) 1993-1996 Id Software, Inc.
	Copyright(C) 2005-2014 Simon Howard

	This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License
	as published by the Free Software Foundation; either version 2 of the License, or (at your option) any later version.

	This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

	DESCRIPTION:
		DOOM selection menu, options, episode etc. Sliders and icons. Kinda widget stuff.
\**********************************************************************************************************************************************/
#include "../../derma/stdafx.h"

#include "../disk_io/deh_main.h"
#include "../m_argv.h"
#include "../m_controls.h"

#include "../i_input.h"
#include "../i_swap.h"
#include "../i_system.h"
#include "../i_timer.h"
#include "../i_video.h"
#include "../m_misc.h"
#include "../v_video.h"
#include "../disk_io/w_wad.h"
#include "../z_zone.h"
#include "../v_trans.h" // colored "invert mouse" message

#include "doomdef.h"
#include "doomstat.h"
#include "dstrings.h"
#include "d_main.h"

#include "r_local.h"
#include "hu_stuff.h"
#include "g_game.h"
#include "p_saveg.h"
#include "p_setup.h"
#include "p_extsaveg.h" // savewadfilename
#include "s_sound.h"
#include "sounds.h"

#include "m_crispy.h" // Crispness menu
#include "m_background.h" // Crispness menu

#include "m_menu.h"

namespace cudadoom
{

extern patch_t* hu_font[HU_FONTSIZE];
extern bool message_dontfuckwithme;
extern bool chat_on; // in heads-up code
extern bool sendpause;
extern bool speedkeydown();

void (*messageRoutine)(int response);

// intermediate gamma levels
constexpr const char gammamsg[5 + 4][26 + 2]{
	GAMMALVL0,
	GAMMALVL05,
	GAMMALVL1,
	GAMMALVL15,
	GAMMALVL2,
	GAMMALVL25,
	GAMMALVL3,
	GAMMALVL35,
	GAMMALVL4
};

constexpr int SKULLXOFF{-32};
constexpr int LINEHEIGHT{16};
constexpr int CRISPY_LINEHEIGHT{10};

static int crispness_cur;
static bool opldev;

static bool joypadSave = false;			// was the save action initiated by joypad?

int saveStringEnter;					// we are going to be entering a savegame string
int saveSlot;							// which slot to save in
int saveCharIndex;						// which char we're editing
char saveOldString[SAVESTRINGSIZE];		// old save description before edit

char savegamestrings[10][SAVESTRINGSIZE];
char endstring[160];

short itemOn;				// menu item skull is on
short skullAnimCounter;		// skull animation counter
short whichSkull;			// which skull to draw

bool inhelpscreens;
bool menuactive;

int mouseSensitivity = 5;
int mouseSensitivity_x2 = 5;
int mouseSensitivity_y = 5;

// temp for screenblocks (0-9)
int screenSize;

// -1 = no quicksave slot picked!
int quickSaveSlot;
// 1 = message to be printed
int messageToPrint;
int messageLastMenuActive;
// timed message = no input from user
bool messageNeedsInput;
// ...and here is the message string!
::std::string messageString;

// graphic name of skulls
// warning: initializer-string for array of chars is too long
::std::array<::std::string, 2> skullName{"M_SKULL1", "M_SKULL2"};

enum class main_e
{
	newgame,
	options,
	loadgame,
	savegame,
	readthis,
	quitdoom,
	main_end
};

enum class episodes_e
{
	ep1,
	ep2,
	ep3,
	ep4,
	ep5,
	ep_end
};

enum class newgame_e
{
	killthings,
	toorough,
	hurtme,
	violence,
	nightmare,
	newg_end
};

enum class load_e
{
	load1,
	load2,
	load3,
	load4,
	load5,
	load6,
	load7,
	load8,
	load_end
};

enum class options_e
{
	endgame,
	messages,
	detail,
	scrnsize,
	option_empty1,
	mousesens,
	soundvol,
	crispness,
	opt_end
};

enum class sound_e
{
	sfx_vol,
	sfx_empty1,
	music_vol,
	sfx_empty2,
	sound_end
};

// mouse sensitivity menu
enum class mouse_e
{
	mouse_horiz,
	mouse_empty1,
	mouse_horiz2,
	mouse_empty2,
	mouse_vert,
	mouse_empty3,
	mouse_invert,
	mouse_end
};

// Read This! MENU 1 & 2
enum class read_e
{
	rdthsempty1,
	read1_end
};

enum class read_e2
{
	rdthsempty2,
	read2_end
};

enum class crispness1_e
{
	crispness_sep_rendering,
	crispness_hires,
	crispness_widescreen,
	crispness_uncapped,
	crispness_vsync,
	crispness_smoothscaling,
	crispness_sep_rendering_,

	crispness_sep_visual,
	crispness_coloredhud,
	crispness_translucency,
	crispness_smoothlight,
	crispness_brightmaps,
	crispness_coloredblood,
	crispness_flipcorpses,
	crispness_sep_visual_,

	crispness1_next,
	crispness1_prev,
	crispness1_end
};

enum class crispness2_e
{
	crispness_sep_audible,
	crispness_soundfull,
	crispness_soundfix,
	crispness_sndchannels,
	crispness_soundmono,
	crispness_sep_audible_,

	crispness_sep_navigational,
	crispness_extautomap,
	crispness_smoothmap,
	crispness_automapstats,
	crispness_leveltime,
	crispness_playercoords,
	crispness_secretmessage,
	crispness_sep_navigational_,

	crispness2_next,
	crispness2_prev,
	crispness2_end
};

enum class crispness3_e
{
	crispness_sep_tactical,
	crispness_freelook,
	crispness_mouselook,
	crispness_bobfactor,
	crispness_centerweapon,
	crispness_weaponsquat,
	crispness_pitch,
	crispness_neghealth,
	crispness_sep_tactical_,

	crispness_sep_crosshair,
	crispness_crosshair,
	crispness_crosshairtype,
	crispness_crosshairhealth,
	crispness_crosshairtarget,
	crispness_sep_crosshair_,

	crispness3_next,
	crispness3_prev,
	crispness3_end
};

enum class crispness4_e
{
	crispness_sep_physical,
	crispness_freeaim,
	crispness_jumping,
	crispness_overunder,
	crispness_recoil,
	crispness_sep_physical_,

	crispness_sep_demos,
	crispness_demotimer,
	crispness_demotimerdir,
	crispness_demobar,
	crispness_demousetimer,
	crispness_sep_demos_,

	crispness4_next,
	crispness4_prev,
	crispness4_end
};

struct menuitem_t
{
	// choice = menu item #
	// if status = 2, choice = 0:leftarrow, 1:rightarrow
	void (*routine)(int choice);

	// 0 = no cursor here, 1 = ok, 2 = arrows ok
	short status;
	// hotkey in menu
	char alphaKey;

	char name[10];

	::std::string alttext;		// alternative text for menu items
};

struct menu_t
{
	menu_t* prevMenu;			// previous menu
	menuitem_t* menuitems;		// menu items
	short numitems;				// # of menu items
	short lastOn;				// last item user was on in menu
	short x;					// x,y of menu
	short y;
	short lumps_missing;		// indicate missing menu graphics lumps
	void (*routine)();			// draw routine
};

// current menudef
menu_t* currentMenu;

static void M_NewGame(int choice);
static void M_Options(int choice);
static void M_LoadGame(int choice);
static void M_SaveGame(int choice);
static void M_ReadThis(int choice);
static void M_QuitDOOM(int choice);
std::array<menuitem_t, 6> MainMenu{
	menuitem_t{ M_NewGame, 1, 'n', "M_NGAME" },
	menuitem_t{ M_Options, 1, 'o', "M_OPTION" },
	menuitem_t{ M_LoadGame, 1, 'l', "M_LOADG" },
	menuitem_t{ M_SaveGame, 1, 's', "M_SAVEG" },
	menuitem_t{ M_ReadThis, 1, 'r', "M_RDTHIS" },
	menuitem_t{ M_QuitDOOM, 1, 'q', "M_QUITG" }
};

static void M_DrawMainMenu();
menu_t MainDef{
	nullptr,
	MainMenu,
	main_e::main_end,
	0,
	97,
	64,
	M_DrawMainMenu
};

static void M_Episode(int choice);
::std::array<menuitem_t, 5> EpisodeMenu{
	menuitem_t{ M_Episode, 1, 'k', "M_EPI1" },
	menuitem_t{ M_Episode, 1, 't', "M_EPI2" },
	menuitem_t{ M_Episode, 1, 'i', "M_EPI3" },
	menuitem_t{ M_Episode, 1, 't', "M_EPI4" },
	menuitem_t{ M_Episode, 1, 's', "M_EPI5" }
};

static void M_DrawEpisode();
menu_t EpiDef{
	&MainDef,					// previous menu
	EpisodeMenu,				// menuitem_t ->
	episodes_e::ep_end,			// # of menu items
	episodes_e::ep1,			// lastOn
	48,							// x,y
	63,
	M_DrawEpisode				// drawing routine ->
};

static void M_ChooseSkill(int choice);
::std::array<menuitem_t, 5> NewGameMenu{
	menuitem_t{ M_ChooseSkill, 1, 'i', "M_JKILL", "I'm too young to die." },
	menuitem_t{ M_ChooseSkill, 1, 'h', "M_ROUGH", "Hey, not too rough!." },
	menuitem_t{ M_ChooseSkill, 1, 'h', "M_HURT", "Hurt me plenty." },
	menuitem_t{ M_ChooseSkill, 1, 'u', "M_ULTRA", "Ultra-Violence." },
	menuitem_t{ M_ChooseSkill, 1, 'n', "M_NMARE", "Nightmare!" }
};

static void M_DrawNewGame();
menu_t NewDef{
	&EpiDef,					// previous menu
	NewGameMenu,				// menuitem_t ->
	newgame_e::newg_end,		// # of menu items
	newgame_e::hurtme,			// lastOn
	48,							// x,y
	63,
	M_DrawNewGame				// drawing routine ->
};

static void M_EndGame(int choice);
static void M_ChangeMessages(int choice);
static void M_ChangeDetail(int choice);
static void M_SizeDisplay(int choice);
static void M_Mouse(int choice);
static void M_Sound(int choice);
static void M_CrispnessCur(int choice);
::std::array<menuitem_t, 8> OptionsMenu{
	menuitem_t{ M_EndGame, 1, 'e', "M_ENDGAM", "End Game" },
	menuitem_t{ M_ChangeMessages, 1, 'm', "M_MESSG", "Messages: " },
	menuitem_t{ M_ChangeDetail, 1, 'g', "M_DETAIL", "Graphic Detail: " },
	menuitem_t{ M_SizeDisplay, 2, 's', "M_SCRNSZ", "Screen Size" },
	menuitem_t{ nullptr, -1, '\0', "" },
	menuitem_t{ M_Mouse, 1, 'm', "M_MSENS", "Mouse Sensitivity" },
	menuitem_t{ M_Sound, 1, 's', "M_SVOL","Sound Volume" },
	menuitem_t{ M_CrispnessCur, 1, 'c', "M_CRISPY", "Crispness" }
};

static void M_DrawOptions();
menu_t OptionsDef{
	&MainDef,
	OptionsMenu,
	options_e::opt_end,
	0,
	60,
	37,
	M_DrawOptions
};

static void M_ChangeSensitivity(int choice);
static void M_ChangeSensitivity_x2(int choice);
static void M_ChangeSensitivity_y(int choice);
static void M_MouseInvert(int choice);
static ::std::array<menuitem_t, 7> MouseMenu{
	menuitem_t{ M_ChangeSensitivity, 2, 'h', "" },
	menuitem_t{ nullptr, -1, '\0', "" },
	menuitem_t{ M_ChangeSensitivity_x2, 2, 's', "" },
	menuitem_t{ nullptr, -1, '\0', "" },
	menuitem_t{ M_ChangeSensitivity_y, 2, 'v', "" },
	menuitem_t{ nullptr, -1, '\0', "" },
	menuitem_t{ M_MouseInvert, 1, 'i', "" }
};

static void M_DrawMouse();
static menu_t MouseDef{
	&OptionsDef,
	MouseMenu,
	mouse_e::mouse_end,
	0,
	80,
	64,
	M_DrawMouse
};

static void M_CrispnessNext(int choice);
static void M_CrispnessPrev(int choice);
static ::std::array<menuitem_t, 17> Crispness1Menu{
	menuitem_t{ nullptr, -1, '\0', "" },
	menuitem_t{ M_CrispyToggleHires, 1, 'h', "" },
	menuitem_t{ M_CrispyToggleWidescreen, 1, 'w', "" },
	menuitem_t{ M_CrispyToggleUncapped, 1, 'u', "" },
	menuitem_t{ M_CrispyToggleVsync, 1, 'v', "" },
	menuitem_t{ M_CrispyToggleSmoothScaling, 1, 's', "" },
	menuitem_t{ nullptr, -1, '\0', "" },
	menuitem_t{ nullptr, -1, '\0', "" },
	menuitem_t{ M_CrispyToggleColoredhud, 1, 'c', "" },
	menuitem_t{ M_CrispyToggleTranslucency, 1, 'e', "" },
	menuitem_t{ M_CrispyToggleSmoothLighting, 1, 's', "" },
	menuitem_t{ M_CrispyToggleBrightmaps, 1, 'b', "" },
	menuitem_t{ M_CrispyToggleColoredblood, 1, 'c', "" },
	menuitem_t{ M_CrispyToggleFlipcorpses, 1, 'r', "" },
	menuitem_t{ nullptr, -1, '\0', "" },
	menuitem_t{ M_CrispnessNext, 1, 'n', "" },
	menuitem_t{ M_CrispnessPrev, 1, 'p', "" }
};

static void M_DrawCrispness1();
static menu_t Crispness1Def{
	&OptionsDef,
	Crispness1Menu,
	crispness1_e::crispness1_end,
	1,
	48,
	28,
	M_DrawCrispness1
};

static ::std::array<menuitem_t, 16> Crispness2Menu{
	menuitem_t{ nullptr, -1, '\0', "" },
	menuitem_t{ M_CrispyToggleFullsounds, 1, 'p', "" },
	menuitem_t{ M_CrispyToggleSoundfixes, 1, 'm', "" },
	menuitem_t{ M_CrispyToggleSndChannels, 1, 's', "" },
	menuitem_t{ M_CrispyToggleSoundMono, 1, 'm', "" },
	menuitem_t{ nullptr, -1, '\0', "" },
	menuitem_t{ nullptr, -1, '\0', "" },
	menuitem_t{ M_CrispyToggleExtAutomap, 1, 'e', "" },
	menuitem_t{ M_CrispyToggleSmoothMap, 1, 'm', "" },
	menuitem_t{ M_CrispyToggleAutomapstats, 1, 's', "" },
	menuitem_t{ M_CrispyToggleLeveltime, 1, 'l', "" },
	menuitem_t{ M_CrispyTogglePlayerCoords, 1, 'p', "" },
	menuitem_t{ M_CrispyToggleSecretmessage, 1, 's', "" },
	menuitem_t{ nullptr, -1, '\0', "" },
	menuitem_t{ M_CrispnessNext, 1, 'n', "" },
	menuitem_t{ M_CrispnessPrev, 1, 'p', "" }
};

static void M_DrawCrispness2();
static menu_t Crispness2Def{
	&OptionsDef,
	Crispness2Menu,
	crispness2_e::crispness2_end,
	1,
	48,
	28,
	M_DrawCrispness2
};

static ::std::array<menuitem_t, 17> Crispness3Menu{
	menuitem_t{ nullptr, -1, '\0', "" },
	menuitem_t{ M_CrispyToggleFreelook, 1, 'a', "" },
	menuitem_t{ M_CrispyToggleMouseLook, 1, 'p', "" },
	menuitem_t{ M_CrispyToggleBobfactor, 1, 'p', "" },
	menuitem_t{ M_CrispyToggleCenterweapon, 1, 'c', "" },
	menuitem_t{ M_CrispyToggleWeaponSquat, 1, 'w', "" },
	menuitem_t{ M_CrispyTogglePitch, 1, 'w', "" },
	menuitem_t{ M_CrispyToggleNeghealth, 1, 'n', "" },
	menuitem_t{ nullptr, -1, '\0', "" },
	menuitem_t{ nullptr, -1, '\0', "" },
	menuitem_t{ M_CrispyToggleCrosshair, 1, 'd', "" },
	menuitem_t{ M_CrispyToggleCrosshairtype, 1, 'c', "" },
	menuitem_t{ M_CrispyToggleCrosshairHealth, 1, 'c', "" },
	menuitem_t{ M_CrispyToggleCrosshairTarget, 1, 'h', "" },
	menuitem_t{ nullptr, -1, '\0', "" },
	menuitem_t{ M_CrispnessNext, 1, 'n', "" },
	menuitem_t{ M_CrispnessPrev, 1, 'p', "" }
};

static void M_DrawCrispness3();
static menu_t Crispness3Def{
	&OptionsDef,
	Crispness3Menu,
	crispness3_e::crispness3_end,
	1,
	48,
	28,
	M_DrawCrispness3
};

static ::std::array<menuitem_t, 14> Crispness4Menu{
	menuitem_t{ nullptr, -1, '\0', "" },
	menuitem_t{ M_CrispyToggleFreeaim, 1, 'v', "" },
	menuitem_t{ M_CrispyToggleJumping, 1, 'a', "" },
	menuitem_t{ M_CrispyToggleOverunder, 1, 'w', "" },
	menuitem_t{ M_CrispyToggleRecoil, 1, 'w', "" },
	menuitem_t{ nullptr, -1, '\0', "" },
	menuitem_t{ nullptr, -1, '\0', "" },
	menuitem_t{ M_CrispyToggleDemoTimer, 1,'v', "" },
	menuitem_t{ M_CrispyToggleDemoTimerDir, 1,'a', "" },
	menuitem_t{ M_CrispyToggleDemoBar, 1,'w', "" },
	menuitem_t{ M_CrispyToggleDemoUseTimer, 1,'u', "" },
	menuitem_t{ nullptr, -1, '\0', "" },
	menuitem_t{ M_CrispnessNext, 1, 'n', "" },
	menuitem_t{ M_CrispnessPrev, 1, 'p', "" }
};

static void M_DrawCrispness4();
static menu_t Crispness4Def{
	&OptionsDef,
	Crispness4Menu,
	crispness4_e::crispness4_end,
	1,
	48,
	28,
	M_DrawCrispness4
};

static ::std::array<menu_t*, 4> CrispnessMenus{
	&Crispness1Def,
	&Crispness2Def,
	&Crispness3Def,
	&Crispness4Def
};

static void M_ReadThis2(int choice);
menuitem_t ReadMenu1{ M_ReadThis2, 1, '\0', "" };

static void M_DrawReadThis1();
menu_t ReadDef1{
	&MainDef,
	ReadMenu1,
	read_e::read1_end,
	0,
	280,
	185,
	M_DrawReadThis1
};

static void M_FinishReadThis(int choice);
menuitem_t ReadMenu2{ M_FinishReadThis, 1, '\0', "" };

static void M_DrawReadThis2();
menu_t ReadDef2{
	&ReadDef1,
	ReadMenu2,
	read_e2::read2_end,
	0,
	330,
	175,
	M_DrawReadThis2
};

static void M_SfxVol(int choice);
static void M_MusicVol(int choice);
::std::array<menuitem_t, 4> SoundMenu{
	menuitem_t{ M_SfxVol, 2, 's', "M_SFXVOL" },
	menuitem_t{ nullptr, -1, '\0', "" },
	menuitem_t{ M_MusicVol, 2, 'm', "M_MUSVOL" },
	menuitem_t{ nullptr, -1, '\0', "" }
};

static void M_DrawSound();
menu_t SoundDef{
	&OptionsDef,
	SoundMenu,
	sound_e::sound_end,
	0,
	80,
	64,
	M_DrawSound
};


static void M_LoadSelect(int choice);
::std::array<menuitem_t, 8> _LoadMenu{
	menuitem_t{ M_LoadSelect, 1, '1', "" },
	menuitem_t{ M_LoadSelect, 1, '2', "" },
	menuitem_t{ M_LoadSelect, 1, '3', "" },
	menuitem_t{ M_LoadSelect, 1, '4', "" },
	menuitem_t{ M_LoadSelect, 1, '5', "" },
	menuitem_t{ M_LoadSelect, 1, '6', "" },
	menuitem_t{ M_LoadSelect, 1, '7', "" },
	menuitem_t{ M_LoadSelect, 1, '8', "" }
};

static void M_DrawLoad();
menu_t LoadDef{
	&MainDef,
	_LoadMenu,
	load_e::load_end,
	0,
	80,
	54,
	M_DrawLoad
};

static void M_SaveSelect(int choice);
::std::array<menuitem_t, 8> SaveMenu{
	menuitem_t{ M_SaveSelect, 1, '1', "" },
	menuitem_t{ M_SaveSelect, 1, '2', "" },
	menuitem_t{ M_SaveSelect, 1, '3', "" },
	menuitem_t{ M_SaveSelect, 1, '4', "" },
	menuitem_t{ M_SaveSelect, 1, '5', "" },
	menuitem_t{ M_SaveSelect, 1, '6', "" },
	menuitem_t{ M_SaveSelect, 1, '7', "" },
	menuitem_t{ M_SaveSelect, 1, '8', "" }
};

static void M_DrawSave();
menu_t SaveDef{
	&MainDef,
	SaveMenu,
	load_e::load_end,
	0,
	80,
	54,
	M_DrawSave
};

// read the strings from the savegame files
void M_ReadSaveStrings()
{
	for (size_t i{0}; i < load_e::load_end; ++i)
	{
		char name[256];
		M_StringCopy(name, P_SaveGameFile(i), sizeof(name));

		auto handle{fopen(name, "rb")};
		if (handle == nullptr)
		{
			M_StringCopy(savegamestrings[i], EMPTYSTRING, SAVESTRINGSIZE);
			LoadMenu[i].status = 0;
			continue;
		}

		auto retval{fread(&savegamestrings[i], 1, SAVESTRINGSIZE, handle)};
		fclose(handle);
		LoadMenu[i].status = (retval == SAVESTRINGSIZE);
	}
}

// Write a string using the hu_font
void M_WriteText(int x, int y, ::std::string string)
{
	int w;
	int c;

	::std::string ch = string;
	auto cx = x;
	auto cy = y;

	while (1)
	{
		c = *ch;
		++ch;
		if (!c)
		{
			break;
		}

		if (c == '\n')
		{
			cx = x;
			cy += 12;
			continue;
		}

		// support multi-colored text
		if (c == cr_esc)
		{
			if (*ch >= '0' && *ch <= '0' + CRMAX - 1)
			{
				c = *ch;
				++ch;
				dp_translation = cr[(int)(c - '0')];
				continue;
			}
		}

		c = toupper(c) - HU_FONTSTART;
		if (c < 0 || c >= HU_FONTSIZE)
		{
			cx += 4;
			continue;
		}

		w = SHORT(hu_font[c]->width);

		if (cx + w > ORIGWIDTH)
		{
			break;
		}

		V_DrawPatchDirect(cx, cy, hu_font[c]);
		cx += w;
	}
}

void M_DrawSaveLoadBorder(int x, int y)
{
	V_DrawPatchDirect(x - 8, y + 7, W_CacheLumpName<patch_t>(DEH_String("M_LSLEFT"), pu_tags_t::PU_CACHE));

	for (size_t i{0}; i < 24; ++i)
	{
		V_DrawPatchDirect(x, y + 7, W_CacheLumpName<patch_t>(DEH_String("M_LSCNTR"), pu_tags_t::PU_CACHE));
		x += 8;
	}

	V_DrawPatchDirect(x, y + 7, W_CacheLumpName<patch_t>(DEH_String("M_LSRGHT"), pu_tags_t::PU_CACHE));
}

// M_LoadGame & Cie.
static int LoadDef_x = 72;
static int LoadDef_y = 28;

void M_DrawLoad()
{
	V_DrawPatchDirect(LoadDef_x, LoadDef_y, W_CacheLumpName<patch_t>(DEH_String("M_LOADG"), pu_tags_t::PU_CACHE));

	for (size_t i{0}; i < load_e::load_end; ++i)
	{
		M_DrawSaveLoadBorder(LoadDef.x, LoadDef.y + LINEHEIGHT * i);

		// shade empty savegame slots
		if (!LoadMenu[i].status)
		{
			dp_translation = cr[CR_DARK];
		}

		M_WriteText(LoadDef.x, LoadDef.y + LINEHEIGHT * i, savegamestrings[i]);

		dp_translation = nullptr;
	}
}

void M_ClearMenus()
{
	menuactive = 0;

	// entering menus while recording demos pauses the game
	if (demorecording && paused)
	{
		sendpause = true;
	}

	// if (!netgame && usergame && paused)
	//{
		//sendpause = true;
	//}
}

void M_LoadSelect(int choice)
{
	char name[256];

	M_StringCopy(name, P_SaveGameFile(choice), sizeof(name));

	// save the last game you loaded
	SaveDef.lastOn = choice;
	G_LoadGame(name);
	M_ClearMenus();

	// allow quickload before quicksave
	if (quickSaveSlot == -2)
	{
		quickSaveSlot = choice;
	}
}

void M_StartMessage(::std::string string, void* routine, bool input)
{
	messageLastMenuActive = menuactive;
	messageToPrint = 1;
	messageString = string;
	messageRoutine = routine;
	messageNeedsInput = input;
	menuactive = true;

	// entering menus while recording demos pauses the game
	if (demorecording && !paused)
	{
		sendpause = true;
	}

	return;
}

void M_SetupNextMenu(menu_t* menudef)
{
	currentMenu = menudef;
	itemOn = currentMenu->lastOn;
}

void M_LoadGame(int choice)
{
	// allow loading game while multiplayer demo playback
	if (netgame && !demoplayback)
	{
		M_StartMessage(DEH_String(LOADNET), nullptr, false);
		return;
	}

	M_SetupNextMenu(&LoadDef);
	M_ReadSaveStrings();
}

// Find string width from hu_font chars
int M_StringWidth(::std::string string)
{
	int width{0};

	for (size_t i{0}; i < strlen(string); ++i)
	{
		// correctly center colorized strings
		if (string[i] == cr_esc)
		{
			if (string[i + 1] >= '0' && string[i + 1] <= '0' + CRMAX - 1)
			{
				++i;
				continue;
			}
		}

		int c = toupper(string[i]) - HU_FONTSTART;
		if (c < 0 || c >= HU_FONTSIZE)
		{
			width += 4;
		}
		else
		{
			width += SHORT(hu_font[c]->width);
		}
	}

	return width;
}

// M_SaveGame & Cie.
static int SaveDef_x = 72;
static int SaveDef_y = 28;

void M_DrawSave()
{
	V_DrawPatchDirect(SaveDef_x, SaveDef_y, W_CacheLumpName<patch_t>(DEH_String("M_SAVEG"), pu_tags_t::PU_CACHE));
	for (size_t i{0}; i < load_e::load_end; ++i)
	{
		M_DrawSaveLoadBorder(LoadDef.x, LoadDef.y + LINEHEIGHT * i);
		M_WriteText(LoadDef.x, LoadDef.y + LINEHEIGHT * i, savegamestrings[i]);
	}

	if (saveStringEnter)
	{
		auto width{M_StringWidth(savegamestrings[saveSlot])};
		// NOTE: SHOULD WIDTH BE ALLOWED TO BE NEGATIVE? TODO INVESTIGATE
		M_WriteText(LoadDef.x + width, LoadDef.y + LINEHEIGHT * saveSlot, "_");
	}
}

// M_Responder calls this when user is finished
void M_DoSave(int slot)
{
	G_SaveGame(slot, savegamestrings[slot]);
	M_ClearMenus();

	// PICK QUICKSAVE SLOT YET?
	if (quickSaveSlot == -2)
	{
		quickSaveSlot = slot;
	}
}

// Generate a default save slot name when the user saves to an empty slot via the joypad.
static void SetDefaultSaveName(int slot)
{
	// map from IWAD or PWAD?
	if (W_IsIWADLump(maplumpinfo) && strcmp(savegamedir, ""))
	{
		M_snprintf(savegamestrings[itemOn], SAVESTRINGSIZE, "%s", maplumpinfo->name);
	}
	else
	{
		auto wadname{::std::string(W_WadNameForLump(maplumpinfo))};
		auto ext = strrchr(*wadname, '.');

		if (ext)
		{
			*ext = '\0';
		}

		M_snprintf(savegamestrings[itemOn], SAVESTRINGSIZE, "%s (%s)", maplumpinfo->name, *wadname);
	}

	M_ForceUppercase(savegamestrings[itemOn]);
	joypadSave = false;
}

// override savegame name if it already starts with a map identifier
static bool StartsWithMapIdentifier(::std::string str)
{
	M_ForceUppercase(str);

	if (strlen(str) >= 4 && str[0] == 'E' && isdigit(str[1]) && str[2] == 'M' && isdigit(str[3]))
	{
		return true;
	}

	if (strlen(str) >= 5 && str[0] == 'M' && str[1] == 'A' && str[2] == 'P' && isdigit(str[3]) && isdigit(str[4]))
	{
		return true;
	}

	return false;
}

// User wants to save. Start string input for M_Responder
void M_SaveSelect(int choice)
{
	// we are going to be intercepting all chars
	saveStringEnter = 1;

	// load the last game you saved
	LoadDef.lastOn = choice;

	// We need to turn on text input:
	auto x{LoadDef.x - 11};
	auto y{LoadDef.y + choice * LINEHEIGHT - 4};
	// NOTE: TODO INVESTIGATE 2*8 + 24*8 = 26*8 = 208, where does this value come from?
	// maybe it makes sense as bits in a byte: 1*128 + 1*64 + 0*32 + 1*16 + 0*8 + 0*4 + 0*2 + 0*1 = 208 = 11010000
	// ... but why write it out as 8 + 24*8 + 8?
	I_StartTextInput(x, y, x + 8 + 24 * 8 + 8, y + LINEHEIGHT - 2);

	saveSlot = choice;
	M_StringCopy(saveOldString, savegamestrings[choice], SAVESTRINGSIZE);
	// override savegame name if it already starts with a map identifier
	if (!strcmp(savegamestrings[choice], EMPTYSTRING) || StartsWithMapIdentifier(savegamestrings[choice]))
	{
		savegamestrings[choice][0] = 0;

		if (joypadSave || true) // always prefill empty savegame slot names
		{
			SetDefaultSaveName(choice);
		}
	}

	saveCharIndex = strlen(savegamestrings[choice]);
}

void M_SaveGame(int choice)
{
	if (!usergame)
	{
		M_StartMessage(DEH_String(SAVEDEAD), nullptr, false);
		return;
	}

	if (gamestate != GameState_t::GS_LEVEL)
	{
		return;
	}

	M_SetupNextMenu(&SaveDef);
	M_ReadSaveStrings();
}

void M_QuickSaveResponse(int key)
{
	if (key == key_menu_confirm)
	{
		M_DoSave(quickSaveSlot);
		S_StartSound(nullptr, sfxenum_t::sfx_swtchx);
	}
}

static char tempstring[90];

void M_QuickSave()
{
	if (!usergame)
	{
		S_StartSound(nullptr, sfxenum_t::sfx_oof);
		return;
	}

	if (gamestate != GameState_t::GS_LEVEL)
	{
		return;
	}

	if (quickSaveSlot < 0)
	{
		M_StartControlPanel();
		M_ReadSaveStrings();
		M_SetupNextMenu(&SaveDef);
		quickSaveSlot = -2;	// means to pick a slot now
		return;
	}
	// print savegame name in golden letters
	auto savegamestring{::std::string(::std::string(crstr[CR_GOLD]) + savegamestrings[quickSaveSlot] + ::std::string(crstr[CR_NONE]))};
	DEH_snprintf(tempstring, sizeof(tempstring), QSPROMPT, *savegamestring);
	M_StartMessage(tempstring, M_QuickSaveResponse, true);
}

void M_QuickLoadResponse(int key)
{
	if (key == key_menu_confirm)
	{
		M_LoadSelect(quickSaveSlot);
		S_StartSound(nullptr, sfxenum_t::sfx_swtchx);
	}
}

void M_QuickLoad()
{
	// allow quickloading game while multiplayer demo playback
	if (netgame && !demoplayback)
	{
		M_StartMessage(DEH_String(QLOADNET), NULL, false);
		return;
	}

	if (quickSaveSlot < 0)
	{
		// allow quickload before quicksave
		M_StartControlPanel();
		M_ReadSaveStrings();
		M_SetupNextMenu(&LoadDef);
		quickSaveSlot = -2;
		return;
	}
	// print savegame name in golden letters
	auto savegamestring{::std::string(::std::string(crstr[CR_GOLD]) + savegamestrings[quickSaveSlot] + ::std::string(crstr[CR_NONE]))};
	DEH_snprintf(tempstring, sizeof(tempstring), QLPROMPT, *savegamestring);
	M_StartMessage(tempstring, M_QuickLoadResponse, true);
}

// Read This Menus
// Had a "quick hack to fix romero bug"
void M_DrawReadThis1()
{
	inhelpscreens = true;

	V_DrawPatchFullScreen(W_CacheLumpName<patch_t>(DEH_String("HELP2"), pu_tags_t::PU_CACHE), false);
}

// Read This Menus - optional second page.
void M_DrawReadThis2()
{
	inhelpscreens = true;

	// We only ever draw the second page if this is
	// gameversion == GameVersion::exe_doom_1_9 and gamemode == GameMode::registered
	V_DrawPatchFullScreen(W_CacheLumpName<patch_t>(DEH_String("HELP1"), pu_tags_t::PU_CACHE), false);
}

void M_DrawReadThisCommercial()
{
	inhelpscreens = true;

	V_DrawPatchFullScreen(W_CacheLumpName<patch_t>(DEH_String("HELP"), pu_tags_t::PU_CACHE), false);
}

void M_DrawThermo(int x, int y, int thermWidth, int thermDot)
{
	char num[4];

	if (!thermDot)
	{
		dp_translation = cr[CR_DARK];
	}

	auto xx = x;

	V_DrawPatchDirect(xx, y, W_CacheLumpName<patch_t>(DEH_String("M_THERML"), pu_tags_t::PU_CACHE));
	xx += 8;

	for (size_t i{0}; i < thermWidth; ++i)
	{
		V_DrawPatchDirect(xx, y, W_CacheLumpName<patch_t>(DEH_String("M_THERMM"), pu_tags_t::PU_CACHE));
		xx += 8;
	}

	V_DrawPatchDirect(xx, y, W_CacheLumpName<patch_t>(DEH_String("M_THERMR"), pu_tags_t::PU_CACHE));

	M_snprintf(num, 4, "%3d", thermDot);
	M_WriteText(xx + 8, y + 3, num);

	if (thermDot >= thermWidth)
	{
		thermDot = thermWidth - 1;
		dp_translation = cr[CR_DARK];
	}

	V_DrawPatchDirect((x + 8) + thermDot * 8, y, W_CacheLumpName<patch_t>(DEH_String("M_THERMO"), pu_tags_t::PU_CACHE));

	dp_translation = nullptr;
}

// Change Sfx & Music volumes
void M_DrawSound()
{
	V_DrawPatchDirect(60, 38, W_CacheLumpName<patch_t>(DEH_String("M_SVOL"), pu_tags_t::PU_CACHE));

	M_DrawThermo(SoundDef.x, SoundDef.y + LINEHEIGHT * (sound_e::sfx_vol + 1), 16, sfxVolume);

	M_DrawThermo(SoundDef.x, SoundDef.y + LINEHEIGHT * (sound_e::music_vol + 1), 16, musicVolume);
}

void M_Sound(int choice)
{
	M_SetupNextMenu(&SoundDef);
}

void M_SfxVol(int choice)
{
	switch (choice)
	{
	case 0:
		if (sfxVolume)
		{
			--sfxVolume;
		}
		break;
	case 1:
		if (sfxVolume < 15)
		{
			++sfxVolume;
		}
		break;
	}

	S_SetSfxVolume(sfxVolume * 8);
}

void M_MusicVol(int choice)
{
	switch (choice)
	{
	case 0:
		if (musicVolume)
		{
			--musicVolume;
		}
		break;
	case 1:
		if (musicVolume < 15)
		{
			++musicVolume;
		}
		break;
	}

	S_SetMusicVolume(musicVolume * 8);
}

void M_DrawMainMenu()
{
	V_DrawPatchDirect(94, 2, W_CacheLumpName<patch_t>(DEH_String("M_DOOM"), pu_tags_t::PU_CACHE));
}

void M_DrawNewGame()
{
	V_DrawPatchDirect(96, 14, W_CacheLumpName<patch_t>(DEH_String("M_NEWG"), pu_tags_t::PU_CACHE));
	V_DrawPatchDirect(54, 38, W_CacheLumpName<patch_t>(DEH_String("M_SKILL"), pu_tags_t::PU_CACHE));
}

void M_NewGame(int choice)
{
	// forbid New Game while recording a demo
	if (demorecording)
	{
		return;
	}

	if (netgame && !demoplayback)
	{
		M_StartMessage(DEH_String(NEWGAME), NULL, false);
		return;
	}

	// Chex Quest disabled the episode select screen, as did Doom II.

	if ((gamemode == GameMode::commercial && crispy->havenerve.empty() && crispy->havemaster.empty()) || gameversion == GameVersion::exe_chex)
	{
		// NRFTL / The Master Levels
		M_SetupNextMenu(&NewDef);
	}
	else
	{
		M_SetupNextMenu(&EpiDef);
	}
}

void M_DrawEpisode()
{
	V_DrawPatchDirect(54, 38, W_CacheLumpName<patch_t>(DEH_String("M_EPISOD"), pu_tags_t::PU_CACHE));
}

int epi;

void M_VerifyNightmare(Keys key)
{
	if (key != key_menu_confirm)
	{
		return;
	}

	G_DeferedInitNew(newgame_e::nightmare, epi + 1, 1);
	M_ClearMenus();
}

void M_ChooseSkill(int choice)
{
	if (choice == newgame_e::nightmare)
	{
		M_StartMessage(DEH_String(NIGHTMARE), M_VerifyNightmare, true);
		return;
	}

	G_DeferedInitNew(choice, epi + 1, 1);
	M_ClearMenus();
}

void M_Episode(int choice)
{
	if ((gamemode == GameMode::shareware) && choice)
	{
		M_StartMessage(DEH_String(SWSTRING), NULL, false);
		M_SetupNextMenu(&ReadDef1);
		return;
	}

	epi = choice;
	M_SetupNextMenu(&NewDef);
}

// Find string height from hu_font chars
int M_StringHeight(::std::string string)
{
	int height{SHORT(hu_font[0]->height)};

	auto h{height};
	for (size_t i{0}; i < strlen(string); ++i)
	{
		if (string[i] == '\n')
		{
			h += height;
		}
	}

	return h;
}

static ::std::array<::std::string, 2> detailNames{"M_GDHIGH", "M_GDLOW"};
static ::std::array<::std::string, 2> msgNames{"M_MSGOFF", "M_MSGON"};

void M_DrawOptions()
{
	V_DrawPatchDirect(108, 15, W_CacheLumpName<patch_t>(DEH_String("M_OPTTTL"), pu_tags_t::PU_CACHE));

	if (OptionsDef.lumps_missing == -1)
	{
		V_DrawPatchDirect(OptionsDef.x + 175, OptionsDef.y + LINEHEIGHT * options_e::detail,
			W_CacheLumpName<patch_t>(DEH_String(detailNames[detailLevel]), pu_tags_t::PU_CACHE));
	}
	else
		if (OptionsDef.lumps_missing > 0)
		{
			M_WriteText(OptionsDef.x + M_StringWidth("Graphic Detail: "),
				OptionsDef.y + LINEHEIGHT * options_e::detail + 8 - (M_StringHeight("HighLow") / 2), detailLevel ? "Low" : "High");
		}

	if (OptionsDef.lumps_missing == -1)
	{
		V_DrawPatchDirect(OptionsDef.x + 120, OptionsDef.y + LINEHEIGHT * options_e::messages,
			W_CacheLumpName<patch_t>(DEH_String(msgNames[showMessages]), pu_tags_t::PU_CACHE));
	}
	else
		if (OptionsDef.lumps_missing > 0)
		{
			M_WriteText(OptionsDef.x + M_StringWidth("Messages: "),
				OptionsDef.y + LINEHEIGHT * options_e::messages + 8 - (M_StringHeight("OnOff") / 2), showMessages ? "On" : "Off");
		}

	// mouse sensitivity menu
	/*
		M_DrawThermo(OptionsDef.x, OptionsDef.y + LINEHEIGHT * (mousesens + 1), 10, mouseSensitivity);
	*/
	M_DrawThermo(OptionsDef.x, OptionsDef.y + LINEHEIGHT * (options_e::scrnsize + 1), 9 + (crispy->widescreen ? 6 : 3), screenSize); // Crispy HUD
}

// mouse sensitivity menu
static void M_DrawMouse()
{
	char mouse_menu_text[48];

	V_DrawPatchDirect(60, LoadDef_y, W_CacheLumpName<patch_t>(DEH_String("M_MSENS"), pu_tags_t::PU_CACHE));

	M_WriteText(MouseDef.x, MouseDef.y + LINEHEIGHT * mouse_e::mouse_horiz + 6, "HORIZONTAL: TURN");

	M_DrawThermo(MouseDef.x, MouseDef.y + LINEHEIGHT * mouse_e::mouse_empty1, 21, mouseSensitivity);

	M_WriteText(MouseDef.x, MouseDef.y + LINEHEIGHT * mouse_e::mouse_horiz2 + 6, "HORIZONTAL: STRAFE");

	M_DrawThermo(MouseDef.x, MouseDef.y + LINEHEIGHT * mouse_e::mouse_empty2, 21, mouseSensitivity_x2);

	M_WriteText(MouseDef.x, MouseDef.y + LINEHEIGHT * mouse_e::mouse_vert + 6, "VERTICAL");

	M_DrawThermo(MouseDef.x, MouseDef.y + LINEHEIGHT * mouse_e::mouse_empty3, 21, mouseSensitivity_y);

	M_snprintf(mouse_menu_text, sizeof(mouse_menu_text), "%sInvert Vertical Axis: %s%s", crstr[CR_NONE],
		mouse_y_invert ? crstr[CR_GREEN] : crstr[CR_DARK], mouse_y_invert ? "On" : "Off");
	M_WriteText(MouseDef.x, MouseDef.y + LINEHEIGHT * mouse_e::mouse_invert + 6, mouse_menu_text);

	dp_translation = nullptr;
}

static void M_DrawCrispnessBackground()
{
	const byte* const src{crispness_background};
	pixel_t* dest = I_VideoBuffer;

	for (size_t y{0}; y < SCREENHEIGHT; ++y)
	{
		for (size_t x{0}; x < SCREENWIDTH; ++x)
		{
#ifndef CRISPY_TRUECOLOR
			* dest = src[(y & 63) * 64 + (x & 63)];
			++dest;
#else
			* dest = colormaps[src[(y & 63) * 64 + (x & 63)]];
			++dest;
#endif
		}
	}

	inhelpscreens = true;
}

static char crispy_menu_text[48];

static void M_DrawCrispnessHeader(::std::string item)
{
	M_snprintf(crispy_menu_text, sizeof(crispy_menu_text), "%s%s", crstr[CR_GOLD], item);
	M_WriteText(ORIGWIDTH / 2 - M_StringWidth(item) / 2, 12, crispy_menu_text);
}

static void M_DrawCrispnessSeparator(int y, ::std::string item)
{
	M_snprintf(crispy_menu_text, sizeof(crispy_menu_text), "%s%s", crstr[CR_GOLD], item);
	M_WriteText(currentMenu->x - 8, currentMenu->y + CRISPY_LINEHEIGHT * y, crispy_menu_text);
}

static void M_DrawCrispnessItem(int y, ::std::string item, int feat, bool cond)
{
	// FIXME WE HEARD YOU LIKED TERNERIES SO WE GOT YOU TERNERIES FOR YOUR TERNERIES
	M_snprintf(crispy_menu_text, sizeof(crispy_menu_text),
		"%s%s: %s%s", cond ? crstr[CR_NONE] : crstr[CR_DARK], item,
		cond ? (feat ? crstr[CR_GREEN] : crstr[CR_DARK]) : crstr[CR_DARK],
		cond && feat ? "On" : "Off");
	M_WriteText(currentMenu->x, currentMenu->y + CRISPY_LINEHEIGHT * y, crispy_menu_text);
}

static void M_DrawCrispnessMultiItem(int y, ::std::string item, multiitem_t* multiitem, int feat, bool cond)
{
	M_snprintf(crispy_menu_text, sizeof(crispy_menu_text),
		"%s%s: %s%s", cond ? crstr[CR_NONE] : crstr[CR_DARK], item,
		cond ? (feat ? crstr[CR_GREEN] : crstr[CR_DARK]) : crstr[CR_DARK],
		cond && feat ? multiitem[feat].name : multiitem[0].name);
	M_WriteText(currentMenu->x, currentMenu->y + CRISPY_LINEHEIGHT * y, crispy_menu_text);
}

static void M_DrawCrispnessGoto(int y, ::std::string item)
{
	M_snprintf(crispy_menu_text, sizeof(crispy_menu_text), "%s%s", crstr[CR_GOLD], item);
	M_WriteText(currentMenu->x, currentMenu->y + CRISPY_LINEHEIGHT * y, crispy_menu_text);
}

static void M_DrawCrispness1()
{
	M_DrawCrispnessBackground();

	M_DrawCrispnessHeader("Crispness 1/4");

	M_DrawCrispnessSeparator(crispness1_e::crispness_sep_rendering, "Rendering");
	M_DrawCrispnessItem(crispness1_e::crispness_hires, "High Resolution Rendering", crispy->hires, true);
	M_DrawCrispnessMultiItem(crispness1_e::crispness_widescreen, "Widescreen Aspect Ratio", multiitem_widescreen, crispy->widescreen, aspect_ratio_correct);
	M_DrawCrispnessItem(crispness1_e::crispness_uncapped, "Uncapped Framerate", crispy->uncapped, true);
	M_DrawCrispnessItem(crispness1_e::crispness_vsync, "Enable VSync", crispy->vsync, !force_software_renderer);
	M_DrawCrispnessItem(crispness1_e::crispness_smoothscaling, "Smooth Pixel Scaling", crispy->smoothscaling, true);

	M_DrawCrispnessSeparator(crispness1_e::crispness_sep_visual, "Visual");
	M_DrawCrispnessMultiItem(crispness1_e::crispness_coloredhud, "Colorize HUD Elements", multiitem_coloredhud, crispy->coloredhud, true);
	M_DrawCrispnessMultiItem(crispness1_e::crispness_translucency, "Enable Translucency", multiitem_translucency, crispy->translucency, true);
	M_DrawCrispnessItem(crispness1_e::crispness_smoothlight, "Smooth Diminishing Lighting", crispy->smoothlight, true);
	M_DrawCrispnessMultiItem(crispness1_e::crispness_brightmaps, "Apply Brightmaps to", multiitem_brightmaps, crispy->brightmaps, true);
	M_DrawCrispnessItem(crispness1_e::crispness_coloredblood, "Colored Blood and Corpses", crispy->coloredblood, gameversion != GameVersion::exe_chex);
	M_DrawCrispnessItem(crispness1_e::crispness_flipcorpses, "Randomly Mirrored Corpses", crispy->flipcorpses, gameversion != GameVersion::exe_chex);

	M_DrawCrispnessGoto(crispness1_e::crispness1_next, "Next Page >");
	M_DrawCrispnessGoto(crispness1_e::crispness1_prev, "< Last Page");

	dp_translation = nullptr;
}

static void M_DrawCrispness2()
{
	M_DrawCrispnessBackground();

	M_DrawCrispnessHeader("Crispness 2/4");

	M_DrawCrispnessSeparator(crispness2_e::crispness_sep_audible, "Audible");
	M_DrawCrispnessItem(crispness2_e::crispness_soundfull, "Play sounds in full length", crispy->soundfull, true);
	M_DrawCrispnessItem(crispness2_e::crispness_soundfix, "Misc. Sound Fixes", crispy->soundfix, true);
	M_DrawCrispnessMultiItem(crispness2_e::crispness_sndchannels, "Sound Channels", multiitem_sndchannels, snd_channels >> 4, snd_sfxdevice != snddevice_t::PCSPEAKER);
	M_DrawCrispnessItem(crispness2_e::crispness_soundmono, "Mono SFX", crispy->soundmono, true);

	M_DrawCrispnessSeparator(crispness2_e::crispness_sep_navigational, "Navigational");
	M_DrawCrispnessItem(crispness2_e::crispness_extautomap, "Extended Automap colors", crispy->extautomap, true);
	M_DrawCrispnessItem(crispness2_e::crispness_smoothmap, "Smooth automap lines", crispy->smoothmap, true);
	M_DrawCrispnessMultiItem(crispness2_e::crispness_automapstats, "Show Level Stats", multiitem_widgets, crispy->automapstats, true);
	M_DrawCrispnessMultiItem(crispness2_e::crispness_leveltime, "Show Level Time", multiitem_widgets, crispy->leveltime, true);
	M_DrawCrispnessMultiItem(crispness2_e::crispness_playercoords, "Show Player Coords", multiitem_widgets, crispy->playercoords, true);
	M_DrawCrispnessMultiItem(crispness2_e::crispness_secretmessage, "Report Revealed Secrets", multiitem_secretmessage, crispy->secretmessage, true);

	M_DrawCrispnessGoto(crispness2_e::crispness2_next, "Next Page >");
	M_DrawCrispnessGoto(crispness2_e::crispness2_prev, "< Prev Page");

	dp_translation = nullptr;
}

static void M_DrawCrispness3()
{
	M_DrawCrispnessBackground();

	M_DrawCrispnessHeader("Crispness 3/4");

	M_DrawCrispnessSeparator(crispness3_e::crispness_sep_tactical, "Tactical");

	M_DrawCrispnessMultiItem(crispness3_e::crispness_freelook, "Allow Free Look", multiitem_freelook, crispy->freelook, true);
	M_DrawCrispnessItem(crispness3_e::crispness_mouselook, "Permanent Mouse Look", crispy->mouselook, true);
	M_DrawCrispnessMultiItem(crispness3_e::crispness_bobfactor, "Player View/Weapon Bobbing", multiitem_bobfactor, crispy->bobfactor, true);
	M_DrawCrispnessMultiItem(crispness3_e::crispness_centerweapon, "Weapon Attack Alignment", multiitem_centerweapon, crispy->centerweapon, crispy->bobfactor != BOBFACTOR_OFF);
	M_DrawCrispnessItem(crispness3_e::crispness_weaponsquat, "Squat weapon down on impact", crispy->weaponsquat, true);
	M_DrawCrispnessItem(crispness3_e::crispness_pitch, "Weapon Recoil Pitch", crispy->pitch, true);
	M_DrawCrispnessItem(crispness3_e::crispness_neghealth, "Negative Player Health", crispy->neghealth, true);
	// M_DrawCrispnessItem(crispness_extsaveg, "Extended Savegames", crispy->extsaveg, true);

	M_DrawCrispnessSeparator(crispness3_e::crispness_sep_crosshair, "Crosshair");

	M_DrawCrispnessMultiItem(crispness3_e::crispness_crosshair, "Draw Crosshair", multiitem_crosshair, crispy->crosshair, true);
	M_DrawCrispnessMultiItem(crispness3_e::crispness_crosshairtype, "Crosshair Shape", multiitem_crosshairtype, crispy->crosshairtype + 1, crispy->crosshair);
	M_DrawCrispnessItem(crispness3_e::crispness_crosshairhealth, "Color indicates Health", crispy->crosshairhealth, crispy->crosshair);
	M_DrawCrispnessItem(crispness3_e::crispness_crosshairtarget, "Highlight on target", crispy->crosshairtarget, crispy->crosshair);

	M_DrawCrispnessGoto(crispness3_e::crispness3_next, "Next Page >");
	M_DrawCrispnessGoto(crispness3_e::crispness3_prev, "< Prev Page");

	dp_translation = nullptr;
}

static void M_DrawCrispness4()
{
	M_DrawCrispnessBackground();

	M_DrawCrispnessHeader("Crispness 4/4");

	M_DrawCrispnessSeparator(crispness4_e::crispness_sep_physical, "Physical");

	M_DrawCrispnessMultiItem(crispness4_e::crispness_freeaim, "Vertical Aiming", multiitem_freeaim, crispy->freeaim, crispy->singleplayer);
	M_DrawCrispnessMultiItem(crispness4_e::crispness_jumping, "Allow Jumping", multiitem_jump, crispy->jump, crispy->singleplayer);
	M_DrawCrispnessItem(crispness4_e::crispness_overunder, "Walk over/under Monsters", crispy->overunder, crispy->singleplayer);
	M_DrawCrispnessItem(crispness4_e::crispness_recoil, "Weapon Recoil Thrust", crispy->recoil, crispy->singleplayer);

	M_DrawCrispnessSeparator(crispness4_e::crispness_sep_demos, "Demos");

	M_DrawCrispnessMultiItem(crispness4_e::crispness_demotimer, "Show Demo Timer", multiitem_demotimer, crispy->demotimer, true);
	M_DrawCrispnessMultiItem(crispness4_e::crispness_demotimerdir, "Playback Timer Direction", multiitem_demotimerdir, crispy->demotimerdir + 1, crispy->demotimer & DEMOTIMER_PLAYBACK);
	M_DrawCrispnessItem(crispness4_e::crispness_demobar, "Show Demo Progress Bar", crispy->demobar, true);
	M_DrawCrispnessItem(crispness4_e::crispness_demousetimer, "\"Use\" Button Timer", crispy->btusetimer, true);

	M_DrawCrispnessGoto(crispness4_e::crispness4_next, "First Page >");
	M_DrawCrispnessGoto(crispness4_e::crispness4_prev, "< Prev Page");

	dp_translation = nullptr;
}

void M_Options(int choice)
{
	M_SetupNextMenu(&OptionsDef);
}

// correctly handle inverted y-axis
static void M_Mouse(int choice)
{
	if (mouseSensitivity_y < 0)
	{
		mouseSensitivity_y = -mouseSensitivity_y;
		mouse_y_invert = 1;
	}

	if (mouse_acceleration_y < 0)
	{
		mouse_acceleration_y = -mouse_acceleration_y;
		mouse_y_invert = 1;
	}

	M_SetupNextMenu(&MouseDef);
}

static void M_CrispnessCur(int choice)
{
	M_SetupNextMenu(CrispnessMenus[crispness_cur]);
}

static void M_CrispnessNext(int choice)
{
	if (++crispness_cur > arrlen(CrispnessMenus) - 1)
	{
		crispness_cur = 0;
	}

	M_CrispnessCur(0);
}

static void M_CrispnessPrev(int choice)
{
	if (--crispness_cur < 0)
	{
		crispness_cur = arrlen(CrispnessMenus) - 1;
	}

	M_CrispnessCur(0);
}

void M_ChangeMessages(int choice)
{
	// warning: unused parameter `int choice'
	choice = 0;
	showMessages = 1 - showMessages;

	if (!showMessages)
	{
		players[consoleplayer].message = DEH_String(MSGOFF);
	}
	else
	{
		players[consoleplayer].message = DEH_String(MSGON);
	}

	message_dontfuckwithme = true;
}

void M_EndGameResponse(int key)
{
	if (key != key_menu_confirm)
	{
		return;
	}

	// killough 5/26/98: make endgame quit if recording or playing back demo
	if (demorecording || singledemo)
	{
		G_CheckDemoStatus();
	}

	// clear quicksave slot
	quickSaveSlot = -1;
	currentMenu->lastOn = itemOn;
	M_ClearMenus();
	D_StartTitle();
}

void M_EndGame(int choice)
{
	choice = 0;
	if (!usergame)
	{
		S_StartSound(nullptr, sfxenum_t::sfx_oof);
		return;
	}

	if (netgame)
	{
		M_StartMessage(DEH_String(NETEND), nullptr, false);
		return;
	}

	M_StartMessage(DEH_String(ENDGAME), M_EndGameResponse, true);
}

void M_ReadThis(int choice)
{
	choice = 0;
	M_SetupNextMenu(&ReadDef1);
}

void M_ReadThis2(int choice)
{
	choice = 0;
	M_SetupNextMenu(&ReadDef2);
}

void M_FinishReadThis(int choice)
{
	choice = 0;
	M_SetupNextMenu(&MainDef);
}

::std::array<sfxenum_t, 8> quitsounds{
	sfxenum_t::sfx_pldeth,
	sfxenum_t::sfx_dmpain,
	sfxenum_t::sfx_popain,
	sfxenum_t::sfx_slop,
	sfxenum_t::sfx_telept,
	sfxenum_t::sfx_posit1,
	sfxenum_t::sfx_posit3,
	sfxenum_t::sfx_sgtatk
};

::std::array<sfxenum_t, 8> quitsounds2{
	sfxenum_t::sfx_vilact,
	sfxenum_t::sfx_getpow,
	sfxenum_t::sfx_boscub,
	sfxenum_t::sfx_slop,
	sfxenum_t::sfx_skeswg,
	sfxenum_t::sfx_kntdth,
	sfxenum_t::sfx_bspact,
	sfxenum_t::sfx_sgtatk
};

void M_QuitResponse(int key)
{
	extern int show_endoom;

	if (key != key_menu_confirm)
	{
		return;
	}

	// play quit sound only if the ENDOOM screen is also shown
	if (!netgame && show_endoom)
	{
		if (gamemode == GameMode::commercial)
		{
			S_StartSound(nullptr, quitsounds2[(gametic >> 2) & 7]);
		}
		else
		{
			S_StartSound(nullptr, quitsounds[(gametic >> 2) & 7]);
		}

		I_WaitVBL(105);
	}

	I_Quit();
}

static ::std::string M_SelectEndMessage()
{
	CHAR_PTR* endmsg;

	if (logical_gamemission == GameMission::doom)
	{
		// Doom 1
		endmsg = doom1_endmsg;
	}
	else
	{
		// Doom 2
		endmsg = doom2_endmsg;
	}

	return endmsg[gametic % NUM_QUITMESSAGES];
}

void M_QuitDOOM(int choice)
{
	// fast exit if "run" key is held down
	if (speedkeydown())
	{
		I_Quit();
	}

	DEH_snprintf(endstring, sizeof(endstring), "%s\n\n" DOSY, DEH_String(M_SelectEndMessage()));

	M_StartMessage(endstring, M_QuitResponse, true);
}

void M_ChangeSensitivity(int choice)
{
	switch (choice)
	{
	case 0:
		if (mouseSensitivity)
		{
			--mouseSensitivity;
		}
		break;
	case 1:
		if (mouseSensitivity < 255) // extended range
		{
			++mouseSensitivity;
		}
		break;
	}
}

void M_ChangeSensitivity_x2(int choice)
{
	switch (choice)
	{
	case 0:
		if (mouseSensitivity_x2)
		{
			--mouseSensitivity_x2;
		}
		break;
	case 1:
		if (mouseSensitivity_x2 < 255) // extended range
		{
			++mouseSensitivity_x2;
		}
		break;
	}
}

static void M_ChangeSensitivity_y(int choice)
{
	switch (choice)
	{
	case 0:
		if (mouseSensitivity_y)
		{
			--mouseSensitivity_y;
		}
		break;
	case 1:
		if (mouseSensitivity_y < 255) // extended range
		{
			++mouseSensitivity_y;
		}
		break;
	}
}

static void M_MouseInvert(int choice)
{
	choice = 0;
	mouse_y_invert = !mouse_y_invert;
}

void M_ChangeDetail(int choice)
{
	choice = 0;
	detailLevel = 1 - detailLevel;

	R_SetViewSize(screenblocks, detailLevel);

	if (!detailLevel)
	{
		players[consoleplayer].message = DEH_String(DETAILHI);
	}
	else
	{
		players[consoleplayer].message = DEH_String(DETAILLO);
	}
}

void M_SizeDisplay(int choice)
{
	switch (choice)
	{
	case 0:
		if (screenSize > 0)
		{
			--screenblocks;
			--screenSize;
		}
		break;
	case 1:
		if (screenSize < 8 + (crispy->widescreen ? 6 : 3)) // Crispy HUD
		{
			++screenblocks;
			++screenSize;
		}
		// reset to fullscreen HUD
		else
		{
			screenblocks = 11;
			screenSize = 8;
		}
		break;
	}

	R_SetViewSize(screenblocks, detailLevel);
}

// These keys evaluate to a "null" key in Vanilla Doom that allows weird jumping in the menus. Preserve this behavior for accuracy.
static bool IsNullKey(int key)
{
	return key == Keys::PAUSE || key == Keys::CAPSLOCK || key == Keys::SCROLLLOCK || key == Keys::NUMLOCK;
}

// reload current level / go to next level adapted from prboom-plus/src/e6y.c:369-449
static int G_ReloadLevel()
{
	bool result{false};

	if (gamestate == GameState_t::GS_LEVEL)
	{
		// restart demos from the map they were started
		if (demorecording)
		{
			gamemap = startmap;
		}

		G_DeferedInitNew(gameskill, gameepisode, gamemap);
		result = true;
	}

	return result;
}

static int G_GotoNextLevel()
{
	byte doom_next[5][9] = {
		{12, 13, 19, 15, 16, 17, 18, 21, 14},
		{22, 23, 24, 25, 29, 27, 28, 31, 26},
		{32, 33, 34, 35, 36, 39, 38, 41, 37},
		{42, 49, 44, 45, 46, 47, 48, 51, 43},
		{52, 53, 54, 55, 56, 59, 58, 11, 57},
	};
	byte doom2_next[33] = {
		2, 3, 4, 5, 6, 7, 8, 9, 10, 11,
		12, 13, 14, 15, 31, 17, 18, 19, 20, 21,
		22, 23, 24, 25, 26, 27, 28, 29, 30, 1,
		32, 16, 3
	};
	byte nerve_next[9] = {
		2, 3, 4, 9, 6, 7, 8, 1, 5
	};

	int changed = false;

	if (gamemode == GameMode::commercial)
	{
		if (crispy->havemap33)
		{
			doom2_next[1] = 33;
		}

		if (W_CheckNumForName("map31") < 0)
		{
			if (gamemission == GameMission::pack_hacx)
			{
				doom2_next[30] = 16;
				doom2_next[20] = 1;
			}
		}

		if (gamemission == GameMission::pack_master)
		{
			doom2_next[1] = 3;
			doom2_next[14] = 16;
			doom2_next[20] = 1;
		}
	}
	else
	{
		if (gamemode == GameMode::shareware)
		{
			doom_next[0][7] = 11;
		}

		if (gamemode == GameMode::registered)
		{
			doom_next[2][7] = 11;
		}

		if (!crispy->haved1e5)
		{
			doom_next[3][7] = 11;
		}

		if (gameversion == GameVersion::exe_chex)
		{
			doom_next[0][2] = 14;
			doom_next[0][4] = 11;
		}
	}

	if (gamestate == GameState_t::GS_LEVEL)
	{
		int epsd;
		int map;

		if (gamemode == GameMode::commercial)
		{
			epsd = gameepisode;
			if (gamemission == GameMission::pack_nerve)
			{
				map = nerve_next[gamemap - 1];
			}
			else
			{
				map = doom2_next[gamemap - 1];
			}
		}
		else
		{
			epsd = doom_next[gameepisode - 1][gamemap - 1] / 10;
			map = doom_next[gameepisode - 1][gamemap - 1] % 10;
		}

		// special-casing for E1M10 "Sewers" support
		if (crispy->havee1m10 && gameepisode == 1)
		{
			if (gamemap == 1)
			{
				map = 10;
			}
			else if (gamemap == 10)
			{
				epsd = 1;
				map = 2;
			}
		}

		G_DeferedInitNew(gameskill, epsd, map);
		changed = true;
	}

	return changed;
}

//
// CONTROL PANEL
//

bool M_Responder(EventType* ev)
{
	int ch;
	int key;
	int i;
	static int mousewait = 0;
	static int mousey = 0;
	static int lasty = 0;
	static int mousex = 0;
	static int lastx = 0;

	// In testcontrols mode, none of the function keys should do anything - the only key is escape to quit.
	if (testcontrols)
	{
		if (ev->type == evtype_t::ev_quit || (ev->type == evtype_t::ev_keydown && (ev->data1 == key_menu_activate || ev->data1 == key_menu_quit)))
		{
			I_Quit();
			return true;
		}

		return false;
	}

	// "close" button pressed on window?
	if (ev->type == evtype_t::ev_quit)
	{
		// First click on close button = bring up quit confirm message. Second click on close button = confirm quit
		if (menuactive && messageToPrint && messageRoutine == M_QuitResponse)
		{
			M_QuitResponse(key_menu_confirm);
		}
		else
		{
			S_StartSound(nullptr, sfxenum_t::sfx_swtchn);
			M_QuitDOOM(0);
		}

		return true;
	}

	// key is the key pressed, ch is the actual character typed
	ch = 0;
	key = -1;

	if (ev->type == evtype_t::ev_joystick)
	{
		// Simulate key presses from joystick events to interact with the menu.
		if (menuactive)
		{
			if (ev->data3 < 0)
			{
				key = key_menu_up;
				joywait = I_GetTime() + 5;
			}
			else if (ev->data3 > 0)
			{
				key = key_menu_down;
				joywait = I_GetTime() + 5;
			}
			if (ev->data2 < 0)
			{
				key = key_menu_left;
				joywait = I_GetTime() + 2;
			}
			else if (ev->data2 > 0)
			{
				key = key_menu_right;
				joywait = I_GetTime() + 2;
			}

#define JOY_BUTTON_MAPPED(x) ((x) >= 0)
#define JOY_BUTTON_PRESSED(x) (JOY_BUTTON_MAPPED(x) && (ev->data1 & (1 << (x))) != 0)

			if (JOY_BUTTON_PRESSED(joybfire))
			{
				// Simulate a 'Y' keypress when Doom show a Y/N dialog with Fire button.
				if (messageToPrint && messageNeedsInput)
				{
					key = key_menu_confirm;
				}
				// Simulate pressing "Enter" when we are supplying a save slot name
				else if (saveStringEnter)
				{
					key = Keys::ENTER;
				}
				else
				{
					// if selecting a save slot via joypad, set a flag
					if (currentMenu == &SaveDef)
					{
						joypadSave = true;
					}
					key = key_menu_forward;
				}
				joywait = I_GetTime() + 5;
			}
			if (JOY_BUTTON_PRESSED(joybuse))
			{
				// Simulate a 'N' keypress when Doom show a Y/N dialog with Use button.
				if (messageToPrint && messageNeedsInput)
				{
					key = key_menu_abort;
				}
				// If user was entering a save name, back out
				else if (saveStringEnter)
				{
					key = Keys::ESCAPE;
				}
				else
				{
					key = key_menu_back;
				}
				joywait = I_GetTime() + 5;
			}
		}
		if (JOY_BUTTON_PRESSED(joybmenu))
		{
			key = key_menu_activate;
			joywait = I_GetTime() + 5;
		}
	}
	else
	{
		if (ev->type == evtype_t::ev_mouse && mousewait < I_GetTime())
		{
			// novert disables controlling the menus with the mouse
			if (!novert)
			{
				mousey += ev->data3;
			}
			if (mousey < lasty - 30)
			{
				key = key_menu_down;
				mousewait = I_GetTime() + 5;
				mousey = lasty -= 30;
			}
			else if (mousey > lasty + 30)
			{
				key = key_menu_up;
				mousewait = I_GetTime() + 5;
				mousey = lasty += 30;
			}

			mousex += ev->data2;
			if (mousex < lastx - 30)
			{
				key = key_menu_left;
				mousewait = I_GetTime() + 5;
				mousex = lastx -= 30;
			}
			else if (mousex > lastx + 30)
			{
				key = key_menu_right;
				mousewait = I_GetTime() + 5;
				mousex = lastx += 30;
			}

			if (ev->data1 & 1)
			{
				key = key_menu_forward;
				mousewait = I_GetTime() + 15;
			}

			if (ev->data1 & 2)
			{
				key = key_menu_back;
				mousewait = I_GetTime() + 15;
			}

			// scroll menus with mouse wheel
			if (mousebprevweapon >= 0 && ev->data1 & (1 << mousebprevweapon))
			{
				key = key_menu_down;
				mousewait = I_GetTime() + 1;
			}
			else if (mousebnextweapon >= 0 && ev->data1 & (1 << mousebnextweapon))
			{
				key = key_menu_up;
				mousewait = I_GetTime() + 1;
			}
		}
		else
		{
			if (ev->type == evtype_t::ev_keydown)
			{
				key = ev->data1;
				ch = ev->data2;
			}
		}
	}

	if (key == -1)
	{
		return false;
	}

	// Save Game string input
	if (saveStringEnter)
	{
		switch (key)
		{
		case Keys::BACKSPACE:
			if (saveCharIndex > 0)
			{
				--saveCharIndex;
				savegamestrings[saveSlot][saveCharIndex] = 0;
			}
			break;

		case Keys::ESCAPE:
			saveStringEnter = 0;
			I_StopTextInput();
			M_StringCopy(savegamestrings[saveSlot], saveOldString, SAVESTRINGSIZE);
			break;

		case Keys::ENTER:
			saveStringEnter = 0;
			I_StopTextInput();
			if (savegamestrings[saveSlot][0])
			{
				M_DoSave(saveSlot);
			}
			break;

		default:
			// Savegame name entry. This is complicated.
			// Vanilla has a bug where the shift key is ignored when entering
			// a savegame name. If vanilla_keyboard_mapping is on, we want
			// to emulate this bug by using ev->data1. But if it's turned off,
			// it implies the user doesn't care about Vanilla emulation:
			// instead, use ev->data3 which gives the fully-translated and
			// modified key input.

			if (ev->type != evtype_t::ev_keydown)
			{
				break;
			}
			if (vanilla_keyboard_mapping)
			{
				ch = ev->data1;
			}
			else
			{
				ch = ev->data3;
			}

			ch = toupper(ch);

			if (ch != ' ' && (ch - HU_FONTSTART < 0 || ch - HU_FONTSTART >= HU_FONTSIZE))
			{
				break;
			}

			if (ch >= 32 && ch <= 127 && saveCharIndex < SAVESTRINGSIZE - 1 && M_StringWidth(savegamestrings[saveSlot]) < (SAVESTRINGSIZE - 2) * 8)
			{
				savegamestrings[saveSlot][saveCharIndex++] = ch;
				savegamestrings[saveSlot][saveCharIndex] = 0;
			}
			break;
		}

		return true;
	}

	// Take care of any messages that need input
	if (messageToPrint)
	{
		if (messageNeedsInput)
		{
			if (key != ' ' && key != Keys::ESCAPE && key != key_menu_confirm && key != key_menu_abort)
			{
				return false;
			}
		}

		menuactive = messageLastMenuActive;
		if (messageRoutine)
		{
			messageRoutine(key);
		}

		// stay in menu
		if (messageToPrint < 2)
		{
			menuactive = false;
		}

		messageToPrint = 0; // moved here
		S_StartSound(nullptr, sfxenum_t::sfx_swtchx);

		return true;
	}

	// take screen shot without weapons and HUD
	if (key != 0 && key == key_menu_cleanscreenshot)
	{
		crispy->cleanscreenshot = (screenblocks > 10) ? 2 : 1;
	}

	if ((devparm && key == key_menu_help) || (key != 0 && (key == key_menu_screenshot || key == key_menu_cleanscreenshot)))
	{
		G_ScreenShot();
		return true;
	}

	// F-Keys
	if (!menuactive)
	{
		if (key == key_menu_decscreen)		// Screen size down
		{
			if (automapactive || chat_on)
			{
				return false;
			}

			M_SizeDisplay(0);
			S_StartSound(nullptr, sfxenum_t::sfx_stnmov);
			return true;
		}
		else if (key == key_menu_incscreen) // Screen size up
		{
			if (automapactive || chat_on)
			{
				return false;
			}

			M_SizeDisplay(1);
			S_StartSound(nullptr, sfxenum_t::sfx_stnmov);
			return true;
		}
		else if (key == key_menu_help)		// Help key
		{
			M_StartControlPanel();

			if (gameversion >= GameVersion::exe_ultimate)
			{
				currentMenu = &ReadDef2;
			}
			else
			{
				currentMenu = &ReadDef1;
			}

			itemOn = 0;
			S_StartSound(nullptr, sfxenum_t::sfx_swtchn);
			return true;
		}
		else if (key == key_menu_save)		// Save
		{
			M_StartControlPanel();
			S_StartSound(nullptr, sfxenum_t::sfx_swtchn);
			M_SaveGame(0);
			return true;
		}
		else if (key == key_menu_load)		// Load
		{
			M_StartControlPanel();
			S_StartSound(nullptr, sfxenum_t::sfx_swtchn);
			M_LoadGame(0);
			return true;
		}
		else if (key == key_menu_volume)	// Sound Volume
		{
			M_StartControlPanel();
			currentMenu = &SoundDef;
			itemOn = (int)sound_e::sfx_vol;
			S_StartSound(nullptr, sfxenum_t::sfx_swtchn);
			return true;
		}
		else if (key == key_menu_detail)	// Detail toggle
		{
			M_ChangeDetail(0);
			S_StartSound(nullptr, sfxenum_t::sfx_swtchn);
			return true;
		}
		else if (key == key_menu_qsave)	// Quicksave
		{
			S_StartSound(nullptr, sfxenum_t::sfx_swtchn);
			M_QuickSave();
			return true;
		}
		else if (key == key_menu_endgame) // End game
		{
			S_StartSound(nullptr, sfxenum_t::sfx_swtchn);
			M_EndGame(0);
			return true;
		}
		else if (key == key_menu_messages) // Toggle messages
		{
			M_ChangeMessages(0);
			S_StartSound(nullptr, sfxenum_t::sfx_swtchn);
			return true;
		}
		else if (key == key_menu_qload)	// Quickload
		{
			S_StartSound(nullptr, sfxenum_t::sfx_swtchn);
			M_QuickLoad();
			return true;
		}
		else if (key == key_menu_quit)		// Quit DOOM
		{
			S_StartSound(nullptr, sfxenum_t::sfx_swtchn);
			M_QuitDOOM(0);
			return true;
		}
		else if (key == key_menu_gamma)	// gamma toggle
		{
			++usegamma;
			if (usegamma > 4 + 4) // intermediate gamma levels
			{
				usegamma = 0;
			}
			players[consoleplayer].message = DEH_String(gammamsg[usegamma]);
#ifndef CRISPY_TRUECOLOR
			I_SetPalette(W_CacheLumpName<patch_t>(DEH_String("PLAYPAL"), pu_tags_t::PU_CACHE));
#else
			{
				extern void R_InitColormaps();
				I_SetPalette(0);
				R_InitColormaps();
				inhelpscreens = true;
				R_FillBackScreen();
				viewactive = false;
			}
#endif
			return true;
		}
		// those two can be considered as shortcuts for the IDCLEV cheat and should be treated as such, i.e. add "if (!netgame)"
		else if (!netgame && key != 0 && key == key_menu_reloadlevel)
		{
			if (G_ReloadLevel())
			{
				return true;
			}
		}
		else if (!netgame && key != 0 && key == key_menu_nextlevel)
		{
			if (G_GotoNextLevel())
			{
				return true;
			}
		}

	}

	// Pop-up menu?
	if (!menuactive)
	{
		if (key == key_menu_activate)
		{
			M_StartControlPanel();
			S_StartSound(nullptr, sfxenum_t::sfx_swtchn);
			return true;
		}
		return false;
	}

	// Keys usable within menu
	if (key == key_menu_down)
	{
		// Move down to next item
		do
		{
			if (itemOn + 1 > currentMenu->numitems - 1)
			{
				itemOn = 0;
			}
			else
			{
				++itemOn;
			}
			S_StartSound(nullptr, sfxenum_t::sfx_pstop);
		} while (currentMenu->menuitems[itemOn].status == -1);

		return true;
	}
	else if (key == key_menu_up)
	{
		// Move back up to previous item
		do
		{
			if (!itemOn)
			{
				itemOn = currentMenu->numitems - 1;
			}
			else
			{
				--itemOn;
			}
			S_StartSound(nullptr, sfxenum_t::sfx_pstop);
		} while (currentMenu->menuitems[itemOn].status == -1);

		return true;
	}
	else if (key == key_menu_left)
	{
		// Slide slider left
		if (currentMenu->menuitems[itemOn].routine && currentMenu->menuitems[itemOn].status == 2)
		{
			S_StartSound(nullptr, sfxenum_t::sfx_stnmov);
			currentMenu->menuitems[itemOn].routine(0);
		}
		return true;
	}
	else if (key == key_menu_right)
	{
		// Slide slider right
		if (currentMenu->menuitems[itemOn].routine && currentMenu->menuitems[itemOn].status == 2)
		{
			S_StartSound(nullptr, sfxenum_t::sfx_stnmov);
			currentMenu->menuitems[itemOn].routine(1);
		}
		return true;
	}
	else if (key == key_menu_forward)
	{
		// Activate menu item
		if (currentMenu->menuitems[itemOn].routine && currentMenu->menuitems[itemOn].status)
		{
			currentMenu->lastOn = itemOn;
			if (currentMenu->menuitems[itemOn].status == 2)
			{
				currentMenu->menuitems[itemOn].routine(1);		// right arrow
				S_StartSound(nullptr, sfxenum_t::sfx_stnmov);
			}
			else
			{
				currentMenu->menuitems[itemOn].routine(itemOn);
				S_StartSound(nullptr, sfxenum_t::sfx_pistol);
			}
		}
		return true;
	}
	else if (key == key_menu_activate)
	{
		// Deactivate menu
		currentMenu->lastOn = itemOn;
		M_ClearMenus();
		S_StartSound(nullptr, sfxenum_t::sfx_swtchx);
		return true;
	}
	else if (key == key_menu_back)
	{
		// Go back to previous menu

		currentMenu->lastOn = itemOn;
		if (currentMenu->prevMenu)
		{
			currentMenu = currentMenu->prevMenu;
			itemOn = currentMenu->lastOn;
			S_StartSound(nullptr, sfxenum_t::sfx_swtchn);
		}
		return true;
	}
	// delete a savegame
	else if (key == key_menu_del)
	{
		if (currentMenu == &LoadDef || currentMenu == &SaveDef)
		{
			if (LoadMenu[itemOn].status)
			{
				currentMenu->lastOn = itemOn;
				M_ConfirmDeleteGame();
				return true;
			}
			else
			{
				S_StartSound(nullptr, sfxenum_t::sfx_oof);
			}
		}
	}
	// next/prev Crispness menu
	else if (key == Keys::PAGEUP)
	{
		currentMenu->lastOn = itemOn;
		if (currentMenu == CrispnessMenus[crispness_cur])
		{
			M_CrispnessPrev(0);
			S_StartSound(nullptr, sfxenum_t::sfx_swtchn);
			return true;
		}
	}
	else if (key == Keys::PAGEDOWN)
	{
		currentMenu->lastOn = itemOn;
		if (currentMenu == CrispnessMenus[crispness_cur])
		{
			M_CrispnessNext(0);
			S_StartSound(nullptr, sfxenum_t::sfx_swtchn);
			return true;
		}
	}

	// Keyboard shortcut?
	// Vanilla Doom has a weird behavior where it jumps to the scroll bars
	// when the certain keys are pressed, so emulate this.

	else if (ch != 0 || IsNullKey(key))
	{
		for (size_t i{itemOn + 1};i < currentMenu->numitems; ++i)
		{
			if (currentMenu->menuitems[i].alphaKey == ch)
			{
				itemOn = i;
				S_StartSound(nullptr, sfxenum_t::sfx_pstop);
				return true;
			}
		}

		for (size_t i{0};i <= itemOn; ++i)
		{
			if (currentMenu->menuitems[i].alphaKey == ch)
			{
				itemOn = i;
				S_StartSound(nullptr, sfxenum_t::sfx_pstop);
				return true;
			}
		}
	}

	return false;
}

//
// M_StartControlPanel
//
void M_StartControlPanel()
{
	// intro might call this repeatedly
	if (menuactive)
	{
		return;
	}

	// entering menus while recording demos pauses the game
	if (demorecording && !paused)
	{
		sendpause = true;
	}

	menuactive = 1;
	currentMenu = &MainDef;			// JDC
	itemOn = currentMenu->lastOn;	// JDC
}

// Display OPL debug messages - hack for GENMIDI development.
static void M_DrawOPLDev()
{
	extern void I_OPL_DevMessages(::std::string, size_t);
	char debug[1024];
	::std::string curr, p;
	int line;

	I_OPL_DevMessages(debug, sizeof(debug));
	curr = debug;
	line = 0;

	for (;;)
	{
		p = strchr(curr, '\n');

		if (p != nullptr)
		{
			*p = '\0';
		}

		M_WriteText(0, line * 8, curr);
		++line;

		if (p == nullptr)
		{
			break;
		}

		curr = p + 1;
	}
}

// Called after the view has been rendered, but before it has been blitted.
void M_Drawer()
{
	static short x;
	static short y;
	unsigned max;
	char string[80];
	::std::string name;
	int start;

	inhelpscreens = false;

	// Horiz. & Vertically center string and print it.
	if (messageToPrint)
	{
		// draw a background for important questions
		if (messageToPrint == 2)
		{
			M_DrawCrispnessBackground();
		}

		start = 0;
		y = ORIGHEIGHT / 2 - M_StringHeight(messageString) / 2;
		while (messageString[start] != '\0')
		{
			bool foundnewline = false;

			for (size_t i{0}; messageString[start + i] != '\0'; ++i)
			{
				if (messageString[start + i] == '\n')
				{
					M_StringCopy(string, messageString + start, sizeof(string));
					if (i < sizeof(string))
					{
						string[i] = '\0';
					}

					foundnewline = true;
					start += i + 1;
					break;
				}
			}

			if (!foundnewline)
			{
				M_StringCopy(string, messageString + start, sizeof(string));
				start += strlen(string);
			}

			x = ORIGWIDTH / 2 - M_StringWidth(string) / 2;
			M_WriteText(x > 0 ? x : 0, y, string); // prevent negative x-coords
			y += SHORT(hu_font[0]->height);
		}

		return;
	}

	if (opldev)
	{
		M_DrawOPLDev();
	}

	if (!menuactive)
	{
		return;
	}

	if (currentMenu->routine)
	{
		currentMenu->routine();			// call Draw routine
	}

	// DRAW MENU
	x = currentMenu->x;
	y = currentMenu->y;
	max = currentMenu->numitems;

	// check current menu for missing menu graphics lumps - only once
	if (currentMenu->lumps_missing == 0)
	{
		for (size_t i{0}; i < max; ++i)
		{
			if (currentMenu->menuitems[i].name[0])
			{
				if (W_CheckNumForName(currentMenu->menuitems[i].name) < 0)
				{
					++currentMenu->lumps_missing;
				}
			}
		}

		// no lump missing, no need to check again
		if (currentMenu->lumps_missing == 0)
		{
			currentMenu->lumps_missing = -1;
		}
	}

	for (size_t i{0}; i < max; ++i)
	{
		::std::string alttext = currentMenu->menuitems[i].alttext;
		name = DEH_String(currentMenu->menuitems[i].name);

		if (name[0] && (W_CheckNumForName(name) > 0 || alttext))
		{
			// shade unavailable menu items
			if ((currentMenu == &MainDef && i == (int)main_e::savegame && (!usergame || gamestate != GameState_t::GS_LEVEL))
				|| (currentMenu == &OptionsDef && i == (int)options_e::endgame && (!usergame || netgame))
				|| (currentMenu == &MainDef && i == (int)main_e::loadgame && (netgame && !demoplayback))
				|| (currentMenu == &MainDef && i == (int)main_e::newgame && (demorecording || (netgame && !demoplayback))))
			{
				dp_translation = cr[CR_DARK];
			}

			if (W_CheckNumForName(name) > 0 && currentMenu->lumps_missing == -1)
			{
				V_DrawPatchDirect(x, y, W_CacheLumpName<patch_t>(name, pu_tags_t::PU_CACHE));
			}
			else if (!alttext.empty())
			{
				M_WriteText(x, y + 8 - (M_StringHeight(alttext) / 2), alttext);
			}

			dp_translation = nullptr;
		}
		y += LINEHEIGHT;
	}

	// DRAW SKULL
	if (currentMenu == CrispnessMenus[crispness_cur])
	{
		char item[4];
		M_snprintf(item, sizeof(item), "%s>", whichSkull ? crstr[CR_NONE] : crstr[CR_DARK]);
		M_WriteText(currentMenu->x - 8, currentMenu->y + CRISPY_LINEHEIGHT * itemOn, item);
		dp_translation = nullptr;
	}
	else
	{
		V_DrawPatchDirect(x + SKULLXOFF, currentMenu->y - 5 + itemOn * LINEHEIGHT, W_CacheLumpName<patch_t>(DEH_String(skullName[whichSkull]), pu_tags_t::PU_CACHE));
	}
}

void M_Ticker()
{
	if (--skullAnimCounter <= 0)
	{
		whichSkull ^= 1;
		skullAnimCounter = 8;
	}
}

void M_Init()
{
	currentMenu = &MainDef;
	menuactive = 0;
	itemOn = currentMenu->lastOn;
	whichSkull = 0;
	skullAnimCounter = 10;
	screenSize = screenblocks - 3;
	messageToPrint = 0;
	messageString.clear();
	messageLastMenuActive = menuactive;
	quickSaveSlot = -1;

	// Here we could catch other version dependencies, like HELP1/2, and four episodes.
	// The same hacks were used in the original Doom EXEs.
	if (gameversion >= GameVersion::exe_ultimate)
	{
		MainMenu[::std::size_t(main_e::readthis)].routine = M_ReadThis2;
		ReadDef2.prevMenu = nullptr;
	}

	if (gameversion >= GameVersion::exe_final && gameversion <= GameVersion::exe_final2)
	{
		ReadDef2.routine = M_DrawReadThisCommercial;
		// rearrange Skull in Final Doom HELP screen
		ReadDef2.y -= 10;
	}

	if (gamemode == GameMode::commercial)
	{
		MainMenu[::std::size_t(main_e::readthis)] = MainMenu[::std::size_t(main_e::quitdoom)];
		--MainDef.numitems;
		MainDef.y += 8;
		NewDef.prevMenu = &MainDef;
		ReadDef1.routine = M_DrawReadThisCommercial;
		ReadDef1.x = 330;
		ReadDef1.y = 165;
		ReadMenu1[::std::size_t(read_e::rdthsempty1)].routine = M_FinishReadThis;
	}

	// Sigil
	if (!crispy->haved1e5)
	{
		EpiDef.numitems = 4;
	}

	// Versions of doom.exe before the Ultimate Doom release only had
	// three episodes; if we're emulating one of those then don't try
	// to show episode four. If we are, then do show episode four
	// (should crash if missing).
	if (gameversion < GameVersion::exe_ultimate)
	{
		--EpiDef.numitems;
	}
	// chex.exe shows only one episode.
	else if (gameversion == GameVersion::exe_chex)
	{
		EpiDef.numitems = 1;
		// never show the Episode menu
		NewDef.prevMenu = &MainDef;
	}

	// NRFTL / The Master Levels
	if (!crispy->havenerve.empty() || !crispy->havemaster.empty())
	{
		int i;

		NewDef.prevMenu = &EpiDef;
		EpisodeMenu[0].alphaKey = gamevariant == GameVariant::freedm || gamevariant == GameVariant::freedoom ? 'f' : 'h';
		EpisodeMenu[0].alttext = gamevariant == GameVariant::freedm ? "FreeDM" : gamevariant == GameVariant::freedoom ? "Freedoom: Phase 2" : "Hell on Earth";
		EpiDef.numitems = 1;

		if (!crispy->havenerve.empty())
		{
			EpisodeMenu[EpiDef.numitems].alphaKey = 'n';
			EpisodeMenu[EpiDef.numitems].alttext = "No Rest for the Living";
			++EpiDef.numitems;

			// render the episode menu with the HUD font if the graphics are not from the BFG Edition Doom 2 IWAD or from a PWAD
			if (gamevariant != GameVariant::bfgedition && (i = W_CheckNumForName("M_EPI2")) != -1 && W_IsIWADLump(lumpinfo[i]))
			{
				EpiDef.lumps_missing = 1;
			}
		}

		if (!crispy->havemaster.empty())
		{
			EpisodeMenu[EpiDef.numitems].alphaKey = 't';
			EpisodeMenu[EpiDef.numitems].alttext = "The Master Levels";
			EpiDef.numitems++;

			// render the episode menu with the HUD font if the graphics are not from a PWAD
			if ((i = W_CheckNumForName("M_EPI1")) != -1 && W_IsIWADLump(lumpinfo[i]))
			{
				EpiDef.lumps_missing = 1;
			}
		}
	}

	// rearrange Load Game and Save Game menus
	{
		const patch_t* patchl = W_CacheLumpName<patch_t>(DEH_String("M_LOADG"), pu_tags_t::PU_CACHE);
		const patch_t* patchs = W_CacheLumpName<patch_t>(DEH_String("M_SAVEG"), pu_tags_t::PU_CACHE);
		const patch_t* patchm = W_CacheLumpName<patch_t>(DEH_String("M_LSLEFT"), pu_tags_t::PU_CACHE);

		LoadDef_x = (ORIGWIDTH - SHORT(patchl->width)) / 2 + SHORT(patchl->leftoffset);
		SaveDef_x = (ORIGWIDTH - SHORT(patchs->width)) / 2 + SHORT(patchs->leftoffset);
		LoadDef.x = SaveDef.x = (ORIGWIDTH - 24 * 8) / 2 + SHORT(patchm->leftoffset); // see M_DrawSaveLoadBorder()

		short captionheight = MAX(SHORT(patchl->height), SHORT(patchs->height));

		short vstep = ORIGHEIGHT - 32; // ST_HEIGHT
		vstep -= captionheight;
		vstep -= (load_e::load_end - 1) * LINEHEIGHT + SHORT(patchm->height);
		vstep /= 3;

		if (vstep > 0)
		{
			LoadDef_y = vstep + captionheight - SHORT(patchl->height) + SHORT(patchl->topoffset);
			SaveDef_y = vstep + captionheight - SHORT(patchs->height) + SHORT(patchs->topoffset);
			LoadDef.y = SaveDef.y = vstep + captionheight + vstep + SHORT(patchm->topoffset) - 7; // see M_DrawSaveLoadBorder()
			MouseDef.y = LoadDef.y;
		}
	}

	// remove DOS reference from the game quit confirmation dialogs
	if (!M_ParmExists("-nodeh"))
	{
		::std::string string;
		::std::string replace;

		// "i wouldn't leave if i were you.\ndos is much worse."
		string = doom1_endmsg[3];
		if (!DEH_HasStringReplacement(string))
		{
			replace = M_StringReplace(string, "dos", crispy->platform);
			DEH_AddStringReplacement(string, replace);
		}

		// "you're trying to say you like dos\nbetter than me, right?"
		string = doom1_endmsg[4];
		if (!DEH_HasStringReplacement(string))
		{
			replace = M_StringReplace(string, "dos", crispy->platform);
			DEH_AddStringReplacement(string, replace);
		}

		// "don't go now, there's a \ndimensional shambler waiting\nat the dos prompt!"
		string = doom2_endmsg[2];
		if (!DEH_HasStringReplacement(string))
		{
			replace = M_StringReplace(string, "dos", "command");
			DEH_AddStringReplacement(string, replace);
		}
	}

	opldev = M_CheckParm("-opldev") > 0;
}

static void M_ForceLoadGameResponse(int key)
{
	if (key != key_menu_confirm || !savemaplumpinfo)
	{
		// no need to end game anymore when denied to load savegame
		//M_EndGameResponse(key_menu_confirm);
		savewadfilename.clear();

		// reload Load Game menu
		M_StartControlPanel();
		M_LoadGame(0);
		return;
	}

	savewadfilename = W_WadNameForLump(savemaplumpinfo);
	gameaction = GameAction_t::ga_loadgame;
}

// extended savegames
::std::string savegwarning;
void M_ForceLoadGame()
{
	savegwarning =
		savemaplumpinfo ?
		::std::string("This savegame requires the file\n"
			+ crstr[CR_GOLD] + savewadfilename + crstr[CR_NONE] + "\n"
			+ "to restore " + crstr[CR_GOLD] + savemaplumpinfo->name + crstr[CR_NONE] + " .\n\n"
			+ "Continue to restore from\n"
			+ crstr[CR_GOLD] + W_WadNameForLump(savemaplumpinfo) + crstr[CR_NONE] + " ?\n\n"
			+ PRESSYN) :
		::std::string("This savegame requires the file\n"
			+ crstr[CR_GOLD] + savewadfilename + crstr[CR_NONE] + "\n"
			+ "to restore a map that is\n"
			+ "currently not available!\n\n"
			+ PRESSKEY);

	M_StartMessage(savegwarning, M_ForceLoadGameResponse, savemaplumpinfo != NULL);
	messageToPrint = 2;
	S_StartSound(nullptr, sfxenum_t::sfx_swtchn);
}

static void M_ConfirmDeleteGameResponse(int key)
{
	if (key == key_menu_confirm)
	{
		char name[256];

		M_StringCopy(name, P_SaveGameFile(itemOn), sizeof(name));
		remove(name);

		M_ReadSaveStrings();
	}
}

void M_ConfirmDeleteGame()
{
	savegwarning = ::std::string("delete savegame\n\n" + ::std::string(crstr[CR_GOLD]) + savegamestrings[itemOn] + ::std::string(crstr[CR_NONE]) + " ?\n\n" + PRESSYN);

	M_StartMessage(savegwarning, M_ConfirmDeleteGameResponse, true);
	messageToPrint = 2;
	S_StartSound(nullptr, sfxenum_t::sfx_swtchn);
}

// indicate game version mismatch
void M_LoadGameVerMismatch()
{
	M_StartMessage("Game Version Mismatch\n\n"PRESSKEY, NULL, false);
	messageToPrint = 2;
	S_StartSound(nullptr, sfxenum_t::sfx_swtchn);
}

} // end namespace cudadoom
