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
#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>

#include "helper.h"
#include "video_int.h"

/*
 * We control the platform and can hence hardcode the path to the UIO
 * node. Nevertheless, it would be more elegant to discover the correct
 * node and parameters from /sys/class/uio/
 */
#define DDR_QOS_MAP_FILE	"/dev/uio1"
#define AFIFM_HP0_MAP_FILE	"/dev/uio4"
#define AFIFM_HP1_MAP_FILE	"/dev/uio5"
#define AFIFM_HP2_MAP_FILE	"/dev/uio6"
#define AFIFM_HP3_MAP_FILE	"/dev/uio7"


struct register_data {
	uintptr_t offs;
	uint32_t val;
};

#define DDR_QOS_PORT_TYPE			0
#define DDR_QOS_PORT_TYPE_BEST_EFFORT		0
#define DDR_QOS_PORT_TYPE_LOW_LATENCY		1
#define DDR_QOS_PORT_TYPE_VIDEO			2
#define DDR_QOS_PORT_TYPE_PORT0_TYPE_SHIFT	0
#define DDR_QOS_PORT_TYPE_PORT1R_TYPE_SHIFT	2
#define DDR_QOS_PORT_TYPE_PORT2R_TYPE_SHIFT	6
#define DDR_QOS_PORT_TYPE_PORT3_TYPE_SHIFT	10
#define DDR_QOS_PORT_TYPE_PORT4_TYPE_SHIFT	12
#define DDR_QOS_PORT_TYPE_PORT5_TYPE_SHIFT	14
static const struct register_data qos_data_dp_ddr[] = {
	{ .offs = DDR_QOS_PORT_TYPE,
	  .val = DDR_QOS_PORT_TYPE_LOW_LATENCY << DDR_QOS_PORT_TYPE_PORT0_TYPE_SHIFT |
		 DDR_QOS_PORT_TYPE_LOW_LATENCY << DDR_QOS_PORT_TYPE_PORT1R_TYPE_SHIFT |
		 DDR_QOS_PORT_TYPE_LOW_LATENCY << DDR_QOS_PORT_TYPE_PORT2R_TYPE_SHIFT |
		 DDR_QOS_PORT_TYPE_VIDEO << DDR_QOS_PORT_TYPE_PORT3_TYPE_SHIFT |
		 DDR_QOS_PORT_TYPE_BEST_EFFORT << DDR_QOS_PORT_TYPE_PORT4_TYPE_SHIFT |
		 DDR_QOS_PORT_TYPE_BEST_EFFORT << DDR_QOS_PORT_TYPE_PORT5_TYPE_SHIFT, },
};

#define AFIFM_RDQOS	8
#define AFIFM_WRQOS	0x1c
static const struct register_data qos_data_afifmhp1[] = {
	{ .offs = AFIFM_WRQOS, .val = 0, },
};

static const struct register_data qos_data_afifmhp2[] = {
	{ .offs = AFIFM_RDQOS, .val = 0, },
	{ .offs = AFIFM_WRQOS, .val = 0, },
};

static const struct register_data qos_data_afifmhp3[] = {
	{ .offs = AFIFM_RDQOS, .val = 0, },
	{ .offs = AFIFM_WRQOS, .val = 0, },
};

static const struct register_data qos_data_dp_afifmhp0[] = {
	{ .offs = AFIFM_RDQOS, .val = 0x7, },
};

struct register_init_data {
	const char *fn;
	size_t sz;
	const struct register_data *data;
};

#define DEFINE_RINIT_DATA(_fn, _arry)	{ .fn = _fn, .sz = ARRAY_SIZE(_arry), .data = _arry, }
static const struct register_init_data qos_settings_dp[] = {
	DEFINE_RINIT_DATA(DDR_QOS_MAP_FILE, qos_data_dp_ddr),
	DEFINE_RINIT_DATA(AFIFM_HP0_MAP_FILE, qos_data_dp_afifmhp0),
	DEFINE_RINIT_DATA(AFIFM_HP1_MAP_FILE, qos_data_afifmhp1),
	DEFINE_RINIT_DATA(AFIFM_HP2_MAP_FILE, qos_data_afifmhp2),
	DEFINE_RINIT_DATA(AFIFM_HP3_MAP_FILE, qos_data_afifmhp3),
};

struct register_init {
	const struct register_init_data *data;
	size_t sz;
};

static const struct register_init qos_settings[] = {
	{ .data = qos_settings_dp, .sz = ARRAY_SIZE(qos_settings_dp) },
};

int vlib_platform_set_qos(size_t qos_setting)
{
	int ret = 0;

	vlib_info("set platform QoS for %s\n", qos_setting ? "HDMI Tx" : "DP Tx");

	const struct register_init *r = &qos_settings[qos_setting];
	ASSERT2(r, "invalid device\n");

	for (size_t i = 0; i < r->sz; i++) {
		const struct register_init_data *rb = &r->data[i];

		int fd = open(rb->fn, O_RDWR);
		if (fd == -1) {
			VLIB_REPORT_ERR("failed to open file '%s': %s",
					rb->fn, strerror(errno));
			vlib_dbg("%s\n", vlib_errstr);
			continue;
		}

		uint32_t *map = mmap(NULL, 0x1000, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
		if (map == MAP_FAILED) {
			VLIB_REPORT_ERR("failed to mmap file '%s': %s",
					rb->fn, strerror(errno));
			vlib_dbg("%s\n", vlib_errstr);
			ret = VLIB_ERROR_FILE_IO;
			goto err_close;
		}

		for (size_t j = 0; j < rb->sz; j++) {
			const struct register_data *rd = &rb->data[j];

			map[rd->offs / sizeof(uint32_t)] = rd->val;
		}

		munmap(map, 0x1000);
err_close:
		close(fd);
	}

	return ret;
}
