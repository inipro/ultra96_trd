DESCRIPTION = "video library"
LICENSE = "MIT"
LIC_FILES_CHKSUM = "file://${COMMON_LICENSE_DIR}/MIT;md5=0835ade698e0bcf8506ecda2f7b4f302"

SRC_URI = " \
		  file://CMakeLists.txt \
		  file://src/ \
		  file://include/ \
		  file://video.pc \
		  "
S = "${WORKDIR}"

inherit cmake

DEPENDS="glib-2.0 libdrm v4l-utils gstreamer1.0 libsds"

EXTRA_OECMAKE += "-DVSRC_INIT=on"
#EXTRA_OECMAKE += "-DWITH_SDSOC=on -DGST_MODE=on"

do_install_append () {
	install -d ${D}${libdir}/pkgconfig
	install -m 0644 ${S}/video.pc ${D}${libdir}/pkgconfig
}

