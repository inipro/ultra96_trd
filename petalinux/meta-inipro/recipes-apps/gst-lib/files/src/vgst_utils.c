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


#include "vgst_lib.h"
#include <math.h>
#include <vgst_utils.h>


void
vgst_print_params (vgst_ip_params *ip_param, vgst_enc_params *enc_param, vgst_op_params *op_param, vgst_cmn_params *cmn_param, vgst_sdx_filter_params *filter_param) {
    GST_DEBUG ("Src name %s", ip_param->src);
    GST_DEBUG ("Device type %d [1 =TPG, 2 =HDMI], 3 = MIPI", ip_param->device_type);
    GST_DEBUG ("Format %s", ip_param->format);
    GST_DEBUG ("Uri %s", ip_param->uri);
    GST_DEBUG ("Width %u", ip_param->width);
    GST_DEBUG ("Height %u", ip_param->height);
    GST_DEBUG ("Raw %d", ip_param->raw);
    if (FALSE == ip_param->raw) {
      GST_DEBUG ("Gop Length %u", enc_param->gop_len);
      GST_DEBUG ("No. of b-frames %u", enc_param->b_frame);
      GST_DEBUG ("Bitrate %u", enc_param->bitrate);
      GST_DEBUG ("Encode name %s", enc_param->enc_name);
      GST_DEBUG ("Enable L2Cache %d", enc_param->enable_l2Cache);
      GST_DEBUG ("Profile %d", enc_param->profile);
      GST_DEBUG ("QP mode %d", enc_param->qp_mode);
      GST_DEBUG ("Rate Control mode %d", enc_param->rc_mode);
      GST_DEBUG ("Slice %u", enc_param->slice);
    }
    GST_DEBUG ("Duration %u minute/s", op_param->duration);
    GST_DEBUG ("Store file path %s",   op_param->file_out);
    GST_DEBUG ("Host ip address %s",   op_param->host_ip);
    GST_DEBUG ("output sink type %d",  cmn_param->sink_type);
}


void
fetch_tag (const GstTagList * list, const gchar * tag, gpointer user_data) {
    uint br =0;
    vgst_playback *play_ptr = (vgst_playback *)user_data;
    if (!play_ptr->file_br && gst_tag_list_get_uint (list, "bitrate", &br)) {
      play_ptr->file_br = br;
      GST_DEBUG ("Average bit rate value : %u", play_ptr->file_br);
    }
}


gboolean
bus_callback (GstBus *bus, GstMessage *msg, gpointer ptr) {
    vgst_playback *play_ptr = (vgst_playback *)ptr;
    switch (GST_MESSAGE_TYPE (msg)) {
    case GST_MESSAGE_EOS:
      GST_DEBUG ("End of stream");
      if (!play_ptr->stop_flag && g_strrstr (GST_ELEMENT_NAME(play_ptr->ip_src), FILE_SRC_NAME)) {
        if (gst_element_seek_simple (play_ptr->pipeline, GST_FORMAT_TIME, GST_SEEK_FLAG_FLUSH | GST_SEEK_FLAG_ACCURATE, 0))
          GST_DEBUG ("seeking to %d succeeded", 0);
        else
          GST_ERROR ("seeking to %d failed", 0);
        break;
      }
      play_ptr->eos_flag = TRUE;
      if (play_ptr->loop && g_main_is_running (play_ptr->loop)) {
        GST_DEBUG ("Quitting the loop");
        g_main_loop_quit (play_ptr->loop);
        g_main_loop_unref (play_ptr->loop);
      }
      break;
    case GST_MESSAGE_ERROR: {
      gchar  *debug;
      GError *error;
      gst_message_parse_error (msg, &error, &debug);
      g_free (debug);
      GST_ERROR ("Error: %s   src[%s]", error->message, GST_OBJECT_NAME(msg->src));
      if (error && !play_ptr->err_msg) {
        play_ptr->err_msg = g_strdup (error->message);
      }
      // playback can't continue in error condition
      play_ptr->err_flag = TRUE;
      g_error_free (error);
      break;
    }
    case GST_MESSAGE_TAG : {
      GstTagList *tags = NULL;
      gst_message_parse_tag (msg, &tags);
      if (tags) {
        gst_tag_list_foreach (tags, fetch_tag, play_ptr);
        gst_tag_list_unref (tags);
      }
      break;
    }
    default:
      break;
    }
    return TRUE;
}


VGST_ERROR_LOG
vgst_create_elements (vgst_ip_params *ip_param, vgst_enc_params *enc_param, vgst_op_params *op_param, vgst_playback *play_ptr, vgst_cmn_params *cmn_param, vgst_sdx_filter_params *filter_param) {
    guint i =0;
    gint ret;
    for (i =0; i < cmn_param->num_src; i++) {
      if (SPLIT_SCREEN == cmn_param->sink_type) {
        if (!(ret = vgst_create_split_pipeline (&ip_param[i], &enc_param[i], &play_ptr[i]))) {
          GST_DEBUG ("Succeed to create pipeline !!!");
        } else {
          GST_ERROR ("failed to create pipeline !!!");
          return ret;
        }

        // set all the property for split-screen
        vgst_set_split_screen_property (&ip_param[i], &enc_param[i], &play_ptr[i], &cmn_param->plane_id);

        // linking all the elements for split screen
        if ((ret = vgst_link_split_elements (&ip_param[i], &play_ptr[i]))) {
          GST_ERROR ("Failed to link elements !!!");
          return ret;
        }
      } else {
       if (!(ret = vgst_create_pipeline (&ip_param[i], &enc_param[i], &play_ptr[i], cmn_param->sink_type, &filter_param[i]))) {
          GST_DEBUG ("Succeed to create pipeline !!!");
        } else {
          GST_ERROR ("failed to create pipeline !!!");
          return ret;
        }

        // set all the property
        vgst_set_property (&ip_param[i], &enc_param[i], &op_param[i], &play_ptr[i], cmn_param, &filter_param[i]);

        // linking all the elements
        if ((ret = vgst_link_elements (&ip_param[i], &play_ptr[i], cmn_param))) {
          GST_ERROR ("Failed to link elements !!!");
          return ret;
        }
      }
    }
    return VGST_SUCCESS;
}


void
on_fps_measurement (GstElement *fps,
        gdouble fps_value,
        gdouble drop_rate,
        gdouble avg_rate,
        gpointer   data) {
    guint *fps_num = (guint *)data;
    *fps_num = round(fps_value);
    GST_INFO ("fps is %u\n", *fps_num);
}


void
vgst_set_property (vgst_ip_params *ip_param, vgst_enc_params *enc_param, vgst_op_params *op_param, vgst_playback *play_ptr, vgst_cmn_params *cmn_param, vgst_sdx_filter_params *filter_param) {
    GstCaps *srcCaps, *encCaps = NULL;
    if (!g_strcmp0 (ip_param->src, V4L2_SRC_NAME)) {
      g_object_set (G_OBJECT(play_ptr->ip_src), "io-mode", VGST_V4L2_IO_MODE_DMABUF_EXPORT, NULL);
      if (!ip_param->raw && (ip_param->filter_type == SDX_FILTER)) {
        g_object_set (G_OBJECT(play_ptr->ip_src), "io-mode", ip_param->io_mode, NULL);
      }
      g_object_set (G_OBJECT(play_ptr->ip_src), "device", vlib_get_devname(ip_param->device_type), NULL);
    }
    if (!ip_param->raw && (ip_param->filter_type == SDX_FILTER)) {
      g_object_set (G_OBJECT (play_ptr->videofilter),  "filter-mode",       filter_param->filter_mode, NULL );
    } else if (!ip_param->raw && !g_strcmp0 (ip_param->src, V4L2_SRC_NAME)) {
      g_object_set (G_OBJECT (play_ptr->videoenc),  "ip-mode",          VGST_ENC_IP_MODE_DMA_IMPORT, NULL );
      g_object_set (G_OBJECT (play_ptr->videoenc),  "gop-length",       enc_param->gop_len, NULL );
      g_object_set (G_OBJECT (play_ptr->videoenc),  "target-bitrate",   enc_param->bitrate, NULL );
      g_object_set (G_OBJECT (play_ptr->videoenc),  "b-frames",         enc_param->b_frame, NULL );
      g_object_set (G_OBJECT (play_ptr->videoenc),  "num-slices",       enc_param->slice,   NULL );
      g_object_set (G_OBJECT (play_ptr->videoenc),  "control-rate",     enc_param->rc_mode, NULL );
      g_object_set (G_OBJECT (play_ptr->videoenc),  "qp-mode",          enc_param->qp_mode, NULL );

      if (enc_param->enable_l2Cache) {
          g_object_set (G_OBJECT (play_ptr->videoenc),  "prefetch-buffer",  TRUE, NULL);
      }

      g_object_set (G_OBJECT (play_ptr->videodec),  "internal-entropy-buffers",  DEC_BUFFERING_COUNT,  NULL );

      gchar * profile;
      profile = enc_param->profile == BASELINE_PROFILE ? "baseline" : enc_param->profile == MAIN_PROFILE ? "main" : "high";
      if (!g_strcmp0(enc_param->enc_name, H264_ENC_NAME)) {
        encCaps  = gst_caps_new_simple ("video/x-h264",
                                        "profile",   G_TYPE_STRING,     profile,
                                        NULL);
      } else if (!g_strcmp0(enc_param->enc_name, H265_ENC_NAME)) {
        encCaps  = gst_caps_new_simple ("video/x-h265",
                                        "profile",   G_TYPE_STRING,     profile,
                                        NULL);
      }
      GST_DEBUG ("new Caps for enc capsfilter %s",gst_caps_to_string(encCaps));
      g_object_set (G_OBJECT (play_ptr->enccapsfilter),  "caps",  encCaps, NULL);
      gst_caps_unref (encCaps);
    }
    if (ip_param->filter_type == SDX_FILTER) {
	srcCaps = gst_caps_new_full (
		gst_structure_new ("video/x-raw",
                           "width",     G_TYPE_INT,        ip_param->width,
                           "height",    G_TYPE_INT,        ip_param->height,
                           "format",    G_TYPE_STRING,     ip_param->format,
                           "framerate", GST_TYPE_FRACTION, cmn_param->monitor_refresh_rate, MAX_FRAME_RATE_DENOM,
                           NULL),
                gst_structure_new ("video/x-raw",
			   "width",     G_TYPE_INT,        ip_param->width,
			   "height",    G_TYPE_INT,        ip_param->height,
			   "format",    G_TYPE_STRING,     ip_param->format,
			   NULL),
		NULL);
    } else {
        srcCaps  = gst_caps_new_simple ("video/x-raw",
                           "width",     G_TYPE_INT,        ip_param->width,
                           "height",    G_TYPE_INT,        ip_param->height,
                           "format",    G_TYPE_STRING,     ip_param->format,
                           "framerate", GST_TYPE_FRACTION, cmn_param->monitor_refresh_rate, MAX_FRAME_RATE_DENOM,
                           NULL);
    }
    if (!g_strcmp0 (ip_param->src, FILE_SRC_NAME) && ip_param->filter_type == SDX_FILTER) {
        g_object_set (G_OBJECT (play_ptr->srccapsfilter),  "format",  4, NULL);
        g_object_set (G_OBJECT (play_ptr->srccapsfilter),  "width",   ip_param->width, NULL);
        g_object_set (G_OBJECT (play_ptr->srccapsfilter),  "height",  ip_param->height, NULL);
        gst_caps_unref (srcCaps);
    } else {
        GST_DEBUG ("Caps for src capsfilter %s",gst_caps_to_string(srcCaps));
        g_object_set (G_OBJECT (play_ptr->srccapsfilter),  "caps",  srcCaps, NULL);
        gst_caps_unref (srcCaps);
    }

    if (cmn_param->sink_type == RECORD) {
      g_object_set (G_OBJECT (play_ptr->videosink), "location",    op_param->file_out, NULL);
      g_object_set (G_OBJECT (play_ptr->ip_src),    "num-buffers", op_param->duration*cmn_param->monitor_refresh_rate*GST_MINUTE, NULL);
      g_object_set (G_OBJECT (play_ptr->videosink), "sync",        FALSE, NULL);
    } else if (cmn_param->sink_type == DISPLAY) {
      g_object_set (G_OBJECT (play_ptr->fpsdisplaysink), "fps-update-interval",     FPS_UPDATE_INTERVAL, NULL);
      g_object_set (G_OBJECT (play_ptr->fpsdisplaysink), "signal-fps-measurements", TRUE, NULL);
      g_object_set (G_OBJECT (play_ptr->fpsdisplaysink), "text-overlay",            FALSE, NULL);
      g_object_set (G_OBJECT (play_ptr->fpsdisplaysink), "sync",                    FALSE, NULL);
      if (cmn_param->driver_type == DP) {
          GST_DEBUG ("only setting bus id\n");
          g_object_set (G_OBJECT (play_ptr->videosink), "bus-id",              DP_BUS_ID, NULL);
          //g_object_set (G_OBJECT (play_ptr->videosink), "plane-id",            cmn_param->plane_id, NULL);
          //g_object_set (G_OBJECT (play_ptr->videosink), "driver-name",         DP_DRIVER_NAME, NULL);
      } else if (cmn_param->driver_type == HDMI_Tx) {
        g_object_set (G_OBJECT (play_ptr->videosink), "bus-id",                MIXER_BUS_ID, NULL);
        g_object_set (G_OBJECT (play_ptr->videosink), "plane-id",              cmn_param->plane_id, NULL);
        g_object_set (G_OBJECT (play_ptr->videosink), "driver-name",           HDMI_DRIVER_NAME, NULL);
      }
      g_object_set (G_OBJECT (play_ptr->fpsdisplaysink), "video-sink",         play_ptr->videosink, NULL);
      g_signal_connect (play_ptr->fpsdisplaysink,        "fps-measurements",   G_CALLBACK (on_fps_measurement), &play_ptr->fps_num[0]);
      cmn_param->plane_id++;
    } else if (cmn_param->sink_type == STREAM_OUT) {
      g_object_set (G_OBJECT (play_ptr->stream_sink), "host",        op_param->host_ip, NULL);
      g_object_set (G_OBJECT (play_ptr->stream_sink), "max-bitrate", enc_param->bitrate, NULL);
      g_object_set (G_OBJECT (play_ptr->stream_sink), "port",        op_param->port_num, NULL);
      g_object_set (G_OBJECT (play_ptr->stream_sink), "qos-dscp",    QOS_DSCP_VALUE, NULL);
      g_object_set (G_OBJECT (play_ptr->stream_sink), "async",       FALSE, NULL);
      g_object_set (G_OBJECT (play_ptr->stream_sink), "sync",        FALSE, NULL);
      g_object_set (G_OBJECT (play_ptr->stream_sink), "buffer-size", 8000000, NULL);
    }
    if (play_ptr->queue) {
      g_object_set (G_OBJECT (play_ptr->queue), "max-size-bytes", 0, NULL);
      g_object_set (G_OBJECT (play_ptr->queue), "max-size-buffers", 5, NULL);
    }
}


void
on_pad_added (GstElement *element,
              GstPad     *pad,
              gpointer   data) {
    gchar *str;
    vgst_playback *play_ptr = (vgst_playback *)data;
    GstCaps *caps = gst_pad_get_current_caps (pad);
    str = gst_caps_to_string(caps);
    if (g_str_has_prefix (str, "video/")) {
      if (!gst_element_link_many(play_ptr->ip_src, play_ptr->queue, play_ptr->fpsdisplaysink, NULL)) {
        GST_ERROR ("Error linking for ip_src --> queue --> fpsdisplaysink");
      } else {
        GST_DEBUG ("Linked for ip_src --> queue --> fpsdisplaysink successfully");
      }
    }
}


VGST_ERROR_LOG
link_streaming_pipeline (vgst_playback *play_ptr) {
    gint ret = VGST_SUCCESS;

    if (!gst_element_link_many (play_ptr->ip_src, play_ptr->srccapsfilter, play_ptr->queue, play_ptr->videoenc, play_ptr->enccapsfilter, play_ptr->rtppay, play_ptr->stream_sink, NULL)) {
      GST_ERROR ("Error linking for ip_src --> srccapsfilter --> queue --> videoenc --> enccapsfilter --> rtppay  --> stream_sink");
      ret = VGST_ERROR_PIPELINE_LINKING_FAIL;
    } else {
      GST_DEBUG ("Linked for ip_src --> srccapsfilter --> queue --> videoenc --> enccapsfilter --> rtppay  --> stream_sink successfully");
    }

    return ret;
}


VGST_ERROR_LOG
vgst_link_elements (vgst_ip_params *ip_param, vgst_playback *play_ptr, vgst_cmn_params *cmn_param) {
    if (!g_strcmp0 (ip_param->src, FILE_SRC_NAME) && (ip_param->filter_type == SDX_FILTER)) {
      g_object_set (G_OBJECT(play_ptr->ip_src), "location", ip_param->uri, NULL);
    } else if (!g_strcmp0 (ip_param->src, FILE_SRC_NAME) && g_str_has_prefix (ip_param->uri, "file://")) {
      g_object_set (G_OBJECT(play_ptr->ip_src), "uri", ip_param->uri, NULL);
      g_signal_connect (play_ptr->ip_src, "pad-added", G_CALLBACK (on_pad_added), play_ptr);
      return VGST_SUCCESS;
    }

    if (ip_param->raw == FALSE && (ip_param->filter_type == SDX_FILTER)) {
      if (cmn_param->sink_type == DISPLAY) {
        if (!gst_element_link_many (play_ptr->ip_src, play_ptr->srccapsfilter, play_ptr->videofilter, play_ptr->queue, play_ptr->fpsdisplaysink, NULL)) {
          GST_ERROR ("Error linking for ip_src --> capsfilter --> videofilter --> queue --> fpsdisplaysink");
          return VGST_ERROR_PIPELINE_LINKING_FAIL;
        } else {
          GST_DEBUG ("Linked for ip_src --> capsfilter --> videofilter --> queue --> fpsdisplaysink successfully");
        }
      }
    } else if (ip_param->raw == FALSE) {
      if (cmn_param->sink_type == DISPLAY) {
        if (!gst_element_link_many (play_ptr->ip_src, play_ptr->srccapsfilter, play_ptr->videoenc, play_ptr->enccapsfilter, play_ptr->enc_queue,
                                    play_ptr->videoparser, play_ptr->videodec, play_ptr->queue, play_ptr->fpsdisplaysink, NULL)) {
          GST_ERROR ("Error linking for ip_src --> capsfilter --> videoenc --> enccapsfilter --> queue --> parser --> videodec --> queue --> fpsdisplaysink");
          return VGST_ERROR_PIPELINE_LINKING_FAIL;
        } else {
          GST_DEBUG ("Linked for ip_src --> capsfilter --> videoenc --> enccapsfilter --> queue --> parser --> videodec --> queue --> fpsdisplaysink successfully");
        }
      } else if (cmn_param->sink_type == STREAM_OUT) {
        if ( VGST_SUCCESS != link_streaming_pipeline (play_ptr) )
          return VGST_ERROR_PIPELINE_LINKING_FAIL;
      } else if (cmn_param->sink_type == RECORD) {
        if (!gst_element_link_many (play_ptr->ip_src, play_ptr->srccapsfilter, play_ptr->videoenc, play_ptr->enc_queue, play_ptr->enccapsfilter,
                                    play_ptr->videoparser, play_ptr->mux, play_ptr->videosink, NULL)) {
          GST_ERROR ("Error linking for ip_src --> capsfilter --> videoenc --> queue --> capsfilter --> videoparser --> mux --> videosink");
          return VGST_ERROR_PIPELINE_LINKING_FAIL;
        } else {
          GST_DEBUG ("Linked for ip_src --> capsfilter --> videoenc --> queue --> capsfilter --> videoparser --> mux --> videosink successfully");
        }
      }
    } else {
      // It Comes here means need to use raw path means src --> sink path
      if (!gst_element_link_many (play_ptr->ip_src, play_ptr->srccapsfilter, play_ptr->fpsdisplaysink, NULL)) {
        GST_ERROR ("Error linking for testsrc --> capsfilter --> fpsdisplaysink");
        return VGST_ERROR_PIPELINE_LINKING_FAIL;
      } else {
        GST_DEBUG ("Linked for ip_src --> capsfilter --> fpsdisplaysink successfully");
      }
    }
    return VGST_SUCCESS;
}
