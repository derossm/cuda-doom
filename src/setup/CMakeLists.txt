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

target_include_directories(setup PRIVATE "../" "${CMAKE_CURRENT_BINARY_DIR}/../../")
target_link_libraries(setup textscreen SDL2::SDL2 SDL2::SDL2_mixer)
target_precompile_headers(textscreen PUBLIC "../../derma/stdafx.h")

if(MSVC)
	target_compile_options(setup PUBLIC
		"/Zc:__cplusplus"
		"/Zc:wchar_t" "/Zc:inline" "/Zc:forScope" "/fp:precise"
		"/permissive-" "/Zc:preprocessor"
		"/W4" "/Od" "/arch:AVX2"
		"/EHsc"
		"/GS" "/Gd" "/Gm-" "/FC" "/nologo" "/WX-" "/MDd"
		#"/experimental:module"
		)
	target_compile_definitions(setup PUBLIC "WIN32_LEAN_AND_MEAN" "NOMINMAX")
endif()
