do_install_append() {
	ln -sf -r ${D}${libdir}/libmediactl.so.0.0.0 ${D}${libdir}/libmediactl.so
}

PACKAGES =+ "media-ctl-dev"
RDEPENDS_media-ctl-dev = "media-ctl"
FILES_media-ctl-dev = "${libdir}/libmediactl.so"
