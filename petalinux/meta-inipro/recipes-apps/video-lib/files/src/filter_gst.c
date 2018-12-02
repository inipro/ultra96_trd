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
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>

#include "filter.h"
#include "helper.h"
#include "video_int.h"

static const char *f2d_modes_gst[] = {
	"HW",
	"SW",
};

const static struct filter_s f2d_FS_gst = {
	.display_text = "2D Filter",
	.dt_comp_string = "sdxfilter2d",
	.pr_file_name = "",
	.pr_buf = NULL,
	.fd = -1,
	.mode = 0,
	.ops = NULL,
	.data = NULL,
	.num_modes = ARRAY_SIZE(f2d_modes_gst),
	.modes = f2d_modes_gst,
};

struct filter_s *filter2d_create_gst(void)
{
	struct filter_s *fs = (struct filter_s *) (malloc(sizeof *fs));
	*fs = f2d_FS_gst;
	return fs;
}

static const char *of_modes_gst[] = {
	"HW",
};

const static struct filter_s of_FS_gst = {
	.display_text = "Optical Flow",
	.dt_comp_string = "sdxopticalflow",
	.pr_file_name = "",
	.pr_buf = NULL,
	.fd = -1,
	.mode = 0,
	.ops = NULL,
	.data = NULL,
	.num_modes = ARRAY_SIZE(of_modes_gst),
	.modes = of_modes_gst,
};

struct filter_s *optical_flow_create_gst(void)
{
	struct filter_s *fs = (struct filter_s *) (malloc(sizeof *fs));
	*fs = of_FS_gst;
	return fs;
}
