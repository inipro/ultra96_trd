cmake_minimum_required (VERSION 2.8.12)

project(gstsdxfilter2d CXX)

find_package(OpenCV REQUIRED)

find_package(PkgConfig)
pkg_check_modules(GST_BASE gstreamer-base-1.0)

include_directories(${CMAKE_CURRENT_SOURCE_DIR} ${OpenCV_INCLUDE_DIRS} ${GST_BASE_INCLUDE_DIRS})

set(SRCS filter2d_cv.cpp gstsdxfilter2d.cpp)

add_library(gstsdxfilter2d SHARED ${SRCS})

#target_link_libraries(gstsdxfilter2d ${OpenCV_LIBS} ${GST_VIDEO_LIBRARIES} gstsdxbase-1.0 gstsdxallocator-1.0)
target_link_libraries(gstsdxfilter2d ${OpenCV_LIBS} ${GST_VIDEO_LIBRARIES} gstsdxbase-1.0)
