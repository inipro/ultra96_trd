cmake_minimum_required (VERSION 2.8.12)

project (gst C)

# Source files
add_library (${PROJECT_NAME} STATIC
	src/vgst_lib.c
	src/vgst_pipeline.c
	src/vgst_sdxfilter2d.c
	src/vgst_utils.c
)

find_package(PkgConfig)
pkg_check_modules(GSTREAMER gstreamer-1.0)

target_compile_definitions (${PROJECT_NAME}
	PRIVATE GST_MODE
)

target_include_directories (${PROJECT_NAME}
	PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include
	PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}
	PRIVATE ${GSTREAMER_INCLUDE_DIRS}
)

target_link_libraries (${PROJECT_NAME})

install (TARGETS ${PROJECT_NAME} ARCHIVE DESTINATION lib)
install (DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/include/ DESTINATION include/vgst)
