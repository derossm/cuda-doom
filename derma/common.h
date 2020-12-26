/**********************************************************************************************************************************************\
	Copyright(C) 2020 Mason DeRoss

	Released under the GNU All-permissive License

	Copying and distribution of this file, with or without modification, are permitted in any medium without royalty,
	provided the copyright notice and this notice are preserved. This file is offered as-is, without any warranty.

	DESCRIPTION:

\**********************************************************************************************************************************************/
#pragma once

#include <limits>

#include <memory>

#include <string>
#include <string_view>
#include <charconv>

#include <vector>
#include <array>

#include <variant>
#include <any>

#include <algorithm>
#include <numeric>

#include <utility>

#include <cassert>

class DepType;

namespace CUDADOOM
{
	namespace TXT
	{
		template<typename T>
		class InputBox;
	}
}

using txt_inputbox_t = CUDADOOM::TXT::InputBox<DepType>;

#include <cstdlib>
#include <cstdio>
#include <cctype>
#include <cstring>

#include <cinttypes>
#include <climits>
#include <cstdarg>

#include <cstddef>

#include <cerrno>

#include <cmath>

#include <ctime>
#include <cstdint>

using byte = uint8_t;

#include <sys/types.h>

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
#endif

#ifndef _WIN32
	#include <fcntl.h>
	#include <unistd.h>
	#include <sys/wait.h>
	#include <sys/stat.h>
	#include <sys/types.h>
	#include <sys/io.h>
	#include <sys/ioctl.h>
	#include <linux/kd.h>
#endif

#ifdef HAVE_DIRENT_H
	#include <dirent.h>
	#include <sys/stat.h>
#endif

#ifdef _MSC_VER
	#include <direct.h>
	//#include <win_opendir.h>
	#include <sys/stat.h>
#endif

#ifdef __WATCOMC__
	#include <direct.h>
#endif

#ifdef HAVE_MMAP
	#include <fcntl.h>
	#include <unistd.h>
	#include <sys/mman.h>
#endif

// OpenBSD/NetBSD:
#ifdef HAVE_DEV_ISA_SPKRIO_H
	#define HAVE_BSD_SPEAKER
	#include <dev/isa/spkrio.h>
#endif

// FreeBSD
#ifdef HAVE_DEV_SPEAKER_SPEAKER_H
	#define HAVE_BSD_SPEAKER
	#include <dev/speaker/speaker.h>
#endif

#ifdef HAVE_BSD_SPEAKER
	#include <fcntl.h>
	#include <unistd.h>
	#include <sys/wait.h>
	#include <sys/types.h>
	#include <sys/ioctl.h>
	#include <sys/signal.h>
	#include <sys/socket.h>
	#include <signal.h>
#endif

// OpenBSD has a i386_iopl on i386 and amd64_iopl on x86_64, even though they do the same thing.
// Take care of this here, and map set_iopl to point to the appropriate name.
#if defined(HAVE_LIBI386)
	#include <sys/types.h>
	#include <machine/sysarch.h>
	#include <i386/pio.h>
	#define set_iopl i386_iopl
#elif defined(HAVE_LIBAMD64)
	#include <sys/types.h>
	#include <machine/sysarch.h>
	#include <amd64/pio.h>
	#define set_iopl amd64_iopl
#else
	#define NO_OBSD_DRIVER
#endif

#ifndef NO_OBSD_DRIVER
	#include <unistd.h>
#endif
