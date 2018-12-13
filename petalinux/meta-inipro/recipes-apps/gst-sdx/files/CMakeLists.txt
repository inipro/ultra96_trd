cmake_minimum_required (VERSION 2.8.12)

project (gst-sdx C CXX)

set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

option(PLUGINS "Build GstSDx Plugins" ON)

set(GStreamer_FIND_VERSION "1.8.3")
find_package(GStreamer REQUIRED)

find_package(PkgConfig)
pkg_check_modules(GLIB glib-2.0)
pkg_check_modules(SDSLIB sds_lib)

string(REPLACE "." ";" GST_VERSION_LIST ${GSTREAMER_VERSION})
list (GET GST_VERSION_LIST 0 GST_VERSION_MAJOR)
list (GET GST_VERSION_LIST 1 GST_VERSION_MINOR)
list (GET GST_VERSION_LIST 2 GST_VERSION_MICRO)
set(GST_API_VERSION "1.0")
set(GST_SO_VERSION "0")
MATH(EXPR GST_LIB_VERSION "${GST_VERSION_MINOR}*100+${GST_VERSION_MICRO}")
# set the lib version compatible with the libtool versioning
set(GST_LIB_VERSION "${GST_SO_VERSION}.${GST_LIB_VERSION}.0")

set(GSTSDX_DEFINITIONS "-D__FORTIFY_SOURCE=2")
set(GSTSDX_LIBRARY_INCLUDEDIR "${PROJECT_SOURCE_DIR}/gst-libs")
set(GSTSDX_INSTALL_INCLUDEDIR "include/gstreamer-${GST_API_VERSION}")
set(GSTSDX_LIB_INSTALL_DIR "lib")
set(GSTSDX_PLUGIN_INSTALL_DIR "${GSTSDX_LIB_INSTALL_DIR}/gstreamer-${GST_API_VERSION}")

set(GSTSDX_FLAGS "-Wall -O2 -fstack-protector-strong")
if (CMAKE_BUILD_TYPE STREQUAL "Debug")
  set(GSTSDX_FLAGS "${GSTSDX_FLAGS} -O0 -ggdb3")
elseif (CMAKE_BUILD_TYPE STREQUAL "RelWithDebInfo")
  set(GSTSDX_FLAGS "${GSTSDX_FLAGS} -ggdb3")
endif()
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${GSTSDX_FLAGS}")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${GSTSDX_FLAGS}")

# Setup common environment
include_directories(
  ${GLIB_INCLUDE_DIRS}
  ${GSTREAMER_INCLUDE_DIRS}
)

add_definitions(
  ${GSTREAMER_DEFINITIONS}
  ${GSTSDX_DEFINITIONS}
)

if (PLUGINS)
add_subdirectory(src)
endif ()
add_subdirectory(gst-libs)
