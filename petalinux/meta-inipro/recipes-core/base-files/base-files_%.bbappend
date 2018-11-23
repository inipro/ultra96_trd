FILESEXTRAPATHS_prepend := "${THISDIR}/files:"
dirs755 += "/media/card"

SRC_URI += " \
            file://dot.profile \
            file://dot.bashrc \
            file://dot.vimrc \
"


do_install_append () {
	echo "/dev/mmcblk0p1 /media/card auto defaults,sync  0  0" >> ${D}${sysconfdir}/fstab

	install -m 0644 ${WORKDIR}/dot.profile ${D}/home/root/.profile
	install -m 0644 ${WORKDIR}/dot.bashrc ${D}/home/root/.bashrc
	install -m 0644 ${WORKDIR}/dot.vimrc ${D}/home/root/.vimrc
}
