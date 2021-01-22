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

namespace cudadoom::opl
{

#ifdef _WIN32
#include <winioctl.h>

#define IOPERM_FILE L"\\\\.\\ioperm"

#define IOCTL_IOPERM CTL_CODE(FILE_DEVICE_UNKNOWN, 0xA00, METHOD_BUFFERED, FILE_ANY_ACCESS)

bool IOperm_EnablePortRange(unsigned from, unsigned num, int turn_on);
bool IOperm_InstallDriver();
bool IOperm_UninstallDriver();

using SC_CallbackPtr = SC_HANDLE*;

struct DLLFunctions
{
	::std::string name;
	SC_CallbackPtr fn;
};

// Globals
inline static SC_HANDLE scm{nullptr};
inline static SC_HANDLE svc{nullptr};
inline static bool service_was_created{false};
inline static bool service_was_started{false};

struct ioperm_data
{
	unsigned long from;
	unsigned long num;
	int turn_on;
};

// Function pointers for advapi32.dll. This DLL does not exist on Windows 9x, so they are dynamically loaded from the DLL at runtime.

// haleyjd 09/09/10: Moved calling conventions into ()'s
static SC_HANDLE(WINAPI* MyOpenSCManagerW)(const wchar_t* lpMachineName, const wchar_t* lpDatabaseName, DWORD dwDesiredAccess) { nullptr };

static SC_HANDLE(WINAPI* MyCreateServiceW)(SC_HANDLE hSCManager,
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

static SC_HANDLE(WINAPI* MyOpenServiceW)(SC_HANDLE hSCManager, const wchar_t* lpServiceName, DWORD dwDesiredAccess);
static bool (WINAPI* MyStartServiceW)(SC_HANDLE hService, DWORD dwNumServiceArgs, const wchar_t** lpServiceArgVectors);
static bool (WINAPI* MyControlService)(SC_HANDLE hService, DWORD dwControl, LPSERVICE_STATUS lpServiceStatus);
static bool (WINAPI* MyCloseServiceHandle)(SC_HANDLE hSCObject);
static bool (WINAPI* MyDeleteService)(SC_HANDLE hService);

inline static DLLFunctions dll_functions[]{
	{ "OpenSCManagerW",		(SC_CallbackPtr)&MyOpenSCManagerW },
	{ "CreateServiceW",		(SC_CallbackPtr)&MyCreateServiceW },
	{ "OpenServiceW",		(SC_CallbackPtr)&MyOpenServiceW },
	{ "StartServiceW",		(SC_CallbackPtr)&MyStartServiceW },
	{ "ControlService",		(SC_CallbackPtr)&MyControlService },
	{ "CloseServiceHandle",	(SC_CallbackPtr)&MyCloseServiceHandle },
	{ "DeleteService",		(SC_CallbackPtr)&MyDeleteService },
};

static bool LoadLibraryPointers()
{
	// Already loaded?
	if (!MyOpenSCManagerW)
	{
		if (HMODULE dll{LoadLibraryW(L"advapi32.dll")}; dll)
		{
			for (size_t i{0}, size{sizeof(dll_functions) / sizeof(*dll_functions)}; i < size; ++i)
			{
				*(dll_functions[i].fn) = (SC_HANDLE)GetProcAddress(dll, dll_functions[i].name.c_str());

				if (*(dll_functions[i].fn) == nullptr)
				{
					fprintf(stderr, "LoadLibraryPointers: Failed to get address for '%s'\n", dll_functions[i].name.c_str());
					return false;
				}
			}
		}
		else
		{
			fprintf(stderr, "LoadLibraryPointers: Failed to open advapi32.dll\n");
			return false;
		}
	}

	return true;
}

bool IOperm_EnablePortRange(unsigned from, unsigned num, int turn_on)
{
	auto handle{CreateFileW(IOPERM_FILE, GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL)};

	bool result{false};

	if (handle == INVALID_HANDLE_VALUE)
	{
		errno = ENODEV;
	}
	else
	{
		ioperm_data ioperm_data{.from = from, .num = num, .turn_on = turn_on};

		// TODO investigate a better way to do this test without needing DWORD BytesReturned that we don't use
		DWORD BytesReturned;
		result = DeviceIoControl(handle, IOCTL_IOPERM, &ioperm_data, sizeof ioperm_data, NULL, 0, &BytesReturned, NULL);

		CloseHandle(handle);

		if (!result)
		{
			errno = EPERM;
		}
	}

	return result;
}

// Load ioperm.sys driver. Returns 1 for success, 0 for failure. Remember to call IOperm_UninstallDriver to uninstall the driver later.
bool IOperm_InstallDriver()
{
	if (!LoadLibraryPointers())
	{
		return false;
	}

	scm = MyOpenSCManagerW(nullptr, nullptr, SC_MANAGER_ALL_ACCESS);

	if (!scm)
	{
		auto error{GetLastError()};
		fprintf(stderr, "IOperm_InstallDriver: OpenSCManager failed (%i).\n", error);
		return false;
	}

	wchar_t driver_path[MAX_PATH];

	// Get the full path to the driver file.
	GetFullPathNameW(L"ioperm.sys", MAX_PATH, driver_path, nullptr);

	// Create the service.
	svc = MyCreateServiceW(scm, L"ioperm", L"ioperm support for Cygwin driver",
		SERVICE_ALL_ACCESS, SERVICE_KERNEL_DRIVER, SERVICE_AUTO_START, SERVICE_ERROR_NORMAL,
		driver_path, nullptr, nullptr, nullptr, nullptr, nullptr);

	if (!svc)
	{
		auto error{GetLastError()};

		if (error != ERROR_SERVICE_EXISTS)
		{
			fprintf(stderr, "IOperm_InstallDriver: Failed to create service (%i).\n", error);
		}
		else
		{
			svc = MyOpenServiceW(scm, L"ioperm", SERVICE_ALL_ACCESS);

			if (!svc)
			{
				error = GetLastError();

				fprintf(stderr, "IOperm_InstallDriver: Failed to open service (%i).\n", error);
			}
		}

		if (!svc)
		{
			MyCloseServiceHandle(scm);
			return false;
		}
	}
	else
	{
		service_was_created = true;
	}

	// Start the service. If the service already existed, it might have already been running as well.
	if (!MyStartServiceW(svc, 0, nullptr))
	{
		auto error{GetLastError()};

		if (error != ERROR_SERVICE_ALREADY_RUNNING)
		{
			fprintf(stderr, "IOperm_InstallDriver: Failed to start service (%i).\n", error);

			// this is the only path where result was set to false
			// failed to start the driver running, so clean up and return failure
			IOperm_UninstallDriver();
			return false;
		}
		else
		{
			printf("IOperm_InstallDriver: ioperm driver already running.\n");
		}
	}
	else
	{
		printf("IOperm_InstallDriver: ioperm driver installed.\n");
		service_was_started = true;
	}

	return true;
}

bool IOperm_UninstallDriver()
{
	bool result{true};

	// If we started the service, stop it.
	if (service_was_started)
	{
		if (SERVICE_STATUS stat; !MyControlService(svc, SERVICE_CONTROL_STOP, &stat))
		{
			auto error{GetLastError()};

			if (error == ERROR_SERVICE_NOT_ACTIVE)
			{
				fprintf(stderr, "IOperm_UninstallDriver: Service not active? (%i)\n", error);
			}
			else
			{
				fprintf(stderr, "IOperm_UninstallDriver: Failed to stop service (%i).\n", error);
				result = false;
			}
		}
	}

	// If we created the service, delete it.
	if (service_was_created)
	{
		if (!MyDeleteService(svc))
		{
			auto error{GetLastError()};

			fprintf(stderr, "IOperm_UninstallDriver: DeleteService failed (%i).\n", error);

			result = false;
		}
		else if (service_was_started)
		{
			printf("IOperm_UnInstallDriver: ioperm driver uninstalled.\n");
		}
	}

	// Close handles.
	if (svc)
	{
		MyCloseServiceHandle(svc);
		svc = nullptr;
	}

	if (scm)
	{
		MyCloseServiceHandle(scm);
		scm = nullptr;
	}

	service_was_created = false;
	service_was_started = false;

	return result;
}

#endif /* #ifndef _WIN32 */

} // end namespace cudadoom::opl
