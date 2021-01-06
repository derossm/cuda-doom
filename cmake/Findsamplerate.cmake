# Findsamplerate.cmake
#
# Copyright (c) 2018, Alex Mayfield <alexmax2742@gmail.com>
# All rights reserved.
#
# Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:
#	* Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
#	* Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the
#		documentation and/or other materials provided with the distribution.
#	* Neither the name of the <organization> nor the names of its contributors may be used to endorse or promote products derived from this
#		software without specific prior written permission.
#
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED
# TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL <COPYRIGHT HOLDER> BE
# LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
# SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER
# IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
# ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

# set in <project_root_dir>/CMakeLists.txt
# Windows: VCPKG_DIR = "<install_location>/vcpkg/packages"
# x64: ARCHITECTURE = "_x64-windows"
set(SAMPLERATE_DIR "${VCPKG_DIR}/libsamplerate${ARCHITECTURE}/")

find_path(SAMPLERATE_INCLUDE_DIR "samplerate.h" HINTS "${SAMPLERATE_DIR}/include" ${PC_SAMPLERATE_INCLUDE_DIRS})

if(PC_SAMPLERATE_VERSION)
	set(SAMPLERATE_VERSION "${PC_SAMPLERATE_VERSION}")
endif()

# PRE_FIX: lib
# LIBRARY: sameplerate
# POS_FIX: -0
find_library(SAMPLERATE_LIBRARY "libsamplerate-0" HINTS "${SAMPLERATE_DIR}/lib" ${PC_SAMPLERATE_LIBRARY_DIRS})

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(samplerate FOUND_VAR SAMPLERATE_FOUND REQUIRED_VARS SAMPLERATE_INCLUDE_DIR SAMPLERATE_LIBRARY VERSION_VAR SAMPLERATE_VERSION)

if(SAMPLERATE_FOUND)
	add_library(samplerate::samplerate UNKNOWN IMPORTED)

	set_target_properties(samplerate::samplerate PROPERTIES
							INTERFACE_COMPILE_OPTIONS "${PC_SAMPLERATE_CFLAGS_OTHER}"
							INTERFACE_INCLUDE_DIRECTORIES "${SAMPLERATE_INCLUDE_DIR}"
							IMPORTED_LOCATION "${SAMPLERATE_LIBRARY}")
endif()
