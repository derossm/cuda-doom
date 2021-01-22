add_library(textscreen STATIC
		textscreen.cpp				textscreen.h
									txt_common.h
									txt_sdl.h
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

set_target_properties(textscreen PROPERTIES CXX_STANDARD 20 LANGUAGE CXX LINKER_LANGUAGE CXX)
target_include_directories(textscreen PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}" "${VCPKG_INCLUDE_DIR}" "${CUDA_INCLUDE_DIR}")
target_link_libraries(textscreen SDL2::SDL2main SDL2::SDL2)
target_precompile_headers(textscreen PUBLIC "../derma/stdafx.h")

if(MSVC)
	target_compile_options(textscreen PUBLIC "${WIN64_COMPILE_OPTIONS}")
	target_compile_definitions(textscreen PUBLIC "${COMMON_DEFINITIONS}")
else()
	target_compile_definitions(textscreen PUBLIC "SDL_MAIN_HANDLED")
endif()
