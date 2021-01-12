# Find_.cmake
#
# Find _, so we can link against it for _ functions.
# If _ doesn't exist, linking against the _ target will have no effect.

#find_library(FREEGLUT_LIBRARY freeglut)

# set in <project_root_dir>/CMakeLists.txt
# Windows: VCPKG_DIR = "<install_location>/vcpkg/packages"
# x64: ARCHITECTURE = "_x64-windows"
set(FREEGLUT_DIR "${VCPKG_DIR}/freeglut${ARCHITECTURE}")

find_path(FREEGLUT_INCLUDE_DIR "freeglut.h" HINTS "${FREEGLUT_DIR}/include/GL" ${PC_FREEGLUT_INCLUDE_DIRS})

if(PC_FREEGLUT_VERSION)
	set(FREEGLUT_VERSION "${PC_FREEGLUT_VERSION}")
endif()

# PRE_FIX:
# LIBRARY:
# POS_FIX:
find_library(FREEGLUT_LIBRARY "freeglut" HINTS "${FREEGLUT_DIR}/lib" ${PC_FREEGLUT_LIBRARY_DIRS})

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(FREEGLUT FOUND_VAR FREEGLUT_FOUND REQUIRED_VARS FREEGLUT_LIBRARY VERSION_VAR FREEGLUT_VERSION)

if(FREEGLUT_FOUND)
	add_library(FREEGLUT::FREEGLUT UNKNOWN IMPORTED)

	set_target_properties(FREEGLUT::FREEGLUT PROPERTIES
							INTERFACE_COMPILE_OPTIONS "${FREEGLUT_CFLAGS_OTHER}"
							INTERFACE_INCLUDE_DIRECTORIES "${FREEGLUT_INCLUDE_DIR}"
							IMPORTED_LOCATION "${FREEGLUT_LIBRARY}")
endif()
