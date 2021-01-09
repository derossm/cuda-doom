add_library(textscreen STATIC
		textscreen.cpp				textscreen.h

									txt_common.h

									txt_sdl.h
									txt_main.h
									txt_utf8.h
									txt_io.h
									txt_gui.h
									txt_desktop.h

									txt_widget.h
									txt_table.h
									txt_window.h
									txt_button.h
									txt_checkbox.h
									txt_conditional.h
									txt_dropdown.h
									txt_fileselect.h
									txt_inputbox.h
									txt_label.h
									txt_radiobutton.h
									txt_scrollpane.h
									txt_separator.h
									txt_spinctrl.h
									txt_strut.h
									txt_window_action.h)

set_target_properties(textscreen PROPERTIES LINKER_LANGUAGE CXX)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

target_include_directories(textscreen INTERFACE "." PRIVATE "../src/" "${CMAKE_CURRENT_BINARY_DIR}/../")
target_link_libraries(textscreen SDL2::SDL2main SDL2::SDL2)
