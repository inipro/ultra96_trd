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


#include <vgst_utils.h>
#include "vgst_lib.h"

vgst_application app;

const gchar *
vgst_error_to_string (VGST_ERROR_LOG error_code) {
    guint i =0;
    switch (error_code) {
    case VGST_SUCCESS :
      return "Success";
    case VGST_ERROR_FILE_IO :
      return "File I/O Error";
    case VGST_ERROR_GOP_NOT_SUPPORTED :
      return "Gop length must be multiple of b-frames+1";
    case VGST_ERROR_PIPELINE_CREATE_FAIL :
      return "pipeline creation failed";
    case VGST_ERROR_PIPELINE_LINKING_FAIL :
      return "pipeline linking failed";
    case VGST_ERROR_STATE_CHANGE_FAIL :
      return "state change failed";
    case VGST_ERROR_B_FRAME_RANGE_MISMATCH :
      return "b-frames should be in the range of 0-4";
    case VGST_ERROR_GOP_LENGTH_RANGE_MISMATCH :
      return "GoP length should be in the range of 1 -1000";
    case VGST_ERROR_ENCODER_TYPE_NOT_SUPPORTED :
      return "Encoder type not supported";
    case VGST_ERROR_SRC_TYPE_NOT_SUPPORTED :
      return "Source type not supported";
    case VGST_ERROR_FORMAT_NOT_SUPPORTED :
      return "format type not supported";
    case VGST_ERROR_SRC_COUNT_INVALID :
      return "source count invalid";
    case VGST_ERROR_RESOLUTION_NOT_SUPPORTED :
      return "Resolution WxH should be 3840x2160";
    case VGST_ERROR_DEVICE_TYPE_INVALID :
      return "Device type invalid";
    case VGST_ERROR_PIPELINE_HANDLE_NULL :
      return "pipeline not initialized";
    case VGST_ERROR_SET_ENC_BUF_ENV_FAILED :
      return "Encoder buffer env setting failed";
    case VGST_ERROR_RUN_TIME_PIPELINE_FAILED :
      for (i =0; i< app.cmn_params->num_src; i++) {
        if (app.playback[i].err_msg)
          return app.playback[i].err_msg;
      }
      break;
    case VGST_ERROR_MULTI_STREAM_FAIL:
      return "Multi stream on DP not supported";
    case VGST_ERROR_SPLIT_SCREEN_FAIL :
      return "Split Screen on DP not supported";
    case VGST_ERROR_DRIVER_TYPE_MISMATCH :
      return "driver type mismatched";
    case VGST_ERROR_SLICE_RANGE_MISMATCH :
      return "Slice range mismatched";
    case VGST_ERROR_BITRATE_NOT_SUPPORTED :
      return "bitrate not supported";
    case VGST_ERROR_QPMODE_NOT_SUPPORTED :
      return "Qp mode not supported";
    case VGST_ERROR_PROFILE_NOT_SUPPORTED :
      return "Profile not supported";
    case VGST_ERROR_RCMODE_NOT_SUPPORTED :
      return "Rate control mode not supported";
    case VGST_ERROR_PORT_NUM_RANGE_MISMATCH :
      return "Port number range mismatched";
    case VGST_ERROR_OVERLAY_CREATION_FAIL :
      return "Failed to create overlay, multi-stream pipeline failed";
    case VGST_ERROR_APP_PTR_NULL :
      return "Application pointers are null";
    case VGST_VLIB_ERROR_SET_FPS :
      return vlib_error_name(VGST_VLIB_ERROR_SET_FPS);
    case VGST_VLIB_ERROR_MIPI_SRC_CONFIG :
      return vlib_error_name(VGST_VLIB_ERROR_MIPI_SRC_CONFIG);
    default :
      return "Unknown Error";
    }
    return "Unknown Error";
}

void
init_struct_params () {
    memset (&app, 0, sizeof(vgst_application));
}


gint
vgst_config_options (vgst_enc_params *enc_param, vgst_ip_params *ip_param, vgst_op_params *op_param,
                     vgst_cmn_params *cmn_param, vgst_sdx_filter_params *filter_param) {
    struct stat file_stat;
    guint i,num_src = cmn_param->num_src;
    gint ret;
    struct vlib_config_data config = {};
    for (i =0; i< num_src; i++) {
      if (g_strcmp0 (ip_param[i].src, V4L2_SRC_NAME) && g_strcmp0 (ip_param[i].src, FILE_SRC_NAME)) {
        GST_ERROR ("Source type is not supported");
        return VGST_ERROR_SRC_TYPE_NOT_SUPPORTED;
      }
      if ((ip_param[i].filter_type != SDX_FILTER) && g_strcmp0 (ip_param[i].format, INPUT_FORMAT)) {
        GST_ERROR ("input format is not supported");
        return VGST_ERROR_FORMAT_NOT_SUPPORTED;
      }
      if (!g_strcmp0 (ip_param[i].src, FILE_SRC_NAME) && (ip_param[i].filter_type != SDX_FILTER) && (stat(ip_param[i].uri +strlen("file:"), &file_stat) < 0)) {
        GST_ERROR ("i/p file does not exist");
        return VGST_ERROR_FILE_IO;
      }
      if (!g_strcmp0 (ip_param[i].src, FILE_SRC_NAME) && (ip_param[i].filter_type == SDX_FILTER) && (stat(ip_param[i].uri, &file_stat) < 0)) {
        GST_ERROR ("i/p file does not exist");
        return VGST_ERROR_FILE_IO;
      }
      if (cmn_param->num_src > MAX_SRC_NUM) {
        GST_ERROR ("source count is invalid");
        return VGST_ERROR_SRC_COUNT_INVALID;
      }
      if (DP == cmn_param->driver_type && cmn_param->num_src > 1) {
        GST_ERROR ("multi stream is not supported on DP");
        return VGST_ERROR_MULTI_STREAM_FAIL;
      }
      if (DP == cmn_param->driver_type && cmn_param->sink_type == SPLIT_SCREEN) {
        GST_ERROR ("Split screen is not supported on DP");
        return VGST_ERROR_SPLIT_SCREEN_FAIL;
      }
      if (DP != cmn_param->driver_type) {
        GST_ERROR ("driver type is invalid");
        return VGST_ERROR_DRIVER_TYPE_MISMATCH;
      }
      if ((ip_param[i].filter_type != SDX_FILTER) && (ip_param[i].width > MAX_WIDTH || ip_param[i].height > MAX_HEIGHT)) {
        GST_ERROR ("Resolution WxH not supported");
        return VGST_ERROR_RESOLUTION_NOT_SUPPORTED;
      }
      if ((ip_param[i].filter_type != SDX_FILTER) && (!g_strcmp0 (ip_param[i].src, V4L2_SRC_NAME) && (ip_param[i].device_type != TPG && ip_param[i].device_type != CSI ))) {
        GST_ERROR ("device type is invalid");
        return VGST_ERROR_DEVICE_TYPE_INVALID;
      }
      if (!g_strcmp0 (ip_param[i].src, V4L2_SRC_NAME) && (FALSE == ip_param[i].raw) && (ip_param[i].filter_type != SDX_FILTER)) {
        if (enc_param[i].b_frame < MIN_B_FRAME || enc_param[i].b_frame > MAX_B_FRAME) {
          GST_ERROR ("b-frames should be in the range of 0-4");
          return VGST_ERROR_B_FRAME_RANGE_MISMATCH;
        }
        if (0 != enc_param[i].gop_len % (enc_param[i].b_frame+1)) {
          GST_ERROR ("GoP length should be multiple of b-frames+1 multiple");
          return VGST_ERROR_GOP_NOT_SUPPORTED;
        }
        if (enc_param[i].gop_len < MIN_GOP_LEN || enc_param[i].gop_len > MAX_GOP_LEN) {
          GST_ERROR ("GoP length should be 1-1000");
          return VGST_ERROR_GOP_LENGTH_RANGE_MISMATCH;
        }
        if (g_strcmp0 (enc_param[i].enc_name, H264_ENC_NAME) && g_strcmp0 (enc_param[i].enc_name, H265_ENC_NAME)) {
          GST_ERROR ("Encoder name is incorrect");
          return VGST_ERROR_ENCODER_TYPE_NOT_SUPPORTED;
        }
        if (!g_strcmp0 (enc_param[i].enc_name, H265_ENC_NAME) && (enc_param[i].slice > MAX_H265_SLICE_VALUE || enc_param[i].slice < MIN_SLICE_VALUE)) {
          GST_ERROR ("Slice range for H265 should be in the range of %d-%d", MIN_SLICE_VALUE, MAX_H265_SLICE_VALUE);
          return VGST_ERROR_SLICE_RANGE_MISMATCH;
        }
        if (!g_strcmp0 (enc_param[i].enc_name, H264_ENC_NAME) && (enc_param[i].slice > MAX_H264_SLICE_VALUE || enc_param[i].slice < MIN_SLICE_VALUE)) {
          GST_ERROR ("Slice range for H264 should be in the range of %d-%d",MIN_SLICE_VALUE, MAX_H264_SLICE_VALUE);
          return VGST_ERROR_SLICE_RANGE_MISMATCH;
        }
        if ((enc_param[i].profile != BASELINE_PROFILE && enc_param[i].profile != MAIN_PROFILE && enc_param[i].profile != HIGH_PROFILE) &&
             !g_strcmp0 (enc_param[i].enc_name, H264_ENC_NAME)) {
          GST_ERROR ("Profile can be either baseline, main or high for H264");
          return VGST_ERROR_PROFILE_NOT_SUPPORTED;
        }
        if ((enc_param[i].profile != MAIN_PROFILE) && !g_strcmp0 (enc_param[i].enc_name, H265_ENC_NAME)) {
          GST_ERROR ("Only main profile supported for H265");
          return VGST_ERROR_PROFILE_NOT_SUPPORTED;
        }
        if (!g_strcmp0 (enc_param[i].enc_name, H265_ENC_NAME) && (enc_param[i].bitrate > MAX_H265_BITRATE)) {
          GST_ERROR ("max bitrate supported for H265 is [%d]Mbps", MAX_H265_BITRATE);
          return VGST_ERROR_BITRATE_NOT_SUPPORTED;
        }
        if (!g_strcmp0 (enc_param[i].enc_name, H264_ENC_NAME) && (enc_param[i].bitrate > MAX_H264_BITRATE)) {
          GST_ERROR ("max bitrate supported for H264 is [%d]Mbps", MAX_H264_BITRATE);
          return VGST_ERROR_BITRATE_NOT_SUPPORTED;
        }
        if ((enc_param[i].qp_mode != UNIFORM && enc_param[i].qp_mode != AUTO)) {
          GST_ERROR ("Qp mode can be either Uniform or Auto");
          return VGST_ERROR_QPMODE_NOT_SUPPORTED;
        }
        if ((enc_param[i].rc_mode != VBR && enc_param[i].rc_mode != CBR)) {
          GST_ERROR ("Rate control mode can be either VBR or CBR");
          return VGST_ERROR_RCMODE_NOT_SUPPORTED;
        }
      }
      if (ip_param[i].filter_type != SDX_FILTER && config.width_in == 0) {
        config.fps.numerator = cmn_param->monitor_refresh_rate;
        config.width_in = ip_param->width;
        config.height_in = ip_param->height;
        if (!g_strcmp0 (ip_param[i].src, V4L2_SRC_NAME) && (ret = vlib_src_config(ip_param[i].device_type, &config))) {
          GST_ERROR ("vlib source config failure %d ",ret);
          return ret;
        }
      }
      if (cmn_param->sink_type == STREAM_OUT) {
        if (op_param[i].port_num < MIN_PORT_NUMBER || op_param[i].port_num > MAX_PORT_NUMBER) {
          GST_ERROR ("Port number should be in the range of [%d]-[%d]",MIN_PORT_NUMBER, MAX_PORT_NUMBER);
          return VGST_ERROR_PORT_NUM_RANGE_MISMATCH;
        }
      }
    }
    for (i =0; i< num_src; i++) {
      vgst_print_params (&ip_param[i], &enc_param[i], &op_param[i], cmn_param, &filter_param[i]);
    }
    init_struct_params ();
    app.enc_params = enc_param;
    app.filter_params = filter_param;
    app.ip_params  = ip_param;
    app.op_params  = op_param;
    app.cmn_params = cmn_param;
    if (ip_param[i].filter_type != SDX_FILTER)
      app.cmn_params->plane_id = DEFAULT_PLANE_ID;
    return VGST_SUCCESS;
}


gint
vgst_start_pipeline (void) {
    VGST_ERROR_LOG ret;
    GstBus *bus;
    guint i =0;
    /* initialize GStreamer */
    gst_init (NULL, NULL);

    // create a pipeline
    if ((ret = vgst_create_elements (app.ip_params, app.enc_params, app.op_params, app.playback, app.cmn_params, app.filter_params))) {
      GST_ERROR ("pipeline creation failed !!!");
      return ret;
    }
    for (i =0; i< app.cmn_params->num_src; i++) {
      bus = gst_pipeline_get_bus (GST_PIPELINE (app.playback[i].pipeline));
      gst_bus_add_watch (bus, bus_callback, &app.playback[i]);
      gst_object_unref (bus);
    }
    GST_DEBUG ("starting playback/capture");
    // run the pipeline
    if ((ret = vgst_run_pipeline (app.ip_params, app.playback, app.cmn_params))) {
      GST_ERROR ("pipeline start = failed !!!");
      return ret;
    }
    return ret;
}


void
vgst_get_fps (guint index, guint *fps) {
    gint i =0;
    if (!fps) {
      GST_ERROR ("Fps pointer is NULL");
      return;
    } else {
      fps[i] = app.playback[index].fps_num[i];
      i++;
      if (app.cmn_params && SPLIT_SCREEN == app.cmn_params->sink_type) {
        // In case of split screen, 0th index(Left) is processed and 1st index(Right) is Raw
        fps[i] = app.playback[index].fps_num[i];
      }
    }
}


guint
vgst_get_bitrate (int index) {
    if (!g_strcmp0 (app.ip_params[index].src, FILE_SRC_NAME) && app.playback[index].file_br) {
      return app.playback[index].file_br;
    } else
      return 0;
}


gint
vgst_stop_pipeline () {
    /* pipeline clean up */
    GST_DEBUG ("cleaning up the pipeline");
    guint i =0;
    if (!app.cmn_params || !app.enc_params || !app.ip_params || !app.op_params || !app.filter_params) {
      GST_ERROR ("Application pointers are null");
      return VGST_ERROR_APP_PTR_NULL;
    }
    for (i =0; i < app.cmn_params->num_src; i++)
      vgst_stop (&app.playback[i], app.ip_params, app.cmn_params->sink_type);

    return VGST_SUCCESS;
}


gint
vgst_poll_event (int *arg, int *index) {
    guint i =0, num_src = app.cmn_params->num_src;
    gboolean eos_flag = TRUE;
    for (i =0; i< num_src; i++) {
      eos_flag &= app.playback[i].eos_flag;
    }

    for (i =0; i< num_src; i++) {
      if (app.playback[i].err_msg) {
        *index = i;
        *arg = VGST_ERROR_RUN_TIME_PIPELINE_FAILED;
        return VGST_EVENT_ERR;
      }
    }
    if (eos_flag) {
      return VGST_EVENT_EOS;
    }
    return VGST_EVENT_NONE;
}

gint vgst_init(void) {
    return vlib_src_init();
}

gint vgst_uninit(void) {
    return vlib_src_uninit();
}
