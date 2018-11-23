set project_name dm1
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
source dm1_preset.tcl
set_property -dict [apply_preset IPINST] [get_bd_cells zynq_ultra_ps_e_0]

create_bd_cell -type ip -vlnv xilinx.com:ip:axi_uartlite:2.0 axi_uartlite_0
set_property -dict [list CONFIG.C_BAUDRATE {115200}] [get_bd_cells axi_uartlite_0]
apply_bd_automation -rule xilinx.com:bd_rule:axi4 -config { Clk_master {Auto} Clk_slave {Auto} Clk_xbar {Auto} Master {/zynq_ultra_ps_e_0/M_AXI_HPM0_LPD} Slave {/axi_uartlite_0/S_AXI} intc_ip {New AXI Interconnect} master_apm {0}}  [get_bd_intf_pins axi_uartlite_0/S_AXI]
connect_bd_net [get_bd_pins axi_uartlite_0/interrupt] [get_bd_pins zynq_ultra_ps_e_0/pl_ps_irq0]

make_bd_intf_pins_external  [get_bd_intf_pins axi_uartlite_0/UART]
set_property name UART0 [get_bd_intf_ports UART_0]

make_bd_pins_external  [get_bd_pins zynq_ultra_ps_e_0/emio_uart0_ctsn]
set_property name BT_ctsn [get_bd_ports emio_uart0_ctsn_0]

make_bd_pins_external  [get_bd_pins zynq_ultra_ps_e_0/emio_uart0_rtsn]
set_property name BT_rtsn [get_bd_ports emio_uart0_rtsn_0]

regenerate_bd_layout
validate_bd_design
save_bd_design

generate_target all [get_files $bd_path/$bd_name.bd]
make_wrapper -files [get_files $bd_path/$bd_name.bd] -top

add_files -norecurse $bd_path/hdl/${bd_name}_wrapper.v

add_files -norecurse -fileset constrs_1 dm1.xdc
import_files -fileset constrs_1 dm1.xdc

set_property verilog_define {TOOL_VIVADO} [current_fileset]

close_project

