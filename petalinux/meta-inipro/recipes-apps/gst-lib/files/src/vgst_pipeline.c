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

/*
 * vgst_single_pipeline.c
 *
 *  Created on: Nov 20, 2017
 *      Author: saketb
 */

#include "vgst_utils.h"

VGST_ERROR_LOG
vgst_create_pipeline (vgst_ip_params *ip_param, vgst_enc_params *enc_param, vgst_playback *play_ptr, guint sink_type, vgst_sdx_filter_params *filter_param) {
    play_ptr->pipeline        = gst_pipeline_new ("vcu-trd");
    play_ptr->ip_src          = gst_element_factory_make (ip_param->src,    NULL);
    play_ptr->srccapsfilter   = gst_element_factory_make ("capsfilter",     NULL);
    play_ptr->queue           = gst_element_factory_make ("queue",          NULL);
    play_ptr->enc_queue       = gst_element_factory_make ("queue",          NULL);
    play_ptr->enccapsfilter   = gst_element_factory_make ("capsfilter",     NULL);
    if (!g_strcmp0 (ip_param->src, FILE_SRC_NAME) && (ip_param->filter_type == SDX_FILTER))
      play_ptr->srccapsfilter  = gst_element_factory_make ("videoparse",  NULL);
    if (sink_type == DISPLAY) {
      play_ptr->videosink       = gst_element_factory_make ("kmssink",       NULL);
      play_ptr->fpsdisplaysink  = gst_element_factory_make ("fpsdisplaysink",NULL);
    } else if (sink_type == RECORD) {
      play_ptr->videosink       = gst_element_factory_make ("filesink",     NULL);
      play_ptr->mux             = gst_element_factory_make ("qtmux",        NULL);
    } else if (sink_type == STREAM_OUT) {
      play_ptr->stream_sink     = gst_element_factory_make ("udpsink",      NULL);
      play_ptr->tee             = gst_element_factory_make ("tee",          NULL);
    }

    if (!play_ptr->pipeline || !play_ptr->ip_src || !play_ptr->srccapsfilter || !play_ptr->queue || !play_ptr->enc_queue || !play_ptr->enccapsfilter) {
      GST_ERROR ("FAILED to create common element");
      return VGST_ERROR_PIPELINE_CREATE_FAIL;
    } else {
      GST_DEBUG ("All common elements are created");
    }
    gst_bin_add_many (GST_BIN(play_ptr->pipeline), play_ptr->ip_src, play_ptr->srccapsfilter, play_ptr->queue, play_ptr->enc_queue,
                      play_ptr->enccapsfilter, NULL);

    if (sink_type == STREAM_OUT) {
      if (!play_ptr->tee || !play_ptr->stream_sink) {
        GST_ERROR ("FAILED to create stream-out elements");
        return VGST_ERROR_PIPELINE_CREATE_FAIL;
      } else {
        GST_DEBUG ("All stream-out elements are created");
      }
      gst_bin_add_many (GST_BIN(play_ptr->pipeline), play_ptr->tee, play_ptr->stream_sink, NULL);
    } else if (sink_type == RECORD) {
      if (!play_ptr->videosink || !play_ptr->mux) {
        GST_ERROR ("FAILED to create record elements");
        return VGST_ERROR_PIPELINE_CREATE_FAIL;
      } else {
        GST_DEBUG ("All record elements are created");
      }
      gst_bin_add_many (GST_BIN(play_ptr->pipeline), play_ptr->videosink, play_ptr->mux, NULL);
    } else if (sink_type == DISPLAY) {
      if (!play_ptr->videosink || !play_ptr->fpsdisplaysink) {
        GST_ERROR ("FAILED to create display elements");
        return VGST_ERROR_PIPELINE_CREATE_FAIL;
      } else {
        GST_DEBUG ("All display elements are created");
      }
      gst_bin_add_many (GST_BIN(play_ptr->pipeline), play_ptr->fpsdisplaysink, NULL);
    }

    if (!ip_param->raw && (ip_param->filter_type == SDX_FILTER)) {
      play_ptr->videofilter = gst_element_factory_make (filter_param->filter_name, NULL);
      if (!play_ptr->videofilter) {
        GST_ERROR ("FAILED to create videofilter elements");
        return VGST_ERROR_PIPELINE_CREATE_FAIL;
      }
      gst_bin_add_many(GST_BIN(play_ptr->pipeline), play_ptr->videofilter, NULL);
    } else if (!ip_param->raw && !g_strcmp0 (ip_param->src, V4L2_SRC_NAME)) {
      play_ptr->videoenc    = gst_element_factory_make (enc_param->enc_name, NULL);
      if (!g_strcmp0(enc_param->enc_name, H264_ENC_NAME)) {
        play_ptr->videodec    = gst_element_factory_make (H264_DEC_NAME,          NULL);
        play_ptr->videoparser = gst_element_factory_make (H264_PARSER_NAME,       NULL);
        play_ptr->rtppay      = gst_element_factory_make (H264_RTP_PAYLOAD_NAME,  NULL);
      } else if (!g_strcmp0(enc_param->enc_name, H265_ENC_NAME)) {
        play_ptr->videodec    = gst_element_factory_make (H265_DEC_NAME,          NULL);
        play_ptr->videoparser = gst_element_factory_make (H265_PARSER_NAME,       NULL);
        play_ptr->rtppay      = gst_element_factory_make (H265_RTP_PAYLOAD_NAME,  NULL);
      }
      if (!play_ptr->videoenc || !play_ptr->videodec || !play_ptr->videoparser || !play_ptr->rtppay) {
        GST_ERROR ("FAILED to create enc-dec-parser-payload elements");
        return VGST_ERROR_PIPELINE_CREATE_FAIL;
      }
      gst_bin_add_many(GST_BIN(play_ptr->pipeline), play_ptr->videoenc, play_ptr->videodec, play_ptr->videoparser, play_ptr->rtppay, NULL);
    }
    return VGST_SUCCESS;
}

gint
vgst_stop  (vgst_playback *play_ptr, vgst_ip_params *ip_param, guint sink_type) {
    if (!play_ptr || !play_ptr->pipeline) {
      GST_ERROR ("Pipeline handle is null");
      return VGST_ERROR_PIPELINE_HANDLE_NULL;
    }
    play_ptr->stop_flag = TRUE;
    GstBus *bus;
    if (!play_ptr->eos_flag && !play_ptr->err_flag) {
      if (CSI != ip_param->device_type) {
        GST_DEBUG ("sending eos event");
        GstEvent *event = gst_event_new_eos();
        if (event) {
          if (gst_element_send_event (play_ptr->ip_src, event)) {
            GST_DEBUG ("send event to src element Succeed");
          } else
            GST_ERROR("send event to src element failed");
        }
        play_ptr->loop = g_main_loop_new (NULL, FALSE);
        g_main_loop_run (play_ptr->loop);
      }
    }

    GST_DEBUG ("setting to NULL state");
    if (GST_STATE_CHANGE_FAILURE == gst_element_set_state (play_ptr->pipeline, GST_STATE_NULL)) {
      GST_ERROR ("state change is failed");
      return VGST_ERROR_STATE_CHANGE_FAIL;
    }
    if (play_ptr->pipeline) {
      if (play_ptr->pad1) {
        GST_DEBUG ("releasing pads");
        gst_element_release_request_pad (play_ptr->tee, play_ptr->pad1);
        gst_object_unref (play_ptr->pad1);
      }
      GST_DEBUG ("removing  bus");
      bus = gst_pipeline_get_bus (GST_PIPELINE (play_ptr->pipeline));
      if (bus) {
        gst_bus_remove_watch (bus);
        gst_object_unref (bus);
      }
      if (play_ptr->err_msg) {
        g_free (play_ptr->err_msg);
        play_ptr->err_msg = NULL;
      }
      gst_object_unref (GST_OBJECT (play_ptr->pipeline));
      play_ptr->pipeline = NULL;
    }
    GST_DEBUG ("returning from stop");
    return VGST_SUCCESS;
}



VGST_ERROR_LOG
vgst_create_split_pipeline (vgst_ip_params *ip_param, vgst_enc_params *enc_param, vgst_playback *play_ptr) {
    play_ptr->pipeline        = gst_pipeline_new ("vcu-trd");
    play_ptr->ip_src          = gst_element_factory_make (ip_param->src,    NULL);
    play_ptr->srccapsfilter   = gst_element_factory_make ("capsfilter",     NULL);
    play_ptr->queue           = gst_element_factory_make ("queue",          NULL);
    play_ptr->enc_queue       = gst_element_factory_make ("queue",          NULL);
    play_ptr->enccapsfilter   = gst_element_factory_make ("capsfilter",     NULL);
    play_ptr->videosink       = gst_element_factory_make ("kmssink", NULL);
    play_ptr->fpsdisplaysink  = gst_element_factory_make ("fpsdisplaysink", NULL);
    play_ptr->videosink2      = gst_element_factory_make ("kmssink", NULL);
    play_ptr->fpsdisplaysink2 = gst_element_factory_make ("fpsdisplaysink", NULL);

    play_ptr->tee             = gst_element_factory_make ("tee",NULL);
    play_ptr->videoenc        = gst_element_factory_make (enc_param->enc_name,  NULL);
    if (!g_strcmp0(enc_param->enc_name, H264_ENC_NAME)) {
      play_ptr->videodec    = gst_element_factory_make (H264_DEC_NAME,      NULL);
    } else if (!g_strcmp0(enc_param->enc_name, H265_ENC_NAME)) {
      play_ptr->videodec    = gst_element_factory_make (H265_DEC_NAME,      NULL);
    }
    if (!play_ptr->pipeline || !play_ptr->ip_src || !play_ptr->srccapsfilter || !play_ptr->queue || !play_ptr->enc_queue) {
      GST_ERROR ("FAILED to create elements required for split-screen");
      return VGST_ERROR_PIPELINE_CREATE_FAIL;
    }
    if (!play_ptr->enccapsfilter || !play_ptr->videosink || !play_ptr->fpsdisplaysink || !play_ptr->tee || !play_ptr->videoenc \
        || !play_ptr->videodec || !play_ptr->videosink2  || !play_ptr->fpsdisplaysink2) {
      GST_ERROR ("FAILED to create elements required for split-screen");
      return VGST_ERROR_PIPELINE_CREATE_FAIL;
    }
    gst_bin_add_many (GST_BIN(play_ptr->pipeline), play_ptr->ip_src, play_ptr->srccapsfilter, play_ptr->queue, play_ptr->enc_queue,NULL);
    gst_bin_add_many (GST_BIN(play_ptr->pipeline), play_ptr->enccapsfilter, play_ptr->fpsdisplaysink, NULL);
    gst_bin_add_many (GST_BIN(play_ptr->pipeline), play_ptr->videoenc, play_ptr->videodec, play_ptr->tee, NULL);
    gst_bin_add_many (GST_BIN(play_ptr->pipeline), play_ptr->fpsdisplaysink2, NULL);
    return VGST_SUCCESS;
}

void
vgst_set_split_screen_property (vgst_ip_params *ip_param, vgst_enc_params *enc_param, vgst_playback *play_ptr, guint *plane_id) {
    GstCaps *srcCaps, *encCaps = NULL;

    if (!g_strcmp0 (ip_param->src, V4L2_SRC_NAME)) {
      g_object_set (G_OBJECT(play_ptr->ip_src), "io-mode", VGST_V4L2_IO_MODE_DMABUF_EXPORT, NULL);
      g_object_set (G_OBJECT(play_ptr->ip_src), "device", vlib_get_devname(ip_param->device_type), NULL);
    }

    if (!ip_param->raw && !g_strcmp0 (ip_param->src, V4L2_SRC_NAME)) {
      g_object_set (G_OBJECT (play_ptr->videoenc),  "ip-mode",          VGST_ENC_IP_MODE_DMA_IMPORT, NULL );
      g_object_set (G_OBJECT (play_ptr->videoenc),  "gop-length",       enc_param->gop_len, NULL );
      g_object_set (G_OBJECT (play_ptr->videoenc),  "gop-freq-idr",     enc_param->gop_len, NULL );
      g_object_set (G_OBJECT (play_ptr->videoenc),  "target-bitrate",   enc_param->bitrate, NULL );
      g_object_set (G_OBJECT (play_ptr->videoenc),  "b-frames",         enc_param->b_frame, NULL );
      g_object_set (G_OBJECT (play_ptr->videoenc),  "num-slices",       enc_param->slice,   NULL );
      g_object_set (G_OBJECT (play_ptr->videoenc),  "control-rate",     enc_param->rc_mode, NULL );
      g_object_set (G_OBJECT (play_ptr->videoenc),  "qp-mode",          enc_param->qp_mode, NULL );
      if (enc_param->enable_l2Cache) {
        g_object_set (G_OBJECT (play_ptr->videoenc),  "prefetch-buffer",  TRUE, NULL);
      }

      g_object_set (G_OBJECT (play_ptr->videodec),  "op-mode",          VGST_DEC_OP_MODE_DMA_EXPORT, NULL );
      g_object_set (G_OBJECT (play_ptr->videodec),  "ip-mode",          VGST_DEC_IP_MODE_ZERO_COPY,  NULL );
      g_object_set (G_OBJECT (play_ptr->videodec),  "internal-entropy-buffers",  DEC_BUFFERING_COUNT,  NULL );
      gchar * profile;
      profile = enc_param->profile == BASELINE_PROFILE ? "baseline" : enc_param->profile == MAIN_PROFILE ? "main" : "high";
      if (!g_strcmp0(enc_param->enc_name, H264_ENC_NAME)) {
        encCaps  = gst_caps_new_simple ("video/x-h264",
                                        "width",     G_TYPE_INT,        ip_param->width,
                                        "height",    G_TYPE_INT,        ip_param->height,
                                        "profile",   G_TYPE_STRING,     profile,
                                        NULL);
      } else if (!g_strcmp0(enc_param->enc_name, H265_ENC_NAME)) {
        encCaps  = gst_caps_new_simple ("video/x-h265",
                                        "width",     G_TYPE_INT,        ip_param->width,
                                        "height",    G_TYPE_INT,        ip_param->height,
                                        "profile",   G_TYPE_STRING,     profile,
                                        NULL);
      }
      GST_DEBUG ("new Caps for enc capsfilter %s",gst_caps_to_string(encCaps));
      g_object_set (G_OBJECT (play_ptr->enccapsfilter),  "caps",  encCaps, NULL);
      gst_caps_unref (encCaps);
    }
    srcCaps  = gst_caps_new_simple ("video/x-raw",
                           "width",     G_TYPE_INT,        ip_param->width,
                           "height",    G_TYPE_INT,        ip_param->height,
                           "format",    G_TYPE_STRING,     ip_param->format,
                           "framerate", GST_TYPE_FRACTION, MAX_FRAME_RATE_NUMERATOR, MAX_FRAME_RATE_DENOM,
                           NULL);

    GST_DEBUG ("Caps for src capsfilter %s",gst_caps_to_string(srcCaps));
    g_object_set (G_OBJECT (play_ptr->srccapsfilter),  "caps",  srcCaps, NULL);
    gst_caps_unref (srcCaps);

    if (play_ptr->queue) {
      g_object_set (G_OBJECT (play_ptr->queue), "max-size-bytes", 0, NULL);
    }
    if (play_ptr->enc_queue) {
      g_object_set (G_OBJECT (play_ptr->enc_queue), "max-size-bytes", 0, NULL);
    }
    g_object_set (G_OBJECT (play_ptr->fpsdisplaysink), "fps-update-interval",     FPS_UPDATE_INTERVAL, NULL);
    g_object_set (G_OBJECT (play_ptr->fpsdisplaysink), "signal-fps-measurements", TRUE, NULL);
    g_object_set (G_OBJECT (play_ptr->fpsdisplaysink), "text-overlay",            FALSE, NULL);
    g_object_set (G_OBJECT (play_ptr->fpsdisplaysink), "video-sink",              play_ptr->videosink, NULL);
    g_object_set (G_OBJECT (play_ptr->videosink),      "plane-id",                *plane_id, NULL);
    g_signal_connect (play_ptr->fpsdisplaysink,        "fps-measurements",        G_CALLBACK (on_fps_measurement), &play_ptr->fps_num[0]);

    *plane_id += 1;
    g_object_set (G_OBJECT (play_ptr->fpsdisplaysink2), "fps-update-interval",     FPS_UPDATE_INTERVAL, NULL);
    g_object_set (G_OBJECT (play_ptr->fpsdisplaysink2), "signal-fps-measurements", TRUE, NULL);
    g_object_set (G_OBJECT (play_ptr->fpsdisplaysink2), "text-overlay",            FALSE, NULL);
    g_object_set (G_OBJECT (play_ptr->fpsdisplaysink2), "video-sink",              play_ptr->videosink2, NULL);
    g_object_set (G_OBJECT (play_ptr->videosink2),      "plane-id",                *plane_id, NULL);
    g_signal_connect (play_ptr->fpsdisplaysink2,        "fps-measurements",        G_CALLBACK (on_fps_measurement), &play_ptr->fps_num[1]);
    *plane_id += 1;
}


VGST_ERROR_LOG
vgst_link_split_elements (vgst_ip_params *ip_param, vgst_playback *play_ptr) {
    gchar *name1 = NULL, *name2 = NULL;
    gint ret = VGST_SUCCESS;
    if (play_ptr->tee) {
      play_ptr->pad1 = gst_element_get_request_pad(play_ptr->tee, "src_1");
      name1 = gst_pad_get_name(play_ptr->pad1);
      play_ptr->pad2 = gst_element_get_request_pad(play_ptr->tee, "src_2");
      name2 = gst_pad_get_name(play_ptr->pad2);
    }
    if (!gst_element_link_many (play_ptr->ip_src, play_ptr->srccapsfilter, play_ptr->tee, NULL)) {
      GST_ERROR ("Error linking for ip_src --> capsfilter --> tee");
      ret = VGST_ERROR_PIPELINE_LINKING_FAIL;
      goto CLEAN_UP;
    } else {
      GST_DEBUG ("Linked for ip_src --> capsfilter --> tee successfully");
    }
    if (!gst_element_link_pads(play_ptr->tee, name1, play_ptr->videoenc, "sink")) {
      GST_ERROR ("Error linking for tee --> queue");
      ret = VGST_ERROR_PIPELINE_LINKING_FAIL;
      goto CLEAN_UP;
    } else {
      GST_DEBUG ("Linked for tee --> queue successfully");
    }
    if (!gst_element_link_pads(play_ptr->tee, name2, play_ptr->enc_queue, "sink")) {
      GST_ERROR ("Error linking for tee --> enc_queue");
      ret = VGST_ERROR_PIPELINE_LINKING_FAIL;
      goto CLEAN_UP;
    } else {
      GST_DEBUG ("Linked for tee --> enc_queue successfully");
    }
    if (!gst_element_link_many (play_ptr->enc_queue, play_ptr->fpsdisplaysink2, NULL)) {
      GST_ERROR ("Error linking for enc_queue --> fpsdisplaysink2");
      ret = VGST_ERROR_PIPELINE_LINKING_FAIL;
      goto CLEAN_UP;
    } else {
      GST_DEBUG ("Linked for enc_queue --> fpsdisplaysink2 successfully");
    }

    if (!gst_element_link_many (play_ptr->videoenc, play_ptr->enccapsfilter, play_ptr->videodec, play_ptr->queue, play_ptr->fpsdisplaysink, NULL)) {
      GST_ERROR ("Error linking for videoenc --> enccapsfilter --> videodec --> queue --> sink");
      ret = VGST_ERROR_PIPELINE_LINKING_FAIL;
      goto CLEAN_UP;
    } else {
      GST_DEBUG ("Linked for videoenc --> enccapsfilter --> videodec --> queue --> sink successfully");
    }

CLEAN_UP :
    if (name1)
      g_free (name1);
    if (name2)
      g_free (name2);

    return ret;
}


void
get_coordinates (guint *x, guint *y, guint cnt) {
	if (cnt == 0) {
		*x = 0;
		*y = 0;
	} else if (cnt == 1) {
		*x = 1920;
		*y = 0;
	} else if (cnt == 2) {
		*x = 0;
		*y = 1080;
	} else if (cnt == 3) {
		*x = 1920;
		*y = 1080;
	}
}


VGST_ERROR_LOG
vgst_run_pipeline (vgst_ip_params *ip_param, vgst_playback *play_ptr, vgst_cmn_params *cmn_param) {
    guint i =0;
    guint x =0, y = 0;
    gint ret = VGST_SUCCESS;
    for (i =0; i< cmn_param->num_src; i++) {
      if (SPLIT_SCREEN == cmn_param->sink_type) {
        play_ptr[i].overlay = GST_VIDEO_OVERLAY (play_ptr[i].videosink);
        play_ptr[i].overlay2 = GST_VIDEO_OVERLAY (play_ptr[i].videosink2);
        if (play_ptr[i].overlay) {
          ret = gst_video_overlay_set_render_rectangle (play_ptr[i].overlay, 0, i*1080, ip_param[i].width/2, ip_param[i].height);
          if (ret) {
            gst_video_overlay_expose (play_ptr[i].overlay);
            ret =0;
          }
        } else {
          GST_ERROR ("Failed to create overlay");
          return VGST_ERROR_OVERLAY_CREATION_FAIL;
        }
        if (play_ptr[i].overlay2) {
          ret = gst_video_overlay_set_render_rectangle (play_ptr[i].overlay2, 1920, i*1080, ip_param[i].width/2, ip_param[i].height);
          if (ret) {
            gst_video_overlay_expose (play_ptr[i].overlay2);
            ret =0;
          }
        } else {
          GST_ERROR ("Failed to create overlay2");
          return VGST_ERROR_OVERLAY_CREATION_FAIL;
        }
      } else if ((DISPLAY == cmn_param->sink_type && cmn_param->num_src >1)) {
        play_ptr[i].overlay = GST_VIDEO_OVERLAY (play_ptr[i].videosink);
        if (play_ptr[i].overlay) {
          get_coordinates (&x, &y, i);
          g_print ("============ x %d and y %d\n", x, y);
          ret = gst_video_overlay_set_render_rectangle (play_ptr[i].overlay, x, y, ip_param[i].width, ip_param[i].height);
          if (ret) {
            gst_video_overlay_expose (play_ptr[i].overlay);
            ret =0;
          }
        } else {
          GST_ERROR ("Failed to create overlay");
          return VGST_ERROR_OVERLAY_CREATION_FAIL;
        }
      }
      if (GST_STATE_CHANGE_FAILURE == gst_element_set_state (play_ptr[i].pipeline, GST_STATE_PLAYING))
        return VGST_ERROR_STATE_CHANGE_FAIL;
    }
    return VGST_SUCCESS;
}
