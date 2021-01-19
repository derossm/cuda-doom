set(DERMA_FILES
									stdafx.h
									common.h
									d_windows.h
									d_unix.h
									deprecating_tools.h
									bitmanipulator.h
									enumbitset.h
									bytetype.h
		test_bytetype.cpp			catch.h)

add_executable("${PROGRAM_PREFIX}catch2_byte" ${DERMA_FILES})
set_target_properties("${PROGRAM_PREFIX}catch2_byte" PROPERTIES CXX_STANDARD 20 LANGUAGE CXX LINKER_LANGUAGE CXX)
target_include_directories("${PROGRAM_PREFIX}catch2_byte" INTERFACE "." PRIVATE "../derma")
#target_precompile_headers("${PROGRAM_PREFIX}catch2_byte" PUBLIC stdafx.h)

if(MSVC)
	target_compile_options("${PROGRAM_PREFIX}catch2_byte" PUBLIC
		"/Zc:__cplusplus"
		"/Zc:wchar_t" "/Zc:inline" "/Zc:forScope" "/fp:precise"
		"/permissive-" "/Zc:preprocessor"
		"/W4" "/Od" "/arch:AVX2"
		"/EHsc"
		"/GS" "/Gd" "/Gm-" "/FC" "/nologo" "/WX-" "/MDd"
		#"/experimental:module"
		)
	target_compile_definitions("${PROGRAM_PREFIX}catch2_byte" PUBLIC "WIN32_LEAN_AND_MEAN" "NOMINMAX")
endif()
