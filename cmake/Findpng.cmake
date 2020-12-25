# Findpng.cmake
#
# Find libpng, so we can link against it for png functions.
# If libpng doesn't exist, linking against the png target will have no effect.

#find_library(PNG_LIBRARY png)

# set in <project_root_dir>/CMakeLists.txt
# Windows: VCPKG_DIR = "<install_location>/vcpkg/packages/"
# x64: ARCHITECTURE = "_x64-windows"
set(PNG_DIR "${VCPKG_DIR}libpng${ARCHITECTURE}")

find_path(PNG_INCLUDE_DIR "png.h" HINTS "${PNG_DIR}/include" ${PC_PNG_INCLUDE_DIRS})

if(PC_PNG_VERSION)
	set(PNG_VERSION "${PC_PNG_VERSION}")
endif()

# PRE_FIX: lib
# LIBRARY: png
# POS_FIX: 16
find_library(PNG_LIBRARY "libpng16" HINTS "$PNG_DIR}/lib" ${PC_PNG_LIBRARY_DIRS})

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(PNG
									FOUND_VAR PNG_FOUND
									REQUIRED_VARS PNG_LIBRARY
									VERSION_VAR PNG_VERSION)

if(PNG_FOUND)
	add_library(PNG::PNG UNKNOWN IMPORTED)
	#target_link_libraries(png INTERFACE "${PNG_LIBRARY}")

	set_target_properties(PNG::PNG PROPERTIES
		INTERFACE_COMPILE_OPTIONS "${PNG_CFLAGS_OTHER}"
		INTERFACE_INCLUDE_DIRECTORIES "${PNG_INCLUDE_DIR}"
		IMPORTED_LOCATION "${PNG_LIBRARY}")
endif()
