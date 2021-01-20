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
	#include <wrl.h>
	#include <shellapi.h>
	#include <aclapi.h>

	#include <d3d12.h>
	#include <dxgi1_4.h>
	#include <D3Dcompiler.h>
	#include <DirectXMath.h>

	#include "../directx/d3dx12.h"

	//#include <tchar.h>
	//#include <shlobj.h>
	//#include <process.h>
	//#include <io.h>
	//#include <winioctl.h>
	//#include <sys/stat.h>

	#ifndef NO_OBSD_DRIVER
		#define NO_OBSD_DRIVER
	#endif
#endif
