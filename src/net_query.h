/**********************************************************************************************************************************************\
	Copyright(C) 2005-2014 Simon Howard

	This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License
	as published by the Free Software Foundation; either version 2 of the License, or (at your option) any later version.

	This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

	DESCRIPTION:
		Querying servers to find their current status.
\**********************************************************************************************************************************************/
#pragma once

#include <string>

#include "../derma/common.h"

#include "net_defs.h"

typedef void (*net_query_callback_t)(net_addr_t* addr, net_querydata_t* querydata, TimeType ping_time, void* user_data);

extern int NET_StartLANQuery();
extern int NET_StartMasterQuery();

extern void NET_LANQuery();
extern void NET_MasterQuery();
extern void NET_QueryAddress(::std::string addr);
extern net_addr_t* NET_FindLANServer();

extern int NET_Query_Poll(net_query_callback_t callback, void* user_data);

extern net_addr_t* NET_Query_ResolveMaster(net_context* context);
extern void NET_Query_AddToMaster(net_addr_t* master_addr);
extern bool NET_Query_CheckAddedToMaster(bool* result);
extern void NET_Query_AddResponse(net_packet_t* packet);
extern void NET_RequestHolePunch(net_context* context, net_addr_t* addr);
