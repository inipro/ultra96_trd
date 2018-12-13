FILESEXTRAPATHS_prepend := "${THISDIR}/files:"
SRC_URI = " \
    file://Makefile \
    file://ultra96-power-button.sh \
    file://ultra96-power-button-check.c \
	file://lcd_display.patch \
    "

do_install() {
    install -d ${D}/sbin
    install -d ${D}${sysconfdir}/init.d/
    install -m 0755 ultra96-power-button-check ${D}/sbin
    install -m 0755 ${S}/ultra96-power-button.sh ${D}${sysconfdir}/init.d/ultra96-power-button.sh
}

