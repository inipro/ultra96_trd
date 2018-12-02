SUMMARY = "libsds library"
SECTION = "libs"
LICENSE = "CLOSED"

RDEPENDS_${PN}-dev = ""

SRC_URI_zynq = "file://aarch32-linux/include/sds_lib.h \
				file://aarch32-linux/lib/libsds_lib.a \
				"

SRC_URI_zynqmp = "file://aarch64-linux/include/sds_lib.h \
				file://aarch64-linux/lib/libsds_lib.a \
				"
SRC_URI_append = " file://sds_lib.pc"

S = "${WORKDIR}"

do_install_zynq () {
	install -d ${D}${includedir}
	install -m 0644 ${WORKDIR}/aarch32-linux/include/sds_lib.h ${D}${includedir}
	install -d ${D}${libdir}
	install -m 0644 ${WORKDIR}/aarch32-linux/lib/libsds_lib.a ${D}${libdir}
}

do_install_zynqmp () {
	install -d ${D}${includedir}
	install -m 0644 ${WORKDIR}/aarch64-linux/include/sds_lib.h ${D}${includedir}
	install -d ${D}${libdir}
	install -m 0644 ${WORKDIR}/aarch64-linux/lib/libsds_lib.a ${D}${libdir}
}

do_install_append () {
	install -d ${D}${libdir}/pkgconfig
	install -m 0644 ${S}/sds_lib.pc ${D}${libdir}/pkgconfig
}
