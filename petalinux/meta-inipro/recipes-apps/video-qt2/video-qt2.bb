SUMMARY = "Qt Gui for Video"
LICENSE = "MIT"
LIC_FILES_CHKSUM = "file://${COMMON_LICENSE_DIR}/MIT;md5=0835ade698e0bcf8506ecda2f7b4f302"

SRC_URI = " \
		  file://CMakeLists.txt \
		  file://src/ \
		  file://qml/ \
		  file://include/ \
		  file://images/ \
		  file://font/ \
		  file://resourcefile.qrc \
		  "
S = "${WORKDIR}"

DEPENDS += "video-lib qtcharts"

#EXTRA_OECMAKE += "-DGST_MODE=on"
#EXTRA_OECMAKE += "-DGST_MODE=on -DSAMPLE_FILTER2D=on"
EXTRA_OECMAKE += "-DGST_MODE=on -DSAMPLE_FILTER2D=on -DSAMPLE_OPTICAL_FLOW=on"


inherit cmake_qt5
