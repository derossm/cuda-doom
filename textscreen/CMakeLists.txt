add_library(textscreen STATIC
									textscreen.h
			txt_conditional.cpp		txt_conditional.h
			txt_checkbox.cpp		txt_checkbox.h
			txt_desktop.cpp			txt_desktop.h
			txt_dropdown.cpp		txt_dropdown.h
			txt_fileselect.cpp		txt_fileselect.h
			txt_gui.cpp				txt_gui.h
			txt_inputbox.cpp		txt_inputbox.h
			txt_io.cpp				txt_io.h
									txt_main.h
			txt_button.cpp			txt_button.h
			txt_label.cpp			txt_label.h
			txt_radiobutton.cpp		txt_radiobutton.h
			txt_scrollpane.cpp		txt_scrollpane.h
			txt_separator.cpp		txt_separator.h
			txt_spinctrl.cpp		txt_spinctrl.h
			txt_sdl.cpp				txt_sdl.h
			txt_strut.cpp			txt_strut.h
			txt_table.cpp			txt_table.h
			txt_utf8.cpp			txt_utf8.h
			txt_widget.cpp			txt_widget.h
			txt_window.cpp			txt_window.h
			txt_window_action.cpp	txt_window_action.h)

target_include_directories(textscreen INTERFACE "." PRIVATE "../src/")
target_link_libraries(textscreen SDL2::SDL2)
