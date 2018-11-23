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

/*
 * helloworld.c: simple test application
 *
 * This application configures UART 16550 to baud rate 9600.
 * PS7 UART (Zynq) is not initialized by this application, since
 * bootrom/bsp configures it to baud rate 115200
 *
 * ------------------------------------------------
 * | UART TYPE   BAUD RATE                        |
 * ------------------------------------------------
 *   uartns550   9600
 *   uartlite    Configurable only in HW design
 *   ps7_uart    115200 (configured by bootrom/bsp)
 */

#include <stdio.h>
#include "platform.h"
#include "perfapm.h"
#include "xil_cache.h"
#include "sleep.h"

void disable_caches();
void print_counters(void)
{
	int k, tempcnt = 0;
	char answer;

	while (1) {
		xil_printf("Turn on traffic generator? Enter 'Y' or 'N': ");
		answer = inbyte();
		if (answer == 'Y' || answer == 'N' || answer == 'y' || answer == 'n')
			break;
		xil_printf("\r\n");
	}

	xil_printf("\033c");
	while (1)
	{
		xil_printf("\033[?1049h\033[H");
		xil_printf("|----------------------------------------------------------------------|\r\n");
		xil_printf("|                      Performance Monitor APP                         |\r\n");
		xil_printf("|----------------------------------------------------------------------|\r\n");
		xil_printf("|Slot                |Write Byte Cnt |Read Byte Cnt |Total RW Byte Cnt |\r\n");
		xil_printf("|----------------------------------------------------------------------|\r\n");

		xil_printf("|DDR Slot1           |%14d |%13d |%17d | \r\n",
				perfmon_get_countervalue(PS_DDRAPM_SLOT_1,2, PERF_SAMPLE_INTERVAL_COUNTER),
				perfmon_get_countervalue(PS_DDRAPM_SLOT_1,3, PERF_SAMPLE_INTERVAL_COUNTER),
				perfmon_get_rd_wr_cnt(PS_DDRAPM_SLOT_1, PERF_SAMPLE_INTERVAL_COUNTER));
		xil_printf("|DDR Slot2           |%14d |%13d |%17d | \r\n",
				perfmon_get_countervalue(PS_DDRAPM_SLOT_2,4, PERF_SAMPLE_INTERVAL_COUNTER),
				perfmon_get_countervalue(PS_DDRAPM_SLOT_2,5, PERF_SAMPLE_INTERVAL_COUNTER),
				perfmon_get_rd_wr_cnt(PS_DDRAPM_SLOT_2, PERF_SAMPLE_INTERVAL_COUNTER));
		xil_printf("|DDR Slot3           |%14d |%13d |%17d | \r\n",
				perfmon_get_countervalue(PS_DDRAPM_SLOT_3,6, PERF_SAMPLE_INTERVAL_COUNTER),
				perfmon_get_countervalue(PS_DDRAPM_SLOT_3,7, PERF_SAMPLE_INTERVAL_COUNTER),
				perfmon_get_rd_wr_cnt(PS_DDRAPM_SLOT_3, PERF_SAMPLE_INTERVAL_COUNTER));
		xil_printf("|DDR Slot4           |%14d |%13d |%17d | \r\n",
				perfmon_get_countervalue(PS_DDRAPM_SLOT_4,8, PERF_SAMPLE_INTERVAL_COUNTER),
				perfmon_get_countervalue(PS_DDRAPM_SLOT_4,9, PERF_SAMPLE_INTERVAL_COUNTER),
				perfmon_get_rd_wr_cnt(PS_DDRAPM_SLOT_4, PERF_SAMPLE_INTERVAL_COUNTER));
		xil_printf("|DDR Slot5           |%14d |%13d |%17d | \r\n",
				perfmon_get_countervalue(PS_DDRAPM_SLOT_5,0, PERF_SAMPLE_INTERVAL_COUNTER),
				perfmon_get_countervalue(PS_DDRAPM_SLOT_5,1, PERF_SAMPLE_INTERVAL_COUNTER),
				perfmon_get_rd_wr_cnt(PS_DDRAPM_SLOT_5, PERF_SAMPLE_INTERVAL_COUNTER));
		xil_printf("|OCM APM             |%14d |%13d |%17d | \r\n",
				perfmon_get_countervalue(PS_OCMAPM_SLOT_0,2, PERF_SAMPLE_INTERVAL_COUNTER),
				perfmon_get_countervalue(PS_OCMAPM_SLOT_0,3, PERF_SAMPLE_INTERVAL_COUNTER),
				perfmon_get_rd_wr_cnt(PS_OCMAPM_SLOT_0, PERF_SAMPLE_INTERVAL_COUNTER));
		xil_printf("|LPD_FPD             |%14d |%13d |%17d | \r\n",
				perfmon_get_countervalue(PS_LPDAPM_SLOT_0,2, PERF_SAMPLE_INTERVAL_COUNTER),
				perfmon_get_countervalue(PS_LPDAPM_SLOT_0,3, PERF_SAMPLE_INTERVAL_COUNTER),
				perfmon_get_rd_wr_cnt(PS_LPDAPM_SLOT_0, PERF_SAMPLE_INTERVAL_COUNTER));
		/*xil_printf("|CCI_Sw    |%8d \t|%8d \t|%8d \t| \n",
		  XAxiPmon_GetMetricCounter(&CCI_CoreSw_PmonInst,0, PERF_SAMPLE_INTERVAL_COUNTER),
		  XAxiPmon_GetMetricCounter(&CCI_CoreSw_PmonInst,1, PERF_SAMPLE_INTERVAL_COUNTER));*/
		xil_printf("|----------------------------------------------------------------------|\r\n");

		perfmon_reset_countervalue(PS_DDRAPM_SLOT_1, PERF_SAMPLE_INTERVAL_COUNTER);
		perfmon_reset_countervalue(PS_DDRAPM_SLOT_2, PERF_SAMPLE_INTERVAL_COUNTER);
		perfmon_reset_countervalue(PS_DDRAPM_SLOT_3, PERF_SAMPLE_INTERVAL_COUNTER);
		perfmon_reset_countervalue(PS_DDRAPM_SLOT_4, PERF_SAMPLE_INTERVAL_COUNTER);
		perfmon_reset_countervalue(PS_DDRAPM_SLOT_5, PERF_SAMPLE_INTERVAL_COUNTER);
		perfmon_reset_countervalue(PS_OCMAPM_SLOT_0, PERF_SAMPLE_INTERVAL_COUNTER);
		perfmon_reset_countervalue(PS_LPDAPM_SLOT_0, PERF_SAMPLE_INTERVAL_COUNTER);

		if (answer == 'Y' || answer == 'y') {
			disable_caches();
			for (k = 0; k < tempcnt; k++) {
				Xil_In32 (XPAR_PSU_OCM_RAM_0_S_AXI_BASEADDR +  k*4); //OCM_ADDR
				Xil_In32 (XPAR_PSU_R5_DDR_0_S_AXI_BASEADDR  +  k*4); //DDR_ADDR
			}
			tempcnt += 128;
			if (tempcnt > 1280)
				tempcnt = 0;
		}

		sleep(1);
		xil_printf("\033[?1049l");
	}
}

int main()
{
	int Status;

	init_platform();

	Status = perfmon_init_all(PERF_BAREMATEL_OS, PERF_SAMPLE_INTERVAL_COUNTER);
	if (Status != XST_SUCCESS) {
		xil_printf("AXI Performance Monitor Polled example failed\r\n");
		return XST_FAILURE;
	}

	print_counters();
	
	Status = perfmon_deinit_all(PERF_BAREMATEL_OS, PERF_SAMPLE_INTERVAL_COUNTER);
	if (Status != XST_SUCCESS) {
		xil_printf("AXI Performance Monitor Polled deinit failed\r\n");
		return XST_FAILURE;
	}
	xil_printf("AXI Performance Monitor Polled deinitexample passed\r\n");

	cleanup_platform();
	return 0;
}
