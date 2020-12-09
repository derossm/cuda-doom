add_library(hexen STATIC
            a_action.cpp
                                am_data.h
            am_map.cpp            am_map.h
            ct_chat.cpp
                                ct_chat.h
            d_net.cpp
            f_finale.cpp
            g_game.cpp
                                h2def.h
            h2_main.cpp
            info.cpp              info.h
            in_lude.cpp
            m_random.cpp          m_random.h
            mn_menu.cpp
            p_acs.cpp
            p_anim.cpp
            p_ceilng.cpp
            p_doors.cpp
            p_enemy.cpp
            p_floor.cpp
            p_inter.cpp
            p_lights.cpp
                                p_local.h
            p_map.cpp
            p_maputl.cpp
            p_mobj.cpp
            po_man.cpp
            p_plats.cpp
            p_pspr.cpp
            p_setup.cpp
            p_sight.cpp
            p_spec.cpp           p_spec.h
            p_switch.cpp
            p_telept.cpp
            p_things.cpp
            p_tick.cpp
            p_user.cpp
            r_bsp.cpp
            r_data.cpp
            r_draw.cpp
                                r_local.h
            r_main.cpp
            r_plane.cpp
            r_segs.cpp
            r_things.cpp
            s_sound.cpp           s_sound.h
            sb_bar.cpp
            sc_man.cpp
            sn_sonix.cpp
            sounds.cpp            sounds.h
            st_start.cpp          st_start.h
            sv_save.cpp
                                textdefs.h
                                xddefs.h)

target_include_directories(hexen PRIVATE "../" "${CMAKE_CURRENT_BINARY_DIR}/../../")
target_link_libraries(hexen SDL2::SDL2 SDL2::mixer SDL2::net)
