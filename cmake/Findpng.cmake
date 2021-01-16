# Findpng.cmake
#
# Find libpng, so we can link against it for png functions.
# If libpng doesn't exist, linking against the png target will have no effect.

find_package(libpng CONFIG REQUIRED)
#target_link_libraries(main PRIVATE libpng)

# set in <project_root_dir>/CMakeLists.txt
# Windows: VCPKG_DIR = "<install_location>/vcpkg/packages"
# x64: ARCHITECTURE = "_x64-windows"
set(libpng_DIR "${VCPKG_DIR}/libpng${ARCHITECTURE}")

find_path(libpng_INCLUDE_DIR "png.h" HINTS "${libpng_DIR}/include" ${PC_libpng_INCLUDE_DIRS})

if(PC_libpng_VERSION)
	set(libpng_VERSION "${PC_libpng_VERSION}")
endif()

# PRE_FIX: lib
# LIBRARY: png
# POS_FIX: 16
find_library(libpng_LIBRARY "libpng16" HINTS "${libpng_DIR}/lib" ${PC_libpng_LIBRARY_DIRS})

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(libpng FOUND_VAR libpng_FOUND REQUIRED_VARS libpng_LIBRARY VERSION_VAR libpng_VERSION)

if(libpng_FOUND)
	add_library(libpng::libpng UNKNOWN IMPORTED)

	set_target_properties(libpng::libpng PROPERTIES
							INTERFACE_COMPILE_OPTIONS "${libpng_CFLAGS_OTHER}"
							INTERFACE_INCLUDE_DIRECTORIES "${libpng_INCLUDE_DIR}"
							IMPORTED_LOCATION "${libpng_LIBRARY}")
endif()
