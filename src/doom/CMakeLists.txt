add_library(doom STATIC
			../disk_io/d_pwad.cpp	../disk_io/d_pwad.h
			am_map.cpp				am_map.h
									d_english.h
									d_items.h
									d_main.h
			doom.cpp				doom.h
									doom_engine.h
									doom_includes.h
			d_net.cpp
									doomdata.h
									doomdef.h
			doomstat.cpp			doomstat.h
									d_player.h
			dstrings.cpp			dstrings.h
									d_think.h
			f_finale.cpp			f_finale.h
			f_wipe.cpp				f_wipe.h
			g_game.cpp				g_game.h
			hu_lib.cpp				hu_lib.h
			hu_stuff.cpp			hu_stuff.h
									info.h
									m_background.h
			m_crispy.cpp			m_crispy.h
			m_menu.cpp				m_menu.h
			m_random.cpp			m_random.h
			p_bexptr.cpp
			p_blockmap.cpp
			p_ceilng.cpp
			p_doors.cpp
			p_enemy.cpp
			p_extnodes.cpp			p_extnodes.h
			p_extsaveg.cpp			p_extsaveg.h
			p_floor.cpp
			p_inter.cpp				p_inter.h
			p_lights.cpp
									p_local.h
			p_map.cpp
			p_maputl.cpp
			p_mobj.cpp				p_mobj.h
			p_plats.cpp
			p_pspr.cpp				p_pspr.h
			p_saveg.cpp				p_saveg.h
			p_setup.cpp				p_setup.h
			p_sight.cpp
			p_spec.cpp				p_spec.h
			p_switch.cpp
			p_telept.cpp
			p_tick.cpp				p_tick.h
			p_user.cpp
			r_bmaps.cpp				r_bmaps.h
			r_bsp.cpp				r_bsp.h
			r_data.cpp				r_data.h
									r_defs.h
			r_draw.cpp				r_draw.h
									r_local.h
			r_main.cpp				r_main.h
			r_plane.cpp				r_plane.h
			r_segs.cpp				r_segs.h
			r_sky.cpp				r_sky.h
									r_state.h
			r_swirl.cpp				r_swirl.h
			r_things.cpp			r_things.h
			s_musinfo.cpp			s_musinfo.h
			s_sound.cpp				s_sound.h
			sounds.cpp				sounds.h
			statdump.cpp			statdump.h
			st_lib.cpp				st_lib.h
			st_stuff.cpp			st_stuff.h
			wi_stuff.cpp			wi_stuff.h)

set_target_properties(doom PROPERTIES CXX_STANDARD 20 LANGUAGE CXX LINKER_LANGUAGE CXX)
target_include_directories(doom PUBLIC "${COMMON_INCLUDE_DIRS}" "${CMAKE_CURRENT_SOURCE_DIR}/../disk_io" "${CMAKE_CURRENT_SOURCE_DIR}/../input" "${CMAKE_CURRENT_SOURCE_DIR}/../network" "${CMAKE_CURRENT_SOURCE_DIR}/../algorithms")
target_link_libraries(doom SDL2::SDL2 SDL2::SDL2_mixer SDL2::SDL2_net)
target_precompile_headers(doom PUBLIC "../../derma/stdafx.h")

if(MSVC)
	target_compile_options(doom PUBLIC "${WIN64_COMPILE_OPTIONS}")
	target_compile_definitions(doom PUBLIC "${COMMON_DEFINITIONS}")
else()
	target_compile_definitions(doom PUBLIC "SDL_MAIN_HANDLED")
endif()
