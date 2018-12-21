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
 *******************************************************************************/

#include <common/xf_common.h>
#include <imgproc/xf_custom_convolution.hpp>
#include <linux/videodev2.h>
#include <stdlib.h>

#include "filter2d_sds.h"

#define F2D_HEIGHT	1080
#define F2D_WIDTH	1920

#define F2D_DM_COPY		0
#define F2D_DM_ZERO_COPY	1
#ifndef F2D_DM
#define F2D_DM			F2D_DM_COPY
#endif

using namespace xf;

struct filter2d_data {
	xf::Mat<XF_8UC1, F2D_HEIGHT, F2D_WIDTH, XF_NPPC1> *inLuma;
	xf::Mat<XF_8UC1, F2D_HEIGHT, F2D_WIDTH, XF_NPPC1> *inoutUV;
	xf::Mat<XF_8UC1, F2D_HEIGHT, F2D_WIDTH, XF_NPPC1> *outLuma;
	uint32_t in_fourcc;
	uint32_t out_fourcc;
};

#pragma SDS data copy("inoutUV.data"[0:"inoutUV.size"])
#pragma SDS data access_pattern("inoutUV.data":SEQUENTIAL)
#pragma SDS data mem_attribute("inoutUV.data":NON_CACHEABLE|PHYSICAL_CONTIGUOUS)
#pragma SDS data mem_attribute(frm_data_in:NON_CACHEABLE|PHYSICAL_CONTIGUOUS)
#if (F2D_DM==F2D_DM_COPY)
  #pragma SDS data copy(frm_data_in[0:pcnt])
#elif (F2D_DM==F2D_DM_ZERO_COPY)
  #pragma SDS data zero_copy(frm_data_in[0:pcnt])
#endif
#pragma SDS data access_pattern(frm_data_in:SEQUENTIAL)
#pragma SDS data mem_attribute("inLuma.data":NON_CACHEABLE|PHYSICAL_CONTIGUOUS)
#pragma SDS data copy("inLuma.data"[0:"inLuma.size"])
#pragma SDS data access_pattern("inLuma.data":SEQUENTIAL)
void read_f2d_input(unsigned short *frm_data_in,
		    xf::Mat<XF_8UC1, F2D_HEIGHT, F2D_WIDTH, XF_NPPC1> &inLuma,
		    xf::Mat<XF_8UC1, F2D_HEIGHT, F2D_WIDTH, XF_NPPC1> &inoutUV,
		    uint32_t in_fourcc, int pcnt)
{
	unsigned short lumamask    = (V4L2_PIX_FMT_YUYV==in_fourcc)? 0x00FF : 0xFF00;
	unsigned short lumashift   = (V4L2_PIX_FMT_YUYV==in_fourcc)? 0      : 8;
	unsigned short chromamask  = (V4L2_PIX_FMT_YUYV==in_fourcc)? 0xFF00 : 0x00FF;
	unsigned short chromashift = (V4L2_PIX_FMT_YUYV==in_fourcc)? 8      : 0;

	for(int i=0; i<pcnt; i++){
#pragma HLS pipeline II=1
		unsigned short yuvpix = frm_data_in[i];
		ap_uint<8> ypix =  (ap_uint<8>)((yuvpix & lumamask)>>lumashift);
		ap_uint<8> uvpix = (ap_uint<8>)((yuvpix & chromamask)>>chromashift);
		inLuma.data[i] = ypix;
		inoutUV.data[i] = uvpix;
	}
}

#pragma SDS data buffer_depth("inoutUV.data":32768)
#pragma SDS data mem_attribute("inoutUV.data":NON_CACHEABLE|PHYSICAL_CONTIGUOUS)
#pragma SDS data copy("inoutUV.data"[0:"inoutUV.size"])
#pragma SDS data access_pattern("inoutUV.data":SEQUENTIAL)
#pragma SDS data mem_attribute("outLuma.data":NON_CACHEABLE|PHYSICAL_CONTIGUOUS)
#pragma SDS data copy("outLuma.data"[0:"outLuma.size"])
#pragma SDS data access_pattern("outLuma.data":SEQUENTIAL)
#pragma SDS data mem_attribute(frm_data_out:NON_CACHEABLE|PHYSICAL_CONTIGUOUS)
#if (F2D_DM==F2D_DM_COPY)
  #pragma SDS data copy(frm_data_out[0:pcnt])
#elif (F2D_DM==F2D_DM_ZERO_COPY)
  #pragma SDS data zero_copy(frm_data_out[0:pcnt])
#endif
#pragma SDS data access_pattern(frm_data_out:SEQUENTIAL)
void write_f2d_output(xf::Mat<XF_8UC1, F2D_HEIGHT, F2D_WIDTH, XF_NPPC1> &outLuma,
		      xf::Mat<XF_8UC1, F2D_HEIGHT, F2D_WIDTH, XF_NPPC1> &inoutUV,
		      unsigned short *frm_data_out, uint32_t out_fourcc, int pcnt)
{
	unsigned short lumashift = (V4L2_PIX_FMT_YUYV==out_fourcc)? 0 : 8;
	unsigned short chromashift = (V4L2_PIX_FMT_YUYV==out_fourcc)? 8 : 0;

	for(int i=0; i<pcnt; i++){
#pragma HLS pipeline II=1
		ap_uint<8> ypix = outLuma.data[i];
		ap_uint<8> uvpix = inoutUV.data[i];
		unsigned short yuvpix = ((unsigned short) uvpix << chromashift) | ((unsigned short) ypix << lumashift);
		frm_data_out[i] = yuvpix;
	}
}

int filter2d_init_sds(size_t in_height, size_t in_width, size_t out_height,
		      size_t out_width, uint32_t in_fourcc,
		      uint32_t out_fourcc, void **priv)
{
	struct filter2d_data *f2d = (struct filter2d_data *) malloc(sizeof(struct filter2d_data));
	if (f2d == NULL) {
		return -1;
	}

	f2d->inLuma = new xf::Mat<XF_8UC1, F2D_HEIGHT, F2D_WIDTH, XF_NPPC1>(in_height, in_width);
	f2d->inoutUV = new xf::Mat<XF_8UC1, F2D_HEIGHT, F2D_WIDTH, XF_NPPC1>(in_height, in_width);
	f2d->outLuma = new xf::Mat<XF_8UC1, F2D_HEIGHT, F2D_WIDTH, XF_NPPC1>(in_height, in_width);
	f2d->in_fourcc = in_fourcc;
	f2d->out_fourcc = out_fourcc;

	*priv = f2d;

	return 0;
}

void filter2d_deinit_sds(void *priv)
{
	struct filter2d_data *f2d = (struct filter2d_data *) priv;

	delete f2d->inLuma;
	delete f2d->inoutUV;
	delete f2d->outLuma;

	free(f2d);
}

void filter2d_sds(unsigned short *frm_data_in, unsigned short *frm_data_out,
		  int height, int width, const coeff_t coeff, void *priv)
{
	struct filter2d_data *f2d = (struct filter2d_data *) priv;
	int pcnt = height*width;


	// split the 16b YUYV... input data into separate 8b YYYY... and 8b UVUV...
	read_f2d_input(frm_data_in, *f2d->inLuma, *f2d->inoutUV, f2d->in_fourcc, pcnt);

	// this is the xfopencv version of filter2D, found in imgproc/xf_custom_convolution.hpp
	xf::filter2D<XF_BORDER_CONSTANT, KSIZE, KSIZE, XF_8UC1, XF_8UC1, F2D_HEIGHT, F2D_WIDTH, XF_NPPC1>
		(*f2d->inLuma, *f2d->outLuma, (short int *) coeff, 0);

	// combine separate 8b YYYY... and 8b UVUV... data into 16b YUYV... output data
	write_f2d_output(*f2d->outLuma, *f2d->inoutUV, frm_data_out, f2d->out_fourcc, pcnt);
}
