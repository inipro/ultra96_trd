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

/*****************************************************************************
 *
 * @file perfmon-openamp-client.h
 *
 * This file provides defines external interfaces for 
 * perfmon-oa-client (perfmon openamp client) library.
 * 
 *
 * <pre>
 * MODIFICATION HISTORY:
 *
 * Ver   Who  Date        Changes
 * ----- ---- -------- -------------------------------------------------------
 * 1.00a preetesh   12/21/15 Initial release
 * </pre>
 *
 * @note
 *
 ******************************************************************************/


#ifndef PERFAPM_OA_CLIENT_H	/* Prevent circular inclusions */
#define PERFAPM_OA_CLIENT_H	/* by using protection macros  */

#ifdef __cplusplus
extern "C"
{
#endif

/***************************** Include Files ********************************/

/************************** Constant Definitions ****************************/

#define EXIT_SUCCESS 0
#define EXIT_FAIL -1

/**************************** Type Definitions *******************************/
/**
 * This typedef contains openamp payload supported by this library
 */
typedef struct _Perf_OA_Payload
{
	unsigned long num;
	unsigned long size;
	unsigned long readcnt[14];
} Perf_OA_Payload;

/**
 * The Monitor points or slots of every AXI Performance Monitor (APM)
 * device in system. One APM can have multiple performance monitor points.
 * Add entries for all slots of all APMs here
 */
typedef enum _Perf_APM_Slot
{
	PS_OCMAPM_SLOT_0 = 0,	/**< OCM has only one monitor point */
	PS_LPDAPM_SLOT_0 = 1,	/**< LPD has only one monitor point */
	PS_CCIAPM_SLOT_0 = 2,	/**< CCI has only one monitor point */
				/**< DDR has 6 monitor points/slots as below */
	PS_DDRAPM_SLOT_0   = 3,	/**< DDR monitor slot for R5 */
	PS_DDRAPM_SLOT_1   = 4,	/**< DDR monitor slot for APU0 */
	PS_DDRAPM_SLOT_2   = 5,	/**< DDR monitor slot for APU1 */
	PS_DDRAPM_SLOT_3   = 6,	/**< DDR monitor slot for DP + HP0 */
	PS_DDRAPM_SLOT_4   = 7,	/**< DDR monitor slot for PL HP1 + HP2 */
	PS_DDRAPM_SLOT_5   = 8,	/**< DDR monitor slot for PL HP3 + FPDDMA */
	PS_DDRAPM_SLOT_ALL = 9,	/**< All 6 DDR monitor slots */
	MAX_PERF_SLOTS,		/**< donot delete this line */
} Perf_APM_Slot;


/**
 * This typedef contains supported operating systems by this library
 */
typedef enum _Perf_OS_Type
{
	PERF_LINUX_OS = 0,
	PERF_BAREMATEL_OS = 1,
	PERF_FREERTOS_OS = 2,
} Perf_OS_Type;

/**
 * This typedef contains a list of Metric Counters supported by this library
 */
typedef enum _Perf_Cnt_Type
{
	PERF_SAMPLE_INTERVAL_COUNTER = 0,
	PERF_MERTIC_COUNTER = 1,
} Perf_Cnt_Type;

/**
 * This typedef contains a list of configurations supported by this library
 * Each configuration configures the Metric counters for a set of events
 * For e.g. cfg1 configures Metric counters as below
 * 	6 counters for OCM, LPD and CCI APMs
 * 		WRITE_TRANSCNT, READ_TRANSCNT, WRITE_BYTECNT, READ_BYTECNT, TOTAL_READ_LATENCY, TOTAL_WRITE_LATENCY
 * 	10 counters for DDR APM
 * 		WRITE_BYTECNT, READ_BYTECNT for 5 ports
 *
 * -----------------------------------------------
 * Config
 * -----------------------------------------------
 * PERF_APM_CONF0  {
 * 			{ WRITE_TRANSCNT_PERF,	// 6 counters for OCM, LPD and CCI APMs
 *			  READ_TRANSCNT_PERF,	// These 3 APM has only one slot
 *			  WRITE_BYTECNT_PERF,
 *			  READ_BYTECNT_PERF,
 *			  TOTAL_READ_LATENCY_PERF,
 *			  TOTAL_WRITE_LATENCY_PERF }
 *
 *			{ WRITE_BYTECNT_PERF,	// DDR APM slot 1
 *			  READ_BYTECNT_PERF,	// DDR APM slot 1
 *			  WRITE_BYTECNT_PERF,	// DDR APM slot 2
 *			  READ_BYTECNT_PERF,	// DDR APM slot 2
 *			  WRITE_BYTECNT_PERF,	// DDR APM slot 3
 *			  READ_BYTECNT_PERF,	// DDR APM slot 3
 *			  WRITE_BYTECNT_PERF,	// DDR APM slot 4
 *			  READ_BYTECNT_PERF,	// DDR APM slot 4
 *			  WRITE_BYTECNT_PERF,	// DDR APM slot 5
 *			  READ_BYTECNT_PERF }	// DDR APM slot 5
 *		   }
 *
 *
 * PERF_APM_CONF1  {
 * 			{ WRITE_TRANSCNT_PERF,	// 6 counters for OCM, LPD and CCI APMs
 *			  READ_TRANSCNT_PERF,	// These 3 APM has only one slot
 *			  WRITE_BYTECNT_PERF,
 *			  READ_BYTECNT_PERF,
 *			  TOTAL_READ_LATENCY_PERF,
 *			  TOTAL_WRITE_LATENCY_PERF }
 *
 *			{ WRITE_BYTECNT_PERF,   // DDR APM slot 0
 *			  READ_BYTECNT_PERF,    // DDR APM slot 0
 *			  WRITE_BYTECNT_PERF,	// DDR APM slot 1
 *			  READ_BYTECNT_PERF,	// DDR APM slot 1
 *			  WRITE_BYTECNT_PERF,	// DDR APM slot 2
 *			  READ_BYTECNT_PERF,	// DDR APM slot 2
 *			  WRITE_BYTECNT_PERF,	// DDR APM slot 3
 *			  READ_BYTECNT_PERF,	// DDR APM slot 3
 *			  WRITE_BYTECNT_PERF,	// DDR APM slot 4
 *			  READ_BYTECNT_PERF }	// DDR APM slot 4
 *		   }
 */
typedef enum _Perf_APM_confs
{
	PERF_APM_CONF0 = 0,
	PERF_APM_CONF1 = 1,
} Perf_APM_confs;

/************************** Function Prototypes *****************************/

/**
 * Functions in perfmon-oa-client.c
 */
int perfoacl_reset_countervalue(Perf_APM_Slot apm_slot, Perf_Cnt_Type cnttype);
Perf_OA_Payload* perfoacl_get_rd_wr_cnt(Perf_APM_Slot apm_slot, Perf_Cnt_Type cnttype);
int perfoacl_init_all(Perf_OS_Type ostype, Perf_Cnt_Type cnttype);
int perfoacl_deinit_all(Perf_OS_Type ostype, Perf_Cnt_Type cnttype);

#ifdef __cplusplus
}
#endif

#endif
