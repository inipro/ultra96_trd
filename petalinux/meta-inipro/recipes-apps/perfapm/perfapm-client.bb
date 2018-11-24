DESCRIPTION = "library for perfapm client"
LICENSE = "MIT"
LIC_FILES_CHKSUM = "file://${COMMON_LICENSE_DIR}/MIT;md5=0835ade698e0bcf8506ecda2f7b4f302"

SRC_URI = "file://Makefile \
		   file://include/perfapm-client.h \
		   file://src/perfapm-client.c \
		   "

S = "${WORKDIR}"

do_install () {
	install -d ${D}${includedir}
	install -d ${D}${libdir}
	oe_runmake install DESTDIR=${D}
}
