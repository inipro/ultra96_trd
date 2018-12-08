proc create_mipi_csi2_rx { parentCell nameHier } {
	if { $parentCell eq "" || $nameHier eq "" } {
		catch {common::send_msg_id "BD_TCL-102" "ERROR" "create_mipi_csi2_rx() - Empty argument(s)!"}
		return
	}

	# Get object for parentCell
	set parentObj [get_bd_cells $parentCell]
	if { $parentObj == "" } {
		catch {common::send_msg_id "BD_TCL-100" "ERROR" "Unable to find parent cell <$parentCell>!"}
		return
	}

	# Make sure parentObj is hier blk
	set parentType [get_property TYPE $parentObj]
	if { $parentType ne "hier" } {
		catch {common::send_msg_id "BD_TCL-101" "ERROR" "Parent <$parentObj> has TYPE = <$parentType>. Expected to be <hier>."}
		return
	}

	# Save current instance; Restore later
	set oldCurInst [current_bd_instance .]

	# Set parent object as current
	current_bd_instance $parentObj

	# Create cell and set as current instance
	set hier_obj [create_bd_cell -type hier $nameHier]
	current_bd_instance $hier_obj

###############################################################################

	create_bd_intf_pin -mode Slave -vlnv xilinx.com:interface:aximm_rtl:1.0 s_axi_csirxss
	create_bd_intf_pin -mode Slave -vlnv xilinx.com:interface:aximm_rtl:1.0 s_axi_ctrl
	create_bd_intf_pin -mode Master -vlnv xilinx.com:interface:aximm_rtl:1.0 M_AXI_S2MM
	create_bd_intf_pin -mode Slave -vlnv xilinx.com:interface:mipi_phy_rtl:1.0 mipi_phy_if

	create_bd_pin -dir I -from 94 -to 0 Din
	create_bd_pin -dir I -type clk s_axi_lite_aclk
	create_bd_pin -dir I -type rst axi_resetn
	create_bd_pin -dir I -type clk m_axi_s2mm_aclk
	create_bd_pin -dir I -type clk dphy_clk_200M

	create_bd_pin -dir O s2mm_introut
	create_bd_pin -dir O csirxss_csi_irq

	create_bd_cell -type ip -vlnv xilinx.com:ip:mipi_csi2_rx_subsystem:3.0 mipi_csi2_rx_subsyst_0
	set_property -dict [list CONFIG.C_EN_BG0_PIN0 {false} CONFIG.CMN_PXL_FORMAT {YUV422_8bit} CONFIG.CMN_NUM_LANES {2} CONFIG.C_DPHY_LANES {2} CONFIG.CMN_NUM_PIXELS {2} CONFIG.C_HS_LINE_RATE {800} CONFIG.DPY_LINE_RATE {800} CONFIG.DPY_EN_REG_IF {true} CONFIG.CSI_BUF_DEPTH {4096} CONFIG.CLK_LANE_IO_LOC {N2} CONFIG.DATA_LANE0_IO_LOC {N5} CONFIG.DATA_LANE1_IO_LOC {M2} CONFIG.DATA_LANE2_IO_LOC {M5} CONFIG.DATA_LANE3_IO_LOC {L2} CONFIG.CLK_LANE_IO_LOC_NAME {IO_L7P_T1L_N0_QBC_AD13P_65} CONFIG.DATA_LANE0_IO_LOC_NAME {IO_L8P_T1L_N2_AD5P_65} CONFIG.DATA_LANE1_IO_LOC_NAME {IO_L9P_T1L_N4_AD12P_65} CONFIG.DATA_LANE2_IO_LOC_NAME {IO_L10P_T1U_N6_QBC_AD4P_65} CONFIG.DATA_LANE3_IO_LOC_NAME {IO_L11P_T1U_N8_GC_65} CONFIG.SupportLevel {1} CONFIG.C_CLK_LANE_IO_POSITION {13} CONFIG.C_DATA_LANE0_IO_POSITION {15} CONFIG.C_DATA_LANE1_IO_POSITION {17} CONFIG.C_HS_SETTLE_NS {147}] [get_bd_cells mipi_csi2_rx_subsyst_0]

	connect_bd_intf_net [get_bd_intf_pins mipi_phy_if] [get_bd_intf_pins mipi_csi2_rx_subsyst_0/mipi_phy_if]
	connect_bd_intf_net [get_bd_intf_pins s_axi_csirxss] [get_bd_intf_pins mipi_csi2_rx_subsyst_0/csirxss_s_axi]
	connect_bd_net [get_bd_pins s_axi_lite_aclk] [get_bd_pins mipi_csi2_rx_subsyst_0/lite_aclk]
	connect_bd_net [get_bd_pins axi_resetn] [get_bd_pins mipi_csi2_rx_subsyst_0/lite_aresetn]
	connect_bd_net [get_bd_pins dphy_clk_200M] [get_bd_pins mipi_csi2_rx_subsyst_0/dphy_clk_200M]
	connect_bd_net [get_bd_pins m_axi_s2mm_aclk] [get_bd_pins mipi_csi2_rx_subsyst_0/video_aclk]
	connect_bd_net [get_bd_pins csirxss_csi_irq] [get_bd_pins mipi_csi2_rx_subsyst_0/csirxss_csi_irq]

	create_bd_cell -type ip -vlnv xilinx.com:ip:xlslice:1.0 xlslice_0
	set_property -dict [list CONFIG.DIN_TO {2} CONFIG.DIN_FROM {2} CONFIG.DIN_WIDTH {95} CONFIG.DIN_FROM {2} CONFIG.DOUT_WIDTH {1}] [get_bd_cells xlslice_0]

	connect_bd_net [get_bd_pins Din] [get_bd_pins xlslice_0/Din]
	connect_bd_net [get_bd_pins xlslice_0/Dout] [get_bd_pins mipi_csi2_rx_subsyst_0/video_aresetn]

	create_bd_cell -type ip -vlnv xilinx.com:ip:axis_subset_converter:1.1 axis_subset_converter_0
	set_property -dict [list CONFIG.S_TDATA_NUM_BYTES {4} CONFIG.M_TDATA_NUM_BYTES {6} CONFIG.S_TDEST_WIDTH {8} CONFIG.M_TDEST_WIDTH {8} CONFIG.S_TUSER_WIDTH {1} CONFIG.M_TUSER_WIDTH {1} CONFIG.S_HAS_TLAST {1} CONFIG.M_HAS_TLAST {1} CONFIG.TDATA_REMAP {16'b0000000000000000,tdata[31:0]} CONFIG.TUSER_REMAP {tuser[0:0]} CONFIG.TDEST_REMAP {tdest[7:0]} CONFIG.TLAST_REMAP {tlast[0]}] [get_bd_cells axis_subset_converter_0]

	connect_bd_intf_net [get_bd_intf_pins mipi_csi2_rx_subsyst_0/video_out] [get_bd_intf_pins axis_subset_converter_0/S_AXIS]
	connect_bd_net [get_bd_pins m_axi_s2mm_aclk] [get_bd_pins axis_subset_converter_0/aclk]
	connect_bd_net [get_bd_pins axis_subset_converter_0/aresetn] [get_bd_pins xlslice_0/Dout]

	create_bd_cell -type ip -vlnv xilinx.com:ip:v_frmbuf_wr:2.1 v_frmbuf_wr_0
	set_property -dict [list CONFIG.HAS_YUYV8 {1} CONFIG.HAS_RGB8 {0} CONFIG.HAS_Y8 {1} CONFIG.HAS_UYVY8 {1}] [get_bd_cells v_frmbuf_wr_0]

	connect_bd_intf_net [get_bd_intf_pins s_axi_ctrl] [get_bd_intf_pins v_frmbuf_wr_0/s_axi_CTRL]
	connect_bd_intf_net [get_bd_intf_pins axis_subset_converter_0/M_AXIS] [get_bd_intf_pins v_frmbuf_wr_0/s_axis_video]
	connect_bd_net [get_bd_pins m_axi_s2mm_aclk] [get_bd_pins v_frmbuf_wr_0/ap_clk]

	create_bd_cell -type ip -vlnv xilinx.com:ip:xlslice:1.0 xlslice_1
	set_property -dict [list CONFIG.DIN_TO {3} CONFIG.DIN_FROM {3} CONFIG.DIN_WIDTH {95} CONFIG.DIN_FROM {3} CONFIG.DOUT_WIDTH {1}] [get_bd_cells xlslice_1]

	connect_bd_net [get_bd_pins Din] [get_bd_pins xlslice_1/Din]
	connect_bd_net [get_bd_pins xlslice_1/Dout] [get_bd_pins v_frmbuf_wr_0/ap_rst_n]
	connect_bd_intf_net [get_bd_intf_pins M_AXI_S2MM] [get_bd_intf_pins v_frmbuf_wr_0/m_axi_mm_video]
	connect_bd_net [get_bd_pins s2mm_introut] [get_bd_pins v_frmbuf_wr_0/interrupt]


###############################################################################
	# Restore current instance
	current_bd_instance $oldCurInst
}
