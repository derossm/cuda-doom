# Find_.cmake
#
# Find _, so we can link against it for _ functions.
# If _ doesn't exist, linking against the _ target will have no effect.

# set in <project_root_dir>/CMakeLists.txt
# Windows: VCPKG_DIR = "<install_location>/vcpkg/packages"
# x64: ARCHITECTURE = "_x64-windows"
set(CUDA_DIR "R:/api/cuda/v11.1/tools")

find_path(CUDA_INCLUDE_DIR "cuda.h" HINTS "${CUDA_DIR}/include" ${PC_CUDA_INCLUDE_DIRS})

if(PC_CUDA_VERSION)
	set(CUDA_VERSION "${PC_CUDA_VERSION}")
endif()

# PRE_FIX:
# LIBRARY:
# POS_FIX:
find_library(CUDA_LIBRARY "cuda" HINTS "${CUDA_DIR}/lib/x64" ${PC_CUDA_LIBRARY_DIRS})

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(CUDA FOUND_VAR CUDA_FOUND REQUIRED_VARS CUDA_LIBRARY VERSION_VAR CUDA_VERSION)

if(CUDA_FOUND)
	add_library(CUDA::CUDA UNKNOWN IMPORTED)

	set_target_properties(CUDA::CUDA PROPERTIES
							INTERFACE_COMPILE_OPTIONS "${CUDA_CFLAGS_OTHER}"
							INTERFACE_INCLUDE_DIRECTORIES "${CUDA_INCLUDE_DIR}"
							IMPORTED_LOCATION "${CUDA_LIBRARY}")
endif()
