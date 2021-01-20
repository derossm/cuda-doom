/**********************************************************************************************************************************************\
	Copyright© 2020-2021 Mason DeRoss

	Released under the GNU All-permissive License

	Copying and distribution of this file, with or without modification, are permitted in any medium without royalty,
	provided the copyright notice and this notice are preserved. This file is offered as-is, without any warranty.

	DESCRIPTION:

\**********************************************************************************************************************************************/
#pragma once

#ifdef _WIN32
	#ifndef WIN32_LEAN_AND_MEAN
		#define WIN32_LEAN_AND_MEAN
	#endif
	#ifndef NOMINMAX
		#define NOMINMAX
	#endif

	#include <windows.h>
	#include <tchar.h>
	#include <shellapi.h>
	#include <shlobj.h>
	#include <process.h>
	#include <io.h>
	#include <winioctl.h>
	#include <sys/stat.h>

	#ifndef NO_OBSD_DRIVER
		#define NO_OBSD_DRIVER
	#endif
#endif
