#include <common/xf_common.h>
#include <imgproc/xf_custom_convolution.hpp>
#include <linux/videodev2.h>
#include <stdlib.h>

#include "filter2d_sds.h"

//#define F2D_HEIGHT	2160
//#define F2D_WIDTH	3840
#define F2D_HEIGHT	1080
#define F2D_WIDTH	1920

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
#pragma SDS data copy(frm_data_in[0:pcnt])
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

	for (int i=0; i<pcnt; i++) {
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
#pragma SDS data copy(frm_data_out[0:pcnt])
#pragma SDS data access_pattern(frm_data_out:SEQUENTIAL)
void write_f2d_output(xf::Mat<XF_8UC1, F2D_HEIGHT, F2D_WIDTH, XF_NPPC1> &outLuma,
					xf::Mat<XF_8UC1, F2D_HEIGHT, F2D_WIDTH, XF_NPPC1> &inoutUV,
					unsigned short *frm_data_out, uint32_t out_fourcc, int pcnt)
{
	unsigned short lumashift = (V4L2_PIX_FMT_YUYV==out_fourcc)? 0 : 8;
	unsigned short chromashift = (V4L2_PIX_FMT_YUYV==out_fourcc)? 8 : 0;

	for (int i=0; i<pcnt; i++) {
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
	struct filter2d_data *f2d = (struct filter2d_data *)malloc(sizeof(struct filter2d_data));
	if (f2d == NULL) {
		return -1;
	}

	f2d->inLuma = new xf::Mat<XF_8UC1, F2D_HEIGHT, F2D_WIDTH, XF_NPPC1>(in_height, in_width);
	f2d->inoutUV = new xf::Mat<XF_8UC1, F2D_HEIGHT, F2D_WIDTH, XF_NPPC1>(in_height, in_width);
	f2d->outLuma = new xf::Mat<XF_8UC1, F2D_HEIGHT, F2D_WIDTH, XF_NPPC1>(in_height, in_width);
	f2d->in_fourcc = in_fourcc;
	//f2d->out_fourcc = out_fourcc;
	f2d->out_fourcc = in_fourcc;

	*priv = f2d;

	return 0;
}
/*
#pragma SDS data copy("_src.data"[0:"_src.size"])
#pragma SDS data access_pattern("_src.data":SEQUENTIAL)
#pragma SDS data mem_attribute("_src.data":NON_CACHEABLE|PHYSICAL_CONTIGUOUS)
#pragma SDS data copy("_dst.data"[0:"_dst.size"])
#pragma SDS data access_pattern("_dst.data":SEQUENTIAL)
#pragma SDS data mem_attribute("_dst.data":NON_CACHEABLE|PHYSICAL_CONTIGUOUS)
void dummyFilter(xf::Mat<XF_8UC1, F2D_HEIGHT, F2D_WIDTH, XF_NPPC1> &_src, xf::Mat<XF_8UC1, F2D_HEIGHT, F2D_WIDTH, XF_NPPC1> &_dst)
{

	for (int i=0; i<_src.rows; i++) {
		for (int j=0; j<(_dst.cols)>>(XF_BITSHIFT(XF_NPPC1)); j++) {
#pragma HLS PIPELINE
			*(_dst.data+i*(_dst.cols>>(XF_BITSHIFT(XF_NPPC1)))+j) = *(_src.data+i*(_src.cols>>(XF_BITSHIFT(XF_NPPC1)))+j);
		}
	}
}
*/
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
	//dummyFilter(*f2d->inLuma, *f2d->outLuma);

	// combine separate 8b YYYY... and 8b UVUV... data into 16b YUYV... output data
	write_f2d_output(*f2d->outLuma, *f2d->inoutUV, frm_data_out, f2d->out_fourcc, pcnt);
}
