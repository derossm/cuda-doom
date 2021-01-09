/**********************************************************************************************************************************************\
	Copyright(C) 2005-2014 Simon Howard

	This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License
	as published by the Free Software Foundation; either version 2 of the License, or (at your option) any later version.

	This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

	DESCRIPTION:
		Definitions for use in networking code.
\**********************************************************************************************************************************************/
#pragma once

#include <string>

#include "../derma/common.h"

#include "doom/doomdef.h"
#include "doomtype.h"
#include "d_ticcmd.h"
#include "sha1.h"

constexpr size_t MAX_MODULES{16};

// Absolute maximum number of "nodes" in the game. This is different to
// NET_MAXPLAYERS, as there may be observers that are not participating
// (eg. left/right monitors)
constexpr size_t MAXNETNODES{16};

// The maximum number of players, multiplayer/networking.
// This is the maximum supported by the networking code; individual games
// have their own values for MAX_PLAYERS that can be smaller.
constexpr size_t NET_MAXPLAYERS{8};

// Maximum length of a player's name.
constexpr size_t MAXPLAYERNAME{30};

// Networking and tick handling related.
constexpr size_t BACKUPTICS{128};

struct net_packet_t
{
	byte* data;
	size_t len;
	size_t alloced;
	unsigned pos;
};

struct net_module_t;

// net_addr_t
struct net_addr_t
{
	net_module_t* mod; // TODO: URGENT -- REFACTOR THIS NAME
	int refcount;
	void* handle;
};

struct net_module_t
{
	// Initialize this module for use as a client
	bool (*InitClient)();

	// Initialize this module for use as a server
	bool (*InitServer)();

	// Send a packet
	void (*SendPacket)(net_addr_t* addr, net_packet_t* packet);

	// Check for new packets to receive
	//
	// Returns true if packet received
	bool (*RecvPacket)(net_addr_t** addr, net_packet_t** packet);

	// Converts an address to a string
	void (*AddrToString)(net_addr_t* addr, std::string buffer, int buffer_len);

	// Free back an address when no longer in use
	void (*FreeAddress)(net_addr_t* addr);

	// Try to resolve a name to an address
	net_addr_t* (*ResolveAddress)(std::string addr);
};

// Magic number sent when connecting to check this is a valid client
constexpr size_t NET_MAGIC_NUMBER{1454104972U};

// Old magic number used by Chocolate Doom versions before v3.0:
constexpr size_t NET_OLD_MAGIC_NUMBER{3436803284U};

// header field value indicating that the packet is a reliable packet
constexpr size_t NET_RELIABLE_PACKET{(1 << 15)};

// Supported protocols. If you're developing a fork of Chocolate
// Doom, you can add your own entry to this list while maintaining
// compatibility with Chocolate Doom servers. Higher-numbered enum values
// will be preferred when negotiating a protocol for the client and server
// to use, so the order matters.
// NOTE: The values in this enum do not have any special value outside of
// the program they're compiled in. What matters is the string representation.
enum class net_protocol_t
{
	// Protocol introduced with Chocolate Doom v3.0. Each compatibility-
	// breaking change to the network protocol will produce a new protocol
	// number in this enum.
	CHOCOLATE_DOOM_0,

	// Add your own protocol here; be sure to add a name for it to the list
	// in net_common.c too.
	NET_NUM_PROTOCOLS,
	UNKNOWN
};

// packet types
enum class net_packet_type
{
	SYN,
	ACK, // deprecated
	REJECTED,
	KEEPALIVE,
	WAITING_DATA,
	GAMESTART,
	GAMEDATA,
	GAMEDATA_ACK,
	DISCONNECT,
	DISCONNECT_ACK,
	RELIABLE_ACK,
	GAMEDATA_RESEND,
	CONSOLE_MESSAGE,
	QUERY,
	QUERY_RESPONSE,
	LAUNCH,
	NAT_HOLE_PUNCH
};

enum class net_master_packet_type
{
	ADD,
	ADD_RESPONSE,
	QUERY,
	QUERY_RESPONSE,
	GET_METADATA,
	GET_METADATA_RESPONSE,
	SIGN_START,
	SIGN_START_RESPONSE,
	SIGN_END,
	SIGN_END_RESPONSE,
	NAT_HOLE_PUNCH,
	NAT_HOLE_PUNCH_ALL
};

// Settings specified when the client connects to the server.
struct net_connect_data
{
	GameMode gamemode;
	GameMission gamemission;
	int lowres_turn;
	int drone;
	int max_players;
	int is_freedoom;
	sha1_digest_t wad_sha1sum;
	sha1_digest_t deh_sha1sum;
	int player_class;
};

// Game settings sent by client to server when initiating game start,
// and received from the server by clients when the game starts.
struct net_gamesettings
{
	TimeType ticdup;
	TimeType extratics;
	int deathmatch;
	int episode;
	int nomonsters;
	int fast_monsters;
	int respawn_monsters;
	int map;
	int skill;
	GameVersion gameversion;
	int lowres_turn;
	int new_sync;
	TimeType timelimit;
	int loadgame;
	int random; // [Strife only]

	// These fields are only used by the server when sending a game
	// start message:
	int num_players;
	int consoleplayer;

	// Hexen player classes:
	int player_classes[NET_MAXPLAYERS];
};

struct net_context
{
	net_module_t* mods[MAX_MODULES];
	int num_modules;
};

constexpr size_t NET_TICDIFF_FORWARD{(1 << 0)};
constexpr size_t NET_TICDIFF_SIDE{(1 << 1)};
constexpr size_t NET_TICDIFF_TURN{(1 << 2)};
constexpr size_t NET_TICDIFF_BUTTONS{(1 << 3)};
constexpr size_t NET_TICDIFF_CONSISTANCY{(1 << 4)};
constexpr size_t NET_TICDIFF_CHATCHAR{(1 << 5)};
constexpr size_t NET_TICDIFF_RAVEN{(1 << 6)};
constexpr size_t NET_TICDIFF_STRIFE{(1 << 7)};

struct net_ticdiff_t
{
	unsigned diff;
	ticcmd_t cmd;
};

// Complete set of ticcmds from all players
struct net_full_ticcmd_t
{
	int latency;
	unsigned seq;
	bool playeringame[NET_MAXPLAYERS];
	net_ticdiff_t cmds[NET_MAXPLAYERS];
};

// Data sent in response to server queries
struct net_querydata_t
{
	std::string version;
	int server_state;
	int num_players;
	int max_players;
	GameMode gamemode;
	GameMission gamemission;
	std::string description;
	net_protocol_t protocol;
};

// Data sent by the server while waiting for the game to start.
struct net_waitdata_t
{
	int num_players;
	int num_drones;
	int ready_players;
	int max_players;
	int is_controller;
	int consoleplayer;
	char player_names[NET_MAXPLAYERS][MAXPLAYERNAME];
	char player_addrs[NET_MAXPLAYERS][MAXPLAYERNAME];
	sha1_digest_t wad_sha1sum;
	sha1_digest_t deh_sha1sum;
	int is_freedoom;
};
