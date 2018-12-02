/******************************************************************************
 * (c) Copyright 2012-2018 Xilinx, Inc. All rights reserved.
 *
 * This file contains confidential and proprietary information of Xilinx, Inc.
 * and is protected under U.S. and international copyright and other
 * intellectual property laws.
 *
 * DISCLAIMER
 * This disclaimer is not a license and does not grant any rights to the
 * materials distributed herewith. Except as otherwise provided in a valid
 * license issued to you by Xilinx, and to the maximum extent permitted by
 * applicable law: (1) THESE MATERIALS ARE MADE AVAILABLE "AS IS" AND WITH ALL
 * FAULTS, AND XILINX HEREBY DISCLAIMS ALL WARRANTIES AND CONDITIONS, EXPRESS,
 * IMPLIED, OR STATUTORY, INCLUDING BUT NOT LIMITED TO WARRANTIES OF
 * MERCHANTABILITY, NON-INFRINGEMENT, OR FITNESS FOR ANY PARTICULAR PURPOSE;
 * and (2) Xilinx shall not be liable (whether in contract or tort, including
 * negligence, or under any other theory of liability) for any loss or damage
 * of any kind or nature related to, arising under or in connection with these
 * materials, including for any direct, or any indirect, special, incidental,
 * or consequential loss or damage (including loss of data, profits, goodwill,
 * or any type of loss or damage suffered as a result of any action brought by
 * a third party) even if such damage or loss was reasonably foreseeable or
 * Xilinx had been advised of the possibility of the same.
 *
 * CRITICAL APPLICATIONS
 * Xilinx products are not designed or intended to be fail-safe, or for use in
 * any application requiring fail-safe performance, such as life-support or
 * safety devices or systems, Class III medical devices, nuclear facilities,
 * applications related to the deployment of airbags, or any other applications
 * that could lead to death, personal injury, or severe property or
 * environmental damage (individually and collectively, "Critical
 * Applications"). Customer assumes the sole risk and liability of any use of
 * Xilinx products in Critical Applications, subject only to applicable laws
 * and regulations governing limitations on product liability.
 *
 * THIS COPYRIGHT NOTICE AND DISCLAIMER MUST BE RETAINED AS PART OF THIS FILE
 * AT ALL TIMES.
 *******************************************************************************/

#ifndef VLIB_GST_H
#define VLIB_GST_H

#ifdef __cplusplus
extern "C"
{
#endif

#ifdef GST_MODE
	#if defined (SAMPLE_FILTER2D)
	#include "vgst_sdxfilter2d.h"
	#endif
	#define vlib_init			vlib_init_gst
	#define vlib_pipeline_stop		vlib_pipeline_stop_gst
	#define vlib_uninit			vlib_uninit_gst
	#define vlib_change_mode		vlib_change_mode_gst
	#define optical_flow_create		optical_flow_create_gst
	#define filter2d_create			filter2d_create_gst
#else
	#if defined (SAMPLE_FILTER2D)
	# include "filter2d.h"
	#endif
	#if defined (SAMPLE_OPTICAL_FLOW)
	# include "optical_flow.h"
	#endif
#endif

/* init/uninit functions for gstreamer library */
int vlib_init_gst(struct vlib_config_data *cfg);
int vlib_uninit_gst(void);

/* video pipeline control functions for gstreamer library */
int vlib_pipeline_stop_gst(void);
int vlib_change_mode_gst(struct vlib_config *config);

/* Accelerator helper functions for gstreamer library */
struct filter_s *filter2d_create_gst(void);
struct filter_s *optical_flow_create_gst(void);

#ifdef __cplusplus
}
#endif

#endif /* VLIB_GST_H */
