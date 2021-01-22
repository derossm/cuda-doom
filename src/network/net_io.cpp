/**********************************************************************************************************************************************\
	Copyright(C) 2005-2014 Simon Howard

	This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License
	as published by the Free Software Foundation; either version 2 of the License, or (at your option) any later version.

	This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

	DESCRIPTION:
		Network packet I/O. Base layer for sending/receiving packets, through the network module system
\**********************************************************************************************************************************************/
#include "../../derma/stdafx.h"

#include "../i_system.h"
#include "net_defs.h"
#include "net_io.h"
#include "../z_zone.h"

net_addr_t net_broadcast_addr;

net_context* NET_NewContext()
{
	net_context* context = Z_Malloc<net_context*>(sizeof(net_context), pu_tags_t::PU_STATIC, 0);
	context->num_modules = 0;

	return context;
}

void NET_AddModule(net_context* context, net_module_t* mod)
{
	if (context->num_modules >= MAX_MODULES)
	{
		I_Error("NET_AddModule: No more modules for context");
	}

	context->mods[context->num_modules] = mod;
	++context->num_modules;
}

net_addr_t* NET_ResolveAddress(net_context* context, ::std::string addr)
{
	int i;
	net_addr_t* result;

	for (i = 0; i < context->num_modules; ++i)
	{
		result = context->mods[i]->ResolveAddress(addr);

		if (result != NULL)
		{
			NET_ReferenceAddress(result);
			return result;
		}
	}

	return nullptr;
}

void NET_SendPacket(net_addr_t* addr, net_packet_t* packet)
{
	addr->mod->SendPacket(addr, packet);
}

void NET_SendBroadcast(net_context* context, net_packet_t* packet)
{
	int i;

	for (i = 0; i < context->num_modules; ++i)
	{
		context->mods[i]->SendPacket(&net_broadcast_addr, packet);
	}
}

bool NET_RecvPacket(net_context* context,
	net_addr_t** addr,
	net_packet_t** packet)
{
	int i;

	// check all modules for new packets

	for (i = 0; i < context->num_modules; ++i)
	{
		if (context->mods[i]->RecvPacket(addr, packet))
		{
			NET_ReferenceAddress(*addr);
			return true;
		}
	}

	return false;
}

// Note: this prints into a static buffer, calling again overwrites
// the first result

::std::string NET_AddrToString(net_addr_t* addr)
{
	static char buf[128];

	addr->mod->AddrToString(addr, buf, sizeof(buf) - 1);

	return buf;
}

void NET_ReferenceAddress(net_addr_t* addr)
{
	if (addr == NULL)
	{
		return;
	}
	++addr->refcount;
	//printf("%s: +refcount=%d\n", NET_AddrToString(addr), addr->refcount);
}

void NET_ReleaseAddress(net_addr_t* addr)
{
	if (addr == NULL)
	{
		return;
	}

	--addr->refcount;
	//printf("%s: -refcount=%d\n", NET_AddrToString(addr), addr->refcount);
	if (addr->refcount <= 0)
	{
		addr->mod->FreeAddress(addr);
	}
}

