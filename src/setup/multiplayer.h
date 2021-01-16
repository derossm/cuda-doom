/**********************************************************************************************************************************************\
	Copyright(C) 2005-2014 Simon Howard

	This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License
	as published by the Free Software Foundation; either version 2 of the License, or (at your option) any later version.

	This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
\**********************************************************************************************************************************************/
#pragma once

#include "../../derma/stdafx.h"

#include "../../derma/common.h"

#include "../doom/d_englsh.h"
#include "../doomtype.h"
#include "../d_iwad.h"
#include "../m_config.h"
#include "../m_misc.h"
#include "../m_controls.h"

#include "../net_io.h"
#include "../net_query.h"
#include "../net_petname.h"

#include "mode.h"
#include "execute.h"

#define MULTI_START_HELP_URL "https://www.chocolate-doom.org/setup-multi-start"
#define MULTI_JOIN_HELP_URL "https://www.chocolate-doom.org/setup-multi-join"
#define MULTI_CONFIG_HELP_URL "https://www.chocolate-doom.org/setup-multi-config"
#define LEVEL_WARP_HELP_URL "https://www.chocolate-doom.org/setup-level-warp"

void StartMultiGame(void* widget, void* user_data);
void WarpMenu(void* widget, void* user_data);
void JoinMultiGame(void* widget, void* user_data);
void MultiplayerConfig(void* widget, void* user_data);

void SetChatMacroDefaults();
void SetPlayerNameDefault();

void BindMultiplayerVariables();

namespace cudadoom
{

constexpr size_t NUM_WADS{10};
constexpr size_t NUM_EXTRA_PARAMS{10};

enum class warptype_t
{
	WARP_ExMy,
	WARP_MAPxy
};

// Fallback IWADs to use if no IWADs are detected.
static const ::std::array<iwad_t, 4> fallback_iwads{
	iwad_t{ "doom.wad",		GameMission::doom,		GameMode::registered,	"Doom" },
	iwad_t{ "heretic.wad",	GameMission::heretic,	GameMode::retail,		"Heretic" },
	iwad_t{ "hexen.wad",	GameMission::hexen,		GameMode::commercial,	"Hexen" },
	iwad_t{ "strife1.wad",	GameMission::strife,	GameMode::commercial,	"Strife" }
};

// Array of IWADs found to be installed
static const iwad_t** found_iwads;
static CHAR_PTR* iwad_labels;

// Index of the currently selected IWAD
static int found_iwad_selected{-1};

// Filename to pass to '-iwad'.
static ::std::string iwadfile;

static ::std::array<::std::string, 4> wad_extensions{ "wad", "lmp", "deh", NULL };
static ::std::array<::std::string, 5> doom_skills{ "I'm too young to die.", "Hey, not too rough.", "Hurt me plenty.", "Ultra-Violence.", "NIGHTMARE!" };
static ::std::array<::std::string, 5> chex_skills{ "Easy does it", "Not so sticky", "Gobs of goo", "Extreme ooze", "SUPER SLIMEY!" };
static ::std::array<::std::string, 5> heretic_skills
	{ "Thou needeth a wet-nurse", "Yellowbellies-R-us", "Bringest them oneth", "Thou art a smite-meister", "Black plague possesses thee" };
static ::std::array<::std::string, 5> hexen_fighter_skills{ "Squire", "Knight", "Warrior", "Berserker", "Titan" };
static ::std::array<::std::string, 5> hexen_cleric_skills{ "Altar boy", "Acolyte", "Priest", "Cardinal", "Pope" };
static ::std::array<::std::string, 5> hexen_mage_skills{ "Apprentice", "Enchanter", "Sorceror", "Warlock", "Archimage" };
static ::std::array<::std::string, 5> strife_skills{ "Training", "Rookie", "Veteran", "Elite", "Bloodbath" };
static ::std::array<::std::string, 3> character_classes{ "Fighter", "Cleric", "Mage" };
static ::std::array<::std::string, 4> gamemodes{ "Co-operative", "Deathmatch", "Deathmatch 2.0", "Deathmatch 3.0" };
static ::std::array<::std::string, 2> strife_gamemodes{ "Normal deathmatch", "Items respawn" }; // (altdeath)

::std::string net_player_name;
::std::array<::std::string, 10> chat_macros;
::std::array<::std::string, NUM_WADS> wads;
::std::array<::std::string, NUM_EXTRA_PARAMS> extra_params;

static int character_class{0};
static int skill{2};
static int nomonsters{0};
static int deathmatch{0};
static int strife_altdeath{0};
static int fast{0};
static int respawn{0};
static int udpport{2342};
static TimeType timer{0};
static int privateserver{0};

static txt::DropDownList* skillbutton;
static txt::Button* warpbutton;
static warptype_t warptype{WARP_MAPxy};
static int warpepisode{1};
static int warpmap{1};

// Address to connect to when joining a game
::std::string connect_address{};

static txt::Window* query_window;
static int query_servers_found;

// Find an IWAD from its description
static const iwad_t* GetCurrentIWAD()
{
	return found_iwads[found_iwad_selected];
}

// Is the currently selected IWAD the Chex Quest chex.wad?
static bool IsChexQuest(const iwad_t* iwad)
{
	return iwad->name.compare("chex.wad");
}

static void AddWADs(ExecuteContext* exec)
{
	bool have_wads{false};

	for (size_t i{0}; i < NUM_WADS; ++i)
	{
		if (!wads[i].empty())
		{
			if (!have_wads)
			{
				AddCmdLineParameter(exec, "-file");
			}

			AddCmdLineParameter(exec, "\"%s\"", wads[i]);
		}
	}
}

static void AddExtraParameters(ExecuteContext* exec)
{
	int i;

	for (i = 0; i < NUM_EXTRA_PARAMS; ++i)
	{
		if (extra_params[i] != NULL && strlen(extra_params[i]) > 0)
		{
			AddCmdLineParameter(exec, "%s", extra_params[i]);
		}
	}
}

static void AddIWADParameter(ExecuteContext* exec)
{
	if (iwadfile != NULL)
	{
		AddCmdLineParameter(exec, "-iwad %s", iwadfile);
	}
}

// Callback function invoked to launch the game. This is used when starting a server and also when starting a single player game via the "warp" menu.
static void StartGame(int multiplayer)
{
	ExecuteContext* exec;

	exec = NewExecuteContext();

	// Extra parameters come first, before all others; this way, they can override any of the options set in the dialog.
	AddExtraParameters(exec);

	AddIWADParameter(exec);
	AddCmdLineParameter(exec, "-skill %i", skill + 1);

	if (gamemission == GameMission::hexen)
	{
		AddCmdLineParameter(exec, "-class %i", character_class);
	}

	if (nomonsters)
	{
		AddCmdLineParameter(exec, "-nomonsters");
	}

	if (fast)
	{
		AddCmdLineParameter(exec, "-fast");
	}

	if (respawn)
	{
		AddCmdLineParameter(exec, "-respawn");
	}

	if (warptype == WARP_ExMy)
	{
		// TODO: select IWAD based on warp type
		AddCmdLineParameter(exec, "-warp %i %i", warpepisode, warpmap);
	}
	else if (warptype == WARP_MAPxy)
	{
		AddCmdLineParameter(exec, "-warp %i", warpmap);
	}

	// Multiplayer-specific options:
	if (multiplayer)
	{
		AddCmdLineParameter(exec, "-server");
		AddCmdLineParameter(exec, "-port %i", udpport);

		if (deathmatch == 1)
		{
			AddCmdLineParameter(exec, "-deathmatch");
		}
		else if (deathmatch == 2 || strife_altdeath != 0)
		{
			AddCmdLineParameter(exec, "-altdeath");
		}
		// AX: this is a Crispy-specific change
		else if (deathmatch == 3)
		{
			AddCmdLineParameter(exec, "-dm3");
		}

		if (timer > 0)
		{
			AddCmdLineParameter(exec, "-timer %i", timer);
		}

		if (privateserver)
		{
			AddCmdLineParameter(exec, "-privateserver");
		}
	}

	AddWADs(exec);

	Shutdown();

	M_SaveDefaults();
	PassThroughArguments(exec);

	ExecuteDoom(exec);

	exit(0);
}

static void StartServerGame(Widget* widget, void* unused)
{
	StartGame(1);
}

static void StartSinglePlayerGame(Widget* widget), void* unused)
{
	StartGame(0);
}

static void UpdateWarpButton()
{
	char buf[10];

	if (warptype == WARP_ExMy)
	{
		M_snprintf(buf, sizeof(buf), "E%iM%i", warpepisode, warpmap);
	}
	else if (warptype == WARP_MAPxy)
	{
		M_snprintf(buf, sizeof(buf), "MAP%02i", warpmap);
	}

	txt::SetButtonLabel(warpbutton, buf);
}

static void UpdateSkillButton()
{
	const iwad_t* iwad = GetCurrentIWAD();

	if (IsChexQuest(iwad))
	{
		skillbutton->values = chex_skills;
	}
	else switch (gamemission)
	{
	default:
	case doom:
		skillbutton->values = doom_skills;
		break;

	case heretic:
		skillbutton->values = heretic_skills;
		break;

	case hexen:
		if (character_class == 0)
		{
			skillbutton->values = hexen_fighter_skills;
		}
		else if (character_class == 1)
		{
			skillbutton->values = hexen_cleric_skills;
		}
		else
		{
			skillbutton->values = hexen_mage_skills;
		}
		break;

	case strife:
		skillbutton->values = strife_skills;
		break;
	}
}

static void SetExMyWarp(txt::UNCAST_ARG(widget), void* val)
{
	int l;

	l = (intptr_t)val;

	warpepisode = l / 10;
	warpmap = l % 10;

	UpdateWarpButton();
}

static void SetMAPxyWarp(txt::UNCAST_ARG(widget), void* val)
{
	int l;

	l = (intptr_t)val;

	warpmap = l;

	UpdateWarpButton();
}

static void CloseLevelSelectDialog(txt::UNCAST_ARG(button), txt::UNCAST_ARG(window))
{
	txt::CAST_ARG(txt::Window, window);

	txt::CloseWindow(window);
}

static void LevelSelectDialog(txt::UNCAST_ARG(widget), txt::UNCAST_ARG(user_data))
{
	txt::Window* window;
	txt::Button* button;
	const iwad_t* iwad;
	char buf[10];
	int episodes;
	intptr_t x;
	intptr_t y;
	intptr_t l;
	int i;

	window = txt::NewWindow("Select level");
	iwad = GetCurrentIWAD();

	if (warptype == WARP_ExMy)
	{
		episodes = D_GetNumEpisodes(iwad->mission, iwad->mode);
		txt::SetTableColumns(window, episodes);

		// ExMy levels

		for (y = 1; y < 10; ++y)
		{
			for (x = 1; x <= episodes; ++x)
			{
				if (IsChexQuest(iwad) && (x > 1 || y > 5))
				{
					continue;
				}

				if (!D_ValidEpisodeMap(iwad->mission, iwad->mode, x, y))
				{
					txt::AddWidget(window, NULL);
					continue;
				}

				M_snprintf(buf, sizeof(buf),
					" E%" PRIiPTR "M%" PRIiPTR " ", x, y);
				button = txt::NewButton(buf);
				txt::SignalConnect(button, "pressed",
					SetExMyWarp, (void*)(x * 10 + y));
				txt::SignalConnect(button, "pressed",
					CloseLevelSelectDialog, window);
				txt::AddWidget(window, button);

				if (warpepisode == x && warpmap == y)
				{
					txt::SelectWidget(window, button);
				}
			}
		}
	}
	else
	{
		txt::SetTableColumns(window, 6);

		for (i = 0; i < 60; ++i)
		{
			x = i % 6;
			y = i / 6;

			l = x * 10 + y + 1;

			if (!D_ValidEpisodeMap(iwad->mission, iwad->mode, 1, l))
			{
				txt::AddWidget(window, NULL);
				continue;
			}

			M_snprintf(buf, sizeof(buf), " MAP%02" PRIiPTR " ", l);
			button = txt::NewButton(buf);
			txt::SignalConnect(button, "pressed",
				SetMAPxyWarp, (void*)l);
			txt::SignalConnect(button, "pressed",
				CloseLevelSelectDialog, window);
			txt::AddWidget(window, button);

			if (warpmap == l)
			{
				txt::SelectWidget(window, button);
			}
		}
	}
}

static void IWADSelected(txt::UNCAST_ARG(widget), txt::UNCAST_ARG(unused))
{
	const iwad_t* iwad;

	// Find the iwad_t selected

	iwad = GetCurrentIWAD();

	// Update iwadfile

	iwadfile = iwad->name;
}

// Called when the IWAD button is changed, to update warptype.

static void UpdateWarpType(txt::UNCAST_ARG(widget), txt::UNCAST_ARG(unused))
{
	warptype_t new_warptype;
	const iwad_t* iwad;

	// Get the selected IWAD

	iwad = GetCurrentIWAD();

	// Find the new warp type

	if (D_IsEpisodeMap(iwad->mission))
	{
		new_warptype = WARP_ExMy;
	}
	else
	{
		new_warptype = WARP_MAPxy;
	}

	// Reset to E1M1 / MAP01 when the warp type is changed.

	if (new_warptype != warptype)
	{
		warpepisode = 1;
		warpmap = 1;
	}

	warptype = new_warptype;

	UpdateWarpButton();
	UpdateSkillButton();
}

// Get an IWAD list with a default fallback IWAD that is appropriate
// for the game we are configuring (matches gamemission global variable).

static const iwad_t** GetFallbackIwadList()
{
	static const iwad_t* fallback_iwad_list[2];
	unsigned i;

	// Default to use if we don't find something better.

	fallback_iwad_list[0] = &fallback_iwads[0];
	fallback_iwad_list[1] = NULL;

	for (i = 0; i < arrlen(fallback_iwads); ++i)
	{
		if (gamemission == fallback_iwads[i].mission)
		{
			fallback_iwad_list[0] = &fallback_iwads[i];
			break;
		}
	}

	return fallback_iwad_list;
}

static txt::Widget* IWADSelector()
{
	txt::txt_dropdown_list_t* dropdown;
	txt::Widget* result;
	int num_iwads;
	unsigned i;

	// Find out what WADs are installed

	found_iwads = GetIwads();

	// Build a list of the descriptions for all installed IWADs

	num_iwads = 0;

	for (i = 0; found_iwads[i] != NULL; ++i)
	{
		++num_iwads;
	}

	iwad_labels = static_cast<decltype(iwad_labels)>(malloc(sizeof(*iwad_labels) * num_iwads));

	for (i = 0; i < num_iwads; ++i)
	{
		iwad_labels[i] = found_iwads[i]->description;
	}

	// If no IWADs are found, provide Doom 2 as an option, but
	// we're probably screwed.

	if (num_iwads == 0)
	{
		found_iwads = GetFallbackIwadList();
		num_iwads = 1;
	}

	// Build a dropdown list of IWADs

	if (num_iwads < 2)
	{
		// We have only one IWAD. Show as a label.

		result = (txt::Widget*)txt::NewLabel(found_iwads[0]->description);
	}
	else
	{
		// Dropdown list allowing IWAD to be selected.

		dropdown = txt::NewDropdownList(&found_iwad_selected, iwad_labels, num_iwads);

		txt::SignalConnect(dropdown, "changed", IWADSelected, NULL);

		result = (txt::Widget*)dropdown;
	}

	// The first time the dialog is opened, found_iwad_selected=-1,
	// so select the first IWAD in the list. Don't lose the setting
	// if we close and reopen the dialog.

	if (found_iwad_selected < 0 || found_iwad_selected >= num_iwads)
	{
		found_iwad_selected = 0;
	}

	IWADSelected(NULL, NULL);

	return result;
}

// Create the window action button to start the game. This invokes
// a different callback depending on whether to start a multiplayer
// or single player game.

static txt::WindowAction* StartGameAction(int multiplayer)
{
	txt::WindowAction* action{txt::NewWindowAction(KEY_F10, "Start")};

	txt::WidgetSignalFunc callback;
	if (multiplayer)
	{
		callback = StartServerGame;
	}
	else
	{
		callback = StartSinglePlayerGame;
	}

	txt::SignalConnect(action, "pressed", callback, NULL);

	return action;
}

static void OpenWadsWindow(Widget* widget, void* user_data)
{
	txt::Window* window{txt::NewWindow("Add WADs")};

	for (size_t i{0}; i < NUM_WADS; ++i)
	{
		txt::AddWidget(window, txt::NewFileSelector(&wads[i], 60, "Select a WAD file", wad_extensions));
	}
}

static void OpenExtraParamsWindow(Widget* widget, void* user_data)
{
	txt::Window* window{txt::NewWindow("Extra command line parameters")};

	for (size_t i{0}; i < NUM_EXTRA_PARAMS; ++i)
	{
		txt::AddWidget(window, txt::NewInputBox(&extra_params[i], 70));
	}
}

static txt::WindowAction* WadWindowAction()
{
	txt::WindowAction* action = txt::NewWindowAction('w', "Add WADs");
	txt::SignalConnect(action, "pressed", OpenWadsWindow, NULL);

	return action;
}

static txt::txt_dropdown_list_t* GameTypeDropdown()
{
	switch (gamemission)
	{
	case doom:
	default:
		return txt::NewDropdownList(&deathmatch, gamemodes, 4);

	// Heretic and Hexen don't support Deathmatch II:
	case heretic:
	case hexen:
		return txt::NewDropdownList(&deathmatch, gamemodes, 2);

	// Strife supports both deathmatch modes, but doesn't support multiplayer co-op. Use a different variable to indicate whether to use altdeath or not.
	case strife:
		return txt::NewDropdownList(&strife_altdeath, strife_gamemodes, 2);
	}
}

// "Start game" menu. This is used for the start server window and the single player warp menu.
// The parameters specify the window title and whether to display multiplayer options.
static void StartGameMenu(::std::string window_title, int multiplayer)
{
	txt::Window* window;
	txt::Widget* iwad_selector;

	window = txt::NewWindow(window_title);
	txt::SetTableColumns(window, 2);
	txt::SetColumnWidths(window, 12, 6);

	if (multiplayer)
	{
		txt::SetWindowHelpURL(window, MULTI_START_HELP_URL);
	}
	else
	{
		txt::SetWindowHelpURL(window, LEVEL_WARP_HELP_URL);
	}

	txt::SetWindowAction(window, txt::HORIZ_CENTER, WadWindowAction());
	txt::SetWindowAction(window, txt::HORIZ_RIGHT, StartGameAction(multiplayer));

	txt::AddWidgets(window, txt::NewLabel("Game"), iwad_selector = IWADSelector(), NULL);

	if (gamemission == hexen)
	{
		txt::DropDownList* cc_dropdown;
		txt::AddWidgets(window, txt::NewLabel("Character class "), cc_dropdown = txt::NewDropdownList(&character_class, character_classes, 3), NULL);

		// Update skill level dropdown when the character class is changed:
		txt::SignalConnect(cc_dropdown, "changed", UpdateWarpType, NULL);
	}

	txt::AddWidgets(window,
					txt::NewLabel("Skill"),
					skillbutton = txt::NewDropdownList(&skill, doom_skills, 5),
					txt::NewLabel("Level warp"),
					warpbutton = txt::NewButton2("?", LevelSelectDialog, NULL),
					NULL);

	if (multiplayer)
	{
		txt::AddWidgets(window,
						txt::NewLabel("Game type"),
						GameTypeDropdown(),
						txt::NewLabel("Time limit"),
						txt::MakeHorizontalTable(txt::NewIntInputBox(&timer, 2), txt::NewLabel("minutes"), NULL),
						NULL);
	}

	txt::AddWidgets(window,
					txt::NewSeparator("Monster options"),
					txt::NewInvertedCheckBox("Monsters enabled", &nomonsters),
					txt::TABLE_OVERFLOW_RIGHT,
					txt::NewCheckBox("Fast monsters", &fast),
					txt::TABLE_OVERFLOW_RIGHT,
					txt::NewCheckBox("Respawning monsters", &respawn),
					txt::TABLE_OVERFLOW_RIGHT,
					NULL);

	if (multiplayer)
	{
		txt::AddWidgets(window,
						txt::NewSeparator("Advanced"),
						txt::NewLabel("UDP port"),
						txt::NewIntInputBox(&udpport, 5),
						txt::NewInvertedCheckBox("Register with master server", &privateserver),
						txt::TABLE_OVERFLOW_RIGHT,
						NULL);
	}

	txt::AddWidgets(window, txt::NewButton2("Add extra parameters...", OpenExtraParamsWindow, NULL), txt::TABLE_OVERFLOW_RIGHT, NULL);

	txt::SignalConnect(iwad_selector, "changed", UpdateWarpType, NULL);

	UpdateWarpType(NULL, NULL);
	UpdateWarpButton();
}

void StartMultiGame(Widget* widget, void* user_data)
{
	StartGameMenu("Start multiplayer game", 1);
}

void WarpMenu(Widget* widget, void* user_data)
{
	StartGameMenu("Level Warp", 0);
}

static void DoJoinGame(void* unused1, void* unused2)
{
	ExecuteContext* exec;

	if (connect_address == NULL || strlen(connect_address) <= 0)
	{
		txt::MessageBox(NULL, "Please enter a server address\nto connect to.");
		return;
	}

	exec = NewExecuteContext();

	AddCmdLineParameter(exec, "-connect %s", connect_address);

	if (gamemission == hexen)
	{
		AddCmdLineParameter(exec, "-class %i", character_class);
	}

	// Extra parameters come first, so that they can be used to override the other parameters.
	AddExtraParameters(exec);
	AddIWADParameter(exec);
	AddWADs(exec);

	Shutdown();

	M_SaveDefaults();

	PassThroughArguments(exec);

	ExecuteDoom(exec);

	exit(0);
}

static txt::WindowAction* JoinGameAction()
{
	txt::WindowAction* action{txt::NewWindowAction(KEY_F10, "Connect")};
	txt::SignalConnect(action, "pressed", DoJoinGame, NULL);

	return action;
}

static void SelectQueryAddress(txt::Button* button, net_querydata_t* querydata)
{
	if (querydata->server_state != 0)
	{
		txt::MessageBox("Cannot connect to server", "Gameplay is already in progress\non this server.");
		return;
	}

	// Set address to connect to:
	//free(connect_address);
	connect_address = ::std::string(button->label);

	// Auto-choose IWAD if there is already a player connected.
	if (querydata->num_players > 0)
	{
		size_t i;
		for (i = 0; found_iwads[i] != NULL; ++i)
		{
			if (found_iwads[i]->mode == querydata->gamemode && found_iwads[i]->mission == querydata->gamemission)
			{
				found_iwad_selected = i;
				iwadfile = found_iwads[i]->name;
				break;
			}
		}

		if (found_iwads[i] == NULL)
		{
			txt::MessageBox(NULL,
				"The game on this server seems to be:\n\n	%s\n\nbut the IWAD file %s is not found!\n"
				"Without the required IWAD file, it may not be\npossible to join this game.",
				D_SuggestGameName(querydata->gamemission, querydata->gamemode),
				D_SuggestIWADName(querydata->gamemission, querydata->gamemode));
		}
	}

	// Finished with search.
	txt::CloseWindow(query_window);
}

static void QueryResponseCallback(net_addr_t* addr, net_querydata_t* querydata, TimeType ping_time, txt_table_t* results_table)
{
	::std::string ping_time_str;
	::std::string description;

	// When we connect we'll have to negotiate a common protocol that we can agree upon between the client and server. If we can't then we
	// won't be able to connect, so it's pointless to include it in the results list. If protocol==net_protocol_t::UNKNOWN then this may be
	// an old, pre-3.0 Chocolate Doom server that doesn't support the new protocol negotiation mechanism, or it may be an incompatible fork.
	if (querydata->protocol == net_protocol_t::UNKNOWN)
	{
		return;
	}

	ping_time_str = M_snprintf("%ims", ping_time);

	// Build description from server name field.
	// Because there is limited space, we only include the player count if there are already players connected to the server.
	if (querydata->num_players > 0)
	{
		description = M_snprintf("(%d/%d) ", querydata->num_players, querydata->max_players);
	}
	else
	{
		description = "";
	}

	description = description + querydata->description;

	AddWidgets(results_table, NewLabel(ping_time_str), NewButton2(NET_AddrToString(addr), SelectQueryAddress, querydata), NewLabel(description), NULL);

	++query_servers_found;
}

static void QueryPeriodicCallback(txt_table_t* results_table)
{
	if (!NET_Query_Poll(QueryResponseCallback, results_table))
	{
		txt::SetPeriodicCallback(NULL, NULL, 0);

		if (query_servers_found == 0)
		{
			txt::AddWidgets(results_table,
				txt::TABLE_EMPTY,
				txt::NewLabel("No compatible servers found."),
				NULL
			);
		}
	}
}

static void QueryWindowClosed(Window* window, void* unused)
{
	txt::SetPeriodicCallback(NULL, NULL, 0);
}

static void ServerQueryWindow(::std::string title)
{
	txt::Table* results_table;

	query_servers_found = 0;

	query_window = txt::NewWindow(title);

	txt::AddWidget(query_window, txt::NewScrollPane(70, 10, results_table = txt::NewTable(3)));

	txt::SetColumnWidths(results_table, 7, 22, 40);
	txt::SetPeriodicCallback(QueryPeriodicCallback, results_table, 1);

	txt::SignalConnect(query_window, "closed", QueryWindowClosed, NULL);
}

static void FindInternetServer(Widget* widget, void* user_data)
{
	NET_StartMasterQuery();
	ServerQueryWindow("Find Internet server");
}

static void FindLANServer(Widget* widget, void* user_data)
{
	NET_StartLANQuery();
	ServerQueryWindow("Find LAN server");
}

void JoinMultiGame(Widget* widget, void* user_data)

	txt::Window* window = txt::NewWindow("Join multiplayer game");
	txt::SetTableColumns(window, 2);
	txt::SetColumnWidths(window, 12, 12);

	txt::SetWindowHelpURL(window, MULTI_JOIN_HELP_URL);

	txt::AddWidgets(window, txt::NewLabel("Game"), IWADSelector(), NULL);

	if (gamemission == GameMission::hexen)
	{
		txt::AddWidgets(window, txt::NewLabel("Character class "), txt::NewDropdownList(&character_class, character_classes, 3), NULL);
	}
{
	txt::InputBox* address_box;
	txt::AddWidgets(window,
					txt::NewSeparator("Server"),
					txt::NewLabel("Connect to address: "),
					address_box = txt::NewInputBox(&connect_address, 30),
					txt::NewButton2("Find server on Internet...", FindInternetServer, NULL),
					txt::TABLE_OVERFLOW_RIGHT,
					txt::NewButton2("Find server on local network...", FindLANServer, NULL),
					txt::TABLE_OVERFLOW_RIGHT,
					txt::NewStrut(0, 1),
					txt::TABLE_OVERFLOW_RIGHT,
					txt::NewButton2("Add extra parameters...", OpenExtraParamsWindow, NULL),
					NULL);

	txt::SelectWidget(window, address_box);

	txt::SetWindowAction(window, txt::HORIZ_CENTER, WadWindowAction());
	txt::SetWindowAction(window, txt::HORIZ_RIGHT, JoinGameAction());
}

void SetChatMacroDefaults()
{
	::std::string const defaults[] =
	{
		HUSTR_CHATMACRO0,
		HUSTR_CHATMACRO1,
		HUSTR_CHATMACRO2,
		HUSTR_CHATMACRO3,
		HUSTR_CHATMACRO4,
		HUSTR_CHATMACRO5,
		HUSTR_CHATMACRO6,
		HUSTR_CHATMACRO7,
		HUSTR_CHATMACRO8,
		HUSTR_CHATMACRO9,
	};

	// If the chat macros have not been set, initialize with defaults.
	for (size_t i{0}; i < 10; ++i)
	{
		if (chat_macros[i] == NULL)
		{
			chat_macros[i] = ::std::string(defaults[i]);
		}
	}
}

void SetPlayerNameDefault()
{
	if (net_player_name == NULL)
	{
		net_player_name = NET_GetRandomPetName();
	}
}

void MultiplayerConfig(Widget* widget, void* user_data)
{
	txt::Window* window = txt::NewWindow("Multiplayer Configuration");
	txt::SetWindowHelpURL(window, MULTI_CONFIG_HELP_URL);

	txt::AddWidgets(window,
					txt::NewStrut(0, 1),
					txt::MakeHorizontalTable(txt::NewLabel("Player name: "), txt::NewInputBox(&net_player_name, 25), NULL),
					txt::NewStrut(0, 1),
					txt::NewSeparator("Chat macros"),
					NULL);

	txt::Table* table = txt::NewTable(2);
	txt::Table* label;
	char buf[10];
	for (size_t i{0}; i < 10; ++i)
	{
		M_snprintf(buf, sizeof(buf), "#%i ", i + 1);

		label = txt::NewLabel(buf);
		txt::SetFGColor(label, txt::ColorType::bright_cyan);

		txt::AddWidgets(table, label, txt::NewInputBox(&chat_macros[(i + 1) % 10], 40), NULL);
	}

	txt::AddWidget(window, table);
}

void BindMultiplayerVariables()
{
	char buf[15];

	M_BindStringVariable("player_name", &net_player_name);

	for (size_t i{0}; i < 10; ++i)
	{
		M_snprintf(buf, sizeof(buf), "chatmacro%i", i);
		M_BindStringVariable(buf, &chat_macros[i]);
	}

	switch (gamemission)
	{
	case GameMission::doom:
		M_BindChatControls(4);
		key_multi_msgplayer[0] = 'g';
		key_multi_msgplayer[1] = 'i';
		key_multi_msgplayer[2] = 'b';
		key_multi_msgplayer[3] = 'r';
		break;

	case GameMission::heretic:
		M_BindChatControls(4);
		key_multi_msgplayer[0] = 'g';
		key_multi_msgplayer[1] = 'y';
		key_multi_msgplayer[2] = 'r';
		key_multi_msgplayer[3] = 'b';
		break;

	case GameMission::hexen:
		M_BindChatControls(8);
		key_multi_msgplayer[0] = 'b';
		key_multi_msgplayer[1] = 'r';
		key_multi_msgplayer[2] = 'y';
		key_multi_msgplayer[3] = 'g';
		key_multi_msgplayer[4] = 'j';
		key_multi_msgplayer[5] = 'w';
		key_multi_msgplayer[6] = 'h';
		key_multi_msgplayer[7] = 'p';
		break;

	default:
		break;
	}
}

}
