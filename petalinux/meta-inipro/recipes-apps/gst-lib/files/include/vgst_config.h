/******************************************************************************
 * (c) Copyright 2012-2017 Xilinx, Inc. All rights reserved.
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


#ifndef INCLUDE_VGST_CONFIG_H_
#define INCLUDE_VGST_CONFIG_H_


#ifdef __cplusplus
extern "C"
{
#endif

typedef enum {
    VGST_ENC_IP_MODE_DEFAULT,
    VGST_ENC_IP_MODE_ZERO_COPY,
    VGST_ENC_IP_MODE_DMA_IMPORT,
    VGST_ENC_IP_MODE_DMA_EXPORT,
} VGST_ENC_IP_MODE;

typedef enum {
    VGST_ENC_CONTROL_RATE_DISABLE,
    VGST_ENC_CONTROL_RATE_VARIABLE,
    VGST_ENC_CONTROL_RATE_CONSTANT,
    VGST_ENC_CONTROL_RATE_LOW_LETENCY,
    VGST_ENC_CONTROL_RATE_DEFAULT =-1,
} VGST_ENC_CONTROL_RATE;

typedef enum {
    VGST_DEC_IP_MODE_DEFAULT,
    VGST_DEC_IP_MODE_ZERO_COPY,
} VGST_DEC_IP_MODE;


typedef enum {
    VGST_DEC_OP_MODE_DEFAULT,
    VGST_DEC_OP_MODE_DMA_EXPORT,
} VGST_DEC_OP_MODE;


#define MAX_WIDTH                    3840
#define MAX_HEIGHT                   2160
#define MAX_FRAME_RATE_DENOM         1
#define MIN_B_FRAME                  0
#define MAX_B_FRAME                  4
#define MIN_SLICE_VALUE              4
#define MAX_H265_SLICE_VALUE         22
#define MAX_H264_SLICE_VALUE         135
#define MAX_H264_BITRATE             60000   //60 Mbps
#define MAX_H265_BITRATE             60000   //60 Mbps
#define MIN_GOP_LEN                  1
#define MAX_GOP_LEN                  1000
#define MAX_SRC_NUM                  4
#define MAX_SPLIT_SCREEN             2
#define H264_ENC_NAME                "omxh264enc"
#define H265_ENC_NAME                "omxh265enc"
#define H264_DEC_NAME                "omxh264dec"
#define H265_DEC_NAME                "omxh265dec"
#define H264_PARSER_NAME             "h264parse"
#define H265_PARSER_NAME             "h265parse"
#define H264_RTP_PAYLOAD_NAME        "rtph264pay"
#define H265_RTP_PAYLOAD_NAME        "rtph265pay"
#define V4L2_SRC_NAME                "v4l2src"
#define INPUT_FORMAT                 "NV12"
#define DEC_BUFFERING_COUNT          5
#define GST_MINUTE                   60
#define MAX_PORT_NUMBER              65534
#define MIN_PORT_NUMBER              1024
#define QOS_DSCP_VALUE               60
#define FPS_UPDATE_INTERVAL          1000  // 1sec
#define DP_BUS_ID                    "fd4a0000.zynqmp-display"
#define DP_DRIVER_NAME               "xlnx"
#define HDMI_DRIVER_NAME             "xlnx"
#define DEFAULT_TPG_FPS_NUM          30
#define DEFAULT_PLANE_ID             29

#ifdef __cplusplus
}
#endif

#endif /* INCLUDE_VGST_CONFIG_H_ */
