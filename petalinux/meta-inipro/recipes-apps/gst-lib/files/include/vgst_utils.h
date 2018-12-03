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



#ifndef INCLUDE_VGST_UTILS_H_
#define INCLUDE_VGST_UTILS_H_

#include "vgst_config.h"
#include "video.h"
#include "vgst_lib.h"


#ifdef __cplusplus
extern "C"
{
#endif

typedef struct
_vgst_playback {
    GstElement         *pipeline, *srccapsfilter, *ip_src, *queue, *enc_queue;
    GstElement         *enccapsfilter, *stream_sink, *demux, *mux, *tee;
    GstElement         *videoparser, *videoenc, *videofilter, *videodec, *videosink;
    GstElement         *fpsdisplaysink, *rtppay, *fpsdisplaysink2, *videosink2;
    GstVideoOverlay    *overlay, *overlay2;
    GstPad             *pad1, *pad2;
    GMainLoop          *loop;
    gboolean           eos_flag, err_flag, stop_flag;
    gchar              *err_msg;
    guint              fps_num[MAX_SPLIT_SCREEN], file_br;
} vgst_playback;

typedef struct
_vgst_application {
    vgst_playback      playback[MAX_SRC_NUM];
    vgst_enc_params    *enc_params;
    vgst_sdx_filter_params    *filter_params;
    vgst_ip_params     *ip_params;
    vgst_op_params     *op_params;
    vgst_cmn_params    *cmn_params;
} vgst_application;

/* This API is to link all the elements required for playback/capture pipeline */
VGST_ERROR_LOG vgst_link_elements (vgst_ip_params *ip_param, vgst_playback *play_ptr, vgst_cmn_params *cmn_param);

/* This API is to link all elements required for streaming pipeline */
VGST_ERROR_LOG link_streaming_pipeline (vgst_playback *play_ptr);

/* This API is interface for creating single/mult-stream pipeline */
VGST_ERROR_LOG vgst_create_elements (vgst_ip_params *ip_param, vgst_enc_params *enc_param, vgst_op_params *op_param, vgst_playback *play_ptr, vgst_cmn_params *cmn_param, vgst_sdx_filter_params *filter_param);

/* This API is to set all the required property to start playback/capture pipeline */
void vgst_set_property (vgst_ip_params *ip_param, vgst_enc_params *enc_param, vgst_op_params *op_param, vgst_playback *play_ptr, vgst_cmn_params *cmn_param, vgst_sdx_filter_params *filter_param);

/* This API is to all the parameters coming from application */
void vgst_print_params (vgst_ip_params *ip_param, vgst_enc_params *enc_param, vgst_op_params *op_param, vgst_cmn_params *cmn_param, vgst_sdx_filter_params *filter_param);

/* This API is to set environment required for application to start */
gint vgst_set_env_var (vgst_enc_params *enc_param);

/* This API is to capture messages from pipeline */
gboolean bus_callback (GstBus *bus, GstMessage *msg, gpointer data);

/* This API is required for linking src pad of decoder to sink element */
void on_pad_added (GstElement *element, GstPad *pad, gpointer data);

/* This API is to initialize pipeline structure */
void init_struct_params (void);

/* This API is to measure the current fps value */
void on_fps_measurement (GstElement *fps, gdouble fps_num, gdouble drop_rate, gdouble avg_rate, gpointer data);

/* This API is to parse the tag and get the bitrate value from file */
void fetch_tag (const GstTagList * list, const gchar * tag, gpointer user_data);

/* This API is to create all the elements required for single/multi-stream pipeline */
VGST_ERROR_LOG vgst_create_pipeline (vgst_ip_params *ip_param, vgst_enc_params *enc_param, vgst_playback *play_ptr, guint sink_type, vgst_sdx_filter_params *filter_param);

/* This API is to stop the single/multi-stream pipeline */
gint vgst_stop  (vgst_playback *play_ptr, vgst_ip_params *ip_param, guint sink_type);

/* This API is to create all the elements required for split screen pipeline */
VGST_ERROR_LOG vgst_create_split_pipeline (vgst_ip_params *ip_param, vgst_enc_params *enc_param, vgst_playback *play_ptr);

/* This API is to link all elements required for split screen pipeline */
VGST_ERROR_LOG vgst_link_split_elements (vgst_ip_params *ip_param, vgst_playback *play_ptr);

/* This API is to set all the required property for split screen pipeline */
void vgst_set_split_screen_property (vgst_ip_params *ip_param, vgst_enc_params *enc_param, vgst_playback *play_ptr, guint *plane_id);

VGST_ERROR_LOG vgst_run_pipeline (vgst_ip_params *ip_param, vgst_playback *play_ptr, vgst_cmn_params *cmn_param);

#ifdef __cplusplus
}
#endif

#endif /* INCLUDE_VGST_UTILS_H_ */
