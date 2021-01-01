add_library(textscreen STATIC
									textscreen.ixx
									txt_defines.h
									txt_enums.ixx
									txt_utf8.ixx
			txt_sdl.cpp				txt_sdl.ixx
									txt_main.ixx
									txt_io.ixx
			txt_gui.cpp				txt_gui.ixx
									txt_desktop.ixx
									txt_widget.ixx
									txt_table.ixx
									txt_window.ixx
									txt_strut.ixx
									txt_separator.ixx
									txt_window_action.ixx
									txt_label.ixx
									txt_inputbox.ixx
									txt_button.ixx
									txt_conditional.ixx
									txt_checkbox.ixx
									txt_dropdown.ixx
									txt_radiobutton.ixx
			txt_scrollpane.cpp		txt_scrollpane.ixx
									txt_spinctrl.ixx
			txt_fileselect.cpp		txt_fileselect.ixx)

target_include_directories(textscreen INTERFACE "." PRIVATE "../src/")
target_link_libraries(textscreen SDL2::SDL2)
