DESCRIPTION = "perfapm client test"
LICENSE = "MIT"
LIC_FILES_CHKSUM = "file://${COMMON_LICENSE_DIR}/MIT;md5=0835ade698e0bcf8506ecda2f7b4f302"

SRC_URI = "file://Makefile \
		   file://src/perfapm-client-test.c \
		   "

S = "${WORKDIR}"

DEPENDS = "perfapm-client"
RDEPENDS_${PN} = "kernel-module-rpmsg-user linux-firmware-perfapm-server"

do_install () {
	install -d ${D}${bindir}
	oe_runmake install DESTDIR=${D}
}
