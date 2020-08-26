/**
 * Copyright (C) 2020 Jan Nowotsch
 * Author Jan Nowotsch	<jan.nowotsch@gmail.com>
 *
 * Released under the terms of the GNU GPL v2.0
 */



#include <stdlib.h>
#include <stdarg.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <getopt.h>
#include <escape.h>
#include <opt.h>
#include <file.h>
#include <statistics.h>


/* macros */
#define STRINGIFY(v)		#v
#define DEFAULT(v)			"(default: " STRINGIFY(v) ")"
#define DEFAULT2(v0, v1)	"(default: " STRINGIFY(v0) ", " STRINGIFY(v1) ")"
#define DEFAULT_THR()		DEFAULT2(DEFAULT_THR_RED, DEFAULT_THR_YELLOW)


/* local/static prototypes */
static int help(char const *err, ...);

static int parse_thr(char const *arg, threshold_t *thr);
static int parse_src_dirs(char *arg);

static int src_dirs_verify(char const **dirs);

static size_t strcnt(char const *s, char c);


/* global variables */
opt_t opts = {
	.thr_func = { .red = DEFAULT_THR_RED, .yellow = DEFAULT_THR_YELLOW },
	.thr_lines = { .red = DEFAULT_THR_RED, .yellow = DEFAULT_THR_YELLOW },
	.thr_branches = { .red = DEFAULT_THR_RED, .yellow = DEFAULT_THR_YELLOW },
	.src_dirs = 0x0,
	.excl_dirs = 0x0,
	.recursive = DEFAULT_RECURSIVE,
	.list_uncovered = DEFAULT_LIST_UNCOVERED,
	.colour = !DEFAULT_NOCOLOUR,
};


/* global functions */
int opt_parse(int argc, char **argv){
	int opt;
	int long_optind;
	struct option const long_opt[] = {
		{ .name = "thr-func",		.has_arg = required_argument,	.flag = 0,	.val = 'f' },
		{ .name = "thr-lines",		.has_arg = required_argument,	.flag = 0,	.val = 'l' },
		{ .name = "thr-branches",	.has_arg = required_argument,	.flag = 0,	.val = 'b' },
		{ .name = "src-dirs",		.has_arg = required_argument,	.flag = 0,	.val = 's' },
		{ .name = "no-recursion",	.has_arg = no_argument,			.flag = 0,	.val = 'n' },
		{ .name = "uncovered",		.has_arg = no_argument,			.flag = 0,	.val = 'u' },
		{ .name = "no-colour",		.has_arg = no_argument,			.flag = 0,	.val = 'c' },
		{ .name = "help",			.has_arg = no_argument,			.flag = 0,	.val = 'h' },
		{ 0, 0, 0, 0}
	};


	/* set colour flag */
	if(file_typefd(fileno(stdout)) != F_CHAR)
		opts.colour = false;

	/* parse arguments */
	while((opt = getopt_long(argc, argv, ":f:l:b:s:nuch", long_opt, &long_optind)) != -1){
		switch(opt){
		case 'f':	if(parse_thr(optarg, &opts.thr_func) != 0) return -1; break;
		case 'l':	if(parse_thr(optarg, &opts.thr_lines) != 0) return -1; break;
		case 'b':	if(parse_thr(optarg, &opts.thr_branches) != 0) return -1; break;
		case 's':	if(parse_src_dirs(optarg) != 0) return -1; break;
		case 'n':	opts.recursive = false; break;
		case 'u':	opts.list_uncovered = true; break;
		case 'c':	opts.colour = false; break;
		case 'h':	(void)help(0x0); return argc;

		case ':':	return help("missing argument to \"%s\"", argv[optind - 1]);
		case '?':	return help("invalid option \"%s\"", argv[optind - 1]);
		default:	return help("unknown error");
		}
	}

	if(opts.src_dirs == 0x0)
		parse_src_dirs(DEFAULT_SRC_DIR);

	/* opt checks */
	if(src_dirs_verify(opts.src_dirs) != 0 || src_dirs_verify(opts.excl_dirs) != 0)
		return help("invalid src-directory configuartion");

	if(stats_thresholds_verify() != 0)
		return help("invalid coverage threshold configuration, valid range [0, 100]");

	return optind;
}

void opts_cleanup(void){
	free(opts.src_dirs);
}


/* local functions */
static int help(char const *err, ...){
	va_list lst;


	if(err != 0x0 && *err != 0){
		va_start(lst, err);

		printf("%serror%s: ", FG_RED, RESET_ATTR);
		vprintf(err, lst);
		printf("\n\n");

		va_end(lst);
	}

	printf(
		"usage: gcovered [options] {<gcov-file | directory>}\n"
		"\n"
		"    Print function, line and branch coverage information for the given gcov-files\n"
		"    and all gcov-files contained in the given directories.\n"
		"\n"
		"Options:\n"
		"%35.35s    %s\n"
		"%35.35s    %s\n"
		"%35.35s    %s\n"
		"\n"
		"%35.35s    %s\n"
		"%35.35s    %s\n"
		"%35.35s    %s\n"
		"%35.35s    %s\n"
		"%35.35s    %s\n"
		"\n"
		"%35.35s    %s\n"
		,
		"-f, --func-thr=<red,yellow>", "function coverage thresholds [%] " DEFAULT_THR(),
		"-l, --line-thr=<red,yellow>", "line coverage thresholds [%] " DEFAULT_THR(),
		"-b, --branch-thr=<red,yellow>", "branch coverage thresholds [%] " DEFAULT_THR(),
		"-u, --uncovered", "list source files (.c, .cc) without coverage data " DEFAULT(DEFAULT_LIST_UNCOVERED),
		"-s, --src-dirs={[+]src|-excl:}", "list of directories to use or ignore when searching for source",
		"", "files, cf. -u " DEFAULT(DEFAULT_SRC_DIR),
		"-n, --no-recursion", "do not recurse into sub-directories " DEFAULT(DEFAULT_RECURSIVE),
		"-c, --no-colour", "disable coloured output " DEFAULT(DEFAULT_NOCOLOUR),
		"-h, --help", "print this help message"
	);

	return (err == 0x0) ? 0 : -1;
}

static int parse_thr(char const *arg, threshold_t *thr){
	char const *c;


	for(c=arg; *c!=0; c++){
		if(*c == ','){
			thr->red = atof(arg);
			thr->yellow = atof(c + 1);

			return 0;
		}
	}

	return help("invalid threshold syntax");
}

static int parse_src_dirs(char *arg){
	size_t n,
		   n_src,
		   n_excl;
	char *e;


	/* allocate a single array containing src_dirs and excl_dirs
	 * 	each array is zero terminated
	 * 	4 additional elements are allocated as
	 * 		one terminator per array
	 * 		one default, in case only exclude directories are specified
	 *		one in case the last separater is omitted
	 */
	n = strcnt(arg, SRC_DIR_SEPARATOR) + 4;

	opts.src_dirs = calloc(n, sizeof(char*));

	if(opts.src_dirs == 0x0){
		fprintf(stderr, "allocation error %s\n", strerror(errno));
		return -1;
	}

	/* parse the arg string adding src_dirs to the top and excl_dirs
	 * to the bottom
	 */
	n_src = 0;
	n_excl = n - 1;

	for(e=arg; ; e++){
		if(*e == SRC_DIR_SEPARATOR || *e == 0){
			if(*arg == '-')			opts.src_dirs[--n_excl] = arg + 1;
			else if(*arg == '+')	opts.src_dirs[n_src++] = arg + 1;
			else					opts.src_dirs[n_src++] = arg;

			if(*e == 0)
				break;

			*e = 0;
			arg = e + 1;
		}
	}

	opts.excl_dirs = opts.src_dirs + n_excl;

	/* set the default src_dir if needed */
	if(opts.src_dirs[0] == 0x0)
		opts.src_dirs[0] = DEFAULT_SRC_DIR;

	return 0;
}

static int src_dirs_verify(char const **dirs){
	for(; *dirs!=0x0; dirs++){
		if(file_type(*dirs) != F_DIR){
			fprintf(stderr, "\"%s\" is not a directory\n", *dirs);
			return -1;
		}
	}

	return 0;
}

static size_t strcnt(char const *s, char c){
	size_t n;


	for(n=0; *s; s++){
		if(*s == c)
			n++;
	}

	return n;
}
