cp  ../petalinux/ultra96_trd/images/linux/zynqmp_fsbl.elf resources/a53_linux/boot/fsbl.elf
cp  ../petalinux/ultra96_trd/images/linux/u-boot.elf resources/a53_linux/boot/
cp  ../petalinux/ultra96_trd/images/linux/bl31.elf resources/a53_linux/boot/
cp  ../petalinux/ultra96_trd/images/linux/pmufw.elf resources/a53_linux/boot/
cp  ../petalinux/ultra96_trd/images/linux/image.ub resources/a53_linux/image/

cp  ../petalinux/ultra96_trd/project-spec/hw-description/system.hdf resources/prebuilt/ultra96_trd.hdf
cp  ../petalinux/ultra96_trd/project-spec/hw-description/system_wrapper.bit resources/prebuilt/bitstream.bit
