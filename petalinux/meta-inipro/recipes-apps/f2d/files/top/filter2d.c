#include "filter.h"
#include "helper.h"

#include "filter2d.h"


/* Foward declaration */
void filter2d_cv(unsigned short *frm_data_in, unsigned short *frm_data_out,
				int height, int width, const coeff_t coeff);

const coeff_t coeff_blur = {
	{1,  1, 1},
	{1, -7, 1},
	{1,  1, 1}
};

const coeff_t coeff_edge = {
	{0,  0, 0},
	{1, -4, 1},
	{0,  1, 0}
};

const coeff_t coeff_edge_h = {
	{0, -1, 0},
	{0,  2, 0},
	{0, -1, 0}
};

const coeff_t coeff_edge_v = {
	{ 0, 0,  0},
	{-1, 2, -1},
	{ 0, 0,  0}
};

const coeff_t coeff_emboss = {
	{-2, -1, 0},
	{-1,  1, 1},
	{ 0,  1, 2}
};

const coeff_t coeff_gradient_h = {
	{-1, -1, -1},
	{ 0,  0,  0},
	{ 1,  1,  1}
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
	{ 0, -1,  0},
	{-1,  5, -1},
	{ 0, -1,  0}
};

const coeff_t coeff_sobel_h = {
	{ 1,  2,  1},
	{ 0,  0,  0},
	{-1, -2, -1}
};

const coeff_t coeff_sobel_v = {
	{ 1, 0, -1},
	{ 2, 0, -2},
	{ 1, 0, -1}
};

/* store current coefficients */
coeff_t coeff_cur;

static struct {
	filter2d_preset preset;
	const char *name;
	const coeff_t *coeff;
} filter2d_presets[] = {
	{ FILTER2D_PRESET_BLUR, "Blur", &coeff_blur },
	{ FILTER2D_PRESET_EDGE, "Edge", &coeff_edge },
	{ FILTER2D_PRESET_EDGE_H, "Edge Horizontal", &coeff_edge_h },
	{ FILTER2D_PRESET_EDGE_V, "Edge Vertical", &coeff_edge_v },
	{ FILTER2D_PRESET_EMBOSS, "Emboss", &coeff_emboss },
	{ FILTER2D_PRESET_GRADIENT_H, "Gradient Horizontal", &coeff_gradient_h },
	{ FILTER2D_PRESET_GRADIENT_V, "Gradient Vertial", &coeff_gradient_v },
	{ FILTER2D_PRESET_IDENTITY, "Identity", &coeff_identity},
	{ FILTER2D_PRESET_SHARPEN, "Sharpen", &coeff_sharpen },
	{ FILTER2D_PRESET_SOBEL_H, "Sobel Horizontal", &coeff_sobel_h },
	{ FILTER2D_PRESET_SOBEL_V, "Sobel Vertical", &coeff_sobel_v } 
};

const char *filter2d_get_preset_name(filter2d_preset preset)
{
	unsigned int i;

	for (i=0; i<ARRAY_SIZE(filter2d_presets); ++i) {
		if (filter2d_presets[i].preset == preset)
			return filter2d_presets[i].name;
	}

	return NULL;
}

void filter2d_set_coeff(struct filter_s *fs, const coeff_t coeff)
{
	unsigned int row;
	unsigned int col;

	/* store new values */
	for (row=0; row<KSIZE; row++)
		for (col=0; col<KSIZE; col++)
			coeff_cur[row][col] = coeff[row][col];
}

coeff_t *filter2d_get_coeff(struct filter_s *fs)
{
	return &coeff_cur;
}

void filter2d_set_preset_coeff(struct filter_s *fs, filter2d_preset preset)
{
	unsigned int i;

	for (i=0; i<ARRAY_SIZE(filter2d_presets); ++i) {
		if (filter2d_presets[i].preset == preset)
			filter2d_set_coeff(fs, *filter2d_presets[i].coeff);
	}
}

const coeff_t *filter2d_get_preset_coeff(filter2d_preset preset)
{
	unsigned int i;

	for (i=0; i<ARRAY_SIZE(filter2d_presets); ++i) {
		if (filter2d_presets[i].preset == preset)
			return filter2d_presets[i].coeff;
	}

	return NULL;
}

static int filter2d_init(struct filter_s *fs, const struct filter_init_data *fid)
{
	/* filter2d requires equal input/output resolution */
	if (fid->in_height != fid->out_height ||
			fid->in_width != fid->out_width) {
		return -1;
	}

	filter2d_set_preset_coeff(fs, FILTER2D_PRESET_SOBEL_H);

#ifdef WITH_SDSOC
	filter2d_init_sds(fid->in_height, fid->in_width, fid->out_height,
					fid->out_width, fid->in_fourcc, fid->out_fourcc,
					&fs->data);
#endif

	return 0;
}

static void filter2d_func(struct filter_s *fs,
					unsigned short *frm_data_in, unsigned short *frm_data_out,
					int height_in, int width_in, int stride_in,
					int height_out, int width_out, int stride_out)
{
	if (fs->mode >= fs->num_modes) {
		return;
	}

	switch (fs->mode) {
		case 0:
#ifdef WITH_SDSOC
			filter2d_sds(frm_data_in, frm_data_out, height_in, width_in,
						coeff_cur, fs->data);
			break;
		case 1:
#endif
		default:
			filter2d_cv(frm_data_in, frm_data_out, height_in, width_in,
						coeff_cur);
			break;
	}
}

static struct filter_ops ops = {
	.init = filter2d_init,
	.func = filter2d_func
};

static const char *f2d_modes[] = {
#ifdef WITH_SDSOC
	"HW",
#endif
	"SW",
};

const static struct filter_s FS = {
	.display_text = "2D Filter",
	.dt_comp_string = "xlnx,v-hls-filter2d",
	.pr_file_name = "",
	.pr_buf = NULL,
	.fd = -1,
	.mode = 0,
	.ops = &ops,
	.data = NULL,
	.num_modes = ARRAY_SIZE(f2d_modes),
	.modes = f2d_modes
};

struct filter_s *filter2d_create(void)
{
	struct filter_s *fs = (struct filter_s *)(malloc(sizeof *fs));
	*fs = FS;

	return fs;
}
