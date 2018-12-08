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


#ifndef INCLUDE_VGST_LIB_H_
#define INCLUDE_VGST_LIB_H_

#include <gst/gst.h>
#include <sys/stat.h>
#include <string.h>
#include "vgst_err.h"
#include <gst/video/videooverlay.h>

#ifdef __cplusplus
extern "C"
{
#endif

#ifdef GST_MODE
	#define vlib_src_init()		0
	#define vlib_src_uninit()	0
	#define vlib_src_config(x, y)	0
	#define vlib_get_devname	vlib_video_src_get_vdev_from_id
	#define V4L2_SRC_NAME           "v4l2src"
	#define FILE_SRC_NAME		"filesrc"
	#define MAX_FRAME_RATE_NUMERATOR	60
	#define GST_FILTER_MODE_HW	1
	#define GST_FILTER_MODE_SW	0
	/*
	 * Adding below enum for compilation purpose only. Will remove it
	 * once we use same vlib library for VCU and SDX filters
	 */
	typedef enum {
	    TPG = 1,
	    CSI,
	} vlib_video_src_dev_type;
#else
	#define FILE_SRC_NAME		"uridecodebin"
	#define MAX_FRAME_RATE_NUMERATOR	30
#endif

typedef struct
_vgst_sdx_filter_params {
    gchar      *filter_name;
    gint       filter_mode;
} vgst_sdx_filter_params;

typedef struct
_vgst_enc_params {
    gchar      *enc_name;
    gboolean   enable_l2Cache;
    guint      bitrate;
    guint      gop_len;
    guint      b_frame;
    guint      slice;
    gint       qp_mode;
    gint       rc_mode;
    gint       profile;
} vgst_enc_params;


typedef struct
_vgst_input_param {
    gchar      *src;
    gchar      *uri;
    gchar      *format;
    gboolean   raw;
    guint      height;
    guint      width;
    gint       device_type;
    gint       filter_type;
    gint       io_mode;
} vgst_ip_params;


typedef struct
_vgst_output_param {
    gchar      *file_out;
    gchar      *host_ip;
    guint      duration;
    guint      port_num;
} vgst_op_params;

typedef struct
_vgst_cmn_param {
    guint      num_src;
    gint       sink_type;
    guint      driver_type;
    guint      plane_id;
    guint      monitor_refresh_rate;
} vgst_cmn_params;


typedef enum {
    STREAM_OUT,
    RECORD,
    DISPLAY,
    SPLIT_SCREEN,
} VGST_SINK_TYPE;


typedef enum {
    UNIFORM,
    AUTO = 1024,
} VGST_QP_MODE;


typedef enum {
    DISABLE,
    VBR,
    CBR,
    LOW_LATENCY,
} VGST_RC_MODE;


typedef enum {
    PLAYING = 1,
    PAUSED,
} VGST_TOGGLE_STATE;

typedef enum {
    BASELINE_PROFILE,
    MAIN_PROFILE,
    HIGH_PROFILE,
} VGST_PROFILE_MODE;

typedef enum {
    DP,
} VGST_DRIVER_TYPE;

typedef enum {
    VGST_EVENT_NONE,
    VGST_EVENT_EOS,
    VGST_EVENT_ERR,
} VGST_EVENT_TYPE;

typedef enum {
    VCU,
    SDX_FILTER,
} VGST_FILTER_TYPE;

typedef enum {
    VGST_V4L2_IO_MODE_AUTO,
    VGST_V4L2_IO_MODE_RW,
    VGST_V4L2_IO_MODE_MMAP,
    VGST_V4L2_IO_MODE_USERPTR,
    VGST_V4L2_IO_MODE_DMABUF_EXPORT,
    VGST_V4L2_IO_MODE_DMABUF_IMPORT,
} VGST_V4L2_IO_MODE;

/* This API is to initialize the library */
gint vgst_init(void);

/* This API is to initialize the options to initiate the pipeline */
gint vgst_config_options (vgst_enc_params *enc_param, vgst_ip_params *ip_param, vgst_op_params *op_param, vgst_cmn_params *cmn_param, vgst_sdx_filter_params *filter_param);

/* This API is to start the pipeline */
gint vgst_start_pipeline (void);

/* This API is interface to stop the single/multi-stream pipeline */
gint vgst_stop_pipeline (void);

/* This API is to convert error number to string */
const gchar * vgst_error_to_string (VGST_ERROR_LOG error_code);

/* This API is to get fps of the pipeline */
void vgst_get_fps (guint index, guint *fps);

/* This API is to get bitrate for file playback */
guint vgst_get_bitrate (int index);

/* This API is to poll events */
gint vgst_poll_event (int *arg, int *index);

/* This API is to un-initialize the library */
gint vgst_uninit(void);

#ifdef __cplusplus
}
#endif

#endif /* INCLUDE_VGST_LIB_H_ */
