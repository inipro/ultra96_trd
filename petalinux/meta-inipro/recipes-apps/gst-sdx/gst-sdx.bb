#
# This is the gst-sdx recipe
#
#

SUMMARY = "gst-sdx allocator and base class"
SECTION = "PETALINUX/apps"
LICENSE = "Proprietary"
LIC_FILES_CHKSUM = "file://${WORKDIR}/gst-libs/gst/base/gstsdxbase.c;beginline=1;endline=28;md5=aa044daaefd215a1f86383df1c842edf"

DEPENDS = "glib-2.0 gstreamer1.0 gstreamer1.0-plugins-base libsds"
RDEPENDS_${PN} = "libsds-staticdev"

SRC_URI = " \
	file://CMakeLists.txt \
	file://cmake/ \
	file://gst-libs/ \
	"

EXTRA_OECMAKE += "-DPLUGINS=off"

S = "${WORKDIR}"

inherit pkgconfig cmake
