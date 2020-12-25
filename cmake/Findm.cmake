# Findm.cmake
#
# Find libm, so we can link against it for math functions.
# If libm doesn't exist, linking against the m target will have no effect.

find_library(M_LIBRARY m)

# set in <project_root_dir>/CMakeLists.txt
# Windows: VCPKG_DIR = "<install_location>/vcpkg/packages/"
# x64: ARCHITECTURE = "_x64-windows"
set(M_DIR "${VCPKG_DIR}fdlibm${ARCHITECTURE}")

find_path(M_INCLUDE_DIR "fdlibm.h" HINTS "${M_DIR}/include" ${PC_M_INCLUDE_DIRS})

if(PC_M_VERSION)
	set(M_VERSION "${PC_M_VERSION}")
endif()

find_library(M_LIBRARY "fdlibm" HINTS "${M_DIR}/lib/manual-link" ${PC_M_LIBRARY_DIRS})

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(m
									FOUND_VAR M_FOUND
									REQUIRED_VARS M_LIBRARY
									VERSION_VAR M_VERSION)

if(M_FOUND)
	add_library(m::m UNKNOWN IMPORTED)
	#target_link_libraries(m INTERFACE "${M_LIBRARY}")

	set_target_properties(m::m PROPERTIES
		INTERFACE_COMPILE_OPTIONS "${PC_M_CFLAGS_OTHER}"
		INTERFACE_INCLUDE_DIRECTORIES "${M_INCLUDE_DIR}"
		IMPORTED_LOCATION "${M_LIBRARY}")
endif()
