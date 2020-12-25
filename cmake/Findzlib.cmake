# Findzlib.cmake
#
# Find libzlib, so we can link against it for zlib functions.
# If libzlib doesn't exist, linking against the zlib target will have no effect.

#find_library(ZLIB_LIBRARY zlib)

# set in <project_root_dir>/CMakeLists.txt
# Windows: VCPKG_DIR = "<install_location>/vcpkg/packages/"
# x64: ARCHITECTURE = "_x64-windows"
set(ZLIB_DIR "${VCPKG_DIR}zlib${ARCHITECTURE}")

find_path(ZLIB_INCLUDE_DIR "zlib.h" HINTS "${ZLIB_DIR}/include" ${PC_ZLIB_INCLUDE_DIRS})

if(PC_ZLIB_VERSION)
	set(ZLIB_VERSION "${PC_ZLIB_VERSION}")
endif()

find_library(ZLIB_LIBRARY "zlib" HINTS "$ZLIB_DIR}/lib" ${PC_ZLIB_LIBRARY_DIRS})

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(zlib
									FOUND_VAR ZLIB_FOUND
									REQUIRED_VARS ZLIB_LIBRARY
									VERSION_VAR ZLIB_VERSION)

if(ZLIB_FOUND)
	add_library(zlib::zlib UNKNOWN IMPORTED)
	#target_link_libraries(zlib INTERFACE "${ZLIB_LIBRARY}")

	set_target_properties(zlib::zlib PROPERTIES
		INTERFACE_COMPILE_OPTIONS "${PC_ZLIB_CFLAGS_OTHER}"
		INTERFACE_INCLUDE_DIRECTORIES "${ZLIB_INCLUDE_DIR}"
		IMPORTED_LOCATION "${ZLIB_LIBRARY}")
endif()
