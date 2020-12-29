foreach(SUBDIR doom heretic hexen strife setup)
	add_subdirectory("${SUBDIR}")
endforeach()

# Common source files used by absolutely everything:

set(COMMON_SOURCE_FILES
						crispy.h
	i_main.cpp
	i_system.cpp		i_system.h
	m_argv.cpp			m_argv.h
	m_misc.cpp			m_misc.h)

# Dedicated server (chocolate-server):

set(DEDSERV_FILES
	d_dedicated.cpp
	d_iwad.cpp			d_iwad.h
	d_mode.cpp			d_mode.h
	deh_str.cpp			deh_str.h
	i_timer.cpp			i_timer.h
	m_config.cpp		m_config.h
	net_common.cpp		net_common.h
	net_dedicated.cpp	net_dedicated.h
	net_io.cpp			net_io.h
	net_packet.cpp		net_packet.h
	net_sdl.cpp			net_sdl.h
	net_query.cpp		net_query.h
	net_server.cpp		net_server.h
	net_structrw.cpp	net_structrw.h
	z_native.cpp		z_zone.h)

add_executable("${PROGRAM_PREFIX}server" WIN32 ${COMMON_SOURCE_FILES} ${DEDSERV_FILES})
target_include_directories("${PROGRAM_PREFIX}server" PRIVATE "${CMAKE_CURRENT_BINARY_DIR}/../")
target_link_libraries("${PROGRAM_PREFIX}server" SDL2::SDL2main SDL2::net)

# Source files used by the game binaries (chocolate-doom, etc.)

set(GAME_SOURCE_FILES
	aes_prng.cpp		aes_prng.h
						aes_prng_tables.h
	d_event.cpp			d_event.h
						doomkeys.h
						doomtype.h
	d_iwad.cpp			d_iwad.h
	d_loop.cpp			d_loop.h
	d_mode.cpp			d_mode.h
						d_ticcmd.h
	deh_str.cpp			deh_str.h
	gusconf.cpp			gusconf.h
	i_cdmus.cpp			i_cdmus.h
	i_endoom.cpp		i_endoom.h
	i_glob.cpp			i_glob.h
	i_input.cpp			i_input.h
	i_joystick.cpp		i_joystick.h
						i_swap.h
	i_midipipe.cpp		i_midipipe.h
	i_musicpack.cpp
	i_oplmusic.cpp
	i_pcsound.cpp
	i_sdlmusic.cpp
	i_sdlsound.cpp
	i_sound.cpp			i_sound.h
	i_timer.cpp			i_timer.h
	i_video.cpp			i_video.h
	i_videohr.cpp		i_videohr.h
	midifile.cpp		midifile.h
	mus2mid.cpp			mus2mid.h
	m_bbox.cpp			m_bbox.h
	m_cheat.cpp			m_cheat.h
	m_config.cpp		m_config.h
	m_controls.cpp		m_controls.h
	m_fixed.cpp			m_fixed.h
	net_client.cpp		net_client.h
	net_common.cpp		net_common.h
	net_dedicated.cpp	net_dedicated.h
						net_defs.h
	net_gui.cpp			net_gui.h
	net_io.cpp			net_io.h
	net_loop.cpp		net_loop.h
	net_packet.cpp		net_packet.h
	net_petname.cpp		net_petname.h
	net_query.cpp		net_query.h
	net_sdl.cpp			net_sdl.h
	net_server.cpp		net_server.h
	net_structrw.cpp	net_structrw.h
	sha1.cpp			sha1.h
	memio.cpp			memio.h
	tables.cpp			tables.h
	v_diskicon.cpp		v_diskicon.h
	v_video.cpp			v_video.h
						v_patch.h
	v_trans.cpp			v_trans.h
	w_checksum.cpp		w_checksum.h
	w_main.cpp			w_main.h
	w_wad.cpp			w_wad.h
	w_file.cpp			w_file.h
	w_file_stdc.cpp
	w_file_posix.cpp
	w_file_win32.cpp
	w_merge.cpp			w_merge.h
	z_zone.cpp			z_zone.h)

set(GAME_INCLUDE_DIRS "${CMAKE_CURRENT_BINARY_DIR}/../")

if(MSVC)
	list(APPEND GAME_SOURCE_FILES "../win32/win_opendir.cpp" "../win32/win_opendir.h")
	list(APPEND GAME_INCLUDE_DIRS "${PROJECT_SOURCE_DIR}/win32/")
endif()

set(DEHACKED_SOURCE_FILES
						deh_defs.h
	deh_io.cpp			deh_io.h
	deh_main.cpp		deh_main.h
	deh_mapping.cpp		deh_mapping.h
	deh_text.cpp)

# Some games support dehacked patches, some don't:

set(SOURCE_FILES ${COMMON_SOURCE_FILES} ${GAME_SOURCE_FILES})
set(SOURCE_FILES_WITH_DEH ${SOURCE_FILES} ${DEHACKED_SOURCE_FILES})

set(EXTRA_LIBS SDL2::SDL2main SDL2::SDL2 SDL2::mixer SDL2::net textscreen pcsound opl)
if(SAMPLERATE_FOUND)
	list(APPEND EXTRA_LIBS samplerate::samplerate)
endif()
if(PNG_FOUND)
	list(APPEND EXTRA_LIBS PNG::PNG)
endif()

if(WIN32)
	add_executable("${PROGRAM_PREFIX}doom" WIN32 ${SOURCE_FILES_WITH_DEH} "${CMAKE_CURRENT_BINARY_DIR}/resource.rc")
else()
	add_executable("${PROGRAM_PREFIX}doom" ${SOURCE_FILES_WITH_DEH})
endif()

target_include_directories("${PROGRAM_PREFIX}doom" PRIVATE ${GAME_INCLUDE_DIRS})
target_link_libraries("${PROGRAM_PREFIX}doom" doom ${EXTRA_LIBS})

if(MSVC)
	set_target_properties("${PROGRAM_PREFIX}doom" PROPERTIES LINK_FLAGS "/MANIFEST:NO")
endif()

if(WIN32)
	add_executable("${PROGRAM_PREFIX}heretic" WIN32 ${SOURCE_FILES_WITH_DEH} "${CMAKE_CURRENT_BINARY_DIR}/resource.rc")
else()
	add_executable("${PROGRAM_PREFIX}heretic" ${SOURCE_FILES_WITH_DEH})
endif()

target_include_directories("${PROGRAM_PREFIX}heretic" PRIVATE ${GAME_INCLUDE_DIRS})
target_link_libraries("${PROGRAM_PREFIX}heretic" heretic ${EXTRA_LIBS})

if(MSVC)
	set_target_properties("${PROGRAM_PREFIX}heretic" PROPERTIES LINK_FLAGS "/MANIFEST:NO")
endif()

if(WIN32)
	add_executable("${PROGRAM_PREFIX}hexen" WIN32 ${SOURCE_FILES} "${CMAKE_CURRENT_BINARY_DIR}/resource.rc")
else()
	add_executable("${PROGRAM_PREFIX}hexen" ${SOURCE_FILES})
endif()

target_include_directories("${PROGRAM_PREFIX}hexen" PRIVATE ${GAME_INCLUDE_DIRS})
target_link_libraries("${PROGRAM_PREFIX}hexen" hexen ${EXTRA_LIBS})

if(MSVC)
	set_target_properties("${PROGRAM_PREFIX}hexen" PROPERTIES LINK_FLAGS "/MANIFEST:NO")
endif()

if(WIN32)
	add_executable("${PROGRAM_PREFIX}strife" WIN32 ${SOURCE_FILES_WITH_DEH} "${CMAKE_CURRENT_BINARY_DIR}/resource.rc")
else()
	add_executable("${PROGRAM_PREFIX}strife" ${SOURCE_FILES_WITH_DEH})
endif()

target_include_directories("${PROGRAM_PREFIX}strife" PRIVATE ${GAME_INCLUDE_DIRS})
target_link_libraries("${PROGRAM_PREFIX}strife" strife ${EXTRA_LIBS})

if(MSVC)
	set_target_properties("${PROGRAM_PREFIX}strife" PROPERTIES LINK_FLAGS "/MANIFEST:NO")
endif()

# Source files needed for chocolate-setup:

set(SETUP_FILES
	deh_str.cpp			deh_str.h
	d_mode.cpp			d_mode.h
	d_iwad.cpp			d_iwad.h
	i_timer.cpp			i_timer.h
	m_config.cpp		m_config.h
	m_controls.cpp		m_controls.h
	net_io.cpp			net_io.h
	net_packet.cpp		net_packet.h
	net_petname.cpp		net_petname.h
	net_sdl.cpp			net_sdl.h
	net_query.cpp		net_query.h
	net_structrw.cpp	net_structrw.h
	z_native.cpp		z_zone.h)

if(WIN32)
	add_executable("${PROGRAM_PREFIX}setup" WIN32 ${SETUP_FILES} ${COMMON_SOURCE_FILES} "${CMAKE_CURRENT_BINARY_DIR}/setup-res.rc")
else()
	add_executable("${PROGRAM_PREFIX}setup" ${SETUP_FILES} ${COMMON_SOURCE_FILES})
endif()

target_include_directories("${PROGRAM_PREFIX}setup" PRIVATE "${CMAKE_CURRENT_BINARY_DIR}/../")
target_link_libraries("${PROGRAM_PREFIX}setup" SDL2::SDL2main SDL2::SDL2 SDL2::mixer SDL2::net setup textscreen)

if(MSVC)
	set_target_properties("${PROGRAM_PREFIX}setup" PROPERTIES LINK_FLAGS "/MANIFEST:NO")
endif()

add_executable(midiread midifile.cpp z_native.cpp i_system.cpp m_argv.cpp m_misc.cpp d_iwad.cpp deh_str.cpp m_config.cpp)
target_compile_definitions(midiread PRIVATE "-DTEST")
target_include_directories(midiread PRIVATE "${CMAKE_CURRENT_BINARY_DIR}/../")
target_link_libraries(midiread SDL2::SDL2main SDL2::SDL2)

add_executable(mus2mid mus2mid.cpp memio.cpp z_native.cpp i_system.cpp m_argv.cpp m_misc.cpp d_iwad.cpp deh_str.cpp m_config.cpp)
target_compile_definitions(mus2mid PRIVATE "-DSTANDALONE")
target_include_directories(mus2mid PRIVATE "${CMAKE_CURRENT_BINARY_DIR}/../")
target_link_libraries(mus2mid SDL2::SDL2main SDL2::SDL2)
