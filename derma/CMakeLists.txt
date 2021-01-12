#add_library(derma STATIC
set(DERMA_FILES
									cpp_headers.h
									c_headers.h
									namespace_forwards.h
									common.h
									d_windows.h
									d_unix.h
									deprecating_tools.h
									bitmanipulator.h
									enumbitset.h
									bytetype.h
		test_bytetype.cpp			catch.h)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable("${PROGRAM_PREFIX}catch2_byte" ${DERMA_FILES})
set_target_properties("${PROGRAM_PREFIX}catch2_byte" PROPERTIES LINKER_LANGUAGE CXX)

target_include_directories("${PROGRAM_PREFIX}catch2_byte" INTERFACE "." PRIVATE "../src/" "../derma")
