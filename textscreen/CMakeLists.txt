add_library(textscreen STATIC
									textscreen.h
									txt_defines.h
									txt_enums.h
									txt_conditional.h
									txt_checkbox.h
									txt_desktop.h
									txt_dropdown.h
			txt_fileselect.cpp		txt_fileselect.h
			txt_gui.cpp				txt_gui.h
									txt_inputbox.h
									txt_io.h
									txt_main.h
									txt_button.h
									txt_label.h
									txt_radiobutton.h
			txt_scrollpane.cpp		txt_scrollpane.h
									txt_separator.h
									txt_spinctrl.h
			txt_sdl.cpp				txt_sdl.h
									txt_strut.h
									txt_table.h
									txt_utf8.h
									txt_widget.h
									txt_window.h
									txt_window_action.h)

target_include_directories(textscreen INTERFACE "." PRIVATE "../src/")
target_link_libraries(textscreen SDL2::SDL2)
