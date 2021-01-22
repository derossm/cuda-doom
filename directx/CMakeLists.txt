project("CUDA Doom DX12" VERSION 0.0.1 LANGUAGES CUDA CXX)

if(WIN64)
set(DX12_FILES
									d3dx12.h
		Win32Application.cpp		Win32Application.h
									DXSampleHelper.h
									ShaderStructs.h
		DX12CudaSample.cpp			DX12CudaSample.h
		simpleD3D12.cpp				simpleD3D12.h
									resource.h
									dynlink_d3d10.h
									dynlink_d3d11.h
									exception.h
									helper_cuda.h
									helper_cuda_drvapi.h
									helper_cusolver.h
									helper_functions.h
									helper_gl.h
									helper_image.h
									helper_math.h
		helper_multiprocess.cpp		helper_multiprocess.h
									helper_string.h
									helper_timer.h
		multithreading.cpp			multithreading.h
									nvMath.h
									nvMatrix.h
									nvQuaternion.h
									nvrtc_helper.h
									nvShaderUtils.h
									nvVector.h
									param.h
									paramgl.h
		rendercheck_d3d9.cpp		rendercheck_d3d9.h
		rendercheck_d3d10.cpp		rendercheck_d3d10.h
		rendercheck_d3d11.cpp		rendercheck_d3d11.h
									rendercheck_gl.h
									rendercheck_gles.h
		Main.cpp
		#shaders.hlsl
)

add_library(cuda17 STATIC sinewave_cuda.cu ShaderStructs.h helper_string.h helper_cuda.h)
set_target_properties(cuda17 PROPERTIES CUDA_ARCHITECTURES 61 CXX_STANDARD 17)
target_include_directories(cuda17 PUBLIC "${COMMON_INCLUDE_DIRS}")
target_link_libraries(cuda17 d3d12 d3dcompiler dxgi odbc32 odbccp32)
target_precompile_headers(cuda17 PUBLIC "../derma/stdafx.h")

if(MSVC)
	target_compile_definitions(cuda17 PUBLIC "${COMMON_DEFINITIONS}")
endif()

add_executable("${PROGRAM_PREFIX}dx12" WIN32 ${DX12_FILES})
set_target_properties("${PROGRAM_PREFIX}dx12" PROPERTIES WIN32_EXECUTABLE TRUE CXX_STANDARD 20 LANGUAGE CXX LINKER_LANGUAGE CXX)
target_include_directories("${PROGRAM_PREFIX}dx12" PUBLIC "${COMMON_INCLUDE_DIRS}")
target_link_libraries("${PROGRAM_PREFIX}dx12" cuda17 d3d12 d3dcompiler dxgi odbc32 odbccp32)
target_precompile_headers("${PROGRAM_PREFIX}dx12" PUBLIC "../derma/stdafx.h")

if(MSVC)
	target_compile_options("${PROGRAM_PREFIX}dx12" PUBLIC "${WIN64_COMPILE_OPTIONS}")
	target_compile_definitions("${PROGRAM_PREFIX}dx12" PUBLIC "${COMMON_DEFINITIONS}")
else()
	target_compile_definitions("${PROGRAM_PREFIX}dx12" PUBLIC "SDL_MAIN_HANDLED")
endif()

#find_package(Boost REQUIRED [COMPONENTS <libs>...])
#target_link_libraries(main PRIVATE Boost::boost Boost::<lib1> Boost::<lib2> ...)
endif()
