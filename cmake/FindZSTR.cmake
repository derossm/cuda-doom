# FindZSTR.cmake
#
# Find libZSTR, so we can link against it for ZSTR functions.
# If libZSTR doesn't exist, linking against the ZSTR target will have no effect.
#
# Header-ONLY library : zstr.hpp, strict_fstream.hpp

#find_package(ZSTR REQUIRED)

# set in <project_root_dir>/CMakeLists.txt
# Windows: VCPKG_DIR = "<install_location>/vcpkg/packages"
# x64: ARCHITECTURE = "_x64-windows"
set(ZSTR_DIR "${VCPKG_DIR}/ZSTR${ARCHITECTURE}")

find_path(ZSTR_INCLUDE_DIR "zstr.hpp" HINTS "${ZSTR_DIR}/include" ${PC_ZSTR_INCLUDE_DIRS})

if(PC_ZSTR_VERSION)
	set(ZSTR_VERSION "${PC_ZSTR_VERSION}")
endif()
