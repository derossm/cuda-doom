add_library(setup STATIC
								compatibility.h
								display.h
								execute.h
								joystick.h
								keyboard.h
								mainmenu.h
								mode.h
								mouse.h
								multiplayer.h
								sound.h
								txt_joyaxis.h
								txt_joybinput.h
								txt_keyinput.h
								txt_mouseinput.h
		#setup_icon.cc
		#setup.cpp
		)

set_target_properties(setup PROPERTIES CXX_STANDARD 20 LANGUAGE CXX LINKER_LANGUAGE CXX)

target_include_directories(setup PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/.." "${COMMON_INCLUDE_DIRS}")
target_link_libraries(setup textscreen SDL2::SDL2 SDL2::SDL2_mixer)
target_precompile_headers(textscreen PUBLIC "../../derma/stdafx.h")

if(MSVC)
	target_compile_options(setup PUBLIC "${WIN64_COMPILE_OPTIONS}")
	target_compile_definitions(setup PUBLIC "${COMMON_DEFINITIONS}")
else()
	target_compile_definitions(setup PUBLIC "SDL_MAIN_HANDLED")
endif()
