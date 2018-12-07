set project_name dm6
set part_name xczu3eg-sbva484-1-i
set ip_dir repo
set bd_name system
set bd_path $project_name/$project_name.srcs/sources_1/bd/$bd_name

file delete -force $project_name

create_project -part $part_name $project_name $project_name

set_property ip_repo_paths $ip_dir [current_project]
update_ip_catalog

create_bd_design $bd_name

create_bd_cell -type ip -vlnv xilinx.com:ip:zynq_ultra_ps_e:3.2 zynq_ultra_ps_e_0
source dm6_preset.tcl
set_property -dict [apply_preset IPINST] [get_bd_cells zynq_ultra_ps_e_0]

create_bd_cell -type ip -vlnv xilinx.com:ip:clk_wiz:6.0 clk_wiz_0
set_property -dict [list CONFIG.PRIM_IN_FREQ {49.999950} CONFIG.CLKOUT2_USED {true} CONFIG.CLKOUT3_USED {true} CONFIG.CLKOUT4_USED {true} CONFIG.CLKOUT5_USED {true} CONFIG.CLKOUT1_REQUESTED_OUT_FREQ {50.000} CONFIG.CLKOUT2_REQUESTED_OUT_FREQ {75.000} CONFIG.CLKOUT3_REQUESTED_OUT_FREQ {150.000} CONFIG.CLKOUT4_REQUESTED_OUT_FREQ {300.000} CONFIG.CLKOUT5_REQUESTED_OUT_FREQ {200.000} CONFIG.USE_RESET {false} CONFIG.CLKIN1_JITTER_PS {200.0} CONFIG.MMCM_DIVCLK_DIVIDE {1} CONFIG.MMCM_CLKFBOUT_MULT_F {24.000} CONFIG.MMCM_CLKIN1_PERIOD {20.000} CONFIG.MMCM_CLKIN2_PERIOD {10.0} CONFIG.MMCM_CLKOUT0_DIVIDE_F {24.000} CONFIG.MMCM_CLKOUT1_DIVIDE {16} CONFIG.MMCM_CLKOUT2_DIVIDE {8} CONFIG.MMCM_CLKOUT3_DIVIDE {4} CONFIG.MMCM_CLKOUT4_DIVIDE {6} CONFIG.NUM_OUT_CLKS {5} CONFIG.CLKOUT1_JITTER {163.696} CONFIG.CLKOUT1_PHASE_ERROR {154.678} CONFIG.CLKOUT2_JITTER {148.365} CONFIG.CLKOUT2_PHASE_ERROR {154.678} CONFIG.CLKOUT3_JITTER {129.922} CONFIG.CLKOUT3_PHASE_ERROR {154.678} CONFIG.CLKOUT4_JITTER {116.496} CONFIG.CLKOUT4_PHASE_ERROR {154.678} CONFIG.CLKOUT5_JITTER {124.134} CONFIG.CLKOUT5_PHASE_ERROR {154.678}] [get_bd_cells clk_wiz_0]

connect_bd_net [get_bd_pins zynq_ultra_ps_e_0/pl_clk0] [get_bd_pins clk_wiz_0/clk_in1]

create_bd_cell -type ip -vlnv xilinx.com:ip:proc_sys_reset:5.0 proc_sys_reset_0
connect_bd_net [get_bd_pins zynq_ultra_ps_e_0/pl_resetn0] [get_bd_pins proc_sys_reset_0/ext_reset_in]
connect_bd_net [get_bd_pins clk_wiz_0/locked] [get_bd_pins proc_sys_reset_0/dcm_locked]

apply_bd_automation -rule xilinx.com:bd_rule:clkrst -config {Clk "/clk_wiz_0/clk_out1 (50 MHz)" }  [get_bd_pins proc_sys_reset_0/slowest_sync_clk]

create_bd_cell -type ip -vlnv xilinx.com:ip:axi_interconnect:2.1 axi_interconnect_0
set_property -dict [list CONFIG.NUM_SI {1} CONFIG.NUM_MI {1} CONFIG.STRATEGY {1}] [get_bd_cells axi_interconnect_0]
connect_bd_intf_net [get_bd_intf_pins zynq_ultra_ps_e_0/M_AXI_HPM0_LPD] -boundary_type upper [get_bd_intf_pins axi_interconnect_0/S00_AXI]
apply_bd_automation -rule xilinx.com:bd_rule:clkrst -config {Clk "/clk_wiz_0/clk_out1 (50 MHz)" }  [get_bd_pins zynq_ultra_ps_e_0/maxihpm0_lpd_aclk]
apply_bd_automation -rule xilinx.com:bd_rule:clkrst -config {Clk "/clk_wiz_0/clk_out1 (50 MHz)" }  [get_bd_pins axi_interconnect_0/ACLK]

create_bd_cell -type ip -vlnv xilinx.com:ip:axi_uartlite:2.0 axi_uartlite_0
set_property -dict [list CONFIG.C_BAUDRATE {115200}] [get_bd_cells axi_uartlite_0]
apply_bd_automation -rule xilinx.com:bd_rule:axi4 -config { Clk_master {/clk_wiz_0/clk_out1 (49 MHz)} Clk_slave {Auto} Clk_xbar {/clk_wiz_0/clk_out1 (49 MHz)} Master {/zynq_ultra_ps_e_0/M_AXI_HPM0_LPD} Slave {/axi_uartlite_0/S_AXI} intc_ip {/axi_interconnect_0} master_apm {0}}  [get_bd_intf_pins axi_uartlite_0/S_AXI]

source tpg_input_hier.tcl
create_tpg_input [current_bd_instance .] tpg_input_0

create_bd_cell -type ip -vlnv xilinx.com:ip:axi_interconnect:2.1 axi_interconnect_1
set_property -dict [list CONFIG.NUM_SI {1} CONFIG.NUM_MI {2} CONFIG.STRATEGY {1}] [get_bd_cells axi_interconnect_1]
connect_bd_intf_net [get_bd_intf_pins zynq_ultra_ps_e_0/M_AXI_HPM0_FPD] -boundary_type upper [get_bd_intf_pins axi_interconnect_1/S00_AXI]
apply_bd_automation -rule xilinx.com:bd_rule:clkrst -config {Clk "/clk_wiz_0/clk_out1 (50 MHz)" }  [get_bd_pins axi_interconnect_1/ACLK]
apply_bd_automation -rule xilinx.com:bd_rule:clkrst -config {Clk "/clk_wiz_0/clk_out1 (50 MHz)" }  [get_bd_pins axi_interconnect_1/S00_ACLK]

connect_bd_intf_net -boundary_type upper [get_bd_intf_pins axi_interconnect_1/M00_AXI] [get_bd_intf_pins tpg_input_0/ctrl1]
connect_bd_net [get_bd_pins axi_interconnect_1/M00_ACLK] [get_bd_pins clk_wiz_0/clk_out1]
connect_bd_net [get_bd_pins axi_interconnect_1/M00_ARESETN] [get_bd_pins proc_sys_reset_0/peripheral_aresetn]
connect_bd_net [get_bd_pins tpg_input_0/s_axi_aclk] [get_bd_pins clk_wiz_0/clk_out1]


create_bd_cell -type ip -vlnv xilinx.com:ip:axi_interconnect:2.1 axi_interconnect_2
set_property -dict [list CONFIG.NUM_MI {3} CONFIG.STRATEGY {1}] [get_bd_cells axi_interconnect_2]
connect_bd_intf_net [get_bd_intf_pins zynq_ultra_ps_e_0/M_AXI_HPM1_FPD] -boundary_type upper [get_bd_intf_pins axi_interconnect_2/S00_AXI]
apply_bd_automation -rule xilinx.com:bd_rule:clkrst -config {Clk "/clk_wiz_0/clk_out4 (300 MHz)" }  [get_bd_pins axi_interconnect_2/ACLK]
apply_bd_automation -rule xilinx.com:bd_rule:clkrst -config {Clk "/clk_wiz_0/clk_out4 (300 MHz)" }  [get_bd_pins axi_interconnect_2/S00_ACLK]

connect_bd_net [get_bd_pins rst_clk_wiz_0_299M/ext_reset_in] [get_bd_pins zynq_ultra_ps_e_0/pl_resetn0]

connect_bd_intf_net -boundary_type upper [get_bd_intf_pins axi_interconnect_2/M00_AXI] [get_bd_intf_pins tpg_input_0/s_axi_ctrl]
connect_bd_net [get_bd_pins axi_interconnect_2/M00_ACLK] [get_bd_pins clk_wiz_0/clk_out4]
connect_bd_net [get_bd_pins axi_interconnect_2/M00_ARESETN] [get_bd_pins rst_clk_wiz_0_299M/peripheral_aresetn]

connect_bd_intf_net -boundary_type upper [get_bd_intf_pins axi_interconnect_2/M01_AXI] [get_bd_intf_pins tpg_input_0/ctrl]
connect_bd_net [get_bd_pins axi_interconnect_2/M01_ACLK] [get_bd_pins clk_wiz_0/clk_out4]
connect_bd_net [get_bd_pins axi_interconnect_2/M01_ARESETN] [get_bd_pins rst_clk_wiz_0_299M/peripheral_aresetn]

connect_bd_net [get_bd_pins tpg_input_0/m_axi_s2mm_aclk] [get_bd_pins clk_wiz_0/clk_out4]
connect_bd_net [get_bd_pins zynq_ultra_ps_e_0/dp_video_ref_clk] [get_bd_pins tpg_input_0/clk_in]
connect_bd_net [get_bd_pins zynq_ultra_ps_e_0/emio_gpio_o] [get_bd_pins tpg_input_0/Din]

create_bd_cell -type ip -vlnv xilinx.com:ip:axi_interconnect:2.1 axi_interconnect_3
set_property -dict [list CONFIG.NUM_SI {2} CONFIG.NUM_MI {1} CONFIG.STRATEGY {2} CONFIG.S00_HAS_DATA_FIFO {2} CONFIG.S01_HAS_DATA_FIFO {2}] [get_bd_cells axi_interconnect_3]
connect_bd_intf_net -boundary_type upper [get_bd_intf_pins axi_interconnect_3/M00_AXI] [get_bd_intf_pins zynq_ultra_ps_e_0/S_AXI_HP0_FPD]
apply_bd_automation -rule xilinx.com:bd_rule:clkrst -config {Clk "/clk_wiz_0/clk_out4 (300 MHz)" }  [get_bd_pins axi_interconnect_3/ACLK]
apply_bd_automation -rule xilinx.com:bd_rule:clkrst -config {Clk "/clk_wiz_0/clk_out4 (300 MHz)" }  [get_bd_pins axi_interconnect_3/M00_ACLK]

connect_bd_intf_net -boundary_type upper [get_bd_intf_pins tpg_input_0/M_AXI_S2MM] [get_bd_intf_pins axi_interconnect_3/S00_AXI]
connect_bd_net [get_bd_pins axi_interconnect_3/S00_ACLK] [get_bd_pins clk_wiz_0/clk_out4]
connect_bd_net [get_bd_pins axi_interconnect_3/S00_ARESETN] [get_bd_pins rst_clk_wiz_0_299M/peripheral_aresetn]

create_bd_cell -type ip -vlnv xilinx.com:ip:xlconcat:2.1 xlconcat_0
set_property -dict [list CONFIG.NUM_PORTS {1}] [get_bd_cells xlconcat_0]
connect_bd_net [get_bd_pins tpg_input_0/s2mm_introut] [get_bd_pins xlconcat_0/In0]
connect_bd_net [get_bd_pins xlconcat_0/dout] [get_bd_pins zynq_ultra_ps_e_0/pl_ps_irq0]

make_bd_intf_pins_external  [get_bd_intf_pins axi_uartlite_0/UART]
set_property name UART0 [get_bd_intf_ports UART_0]

make_bd_pins_external  [get_bd_pins zynq_ultra_ps_e_0/emio_uart0_ctsn]
set_property name BT_ctsn [get_bd_ports emio_uart0_ctsn_0]

make_bd_pins_external  [get_bd_pins zynq_ultra_ps_e_0/emio_uart0_rtsn]
set_property name BT_rtsn [get_bd_ports emio_uart0_rtsn_0]

assign_bd_address

###
connect_bd_net [get_bd_pins axi_interconnect_1/M01_ACLK] [get_bd_pins clk_wiz_0/clk_out1]
connect_bd_net [get_bd_pins axi_interconnect_1/M01_ARESETN] [get_bd_pins proc_sys_reset_0/peripheral_aresetn]
connect_bd_net [get_bd_pins axi_interconnect_2/M02_ACLK] [get_bd_pins clk_wiz_0/clk_out4]
connect_bd_net [get_bd_pins axi_interconnect_2/M02_ARESETN] [get_bd_pins rst_clk_wiz_0_299M/peripheral_aresetn]
connect_bd_net [get_bd_pins axi_interconnect_3/S01_ACLK] [get_bd_pins clk_wiz_0/clk_out4]
connect_bd_net [get_bd_pins axi_interconnect_3/S01_ARESETN] [get_bd_pins rst_clk_wiz_0_299M/peripheral_aresetn]
###

regenerate_bd_layout
validate_bd_design
save_bd_design

generate_target all [get_files $bd_path/$bd_name.bd]
make_wrapper -files [get_files $bd_path/$bd_name.bd] -top

add_files -norecurse $bd_path/hdl/${bd_name}_wrapper.v

add_files -norecurse -fileset constrs_1 dm6.xdc
import_files -fileset constrs_1 dm6.xdc

set_property verilog_define {TOOL_VIVADO} [current_fileset]

close_project

