/**********************************************************************************************************************************************\
	Copyright(C) 2005-2014 Simon Howard

	This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License
	as published by the Free Software Foundation; either version 2 of the License, or (at your option) any later version.

	This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

	DESCRIPTION:
		Common code shared between the client and server
\**********************************************************************************************************************************************/
#pragma once

#include "../derma/common.h"

#include "doomtype.h"
#include "d_mode.h"
#include "i_system.h"
#include "i_timer.h"
#include "m_argv.h"

#include "net_io.h"
#include "net_packet.h"
#include "net_structrw.h"
#include "net_defs.h"
#include "net_packet.h"

enum class net_connstate_t
{
	// Client has sent a SYN, is waiting for a SYN in response.
	CONNECTING,

	// Successfully connected.
	CONNECTED,

	// Sent a DISCONNECT packet, waiting for a DISCONNECT_ACK reply
	DISCONNECTING,

	// Client successfully disconnected
	DISCONNECTED,

	// We are disconnected, but in a sleep state, waiting for several
	// seconds. This is in case the DISCONNECT_ACK we sent failed
	// to arrive, and we need to send another one. We keep this as
	// a valid connection for a few seconds until we are sure that
	// the other end has successfully disconnected as well.
	DISCONNECTED_SLEEP
};

// Reason a connection was terminated

enum class net_disconnect_reason_t
{
	// As the result of a local disconnect request
	LOCAL,

	// As the result of a remote disconnect request
	REMOTE,

	// Timeout (no data received in a long time)
	TIMEOUT
};

constexpr size_t MAX_RETRIES{5};

// connections time out after 30 seconds
constexpr size_t CONNECTION_TIMEOUT_LEN{30};

// maximum time between sending packets
constexpr size_t KEEPALIVE_PERIOD{1};

// reliable packet that is guaranteed to reach its destination
struct net_reliable_packet_t
{
	net_packet_t* packet;
	TimeType last_send_time;
	int seq;
	net_reliable_packet_t* next;
};

struct net_connection_t
{
	net_connstate_t state;
	net_disconnect_reason_t disconnect_reason;
	net_addr_t* addr;
	net_protocol_t protocol;
	TimeType last_send_time;
	int num_retries;
	TimeType keepalive_send_time;
	TimeType keepalive_recv_time;
	net_reliable_packet_t* reliable_packets;
	int reliable_send_seq;
	int reliable_recv_seq;
};

void NET_Conn_SendPacket(net_connection_t* conn, net_packet_t* packet);
void NET_Conn_InitClient(net_connection_t* conn, net_addr_t* addr, net_protocol_t protocol);
void NET_Conn_InitServer(net_connection_t* conn, net_addr_t* addr, net_protocol_t protocol);
bool NET_Conn_Packet(net_connection_t* conn, net_packet_t* packet, unsigned* packet_type);
void NET_Conn_Disconnect(net_connection_t* conn);
void NET_Conn_Run(net_connection_t* conn);
net_packet_t* NET_Conn_NewReliable(net_connection_t* conn, int packet_type);

// Other miscellaneous common functions
unsigned NET_ExpandTicNum(unsigned relative, unsigned b);
bool NET_ValidGameSettings(GameMode mode, GameMission mission, net_gamesettings* settings);

void NET_OpenLog();
void NET_Log(::std::string fmt, ...);
void NET_LogPacket(net_packet_t* packet);
