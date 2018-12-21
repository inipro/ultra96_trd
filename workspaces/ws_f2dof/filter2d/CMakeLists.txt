cmake_minimum_required (VERSION 2.8.12)

project(gstsdxfilter2d CXX)

set(SRCS gstsdxfilter2d.cpp filter2d_cv.cpp)

add_library(${PROJECT_NAME} SHARED ${SRCS})

target_link_libraries(${PROJECT_NAME}
	${GLIB_LIBRARIES}
	${GSTREAMER_LIBRARIES}
	${GSTREAMER_VIDEO_LIBRARIES}
	gstsdxbase-1.0
	gstsdxallocator-1.0
	filter2d_optflow
	${OpenCV_LIBRARIES}
)
