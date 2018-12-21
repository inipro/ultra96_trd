cmake_minimum_required(VERSION 2.8.9)

find_package(OpenCV REQUIRED)

find_package(PkgConfig)
pkg_check_modules(GLIB glib-2.0)
pkg_check_modules(GSTREAMER gstreamer-1.0)
pkg_check_modules(GSTREAMER_VIDEO gstreamer-video-1.0)
include_directories(inc ${GLIB_INCLUDE_DIRS} ${GSTREAMER_INCLUDE_DIRS} ${OpenCV_INCLUDE_DIRS})
link_directories(lib)

add_subdirectory(filter2d)
add_subdirectory(optical_flow)
