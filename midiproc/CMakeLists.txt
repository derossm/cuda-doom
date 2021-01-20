if(WIN32)
	add_executable("${PROGRAM_PREFIX}midiproc" buffer.h main.cpp) 
	set_target_properties("${PROGRAM_PREFIX}midiproc" PROPERTIES CXX_STANDARD 20 LANGUAGE CXX LINKER_LANGUAGE CXX)
	target_include_directories("${PROGRAM_PREFIX}midiproc" PRIVATE ".;${CMAKE_CURRENT_BINARY_DIR}/../;${VCPKG_INCLUDE_DIR};${CUDA_INCLUDE_DIR}")
	target_link_libraries("${PROGRAM_PREFIX}midiproc" SDL2::SDL2main SDL2::SDL2 SDL2::SDL2_mixer)
	#target_precompile_headers("${PROGRAM_PREFIX}midiproc" PUBLIC "../derma/stdafx.h")

	target_compile_options("${PROGRAM_PREFIX}midiproc" PUBLIC
		"/Zc:__cplusplus"
		"/Zc:wchar_t" "/Zc:inline" "/Zc:forScope" "/fp:precise"
		"/permissive-" "/Zc:preprocessor"
		"/W4" "/Od" "/arch:AVX2"
		"/EHsc"
		"/GS" "/Gd" "/Gm-" "/FC" "/nologo" "/WX-" "/MDd"
		#"/experimental:module"
		)
	target_compile_definitions("${PROGRAM_PREFIX}midiproc" PUBLIC "WIN32_LEAN_AND_MEAN" "NOMINMAX")
endif()
