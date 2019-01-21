FILESEXTRAPATHS_prepend := "${THISDIR}/files:"
SRC_URI_append = "\
	file://ntpd.cfg \
	file://busybox-ntpd \
	file://busybox-ntpd.conf \
	"
PACKAGES =+ "${PN}-ntpd"
FILES_${PN}-ntpd = "${sysconfdir}/init.d/busybox-ntpd ${sysconfdir}/busybox-ntpd.conf"
INITSCRIPT_PACKAGES += "${PN}-ntpd"
INITSCRIPT_NAME_${PN}-ntpd = "busybox-ntpd"
INITSCRIPT_PARAMS_${PN}-ntpd = "start 99 S ."

do_install_append () {
	if grep "CONFIG_NTPD=y" ${B}/.config; then
		install -m 0755 ${WORKDIR}/busybox-ntpd ${D}${sysconfdir}/init.d/
		install -m 0644 ${WORKDIR}/busybox-ntpd.conf ${D}${sysconfdir}/
	fi
}
