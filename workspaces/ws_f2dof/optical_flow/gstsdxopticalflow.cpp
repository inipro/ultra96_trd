/*
 * Copyright (C) 2017 – 2018 Xilinx, Inc.  All rights reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * Use of the Software is limited solely to applications: (a) running on a
 * Xilinx device, or (b) that interact with a Xilinx device through a bus or
 * interconnect.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
 * XILINX BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
 * ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
 * WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 * Except as contained in this notice, the name of the Xilinx shall not be used
 * in advertising or otherwise to promote the sale, use or other dealings in
 * this Software without prior written authorization from Xilinx.
 */

/**
 * SECTION:sdxoptiocalflow
 *
 * This element draw the computed optical flow using an SDx accelerator.
 *
 * <refsect2>
 * <title>Example launch line</title>
 * |[
 * gst-launch videotestsrc ! sdxopticalflow ! kmssink
 * ]|
 * </refsect2>
 */

#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

#include <optical_flow_sds.h>

#include "gstsdxopticalflow.h"

GST_DEBUG_CATEGORY_STATIC (gst_sdx_optical_flow_debug);
#define GST_CAT_DEFAULT gst_sdx_optical_flow_debug

#define SDX_OPTICAL_FLOW_FRAME_INPUTS_PER_OUTPUT      2

#define SDX_OPTICAL_FLOW_CAPS \
    "video/x-raw, " \
    "format = (string) {YUY2, UYVY}, " \
    "width = (int) [ 1, 1920 ], " \
    "height = (int) [ 1, 1080 ], " \
    "framerate = " GST_VIDEO_FPS_RANGE

static GstStaticPadTemplate src_template = GST_STATIC_PAD_TEMPLATE ("src",
    GST_PAD_SRC,
    GST_PAD_ALWAYS,
    GST_STATIC_CAPS (SDX_OPTICAL_FLOW_CAPS)
    );

static GstStaticPadTemplate sink_template = GST_STATIC_PAD_TEMPLATE ("sink",
    GST_PAD_SINK,
    GST_PAD_ALWAYS,
    GST_STATIC_CAPS (SDX_OPTICAL_FLOW_CAPS)
    );

#define gst_sdx_optical_flow_parent_class parent_class
G_DEFINE_TYPE (GstSdxOpticalFlow, gst_sdx_optical_flow, GST_TYPE_SDX_BASE);

static uint32_t
video_format_to_fourcc (GstVideoFormat fmt)
{
  switch (fmt) {
    case GST_VIDEO_FORMAT_YUY2:
      return GST_MAKE_FOURCC ('Y', 'U', 'Y', 'V');
    case GST_VIDEO_FORMAT_UYVY:
      return GST_MAKE_FOURCC ('U', 'Y', 'V', 'Y');
    default:
      return 0;
  }
}

static gboolean
gst_sdx_optical_flow_set_caps (GstBaseTransform * trans, GstCaps * in_caps,
    GstCaps * out_caps)
{
  GstSdxBase *base = GST_SDX_BASE (trans);
  GstSdxOpticalFlow *filter = GST_SDX_OPTICAL_FLOW (trans);
  GstVideoInfo info;

  if (gst_sdx_base_get_filter_mode (base) != GST_SDX_BASE_FILTER_MODE_HW) {
    GST_ELEMENT_ERROR (base, STREAM, NOT_IMPLEMENTED,
        ("Optical flow does not support SW mode"), (NULL));
    return FALSE;
  }

  /* Caps is used here only to initialize optical flow without modifying */
  GST_DEBUG_OBJECT (filter, "in caps : %" GST_PTR_FORMAT, in_caps);

  if (!gst_video_info_from_caps (&info, in_caps)) {
    GST_WARNING_OBJECT (filter, "failed to get video info from caps");
    return FALSE;
  }

  optical_flow_init_sds (GST_VIDEO_INFO_HEIGHT (&info),
      GST_VIDEO_INFO_WIDTH (&info), GST_VIDEO_INFO_HEIGHT (&info),
      GST_VIDEO_INFO_WIDTH (&info),
      video_format_to_fourcc (GST_VIDEO_INFO_FORMAT (&info)),
      video_format_to_fourcc (GST_VIDEO_INFO_FORMAT (&info)), &filter->data);

  return TRUE;
}

static GstFlowReturn
gst_sdx_optical_flow_process_frames (GstSdxBase * base,
    GstSdxFrame ** in_frames, GstSdxFrame * out_frame)
{
  GstSdxOpticalFlow *filter = GST_SDX_OPTICAL_FLOW (base);
  GstVideoFrame *out_vframe = &out_frame->vframe;
  gushort *data_in1;
  gushort *data_in2;
  gushort *data_out;

  data_in1 = (gushort *) GST_VIDEO_FRAME_PLANE_DATA (&in_frames[0]->vframe, 0);
  data_in2 = (gushort *) GST_VIDEO_FRAME_PLANE_DATA (&in_frames[1]->vframe, 0);
  data_out = (gushort *) GST_VIDEO_FRAME_PLANE_DATA (&out_frame->vframe, 0);

  optical_flow_sds (data_in2, data_in1, data_out,
      GST_VIDEO_FRAME_HEIGHT (out_vframe), GST_VIDEO_FRAME_WIDTH (out_vframe),
      filter->data);

  GST_LOG_OBJECT (base, "input frames processed");

  return GST_FLOW_OK;
}

static gboolean
gst_sdx_optical_flow_stop (GstBaseTransform * trans)
{
  GstSdxBase *base = GST_SDX_BASE (trans);
  GstSdxOpticalFlow *filter = GST_SDX_OPTICAL_FLOW (trans);

  if (!filter->data)
    return TRUE;

  if (gst_sdx_base_get_filter_mode (base) == GST_SDX_BASE_FILTER_MODE_HW)
    optical_flow_deinit_sds (filter->data);

  filter->data = NULL;

  if (GST_BASE_TRANSFORM_CLASS (parent_class)->stop)
    return GST_BASE_TRANSFORM_CLASS (parent_class)->stop (trans);

  return TRUE;
}

static void
gst_sdx_optical_flow_init (GstSdxOpticalFlow * filter)
{
  GstSdxBase *base = GST_SDX_BASE (filter);
  gst_sdx_base_set_filter_mode (base, GST_SDX_BASE_FILTER_MODE_HW);
  gst_sdx_base_set_inputs_per_output (base,
      SDX_OPTICAL_FLOW_FRAME_INPUTS_PER_OUTPUT);
}

static void
gst_sdx_optical_flow_class_init (GstSdxOpticalFlowClass * klass)
{
  GstElementClass *element_class;
  GstBaseTransformClass *transform_class;
  GstSdxBaseClass *sdxbase_class;

  element_class = (GstElementClass *) klass;
  transform_class = GST_BASE_TRANSFORM_CLASS (klass);
  sdxbase_class = GST_SDX_BASE_CLASS (klass);

  transform_class->stop = GST_DEBUG_FUNCPTR (gst_sdx_optical_flow_stop);
  transform_class->set_caps = GST_DEBUG_FUNCPTR (gst_sdx_optical_flow_set_caps);
  sdxbase_class->process_frames =
      GST_DEBUG_FUNCPTR (gst_sdx_optical_flow_process_frames);

  gst_element_class_set_static_metadata (element_class,
      "SDx Optical Flow",
      "Filter/Effect/Video",
      "SDx Optical Flow", "Sören Brinkmann <soren.brinkmann@xilinx.com>");

  gst_element_class_add_static_pad_template (element_class, &sink_template);
  gst_element_class_add_static_pad_template (element_class, &src_template);
}

static gboolean
plugin_init (GstPlugin * plugin)
{
  GST_DEBUG_CATEGORY_INIT (gst_sdx_optical_flow_debug, "sdxopticalflow", 0,
      "SDx Optical Flow");

  return gst_element_register (plugin, "sdxopticalflow",
      GST_RANK_NONE, GST_TYPE_SDX_OPTICAL_FLOW);
}

/* PACKAGE: this is usually set by autotools depending on some _INIT macro
 * in configure.ac and then written into and defined in config.h, but we can
 * just set it ourselves here in case someone doesn't use autotools to
 * compile this code. GST_PLUGIN_DEFINE needs PACKAGE to be defined.
 */
#ifndef PACKAGE
#define PACKAGE "sdxopticalflow"
#endif

GST_PLUGIN_DEFINE (GST_VERSION_MAJOR,
    GST_VERSION_MINOR,
    sdxopticalflow,
    "SDx Optical Flow plugin",
    plugin_init, "0.1", "LGPL", "GStreamer", "http://xilinx.com/")
