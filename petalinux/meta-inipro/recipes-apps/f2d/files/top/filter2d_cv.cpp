/* Temporay fix for SDx Clang issue */
#ifdef __SDSOC__
#undef __ARM_NEON__
#undef __ARM_NEON
#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>
#define __ARM_NEON__
#define __ARM_NEON
#else
#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>
#endif
#include "filter2d_sds.h"

using namespace cv;

#ifdef __cplusplus
extern "C" {
#endif

void filter2d_cv(unsigned short *frm_data_in, unsigned short *frm_data_out,
				int height, int width, const coeff_t coeff)
{
	Mat src(height, width, CV_8UC2, frm_data_in);
	Mat dst(height, width, CV_8UC2, frm_data_out);

	// planes
	std::vector<Mat> planes;

	// convert kernel from short to int
	int coeff_i[KSIZE][KSIZE];
	for (int i=0; i<KSIZE; i++)
		for (int j=0; j<KSIZE; j++)
			coeff_i[i][j] = coeff[i][j];

	Mat kernel = Mat(KSIZE, KSIZE, CV_32SC1, (int *)coeff_i);

	//anchor
	Point anchor = Point(-1, -1);

	//filter
	split(src, planes);
	filter2D(planes[0], planes[0], -1, kernel, anchor, 0, BORDER_DEFAULT);
	/*
	planes.erase(planes.begin()+1);
	planes.push_back(Mat(height, width, CV_8UC1, cvScalar(128)));
	*/
	merge(planes, dst);
}

#ifdef __cplusplus
}
#endif
