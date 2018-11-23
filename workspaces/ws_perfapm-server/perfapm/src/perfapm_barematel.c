/******************************************************************************
 *
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
 *
 ******************************************************************************/
/****************************************************************************/
/**
 *
 * @file perfmon_barematel.c
 * @addtogroup perfapm
 * @{
 *
 * This file contains operating system specifi library API functions that can be used to access
 * the Performance Monitor devices in MPSoC.
 *
 * Refer to the perfmon.h header file for more information about this library.
 *
 * @note
 * This library uses low level AXI Performance Monitor (APM) library APIs
 *
 * <pre>
 *
 * MODIFICATION HISTORY:
 *
 * Ver   Who    Date     Changes
 * ----- -----  -------- -----------------------------------------------------
 * 1.00a preetesh   12/23/15  First release
 * </pre>
 *
 *****************************************************************************/

/***************************** Include Files ********************************/
#include "perfapm.h"
/************************** Constant Definitions *****************************/
static int g_ostype = PERF_BAREMATEL_OS;

/*****************************************************************************/
/**
 *
 * This function initializes barematel specific routines for XAxiPmon device/instance.
 *
 * @param	ostype is an enum to type of operating system
 *
 * @return
 *		- XST_SUCCESS if successful.
 *
 * @note	None.
 *
 ******************************************************************************/
int perfmon_preinit(Perf_OS_Type ostype)
{
	int ddr_reset_regoff = 0xFD1A0000 + 0x108;
	g_ostype = ostype;

	//xil_printf("R5-%d %s: checkpoint 1\r\n", XPAR_CPU_ID, __func__);
	if (ostype == PERF_BAREMATEL_OS) {
		Xil_In32(ddr_reset_regoff);
		Xil_Out32(ddr_reset_regoff, 0x0);
		Xil_In32(ddr_reset_regoff);
	}

	return EXIT_SUCCESS;
}

/*****************************************************************************/
/**
 *
 * This function deinitializes barematel specific routines for XAxiPmon device/instance.
 *
 * @param	ostype is an enum to type of operating system
 *
 * @return
 *		- XST_SUCCESS if successful.
 *
 * @note	None.
 *
 ******************************************************************************/
int perfmon_postdeinit(Perf_OS_Type ostype)
{
	int ret = 0;

	if (ostype == PERF_BAREMATEL_OS) {
	}
	return ret;
}
