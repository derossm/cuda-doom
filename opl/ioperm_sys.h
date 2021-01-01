/**********************************************************************************************************************************************\
	Copyright(C) 2002, 2003 Marcel Telka
	Copyright(C) 2005-2014 Simon Howard

	This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License
	as published by the Free Software Foundation; either version 2 of the License, or (at your option) any later version.

	This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

	DESCRIPTION:
		Interface to the ioperm.sys driver, based on code from the Cygwin ioperm library.
\**********************************************************************************************************************************************/
#pragma once

#include "../derma/common.h"

#define IOPERM_FILE L"\\\\.\\ioperm"

#define IOCTL_IOPERM CTL_CODE(FILE_DEVICE_UNKNOWN, 0xA00, METHOD_BUFFERED, FILE_ANY_ACCESS)

struct ioperm_data
{
	unsigned long from;
	unsigned long num;
	int turn_on;
};

// Function pointers for advapi32.dll. This DLL does not exist on Windows 9x, so they are dynamically loaded from the DLL at runtime.

// haleyjd 09/09/10: Moved calling conventions into ()'s
static SC_HANDLE (WINAPI* MyOpenSCManagerW)(const wchar_t* lpMachineName, const wchar_t* lpDatabaseName, DWORD dwDesiredAccess){nullptr};

static SC_HANDLE (WINAPI* MyCreateServiceW)(SC_HANDLE hSCManager,
											const wchar_t* lpServiceName,
											const wchar_t* lpDisplayName,
											DWORD dwDesiredAccess,
											DWORD dwServiceType,
											DWORD dwStartType,
											DWORD dwErrorControl,
											const wchar_t* lpBinaryPathName,
											const wchar_t* lpLoadOrderGroup,
											LPDWORD lpdwTagId,
											const wchar_t* lpDependencies,
											const wchar_t* lpServiceStartName,
											const wchar_t* lpPassword);

static SC_HANDLE (WINAPI* MyOpenServiceW)(SC_HANDLE hSCManager, const wchar_t* lpServiceName, DWORD dwDesiredAccess);
static bool (WINAPI* MyStartServiceW)(SC_HANDLE hService, DWORD dwNumServiceArgs, const wchar_t** lpServiceArgVectors);
static bool (WINAPI* MyControlService)(SC_HANDLE hService, DWORD dwControl, LPSERVICE_STATUS lpServiceStatus);
static bool (WINAPI* MyCloseServiceHandle)(SC_HANDLE hSCObject);
static bool (WINAPI* MyDeleteService)(SC_HANDLE hService);

using SC_CallbackPtr = SC_HANDLE*;

struct DLLFunctions
{
	const char* name;
	SC_CallbackPtr fn;
};

bool IOperm_EnablePortRange(unsigned from, unsigned num, int turn_on);
bool IOperm_InstallDriver();
bool IOperm_UninstallDriver();
