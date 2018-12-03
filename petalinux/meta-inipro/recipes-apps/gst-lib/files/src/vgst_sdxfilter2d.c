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
#include "vgst_sdxfilter2d.h"

extern vgst_application app;

const coeff_t coeff_blur = {
  {1, 1, 1},
  {1, -7, 1},
  {1, 1, 1}
};

const coeff_t coeff_edge = {
  {0, 1, 0},
  {1, -4, 1},
  {0, 1, 0}
};

const coeff_t coeff_edge_h = {
  {0, -1, 0},
  {0, 2, 0},
  {0, -1, 0}
};

const coeff_t coeff_edge_v = {
  {0, 0, 0},
  {-1, 2, -1},
  {0, 0, 0}
};

const coeff_t coeff_emboss = {
  {-2, -1, 0},
  {-1, 1, 1},
  {0, 1, 2}
};

const coeff_t coeff_gradient_h = {
  {-1, -1, -1},
  {0, 0, 0},
  {1, 1, 1}
};

const coeff_t coeff_gradient_v = {
  {-1, 0, 1},
  {-1, 0, 1},
  {-1, 0, 1}
};

const coeff_t coeff_identity = {
  {0, 0, 0},
  {0, 1, 0},
  {0, 0, 0}
};

const coeff_t coeff_sharpen = {
  {0, -1, 0},
  {-1, 5, -1},
  {0, -1, 0}
};

const coeff_t coeff_sobel_h = {
  {1, 2, 1},
  {0, 0, 0},
  {-1, -2, -1}
};

const coeff_t coeff_sobel_v = {
  {1, 0, -1},
  {2, 0, -2},
  {1, 0, -1}
};

/* store current coefficients */
static coeff_t coeff_cur;

static struct
{
  filter2d_preset preset;
  const char *name;
  const coeff_t *coeff;
} filter2d_presets[] = {
  {
  FILTER2D_PRESET_BLUR, "Blur", &coeff_blur}, {
  FILTER2D_PRESET_EDGE, "Edge", &coeff_edge}, {
  FILTER2D_PRESET_EDGE_H, "Edge Horizontal", &coeff_edge_h}, {
  FILTER2D_PRESET_EDGE_V, "Edge Vertical", &coeff_edge_v}, {
  FILTER2D_PRESET_EMBOSS, "Emboss", &coeff_emboss}, {
  FILTER2D_PRESET_GRADIENT_H, "Gradient Horizontal", &coeff_gradient_h}, {
  FILTER2D_PRESET_GRADIENT_V, "Gradient Vertical", &coeff_gradient_v}, {
  FILTER2D_PRESET_IDENTITY, "Identity", &coeff_identity}, {
  FILTER2D_PRESET_SHARPEN, "Sharpen", &coeff_sharpen}, {
  FILTER2D_PRESET_SOBEL_H, "Sobel Horizontal", &coeff_sobel_h}, {
  FILTER2D_PRESET_SOBEL_V, "Sobel Vertical", &coeff_sobel_v}
};

void
filter2d_set_preset_coeff (struct filter_s *fs, filter2d_preset preset)
{
  unsigned int i;

  for (i = 0; i < ARRAY_SIZE (filter2d_presets); ++i) {
    if (filter2d_presets[i].preset == preset) {
      filter2d_set_coeff (fs, *filter2d_presets[i].coeff);
    }
  }
}

const char *
filter2d_get_preset_name (filter2d_preset preset)
{
  unsigned int i;

  for (i = 0; i < ARRAY_SIZE (filter2d_presets); ++i) {
    if (filter2d_presets[i].preset == preset)
      return filter2d_presets[i].name;
  }

  return NULL;
}

void
filter2d_set_coeff (struct filter_s *fs, const coeff_t coeff)
{
  vgst_ip_params *ip_param = app.ip_params;
  unsigned int row;
  unsigned int col;
  char tmp_str[256];
  gchar *matrix;

  /* set the coefficients */
  sprintf (tmp_str, "< < %d, %d, %d >, < %d, %d, %d>, < %d, %d, %d> >",
      coeff[0][0], coeff[0][1], coeff[0][2],
      coeff[1][0], coeff[1][1], coeff[1][2],
      coeff[2][0], coeff[2][1], coeff[2][2]);
  matrix = g_strdup (tmp_str);
  if (ip_param && (!ip_param->raw) && (ip_param->filter_type == SDX_FILTER)) {
    gst_util_set_object_arg (G_OBJECT (app.playback->videofilter),
        "coefficients", matrix);
  }

  /* store new values */
  for (row = 0; row < KSIZE; row++)
    for (col = 0; col < KSIZE; col++)
      coeff_cur[row][col] = coeff[row][col];

  g_free (matrix);
}

coeff_t *
filter2d_get_coeff (struct filter_s *fs)
{
  return &coeff_cur;
}

const coeff_t *
filter2d_get_preset_coeff (filter2d_preset preset)
{
  unsigned int i;

  for (i = 0; i < ARRAY_SIZE (filter2d_presets); ++i) {
    if (filter2d_presets[i].preset == preset)
      return filter2d_presets[i].coeff;
  }

  return NULL;
}
