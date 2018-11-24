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
 *******************************************************************************/

/*
 * Unit test application running on A53 Linux to demo use 
 * of perfapm-oa-client library
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <time.h>
#include <fcntl.h>
#include <string.h>
#include "perfapm-client.h"

int main(int argc, char *argv[])
{
	int Status = EXIT_SUCCESS;
	int i;
	Perf_OA_Payload *openamp_payload = NULL;
	FILE *sysfs_fptr;

	sysfs_fptr = fopen("/sys/class/remoteproc/remoteproc0/firmware", "w");
	if (sysfs_fptr == NULL)
	{
		perror("Failed to open remoteproc firmware file");
		return EXIT_FAIL;
	}
	fputs("perfapm-server.elf", sysfs_fptr);
	fclose(sysfs_fptr);

	Status = perfoacl_init_all(PERF_LINUX_OS, PERF_SAMPLE_INTERVAL_COUNTER);
	if (Status != EXIT_SUCCESS) {
		printf("init failed\n");
		return Status;
	}

	/*
	 * Read 14 counters from perfoacl_get_rd_wr_cnt API. Each counter is 64 bit in size.
	 * counters 0 to 9 belongs to DDR APM slots. These counters are write and read byte counts.
	 * DDR APM has total 5 slots. 
	 * counters 10 and 11 belongs to OCM APM slot. These counters are write and read byte counts.
	 * counters 12 and 13 belongs to LPD APM slot. These counters are write and read byte counts.
	 */

	printf("\033c");
	for (i = 0; i < 500 ; i++) {
		printf("\033[?1049h\033[H");
		openamp_payload =  perfoacl_get_rd_wr_cnt(MAX_PERF_SLOTS, PERF_SAMPLE_INTERVAL_COUNTER);
		if (openamp_payload != NULL) {
			printf("|----------------------------------------------------------------------|\r\n");
			printf("|                      Performance Monitor APP                         |\r\n");
			printf("|----------------------------------------------------------------------|\r\n");
			printf("|Slot                |Write Byte Cnt |Read Byte Cnt |Total RW Byte Cnt |\r\n");
			printf("|----------------------------------------------------------------------|\r\n");

			printf("|DDR Slot1           |%14ld |%13ld |%17ld | \r\n",
					openamp_payload->readcnt[0],
					openamp_payload->readcnt[1],
					openamp_payload->readcnt[0] + openamp_payload->readcnt[1]);
			printf("|DDR Slot2           |%14ld |%13ld |%17ld | \r\n",
					openamp_payload->readcnt[2],
					openamp_payload->readcnt[3],
					openamp_payload->readcnt[2] + openamp_payload->readcnt[3]);
			printf("|DDR Slot3           |%14ld |%13ld |%17ld | \r\n",		/* values for DP and HP0 */
					openamp_payload->readcnt[4],
					openamp_payload->readcnt[5],
					openamp_payload->readcnt[4] + openamp_payload->readcnt[5]);
			printf("|DDR Slot4           |%14ld |%13ld |%17ld | \r\n",		/* values for HP1 and HP2 */
					openamp_payload->readcnt[6],
					openamp_payload->readcnt[7],
					openamp_payload->readcnt[6] + openamp_payload->readcnt[7]);
			printf("|DDR Slot5           |%14ld |%13ld |%17ld | \r\n",		/* values for HP3 and FPDDMA */
					openamp_payload->readcnt[8],
					openamp_payload->readcnt[9],
					openamp_payload->readcnt[8] + openamp_payload->readcnt[9]);
			printf("|OCM APM             |%14ld |%13ld |%17ld | \r\n",
					openamp_payload->readcnt[10],
					openamp_payload->readcnt[11],
					openamp_payload->readcnt[10] + openamp_payload->readcnt[11]);
			printf("|LPD_FPD             |%14ld |%13ld |%17ld | \r\n",
					openamp_payload->readcnt[12],
					openamp_payload->readcnt[13],
					openamp_payload->readcnt[12] + openamp_payload->readcnt[13]);
			printf("|----------------------------------------------------------------------|\r\n");

			printf("DDRAPM_SLOT_DP+HP0     throughput = %f gigabits/sec\r\n", (float) ((openamp_payload->readcnt[4] + openamp_payload->readcnt[5]) * 8 / 1000000000.0));
			printf("DDRAPM_SLOT_HP1+HP2    throughput = %f gigabits/sec\r\n", (float) ((openamp_payload->readcnt[6] + openamp_payload->readcnt[7]) * 8 / 1000000000.0));
			printf("DDRAPM_SLOT_HP3+FPDDMA throughput = %f gigabits/sec\r\n", (float) ((openamp_payload->readcnt[8] + openamp_payload->readcnt[9]) * 8 / 1000000000.0));
		}
		sleep(1);
		printf("\033[?1049l");
	}

	Status = perfoacl_deinit_all(PERF_LINUX_OS, PERF_SAMPLE_INTERVAL_COUNTER);
	return Status;
}
