# Findfreeglut.cmake
#
# Find freeglut, so we can link against it forfreeglut functions.
# If freeglut doesn't exist, linking against the freeglut target will have no effect.

#find_package(GLUT REQUIRED)
#target_link_libraries(main PRIVATE GLUT::GLUT)

# set in <project_root_dir>/CMakeLists.txt
# Windows: VCPKG_DIR = "<install_location>/vcpkg/packages"
# x64: ARCHITECTURE = "_x64-windows"
set(GLUT_DIR "${VCPKG_DIR}/freeglut${ARCHITECTURE}")

find_path(GLUT_INCLUDE_DIR "freeglut.h" HINTS "${GLUT_DIR}/include/GL" ${PC_GLUT_INCLUDE_DIRS})

if(PC_GLUT_VERSION)
	set(GLUT_VERSION "${PC_GLUT_VERSION}")
endif()

# PRE_FIX:
# LIBRARY:
# POS_FIX:
find_library(GLUT_LIBRARY "freeglut" HINTS "${GLUT_DIR}/lib" ${PC_GLUT_LIBRARY_DIRS})

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(GLUT FOUND_VAR GLUT_FOUND REQUIRED_VARS GLUT_LIBRARY VERSION_VAR GLUT_VERSION)

if(GLUT_FOUND)
	add_library(GLUT::GLUT UNKNOWN IMPORTED)

	set_target_properties(GLUT::GLUT PROPERTIES
							INTERFACE_COMPILE_OPTIONS "${GLUT_CFLAGS_OTHER}"
							INTERFACE_INCLUDE_DIRECTORIES "${GLUT_INCLUDE_DIR}"
							IMPORTED_LOCATION "${GLUT_LIBRARY}")
endif()
