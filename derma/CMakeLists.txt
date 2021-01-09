add_library(derma STATIC
									cpp_headers.h
									c_headers.h
									namespace_forwards.h
									common.h
									d_windows.h
									d_unix.h
									deprecating_tools.h
									bitmanipulator.h
									enumbitset.h)

set_target_properties(derma PROPERTIES LINKER_LANGUAGE CXX)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

target_include_directories(derma INTERFACE "." PRIVATE "../src/")
