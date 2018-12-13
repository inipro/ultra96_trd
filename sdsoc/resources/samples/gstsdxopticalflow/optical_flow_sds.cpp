/***************************************************************************
 Copyright (c) 2016, Xilinx, Inc.
 All rights reserved.

 Redistribution and use in source and binary forms, with or without modification,
 are permitted provided that the following conditions are met:

 1. Redistributions of source code must retain the above copyright notice,
 this list of conditions and the following disclaimer.

 2. Redistributions in binary form must reproduce the above copyright notice,
 this list of conditions and the following disclaimer in the documentation
 and/or other materials provided with the distribution.

 3. Neither the name of the copyright holder nor the names of its contributors
 may be used to endorse or promote products derived from this software
 without specific prior written permission.

 THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

 ***************************************************************************/
#include <cmath>
#include <common/xf_common.h>
#include <imgproc/xf_dense_npyr_optical_flow.hpp>
#include <linux/videodev2.h>
#include <stdlib.h>

#include "optical_flow_sds.h"

#define OF_WINDOW_SIZE		25
#ifndef OF_PIX_PER_CLOCK
#define OF_PIX_PER_CLOCK	1
#endif

#define OF_DM_COPY		0
#define OF_DM_ZERO_COPY		1
#ifndef OF_DM
#define OF_DM 			OF_DM_COPY
#endif

#ifdef REPORT_ELAPSED_TIME
#include "sds_lib.h"
static unsigned long long clock_start, clock_end, tks;
static unsigned long long tps;
#define TIME_STAMP_INIT  { clock_start = sds_clock_counter(); }
#define TIME_STAMP  { clock_end = sds_clock_counter(); tks=clock_end-clock_start; printf("%dth frame: %5.1f ms %5.1f fps\n", fct, 1000.0f*(float)tks/(float)tps, (float)tps/(float)tks ); }
#endif

struct optical_flow_data {
	xf::Mat<XF_8UC1,  OF_HEIGHT, OF_WIDTH, OF_PIX_PER_CLOCK> *luma_prev;
	xf::Mat<XF_8UC1,  OF_HEIGHT, OF_WIDTH, OF_PIX_PER_CLOCK> *luma_curr;
	xf::Mat<XF_32FC1, OF_HEIGHT, OF_WIDTH, OF_PIX_PER_CLOCK> *flow_x;
	xf::Mat<XF_32FC1, OF_HEIGHT, OF_WIDTH, OF_PIX_PER_CLOCK> *flow_y;
	uint32_t in_fourcc;
	uint32_t out_fourcc;
};

#pragma SDS data mem_attribute(frame_prev:NON_CACHEABLE|PHYSICAL_CONTIGUOUS)
#pragma SDS data mem_attribute(frame_curr:NON_CACHEABLE|PHYSICAL_CONTIGUOUS)
#if (OF_PIX_PER_CLOCK==1 && OF_DM==OF_DM_COPY)
  #pragma SDS data copy(frame_prev[0:pcnt])
  #pragma SDS data copy(frame_curr[0:pcnt])
#elif (OF_PIX_PER_CLOCK==1 && OF_DM==OF_DM_ZERO_COPY)
  #pragma SDS data zero_copy(frame_prev[0:pcnt])
  #pragma SDS data zero_copy(frame_curr[0:pcnt])
#elif (OF_PIX_PER_CLOCK==2 && OF_DM==OF_DM_COPY)
  #pragma SDS data copy(frame_prev[0:pcnt>>1])
  #pragma SDS data copy(frame_curr[0:pcnt>>1])
#elif (OF_PIX_PER_CLOCK==2 && OF_DM==OF_DM_ZERO_COPY)
  #pragma SDS data zero_copy(frame_prev[0:pcnt>>1])
  #pragma SDS data zero_copy(frame_curr[0:pcnt>>1])
#endif
#pragma SDS data access_pattern(frame_prev:SEQUENTIAL)
#pragma SDS data access_pattern(frame_curr:SEQUENTIAL)
#pragma SDS data mem_attribute("luma_prev.data":NON_CACHEABLE|PHYSICAL_CONTIGUOUS)
#pragma SDS data mem_attribute("luma_curr.data":NON_CACHEABLE|PHYSICAL_CONTIGUOUS)
#pragma SDS data copy("luma_prev.data"[0:"luma_prev.size"])
#pragma SDS data copy("luma_curr.data"[0:"luma_curr.size"])
#pragma SDS data access_pattern("luma_prev.data":SEQUENTIAL)
#pragma SDS data access_pattern("luma_curr.data":SEQUENTIAL)
void read_optflow_input(
			XF_TNAME(XF_16UC1, OF_PIX_PER_CLOCK) *frame_prev, 
			XF_TNAME(XF_16UC1, OF_PIX_PER_CLOCK) *frame_curr,
			xf::Mat<XF_8UC1, OF_HEIGHT, OF_WIDTH, OF_PIX_PER_CLOCK>& luma_prev,
			xf::Mat<XF_8UC1, OF_HEIGHT, OF_WIDTH, OF_PIX_PER_CLOCK>& luma_curr,
			uint32_t in_fourcc,
			int pcnt)
{
#if (OF_PIX_PER_CLOCK==1)
	ap_uint<16> lumamask = (V4L2_PIX_FMT_UYVY==in_fourcc)? 0xFF00 : 0x00FF;
	int lumashift        = (V4L2_PIX_FMT_UYVY==in_fourcc)? 8 : 0;

	for(int i=0; i<pcnt; i++){
#pragma HLS loop_tripcount min=921600 max=8294400 avg=2073600
#pragma HLS pipeline II=1
		luma_prev.data[i] = (ap_uint<8>)((frame_prev[i] & lumamask)>>lumashift);
		luma_curr.data[i] = (ap_uint<8>)((frame_curr[i] & lumamask)>>lumashift);
	}
#else
	ap_uint<32> lumamask_hi = (V4L2_PIX_FMT_UYVY==in_fourcc)? 0xFF000000 : 0x00FF0000;
	int lumashift_hi        = (V4L2_PIX_FMT_UYVY==in_fourcc)? 16 : 8;
	ap_uint<32> lumamask_lo = (V4L2_PIX_FMT_UYVY==in_fourcc)? 0x0000FF00 : 0x000000FF;
	int lumashift_lo        = (V4L2_PIX_FMT_UYVY==in_fourcc)? 8 : 0;

	for(int i=0; i<(pcnt>>1); i++){
#pragma HLS loop_tripcount min=460800 max=4147200 avg=1036800
#pragma HLS pipeline II=1
		luma_prev.data[i] = (ap_uint<16>)((ap_uint<16>)((frame_prev[i] & lumamask_hi)>>lumashift_hi)) | ((ap_uint<16>)((frame_prev[i] & lumamask_lo)>>lumashift_lo));
		luma_curr.data[i] = (ap_uint<16>)((ap_uint<16>)((frame_curr[i] & lumamask_hi)>>lumashift_hi)) | ((ap_uint<16>)((frame_curr[i] & lumamask_lo)>>lumashift_lo));
	}
#endif
}


unsigned char getLuma (float fx, float fy, float clip_flowmag)
{
#pragma HLS inline
	float rad = sqrtf(fx*fx + fy*fy);

	/* clamp to MAX */
	if (rad > clip_flowmag)
		rad = clip_flowmag;

	/* convert 0..MAX to 0.0..1.0 */
	rad /= clip_flowmag;

	unsigned char pix = (unsigned char) (255.0f * rad);

	return pix;
}

unsigned char getChroma (float f, float clip_flowmag)
{
#pragma HLS inline
	/* clamp big positive f to  MAX */
	if (f > clip_flowmag)
		f =  clip_flowmag;

	/* clamp big negative f to -MAX */
	if (f < (-clip_flowmag))
		f = -clip_flowmag;

	/* convert -MAX..MAX to -1.0..1.0 */
	f /= clip_flowmag;

	/* convert -1.0..1.0 to -127..127 to 1..255 */
	unsigned char pix = (unsigned char) (127.0f * f + 128.0f);

	return pix;
}

#pragma SDS data mem_attribute("flowx.data":NON_CACHEABLE|PHYSICAL_CONTIGUOUS)
#pragma SDS data mem_attribute("flowy.data":NON_CACHEABLE|PHYSICAL_CONTIGUOUS)
#pragma SDS data copy("flowx.data"[0:"flowx.size"])
#pragma SDS data copy("flowy.data"[0:"flowy.size"])
#pragma SDS data access_pattern("flowx.data":SEQUENTIAL)
#pragma SDS data access_pattern("flowy.data":SEQUENTIAL)
#pragma SDS data mem_attribute(frame_out:NON_CACHEABLE|PHYSICAL_CONTIGUOUS)
#if (OF_PIX_PER_CLOCK==1 && OF_DM==OF_DM_COPY)
  #pragma SDS data copy(frame_out[0:pcnt])
#elif (OF_PIX_PER_CLOCK==1 && OF_DM==OF_DM_ZERO_COPY)
  #pragma SDS data zero_copy(frame_out[0:pcnt])
#elif (OF_PIX_PER_CLOCK==2 && OF_DM==OF_DM_COPY)
  #pragma SDS data copy(frame_out[0:pcnt>>1])
#elif (OF_PIX_PER_CLOCK==2 && OF_DM==OF_DM_ZERO_COPY)
  #pragma SDS data zero_copy(frame_out[0:pcnt>>1])
#endif
#pragma SDS data access_pattern(frame_out:SEQUENTIAL)
void write_optflow_output(
			xf::Mat<XF_32FC1, OF_HEIGHT, OF_WIDTH, OF_PIX_PER_CLOCK> &flowx,
			xf::Mat<XF_32FC1, OF_HEIGHT, OF_WIDTH, OF_PIX_PER_CLOCK> &flowy,
			XF_TNAME(XF_16UC1, OF_PIX_PER_CLOCK) *frame_out,
			uint32_t out_fourcc,
			int pcnt)
{
	float clip_flowmag = 10.0f;
	int lumashift = (V4L2_PIX_FMT_UYVY==out_fourcc)? 8 : 0;
	int chromashift = (V4L2_PIX_FMT_UYVY==out_fourcc)? 0 : 8;

#if (OF_PIX_PER_CLOCK==1)
	for(int i=0; i<pcnt; i++){
#pragma HLS loop_tripcount min=921600 max=8294400 avg=2073600
#pragma HLS pipeline II=1
		float fx = flowx.data[i];
		float fy = flowy.data[i];

		unsigned char outLuma = getLuma (fx, fy, clip_flowmag);
		unsigned char outChroma = (i&1)? getChroma (fy, clip_flowmag) : getChroma (fx, clip_flowmag);
		unsigned short yuvpix = ((unsigned short)outLuma << lumashift) | ((unsigned short)outChroma << chromashift);
		frame_out[i] = yuvpix;
	}
#else
	for(int i=0; i<(pcnt>>1); i++){
#pragma HLS loop_tripcount min=460800 max=4147200 avg=1036800
#pragma HLS pipeline II=1
		float *fx_lo, *fx_hi;
		float *fy_lo, *fy_hi;

		ap_uint<64> dfx = flowx.data[i];
		ap_uint<64> dfy = flowy.data[i];

		ap_uint<32> ifx_lo = (ap_uint<32>)(dfx & 0x00000000FFFFFFFFULL); 
		ap_uint<32> ify_lo = (ap_uint<32>)(dfy & 0x00000000FFFFFFFFULL);
		fx_lo = (float *)(&ifx_lo);
		fy_lo = (float *)(&ify_lo);
		unsigned char outLuma_lo = getLuma (*fx_lo, *fy_lo, clip_flowmag);
		unsigned char outChroma_lo = getChroma (*fx_lo, clip_flowmag);
		unsigned short yuvpix_lo = ((unsigned short)outLuma_lo << lumashift) | ((unsigned short)outChroma_lo << chromashift);

		ap_uint<32> ifx_hi = (ap_uint<32>)((dfx & 0xFFFFFFFF00000000ULL)>>32); 
		ap_uint<32> ify_hi = (ap_uint<32>)((dfy & 0xFFFFFFFF00000000ULL)>>32);
		fx_hi = (float *)(&ifx_hi);
		fy_hi = (float *)(&ify_hi);
		unsigned char outLuma_hi = getLuma (*fx_hi, *fy_hi, clip_flowmag);
		unsigned char outChroma_hi = getChroma (*fy_hi, clip_flowmag);
		unsigned short yuvpix_hi = ((unsigned short)outLuma_hi << lumashift) | ((unsigned short)outChroma_hi << chromashift);

		frame_out[i] = (ap_uint<32>)(((ap_uint<32>)yuvpix_hi<<16) | yuvpix_lo);
	}
#endif
}

int optical_flow_init_sds(size_t in_height, size_t in_width, size_t out_height,
			  size_t out_width, uint32_t in_fourcc,
			  uint32_t out_fourcc, void **priv)
{
	struct optical_flow_data *ofd = (struct optical_flow_data *) malloc(sizeof(struct optical_flow_data));
	if (ofd == NULL) {
		return -1;
	}

	ofd->luma_prev = new xf::Mat<XF_8UC1,  OF_HEIGHT, OF_WIDTH, OF_PIX_PER_CLOCK> (in_height, in_width);
	ofd->luma_curr = new xf::Mat<XF_8UC1,  OF_HEIGHT, OF_WIDTH, OF_PIX_PER_CLOCK> (in_height, in_width);
	ofd->flow_x = new xf::Mat<XF_32FC1, OF_HEIGHT, OF_WIDTH, OF_PIX_PER_CLOCK> (in_height, in_width);
	ofd->flow_y = new xf::Mat<XF_32FC1, OF_HEIGHT, OF_WIDTH, OF_PIX_PER_CLOCK> (in_height, in_width);
	ofd->in_fourcc = in_fourcc;
	ofd->out_fourcc = out_fourcc;

	*priv = ofd;

	return 0;
}

void optical_flow_deinit_sds(void *priv)
{
	struct optical_flow_data *ofd = (struct optical_flow_data *) priv;

	delete ofd->luma_prev;
	delete ofd->luma_curr;
	delete ofd->flow_x;
	delete ofd->flow_y;

	free(ofd);
}

void optical_flow_sds(unsigned short *frame_prev, unsigned short *frame_curr,
		      unsigned short *frame_out, int height, int width,
		      void *priv)
{
	struct optical_flow_data *ofd = (struct optical_flow_data *) priv;
	int pcnt = height*width;

	XF_TNAME(XF_16UC1, OF_PIX_PER_CLOCK) *f_prev = (XF_TNAME(XF_16UC1, OF_PIX_PER_CLOCK) *)frame_prev;
	XF_TNAME(XF_16UC1, OF_PIX_PER_CLOCK) *f_curr = (XF_TNAME(XF_16UC1, OF_PIX_PER_CLOCK) *)frame_curr;
	XF_TNAME(XF_16UC1, OF_PIX_PER_CLOCK) *f_out = (XF_TNAME(XF_16UC1, OF_PIX_PER_CLOCK) *)frame_out;

#ifdef REPORT_ELAPSED_TIME
	static int fct = 0;
	tps = sds_clock_frequency();

	/* calculate time every 60 frames (1 sec @ 60 hz) */
	if ((++fct % 60) == 0) {
		TIME_STAMP_INIT
	}

#endif

	read_optflow_input(f_prev, f_curr, *ofd->luma_prev, *ofd->luma_curr, ofd->in_fourcc, pcnt);
	xf::DenseNonPyrLKOpticalFlow<OF_WINDOW_SIZE, XF_8UC1, OF_HEIGHT, OF_WIDTH, OF_PIX_PER_CLOCK>(*ofd->luma_prev, *ofd->luma_curr, *ofd->flow_x, *ofd->flow_y);
	write_optflow_output(*ofd->flow_x, *ofd->flow_y, f_out, ofd->out_fourcc, pcnt);

#ifdef REPORT_ELAPSED_TIME
	/* print accelerator elapsed time */
	if ((fct % 60) == 0) {
		TIME_STAMP
	}
#endif

	return;
}
