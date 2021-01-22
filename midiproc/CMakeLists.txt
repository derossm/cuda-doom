
###
if(WIN32)
	add_executable("${PROGRAM_PREFIX}midiproc" WIN32 buffer.h main.cpp) 
else()
	add_executable("${PROGRAM_PREFIX}midiproc" buffer.h main.cpp) 
endif()

set_target_properties("${PROGRAM_PREFIX}midiproc" PROPERTIES CXX_STANDARD 20 LANGUAGE CXX LINKER_LANGUAGE CXX)
target_include_directories("${PROGRAM_PREFIX}midiproc" PUBLIC "${COMMON_INCLUDE_DIRS}")
target_link_libraries("${PROGRAM_PREFIX}midiproc" SDL2::SDL2main SDL2::SDL2 SDL2::SDL2_mixer)
target_precompile_headers("${PROGRAM_PREFIX}midiproc" PUBLIC "../derma/stdafx.h")

if(MSVC)
	target_compile_options("${PROGRAM_PREFIX}midiproc" PUBLIC "${WIN64_COMPILE_OPTIONS}")
	target_compile_definitions("${PROGRAM_PREFIX}midiproc" PUBLIC "${COMMON_DEFINITIONS}")
else()
	target_compile_definitions("${PROGRAM_PREFIX}midiproc" PUBLIC "SDL_MAIN_HANDLED")
endif()