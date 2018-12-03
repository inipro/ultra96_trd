DESCRIPTION = "gst library"
LICENSE = "MIT"
LIC_FILES_CHKSUM = "file://${COMMON_LICENSE_DIR}/MIT;md5=0835ade698e0bcf8506ecda2f7b4f302"

SRC_URI = " \
		  file://CMakeLists.txt \
		  file://video.h \
		  file://filter.h \
		  file://helper.h \
		  file://common.h \
		  file://src/ \
		  file://include/ \
		  file://gst.pc \
		  "
S = "${WORKDIR}"

inherit cmake

DEPENDS="gstreamer1.0 gstreamer1.0-plugins-bad"

do_install_append () {
	install -d ${D}${libdir}/pkgconfig
	install -m 0644 ${S}/gst.pc ${D}${libdir}/pkgconfig
}

