/******************************************************************************
 * (c) Copyright 2012-2016 Xilinx, Inc. All rights reserved.
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

#include <helper.h>
#include "video_cfg.h"
#include <QString>
struct vplay_mode {
	const char *name;
	const char *short_name;
	bool has_panel;
};

struct v_source {
	video_src src;
	const char *short_name;
	bool has_panel;
	const struct vlib_vdev *vd;
};

struct v_filter {
	const char *name;
	const char *short_name;
	bool has_panel;
};

static struct vplay_mode playmode_tbl[] = {
	{"Manual", "Manual", true},
	{"Demo", "Demo", true},
};

struct vplay_mode* get_vplaymode_by_name(const char *name) {
	for (unsigned int i = 0; i < ARRAY_SIZE(playmode_tbl); i++) {
		if ((QString :: compare(QString(name), QString(playmode_tbl[i].name), Qt::CaseInsensitive)) == 0) {
			return &playmode_tbl[i];
		}
	}
	return nullptr;
}

static struct v_source vsrc_tbl[] = {
	{VLIB_VCLASS_VIVID, "TPG (SW)", false, NULL},
	{VLIB_VCLASS_UVC, "USB", false, NULL},
	{VLIB_VCLASS_TPG, "TPG (PL)", true, NULL},
//	{VLIB_VCLASS_HDMII, "HDMI", false, NULL},
	{VLIB_VCLASS_CSI, "CSI", true, NULL},
	{VLIB_VCLASS_FILE, "FILE", true, NULL},
};

static struct v_filter filter_tbl[] = {
	{"2D Filter", "2D Filter", true},
	{"Simple Posterize", "Simple Posterize", false},
	{"Optical Flow", "Optical Flow", false},
};

static struct v_source* get_vsrc_by_id(video_src id) {
	for (unsigned int i = 0; i < ARRAY_SIZE(vsrc_tbl); i++) {
		if (id == vsrc_tbl[i].src) {
			return &vsrc_tbl[i];
		}
	}
	return nullptr;
}

bool vsrc_get_has_panel(video_src id) {
	return get_vsrc_by_id(id)->has_panel;
}

const char* vsrc_get_short_name(video_src id) {
	return get_vsrc_by_id(id)->short_name;
}

struct v_filter* get_vfilter_by_name(const char *name) {
	for (unsigned int i = 0; i < ARRAY_SIZE(filter_tbl); i++) {
		if ((QString :: compare(QString(name), QString(filter_tbl[i].name), Qt::CaseInsensitive)) == 0) {
			return &filter_tbl[i];
		}
	}
	return nullptr;
}

bool vfilter_get_has_panel(const char *name) {
	return get_vfilter_by_name(name)->has_panel;
}

const char* vfilter_get_short_name(const char *name) {
	return get_vfilter_by_name(name)->short_name;
}

bool vplaymode_get_has_panel(const char *name) {
	return get_vplaymode_by_name(name)->has_panel;
}

const char* vplaymode_get_short_name(const char *name) {
	return get_vplaymode_by_name(name)->short_name;
}

const struct vlib_vdev *vsrc_get_vd(video_src id)
{
	return get_vsrc_by_id(id)->vd;
}

void vsrc_set_vd(video_src vsrc, const struct vlib_vdev *vd)
{
	struct v_source *vs = get_vsrc_by_id(vsrc);
	if (!vs) {
		return;
	}

	vs->vd = vd;
}
