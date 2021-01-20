/**********************************************************************************************************************************************\
	Copyright© 2020-2021 Mason DeRoss

	Released under the GNU All-permissive License

	Copying and distribution of this file, with or without modification, are permitted in any medium without royalty,
	provided the copyright notice and this notice are preserved. This file is offered as-is, without any warranty.

	DESCRIPTION:

\**********************************************************************************************************************************************/
#pragma once

#ifndef _WIN32
	#include <fcntl.h>
	#include <unistd.h>
	#include <signal.h>

	#include <sys/io.h>
	#include <sys/ioctl.h>
	#include <sys/signal.h>
	#include <sys/socket.h>
	#include <sys/stat.h>
	#include <sys/types.h>
	#include <sys/wait.h>

	#include <linux/kd.h>
	#include <machine/sysarch.h>

	#ifdef HAVE_MMAP
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

	// OpenBSD has a i386_iopl on i386 and amd64_iopl on x86_64, even though they do the same thing.
	// Take care of this here, and map set_iopl to point to the appropriate name.
	#if defined(HAVE_LIBI386)
		#include <i386/pio.h>
		#ifndef set_iopl
			#define set_iopl i386_iopl
		#endif
	#elif defined(HAVE_LIBAMD64)
		#include <amd64/pio.h>
		#ifndef set_iopl
			#define set_iopl amd64_iopl
		#endif
	#elif !defined(NO_OBSD_DRIVER)
		#define NO_OBSD_DRIVER
	#endif

#endif // _WIN32
