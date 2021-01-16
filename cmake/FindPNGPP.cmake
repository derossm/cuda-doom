# FindPNGPP.cmake
#
# Find libPNGPP, so we can link against it for PNGPP functions.
# If libPNGPP doesn't exist, linking against the PNGPP target will have no effect.
#
# Header-ONLY library : png.hpp, ... +28 *.hpp

#find_package(PNGPP REQUIRED)

# set in <project_root_dir>/CMakeLists.txt
# Windows: VCPKG_DIR = "<install_location>/vcpkg/packages"
# x64: ARCHITECTURE = "_x64-windows"
set(PNGPP_DIR "${VCPKG_DIR}/PNGPP${ARCHITECTURE}")

find_path(PNGPP_INCLUDE_DIR "png.hpp" HINTS "${PNGPP_DIR}/include" ${PC_PNGPP_INCLUDE_DIRS})

if(PC_PNGPP_VERSION)
	set(PNGPP_VERSION "${PC_PNGPP_VERSION}")
endif()
