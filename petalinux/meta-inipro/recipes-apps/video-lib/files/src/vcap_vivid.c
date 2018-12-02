/******************************************************************************
 * (c) Copyright 2016 Xilinx, Inc. All rights reserved.
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

#include <v4l2_helper.h>
#include <video_int.h>
#include <sys/ioctl.h>

#define VIVID_CTRL_TEST_PATTERN	0xf0f000
#define VIVID_CTRL_HOR_MOVEMENT	0xf0f002
#define VIVID_CTRL_VER_MOVEMENT	0xf0f003

struct vlib_vdev *vcap_vivid_init(const struct matchtable *mte, void *data)
{
	int ret;
	int fd = (uintptr_t)data;

	struct v4l2_capability vcap;
	ret = ioctl(fd, VIDIOC_QUERYCAP, &vcap);
	if (ret) {
		return NULL;
	}

	if (!(vcap.capabilities & V4L2_CAP_DEVICE_CAPS)) {
		return NULL;
	}

	if (!(vcap.device_caps & V4L2_CAP_VIDEO_CAPTURE)) {
		return NULL;
	}

	if (!(vcap.device_caps & V4L2_CAP_STREAMING)) {
		return NULL;
	}

	struct vlib_vdev *vd = calloc(1, sizeof(*vd));
	if (!vd) {
		return NULL;
	}

	vd->vsrc_type = VSRC_TYPE_V4L2;
	vd->data.v4l2.vnode = fd;
	vd->vsrc_class = VLIB_VCLASS_VIVID;
	vd->display_text = "Virtual Video Device";
	vd->entity_name = mte->s;

	/* set default test pattern */
	struct v4l2_ext_control ctrl[3];
	memset(&ctrl, 0, sizeof(ctrl));
	/* test pattern: color squares */
	ctrl[0].id = VIVID_CTRL_TEST_PATTERN;
	ctrl[0].value = 4;
	/* horizontal movement: fast right */
	ctrl[1].id = VIVID_CTRL_HOR_MOVEMENT;
	ctrl[1].value = 6;
	/* vertical movement: fast down */
	ctrl[2].id = VIVID_CTRL_VER_MOVEMENT;
	ctrl[2].value = 6;

	struct v4l2_ext_controls ctrls;
	memset(&ctrls, 0, sizeof(ctrls));
	ctrls.ctrl_class = V4L2_CTRL_CLASS_USER;
	ctrls.which = V4L2_CTRL_WHICH_CUR_VAL;
	ctrls.count = ARRAY_SIZE(ctrl);
	ctrls.controls = ctrl;

	ret = ioctl(fd, VIDIOC_S_EXT_CTRLS, &ctrls);
	ASSERT2(!ret, "failed to set vivid controls: %s", strerror(errno));

	return vd;
}
