cmake_minimum_required (VERSION 2.8.12)

project (video C)

# Cmake options
option (DEBUG_MODE "Enable debug output" OFF)
option (ENABLE_VCAP_UVC "Enable video source USB Video Class" ON)
option (ENABLE_VCAP_VIVID "Enable video source Vivid virtual video device" ON)
option (ENABLE_VCAP_FILE "Enable file source" ON)

# Source files
add_library (${PROJECT_NAME} STATIC
	src/drm_helper.c
	src/filter.c
	src/log_events.c
	src/m2m_sw_pipeline.c
	src/mediactl_helper.c
	src/qos.c
	src/s2m_pipeline.c
	src/v4l2_helper.c
	src/vcap_csi.c
	src/vcap_file.c
	src/vcap_tpg.c
	src/vcap_uvc.c
	src/vcap_vivid.c
	src/video.c
	src/video_src.c
)

find_package(PkgConfig)
pkg_check_modules(GLIB glib-2.0)
pkg_check_modules(DRM libdrm)

# Dependencies
find_path (V4L2SUBDEV_INCLUDE_DIRS mediactl/v4l2subdev.h)
find_path (MEDIACTL_INCLUDE_DIRS mediactl/mediactl.h)
find_package (Threads REQUIRED)
if(WITH_SDSOC) 
pkg_check_modules(SDSLIB sds_lib)
endif()
if(GST_MODE)
pkg_check_modules(GSTREMER gstreamer-1.0)
pkg_check_modules(gst gst_lib)
endif()

target_compile_definitions (${PROJECT_NAME}
	PUBLIC $<$<BOOL:${WITH_SDSOC}>:WITH_SDSOC>
	PRIVATE $<$<BOOL:${DEBUG_MODE}>:DEBUG_MODE>
	PRIVATE $<$<BOOL:${ENABLE_VCAP_UVC}>:ENABLE_VCAP_UVC>
	PRIVATE $<$<BOOL:${ENABLE_VCAP_VIVID}>:ENABLE_VCAP_VIVID>
	PRIVATE $<$<BOOL:${ENABLE_VCAP_FILE}>:ENABLE_VCAP_FILE>
	PRIVATE $<$<BOOL:${GST_MODE}>:GST_MODE>
)

target_include_directories (${PROJECT_NAME}
	PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include
	PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src/include
	PRIVATE ${DRM_INCLUDE_DIRS}
	PRIVATE ${MEDIACTL_INCLUDE_DIRS}
	PRIVATE ${V4L2SUBDEV_INCLUDE_DIRS}
	PUBLIC ${GLIB_INCLUDE_DIRS}
	PRIVATE $<$<BOOL:${WITH_SDSOC}>:${SDSLIB_INCLUDE_DIRS}>
	PRIVATE $<$<BOOL:${GST_MODE}>:${gst_INCLUDE_DIRS}>
	PUBLIC $<$<BOOL:${GST_MODE}>:${GSTREAMER_INCLUDE_DIRS}>
)

target_link_libraries (${PROJECT_NAME})

install (TARGETS ${PROJECT_NAME} ARCHIVE DESTINATION lib)
install (DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/include/ DESTINATION include/video)
