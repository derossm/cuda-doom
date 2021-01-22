add_library(opl STATIC
							opl.h
							opl3.h
							opl3_chip.h
							opl3_channel.h
							opl3_slot.h
							opl_timer.h
							opl_sdl.h
							ioperm_sys.h
			opl_win32.cpp
			opl_linux.cpp
			opl_obsd.cpp)

set_target_properties(opl PROPERTIES CXX_STANDARD 20 LANGUAGE CXX LINKER_LANGUAGE CXX)
target_include_directories(opl PUBLIC "${COMMON_INCLUDE_DIRS}")
target_link_libraries(opl SDL2::SDL2main SDL2::SDL2 SDL2::SDL2_mixer)
target_precompile_headers(opl PUBLIC "../derma/stdafx.h")

if(MSVC)
	target_compile_options(opl PUBLIC "${WIN64_COMPILE_OPTIONS}")
	target_compile_definitions(opl PUBLIC "${COMMON_DEFINITIONS}")
else()
	target_compile_definitions(opl PUBLIC "SDL_MAIN_HANDLED")
endif()
