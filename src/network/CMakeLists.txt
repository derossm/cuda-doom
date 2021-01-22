
# Dedicated server (chocolate-server):
set(DEDSERV_FILES
								../disk_io/d_iwad.h
	../disk_io/deh_str.cpp		../disk_io/deh_str.h
	../d_mode.cpp				../d_mode.h
	../i_timer.cpp				../i_timer.h
	../m_config.cc
	../m_config.cpp				../m_config.h
								../z_zone.h

	net_common.cpp				net_common.h
	net_dedicated.cpp			net_dedicated.h
	net_io.cpp					net_io.h
	net_packet.cpp				net_packet.h
	net_sdl.cpp					net_sdl.h
	net_query.cpp				net_query.h
	net_server.cpp				net_server.h
	net_structrw.cpp			net_structrw.h

	d_dedicated.cpp)

if(MSVC)
	add_executable("${PROGRAM_PREFIX}server" WIN32 "${COMMON_SOURCE_FILES}" "${DEDSERV_FILES}")
else()
	add_executable("${PROGRAM_PREFIX}server" "${COMMON_SOURCE_FILES}" "${DEDSERV_FILES}")
endif()

set_target_properties("${PROGRAM_PREFIX}server" PROPERTIES CXX_STANDARD 20 LANGUAGE CXX LINKER_LANGUAGE CXX)
target_include_directories("${PROGRAM_PREFIX}server" PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/.." "${COMMON_INCLUDE_DIRS}")
target_link_libraries("${PROGRAM_PREFIX}server" SDL2::SDL2main SDL2::SDL2_net)
target_precompile_headers("${PROGRAM_PREFIX}server" PUBLIC "../../derma/stdafx.h")

if(MSVC)
	target_compile_options("${PROGRAM_PREFIX}server" PUBLIC "${WIN64_COMPILE_OPTIONS}")
	target_compile_definitions("${PROGRAM_PREFIX}server" PUBLIC "${COMMON_DEFINITIONS}")
else()
	target_compile_definitions("${PROGRAM_PREFIX}server" PUBLIC "SDL_MAIN_HANDLED")
endif()
