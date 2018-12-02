/******************************************************************************
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
 *******************************************************************************/
#include <glob.h>
#include <poll.h>
#include <pthread.h>
#include <stdio.h>
#include <drm/drm_fourcc.h>
#include <sys/types.h>
#include <sys/stat.h>

#include "platform.h"
#include "common.h"
#include "helper.h"
#include "video_int.h"
#include "log_events.h"
#include "m2m_sw_pipeline.h"
#include "mediactl_helper.h"
#include "s2m_pipeline.h"
#include "filter.h"

#ifdef GST_MODE
#include "vgst_lib.h"
vgst_enc_params		enc_param;
vgst_sdx_filter_params	filter_param;
vgst_ip_params		input_param;
vgst_op_params		output_param;
vgst_cmn_params		cmn_param;
#endif

/* Maximum number of bytes in a log line */
#define VLIB_LOG_SIZE 256

/* number of frame buffers */
#define BUFFER_CNT_MIN     6
#define BUFFER_CNT_DEFAULT 6

/* global variables */
char vlib_errstr[VLIB_ERRSTR_SIZE];

static struct video_pipeline *video_setup;

static int vlib_platform_setup(struct vlib_config_data *cfg)
{
	int ret = 0;

	switch (cfg->display_id) {
	case DRI_CARD_DP:
		ret = vlib_platform_set_qos(cfg->display_id);
		if (ret) {
			return ret;
		}
		break;
	default:
		VLIB_REPORT_ERR("No valid DRM device found");
		return VLIB_ERROR_INVALID_PARAM;
		break;
	}

	return ret;
}

static void vlib_platform_cleanup(struct video_pipeline *vp)
{
}

static int vlib_filter_init(struct video_pipeline *vp)
{
	int ret;
	struct filter_tbl *ft = vp->ft;
	struct filter_s *fs;
	struct filter_init_data fid = {
		.in_width = vp->w,
		.in_height = vp->h,
		.in_fourcc = vp->in_fourcc,
		.out_width = vp->drm.overlay_plane.vlib_plane.width,
		.out_height = vp->drm.overlay_plane.vlib_plane.height,
		.out_fourcc = vp->out_fourcc,
	};

	for (size_t i = 0; i < ft->size; i++) {
		fs = g_ptr_array_index(ft->filter_types, i);
		if (!fs) {
			return VLIB_ERROR_OTHER;
		}

		/* Initialize filter */
		ret = fs->ops->init(fs, &fid);
		if (ret) {
			vlib_warn("initializing filter '%s' failed\n",
				  filter_type_get_display_text(fs));
			filter_type_unregister(ft, fs);
			i--;
			continue;
		}

		/* Prefetch partial binaries of filters */
		if (video_setup->flags & VLIB_CFG_FLAG_PR_ENABLE) {
			filter_type_prefetch_bin(fs);
		}
	}

	return VLIB_SUCCESS;
}

static int vlib_drm_id2card(struct drm_device *dev, unsigned int dri_card_id)
{
	int ret;
	glob_t pglob;
	const char *drm_str;
	unsigned int dri_card;

	switch (dri_card_id) {
	case DRI_CARD_DP:
		drm_str = "DP";
		ret = glob("/sys/class/drm/card*-DP-1", 0, NULL, &pglob);
		break;
	default:
		VLIB_REPORT_ERR("No valid DRM device found");
		vlib_dbg("%s\n", vlib_errstr);
		ret = VLIB_ERROR_INVALID_PARAM;
		goto error;
	}

	if (ret || pglob.gl_pathc != 1) {
		VLIB_REPORT_ERR("No %s DRM device found", drm_str);
		vlib_dbg("%s\n", vlib_errstr);
		ret = VLIB_ERROR_OTHER;
		goto error;
	}

	sscanf(pglob.gl_pathv[0], "/sys/class/drm/card%u-*", &dri_card);
	snprintf(dev->dri_card, sizeof(dev->dri_card), "/dev/dri/card%u",
		 dri_card);
	vlib_info("%s DRM device found at %s\n", drm_str, dev->dri_card);

error:
	globfree(&pglob);

	return ret;
}

/**
 * vlib_drm_try_mode - Check if a mode with matching resolution is valid
 * @display_id: Display ID
 * @width: Desired mode width
 * @height: Desired mode height
 * @vrefresh: Refresh rate of found mode
 *
 * Search for a mode that supports the desired @widthx@height. If a matching
 * mode is found @vrefresh is populated with the refresh rate for that mode.
 *
 * Return: 0 on success, error code otherwise.
 */
int vlib_drm_try_mode(unsigned int display_id, int width, int height,
		      size_t *vrefresh)
{
	int ret;
	size_t vr;
	struct drm_device drm_dev;

	ret = vlib_drm_id2card(&drm_dev, display_id);
	if (ret) {
		return ret;
	}

	ret = drm_try_mode(&drm_dev, width, height, &vr);
	if (vrefresh) {
		*vrefresh = vr;
	}

	return ret;
}

static int vlib_drm_init(struct vlib_config_data *cfg)
{
	int ret;
	size_t bpp;
	struct drm_device *drm_dev = &video_setup->drm;

	ret = vlib_drm_id2card(drm_dev, cfg->display_id);
	if (ret) {
		return ret;
	}

	drm_dev->overlay_plane.vlib_plane = cfg->plane;
	drm_dev->format = video_setup->out_fourcc;
	drm_dev->vrefresh = cfg->vrefresh;
	drm_dev->buffer_cnt = cfg->buffer_cnt;

	drm_dev->d_buff = calloc(drm_dev->buffer_cnt, sizeof(*drm_dev->d_buff));
	ASSERT2(drm_dev->d_buff, "failed to allocate DRM buffer structs\n");

	bpp = vlib_fourcc2bpp(drm_dev->format);
	if (!bpp) {
		VLIB_REPORT_ERR("unsupported pixel format '%s'",
				(const char *)&drm_dev->format);
		vlib_dbg("%s\n", vlib_errstr);
		return VLIB_ERROR_INVALID_PARAM;
	}

	drm_init(drm_dev, &cfg->plane);

	/* Set display resolution */
	if (!cfg->height_out) {
		/* set preferred mode */
		int ret = drm_find_preferred_mode(drm_dev);
		if (ret)
			return ret;

		video_setup->h_out = drm_dev->preferred_mode->vdisplay;
		video_setup->w_out = drm_dev->preferred_mode->hdisplay;

		if (!video_setup->h) {
			video_setup->h = video_setup->h_out;
			video_setup->w = video_setup->w_out;
			video_setup->stride = video_setup->w * bpp;
		}
	} else {
		video_setup->h_out = cfg->height_out;
		video_setup->w_out = cfg->width_out;
	}

	/* if not specified on the command line make the plane fill the whole screen */
	if (!cfg->plane.width) {
		drm_dev->overlay_plane.vlib_plane.width = video_setup->w_out;
		drm_dev->overlay_plane.vlib_plane.height = video_setup->h_out;
	}

	video_setup->stride_out = drm_dev->overlay_plane.vlib_plane.width * bpp;

	drm_post_init(drm_dev, cfg->drm_background);

	if (!(cfg->flags & VLIB_CFG_FLAG_MULTI_INSTANCE)) {
		/* Move video layer to the back and disable global alpha */
		if (drm_set_plane_prop(drm_dev,
				       video_setup->drm.overlay_plane.drm_plane->plane_id,
				       "zpos", 0)) {
			vlib_warn("failed to set zpos\n");
		}

		if (drm_set_plane_prop(drm_dev,
				       video_setup->drm.prim_plane.drm_plane->plane_id,
				       "zpos", 1) ) {
			vlib_warn("failed to set zpos\n");
		}

		if (drm_set_plane_prop(drm_dev,
				       video_setup->drm.prim_plane.drm_plane->plane_id,
				       "global alpha enable", 0) ) {
			vlib_warn("failed to set 'global alpha'\n");
		}
	}

	if (cfg->display_id == DRI_CARD_DP) {
		/*
		 * New DRM driver sets global alpha ON by default for DP-Tx
		 * Disable it here
		 */
		if (drm_set_plane_prop(drm_dev,
				       video_setup->drm.prim_plane.drm_plane->plane_id,
				       "g_alpha_en", 0) ) {
			vlib_warn("failed to disable 'global alpha'\n");
		}
	}

	vlib_dbg("vlib :: DRM Init done ..\n");

	return VLIB_SUCCESS;
}

int vlib_init(struct vlib_config_data *cfg)
{
	int ret;
	size_t bpp;

	ret = vlib_platform_setup(cfg);
	if (ret) {
		return ret;
	}

	cfg->buffer_cnt = cfg->buffer_cnt ? cfg->buffer_cnt : BUFFER_CNT_DEFAULT;
	if (cfg->buffer_cnt < BUFFER_CNT_MIN) {
		vlib_warn("buffer-count = %zu too low, using %u\n",
			  cfg->buffer_cnt, BUFFER_CNT_MIN);
		cfg->buffer_cnt = BUFFER_CNT_MIN;
	}


#ifdef VSRC_INIT
	ret = vlib_video_src_init(cfg);
	if (ret)
		return ret;
#endif


	/* Allocate video_setup struct and zero out memory */
	video_setup = calloc (1, sizeof(*video_setup));
	video_setup->app_state = MODE_INIT;
	video_setup->in_fourcc = cfg->fmt_in ? cfg->fmt_in : INPUT_PIX_FMT;
	video_setup->out_fourcc = cfg->fmt_out ? cfg->fmt_out : OUTPUT_PIX_FMT;
	video_setup->flags = cfg->flags;
	video_setup->ft = cfg->ft;
	video_setup->buffer_cnt = cfg->buffer_cnt;
	for (size_t i = 0; i < NUM_EVENTS; i++) {
		const char *event_name[] = {
			"Capture", "Display", "Filter-In", "Filter-Out",
		};
		video_setup->events[i] = levents_counter_create(event_name[i]);
		ASSERT2(video_setup->events[i], "failed to create event counter\n");
	}

	bpp = vlib_fourcc2bpp(video_setup->in_fourcc);
	if (!bpp) {
		VLIB_REPORT_ERR("unsupported pixel format '%.4s'",
				(const char *)&video_setup->in_fourcc);
		vlib_dbg("%s\n", vlib_errstr);
		return VLIB_ERROR_INVALID_PARAM;
	}

	/* Set input resolution */
	video_setup->h = cfg->height_in;
	video_setup->w = cfg->width_in;
	video_setup->stride = video_setup->w * vlib_fourcc2bpp(video_setup->in_fourcc);
	video_setup->fps.numerator = cfg->fps.numerator;
	video_setup->fps.denominator = cfg->fps.denominator;

	ret = vlib_drm_init(cfg);
	if (ret) {
		return ret;
	}

	/* Convert V4L fourcc (in_fourcc) to DRM fourcc (out_fourcc) for buffer allocation.
	 * The V4L and DRM fourcc codes do not match in all cases. Since we are using DRM
	 * to allocate input buffers, convert the V4L fourcc to a DRM fourcc with matching
	 * bpp value.
	 *
	 * Example: V4L uses RGB3 and DRM BG24 for the same 24-bit RGB pixel format.
	 */
	video_setup->in_bufs = calloc(video_setup->buffer_cnt,
				       sizeof(*video_setup->in_bufs));
	for (size_t i = 0; i < video_setup->buffer_cnt; i++) {
		ret = drm_buffer_create(&video_setup->drm,
					video_setup->in_bufs + i,
					video_setup->w, video_setup->h,
					video_setup->stride,
					vlib_fourcc_v4l2drm(video_setup->in_fourcc));
		ASSERT2(!ret, "unable to allocate frame buffer: %s",
			strerror(errno));
	}

	/* Initialize filters */
	if (video_setup->ft) {
		ret = vlib_filter_init(video_setup);
		if (ret) {
			return ret;
		}
	}

	/* disable TPG unless input and output resolution match */
	if (video_setup->w != video_setup->w_out ||
		video_setup->h != video_setup->h_out) {
		vlib_video_src_class_disable(VLIB_VCLASS_TPG);
	}

	return ret;
}

int vlib_get_active_height(void)
{
	return video_setup->h_out;
}

int vlib_get_active_width(void)
{
	return video_setup->w_out;
}

static int vlib_pipeline_term_threads(struct video_pipeline *vp)
{
	int ret = 0;

	int ret_i = pthread_cancel(video_setup->fps_thread);
	if (ret_i) {
		vlib_warn("failed to cancel fps thread (%d)\n",ret);
		ret |= ret_i;
	}
	ret_i = pthread_join(video_setup->fps_thread, NULL);
	if (ret_i) {
		vlib_warn("failed to join fps thread (%d)\n",ret);
		ret |= ret_i;
	}
	ret_i = pthread_cancel(video_setup->eventloop);
	if (ret_i) {
		vlib_warn("failed to cancel eventloop(%d)\n",ret);
		ret |= ret_i;
	}

	ret_i = pthread_join(video_setup->eventloop, NULL);
	if (ret_i) {
		vlib_warn("failed to join eventloop (%d)\n",ret);
		ret |= ret_i;
	}

	video_setup->eventloop = 0;

	return ret;
}

int vlib_pipeline_stop(void)
{
	int ret = 0;

	/* Add cleanup code */
	if (video_setup->eventloop) {
		/* Set application state */
		video_setup->app_state = MODE_EXIT;
		/* Stop previous running mode if any */
		ret |= vlib_pipeline_term_threads(video_setup);
		levents_counter_clear(video_setup->events[DISPLAY_EVENT]);
	}
	if (!(video_setup->flags & VLIB_CFG_FLAG_MULTI_INSTANCE)) {
		/* Disable video layer on pipeline stop */
		ret |= drm_set_plane_state(&video_setup->drm,
					   video_setup->drm.overlay_plane.drm_plane->plane_id,
					   0);
	}

	return ret;
}

static int vlib_filter_uninit(struct filter_s *fs)
{
	if (!fs)
		return VLIB_ERROR_OTHER;

	/* free buffers for partial bitstreams */
	if (video_setup->flags & VLIB_CFG_FLAG_PR_ENABLE)
		filter_type_free_bin(fs);

	return VLIB_SUCCESS;
}

int vlib_uninit(void)
{
	struct filter_s *fs;
	int ret;

	/* free input buffers */
	for (size_t i = 0; i < video_setup->buffer_cnt; i++) {
		drm_buffer_destroy(video_setup->drm.fd,
				   video_setup->in_bufs + i);
	}
	free(video_setup->in_bufs);

	drm_uninit(&video_setup->drm);

	vlib_video_src_uninit();

	/* Uninitialize filters */
	if (video_setup->ft) {
		for (size_t i = 0; i < video_setup->ft->size; i++) {
			fs = filter_type_get_obj(video_setup->ft, i);
			ret = vlib_filter_uninit(fs);
			if (ret)
				return ret;
		}
		if (video_setup->ft->size) {
			g_ptr_array_free(video_setup->ft->filter_types, TRUE);
		}
	}

	for (size_t i = 0; i < NUM_EVENTS; i++) {
		levents_counter_destroy(video_setup->events[i]);
	}

	vlib_platform_cleanup(video_setup);

	free(video_setup);

	return ret;
}

static void drm_event_handler(int fd __attribute__((__unused__)),
	unsigned int frame __attribute__((__unused__)),
	unsigned int sec __attribute__((__unused__)),
	unsigned int usec __attribute__((__unused__)),
	void *data )
{
	struct video_pipeline *v_pipe = (struct video_pipeline *)data;

	ASSERT2(v_pipe, " %s :: argument NULL ", __func__);
	v_pipe->pflip_pending = 0;
	/* Count number of VBLANK events */
	levents_capture_event(v_pipe->events[DISPLAY_EVENT]);
}

static void *fps_count_thread(void *arg)
{
	struct video_pipeline *s = arg;

	struct pollfd fds[] = {
		{.fd = s->drm.fd, .events = POLLIN},
	};

	/* setup drm event context */
	drmEventContext evctx;
	memset(&evctx, 0, sizeof(evctx));
	evctx.version = DRM_EVENT_CONTEXT_VERSION;
	evctx.vblank_handler = drm_event_handler;

	while (poll(fds, ARRAY_SIZE(fds), POLL_TIMEOUT_MSEC) > 0 ) {
		if (fds[0].revents & POLLIN) {
			/* Processes outstanding DRM events on the DRM file-descriptor*/
			int ret = drmHandleEvent(s->drm.fd, &evctx);
			ASSERT2(!ret, "drmHandleEvent failed: %s\n", ERRSTR);
		}
	}

	pthread_exit(NULL);
}

int vlib_change_mode(struct vlib_config *config)
{
	int ret;
	struct filter_s *fs = NULL;
	void *(*process_thread_fptr)(void *);

	/* Print requested config */
	vlib_dbg("config: src=%zu, type=%d, mode=%zu\n", config->vsrc,
		 config->type, config->mode);

	if (config->vsrc >= vlib_video_src_cnt_get()) {
		VLIB_REPORT_ERR("invalid video source '%zu'",
				config->vsrc);
		vlib_dbg("%s\n", vlib_errstr);
		return VLIB_ERROR_INVALID_PARAM;
	}

	/* filter is required when output resolution != input resolution */
	if ((video_setup->w != video_setup->drm.overlay_plane.vlib_plane.width ||
	     video_setup->h != video_setup->drm.overlay_plane.vlib_plane.height) &&
	    !config->type) {
		VLIB_REPORT_ERR("invalid filter 'pass through' for selected input/output resolutions");
		vlib_dbg("%s\n", vlib_errstr);
		return VLIB_ERROR_INVALID_PARAM;
	}

	/* Stop processing loop */
	if (video_setup->eventloop) {
		/* Set application state */
		video_setup->app_state = MODE_CHANGE;

		/* Stop previous running mode if any */
		ret = vlib_pipeline_term_threads(video_setup);
	}

	const struct vlib_vdev *vdev = vlib_video_src_get(config->vsrc);
	if (!vdev) {
		return VLIB_ERROR_INVALID_PARAM;
	}

	/* Set video source */
	video_setup->vid_src = vdev;

	if (vdev->ops && vdev->ops->change_mode) {
		ret = vdev->ops->change_mode(video_setup, config);
		if (ret) {
			return ret;
		}
	}

	/* Initialize filter mode */
	if (config->type > 0) {
		fs = filter_type_get_obj(video_setup->ft, config->type - 1);
		if (!fs) {
			config->mode = 0;
		}
	}

	if (fs && config->mode >= filter_type_get_num_modes(fs)) {
		vlib_warn("invalid filter mode '%zu' for filter '%s'\n",
			  config->mode, filter_type_get_display_text(fs));
		config->mode = 0;
	}

	const struct stream_handle *sh;
	switch(config->type) {
	case 0:
		sh = s2m_pipeline_init(video_setup);
		if (!sh) {
			return VLIB_ERROR_CAPTURE;
		}
		process_thread_fptr = s2m_process_event_loop;
		break;
	default:
		filter_type_set_mode(fs, config->mode);	/* 0 is pass through */
		sh = m2m_sw_pipeline_init(video_setup, fs);
		if (!sh) {
			return VLIB_ERROR_CAPTURE;
		}
		process_thread_fptr = m2m_sw_process_event_loop;
		break;
	}

	/* start fps counter thread */
	ret = pthread_create(&video_setup->fps_thread, NULL, fps_count_thread,
			     video_setup);
	if (ret) {
		vlib_warn("failed to create FPS count thread\n");
	}

	/* Start the processing loop */
	ret = pthread_create(&video_setup->eventloop, NULL, process_thread_fptr,
			     (void *)sh);
	ASSERT2(ret >= 0, "thread creation failed \n");

	return VLIB_SUCCESS;
}

int vlib_drm_set_layer0_state(int enable_state)
{
	/* Map primary-plane cordinates into CRTC using drmModeSetPlane */
	drm_set_plane_state(&video_setup->drm,
			    video_setup->drm.prim_plane.drm_plane->plane_id,
			    enable_state);
	return VLIB_SUCCESS;
}

int vlib_drm_set_layer0_transparency(int transparency)
{
	/* Set Layer Alpha for graphics layer */
	drm_set_plane_prop(&video_setup->drm,
			   video_setup->drm.prim_plane.drm_plane->plane_id,
			DRM_ALPHA_PROP, (DRM_MAX_ALPHA-transparency));

	return VLIB_SUCCESS;
}

int vlib_drm_set_layer0_position(int x, int y)
{
	drm_set_prim_plane_pos(&video_setup->drm, x, y);
	return VLIB_SUCCESS;
}

/* Set event-log state */
int vlib_set_event_log(int state)
{
	video_setup->enable_log_event = state;
	return VLIB_SUCCESS;
}

/**
 * vlib_get_event_cnt - Retrieve normalized event counter
 * @event: Event counter to return
 *
 * Return: Normalized event count for @event.
 */
float vlib_get_event_cnt(pipeline_event event)
{
	if (video_setup->enable_log_event && event < NUM_EVENTS) {
#ifdef GST_MODE
		unsigned int getFpsArr[2] = {0};
		vgst_get_fps(0, &getFpsArr[0]);
		return ((float) getFpsArr[0]);
#else
		return levents_counter_get_value(video_setup->events[event]);
#endif
	}

	return VLIB_ERROR_OTHER;
}

/** This function returns a constant NULL-terminated string with the ASCII name of a vlib
 *  error. The caller must not free() the returned string.
 *
 *  \param error_code The \ref vlib_error to return the name of.
 *  \returns The error name, or the string **UNKNOWN** if the value of
 *  error_code is not a known error.
 */
const char *vlib_error_name(vlib_error error_code)
{
	switch (error_code) {
	case VLIB_ERROR_INTERNAL:
		return "VLIB Internal Error";
	case VLIB_ERROR_CAPTURE:
		return "VLIB Capture Error";
	case VLIB_ERROR_INVALID_PARAM:
		return "VLIB Invalid Parameter Error";
	case VLIB_ERROR_FILE_IO:
		return "VLIB File I/O Error";
	case VLIB_ERROR_NOT_SUPPORTED:
		return "VLIB Not Supported Error";
	case VLIB_ERROR_OTHER:
		return "VLIB Other Error";
	case VLIB_SUCCESS:
		return "VLIB Success";
	default:
		return "VLIB Unknown Error";
	}
}

/** This function returns a string with a short description of the given error code.
 *  This description is intended for displaying to the end user.
 *
 *  The messages always start with a capital letter and end without any dot.
 *  The caller must not free() the returned string.
 *
 *  \returns a short description of the error code in UTF-8 encoding
 */
char *vlib_strerror(void)
{
	return vlib_errstr;
}

/* This function returns a string with a log-information w.r.t to the input log-level */
static void vlib_log_str(vlib_log_level level, const char *str)
{
	fputs(str, stderr);
	UNUSED(level);
}

void vlib_log_v(vlib_log_level level, const char *format, va_list args)
{
	const char *prefix = "";
	char buf[VLIB_LOG_SIZE];
	int header_len, text_len;

	switch (level) {
	case VLIB_LOG_LEVEL_INFO:
		prefix = "[vlib info] ";
		break;
	case VLIB_LOG_LEVEL_WARNING:
		prefix = "[vlib warning] ";
		break;
	case VLIB_LOG_LEVEL_ERROR:
		prefix = "[vlib error] ";
		break;
	case VLIB_LOG_LEVEL_DEBUG:
		prefix = "[vlib debug] ";
		break;
	case VLIB_LOG_LEVEL_EVENT:
		prefix = "[vlib event] ";
		break;
	case VLIB_LOG_LEVEL_NONE:
	default:
		return;
	}

	header_len = snprintf(buf, sizeof(buf), "%s", prefix);
	if (header_len < 0 || header_len >= (int)sizeof(buf)) {
		/* Somehow snprintf failed to write to the buffer,
		 * remove the header so something useful is output. */
		header_len = 0;
	}
	/* Make sure buffer is NULL terminated */
	buf[header_len] = '\0';

	text_len = vsnprintf(buf + header_len, sizeof(buf) - header_len, format, args);
	if (text_len < 0 || text_len + header_len >= (int)sizeof(buf)) {
		/* Truncated log output. On some platforms a -1 return value means
		 * that the output was truncated. */
		text_len = sizeof(buf) - header_len;
	}

	if (header_len + text_len >= sizeof(buf)) {
		/* Need to truncate the text slightly to fit on the terminator. */
		text_len -= (header_len + text_len) - sizeof(buf);
	}

	vlib_log_str(level, buf);
}

void vlib_log(vlib_log_level level, const char *format, ...)
{
	va_list args;

	va_start (args, format);
	vlib_log_v(level, format, args);
	va_end (args);
}

int vlib_pipeline_v4l2_init(struct stream_handle *sh, struct video_pipeline *s)
{
	/* Initialize v4l2 video input device */
	sh->video_in.fd = vlib_video_src_get_vnode(s->vid_src);
	sh->video_in.format.pixelformat = s->in_fourcc;
	sh->video_in.format.width = s->w;
	sh->video_in.format.height = s->h;
	sh->video_in.format.bytesperline = s->stride;
	sh->video_in.format.colorspace = V4L2_COLORSPACE_SRGB;
	sh->video_in.mem_type = V4L2_MEMORY_DMABUF;
	sh->video_in.setup_ptr = s;
	sh->video_in.vdev = s->vid_src;
	return v4l2_init(&sh->video_in, s->buffer_cnt);
}

/**
 * vlib_fourcc2bpp - Get bytes per pixel
 * @fourcc: Fourcc pixel format code
 *
 * Return: Number of bytes per pixel for @fourcc or 0.
 */
size_t vlib_fourcc2bpp(uint32_t fourcc)
{
	size_t bpp;

	/* look up bits per pixel */
	switch (fourcc) {
	case V4L2_PIX_FMT_RGB332:
	case V4L2_PIX_FMT_HI240:
	case V4L2_PIX_FMT_HM12:
	case DRM_FORMAT_RGB332:
	case DRM_FORMAT_BGR233:
		bpp = 8;
		break;
	case V4L2_PIX_FMT_YVU410:
	case V4L2_PIX_FMT_YUV410:
		bpp = 9;
		break;
	case V4L2_PIX_FMT_YVU420:
	case V4L2_PIX_FMT_YUV420:
	case V4L2_PIX_FMT_M420:
	case V4L2_PIX_FMT_Y41P:
		bpp = 12;
		break;
	case V4L2_PIX_FMT_RGB444:
	case V4L2_PIX_FMT_ARGB444:
	case V4L2_PIX_FMT_XRGB444:
	case V4L2_PIX_FMT_RGB555:
	case V4L2_PIX_FMT_ARGB555:
	case V4L2_PIX_FMT_XRGB555:
	case V4L2_PIX_FMT_RGB565:
	case V4L2_PIX_FMT_RGB555X:
	case V4L2_PIX_FMT_ARGB555X:
	case V4L2_PIX_FMT_XRGB555X:
	case V4L2_PIX_FMT_RGB565X:
	case V4L2_PIX_FMT_YUYV:
	case V4L2_PIX_FMT_YYUV:
	case V4L2_PIX_FMT_YVYU:
	case V4L2_PIX_FMT_UYVY:
	case V4L2_PIX_FMT_VYUY:
	case V4L2_PIX_FMT_YUV422P:
	case V4L2_PIX_FMT_YUV411P:
	case V4L2_PIX_FMT_YUV444:
	case V4L2_PIX_FMT_YUV555:
	case V4L2_PIX_FMT_YUV565:
	case DRM_FORMAT_XBGR4444:
	case DRM_FORMAT_RGBX4444:
	case DRM_FORMAT_BGRX4444:
	case DRM_FORMAT_ABGR4444:
	case DRM_FORMAT_RGBA4444:
	case DRM_FORMAT_BGRA4444:
	case DRM_FORMAT_XBGR1555:
	case DRM_FORMAT_RGBX5551:
	case DRM_FORMAT_BGRX5551:
	case DRM_FORMAT_ABGR1555:
	case DRM_FORMAT_RGBA5551:
	case DRM_FORMAT_BGRA5551:
	case DRM_FORMAT_RGB565:
	case DRM_FORMAT_BGR565:
		bpp = 16;
		break;
	case V4L2_PIX_FMT_BGR666:
		bpp = 18;
		break;
	case V4L2_PIX_FMT_BGR24:
	case V4L2_PIX_FMT_RGB24:
	case DRM_FORMAT_RGB888:
	case DRM_FORMAT_BGR888:
		bpp = 24;
		break;
	case V4L2_PIX_FMT_BGR32:
	case V4L2_PIX_FMT_ABGR32:
	case V4L2_PIX_FMT_XBGR32:
	case V4L2_PIX_FMT_RGB32:
	case V4L2_PIX_FMT_ARGB32:
	case V4L2_PIX_FMT_XRGB32:
	case V4L2_PIX_FMT_YUV32:
	case DRM_FORMAT_XBGR8888:
	case DRM_FORMAT_RGBX8888:
	case DRM_FORMAT_ABGR8888:
	case DRM_FORMAT_RGBA8888:
	case DRM_FORMAT_XRGB2101010:
	case DRM_FORMAT_XBGR2101010:
	case DRM_FORMAT_RGBX1010102:
	case DRM_FORMAT_BGRX1010102:
	case DRM_FORMAT_ARGB2101010:
	case DRM_FORMAT_ABGR2101010:
	case DRM_FORMAT_RGBA1010102:
	case DRM_FORMAT_BGRA1010102:
		bpp = 32;
		break;
	default:
		return 0;
	}

	/* return bytes required to hold one pixel */
	return (bpp + 7) >> 3;
}

/**
 * vlib_fourcc2bpp - Return media bus string for pixel format
 * @fourcc: Fourcc pixel format code
 *
 * Return: Media bus string for @fourcc or 0.
 */
const char *vlib_fourcc2mbus(uint32_t fourcc)
{
	/* look up mbus format string */
	switch (fourcc) {
	case V4L2_PIX_FMT_YUYV:
	case V4L2_PIX_FMT_UYVY:
		return "UYVY";
	case V4L2_PIX_FMT_BGR24:
	case V4L2_PIX_FMT_RGB24:
	case V4L2_PIX_FMT_BGR32:
	case V4L2_PIX_FMT_ABGR32:
	case V4L2_PIX_FMT_XBGR32:
	case V4L2_PIX_FMT_RGB32:
	case V4L2_PIX_FMT_ARGB32:
	case V4L2_PIX_FMT_XRGB32:
		return "RBG24";
	default:
		return 0;
	}
}

/**
 * vlib_fourcc_v4l2drm - Return DRM fourcc code with matching bpp
 * for V4L fourcc code.
 * @fourcc: V4L fourcc pixel format code
 *
 * Return: DRM fourcc pixel format code or 0.
 */
uint32_t vlib_fourcc_v4l2drm(uint32_t fourcc)
{
	/* look up bits per pixel */
	switch (fourcc) {
	/* 8-bit formats */
//	case V4L2_PIX_FMT_GREY:
//		return DRM_FORMAT_Y8;
	/* 16-bit formats */
	case V4L2_PIX_FMT_YUYV:
	case V4L2_PIX_FMT_UYVY:
		return DRM_FORMAT_YUYV;
	/* 24-bit formats */
	case V4L2_PIX_FMT_BGR24:
	case V4L2_PIX_FMT_RGB24:
//	case V4L2_PIX_FMT_VUY24:
		return DRM_FORMAT_BGR888;
	/* 32-bit formats */
//	case V4L2_PIX_FMT_BGRX32:
//	case V4L2_PIX_FMT_RGBX32:
	case V4L2_PIX_FMT_XBGR32:
	case V4L2_PIX_FMT_XRGB32:
//	case V4L2_PIX_FMT_XYUV32:
//	case V4L2_PIX_FMT_XVUY32:
		return DRM_FORMAT_XBGR8888;
	default:
		return 0;
	}
}

void vlib_drm_drop_master(void)
{
	drmDropMaster(video_setup->drm.fd);
}

void vlib_store_fname_src(const char *file_name)
{
	if (file_name && video_setup)
		video_setup->file_src = file_name;
}

#ifdef GST_MODE
int vlib_pipeline_stop_gst(void)
{
	int ret = 0;

	vgst_stop_pipeline();

	/* Add cleanup code */
	if (video_setup->eventloop) {
		/* Set application state */
		video_setup->app_state = MODE_EXIT;
		levents_counter_clear(video_setup->events[DISPLAY_EVENT]);
	}
	if (!(video_setup->flags & VLIB_CFG_FLAG_MULTI_INSTANCE)) {
		// Disable video layer on pipeline stop
		ret |= drm_set_plane_state(&video_setup->drm,
					   video_setup->drm.overlay_plane.drm_plane->plane_id,
					   0);
	}

	return ret;
}

int vlib_init_gst(struct vlib_config_data *cfg)
{
	int ret;
	size_t bpp;

	ret = vlib_platform_setup(cfg);
	if (ret) {
		return ret;
	}

	cfg->buffer_cnt = 0;

	/* Allocate video_setup struct and zero out memory */
	video_setup = calloc (1, sizeof(*video_setup));
	video_setup->app_state = MODE_INIT;
	video_setup->in_fourcc = cfg->fmt_in ? cfg->fmt_in : INPUT_PIX_FMT;
	video_setup->out_fourcc = cfg->fmt_out ? cfg->fmt_out : OUTPUT_PIX_FMT;
	video_setup->flags = cfg->flags;
	video_setup->ft = cfg->ft;
	video_setup->buffer_cnt = cfg->buffer_cnt;
	for (size_t i = 0; i < NUM_EVENTS; i++) {
		const char *event_name[] = {
			"Capture", "Display", "Filter-In", "Filter-Out",
		};
		video_setup->events[i] = levents_counter_create(event_name[i]);
		ASSERT2(video_setup->events[i], "failed to create event counter\n");
	}

	bpp = vlib_fourcc2bpp(video_setup->in_fourcc);
	if (!bpp) {
		VLIB_REPORT_ERR("unsupported pixel format '%.4s'",
				(const char *)&video_setup->in_fourcc);
		vlib_dbg("%s\n", vlib_errstr);
		return VLIB_ERROR_INVALID_PARAM;
	}

	/* Set input resolution */
	video_setup->h = cfg->height_in;
	video_setup->w = cfg->width_in;
	video_setup->stride = video_setup->w * vlib_fourcc2bpp(video_setup->in_fourcc);
	video_setup->fps.numerator = cfg->fps.numerator;
	video_setup->fps.denominator = cfg->fps.denominator;
	video_setup->display_id = cfg->display_id;

	/* Skip DRM init if -X flag is set */
	if (cfg->flags & VLIB_CFG_FLAG_MEDIA_EXIT) {
		return VLIB_SUCCESS;
	}

	/* Initialize DRM device */
	ret = vlib_drm_init(cfg);
	if (ret) {
		return ret;
	}

	/* Disable TPG unless input and output resolution match */
	if (video_setup->w != video_setup->w_out ||
	    video_setup->h != video_setup->h_out) {
		vlib_video_src_class_disable(VLIB_VCLASS_TPG);
	}

	if (cfg->vcap_file_fn)
		vlib_store_fname_src(cfg->vcap_file_fn);

	/* Set fps to monitor refresh rate if not provided by user */
	if (!video_setup->fps.numerator) {
		size_t vr;
		ret = vlib_drm_try_mode(video_setup->display_id, video_setup->w_out, video_setup->h_out, &vr);
		if (ret == VLIB_SUCCESS) {
			video_setup->fps.numerator = vr;
			video_setup->fps.denominator = 1;
		} else {
			return ret;
		}
	}

	/* Initialize gst_lib structs */
	cmn_param.num_src = 1;
	cmn_param.sink_type = DISPLAY;
	cmn_param.monitor_refresh_rate = video_setup->fps.numerator;
	cmn_param.driver_type = video_setup->display_id;
	cmn_param.plane_id = video_setup->drm.overlay_plane.drm_plane->plane_id;

	input_param.height = vlib_get_active_height();
	input_param.width = vlib_get_active_width();
	input_param.filter_type = SDX_FILTER;
	input_param.format = (char *)& (video_setup->in_fourcc);
	/* Convert YUYV to YUY2 */
	if (video_setup->in_fourcc == v4l2_fourcc('Y','U','Y','V')) {
		input_param.format = "YUY2";
	}

	return ret;
}

int vlib_uninit_gst(void)
{
	int ret = 0;

	drm_uninit(&video_setup->drm);

	vlib_video_src_uninit();

	for (size_t i = 0; i < NUM_EVENTS; i++) {
		levents_counter_destroy(video_setup->events[i]);
	}

	vlib_platform_cleanup(video_setup);

	free(video_setup);

	return ret;
}

int vlib_change_mode_gst(struct vlib_config *config)
{
	int ret = VLIB_SUCCESS;
	static int stop_flag = 0;
	struct filter_s *fs = NULL;

	/* Print requested config */
	vlib_dbg("config: src=%zu, type=%d, mode=%zu\n", config->vsrc,
		 config->type, config->mode);

	if (config->vsrc >= vlib_video_src_cnt_get()) {
		VLIB_REPORT_ERR("invalid video source '%zu'",
				config->vsrc);
		vlib_dbg("%s\n", vlib_errstr);
		return VLIB_ERROR_INVALID_PARAM;
	}

	/* Stop processing loop */
	if (video_setup->eventloop) {
		/* Set application state */
		video_setup->app_state = MODE_CHANGE;
	}

	const struct vlib_vdev *vdev = vlib_video_src_get(config->vsrc);
	if (!vdev) {
		return VLIB_ERROR_INVALID_PARAM;
	}

	/* Set video source */
	video_setup->vid_src = vdev;

	if (vdev->ops && vdev->ops->change_mode) {
		ret = vdev->ops->change_mode(video_setup, config);
		if (ret) {
			return ret;
		}
	}

	/* Configure media pipeline */
	if (vdev->ops && vdev->ops->set_media_ctrl) {
		ret = vdev->ops->set_media_ctrl(video_setup, vdev);
		ASSERT2(!ret, "failed to configure media pipeline\n");
	}

	/* if custom frame rate handler exists call it instead */
	if (video_setup->fps.numerator != 0 &&
	    video_setup->fps.denominator != 0 &&
	    vdev->ops && vdev->ops->set_frame_rate) {
		ret = vdev->ops->set_frame_rate(vdev,
				video_setup->fps.numerator,
				video_setup->fps.denominator);
		if (ret) {
			return ret;
		}
	}

	if (vdev->ops && vdev->ops->prepare) {
		ret = vdev->ops->prepare(video_setup, vdev);
		VLIB_REPORT_ERR("preparing video source failed");
		vlib_dbg("%s\n", vlib_errstr);
		if (ret) {
			return ret;
		}
	}

	if (video_setup->flags && (video_setup->flags & VLIB_CFG_FLAG_MEDIA_EXIT)) {
		return ret;
	}

	if (stop_flag) {
		vgst_stop_pipeline();
	}

	if (stop_flag == 0) {
		stop_flag = 1;
	}

	/* Select between v4l2src and filesrc */
	input_param.device_type = config->vsrc;
	input_param.src = V4L2_SRC_NAME;
	if ((vdev->vsrc_class == VLIB_VCLASS_FILE) && (config->vsrc == (vlib_video_src_cnt_get() - 1))) {
		if (video_setup->file_src == NULL) {
			VLIB_REPORT_ERR("No video file selected. Use file browser to select "
				"input file.");
			vlib_dbg("%s\n", vlib_errstr);
			return VLIB_ERROR_FILE_IO;
		}
		input_param.src = FILE_SRC_NAME;
		input_param.uri = strdup(video_setup->file_src);
	}

	/* Select between passthrough and sdxfilter*/
	input_param.raw = TRUE;
	input_param.io_mode = VGST_V4L2_IO_MODE_DMABUF_EXPORT;
	if (config && config->type > 0) {
		input_param.raw = FALSE;
		fs = filter_type_get_obj(video_setup->ft, config->type - 1);
		filter_param.filter_name = strdup(fs->dt_comp_string);
		if (fs && config->mode >= filter_type_get_num_modes(fs)) {
			vlib_warn("invalid filter mode '%zu' for filter '%s'\n",
			  config->mode, filter_type_get_display_text(fs));
			config->mode = 0;
		}
		if (strcmp(filter_type_get_mode(fs, config->mode), "HW") == 0) {
			filter_param.filter_mode = GST_FILTER_MODE_HW;
		} else {
			filter_param.filter_mode = GST_FILTER_MODE_SW;
		}
		if (vdev->vsrc_class == VLIB_VCLASS_FILE ||
		    vdev->vsrc_class == VLIB_VCLASS_UVC  ||
		    vdev->vsrc_class == VLIB_VCLASS_VIVID) {
			input_param.io_mode = VGST_V4L2_IO_MODE_AUTO;
		}
	}

	/* Apply all config parameters */
	ret = vgst_config_options(&enc_param, &input_param, &output_param, &cmn_param, &filter_param);
	if (ret != VGST_SUCCESS) {
		fprintf(stderr, "ERROR: vgst_config_options failed\n");
	}

	ret = vgst_start_pipeline();

	if (config && config->type > 0)
		free(filter_param.filter_name);
	if (config->vsrc == (vlib_video_src_cnt_get() - 1))
		free(input_param.uri);

	return ret;
}
#endif
