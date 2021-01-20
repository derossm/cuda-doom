# Findzstr.cmake
#
# Find zstr, so we can link against it for zstr functions.
# If zstr doesn't exist, linking against the zstr target will have no effect.
#
# Header-ONLY library : zstr.hpp, strict_fstream.hpp

# set in <project_root_dir>/CMakeLists.txt
# VCPKG_DIR = "<install_location>/vcpkg/installed"
# ARCHITECTURE = "x64"
# PLATFORM = "windows"
# VCPKG_INCLUDE_DIR = "${VCPKG_DIR}/${ARCHITECTURE}-${PLATFORM}/include")
# VCPKG_LIBRARY_DIR = "${VCPKG_DIR}/${ARCHITECTURE}-${PLATFORM}/lib")

# PRE_FIX:
# LIBRARY: zstr
# POS_FIX:
# SUB_FOLDER: zstr

find_path(ZSTR_INCLUDE_DIR "zstr.hpp" HINTS "${VCPKG_INCLUDE_DIR}/zstr" ${PC_ZSTR_INCLUDE_DIRS})

if(PC_ZSTR_VERSION)
	set(ZSTR_VERSION "${PC_ZSTR_VERSION}")
endif()

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(zstr FOUND_VAR ZSTR_FOUND REQUIRED_VARS ZSTR_INCLUDE_DIR VERSION_VAR ZSTR_VERSION)

if(ZSTR_FOUND)
	add_library(zstr::zstr UNKNOWN IMPORTED)

	set_target_properties(zstr::zstr PROPERTIES
							INTERFACE_COMPILE_OPTIONS "${PC_ZSTR_CFLAGS_OTHER}"
							INTERFACE_INCLUDE_DIRECTORIES "${ZSTR_INCLUDE_DIR}")
endif()
