foreach(SUBDIR doom setup algorithms disk_io input network)
	add_subdirectory("${SUBDIR}")
endforeach()

set(NETWORK_SOURCE
network/net_client.cpp		network/net_client.h
network/net_common.cpp		network/net_common.h
network/net_dedicated.cpp	network/net_dedicated.h
							network/net_defs.h
network/net_gui.cpp			network/net_gui.h
network/net_io.cpp			network/net_io.h
network/net_loop.cpp		network/net_loop.h
network/net_packet.cpp		network/net_packet.h
network/net_petname.cpp		network/net_petname.h
network/net_query.cpp		network/net_query.h
network/net_sdl.cpp			network/net_sdl.h
network/net_server.cpp		network/net_server.h
network/net_structrw.cpp	network/net_structrw.h
)

set(ALGORITHMS_SOURCE
	algorithms/aes_prng.cpp		algorithms/aes_prng.h
								algorithms/aes_prng_tables.cc
								algorithms/sha1.h
)

set(INPUT_SOURCE
input/i_input.cpp			input/i_input.h
input/i_joystick.cpp		input/i_joystick.h
input/m_controls.cpp		input/m_controls.h
							input/d_ticcmd.h
)

set(DISK_IO_SOURCE
	disk_io/v_diskicon.cpp		disk_io/v_diskicon.h
								disk_io/d_iwad.h
	disk_io/w_checksum.cpp		disk_io/w_checksum.h
	disk_io/w_main.cpp			disk_io/w_main.h
	disk_io/w_wad.cpp			disk_io/w_wad.h
	disk_io/w_file.cpp			disk_io/w_file.h
	disk_io/w_file_stdc.cpp
	disk_io/w_file_posix.cpp
	disk_io/w_file_win32.cpp
	disk_io/w_merge.cpp			disk_io/w_merge.h
	disk_io/i_glob.cpp			disk_io/i_glob.h
)

set(GAME_SOURCE
								m_bbox.h
	m_cheat.cpp					m_cheat.h
	m_fixed.cpp					m_fixed.h
	m_config.cpp				m_config.h
	m_config.cc

	d_event.cpp					d_event.h
	d_loop.cpp					d_loop.h
	d_mode.cpp					d_mode.h

	v_video.cpp					v_video.h
	v_trans.cpp					v_trans.h
								v_patch.h

								i_cdmus.h
								i_endoom.h
								i_swap.h
	i_sound.cpp					i_sound.h
	i_timer.cpp					i_timer.h
	i_video.cpp					i_video.h
	i_videohr.cpp				i_videohr.h

	i_musicpack.cpp

	gusconf.cpp					gusconf.h
								z_zone.h

	i_midipipe.cpp				i_midipipe.h
	i_pcsound.cpp
	i_sdlmusic.cpp
	i_sdlsound.cpp
	i_oplmusic.cpp
	i_oplmusic.cc

	midifile.cpp				midifile.h
	mus2mid.cpp					mus2mid.h

	tables.cc					tables.h

	memio.cpp					memio.h
)

set(GAME_INCLUDE_DIRS	"${CMAKE_CURRENT_BINARY_DIR}/.."
						"${CMAKE_CURRENT_SOURCE_DIR}/algorithms"
						"${CMAKE_CURRENT_SOURCE_DIR}/disk_io"
						"${CMAKE_CURRENT_SOURCE_DIR}/input"
						"${CMAKE_CURRENT_SOURCE_DIR}/network")

#if(MSVC)
	#list(APPEND GAME_SOURCE_FILES "../win32/win_opendir.cpp" "../derma/os_native.h" "../derma/d_windows.h" "../win32/win_opendir.h")
	#list(APPEND GAME_INCLUDE_DIRS "${PROJECT_SOURCE_DIR}/win32/")
#endif()

set(DEHACKED_SOURCE_FILES
								disk_io/deh_defs.h
	disk_io/deh_io.cpp			disk_io/deh_io.h
	disk_io/deh_main.cpp		disk_io/deh_main.h
	disk_io/deh_mapping.cpp		disk_io/deh_mapping.h
	disk_io/deh_text.cpp

	disk_io/deh_str.cpp			disk_io/deh_str.h
	disk_io/deh_ammo.cpp
	disk_io/deh_bexincl.cpp
	disk_io/deh_bexpars.cpp		disk_io/deh_bexpars.h
	disk_io/deh_bexptr.cpp
	disk_io/deh_bexstr.cpp
	disk_io/deh_cheat.cpp
	disk_io/deh_doom.cpp
	disk_io/deh_frame.cpp
	disk_io/deh_misc.cpp		disk_io/deh_misc.h
	disk_io/deh_ptr.cpp
	disk_io/deh_sound.cpp
	disk_io/deh_thing.cpp
	disk_io/deh_weapon.cpp

)

# Common source files used by absolutely everything:
set(COMMON_SOURCE_FILES
	i_main.cpp
	i_system.cpp		i_system.h
	m_argv.cpp			m_argv.h
						m_misc.h
						crispy.h)

set(GAME_SOURCE_FILES "${ALGORITHMS_SOURCE}" "${DISK_IO_SOURCE}" "${INPUT_SOURCE}" "${NETWORK_SOURCE}" "${GAME_SOURCE}")

set(SOURCE_FILES_WITH_DEH "${COMMON_SOURCE_FILES}" "${GAME_SOURCE_FILES}" "${DEHACKED_SOURCE_FILES}")

set(EXTRA_LIBS SDL2::SDL2main SDL2::SDL2 SDL2::SDL2_mixer SDL2::SDL2_net textscreen pcsound opl)

if(SAMPLERATE_FOUND)
	list(APPEND EXTRA_LIBS samplerate::samplerate)
endif()

if(PNG_FOUND)
	list(APPEND EXTRA_LIBS PNG::PNG)
endif()

if(WIN32)
	add_executable("${PROGRAM_PREFIX}doom" WIN32 "${SOURCE_FILES_WITH_DEH}" "${CMAKE_CURRENT_BINARY_DIR}/resource.rc")
else()
	add_executable("${PROGRAM_PREFIX}doom" "${SOURCE_FILES_WITH_DEH}")
endif()

set_target_properties("${PROGRAM_PREFIX}doom" PROPERTIES CXX_STANDARD 20 LANGUAGE CXX LINKER_LANGUAGE CXX)
target_include_directories("${PROGRAM_PREFIX}doom" PUBLIC "${GAME_INCLUDE_DIRS}" "${COMMON_INCLUDE_DIRS}")
target_link_libraries("${PROGRAM_PREFIX}doom" doom "${EXTRA_LIBS}")
target_precompile_headers("${PROGRAM_PREFIX}doom" PUBLIC "../derma/stdafx.h")

if(MSVC)
	target_compile_options("${PROGRAM_PREFIX}doom" PUBLIC "${WIN64_COMPILE_OPTIONS}")
	target_compile_definitions("${PROGRAM_PREFIX}doom" PUBLIC "${COMMON_DEFINITIONS}")
else()
	target_compile_definitions("${PROGRAM_PREFIX}doom" PUBLIC "SDL_MAIN_HANDLED")
endif()

# Source files needed for chocolate-setup:
set(SETUP_FILES
	disk_io/deh_str.cpp			disk_io/deh_str.h
								disk_io/d_iwad.h
	network/net_io.cpp			network/net_io.h
	network/net_packet.cpp		network/net_packet.h
	network/net_petname.cpp		network/net_petname.h
	network/net_sdl.cpp			network/net_sdl.h
	network/net_query.cpp		network/net_query.h
	network/net_structrw.cpp	network/net_structrw.h
	d_mode.cpp					d_mode.h
	i_timer.cpp					i_timer.h
	m_config.cpp				m_config.h
	m_config.cc
	m_controls.cpp				m_controls.h
								z_zone.h)

if(WIN32)
	add_executable("${PROGRAM_PREFIX}setup" WIN32 "${SETUP_FILES}" "${COMMON_SOURCE_FILES}" "${CMAKE_CURRENT_BINARY_DIR}/setup-res.rc")
else()
	add_executable("${PROGRAM_PREFIX}setup" "${SETUP_FILES}" "${COMMON_SOURCE_FILES}")
endif()

set_target_properties("${PROGRAM_PREFIX}setup" PROPERTIES CXX_STANDARD 20 LANGUAGE CXX LINKER_LANGUAGE CXX)
target_include_directories("${PROGRAM_PREFIX}setup" PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/.." "${COMMON_INCLUDE_DIRS}")
target_link_libraries("${PROGRAM_PREFIX}setup" SDL2::SDL2main SDL2::SDL2 SDL2::SDL2_mixer SDL2::SDL2_net setup textscreen)
target_precompile_headers("${PROGRAM_PREFIX}setup" PUBLIC "../derma/stdafx.h")

if(MSVC)
	target_compile_options("${PROGRAM_PREFIX}setup" PUBLIC "${WIN64_COMPILE_OPTIONS}")
	target_compile_definitions("${PROGRAM_PREFIX}setup" PUBLIC "${COMMON_DEFINITIONS}")
else()
	target_compile_definitions("${PROGRAM_PREFIX}setup" PUBLIC "SDL_MAIN_HANDLED")
endif()

if(WIN32)
	add_executable(midiread WIN32 midifile.cpp z_native.cpp i_system.cpp m_argv.cpp disk_io/deh_str.cpp m_config.cpp)
else()
	add_executable(midiread midifile.cpp z_native.cpp i_system.cpp m_argv.cpp disk_io/deh_str.cpp m_config.cpp)
endif()

set_target_properties(midiread PROPERTIES CXX_STANDARD 20 LANGUAGE CXX LINKER_LANGUAGE CXX)
target_include_directories(midiread PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/.." "${COMMON_INCLUDE_DIRS}")
target_link_libraries(midiread SDL2::SDL2main SDL2::SDL2)
target_precompile_headers(midiread PUBLIC "../derma/stdafx.h")

if(MSVC)
	target_compile_options(midiread PUBLIC "${WIN64_COMPILE_OPTIONS}")
	target_compile_definitions(midiread PUBLIC "-DTEST" "${COMMON_DEFINITIONS}")
else()
	target_compile_definitions(midiread PUBLIC "-DTEST" "SDL_MAIN_HANDLED")
endif()

if(WIN32)
	add_executable(mus2mid WIN32 mus2mid.cpp memio.cpp z_native.cpp i_system.cpp m_argv.cpp disk_io/deh_str.cpp m_config.cpp)
else()
	add_executable(mus2mid mus2mid.cpp memio.cpp z_native.cpp i_system.cpp m_argv.cpp disk_io/deh_str.cpp m_config.cpp)
endif()

set_target_properties(mus2mid PROPERTIES CXX_STANDARD 20 LANGUAGE CXX LINKER_LANGUAGE CXX)
target_include_directories(mus2mid PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/.." "${COMMON_INCLUDE_DIRS}")
target_link_libraries(mus2mid SDL2::SDL2main SDL2::SDL2)
target_precompile_headers(mus2mid PUBLIC "../derma/stdafx.h")

if(MSVC)
	target_compile_options(mus2mid PUBLIC "${WIN64_COMPILE_OPTIONS}")
	target_compile_definitions(mus2mid PUBLIC "-DSTANDALONE" "${COMMON_DEFINITIONS}")
else()
	target_compile_definitions(mus2mid PUBLIC "-DSTANDALONE" "SDL_MAIN_HANDLED")
endif()
