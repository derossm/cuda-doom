
find_library(ZLIB_LIBRARY zlib)

#set(ZLIB_DIR "G:/dev/api/zlib")
#set(ZLIB_INCLUDE_DIR "${ZLIB_DIR}")
#set(ZLIB_LIBRARY_DIR "${ZLIB_DIR}")

add_library(zlib INTERFACE)
if(ZLIB_LIBRARY)
	target_link_libraries(zlib INTERFACE "${ZLIB_LIBRARY}")
endif()

#target_link_libraries(zlib PRIVATE ZLIB::ZLIB)
