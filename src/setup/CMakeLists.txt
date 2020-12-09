add_library(setup STATIC
            compatibility.cpp     compatibility.h
            display.cpp           display.h
            joystick.cpp          joystick.h
            keyboard.cpp          keyboard.h
            mainmenu.cpp
            mode.cpp              mode.h
            mouse.cpp             mouse.h
            multiplayer.cpp       multiplayer.h
            sound.cpp             sound.h
            execute.cpp           execute.h
            txt_joyaxis.cpp       txt_joyaxis.h
            txt_joybinput.cpp     txt_joybinput.h
            txt_keyinput.cpp      txt_keyinput.h
            txt_mouseinput.cpp    txt_mouseinput.h)

target_include_directories(setup PRIVATE "../" "${CMAKE_CURRENT_BINARY_DIR}/../../")
target_link_libraries(setup textscreen SDL2::SDL2 SDL2::mixer)
