#ifndef _FILTER2D_H_
#define _FILTER2D_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "filter2d_sds.h"

/* Filter presets */
#define FILTER2D_PRESET_CNT 11

/* Filter presets */
typedef enum
{
	FILTER2D_PRESET_BLUR,
	FILTER2D_PRESET_EDGE,
	FILTER2D_PRESET_EDGE_H,
	FILTER2D_PRESET_EDGE_V,
	FILTER2D_PRESET_EMBOSS,
	FILTER2D_PRESET_GRADIENT_H,
	FILTER2D_PRESET_GRADIENT_V,
	FILTER2D_PRESET_IDENTITY,
	FILTER2D_PRESET_SHARPEN,
	FILTER2D_PRESET_SOBEL_H,
	FILTER2D_PRESET_SOBEL_V
} filter2d_preset;

/* 2D filter functions */
struct filter_s *filter2d_create();
const char *filter2d_get_preset_name(filter2d_preset preset);
void filter2d_set_coeff(struct filter_s *fs, const coeff_t coeff);
coeff_t *filter2d_get_coeff(struct filter_s *fs);
void filter2d_set_preset_coeff(struct filter_s *fs, filter2d_preset preset);
const coeff_t *filter2d_get_preset_coeff(filter2d_preset preset);

#ifdef __cplusplus
}
#endif

#endif /* _FILTER2D_H_ */
