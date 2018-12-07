proc create_tpg_input { parentCell nameHier } {
    if { $parentCell eq "" || $nameHier eq "" } {
        catch {common::send_msg_id "BD_TCL-102" "ERROR" "create_tpg_input() - Empty argument(s)!"}
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
    create_bd_intf_pin -mode Slave -vlnv xilinx.com:interface:aximm_rtl:1.0 ctrl1
    create_bd_pin -dir I -type clk s_axi_aclk
    create_bd_pin -dir I -type clk clk_in
    create_bd_pin -dir I -type clk m_axi_s2mm_aclk
    create_bd_intf_pin -mode Slave -vlnv xilinx.com:interface:aximm_rtl:1.0 s_axi_ctrl
    create_bd_intf_pin -mode Slave -vlnv xilinx.com:interface:aximm_rtl:1.0 ctrl
    create_bd_pin -dir I -from 94 -to 0 Din
    create_bd_intf_pin -mode Master -vlnv xilinx.com:interface:aximm_rtl:1.0 M_AXI_S2MM
    create_bd_pin -dir O s2mm_introut

	create_bd_cell -type ip -vlnv xilinx.com:ip:v_tc:6.1 v_tc_0
	set_property -dict [list CONFIG.max_clocks_per_line {8192} CONFIG.max_lines_per_frame {8192} CONFIG.VIDEO_MODE {1080p} CONFIG.GEN_F0_VSYNC_VSTART {1083} CONFIG.GEN_F1_VSYNC_VSTART {1083} CONFIG.GEN_HACTIVE_SIZE {1920} CONFIG.GEN_HSYNC_END {2052} CONFIG.GEN_HFRAME_SIZE {2200} CONFIG.GEN_F0_VSYNC_HSTART {1920} CONFIG.GEN_F1_VSYNC_HSTART {1920} CONFIG.GEN_F0_VSYNC_HEND {1920} CONFIG.GEN_F1_VSYNC_HEND {1920} CONFIG.GEN_F0_VFRAME_SIZE {1125} CONFIG.GEN_F1_VFRAME_SIZE {1125} CONFIG.GEN_F0_VSYNC_VEND {1088} CONFIG.GEN_F1_VSYNC_VEND {1088} CONFIG.GEN_F0_VBLANK_HEND {1920} CONFIG.GEN_F1_VBLANK_HEND {1920} CONFIG.GEN_HSYNC_START {2008} CONFIG.GEN_VACTIVE_SIZE {1080} CONFIG.GEN_F0_VBLANK_HSTART {1920} CONFIG.GEN_F1_VBLANK_HSTART {1920} CONFIG.enable_detection {false}] [get_bd_cells v_tc_0]

	connect_bd_intf_net [get_bd_intf_pins ctrl1] [get_bd_intf_pins v_tc_0/ctrl]
	connect_bd_net [get_bd_pins s_axi_aclk] [get_bd_pins v_tc_0/s_axi_aclk]
	connect_bd_net [get_bd_pins clk_in] [get_bd_pins v_tc_0/clk]

	create_bd_cell -type ip -vlnv xilinx.com:ip:v_vid_in_axi4s:4.0 v_vid_in_axi4s_0
	set_property -dict [list CONFIG.C_PIXELS_PER_CLOCK {2} CONFIG.C_HAS_ASYNC_CLK {1} CONFIG.C_ADDR_WIDTH {5}] [get_bd_cells v_vid_in_axi4s_0]

	connect_bd_net [get_bd_pins v_tc_0/active_video_out] [get_bd_pins v_vid_in_axi4s_0/vid_active_video]
	connect_bd_net [get_bd_pins v_tc_0/hblank_out] [get_bd_pins v_vid_in_axi4s_0/vid_hblank]
	connect_bd_net [get_bd_pins v_tc_0/hsync_out] [get_bd_pins v_vid_in_axi4s_0/vid_hsync]
	connect_bd_net [get_bd_pins v_tc_0/vblank_out] [get_bd_pins v_vid_in_axi4s_0/vid_vblank]
	connect_bd_net [get_bd_pins v_tc_0/vsync_out] [get_bd_pins v_vid_in_axi4s_0/vid_vsync]
	connect_bd_net [get_bd_pins clk_in] [get_bd_pins v_vid_in_axi4s_0/vid_io_in_clk]
	connect_bd_net [get_bd_pins m_axi_s2mm_aclk] [get_bd_pins v_vid_in_axi4s_0/aclk]

	create_bd_cell -type ip -vlnv xilinx.com:ip:xlconstant:1.1 xlconstant_0
	set_property -dict [list CONFIG.CONST_WIDTH {48} CONFIG.CONST_VAL {0}] [get_bd_cells xlconstant_0]

	connect_bd_net [get_bd_pins xlconstant_0/dout] [get_bd_pins v_vid_in_axi4s_0/vid_data]

	create_bd_cell -type ip -vlnv xilinx.com:ip:v_tpg:7.0 v_tpg_0
	set_property -dict [list CONFIG.SAMPLES_PER_CLOCK {2} CONFIG.MAX_COLS {3840} CONFIG.HAS_AXI4S_SLAVE {1}] [get_bd_cells v_tpg_0]

	connect_bd_intf_net [get_bd_intf_pins ctrl] [get_bd_intf_pins v_tpg_0/s_axi_CTRL]
	connect_bd_net [get_bd_pins m_axi_s2mm_aclk] [get_bd_pins v_tpg_0/ap_clk]
	connect_bd_intf_net [get_bd_intf_pins v_vid_in_axi4s_0/video_out] [get_bd_intf_pins v_tpg_0/s_axis_video]

	create_bd_cell -type ip -vlnv xilinx.com:ip:xlslice:1.0 xlslice_0
	set_property -dict [list CONFIG.DIN_WIDTH {95} CONFIG.DIN_TO {0} CONFIG.DIN_FROM {0}] [get_bd_cells xlslice_0]

	connect_bd_net [get_bd_pins Din] [get_bd_pins xlslice_0/Din]
	connect_bd_net [get_bd_pins xlslice_0/Dout] [get_bd_pins v_tpg_0/ap_rst_n]

	create_bd_cell -type ip -vlnv xilinx.com:ip:v_frmbuf_wr:2.1 v_frmbuf_wr_0
	set_property -dict [list CONFIG.HAS_YUYV8 {1} CONFIG.HAS_RGB8 {0} CONFIG.HAS_Y8 {1} CONFIG.HAS_UYVY8 {1}] [get_bd_cells v_frmbuf_wr_0]

	connect_bd_intf_net [get_bd_intf_pins v_tpg_0/m_axis_video] [get_bd_intf_pins v_frmbuf_wr_0/s_axis_video]


	connect_bd_intf_net [get_bd_intf_pins s_axi_ctrl] [get_bd_intf_pins v_frmbuf_wr_0/s_axi_CTRL]
	connect_bd_net [get_bd_pins m_axi_s2mm_aclk] [get_bd_pins v_frmbuf_wr_0/ap_clk]

	create_bd_cell -type ip -vlnv xilinx.com:ip:xlslice:1.0 xlslice_1
	set_property -dict [list CONFIG.DIN_TO {1} CONFIG.DIN_FROM {1} CONFIG.DIN_WIDTH {95} CONFIG.DIN_FROM {1} CONFIG.DOUT_WIDTH {1}] [get_bd_cells xlslice_1]

	connect_bd_net [get_bd_pins Din] [get_bd_pins xlslice_1/Din]
	connect_bd_net [get_bd_pins xlslice_1/Dout] [get_bd_pins v_frmbuf_wr_0/ap_rst_n]

	connect_bd_intf_net [get_bd_intf_pins M_AXI_S2MM] [get_bd_intf_pins v_frmbuf_wr_0/m_axi_mm_video]
	connect_bd_net [get_bd_pins s2mm_introut] [get_bd_pins v_frmbuf_wr_0/interrupt]


###############################################################################
    # Restore current instance
    current_bd_instance $oldCurInst
}

