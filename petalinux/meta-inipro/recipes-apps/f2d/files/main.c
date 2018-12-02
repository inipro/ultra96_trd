#include <stdio.h>
#include <stdint.h>
#include <getopt.h>
#include <errno.h>
#include <signal.h>
#include <stdlib.h>
#include <linux/videodev2.h>
#include <drm/drm_fourcc.h>

#include <unistd.h>

#include "video.h"

#if defined (SAMPLE_FILTER2D)
#include "filter2d.h"
	int ret;
#endif

#ifndef WITH_SDSOC
__attribute__((constructor))
    void open_xlnk() {
            cf_xlnk_open(1);
    }   
__attribute__((destructor))
    void close_xlnk() {
            xlnkClose(1, NULL);
    }
#endif

static void filter_init(struct filter_tbl *ft, void *sv_params)
{
	UNUSED(ft);
#if defined (SAMPLE_FILTER2D)
	{
		struct filter_s *fs = filter2d_create();
		if (filter_type_register(ft, fs)) {
			printf("Failed to register filter %s\n",
					filter_type_get_display_text(fs));
		}
	}
#endif
}

static struct {
	unsigned int flags;
} vcmd_cleanup_data;

static void vcmd_cleanup(void)
{
	vlib_pipeline_stop();
	if (!(vcmd_cleanup_data.flags & VLIB_CFG_FLAG_MULTI_INSTANCE)) {
		vlib_drm_set_layer0_state(1);
	}
	vlib_uninit();
}

static void vcmd_sig_handler(int signo)
{
	switch (signo) {
		case SIGINT:
		case SIGTERM:
			//usleep(100000);
			vcmd_cleanup();
			exit(0);
		default:
			break;
	}
}

static int getInput(void)
{
	int ch;
	int ret = -1;

	ch = getchar();
	if (ferror(stdin)) {
		perror("error reading input stream");
		abort();
	}

	if (ch >= '0' && ch <= '9')
		ret = ch - '0';

	while ((ch = getchar()) != '\n' && ch != EOF);

	return ret;
}

static void list_video_sources(void)
{
	printf("%16.16s\tID\n", "VIDEO SOURCE");

	for (size_t i=0; i<vlib_video_src_cnt_get(); i++) {
		printf("%16.16s\t%zu\n", vlib_video_src_get_display_text_from_id(i), i);
	}
}

static void list_filters(const struct filter_tbl *ft)
{
	printf("%16.16s\tID\n", "FILTER");
	printf("%16.16s\t%u\n", "pass through", 0);

	for (size_t i=0; i<ft->size; i++) {
		struct filter_s *fs = g_ptr_array_index(ft->filter_types, i);
		printf("%16.16s\t%zu\n", filter_type_get_display_text(fs), i+1);
	}
}

static void list_filter_modes(const struct filter_tbl *ft)
{
	for (size_t i=0; i<ft->size; i++) {
		struct filter_s *fs = g_ptr_array_index(ft->filter_types, i);
		printf("%s (%zu):\n", filter_type_get_display_text(fs), i+1);
		printf("%16.16s\tID\n", "MODE");

		for (size_t j=0; j<filter_type_get_num_modes(fs); j++) {
			printf("%16.16s\t%zu\n", filter_type_get_mode(fs,j), j);
		}
	}
}

#define VCMD_LIST_FLAG_SOURCES		BIT(0)
#define VCMD_LIST_FLAG_FILTERS		BIT(1)
#define VCMD_LIST_FLAG_FILTERMODES	BIT(2)
static void list_any(unsigned int flags, const struct filter_tbl *ft)
{
	if (flags & VCMD_LIST_FLAG_SOURCES) {
		list_video_sources();
		printf("\n");
	}

	if (flags & VCMD_LIST_FLAG_FILTERS) {
		list_filters(ft);
		printf("\n");
	}

	if (flags & VCMD_LIST_FLAG_FILTERMODES) {
		list_filter_modes(ft);
		printf("\n");
	}
}

static void usage(const char *argv0)
{
	printf("Usage: %s [options]\n", argv0);
	printf("-d, --display N                      Display number N (default N=0)\n");
	printf("-h, --help                            Show this help screen\n");
#ifdef EXPERT_OPTIONS
	printf("-p, --partial-reconfig                Enable partial reconfiguration of image filter\n");
#endif
	printf("-i, --input-format WxH[@FMT]          Input Width'x'Height@Fmt\n");
	printf("-o, --output-format WxH[-HZ][@FMT]    Output Width'x'Height-Hz@Fmt\n");
	printf("-f, --fps N/D                         Capture frame rate\n");
#ifdef EXPERT_OPTIONS
	printf("-u, --multi-instance                  Multi-instance mode\n");
#endif
	printf("-I, --non-interactive                 Non-interactive mode\n");
	printf("-S, --list-sources                    List video sources\n");
	printf("-L, --list-filters                    List filters\n");
	printf("-M, --list-filter-modes               List filter modes\n");
	printf("-s, --video-source                    Video source ID\n");
	printf("-l, --filter                          Set filter\n");
	printf("-m, --filter-mode                     Set filter mode\n");
	printf("-P, --plane <id>[:<w>x<h>[+<x>+<y>]]  Use specific plane\n");
	printf("-b, --buffer-count                    Number of frame buffers\n");
#ifdef EXPERT_OPTIONS
	printf("    --vcap-file-file                  File for file source\n");
	printf("    --background-file                 File for background\n");
#endif
	printf("    --filter-sv-cam-params            File for stereo camera parameters\n");
}

static struct option opts[] = {
	{ "display", required_argument, NULL, 'd' },
	{ "help", no_argument, NULL, 'h' },
	{ "partial-reconfig", no_argument, NULL, 'p' },
	{ "input-format", required_argument, NULL, 'i' },
	{ "output-format", required_argument, NULL, 'o' },
	{ "fps", required_argument, NULL, 'f' },
	{ "multi-instance", no_argument, NULL, 'u' },
	{ "list-sources", no_argument, NULL, 'S' },
	{ "list-filters", no_argument, NULL, 'L' },
	{ "video-source", required_argument, NULL, 's' },
	{ "list-filter-modes", no_argument, NULL, 'M' },
	{ "set-filter-mode", required_argument, NULL, 'm' },
	{ "interactive", no_argument, NULL, 'I' },
	{ "filter", required_argument, NULL, 'l' },
	{ "vcap-file-file", required_argument, NULL, 'q' },
	{ "plane", required_argument, NULL, 'P' },
	{ "background-file", required_argument, NULL, 'Q' },
	{ "buffer-count", required_argument, NULL, 'b' },
	{ "filter-sv-cam-params", required_argument, NULL, 'Z' },
	{ NULL, 0, NULL, 0 }
};

static void menu_loop(struct vlib_config *config, struct filter_tbl *ft,
					unsigned int flags)
{
	int ret, choice, current_choice = -1;
	struct filter_s *fs = NULL;

	/* Main control menu */
	do {
		struct vlib_config current_config;
		const char *t;
		/* start with menu index 1 since 0 is used for exit */
		size_t i, k, j = 1;

		printf("\n--------------- Select Video Source ---------------\n");
		for (i=0; i<vlib_video_src_cnt_get(); i++) {
			if (config->vsrc == i)
				t = "(*)";
			else
				t = "";
			printf("%zu : %s  %s\n", j++,
					vlib_video_src_get_display_text_from_id(i), t);
		}
		k = j;
		if (!ft->size)
			goto menu_exit;
		printf("\n--------------- Select Filter Type ----------------\n");
		printf("%zu : %s  %s\n", k++, "pass through",
								config->type ? "" : "(*)");
		for (i=0; i<ft->size; i++) {
			fs = filter_type_get_obj(ft, i);
			ASSERT2(fs, "no filter\n");
			printf("%zu : %s  %s\n", k++,
									filter_type_get_display_text(fs),
									config->type == i+1 ? "(*)" : "");
		}
		if (config->type) {
			printf("\n--------------- Toggle Filter Mode ----------------\n");
			fs = filter_type_get_obj(ft, config->type - 1);
			printf("%zu : ", k);
			for (size_t i=0; i<filter_type_get_num_modes(fs); i++) {
				printf("%s%s", i ? "/" : "",
								filter_type_get_mode(fs, i));
			}
			printf(" (%s)\n", filter_type_get_mode(fs, config->mode));
		}
menu_exit:
		printf("\n--------------- Exit Application ------------------\n");
		printf("0 : Exit\n\n");
		printf("\nEnter your choice : ");

		choice = getInput();

		/* Same option selected */
		if (current_choice == choice && choice < k)
			continue;

		current_config = *config;

		if (choice == 0) {
			/* exit application */
			vcmd_cleanup();
			exit(0);
		} else if (choice > 0 && choice < j) {
			/* input source select */
			config->vsrc = choice - 1;
		} else if (choice >= j && choice < k) {
			/* filter select */
			config->type = choice - j;
		} else if (choice == k) {
			/* cycle filter mode */
			config->mode++;
			config->mode %= filter_type_get_num_modes(fs);
		} else {
			printf("\n\n********* Invalid input, Please try Again ***********\n");
			continue;
		}

		/* Switch to selected video src, filter type, filter mode */
		ret = vlib_change_mode(config);
		if (ret) {
			printf("ERROR: %s\n", vlib_errstr);
			*config = current_config;
			vlib_change_mode(config);
		} else {
			current_choice = choice;
		}
	} while (choice);
}

static int parse_output_arg(char *arg, struct vlib_config_data *cfg)
{
	int ret;

	ret = sscanf(arg, "%ux%u", &cfg->width_out, &cfg->height_out);
	if (ret < 2) {
		return 1;
	}

	char *start = strpbrk(arg, "-@");
	if (!start) {
		return 0;
	}

	char *end = start + strlen(start);

	if (*start == '-') {
		cfg->vrefresh = strtol(start + 1, &end, 10);
		start = end;
	}

	if (*start == '@') {
		cfg->fmt_out = v4l2_fourcc(start[1],
								start[2],
								start[3],
								start[4]);
	}

	return 0;
}

int main(int argc, char *argv[])
{
	int c, ret = 0;
	int list_flags = 0;
	int interactive_mode = 1;
	unsigned int video_source = 0;
	char *sv_cam_params = NULL;
	struct filter_tbl ft = {};
	struct vlib_config_data cfg = {
		.ft = &ft,
		.fmt_in = V4L2_PIX_FMT_YUYV, .fmt_out = V4L2_PIX_FMT_YUYV,
	};
	struct vlib_config config = {};
	const struct sigaction sa_sigintterm = {
		.sa_handler = vcmd_sig_handler,
	};

	/* Parse command line arguments */
	while ((c = getopt_long(argc, argv, "d:hpi:o:f:uSLs:Mm:Il:P:b:", opts, NULL)) != -1) {
		switch (c) {
			case 'd':
				sscanf(optarg, "%u", &cfg.display_id);
				break;
			case 'h':
				usage(argv[0]);
				return 0;
			case 'p':
				cfg.flags |= VLIB_CFG_FLAG_PR_ENABLE;
				break;
			case 'i':
				{
					char *fmt_s_in;

					ret = sscanf(optarg, "%ux%u@%ms", &cfg.width_in,
							&cfg.height_in, &fmt_s_in);
					if (ret < 2) {
						printf("Invalid size '%s'\n", optarg);
						return 1;
					}

					if (ret > 2) {
						cfg.fmt_in = v4l2_fourcc(fmt_s_in[0],
												fmt_s_in[1],
												fmt_s_in[2],
												fmt_s_in[3]);
						free(fmt_s_in);
					}

					/*
					if (!cfg.width_out) {
						cfg.width_out = cfg.width_in;
						cfg.height_out = cfg.height_in;
						cfg.fmt_out = cfg.fmt_in;
					}
					*/
				}
				break;
			case 'o':
				ret = parse_output_arg(optarg, &cfg);
				if (ret) {
					fprintf(stderr, "invalid output specifier '%s'\n", optarg);
					return ret;
				}
				break;
			case 'f':
				ret = sscanf(optarg, "%u/%u", &cfg.fps.numerator,
						&cfg.fps.denominator);
				if (ret < 1) {
					printf("Invalid frame rate '%s'\n", optarg);
					return 1;
				}

				if (ret < 2) {
					cfg.fps.denominator = 1;
				}
				break;
			case 'u':
				cfg.flags |= VLIB_CFG_FLAG_MULTI_INSTANCE;
				vcmd_cleanup_data.flags |= VLIB_CFG_FLAG_MULTI_INSTANCE;
				break;
			case 'S':
				list_flags |= VCMD_LIST_FLAG_SOURCES;
				break;
			case 'L':
				list_flags |= VCMD_LIST_FLAG_FILTERS;
				break;
			case 's':
				video_source = strtol(optarg, NULL, 0);
				video_source++;
				break;
			case 'M':
				list_flags |= VCMD_LIST_FLAG_FILTERMODES;
				break;
			case 'm':
				config.mode = strtol(optarg, NULL, 0);
				break;
			case 'I':
				interactive_mode = 0;
				break;
			case 'l':
				config.type = strtol(optarg, NULL, 0);
				break;
			case 'q':
				cfg.vcap_file_fn = optarg;
				break;
			case 'P':
				ret = sscanf(optarg, "%zu:%zux%zu+%zu+%zu", &cfg.plane.id,
						&cfg.plane.width, &cfg.plane.height,
						&cfg.plane.xoffs, &cfg.plane.yoffs);
				if (ret < 1 || ret == 2 || ret == 4) {
					fprintf(stderr, "ERROR: Invalid plane specifier '%s'\n",
							optarg);
					return -1;
				}

				break;
			case 'Q':
				cfg.drm_background = optarg;
				break;
			case 'b':
				cfg.buffer_cnt = strtol(optarg, NULL, 0);
				break;
			case 'Z':
				sv_cam_params = optarg;
				break;
			default:
				printf("Invalid option '%c'\n", c);
				printf("Run %s -h for help\n", argv[0]);
				return 1;
		}
	}

	/* Register filters */
	filter_init(&ft, sv_cam_params);

	if (!cfg.width_in) {
		cfg.width_in = cfg.width_out;
		cfg.height_in = cfg.height_out;
	}

	/* Initialize video library */
	ret = vlib_init(&cfg);
	if (ret) {
		fprintf(stderr, "ERROR: vlib_init failed: %s\n", vlib_errstr);
		return ret;
	}

	if (list_flags) {
		list_any(list_flags, &ft);
		vlib_uninit();
		return 0;
	}

	if (!(cfg.flags & VLIB_CFG_FLAG_MULTI_INSTANCE)) {
		vlib_drm_set_layer0_state(0); // Disable Layer0 (used for graphics only)
	}

	/* Print application status */
	printf("Video Control application:\n");
	printf("------------------------\n");
	printf("Display resolution: %dx%d\n", vlib_get_active_width(), vlib_get_active_height());

	if (video_source) {
		config.vsrc = video_source - 1;
	}

	if (!interactive_mode) vlib_set_event_log(1);

	/* Start default video src, filter type, filter mode */
	ret = vlib_change_mode(&config);
	if (ret) {
		printf("ERROR: %s\n", vlib_errstr);
	}
	
	ret = sigaction(SIGINT, &sa_sigintterm, NULL);
	if (ret) {
		fprintf(stderr, "WARN: cannot install handler for SIGINT\n");
	}
	ret = sigaction(SIGTERM, &sa_sigintterm, NULL);
	if (ret) {
		fprintf(stderr, "WARN: cannot install handler for SIGTERM\n");
	}

	if (interactive_mode) {
		menu_loop(&config, &ft, cfg.flags);
	} else {
		while (1) {
			;
		}
	}

	return 0;
}
