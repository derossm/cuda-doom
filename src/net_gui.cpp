/**********************************************************************************************************************************************\
	Copyright(C) 2005-2014 Simon Howard

	This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License
	as published by the Free Software Foundation; either version 2 of the License, or (at your option) any later version.

	This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

	DESCRIPTION:
		Graphical stuff related to the networking code:
		The client waiting screen when we are waiting for the server to start the game.
\**********************************************************************************************************************************************/

#include <string>

#include "config.h"
#include "doomkeys.h"

#include "i_system.h"
#include "i_timer.h"
#include "i_video.h"
#include "m_argv.h"
#include "m_misc.h"

#include "net_client.h"
#include "net_gui.h"
#include "net_query.h"
#include "net_server.h"

#include "../textscreen/textscreen.h"

static cudadoom::txt::Window* window;
static int old_max_players;
static cudadoom::txt::txt_label_t* player_labels[NET_MAXPLAYERS];
static cudadoom::txt::txt_label_t* ip_labels[NET_MAXPLAYERS];
static cudadoom::txt::txt_label_t* drone_label;
static cudadoom::txt::txt_label_t* master_msg_label;
static bool had_warning;

// Number of players we expect to be in the game. When the number is
// reached, we auto-start the game (if we're the controller). If
// zero, do not autostart.
static int expected_nodes;

static void EscapePressed(cudadoom::txt::UNCAST_ARG(widget), void* unused)
{
	cudadoom::txt::Shutdown();
	I_Quit();
}

static void StartGame(cudadoom::txt::UNCAST_ARG(widget), cudadoom::txt::UNCAST_ARG(unused))
{
	NET_CL_LaunchGame();
}

static void OpenWaitDialog()
{
	cudadoom::txt::WindowAction* cancel;

	cudadoom::txt::SetDesktopTitle(PACKAGE_STRING);

	window = cudadoom::txt::NewWindow("Waiting for game start...");

	cudadoom::txt::AddWidget(window, cudadoom::txt::NewLabel("\nPlease wait...\n\n"));

	cancel = cudadoom::txt::NewWindowAction(KEY_ESCAPE, "Cancel");
	cudadoom::txt::SignalConnect(cancel, "pressed", EscapePressed, NULL);

	cudadoom::txt::SetWindowAction(window, cudadoom::txt::HORIZ_LEFT, cancel);
	cudadoom::txt::SetWindowPosition(window, cudadoom::txt::HORIZ_CENTER, cudadoom::txt::VERT_BOTTOM,
		cudadoom::txt::SCREEN_W / 2, cudadoom::txt::SCREEN_H - 9);

	old_max_players = 0;
}

static void BuildWindow()
{
	char buf[50];
	cudadoom::txt::txt_table_t* table;
	int i;

	cudadoom::txt::ClearTable(window);
	table = cudadoom::txt::NewTable(3);
	cudadoom::txt::AddWidget(window, table);

	// Add spacers

	cudadoom::txt::AddWidget(table, NULL);
	cudadoom::txt::AddWidget(table, cudadoom::txt::NewStrut(25, 1));
	cudadoom::txt::AddWidget(table, cudadoom::txt::NewStrut(17, 1));

	// Player labels

	for (i = 0; i < net_client_wait_data.max_players; ++i)
	{
		M_snprintf(buf, sizeof(buf), " %i. ", i + 1);
		cudadoom::txt::AddWidget(table, cudadoom::txt::NewLabel(buf));
		player_labels[i] = cudadoom::txt::NewLabel("");
		ip_labels[i] = cudadoom::txt::NewLabel("");
		cudadoom::txt::AddWidget(table, player_labels[i]);
		cudadoom::txt::AddWidget(table, ip_labels[i]);
	}

	drone_label = cudadoom::txt::NewLabel("");

	cudadoom::txt::AddWidget(window, drone_label);
}

static void UpdateGUI()
{
	cudadoom::txt::WindowAction* startgame;
	char buf[50];
	unsigned i;

	// If the value of max_players changes, we must rebuild the
	// contents of the window. This includes when the first
	// waiting data packet is received.

	if (net_client_received_wait_data)
	{
		if (net_client_wait_data.max_players != old_max_players)
		{
			BuildWindow();
		}
	}
	else
	{
		return;
	}

	for (i = 0; i < net_client_wait_data.max_players; ++i)
	{
		cudadoom::txt::ColorType color = cudadoom::txt::ColorType::bright_white;

		if ((int)i == net_client_wait_data.consoleplayer)
		{
			color = cudadoom::txt::ColorType::yellow;
		}

		cudadoom::txt::SetFGColor(player_labels[i], color);
		cudadoom::txt::SetFGColor(ip_labels[i], color);

		if (i < net_client_wait_data.num_players)
		{
			cudadoom::txt::SetLabel(player_labels[i], net_client_wait_data.player_names[i]);
			cudadoom::txt::SetLabel(ip_labels[i], net_client_wait_data.player_addrs[i]);
		}
		else
		{
			cudadoom::txt::SetLabel(player_labels[i], "");
			cudadoom::txt::SetLabel(ip_labels[i], "");
		}
	}

	if (net_client_wait_data.num_drones > 0)
	{
		M_snprintf(buf, sizeof(buf), " (+%i observer clients)", net_client_wait_data.num_drones);
		cudadoom::txt::SetLabel(drone_label, buf);
	}
	else
	{
		cudadoom::txt::SetLabel(drone_label, "");
	}

	if (net_client_wait_data.is_controller)
	{
		startgame = cudadoom::txt::NewWindowAction(' ', "Start game");
		cudadoom::txt::SignalConnect(startgame, "pressed", StartGame, NULL);
	}
	else
	{
		startgame = NULL;
	}

	cudadoom::txt::SetWindowAction(window, cudadoom::txt::HORIZ_RIGHT, startgame);
}

static void BuildMasterStatusWindow()
{
	cudadoom::txt::Window* master_window;

	master_window = cudadoom::txt::NewWindow(NULL);
	master_msg_label = cudadoom::txt::NewLabel("");
	cudadoom::txt::AddWidget(master_window, master_msg_label);

	// This window is here purely for information, so it should be
	// in the background.

	cudadoom::txt::LowerWindow(master_window);
	cudadoom::txt::SetWindowPosition(master_window, cudadoom::txt::HORIZ_CENTER, cudadoom::txt::VERT_CENTER,
		cudadoom::txt::SCREEN_W / 2, cudadoom::txt::SCREEN_H - 4);
	cudadoom::txt::SetWindowAction(master_window, cudadoom::txt::HORIZ_LEFT, NULL);
	cudadoom::txt::SetWindowAction(master_window, cudadoom::txt::HORIZ_CENTER, NULL);
	cudadoom::txt::SetWindowAction(master_window, cudadoom::txt::HORIZ_RIGHT, NULL);
}

static void CheckMasterStatus()
{
	bool added;

	if (!NET_Query_CheckAddedToMaster(&added))
	{
		return;
	}

	if (master_msg_label == NULL)
	{
		BuildMasterStatusWindow();
	}

	if (added)
	{
		cudadoom::txt::SetLabel(master_msg_label,
			"Your server is now registered with the global master server.\n"
			"Other players can find your server online.");
	}
	else
	{
		cudadoom::txt::SetLabel(master_msg_label,
			"Failed to register with the master server. Your server is not\n"
			"publicly accessible. You may need to reconfigure your Internet\n"
			"router to add a port forward for UDP port 2342. Look up\n"
			"information on port forwarding online.");
	}
}

static void PrintSHA1Digest(::std::string s, const byte* digest)
{
	unsigned i;

	printf("%s: ", s);

	for (i = 0; i < sizeof(sha1_digest_t); ++i)
	{
		printf("%02x", digest[i]);
	}

	printf("\n");
}

static void CloseWindow(cudadoom::txt::UNCAST_ARG(widget), cudadoom::txt::UNCAST_ARG(window))
{
	cudadoom::txt::CAST_ARG(cudadoom::txt::Window, window);

	cudadoom::txt::CloseWindow(window);
}

static void CheckSHA1Sums()
{
	bool correct_wad;
	bool correct_deh;
	bool same_freedoom;
	cudadoom::txt::Window* window;
	cudadoom::txt::WindowAction* cont_button;

	if (!net_client_received_wait_data || had_warning)
	{
		return;
	}

	correct_wad = memcmp(net_local_wad_sha1sum,
		net_client_wait_data.wad_sha1sum,
		sizeof(sha1_digest_t)) == 0;
	correct_deh = memcmp(net_local_deh_sha1sum,
		net_client_wait_data.deh_sha1sum,
		sizeof(sha1_digest_t)) == 0;
	same_freedoom = net_client_wait_data.is_freedoom == net_local_is_freedoom;

	if (correct_wad && correct_deh && same_freedoom)
	{
		return;
	}

	if (!correct_wad)
	{
		printf("Warning: WAD SHA1 does not match server:\n");
		PrintSHA1Digest("Local", net_local_wad_sha1sum);
		PrintSHA1Digest("Server", net_client_wait_data.wad_sha1sum);
	}

	if (!same_freedoom)
	{
		printf("Warning: Mixing Freedoom with non-Freedoom\n");
		printf("Local: %u Server: %i\n",
			net_local_is_freedoom,
			net_client_wait_data.is_freedoom);
	}

	if (!correct_deh)
	{
		printf("Warning: Dehacked SHA1 does not match server:\n");
		PrintSHA1Digest("Local", net_local_deh_sha1sum);
		PrintSHA1Digest("Server", net_client_wait_data.deh_sha1sum);
	}

	window = cudadoom::txt::NewWindow("WARNING!");

	cont_button = cudadoom::txt::NewWindowAction(KEY_ENTER, "Continue");
	cudadoom::txt::SignalConnect(cont_button, "pressed", CloseWindow, window);

	cudadoom::txt::SetWindowAction(window, cudadoom::txt::HORIZ_LEFT, NULL);
	cudadoom::txt::SetWindowAction(window, cudadoom::txt::HORIZ_CENTER, cont_button);
	cudadoom::txt::SetWindowAction(window, cudadoom::txt::HORIZ_RIGHT, NULL);

	if (!same_freedoom)
	{
		// If Freedoom and Doom IWADs are mixed, the WAD directory
		// will be wrong, but this is not neccessarily a problem.
		// Display a different message to the WAD directory message.

		if (net_local_is_freedoom)
		{
			cudadoom::txt::AddWidget(window, cudadoom::txt::NewLabel
			("You are using the Freedoom IWAD to play with players\n"
				"using an official Doom IWAD. Make sure that you are\n"
				"playing the same levels as other players.\n"));
		}
		else
		{
			cudadoom::txt::AddWidget(window, cudadoom::txt::NewLabel
			("You are using an official IWAD to play with players\n"
				"using the Freedoom IWAD. Make sure that you are\n"
				"playing the same levels as other players.\n"));
		}
	}
	else if (!correct_wad)
	{
		cudadoom::txt::AddWidget(window, cudadoom::txt::NewLabel
		("Your WAD directory does not match other players in the game.\n"
			"Check that you have loaded the exact same WAD files as other\n"
			"players.\n"));
	}

	if (!correct_deh)
	{
		cudadoom::txt::AddWidget(window, cudadoom::txt::NewLabel
		("Your dehacked signature does not match other players in the\n"
			"game. Check that you have loaded the same dehacked patches\n"
			"as other players.\n"));
	}

	cudadoom::txt::AddWidget(window, cudadoom::txt::NewLabel
	("If you continue, this may cause your game to desync."));

	had_warning = true;
}

static void ParseCommandLineArgs()
{
	int i;

	// Autostart the netgame when n nodes (clients) have joined the server.
	i = M_CheckParmWithArgs("-nodes", 1);
	if (i > 0)
	{
		expected_nodes = atoi(myargv[i + 1]);
	}
}

static void CheckAutoLaunch()
{
	int nodes;

	if (net_client_received_wait_data && net_client_wait_data.is_controller && expected_nodes > 0)
	{
		nodes = net_client_wait_data.num_players + net_client_wait_data.num_drones;

		if (nodes >= expected_nodes)
		{
			StartGame(NULL, NULL);
			expected_nodes = 0;
		}
	}
}

void NET_WaitForLaunch()
{
	if (!cudadoom::txt::Init())
	{
		fprintf(stderr, "Failed to initialize GUI\n");
		exit(-1);
	}

	cudadoom::txt::SetColor(cudadoom::txt::ColorType::blue, 0x04, 0x14, 0x40); // Romero's "funky blue" color

	// Crispy colors for Crispy Network GUI
	cudadoom::txt::SetColor(cudadoom::txt::ColorType::bright_green, 249, 227, 0); // 0xF9, 0xE3, 0x00
	cudadoom::txt::SetColor(cudadoom::txt::ColorType::cyan, 220, 153, 0);			// 0xDC, 0x99, 0x00
	cudadoom::txt::SetColor(cudadoom::txt::ColorType::bright_cyan, 76, 160, 223); // 0x4C, 0xA0, 0xDF

	I_InitWindowIcon();

	ParseCommandLineArgs();
	OpenWaitDialog();
	had_warning = false;

	while (net_waiting_for_launch)
	{
		UpdateGUI();
		CheckAutoLaunch();
		CheckSHA1Sums();
		CheckMasterStatus();

		cudadoom::txt::DispatchEvents();
		cudadoom::txt::DrawDesktop();

		NET_CL_Run();
		NET_SV_Run();

		if (!net_client_connected)
		{
			I_Error("Lost connection to server");
		}

		cudadoom::txt::Sleep(100);
	}

	cudadoom::txt::Shutdown();
}
