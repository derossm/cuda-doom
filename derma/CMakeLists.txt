add_library(derma STATIC
									cpp_headers.h
									c_headers.h
									namespace_forwards.h
									common.h
									d_windows.h
									d_unix.h
									deprecating_tools.h)

target_include_directories(derma INTERFACE "." PRIVATE "../src/")

set_target_properties(derma PROPERTIES LINKER_LANGUAGE CXX)
