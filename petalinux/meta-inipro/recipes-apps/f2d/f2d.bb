DESCRIPTION = "video library"
LICENSE = "MIT"
LIC_FILES_CHKSUM = "file://${COMMON_LICENSE_DIR}/MIT;md5=0835ade698e0bcf8506ecda2f7b4f302"

SRC_URI = " \
		  file://CMakeLists.txt \
		  file://main.c \
		  file://src/ \
		  file://top/ \
		  "
S = "${WORKDIR}"

inherit cmake

DEPENDS="video-lib opencv"

EXTRA_OECMAKE += "-DSW_ONLY=on"
