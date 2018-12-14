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
 * SECTION:sdxfilter2d
 *
 * This is an example SDX accelerated filter2d. It implements various 2D
 * filters like "edge", "blur" etc.
 *
 * <refsect2>
 * <title>Example launch line</title>
 * |[
 * gst-launch -v videotestsrc ! sdxfilter2d filter-preset=edge ! kmssink
 * ]|
 * </refsect2>
 */

#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

#include <filter2d_sds.h>

#include "gstsdxfilter2d.h"

GST_DEBUG_CATEGORY_STATIC (gst_sdx_filter2d_debug);
#define GST_CAT_DEFAULT gst_sdx_filter2d_debug

#define SDX_FILTER2D_INPUT_PER_OUTPUT      1

int filter2d_init_cv (size_t in_height, size_t in_width, size_t out_height,
    size_t out_width, uint32_t in_fourcc, uint32_t out_fourcc, void **priv);
void filter2d_cv (unsigned short *frm_data_in, unsigned short *frm_data_out,
    int height, int width, const coeff_t coeff, void *priv);
void filter2d_deinit_cv (void *priv);

enum
{
  PROP_0,
  PROP_FILTER_PRESET,
  PROP_FILTER_COEFFICIENTS
};

#define SDX_FILTER2D_CAPS \
    "video/x-raw, " \
    "format = (string) {YUY2, UYVY}, " \
    "width = (int) [ 1, 3840 ], " \
    "height = (int) [ 1, 2160 ], " \
    "framerate = " GST_VIDEO_FPS_RANGE

static GstStaticPadTemplate src_template = GST_STATIC_PAD_TEMPLATE ("src",
    GST_PAD_SRC,
    GST_PAD_ALWAYS,
    GST_STATIC_CAPS (SDX_FILTER2D_CAPS)
    );

static GstStaticPadTemplate sink_template = GST_STATIC_PAD_TEMPLATE ("sink",
    GST_PAD_SINK,
    GST_PAD_ALWAYS,
    GST_STATIC_CAPS (SDX_FILTER2D_CAPS)
    );

#define gst_sdx_filter2d_parent_class parent_class
G_DEFINE_TYPE (GstSdxFilter2d, gst_sdx_filter2d, GST_TYPE_SDX_BASE);

static const coeff_t coeffs[] = {
  [GST_SDXFILTER2D_PRESET_BLUR] = {
        {1, 1, 1},
        {1, -7, 1},
        {1, 1, 1}
      },
  [GST_SDXFILTER2D_PRESET_EDGE] = {
        {0, 1, 0},
        {1, -4, 1},
        {0, 1, 0}
      },
  [GST_SDXFILTER2D_PRESET_HEDGE] = {
        {0, -1, 0},
        {0, 2, 0},
        {0, -1, 0}
      },
  [GST_SDXFILTER2D_PRESET_VEDGE] = {
        {0, 0, 0},
        {-1, 2, -1},
        {0, 0, 0}
      },
  [GST_SDXFILTER2D_PRESET_EMBOSS] = {
        {-2, -1, 0},
        {-1, 1, 1},
        {0, 1, 2}
      },
  [GST_SDXFILTER2D_PRESET_HGRAD] = {
        {-1, -1, -1},
        {0, 0, 0},
        {1, 1, 1}
      },
  [GST_SDXFILTER2D_PRESET_VGRAD] = {
        {-1, 0, 1},
        {-1, 0, 1},
        {-1, 0, 1}
      },
  [GST_SDXFILTER2D_PRESET_IDENTITY] = {
        {0, 0, 0},
        {0, 1, 0},
        {0, 0, 0}
      },
  [GST_SDXFILTER2D_PRESET_SHARPEN] = {
        {0, -1, 0},
        {-1, 5, -1},
        {0, -1, 0}
      },
  [GST_SDXFILTER2D_PRESET_HSOBEL] = {
        {1, 2, 1},
        {0, 0, 0},
        {-1, -2, -1}
      },
  [GST_SDXFILTER2D_PRESET_VSOBEL] = {
        {1, 0, -1},
        {2, 0, -2},
        {1, 0, -1}
      }
};

static GType
gst_sdx_filter2d_filter_preset_get_type (void)
{
  static GType sdxfilter2d_filter_preset = 0;

  if (!sdxfilter2d_filter_preset) {
    static const GEnumValue filter_presets[] = {
      {GST_SDXFILTER2D_PRESET_BLUR, "GST_SDXFILTER2D_PRESET_BLUR", "blur"},
      {GST_SDXFILTER2D_PRESET_EDGE, "GST_SDXFILTER2D_PRESET_EDGE", "edge"},
      {GST_SDXFILTER2D_PRESET_HEDGE, "GST_SDXFILTER2D_PRESET_HEDGE",
          "horizontal edge"},
      {GST_SDXFILTER2D_PRESET_VEDGE, "GST_SDXFILTER2D_PRESET_VEDGE",
          "vertical edge"},
      {GST_SDXFILTER2D_PRESET_EMBOSS, "GST_SDXFILTER2D_PRESET_EMBOSS",
          "emboss"},
      {GST_SDXFILTER2D_PRESET_HGRAD, "GST_SDXFILTER2D_PRESET_HGRAD",
          "horizontal gradient"},
      {GST_SDXFILTER2D_PRESET_VGRAD, "GST_SDXFILTER2D_PRESET_VGRAD",
          "vertical gradient"},
      {GST_SDXFILTER2D_PRESET_IDENTITY, "GST_SDXFILTER2D_PRESET_IDENTITY",
          "identity"},
      {GST_SDXFILTER2D_PRESET_SHARPEN, "GST_SDXFILTER2D_PRESET_SHARPEN",
          "sharpen"},
      {GST_SDXFILTER2D_PRESET_HSOBEL, "GST_SDXFILTER2D_PRESET_HSOBEL",
          "horizontal sobel"},
      {GST_SDXFILTER2D_PRESET_VSOBEL, "GST_SDXFILTER2D_PRESET_VSOBEL",
          "vertical sobel"},
      {0, NULL, NULL}
    };
    sdxfilter2d_filter_preset =
        g_enum_register_static ("GstSdxfilter2dFilterPreset", filter_presets);
  }
  return sdxfilter2d_filter_preset;
}

static guint32
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
gst_sdx_filter2d_set_caps (GstBaseTransform * trans, GstCaps * in_caps,
    GstCaps * out_caps)
{
  GstSdxFilter2d *filter = GST_SDX_FILTER2D (trans);
  GstSdxBase *base = GST_SDX_BASE (filter);

  GstVideoInfo info;

  /* Caps is just used to initialize optical flow without modification */

  GST_DEBUG_OBJECT (filter, "in caps : %" GST_PTR_FORMAT, in_caps);

  if (!gst_video_info_from_caps (&info, in_caps)) {
    GST_WARNING_OBJECT (filter, "failed to get video info from caps");
    return FALSE;
  }

    filter2d_init_cv (GST_VIDEO_INFO_HEIGHT (&info),
        GST_VIDEO_INFO_WIDTH (&info), GST_VIDEO_INFO_HEIGHT (&info),
        GST_VIDEO_INFO_WIDTH (&info),
        video_format_to_fourcc (GST_VIDEO_INFO_FORMAT (&info)),
        video_format_to_fourcc (GST_VIDEO_INFO_FORMAT (&info)), &filter->data);

  return TRUE;
}

static GstFlowReturn
gst_sdx_filter2d_process_frames (GstSdxBase * base, GstSdxFrame ** in_frames,
    GstSdxFrame * out_frame)
{
  GstSdxFilter2d *filter = GST_SDX_FILTER2D (base);
  GstVideoFrame *in_vframe, *out_vframe;
  GstSdxFrame *in_frame;
  const coeff_t *coefficient;
  GstVideoInfo *out_info;
  gushort *in_data, *out_data;

  g_return_val_if_fail (out_frame != NULL && in_frames != NULL, GST_FLOW_ERROR);

  in_frame = in_frames[0];
  if (in_frame == NULL) {
    GST_WARNING_OBJECT (base, "input frame is invalid");
    return GST_FLOW_ERROR;
  }

  in_vframe = &in_frame->vframe;
  out_vframe = &out_frame->vframe;
  out_info = &out_frame->info;
  in_data = (gushort *) GST_VIDEO_FRAME_PLANE_DATA (in_vframe, 0);
  out_data = (gushort *) GST_VIDEO_FRAME_PLANE_DATA (out_vframe, 0);

  coefficient = filter->coefficients ?
      (const coeff_t *) filter->coefficients : &coeffs[filter->filter_preset];

    filter2d_cv (in_data, out_data, GST_VIDEO_INFO_HEIGHT (out_info),
        GST_VIDEO_INFO_WIDTH (out_info), *coefficient, filter->data);

  GST_DEBUG_OBJECT (base, "input frames processed");

  return GST_FLOW_OK;
}

static gboolean
gst_sdx_filter2d_stop (GstBaseTransform * trans)
{
  GstSdxBase *base = GST_SDX_BASE (trans);
  GstSdxFilter2d *filter = GST_SDX_FILTER2D (trans);

  if (!filter->data)
    return TRUE;

    filter2d_deinit_cv (filter->data);

  filter->data = NULL;

  if (GST_BASE_TRANSFORM_CLASS (parent_class)->stop)
    return GST_BASE_TRANSFORM_CLASS (parent_class)->stop (trans);

  return TRUE;
}

static void
gst_sdx_filter2d_set_property (GObject * object, guint prop_id,
    const GValue * value, GParamSpec * pspec)
{
  GstSdxFilter2d *filter = GST_SDX_FILTER2D (object);

  switch (prop_id) {
    case PROP_FILTER_PRESET:
      filter->filter_preset =
          (GstSdxfilter2dFilterPreset) g_value_get_enum (value);
      break;
    case PROP_FILTER_COEFFICIENTS:{
      gint16 in, out;

      if (filter->coefficients)
        g_free (filter->coefficients);
      filter->coefficients = g_new (gint16, KSIZE * KSIZE);

      g_return_if_fail (gst_value_array_get_size (value) == KSIZE);
      for (out = 0; out < KSIZE; out++) {
        const GValue *row = gst_value_array_get_value (value, out);
        g_return_if_fail (gst_value_array_get_size (row) == KSIZE);

        for (in = 0; in < KSIZE; in++) {
          const GValue *item;
          gint16 coefficient;

          item = gst_value_array_get_value (row, in);
          g_return_if_fail (G_VALUE_HOLDS_INT (item));
          coefficient = g_value_get_int (item);
          filter->coefficients[out * KSIZE + in] = coefficient;
        }
      }
      break;
    }
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
      break;
  }
}

static void
gst_sdx_filter2d_get_property (GObject * object, guint prop_id,
    GValue * value, GParamSpec * pspec)
{
  GstSdxFilter2d *filter = GST_SDX_FILTER2D (object);

  switch (prop_id) {
    case PROP_FILTER_PRESET:
      g_value_set_enum (value, filter->filter_preset);
      break;
    case PROP_FILTER_COEFFICIENTS:{
      gint16 in, out;

      if (filter->coefficients == NULL)
        break;

      for (out = 0; out < KSIZE; out++) {
        GValue row = G_VALUE_INIT;

        g_value_init (&row, GST_TYPE_ARRAY);
        for (in = 0; in < KSIZE; in++) {
          GValue item = G_VALUE_INIT;
          g_value_init (&item, G_TYPE_INT);
          g_value_set_int (&item, filter->coefficients[out * KSIZE + in]);
          gst_value_array_append_value (&row, &item);
          g_value_unset (&item);
        }

        gst_value_array_append_value (value, &row);
        g_value_unset (&row);
      }
      break;
    }
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
      break;
  }
}

static void
gst_sdx_filter2d_finalize (GObject * object)
{
  GstSdxFilter2d *filter = GST_SDX_FILTER2D (object);

  if (filter->coefficients) {
    g_free (filter->coefficients);
    filter->coefficients = NULL;
  }

  G_OBJECT_CLASS (parent_class)->finalize (object);
}

static void
gst_sdx_filter2d_init (GstSdxFilter2d * filter)
{
  GstSdxBase *base = GST_SDX_BASE (filter);
  filter->coefficients = NULL;
  gst_sdx_base_set_inputs_per_output (base, SDX_FILTER2D_INPUT_PER_OUTPUT);
}

static void
gst_sdx_filter2d_class_init (GstSdxFilter2dClass * klass)
{
  GObjectClass *gobject_class;
  GstElementClass *element_class;
  GstSdxBaseClass *sdxbase_class;
  GstBaseTransformClass *transform_class;

  gobject_class = G_OBJECT_CLASS (klass);
  element_class = GST_ELEMENT_CLASS (klass);
  transform_class = GST_BASE_TRANSFORM_CLASS (klass);
  sdxbase_class = GST_SDX_BASE_CLASS (klass);

  gst_element_class_add_static_pad_template (element_class, &sink_template);
  gst_element_class_add_static_pad_template (element_class, &src_template);

  gobject_class->set_property = gst_sdx_filter2d_set_property;
  gobject_class->get_property = gst_sdx_filter2d_get_property;
  gobject_class->finalize = gst_sdx_filter2d_finalize;

  transform_class->stop = GST_DEBUG_FUNCPTR (gst_sdx_filter2d_stop);
  transform_class->set_caps = GST_DEBUG_FUNCPTR (gst_sdx_filter2d_set_caps);
  sdxbase_class->process_frames =
      GST_DEBUG_FUNCPTR (gst_sdx_filter2d_process_frames);

  g_object_class_install_property (gobject_class, PROP_FILTER_PRESET,
      g_param_spec_enum ("filter-preset", "filter preset", "filter preset",
          gst_sdx_filter2d_filter_preset_get_type (),
          GST_SDXFILTER2D_PRESET_EDGE,
          (GParamFlags) (G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS)));

  g_object_class_install_property (gobject_class, PROP_FILTER_COEFFICIENTS,
      gst_param_spec_array ("coefficients",
          "3x3 2d array with 9 coefficients matrix",
          "3x3 2d array with 9 coefficients matrix",
          gst_param_spec_array ("matrix-in1", "rows", "rows",
              g_param_spec_int ("matrix-in2", "cols", "cols",
                  G_MINSHORT, G_MAXSHORT, 0,
                  (GParamFlags) (G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS)),
              (GParamFlags) (G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS)),
          (GParamFlags) (G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS)));

  gst_element_class_set_static_metadata (element_class,
      "SDx Filter 2D",
      "Filter/Effect/Video",
      "SDx Filter 2D", "Sören Brinkmann <soren.brinkmann@xilinx.com>");
}

static gboolean
plugin_init (GstPlugin * plugin)
{
  GST_DEBUG_CATEGORY_INIT (gst_sdx_filter2d_debug, "sdxfilter2d", 0,
      "SDx Filter 2D");

  return gst_element_register (plugin, "sdxfilter2d", GST_RANK_NONE,
      GST_TYPE_SDX_FILTER2D);
}

/* PACKAGE: this is usually set by autotools depending on some _INIT macro
 * in configure.ac and then written into and defined in config.h, but we can
 * just set it ourselves here in case someone doesn't use autotools to
 * compile this code. GST_PLUGIN_DEFINE needs PACKAGE to be defined.
 */
#ifndef PACKAGE
#define PACKAGE "sdxfilter2d"
#endif

GST_PLUGIN_DEFINE (GST_VERSION_MAJOR,
    GST_VERSION_MINOR,
    sdxfilter2d,
    "SDx Filter 2D plugin",
    plugin_init,
    "0.1",
    "LGPL",
    "GStreamer SDX",
    "http://xilinx.com/")
