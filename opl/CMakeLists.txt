add_library(opl STATIC
							opl.h
							opl3.h
							opl_queue.h
							opl_timer.h
							opl_sdl.h
							ioperm_sys.h
			opl_win32.cpp
			opl_linux.cpp
			opl_obsd.cpp)

set_target_properties(opl PROPERTIES LINKER_LANGUAGE CXX)
target_include_directories(opl INTERFACE "." PRIVATE "${CMAKE_CURRENT_BINARY_DIR}/../")
target_link_libraries(opl SDL2::mixer)
