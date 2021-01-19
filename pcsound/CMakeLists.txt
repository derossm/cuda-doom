add_library(pcsound STATIC
			pcsound.cpp			pcsound.h
			pcsound_bsd.cpp
			pcsound_sdl.cpp
			pcsound_linux.cpp
			pcsound_win32.cpp
								pcsound_internal.h)

set_target_properties(pcsound PROPERTIES CXX_STANDARD 20 LANGUAGE CXX LINKER_LANGUAGE CXX)
target_include_directories(pcsound INTERFACE "." PRIVATE "${CMAKE_CURRENT_BINARY_DIR}/../")
target_link_libraries(pcsound SDL2::SDL2_mixer)
#target_precompile_headers(cuda17 PUBLIC "../derma/stdafx.h")

if(MSVC)
	target_compile_options(pcsound PUBLIC
		"/Zc:__cplusplus"
		"/Zc:wchar_t" "/Zc:inline" "/Zc:forScope" "/fp:precise"
		"/permissive-" "/Zc:preprocessor"
		"/W4" "/Od" "/arch:AVX2"
		"/EHsc"
		"/GS" "/Gd" "/Gm-" "/FC" "/nologo" "/WX-" "/MDd"
		#"/experimental:module"
		)
	target_compile_definitions(pcsound PUBLIC "WIN32_LEAN_AND_MEAN" "NOMINMAX")
endif()
