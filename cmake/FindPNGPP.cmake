# FindPNGPP.cmake
#
# Find libPNGPP, so we can link against it for PNGPP functions.
# If libPNGPP doesn't exist, linking against the PNGPP target will have no effect.
#
# Header-ONLY library : png.hpp, ... +28 *.hpp

# set in <project_root_dir>/CMakeLists.txt
# VCPKG_DIR = "<install_location>/vcpkg/installed"
# ARCHITECTURE = "x64"
# PLATFORM = "windows"
# VCPKG_INCLUDE_DIR = "${VCPKG_DIR}/${ARCHITECTURE}-${PLATFORM}/include")
# VCPKG_LIBRARY_DIR = "${VCPKG_DIR}/${ARCHITECTURE}-${PLATFORM}/lib")

# PRE_FIX:
# LIBRARY: pngpp
# POS_FIX:
# SUB_FOLDER: png++

find_path(PNGPP_INCLUDE_DIR "png.hpp" HINTS "${VCPKG_INCLUDE_DIR}/png++" ${PC_PNGPP_INCLUDE_DIRS})

if(PC_PNGPP_VERSION)
	set(PNGPP_VERSION "${PC_PNGPP_VERSION}")
endif()

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(pngpp FOUND_VAR PNGPP_FOUND REQUIRED_VARS PNGPP_INCLUDE_DIR VERSION_VAR PNGPP_VERSION)

if(PNGPP_FOUND)
	add_library(pngpp::pngpp UNKNOWN IMPORTED)

	set_target_properties(pngpp::pngpp PROPERTIES
							INTERFACE_COMPILE_OPTIONS "${PC_PNGPP_CFLAGS_OTHER}"
							INTERFACE_INCLUDE_DIRECTORIES "${PNGPP_INCLUDE_DIR}")
endif()
