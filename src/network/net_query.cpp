/**********************************************************************************************************************************************\
	Copyright(C) 2005-2014 Simon Howard

	This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License
	as published by the Free Software Foundation; either version 2 of the License, or (at your option) any later version.

	This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

	DESCRIPTION:
		Querying servers to find their current status.
\**********************************************************************************************************************************************/
#include "../../derma/stdafx.h"

#include "../i_system.h"
#include "../i_timer.h"
#include "../m_misc.h"

#include "net_common.h"
#include "net_defs.h"
#include "net_io.h"
#include "net_packet.h"
#include "net_query.h"
#include "net_structrw.h"
#include "net_sdl.h"

// DNS address of the Internet master server.

#define MASTER_SERVER_ADDRESS "master.chocolate-doom.org:2342"

// Time to wait for a response before declaring a timeout.

constexpr size_t QUERY_TIMEOUT_SECS{2};

// Time to wait for secure demo signatures before declaring a timeout.

constexpr size_t SIGNATURE_TIMEOUT_SECS{5};

// Number of query attempts to make before giving up on a server.

constexpr size_t QUERY_MAX_ATTEMPTS{3};

enum class query_target_type_t
{
	SERVER,		// Normal server target.
	MASTER,		// The master server.
	BROADCAST		// Send a broadcast query
};

enum class query_target_state_t
{
	QUEUED,		// Query not yet sent
	QUERIED,		// Query sent, waiting response
	RESPONDED,	// Response received
	NO_RESPONSE
};

struct query_target_t
{
	query_target_type_t type;
	query_target_state_t state;
	net_addr_t* addr;
	net_querydata_t data;
	TimeType ping_time;
	TimeType query_time;
	unsigned query_attempts;
	bool printed;
};

static bool registered_with_master = false;
static bool got_master_response = false;

static net_context* query_context;
static query_target_t* targets;
static int num_targets;

static bool query_loop_running = false;
static bool printed_header = false;
static TimeType last_query_time = 0;

::std::string securedemo_start_message = NULL;

// Resolve the master server address.

net_addr_t* NET_Query_ResolveMaster(net_context* context)
{
	net_addr_t* addr;

	addr = NET_ResolveAddress(context, MASTER_SERVER_ADDRESS);

	if (addr == NULL)
	{
		fprintf(stderr, "Warning: Failed to resolve address for master server: %s\n", MASTER_SERVER_ADDRESS);
	}

	return addr;
}

// Send a registration packet to the master server to register
// ourselves with the global list.

void NET_Query_AddToMaster(net_addr_t* master_addr)
{
	net_packet_t* packet;

	packet = NET_NewPacket(10);
	NET_WriteInt16(packet, (unsigned)net_master_packet_type::ADD);
	NET_SendPacket(master_addr, packet);
	NET_FreePacket(packet);
}

// Process a packet received from the master server.

void NET_Query_AddResponse(net_packet_t* packet)
{
	unsigned result;

	if (!NET_ReadInt16(packet, &result))
	{
		return;
	}

	if (result != 0)
	{
		// Only show the message once.

		if (!registered_with_master)
		{
			printf("Registered with master server at %s\n", MASTER_SERVER_ADDRESS);
			registered_with_master = true;
		}
	}
	else
	{
		// Always show rejections.

		printf("Failed to register with master server at %s\n",
			MASTER_SERVER_ADDRESS);
	}

	got_master_response = true;
}

bool NET_Query_CheckAddedToMaster(bool* result)
{
	// Got response from master yet?

	if (!got_master_response)
	{
		return false;
	}

	*result = registered_with_master;
	return true;
}

// Send a query to the master server.

static void NET_Query_SendMasterQuery(net_addr_t* addr)
{
	net_packet_t* packet;

	packet = NET_NewPacket(4);
	NET_WriteInt16(packet, (unsigned)net_master_packet_type::QUERY);
	NET_SendPacket(addr, packet);
	NET_FreePacket(packet);

	// We also send a NAT_HOLE_PUNCH_ALL packet so that servers behind
	// NAT gateways will open themselves up to us.
	packet = NET_NewPacket(4);
	NET_WriteInt16(packet, (unsigned)net_master_packet_type::NAT_HOLE_PUNCH_ALL);
	NET_SendPacket(addr, packet);
	NET_FreePacket(packet);
}

// Send a hole punch request to the master server for the server at the
// given address.
void NET_RequestHolePunch(net_context* context, net_addr_t* addr)
{
	net_addr_t* master_addr;
	net_packet_t* packet;

	master_addr = NET_Query_ResolveMaster(context);
	if (master_addr == NULL)
	{
		return;
	}

	packet = NET_NewPacket(32);
	NET_WriteInt16(packet, (unsigned)net_master_packet_type::NAT_HOLE_PUNCH);
	NET_WriteString(packet, ::std::forward(NET_AddrToString(addr)));
	NET_SendPacket(master_addr, packet);

	NET_FreePacket(packet);
	NET_ReleaseAddress(master_addr);
}

// Given the specified address, find the target associated. If no
// target is found, and 'create' is true, a new target is created.

static query_target_t* GetTargetForAddr(net_addr_t* addr, bool create)
{
	query_target_t* target;
	int i;

	for (i = 0; i < num_targets; ++i)
	{
		if (targets[i].addr == addr)
		{
			return &targets[i];
		}
	}

	if (!create)
	{
		return nullptr;
	}

	targets = I_Realloc(targets, sizeof(query_target_t) * (num_targets + 1));

	target = &targets[num_targets];
	target->type = query_target_type_t::SERVER;
	target->state = query_target_state_t::QUEUED;
	target->printed = false;
	target->query_attempts = 0;
	target->addr = addr;
	NET_ReferenceAddress(addr);
	++num_targets;

	return target;
}

static void FreeTargets()
{
	int i;

	for (i = 0; i < num_targets; ++i)
	{
		NET_ReleaseAddress(targets[i].addr);
	}
	free(targets);
	targets = NULL;
	num_targets = 0;
}

// Transmit a query packet
static void NET_Query_SendQuery(net_addr_t* addr)
{
	net_packet_t* request;

	request = NET_NewPacket(10);
	NET_WriteInt16(request, (unsigned)net_packet_type::QUERY);

	if (addr == NULL)
	{
		NET_SendBroadcast(query_context, request);
	}
	else
	{
		NET_SendPacket(addr, request);
	}

	NET_FreePacket(request);
}

static void NET_Query_ParseResponse(net_addr_t* addr, net_packet_t* packet, net_query_callback_t callback, void* user_data)
{
	net_packet_type packet_type;
	net_querydata_t querydata;
	query_target_t* target;

	// Read the header
	if (!NET_ReadInt16(packet, (unsigned*)&packet_type) || packet_type != net_packet_type::QUERY_RESPONSE)
	{
		return;
	}

	// Read query data
	if (!NET_ReadQueryData(packet, &querydata))
	{
		return;
	}

	// Find the target that responded.
	target = GetTargetForAddr(addr, false);

	// If the target is not found, it may be because we are doing
	// a LAN broadcast search, in which case we need to create a
	// target for the new responder.
	if (target == NULL)
	{
		query_target_t* broadcast_target;

		broadcast_target = GetTargetForAddr(NULL, false);

		// Not in broadcast mode, unexpected response that came out
		// of nowhere. Ignore.
		if (broadcast_target == NULL || broadcast_target->state != query_target_state_t::QUERIED)
		{
			return;
		}

		// Create new target.
		target = GetTargetForAddr(addr, true);
		target->state = query_target_state_t::QUERIED;
		target->query_time = broadcast_target->query_time;
	}

	if (target->state != query_target_state_t::RESPONDED)
	{
		target->state = query_target_state_t::RESPONDED;
		memcpy(&target->data, &querydata, sizeof(net_querydata_t));

		// Calculate RTT.
		target->ping_time = I_GetTimeMS() - target->query_time;

		// Invoke callback to signal that we have a new address.
		callback(addr, &target->data, target->ping_time, user_data);
	}
}

// Parse a response packet from the master server.
static void NET_Query_ParseMasterResponse(net_addr_t* master_addr, net_packet_t* packet)
{
	unsigned packet_type;
	query_target_t* target;
	::std::string addr_str;
	net_addr_t* addr;

	// Read the header. We are only interested in query responses.
	if (!NET_ReadInt16(packet, &packet_type) || packet_type != (unsigned)net_master_packet_type::QUERY_RESPONSE)
	{
		return;
	}

	// Read a list of strings containing the addresses of servers that the master knows about.
	for (;;)
	{
		addr_str = NET_ReadString(packet);

		if (addr_str.empty())
		{
			break;
		}

		// Resolve address and add to targets list if it is not already there.
		addr = NET_ResolveAddress(query_context, addr_str);
		if (addr != NULL)
		{
			GetTargetForAddr(addr, true);
			NET_ReleaseAddress(addr);
		}
	}

	// Mark the master as having responded.
	target = GetTargetForAddr(master_addr, true);
	target->state = query_target_state_t::RESPONDED;
}

static void NET_Query_ParsePacket(net_addr_t* addr, net_packet_t* packet, net_query_callback_t callback, void* user_data)
{
	query_target_t* target;

	// This might be the master server responding.
	target = GetTargetForAddr(addr, false);

	if (target != NULL && target->type == query_target_type_t::MASTER)
	{
		NET_Query_ParseMasterResponse(addr, packet);
	}
	else
	{
		NET_Query_ParseResponse(addr, packet, callback, user_data);
	}
}

static void NET_Query_GetResponse(net_query_callback_t callback, void* user_data)
{
	net_addr_t* addr;
	net_packet_t* packet;

	if (NET_RecvPacket(query_context, &addr, &packet))
	{
		NET_Query_ParsePacket(addr, packet, callback, user_data);
		NET_ReleaseAddress(addr);
		NET_FreePacket(packet);
	}
}

// Find a target we have not yet queried and send a query.
static void SendOneQuery()
{
	unsigned now;
	unsigned i;

	now = I_GetTimeMS();

	// Rate limit - only send one query every 50ms.
	if (now - last_query_time < 50)
	{
		return;
	}

	for (i = 0; i < num_targets; ++i)
	{
		// Not queried yet? Or last query timed out without a response?
		if (targets[i].state == query_target_state_t::QUEUED
			|| (targets[i].state == query_target_state_t::QUERIED
				&& now - targets[i].query_time > QUERY_TIMEOUT_SECS * 1000))
		{
			break;
		}
	}

	if (i >= num_targets)
	{
		return;
	}

	// Found a target to query. Send a query; how to do this depends on the target type.
	switch (targets[i].type)
	{
	case query_target_type_t::SERVER:
		NET_Query_SendQuery(targets[i].addr);
		break;

	case query_target_type_t::BROADCAST:
		NET_Query_SendQuery(NULL);
		break;

	case query_target_type_t::MASTER:
		NET_Query_SendMasterQuery(targets[i].addr);
		break;
	}

	//printf("Queried %s\n", NET_AddrToString(targets[i].addr));
	targets[i].state = query_target_state_t::QUERIED;
	targets[i].query_time = now;
	++targets[i].query_attempts;

	last_query_time = now;
}

// Time out servers that have been queried and not responded.

static void CheckTargetTimeouts()
{
	unsigned i;
	unsigned now;

	now = I_GetTimeMS();

	for (i = 0; i < num_targets; ++i)
	{
		/*
		printf("target %i: state %i, queries %i, query time %i\n",
				i, targets[i].state, targets[i].query_attempts,
				now - targets[i].query_time);
		*/

		// We declare a target to be "no response" when we've sent
		// multiple query packets to it (QUERY_MAX_ATTEMPTS) and
		// received no response to any of them.
		if (targets[i].state == query_target_state_t::QUERIED
			&& targets[i].query_attempts >= QUERY_MAX_ATTEMPTS
			&& now - targets[i].query_time > QUERY_TIMEOUT_SECS * 1000)
		{
			targets[i].state = query_target_state_t::NO_RESPONSE;

			if (targets[i].type == query_target_type_t::MASTER)
			{
				fprintf(stderr, "NET_MasterQuery: no response from master server.\n");
			}
		}
	}
}

// If all targets have responded or timed out, returns true.
static bool AllTargetsDone()
{
	unsigned i;

	for (i = 0; i < num_targets; ++i)
	{
		if (targets[i].state != query_target_state_t::RESPONDED
			&& targets[i].state != query_target_state_t::NO_RESPONSE)
		{
			return false;
		}
	}

	return true;
}

// Polling function, invoked periodically to send queries and
// interpret new responses received from remote servers.
// Returns zero when the query sequence has completed and all targets
// have returned responses or timed out.
int NET_Query_Poll(net_query_callback_t callback, void* user_data)
{
	CheckTargetTimeouts();

	// Send a query. This will only send a single query at once.
	SendOneQuery();

	// Check for a response
	NET_Query_GetResponse(callback, user_data);

	return !AllTargetsDone();
}

// Stop the query loop
static void NET_Query_ExitLoop()
{
	query_loop_running = false;
}

// Loop waiting for responses.
// The specified callback is invoked when a new server responds.
static void NET_Query_QueryLoop(net_query_callback_t callback, void* user_data)
{
	query_loop_running = true;

	while (query_loop_running && NET_Query_Poll(callback, user_data))
	{
		// Don't thrash the CPU

		I_Sleep(1);
	}
}

void NET_Query_Init()
{
	if (query_context == NULL)
	{
		query_context = NET_NewContext();
		NET_AddModule(query_context, &net_sdl_module);
		net_sdl_module.InitClient();
	}

	free(targets);
	targets = NULL;
	num_targets = 0;

	printed_header = false;
}

// Callback that exits the query loop when the first server is found.
static void NET_Query_ExitCallback(net_addr_t* addr, net_querydata_t* data, TimeType ping_time, void* user_data)
{
	NET_Query_ExitLoop();
}

// Search the targets list and find a target that has responded.
// If none have responded, returns NULL.
static query_target_t* FindFirstResponder()
{
	unsigned i;

	for (i = 0; i < num_targets; ++i)
	{
		if (targets[i].type == query_target_type_t::SERVER
			&& targets[i].state == query_target_state_t::RESPONDED)
		{
			return &targets[i];
		}
	}

	return nullptr;
}

// Return a count of the number of responses.
static int GetNumResponses()
{
	unsigned i;
	int result;

	result = 0;

	for (i = 0; i < num_targets; ++i)
	{
		if (targets[i].type == query_target_type_t::SERVER
			&& targets[i].state == query_target_state_t::RESPONDED)
		{
			++result;
		}
	}

	return result;
}

int NET_StartLANQuery()
{
	query_target_t* target;

	NET_Query_Init();

	// Add a broadcast target to the list.

	target = GetTargetForAddr(NULL, true);
	target->type = query_target_type_t::BROADCAST;

	return 1;
}

int NET_StartMasterQuery()
{
	net_addr_t* master;
	query_target_t* target;

	NET_Query_Init();

	// Resolve master address and add to targets list.

	master = NET_Query_ResolveMaster(query_context);

	if (master == NULL)
	{
		return 0;
	}

	target = GetTargetForAddr(master, true);
	target->type = query_target_type_t::MASTER;
	NET_ReleaseAddress(master);

	return 1;
}

static void formatted_printf(int wide, ::std::string s, ...) PRINTF_ATTR(2, 3);
static void formatted_printf(int wide, ::std::string s, ...)
{
	va_list args;
	int i;

	va_start(args, s);
	i = vprintf(s, args);
	va_end(args);

	while (i < wide)
	{
		putchar(' ');
		++i;
	}
}

static ::std::string GameDescription(GameMode mode, GameMission mission)
{
	switch (mission)
	{
	case GameMission::doom:
		if (mode == GameMode::shareware)
		{
			return "swdoom";
		}
		else if (mode == GameMode::registered)
		{
			return "regdoom";
		}
		else if (mode == GameMode::retail)
		{
			return "ultdoom";
		}
		else
		{
			return "doom";
		}

	case GameMission::doom2:
		return "doom2";

	case GameMission::pack_tnt:
		return "tnt";

	case GameMission::pack_plut:
		return "plutonia";

	case GameMission::pack_chex:
		return "chex";

	case GameMission::pack_hacx:
		return "hacx";

	case GameMission::heretic:
		return "heretic";

	case GameMission::hexen:
		return "hexen";

	case GameMission::strife:
		return "strife";

	default:
		return "?";
	}
}

static void PrintHeader()
{
	int i;

	putchar('\n');
	formatted_printf(5, "Ping");
	formatted_printf(18, "Address");
	formatted_printf(8, "Players");
	puts("Description");

	for (i = 0; i < 70; ++i)
		putchar('=');
	putchar('\n');
}

// Callback function that just prints information in a table.

static void NET_QueryPrintCallback(net_addr_t* addr, net_querydata_t* data, unsigned TimeType ping_time, void* user_data)
{
	// If this is the first server, print the header.

	if (!printed_header)
	{
		PrintHeader();
		printed_header = true;
	}

	formatted_printf(5, "%4i", ping_time);
	formatted_printf(22, "%s", NET_AddrToString(addr));
	formatted_printf(4, "%i/%i ", data->num_players,
		data->max_players);

	if (data->gamemode != GameMode::indetermined)
	{
		printf("(%s) ", GameDescription(data->gamemode,
			data->gamemission));
	}

	if (data->server_state)
	{
		printf("(game running) ");
	}

	printf("%s\n", data->description);
}

void NET_LANQuery()
{
	if (NET_StartLANQuery())
	{
		printf("\nSearching for servers on local LAN ...\n");

		NET_Query_QueryLoop(NET_QueryPrintCallback, NULL);

		printf("\n%i server(s) found.\n", GetNumResponses());
		FreeTargets();
	}
}

void NET_MasterQuery()
{
	if (NET_StartMasterQuery())
	{
		printf("\nSearching for servers on Internet ...\n");

		NET_Query_QueryLoop(NET_QueryPrintCallback, NULL);

		printf("\n%i server(s) found.\n", GetNumResponses());
		FreeTargets();
	}
}

void NET_QueryAddress(::std::string addr_str)
{
	net_addr_t* addr;
	query_target_t* target;

	NET_Query_Init();

	addr = NET_ResolveAddress(query_context, addr_str);

	if (addr == NULL)
	{
		I_Error("NET_QueryAddress: Host '%s' not found!", addr_str);
	}

	// Add the address to the list of targets.

	target = GetTargetForAddr(addr, true);

	printf("\nQuerying '%s'...\n", addr_str);

	// Run query loop.

	NET_Query_QueryLoop(NET_Query_ExitCallback, NULL);

	// Check if the target responded.

	if (target->state == query_target_state_t::RESPONDED)
	{
		NET_QueryPrintCallback(addr, &target->data, target->ping_time, NULL);
		NET_ReleaseAddress(addr);
		FreeTargets();
	}
	else
	{
		I_Error("No response from '%s'", addr_str);
	}
}

net_addr_t* NET_FindLANServer()
{
	query_target_t* target;
	query_target_t* responder;
	net_addr_t* result;

	NET_Query_Init();

	// Add a broadcast target to the list.

	target = GetTargetForAddr(NULL, true);
	target->type = query_target_type_t::BROADCAST;

	// Run the query loop, and stop at the first target found.

	NET_Query_QueryLoop(NET_Query_ExitCallback, NULL);

	responder = FindFirstResponder();

	if (responder != NULL)
	{
		result = responder->addr;
		NET_ReferenceAddress(result);
	}
	else
	{
		result = NULL;
	}

	FreeTargets();
	return result;
}

// Block until a packet of the given type is received from the given
// address.

static net_packet_t* BlockForPacket(net_addr_t* addr, unsigned packet_type, TimeType timeout_ms)
{
	net_packet_t* packet;
	net_addr_t* packet_src;
	unsigned read_packet_type;
	TimeType start_time;

	start_time = I_GetTimeMS();

	while (I_GetTimeMS() < start_time + timeout_ms)
	{
		if (!NET_RecvPacket(query_context, &packet_src, &packet))
		{
			I_Sleep(20);
			continue;
		}

		// Caller doesn't need additional reference.
		NET_ReleaseAddress(packet_src);

		if (packet_src == addr
			&& NET_ReadInt16(packet, &read_packet_type)
			&& packet_type == read_packet_type)
		{
			return packet;
		}

		NET_FreePacket(packet);
	}

	// Timeout - no response.

	return nullptr;
}

// Query master server for secure demo start seed value.

bool NET_StartSecureDemo(prng_seed_t seed)
{
	net_packet_t* request, * response;
	net_addr_t* master_addr;
	::std::string signature;
	bool result;

	NET_Query_Init();
	master_addr = NET_Query_ResolveMaster(query_context);

	// Send request packet to master server.

	request = NET_NewPacket(10);
	NET_WriteInt16(request, net_master_packet_type::SIGN_START);
	NET_SendPacket(master_addr, request);
	NET_FreePacket(request);

	// Block for response and read contents.
	// The signed start message will be saved for later.

	response = BlockForPacket(master_addr,
		net_master_packet_type::SIGN_START_RESPONSE,
		SIGNATURE_TIMEOUT_SECS * 1000);

	result = false;

	if (response)
	{
		if (NET_ReadPRNGSeed(response, seed))
		{
			signature = NET_ReadString(response);

			if (!signature.empty())
			{
				securedemo_start_message = ::std::string(signature);
				result = true;
			}
		}

		NET_FreePacket(response);
	}

	return result;
}

// Query master server for secure demo end signature.

::std::string NET_EndSecureDemo(sha1_digest_t demo_hash)
{
	net_packet_t* request, * response;
	net_addr_t* master_addr;
	::std::string signature;

	master_addr = NET_Query_ResolveMaster(query_context);

	// Construct end request and send to master server.

	request = NET_NewPacket(10);
	NET_WriteInt16(request, net_master_packet_type::SIGN_END);
	NET_WriteSHA1Sum(request, demo_hash);
	NET_WriteString(request, securedemo_start_message);
	NET_SendPacket(master_addr, request);
	NET_FreePacket(request);

	// Block for response. The response packet simply contains a string
	// with the ASCII signature.

	response = BlockForPacket(master_addr,
		net_master_packet_type::SIGN_END_RESPONSE,
		SIGNATURE_TIMEOUT_SECS * 1000);

	if (response == NULL)
	{
		return nullptr;
	}

	signature = NET_ReadString(response);

	NET_FreePacket(response);

	return signature;
}

