cmake_minimum_required(VERSION 2.8.9)

project(f2d.elf)


find_package(PkgConfig)
pkg_check_modules(VIDEO video)
pkg_check_modules(SDSLIB sds_lib)
pkg_check_modules(GLIB glib-2.0)
pkg_check_modules(DRM libdrm)

find_package(OpenCV REQUIRED)

set(SRCS main.c top/filter2d.c top/filter2d_cv.cpp)

set_source_files_properties(main.c PROPERTIES COMPILE_DEFINITIONS SAMPLE_FILTER2D)

add_executable(${PROJECT_NAME} ${SRCS})

target_include_directories(${PROJECT_NAME}
	PRIVATE ${VIDEO_INCLUDE_DIRS}
	PRIVATE ${GLIB_INCLUDE_DIRS}
	PRIVATE ${OpenCV_INCLUDE_DIRS} 
	PRIVATE	${CMAKE_CURRENT_SOURCE_DIR}/top 
	PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src
)

target_link_libraries(${PROJECT_NAME}
	${OpenCV_LIBS} 
	${VIDEO_LIBRARIES}
	${DRM_LIBRARIES} 
	${GLIB_LIBRARIES} 
	mediactl 
	v4l2subdev 
	pthread
)

if (SW_ONLY)
	target_link_libraries(${PROJECT_NAME} ${SDSLIB_LIBRARIES}) 
else ()
	add_definitions(-DWITH_SDSOC)
	add_library(f2d SHARED IMPORTED)
	set_target_properties(f2d PROPERTIES IMPORTED_LOCATION ${CMAKE_SOURCE_DIR}/libf2d.so)
	target_link_libraries(${PROJECT_NAME} f2d)
	install(TARGETS f2d LIBRARY DESTINATION lib)
endif()

install(TARGETS ${PROJECT_NAME} RUNTIME DESTINATION bin)
