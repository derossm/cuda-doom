# Findm.cmake
#
# Find libm, so we can link against it for math functions.
# If libm doesn't exist, linking against the m target will have no effect.

# set in <project_root_dir>/CMakeLists.txt
# VCPKG_DIR = "<install_location>/vcpkg/installed"
# ARCHITECTURE = "x64"
# PLATFORM = "windows"
# VCPKG_INCLUDE_DIR = "${VCPKG_DIR}/${ARCHITECTURE}-${PLATFORM}/include")
# VCPKG_LIBRARY_DIR = "${VCPKG_DIR}/${ARCHITECTURE}-${PLATFORM}/lib")

# PRE_FIX: fdlib
# LIBRARY: m
# POS_FIX:
# SUB_FOLDER: lib/manual-link

find_path(M_INCLUDE_DIR "fdlibm.h" HINTS "${VCPKG_INCLUDE_DIR}" ${PC_M_INCLUDE_DIRS})

if(PC_M_VERSION)
	set(M_VERSION "${PC_M_VERSION}")
endif()

find_library(M_LIBRARY "fdlibm" HINTS "${VCPKG_LIBRARY_DIR}/manual-link" ${PC_M_LIBRARY_DIRS})

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(m FOUND_VAR m_FOUND REQUIRED_VARS M_LIBRARY VERSION_VAR M_VERSION)

if(m_FOUND)
	add_library(m::m UNKNOWN IMPORTED)

	set_target_properties(m::m PROPERTIES
							INTERFACE_COMPILE_OPTIONS "${PC_M_CFLAGS_OTHER}"
							INTERFACE_INCLUDE_DIRECTORIES "${M_INCLUDE_DIR}"
							IMPORTED_LOCATION "${M_LIBRARY}")
endif()
