
find_library(PNG_LIBRARY png)

#set(LIBPNG_DIR "G:/dev/api/libpng")
#set(LIBPNG_INCLUDE_DIR "${LIBPNG_DIR}")
#set(LIBPNG_LIBRARY_DIR "${LIBPNG_DIR}")

add_library(png INTERFACE)
if(PNG_LIBRARY)
	target_link_libraries(png INTERFACE "${PNG_LIBRARY}")
endif()

#target_link_libraries(libpng PRIVATE png)
