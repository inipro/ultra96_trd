SUMMARY = "Show Image(OpenCV with Qt C++ Examples)"
LICENSE = "MIT"
LIC_FILES_CHKSUM = "file://${COMMON_LICENSE_DIR}/MIT;md5=0835ade698e0bcf8506ecda2f7b4f302"

SRC_URI = " \
		  file://CMakeLists.txt \
		  file://main.cpp \
		  file://mainwindow.cpp \
		  file://mainwindow.h \
		  file://mainwindow.ui \
		  file://lena.jpg \
		  "
S = "${WORKDIR}"

DEPENDS += "opencv"

inherit cmake_qt5

#FILES_${PN} += "${datadir}/${PN}/lena.jpg"

do_install_append () {
	install -d ${D}${datadir}/${PN}
	install -m 0644 ${S}/lena.jpg ${D}${datadir}/${PN}
}
