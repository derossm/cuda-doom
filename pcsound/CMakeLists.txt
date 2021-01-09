add_library(pcsound STATIC
			pcsound.cpp			pcsound.h
			pcsound_bsd.cpp
			pcsound_sdl.cpp
			pcsound_linux.cpp
			pcsound_win32.cpp
								pcsound_internal.h)

set_target_properties(pcsound PROPERTIES LINKER_LANGUAGE CXX)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

target_include_directories(pcsound INTERFACE "." PRIVATE "${CMAKE_CURRENT_BINARY_DIR}/../")
target_link_libraries(pcsound SDL2::mixer)
