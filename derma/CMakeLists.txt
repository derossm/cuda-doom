set(DERMA_FILES
								stdafx.h
								common.h
								os_native.h
								os_windows.h
								os_unix.h
								derma_strongtype.h
								derma_byte.h
								derma_enum.h
								derma_enum_free.h
								derma_string.h
								derma_vector.h
		test_byte.cpp			catch.hpp)

add_executable(catch2tests "${DERMA_FILES}")
set_target_properties(catch2tests PROPERTIES CXX_STANDARD 20 LANGUAGE CXX LINKER_LANGUAGE CXX)
target_include_directories(catch2tests PUBLIC "${COMMON_INCLUDE_DIRS}")
target_precompile_headers(catch2tests PUBLIC stdafx.h)

if(MSVC)
	target_compile_options(catch2tests PUBLIC "${WIN64_COMPILE_OPTIONS}")
	target_compile_definitions(catch2tests PUBLIC "${COMMON_DEFINITIONS}")
else()
	target_compile_definitions(catch2tests PUBLIC "SDL_MAIN_HANDLED")
endif()
