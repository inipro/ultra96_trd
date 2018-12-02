#ifndef _FILTER2D_SDS_H_
#define _FILTER2D_SDS_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stddef.h>
#include <stdint.h>

/* Kernel size */
#define KSIZE 3

/* 2D array of coefficients */
typedef short int coeff_t[KSIZE][KSIZE];

#ifdef WITH_SDSOC
int filter2d_init_sds(size_t in_height, size_t in_width, size_t out_height,
				size_t out_width, uint32_t in_fourcc,
				uint32_t out_fourcc, void **priv);

void filter2d_sds(unsigned short *frm_data_in, unsigned short *frm_data_out,
		int height, int width, const coeff_t coeff, void *priv);
#endif

#ifdef __cplusplus
}
#endif

#endif /* _FILTER2D_SDS_H_ */
