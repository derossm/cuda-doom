add_library(pcsound STATIC
			pcsound.cpp			pcsound.h
								pcsound_internal.h
			pcsound_sdl.cpp
			pcsound_win32.cpp
			pcsound_linux.cpp
			pcsound_bsd.cpp)

set_target_properties(pcsound PROPERTIES CXX_STANDARD 20 LANGUAGE CXX LINKER_LANGUAGE CXX)
target_include_directories(pcsound PUBLIC "${COMMON_INCLUDE_DIRS}")
target_link_libraries(pcsound SDL2::SDL2_mixer)
target_precompile_headers(pcsound PUBLIC "../derma/stdafx.h")

if(MSVC)
	target_compile_options(pcsound PUBLIC "${WIN64_COMPILE_OPTIONS}")
	target_compile_definitions(pcsound PUBLIC "${COMMON_DEFINITIONS}")
else()
	target_compile_definitions(pcsound PUBLIC "SDL_MAIN_HANDLED")
endif()
