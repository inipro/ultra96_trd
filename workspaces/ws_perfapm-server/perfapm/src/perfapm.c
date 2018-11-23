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
 * @file perfmon.c
 * @addtogroup perfapm
 * @{
 *
 * This file contains the library API functions that can be used to access
 * the Performance Monitor devices in MPSoC.
 *
 * Refer to the perfmon.h header file for more information about this library.
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
#include "perfapm.h"
#include "xaxipmon.h"
/************************** Constant Definitions *****************************/
static XAxiPmon  APM_PmonInst[MAX_PERF_SLOTS];
static XAxiPmon_Config *APM_ConfigPtr[MAX_PERF_SLOTS] = { NULL };
static volatile u64 apm_readcnt[MAX_PERF_SLOTS];
static volatile u64 apm_writecnt[MAX_PERF_SLOTS];

/**
 * This table contains system device_id for all slots mentioned in Perf_APM_Slot enum.
 * There should be 1-to-1 mapping between entries of this table and above enum.
 */
static int Perf_APM_DEVICE_ID[] = {	/**< device_id of apm monitor points from Perf_APM_Slot */
	XPAR_AXIPMON_1_DEVICE_ID,	/**< device_id for monitor point 0 */
	XPAR_AXIPMON_2_DEVICE_ID,	/**< device_id for monitor point 1 */
	XPAR_AXIPMON_3_DEVICE_ID,	/**< device_id for monitor point 2 */ // TODO fix this

	XPAR_AXIPMON_0_DEVICE_ID,	/**< device_id for monitor point 3 */
	XPAR_AXIPMON_0_DEVICE_ID,	/**< device_id for monitor point 4 */
	XPAR_AXIPMON_0_DEVICE_ID,	/**< device_id for monitor point 5 */
	XPAR_AXIPMON_0_DEVICE_ID,	/**< device_id for monitor point 6 */
	XPAR_AXIPMON_0_DEVICE_ID,	/**< device_id for monitor point 7 */
	XPAR_AXIPMON_0_DEVICE_ID,	/**< device_id for monitor point 8 */
	XPAR_AXIPMON_0_DEVICE_ID,	/**< device_id for monitor point 9 */
};

/**************************** Type Definitions *******************************/

/***************** Macros (Inline Functions) Definitions *********************/

/************************** Function Prototypes ******************************/

/************************** Variable Definitions *****************************/

/*****************************************************************************/
/**
 *
 * This function configures a specific XAxiPmon device/instance for read/write.
 *
 * @param	apm_slot is an enum to the XAxiPmon instance monitor ports.
 *
 * @return
 *		- XST_SUCCESS if successful.
 *
 * @note	We have one DDR APM with 6 monitor ports. 
 * 		Remaining 3 APMs have 1 monitor port each.
 *		The DDR APM can be configured to have a maximum of 10 counters 
 *		for 6 monitor ports.
 *		Remaining 3 APMs have 8 counters each.
 *
 ******************************************************************************/
static int config_apms_rdwr(Perf_APM_Slot apm_slot, Perf_APM_confs conf)
{
	int Status = XST_SUCCESS;
	XAxiPmon *InstancePtr = &APM_PmonInst[apm_slot];

	Status |= XAxiPmon_ResetMetricCounter(InstancePtr);
	XAxiPmon_ResetGlobalClkCounter(InstancePtr);

	if (apm_slot <= PS_CCIAPM_SLOT_0) { // 3 APMs with only one monitor point and 8 counters each
		Status |= XAxiPmon_SetMetrics(InstancePtr,0,XAPM_METRIC_SET_0,XAPM_METRIC_COUNTER_0); /* Write Transaction Count */
		Status |= XAxiPmon_SetMetrics(InstancePtr,0,XAPM_METRIC_SET_1,XAPM_METRIC_COUNTER_1); /* Read Transaction Count  */
		Status |= XAxiPmon_SetMetrics(InstancePtr,0,XAPM_METRIC_SET_2,XAPM_METRIC_COUNTER_2); /* Write Byte Count */
		Status |= XAxiPmon_SetMetrics(InstancePtr,0,XAPM_METRIC_SET_3,XAPM_METRIC_COUNTER_3); /* Read Byte Count  */
		Status |= XAxiPmon_SetMetrics(InstancePtr,0,XAPM_METRIC_SET_5,XAPM_METRIC_COUNTER_5); /* Total Read Latency */
		Status |= XAxiPmon_SetMetrics(InstancePtr,0,XAPM_METRIC_SET_6,XAPM_METRIC_COUNTER_6); /* Total Write Latency */
	} else if (apm_slot == PS_DDRAPM_SLOT_ALL) { // DDR APM with 10 counters and 6 monitor points
		Status |= XAxiPmon_SetMetrics(InstancePtr,5,XAPM_METRIC_SET_2,XAPM_METRIC_COUNTER_0); /* Write Byte Count */
		Status |= XAxiPmon_SetMetrics(InstancePtr,5,XAPM_METRIC_SET_3,XAPM_METRIC_COUNTER_1); /* Read Byte Count  */
		Status |= XAxiPmon_SetMetrics(InstancePtr,1,XAPM_METRIC_SET_2,XAPM_METRIC_COUNTER_2); /* Write Byte Count */
		Status |= XAxiPmon_SetMetrics(InstancePtr,1,XAPM_METRIC_SET_3,XAPM_METRIC_COUNTER_3); /* Read Byte Count  */
		Status |= XAxiPmon_SetMetrics(InstancePtr,2,XAPM_METRIC_SET_2,XAPM_METRIC_COUNTER_4); /* Write Byte Count */
		Status |= XAxiPmon_SetMetrics(InstancePtr,2,XAPM_METRIC_SET_3,XAPM_METRIC_COUNTER_5); /* Read Byte Count  */
		Status |= XAxiPmon_SetMetrics(InstancePtr,3,XAPM_METRIC_SET_2,XAPM_METRIC_COUNTER_6); /* Write Byte Count */
		Status |= XAxiPmon_SetMetrics(InstancePtr,3,XAPM_METRIC_SET_3,XAPM_METRIC_COUNTER_7); /* Read Byte Count  */
		Status |= XAxiPmon_SetMetrics(InstancePtr,4,XAPM_METRIC_SET_2,XAPM_METRIC_COUNTER_8); /* Write Byte Count */
		Status |= XAxiPmon_SetMetrics(InstancePtr,4,XAPM_METRIC_SET_3,XAPM_METRIC_COUNTER_9); /* Read Byte Count  */
	}

	return Status;
}

/*****************************************************************************/
/**
 *
 * This function enables metrics counters for a specific XAxiPmon device/instance.
 *
 * @param	apm_slot is an enum to the XAxiPmon instance monitor ports.
 * @param	cnttype is an enum to type of performance counter
 *
 * @return
 *		- XST_SUCCESS if successful.
 *
 * @note	None.
 *
 ******************************************************************************/
static int enable_apms_counters(Perf_APM_Slot apm_slot, Perf_Cnt_Type cnttype)
{
	int Status = XST_SUCCESS;
	XAxiPmon *InstancePtr = &APM_PmonInst[apm_slot];

	if (cnttype == PERF_SAMPLE_INTERVAL_COUNTER) {
		XAxiPmon_StartCounters(InstancePtr, APM_SAMPLE_VALUE);
		XAxiPmon_WriteReg(InstancePtr->Config.BaseAddress, XAPM_SICR_OFFSET, 0x101);
	}
	else
		XAxiPmon_EnableMetricsCounter(InstancePtr);

	return Status;
}

/*****************************************************************************/
/**
 *
 * This function disables metrics counters for a specific XAxiPmon device/instance.
 *
 * @param	apm_slot is an enum to the XAxiPmon instance monitor ports.
 * @param	cnttype is an enum to type of performance counter
 *
 * @return
 *		- XST_SUCCESS if successful.
 *
 * @note	None.
 *
 ******************************************************************************/
static int disable_apms_counters(Perf_APM_Slot apm_slot, Perf_Cnt_Type cnttype)
{
	int Status = XST_SUCCESS;
	XAxiPmon *InstancePtr = &APM_PmonInst[apm_slot];

	XAxiPmon_DisableMetricsCounter(InstancePtr);

	return Status;
}

/*****************************************************************************/
/**
 *
 * This function searches a specific XAxiPmon device/instance from APM SDK.
 *
 * @param	apm_slot is an enum to the XAxiPmon instance monitor ports.
 *
 * @return
 *		- XST_SUCCESS if successful.
 *
 * @note	None.
 *
 ******************************************************************************/
static int lookup_apms(Perf_APM_Slot apm_slot)
{
	int Status;

	APM_ConfigPtr[apm_slot] = XAxiPmon_LookupConfig(Perf_APM_DEVICE_ID[apm_slot]);
	if (APM_ConfigPtr[apm_slot] == NULL) {
		return XST_FAILURE;
	}

	Status = XAxiPmon_CfgInitialize(&APM_PmonInst[apm_slot], APM_ConfigPtr[apm_slot], APM_ConfigPtr[apm_slot]->BaseAddress);
	if (Status != XST_SUCCESS) {
		return XST_FAILURE;
	}

	return XST_SUCCESS;
}

/*****************************************************************************/
/**
 *
 * This function provides a counter value for a specific XAxiPmon device/instance.
 *
 * @param	apm_slot is an enum to the XAxiPmon instance monitor ports.
 * @param	CounterNum is counter number for e.g. XAPM_METRIC_COUNTER_2
 * @param	cnttype is an enum to type of performance counter
 *
 * @return
 *		- metric counter value if successful.
 *
 * @note	None.
 *
 ******************************************************************************/
unsigned long perfmon_get_countervalue(Perf_APM_Slot apm_slot, int CounterNum, Perf_Cnt_Type cnttype)
{
	Xil_AssertNonvoid(apm_slot != PS_DDRAPM_SLOT_ALL);
	Xil_AssertNonvoid(apm_slot != MAX_PERF_SLOTS);
	Xil_AssertNonvoid(apm_slot < MAX_PERF_SLOTS);
	if (cnttype == PERF_SAMPLE_INTERVAL_COUNTER)
		return XAxiPmon_GetSampledMetricCounter(&APM_PmonInst[apm_slot], CounterNum);
	else
		return XAxiPmon_GetMetricCounter(&APM_PmonInst[apm_slot], CounterNum);
}

/*****************************************************************************/
/**
 *
 * This function provides read/write counter value for a specific XAxiPmon device/instance.
 *
 * @param	apm_slot is an enum to the XAxiPmon instance monitor ports.
 * @param	cnttype is an enum to type of performance counter
 *
 * @return
 *		- Total read and write counts if successful.
 *
 * @note	None.
 *
 ******************************************************************************/
u64 perfmon_get_rd_wr_cnt(Perf_APM_Slot apm_slot, Perf_Cnt_Type cnttype)
{
	Xil_AssertNonvoid(apm_slot != PS_DDRAPM_SLOT_ALL);
	Xil_AssertNonvoid(apm_slot != MAX_PERF_SLOTS);
	Xil_AssertNonvoid(apm_slot < MAX_PERF_SLOTS);

	if (apm_slot <= PS_CCIAPM_SLOT_0) { // 3 APMs with only one monitor point
		apm_writecnt[apm_slot] +=  perfmon_get_countervalue(apm_slot, XAPM_METRIC_COUNTER_2, cnttype);
		apm_readcnt[apm_slot]  +=  perfmon_get_countervalue(apm_slot, XAPM_METRIC_COUNTER_3, cnttype);
		return (apm_readcnt[apm_slot] + apm_writecnt[apm_slot]);
	} else if ( (apm_slot >= PS_DDRAPM_SLOT_0) &&  (apm_slot <= PS_DDRAPM_SLOT_4)) { // DDR APM with specific monitor point
		apm_writecnt[apm_slot] +=  perfmon_get_countervalue(apm_slot, (apm_slot - PS_DDRAPM_SLOT_0) * 2, cnttype);
		apm_readcnt[apm_slot]  +=  perfmon_get_countervalue(apm_slot, ((apm_slot - PS_DDRAPM_SLOT_0) * 2) + 1, cnttype);
		return (apm_readcnt[apm_slot] + apm_writecnt[apm_slot]);
	} else if ( apm_slot == PS_DDRAPM_SLOT_5) { // DDR APM with specific monitor point
		apm_writecnt[apm_slot] +=  perfmon_get_countervalue(apm_slot, 0, cnttype);
		apm_readcnt[apm_slot]  +=  perfmon_get_countervalue(apm_slot, 1, cnttype);
		return (apm_readcnt[apm_slot] + apm_writecnt[apm_slot]);
	}

	return XST_SUCCESS;
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
 *		- XST_SUCCESS if successful.
 *
 * @note	None.
 *
 ******************************************************************************/
int perfmon_reset_countervalue(Perf_APM_Slot apm_slot, Perf_Cnt_Type cnttype)
{
	int Status = XST_SUCCESS;
	XAxiPmon *InstancePtr = &APM_PmonInst[apm_slot];

	Xil_AssertNonvoid(apm_slot != PS_DDRAPM_SLOT_ALL);
	Xil_AssertNonvoid(apm_slot != MAX_PERF_SLOTS);
	Xil_AssertNonvoid(apm_slot < MAX_PERF_SLOTS);

	if (cnttype == PERF_MERTIC_COUNTER) {
		Status |= XAxiPmon_ResetMetricCounter(InstancePtr);
		XAxiPmon_ResetGlobalClkCounter(InstancePtr);
	}

	apm_writecnt[apm_slot] = apm_readcnt[apm_slot] = 0;

	return Status;
}

/*****************************************************************************/
/**
 *
 * This function de-initializes a specific XAxiPmon device/instance.
 *
 * @param	apm_slot is an enum to the XAxiPmon instance monitor ports.
 * @param	cnttype is an enum to type of performance counter
 *
 * @return
 *		- XST_SUCCESS if successful.
 *
 * @note	None.
 *
 ******************************************************************************/
int perfmon_deinit(Perf_APM_Slot apm_slot, Perf_Cnt_Type cnttype)
{
	int Status;

	Xil_AssertNonvoid(apm_slot < MAX_PERF_SLOTS);

	/* Disable Metric counters */
	Status = disable_apms_counters(apm_slot, cnttype);
	if (Status != XST_SUCCESS) {
		return XST_FAILURE;
	}

	return XST_SUCCESS;
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
 *		- XST_SUCCESS if successful.
 *
 * @note	None.
 *
 ******************************************************************************/
int perfmon_deinit_all(Perf_OS_Type ostype, Perf_Cnt_Type cnttype)
{
	int Status = XST_SUCCESS;

	/* 
	 * All APMs with most used configurations
	 */
	Status |= perfmon_deinit(PS_OCMAPM_SLOT_0, cnttype);
	Status |= perfmon_deinit(PS_LPDAPM_SLOT_0, cnttype);
	Status |= perfmon_deinit(PS_CCIAPM_SLOT_0, cnttype);
	Status |= perfmon_deinit(PS_DDRAPM_SLOT_ALL, cnttype);

	/* 
	 * post-deinit based on ostype
	 */
	Status |= perfmon_postdeinit(ostype);

	return Status;
}

/*****************************************************************************/
/**
 *
 * This function initializes a specific XAxiPmon device/instance.
 *
 * @param	apm_slot is an enum to the XAxiPmon instance monitor ports.
 * @param	cnttype is an enum to type of performance counter
 *
 * @return
 *		- XST_SUCCESS if successful.
 *
 * @note	None.
 *
 ******************************************************************************/
int perfmon_init(Perf_APM_Slot apm_slot, Perf_Cnt_Type cnttype)
{
	int Status;

	Xil_AssertNonvoid(apm_slot < MAX_PERF_SLOTS);

	/* init APMs */
	Status = lookup_apms(apm_slot);
	if (Status != XST_SUCCESS) {
		return XST_FAILURE;
	}

	/* configure APMs to record counters */
	Status = config_apms_rdwr(apm_slot, PERF_APM_CONF0);
	if (Status != XST_SUCCESS) {
		return XST_FAILURE;
	}

	/* Enable Metric counters */
	Status = enable_apms_counters(apm_slot, cnttype);
	if (Status != XST_SUCCESS) {
		return XST_FAILURE;
	}

	return XST_SUCCESS;
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
 *		- XST_SUCCESS if successful.
 *
 * @note	None.
 *
 ******************************************************************************/
int perfmon_init_all(Perf_OS_Type ostype, Perf_Cnt_Type cnttype)
{
	int Status = XST_SUCCESS;

	/* 
	 * pre-init based on ostype
	 */
	Status |= perfmon_preinit(ostype);

	/* 
	 * All APMs with most used configurations
	 */
	Status |= perfmon_init(PS_OCMAPM_SLOT_0, cnttype);
	Status |= perfmon_init(PS_LPDAPM_SLOT_0, cnttype);
	Status |= perfmon_init(PS_CCIAPM_SLOT_0, cnttype);
	Status |= perfmon_init(PS_DDRAPM_SLOT_ALL, cnttype);

	APM_PmonInst[PS_DDRAPM_SLOT_0] = APM_PmonInst[PS_DDRAPM_SLOT_1] = 
	APM_PmonInst[PS_DDRAPM_SLOT_2] = APM_PmonInst[PS_DDRAPM_SLOT_3] = 
	APM_PmonInst[PS_DDRAPM_SLOT_4] = APM_PmonInst[PS_DDRAPM_SLOT_5] =
	APM_PmonInst[PS_DDRAPM_SLOT_ALL];

	return Status;
}
