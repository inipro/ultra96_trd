FILESEXTRAPATHS_prepend := "${THISDIR}/files:"

SRC_URI_append = " file://perfapm-server.elf"
PACKAGES =+ "${PN}-perfapm-server"

FILES_${PN}-perfapm-server = "${nonarch_base_libdir}/firmware/perfapm-server.elf"

do_install_append () {
	install -d ${D}${nonarch_base_libdir}/firmware
	install -m 0644 ../perfapm-server.elf ${D}${nonarch_base_libdir}/firmware
}
