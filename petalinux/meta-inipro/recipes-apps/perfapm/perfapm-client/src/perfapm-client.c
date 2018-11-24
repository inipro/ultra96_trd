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

/****************************************************************************/
/**
 *
 * @file perfmon-openamp-client.c
 * @addtogroup perfapm
 * @{
 *
 * This file contains the library API functions that can be used to access
 * the Performance Monitor devices in MPSoC via OpenAMP client. OpenAMP is
 * used as a communication protocol between A53 and R5 cores in MPSoC
 *
 * Refer to the perfmon-openamp-client.h header file for more information about this library.
 *
 * @note
 * There are three modes : Advanced, Profile and Trace. APM (hardned in PS)
 * supports only Advanced mode. 
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
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <perfapm-client.h>
/************************** Constant Definitions *****************************/
#define RPMSG_DEV	"/dev/rpmsg0"
#define SYS_REMOTEPROC_FILE	"/sys/class/remoteproc/remoteproc0/state"
#define RPMSG_HEADER_LEN 16
#define MAX_RPMSG_BUFF_SIZE (512 - RPMSG_HEADER_LEN)
#define PAYLOAD_MAX_SIZE	(MAX_RPMSG_BUFF_SIZE - 24)

static int openamp_fd;
static Perf_OA_Payload *i_payload;
static Perf_OA_Payload *r_payload;
/**************************** Type Definitions *******************************/

/***************** Macros (Inline Functions) Definitions *********************/

/************************** Function Prototypes ******************************/

/************************** Variable Definitions *****************************/

/*****************************************************************************/
/**
 *
 * This function provides read/write counter value for a specific XAxiPmon device/instance.
 *
 * @param	apm_slot is an enum to the XAxiPmon instance monitor ports.
 * @param	cnttype is an enum to type of performance counter
 *
 * @return
 *		- pointer of type struct Perf_OA_Payload if successful.
 *		- NULL pointer if failed
 *
 * @note	None.
 *
 ******************************************************************************/
Perf_OA_Payload* perfoacl_get_rd_wr_cnt(Perf_APM_Slot apm_slot, Perf_Cnt_Type cnttype)
{
	int bytes_rcvd, bytes_sent;

	i_payload->num = r_payload->num = 0;
	i_payload->size = sizeof(i_payload->readcnt);

	bytes_sent = write(openamp_fd, i_payload, sizeof(Perf_OA_Payload));
	if (bytes_sent <= 0) {
		perror("\r\n Error sending data");
		return NULL;
	}

	bytes_rcvd = read(openamp_fd, r_payload, (2 * sizeof(unsigned long)) + PAYLOAD_MAX_SIZE);
	while (bytes_rcvd <= 0) {
		usleep(10000);
		bytes_rcvd = read(openamp_fd, r_payload,
				(2 * sizeof(unsigned long)) + PAYLOAD_MAX_SIZE);
	}

	return r_payload;
}

/*****************************************************************************/
/**
 *
 * This function resets metrics counters for a specific XAxiPmon device/instance.
 *
 * @param	apm_slot is an enum to the XAxiPmon instance monitor ports.
 * @param	cnttype is an enum to type of performance counter
 *
 * @return
 *		- EXIT_SUCCESS if successful.
 *
 * @note	None.
 *
 ******************************************************************************/
int perfoacl_reset_countervalue(Perf_APM_Slot apm_slot, Perf_Cnt_Type cnttype)
{
	int Status = EXIT_SUCCESS;
	return Status;
}


/*****************************************************************************/
/**
 *
 * This function disables all available XAxiPmon device/instance.
 *
 * @param	ostype is an enum to type of operating system
 * @param	cnttype is an enum to type of performance counter
 *
 * @return
 *		- EXIT_SUCCESS if successful.
 *
 * @note	None.
 *
 ******************************************************************************/
int perfoacl_deinit_all(Perf_OS_Type ostype, Perf_Cnt_Type cnttype)
{
	int Status = EXIT_SUCCESS;
	FILE *sysfs_fptr;

	free(i_payload);
	free(r_payload);
	close(openamp_fd);

	/*
	 * close the channel using sysfs interface
	 */
	sysfs_fptr = fopen(SYS_REMOTEPROC_FILE, "w");
	if(sysfs_fptr)
	{
		fputs("stop", sysfs_fptr);
		fclose(sysfs_fptr);
	}

	return Status;
}


/*****************************************************************************/
/**
 *
 * This function initializes all available XAxiPmon device/instance.
 *
 * @param	ostype is an enum to type of operating system
 * @param	cnttype is an enum to type of performance counter
 *
 * @return
 *		- EXIT_SUCCESS if successful.
 *
 * @note	None.
 *
 ******************************************************************************/
int perfoacl_init_all(Perf_OS_Type ostype, Perf_Cnt_Type cnttype)
{
	int Status = EXIT_SUCCESS;
	FILE *sysfs_fptr;

	/*
	 * open the channel using sysfs interface
	 */
	sysfs_fptr = fopen(SYS_REMOTEPROC_FILE, "w");
	if(sysfs_fptr == NULL)
	{
		perror("Failed to open remoteproc sysfs file");
		return EXIT_FAIL;
	}
	fputs("stop", sysfs_fptr);
	fflush(sysfs_fptr);
	fputs("start", sysfs_fptr);
	fclose(sysfs_fptr);

	/*
	 * Wait for remoteproc to load the RPU firmware
	 */
	sleep(1);

	openamp_fd = open(RPMSG_DEV, O_RDWR | O_NONBLOCK);
	if (openamp_fd < 0) {
		perror("Failed to open rpmsg file /dev/rpmsg0.");
		return EXIT_FAIL;
	}

	i_payload = (Perf_OA_Payload *)malloc(2 * sizeof(unsigned long) + PAYLOAD_MAX_SIZE);
	r_payload = (Perf_OA_Payload *)malloc(2 * sizeof(unsigned long) + PAYLOAD_MAX_SIZE);
	if (i_payload == 0 || r_payload == 0) {
		perror("Failed to allocate memory for payload.\n");
		close(openamp_fd);
		return EXIT_FAIL;
	}

	return Status;
}
