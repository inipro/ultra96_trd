FILESEXTRAPATHS_prepend := "${THISDIR}/linux-xlnx:"

SRC_URI_append = " \
    file://0001-drm-xilinx-dp_sub-Reset-the-coefficients-in-set-form.patch \
    file://0001-drm-xlnx-zynqmp-Don-t-update-the-plane-for-the-same-.patch \
    file://0001-media-xilinx-TPG-Add-IOCTL-to-set-PPC.patch \
    file://0003-HACK-media-xilinx-vip-Don-t-print-version.patch \
    file://0004-ov5640-for-pcam5.patch \
    file://0005-xilinx-media-hack.patch \
    file://0006-xilinx-csi2-for-pcam5.patch \
	"
