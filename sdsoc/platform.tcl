set name ultra96_trd

platform -name $name \
	-desc "A SDSoc platform for the Ultra96 TRD" \
	-hw [format ../%s.dsa $name] \
	-prebuilt \
	-samples ./resources/samples \
	-out ./output 

system -name a53_linux -display-name "A53 Linux" -boot ./resources/a53_linux/boot \
		   -readme ./resources/a53_linux/boot/generic.readme

domain -name a53_linux -display-name "A53 Linux" -proc psu_cortexa53 -os linux -image ./resources/a53_linux/image 

domain -prebuilt-data ./resources/prebuilt

boot -bif ./resources/a53_linux/boot/linux.bif

library -inc-path ./resources/a53_linux/inc/xfopencv

platform -generate
