cmake_minimum_required (VERSION 3.3)

project (video_qt2 C CXX)

# To automatically run MOC when building (Meta Object Compiler)
set(CMAKE_AUTOMOC ON)
# Find includes in corresponding build directories
set(CMAKE_INCLUDE_CURRENT_DIR ON)
# To automatically run RCC when building (Resource Compiler)
set(CMAKE_AUTORCC ON)

# Cmake options
option (ENABLE_DEMO_MODE "Enable demo mode" ON)

# Source files
add_executable(${PROJECT_NAME}
	src/CPUStat.cpp
	include/dirop.h
	src/dirop.cpp
	include/maincontroller.h
	src/maincontroller.cpp
	src/main.cpp
	src/video_cfg.cpp
	resourcefile.qrc
)

find_package(Qt5 REQUIRED Core Charts Widgets Quick Qml)

find_package(PkgConfig)
pkg_check_modules(VIDEO video)
pkg_check_modules(GLIB glib-2.0)
pkg_check_modules(DRM libdrm)

# Dependencies
find_path (V4L2SUBDEV_INCLUDE_DIRS mediactl/v4l2subdev.h)
find_path (MEDIACTL_INCLUDE_DIRS mediactl/mediactl.h)
find_package (Threads REQUIRED)
if(WITH_SDSOC)
pkg_check_modules(SDS sds_lib)
endif()
if(GST_MODE)
pkg_check_modules(GSTREAMER gstreamer-1.0)
pkg_check_modules(GSTREAMER_VIDEO gstreamer-video-1.0)
pkg_check_modules(gst gst)
endif()

target_compile_definitions (${PROJECT_NAME}
	PRIVATE $<$<BOOL:${GST_MODE}>:GST_MODE>
	PRIVATE $<$<BOOL:${ENABLE_DEMO_MODE}>:ENABLE_DEMO_MODE>
	PRIVATE $<$<BOOL:${SAMPLE_FILTER2D}>:SAMPLE_FILTER2D>
	PRIVATE $<$<BOOL:${SAMPLE_OPTICAL_FLOW}>:SAMPLE_OPTICAL_FLOW>
	PRIVATE $<$<BOOL:${PERFAPM_CLIENT}>:PERFAPM_CLIENT>
)


target_include_directories (${PROJECT_NAME}
	PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include
	PRIVATE ${VIDEO_INCLUDE_DIRS}
	PRIVATE ${GLIB_INCLUDE_DIRS}
	PRIVATE $<$<BOOL:${GST_MODE}>:${gst_INCLUDE_DIRS}>
	PRIVATE $<$<BOOL:${GST_MODE}>:${GSTREAMER_INCLUDE_DIRS}>
)

target_link_libraries (${PROJECT_NAME}
	Qt5::Core
	Qt5::Charts
	Qt5::Qml
	Qt5::Quick
	Qt5::Widgets
	${VIDEO_LIBRARIES}
	${DRM_LIBRARIES}
	${GLIB_LIBRARIES}
	mediactl
	v4l2subdev
	pthread
)
if(WITH_SDSOC)
target_link_libraries(${PROJECT_NAME} 
	${SDS_LIBRARIES}
)
endif()
if (GST_MODE)
target_link_libraries(${PROJECT_NAME} 
	${gst_LIBRARIES}
	${GSTREAMER_LIBRARIES}
	${GSTREAMER_VIDEO_LIBRARIES}
)
endif()

install (TARGETS ${PROJECT_NAME} RUNTIME DESTINATION bin)
