#SDSOC Platform DSA Generation Script

set pfm_board_id ultra96
set pfm_vendor inipro.net
set pfm_ver 1.0
set pfm_name $pfm_vendor:$pfm_board_id:ultra96_trd:$pfm_ver
set bd_name system
set project_name dm6

set_property PFM_NAME $pfm_name [get_files ./$project_name.srcs/sources_1/bd/$bd_name/$bd_name.bd]

set_property PFM.CLOCK { \
	clk_out4 {id "0" is_default "true" proc_sys_reset "rst_clk_wiz_0_300M"} \
	clk_out1 {id "1" is_default "false" proc_sys_reset "proc_sys_reset_0"} \
} [get_bd_cells /clk_wiz_0]

set parVal []
for {set i 2} {$i < 64} {incr i} {
	lappend parVal M[format %02d $i]_AXI \
	{memport "M_AXI_GP"}
}
set_property PFM.AXI_PORT $parVal [get_bd_cells /axi_interconnect_1]

set parVal []
for {set i 3} {$i < 64} {incr i} {
	lappend parVal M[format %02d $i]_AXI \
	{memport "M_AXI_GP"}
}
set_property PFM.AXI_PORT $parVal [get_bd_cells /axi_interconnect_2]

set parVal []
for {set i 2} {$i < 16} {incr i} {
	lappend parVal S[format %02d $i]_AXI \
	{memport "S_AXI_HP" sptag "HP0"}
}
set_property PFM.AXI_PORT $parVal [get_bd_cells /axi_interconnect_3]

set_property PFM.AXI_PORT { \
	S_AXI_HP1_FPD {memport "S_AXI_HP" sptag "HP1"} \
	S_AXI_HP2_FPD {memport "S_AXI_HP" sptag "HP2"} \
	S_AXI_HP3_FPD {memport "S_AXI_HP" sptag "HP3"} \
} [get_bd_cells /zynq_ultra_ps_e_0]

set intVar []
for {set i 3} {$i < 8} {incr i} {
	lappend intVar In$i {}
}
set_property PFM.IRQ $intVar [get_bd_cells /xlconcat_0]

set intVar []
for {set i 0} {$i < 8} {incr i} {
	lappend intVar In$i {}
}
set_property PFM.IRQ $intVar [get_bd_cells /xlconcat_1]

set_property dsa.ip_cache_dir [get_property ip_output_repo [current_project]] [current_project]

write_dsa -force -include_bit ../ultra96_trd.dsa
