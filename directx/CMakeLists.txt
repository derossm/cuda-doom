#set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/../cmake")

cmake_minimum_required(VERSION 3.7.2)
project("CUDA Doom DX12" VERSION 0.0.1 LANGUAGES CUDA CXX)

# Autotools variables
#set(top_srcdir ${CMAKE_CURRENT_SOURCE_DIR})
#set(top_builddir ${CMAKE_CURRENT_BINARY_DIR})

# AC_INIT variables
#set(PACKAGE_NAME "${PROJECT_NAME}")
#set(PACKAGE_TARNAME "cuda-doom")
#set(PACKAGE_VERSION "${PROJECT_VERSION}")
#set(PACKAGE_STRING "${PROJECT_NAME} ${PROJECT_VERSION}")
#set(PACKAGE_BUGREPORT "https://github.com/derossm/cuda-doom")

#string(REGEX REPLACE " Doom$" "" PACKAGE_SHORTNAME "${PACKAGE_NAME}")
#set(PACKAGE_COPYRIGHT "Copyright© 2020-2021")
#set(PACKAGE_LICENSE "CUDA Doom released under the GNU All-permissive License, where allowed by law.")

#set(CMAKE_CXX_STANDARD 17)
#set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Any settings that should apply to all targets in this directory and all subdirectories should go here. Use judiciously.
#if(MSVC)
#	#add_definitions("/D_CRT_SECURE_NO_WARNINGS" "/D_CRT_SECURE_NO_DEPRECATE" "/D_CRT_NONSTDC_NO_DEPRECATE")
#	add_compile_options(
#		"/Zc:__cplusplus"
#		"/Zc:wchar_t" "/Zc:inline" "/Zc:forScope" "/fp:precise" "/EHsc" "/GS" "/Gd" "/Gm-" "/FC" "/nologo" "/WX-" "/MD"
#		"/permissive-" "/Zc:preprocessor"
#		"/W4" "/Od" "/arch:AVX2"
#		#"/wC4005"
#		#"/experimental:module"
#		#"/Zc:externConstexpr"
#		)
#	add_compile_definitions("WIN32_LEAN_AND_MEAN" "NOMINMAX"
#							#"SDL_MAIN_HANDLED"
#							#"MICROSOFT_WINDOWS_WINBASE_H_DEFINE_INTERLOCKED_CPLUSPLUS_OVERLOADS")
#	)
#else()
#	add_compile_options("-Wall" "-Wdeclaration-after-statement" "-Wredundant-decls")
#endif()

#set(VCPKG_DIR "G:/dev/vcpkg/packages")
#set(ARCHITECTURE "_x64-windows")

#string(CONCAT WINDOWS_RC_VERSION "${PROJECT_VERSION_MAJOR}, " "${PROJECT_VERSION_MINOR}, ${PROJECT_VERSION_PATCH}, 0")

# Without a hyphen. This is used for the bash-completion scripts.
#string(TOLOWER "${PACKAGE_SHORTNAME}" PROGRAM_SPREFIX)

# With a hyphen, used almost everywhere else.
#set(PROGRAM_PREFIX "${PROGRAM_SPREFIX}-")

#configure_file(../cmake/config.h.cin config.h)
#configure_file(src/resource.rc.in src/resource.rc)

#foreach(SUBDIR derma textscreen midiproc opl pcsound src)
#	add_subdirectory("${SUBDIR}")
#endforeach()

#set(DERMA_FILES
#									common.h
#									bitmanipulator.h
#									enumbitset.h
#									bytetype.h
#									catch.h
#		test_bytetype.cpp							)

set(DX12_FILES
		stdafx.cpp					stdafx.h
		#shaders.hlsl							
									d3dx12.h
		Win32Application.cpp		Win32Application.h
									DXSampleHelper.h
									ShaderStructs.h
		Main.cpp
		DX12CudaSample.cpp			DX12CudaSample.h
		simpleD3D12.cpp				simpleD3D12.h
)
add_library(cuda17 STATIC sinewave_cuda.cu)
set_target_properties(cuda17 PROPERTIES CUDA_ARCHITECTURES 61 CXX_STANDARD 17)
target_include_directories(cuda17 INTERFACE "." PRIVATE "inc" "src" "data" "../derma")

#if(MSVC)
#	#add_definitions("/D_CRT_SECURE_NO_WARNINGS" "/D_CRT_SECURE_NO_DEPRECATE" "/D_CRT_NONSTDC_NO_DEPRECATE")
#	target_compile_options(cuda17 PUBLIC
#		"/Zc:__cplusplus"
#		#"/Zc:wchar_t" "/Zc:inline" "/Zc:forScope" "/fp:precise" "/EHsc" "/GS" "/Gd" "/Gm-" "/FC" "/nologo" "/WX-" "/MD"
#		#"/permissive-" "/Zc:preprocessor"
#		#"/W4" "/Od" "/arch:AVX2"
#		#"/wC4005"
#		#"/experimental:module"
#		#"/Zc:externConstexpr"
#		"-O0" #"-g"
#		)
#	target_compile_definitions(cuda17 PUBLIC
#		"WIN32_LEAN_AND_MEAN" "NOMINMAX"
#		#"SDL_MAIN_HANDLED"
#		#"MICROSOFT_WINDOWS_WINBASE_H_DEFINE_INTERLOCKED_CPLUSPLUS_OVERLOADS")
#	)
#else()
#	target_compile_definitions(cuda17 PUBLIC
#		"-Wall" "-Wdeclaration-after-statement" "-Wredundant-decls")
#endif()
set(CMAKE_BUILD_TYPE Debug)

set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /SUBSYSTEM:WINDOWS /ENTRY:WinMainCRTStartup")
add_executable("${PROGRAM_PREFIX}dx12" WIN32 main.cpp ${DX12_FILES})
#set_target_properties("${PROGRAM_PREFIX}dx12" PROPERTIES CXX_STANDARD 20 LANGUAGE CXX)
target_link_libraries("${PROGRAM_PREFIX}dx12" FREEGLUT::FREEGLUT cuda17 d3d12 d3dcompiler dxgi #cudart_static 
						#kernel32 user32 gdi32 winspool comdlg32 advapi32 shell32 ole32 oleaut32 uuid odbc32 odbccp32.lib
						)
target_include_directories("${PROGRAM_PREFIX}dx12" INTERFACE "." PRIVATE "inc" "src" "data" "../derma" "R:/api/cuda/v11.1/tools/include")
target_link_directories("${PROGRAM_PREFIX}dx12" PRIVATE "R:/api/cuda/v11.1/tools/lib/x64")
#target_compile_options("${PROGRAM_PREFIX}dx12" PRIVATE $<$<COMPILE_LANGUAGE:CUDA>: -arch=sm_61>)
#set_target_properties("${PROGRAM_PREFIX}dx12" PROPERTIES $<$<COMPILE_LANGUAGE:CUDA>: -std=c++20>)
set_target_properties("${PROGRAM_PREFIX}dx12" PROPERTIES WIN32_EXECUTABLE TRUE CXX_STANDARD 20 LANGUAGE CXX LINKER_LANGUAGE CXX)

if(MSVC)
	#add_definitions("/D_CRT_SECURE_NO_WARNINGS" "/D_CRT_SECURE_NO_DEPRECATE" "/D_CRT_NONSTDC_NO_DEPRECATE")
	target_compile_options("${PROGRAM_PREFIX}dx12" PUBLIC
		"/Zc:__cplusplus"
		"/Zc:wchar_t" "/Zc:inline" "/Zc:forScope" "/fp:precise"
		"/EHsc"
		#"/GS" "/Gd" "/Gm-" "/FC" "/nologo" "/WX-" "/MDd"
		"/permissive-" "/Zc:preprocessor"
		"/W4" "/Od" "/arch:AVX2"
		#"/wC4005"
		#"/experimental:module"
		#"/Zc:externConstexpr"
		)
	target_compile_definitions("${PROGRAM_PREFIX}dx12" PUBLIC
		"WIN32_LEAN_AND_MEAN" "NOMINMAX" "_ITERATOR_DEBUG_LEVEL=2"
		#"SDL_MAIN_HANDLED"
		#"MICROSOFT_WINDOWS_WINBASE_H_DEFINE_INTERLOCKED_CPLUSPLUS_OVERLOADS"
		)
else()
	target_compile_options("${PROGRAM_PREFIX}dx12" PUBLIC
		"-Wall" "-Wdeclaration-after-statement" "-Wredundant-decls")
endif()

