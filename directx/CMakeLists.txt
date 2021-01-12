project("CUDA Doom DX12" VERSION 0.0.1 LANGUAGES CUDA CXX)

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

#set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /SUBSYSTEM:Windows /ENTRY:WinMainCRTStartup")
add_executable("${PROGRAM_PREFIX}dx12" WIN32 main.cpp ${DX12_FILES})
#target_link_options("${PROGRAM_PREFIX}dx12" PUBLIC )
target_link_libraries("${PROGRAM_PREFIX}dx12" FREEGLUT::FREEGLUT cuda17 d3d12 d3dcompiler dxgi
		#cudart_static kernel32 user32 gdi32 winspool comdlg32 advapi32 shell32 ole32 oleaut32 uuid odbc32 odbccp32.lib
		)
target_include_directories("${PROGRAM_PREFIX}dx12" INTERFACE "." PRIVATE "inc" "src" "data" "../derma" "R:/api/cuda/v11.1/tools/include")
target_link_directories("${PROGRAM_PREFIX}dx12" PRIVATE "R:/api/cuda/v11.1/tools/lib/x64")
set_target_properties("${PROGRAM_PREFIX}dx12" PROPERTIES WIN32_EXECUTABLE TRUE CXX_STANDARD 20 LANGUAGE CXX LINKER_LANGUAGE CXX)

if(MSVC)
	#add_definitions("/D_CRT_SECURE_NO_WARNINGS" "/D_CRT_SECURE_NO_DEPRECATE" "/D_CRT_NONSTDC_NO_DEPRECATE")
	target_compile_options("${PROGRAM_PREFIX}dx12" PUBLIC
		"/Zc:__cplusplus"
		"/Zc:wchar_t" "/Zc:inline" "/Zc:forScope" "/fp:precise"
		"/permissive-" "/Zc:preprocessor"
		"/W4" "/Od" "/arch:AVX2"
		"/EHsc"
		#"/GS" "/Gd" "/Gm-" "/FC" "/nologo" "/WX-" "/MDd"
		#"/experimental:module"
		#"/Zc:externConstexpr"
		)
	target_compile_definitions("${PROGRAM_PREFIX}dx12" PUBLIC
		"WIN32_LEAN_AND_MEAN" "NOMINMAX"
		)
else()
	target_compile_options("${PROGRAM_PREFIX}dx12" PUBLIC
		"-Wall" "-Wdeclaration-after-statement" "-Wredundant-decls")
endif()

