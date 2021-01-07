set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

cmake_minimum_required(VERSION 3.7.2)
project("CUDA Doom" VERSION 0.0.1 LANGUAGES CXX)

# Autotools variables
set(top_srcdir ${CMAKE_CURRENT_SOURCE_DIR})
set(top_builddir ${CMAKE_CURRENT_BINARY_DIR})

# AC_INIT variables
set(PACKAGE_NAME "${PROJECT_NAME}")
set(PACKAGE_TARNAME "cuda-doom")
set(PACKAGE_VERSION "${PROJECT_VERSION}")
set(PACKAGE_STRING "${PROJECT_NAME} ${PROJECT_VERSION}")
set(PACKAGE_BUGREPORT "https://github.com/derossm/cuda-doom")

string(REGEX REPLACE " Doom$" "" PACKAGE_SHORTNAME "${PACKAGE_NAME}")
set(PACKAGE_COPYRIGHT "Copyright (C) 1993-2021")
set(PACKAGE_LICENSE "Crsipy Doom released under the GNU General Public License, version 2. Any original work added by CUDA Doom released under the GNU All-permissive License, where allowed by law.")

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Any settings that should apply to all targets in this directory and all subdirectories should go here. Use judiciously.
if(MSVC)
	#add_definitions("/D_CRT_SECURE_NO_WARNINGS" "/D_CRT_SECURE_NO_DEPRECATE" "/D_CRT_NONSTDC_NO_DEPRECATE")
	add_compile_options(
		"/Zc:__cplusplus"
		"/Zc:wchar_t" "/Zc:inline" "/Zc:forScope" "/fp:precise" "/EHsc" "/GS" "/Gd" "/Gm-" "/FC" "/nologo" "/WX-" "/MD"
		"/permissive-" "/Zc:preprocessor"
		"/W4" "/Od" "/arch:AVX2"
		#"/experimental:module"
		#"/Zc:externConstexpr"
		)
	add_compile_definitions("WIN32_LEAN_AND_MEAN" "NOMINMAX")
else()
	add_compile_options("-Wall" "-Wdeclaration-after-statement" "-Wredundant-decls")
endif()

set(VCPKG_DIR "G:/dev/vcpkg/packages")
set(ARCHITECTURE "_x64-windows")

# libm or fdlibm (Freely Distributable libm) : C math library IEEE 754 floating-point arithmetic
find_package(m 5.3 REQUIRED)
if(M_FOUND)
	set(HAVE_LIBM TRUE)
endif()

# zlib : compression library
find_package(zlib 1.2.0 REQUIRED)
if(ZLIB_FOUND)
	set(HAVE_ZLIB TRUE)
endif()

# libpng : interface for reading/writing PNG format files
find_package(PNG 1.6.0 REQUIRED)
if(PNG_FOUND)
	set(HAVE_LIBPNG TRUE)
endif()

# samplerate : samplerate converter for audio
find_package(samplerate 0.1.0 REQUIRED)
if(SAMPLERATE_FOUND)
	set(HAVE_LIBSAMPLERATE TRUE)
endif()

# sdl2 : Simple DirectMedia Layer
find_package(SDL2 2.0.0 REQUIRED)
find_package(SDL2_mixer 2.0.0 REQUIRED)
find_package(SDL2_net 2.0.0 REQUIRED)

#include(CheckSymbolExists)
#include(CheckIncludeFile)

string(CONCAT WINDOWS_RC_VERSION "${PROJECT_VERSION_MAJOR}, " "${PROJECT_VERSION_MINOR}, ${PROJECT_VERSION_PATCH}, 0")

# Without a hyphen. This is used for the bash-completion scripts.
string(TOLOWER "${PACKAGE_SHORTNAME}" PROGRAM_SPREFIX)

# With a hyphen, used almost everywhere else.
set(PROGRAM_PREFIX "${PROGRAM_SPREFIX}-")

configure_file(cmake/config.h.cin config.h)
configure_file(src/resource.rc.in src/resource.rc)
configure_file(src/setup-res.rc.in src/setup-res.rc)
configure_file(src/setup/setup-manifest.xml.in src/setup/setup-manifest.xml)

foreach(SUBDIR derma textscreen midiproc opl pcsound src)
	add_subdirectory("${SUBDIR}")
endforeach()
