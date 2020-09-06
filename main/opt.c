/**
 * Copyright (C) 2020 Jan Nowotsch
 * Author Jan Nowotsch	<jan.nowotsch@gmail.com>
 *
 * Released under the terms of the GNU GPL v2.0
 */



#include <version.h>
#include <sys/types.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdarg.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <getopt.h>
#include <util/file.h>
#include <util/vector.h>
#include <util/string.h>
#include <util/escape.h>
#include <rc/parser.tab.h>
#include <cov/cov.h>
#include <opt.h>


/* macros */
#define STRINGIFY(v)		#v
#define DEFAULT(v)			"(default: " STRINGIFY(v) ")"
#define DEFAULT2(v0, v1)	"(default: " STRINGIFY(v0) ", " STRINGIFY(v1) ")"
#define DEFAULT_THR()		DEFAULT2(DEFAULT_THR_RED, DEFAULT_THR_YELLOW)


/* local/static prototypes */
static int help(char const *err, ...);
static void version(void);

static int parse_thr(char const *arg, threshold_t *thr);
static int parse_dirs(char *cfg, vector_t *tgt);

static int dirs_verify(vector_t *files, bool allow_file);

/* global variables */
opt_t opts = {
	.rc_file = DEFAULT_RC_FILE,
	.thr_func = { .red = DEFAULT_THR_RED, .yellow = DEFAULT_THR_YELLOW },
	.thr_lines = { .red = DEFAULT_THR_RED, .yellow = DEFAULT_THR_YELLOW },
	.thr_branches = { .red = DEFAULT_THR_RED, .yellow = DEFAULT_THR_YELLOW },
	.src_dirs = VECTOR_INIT(),
	.excludes = VECTOR_INIT(),
	.recursive = DEFAULT_RECURSIVE,
	.list_uncovered = DEFAULT_LIST_UNCOVERED,
	.list_excluded = DEFAULT_LIST_EXCLUDED,
	.colour = !DEFAULT_NOCOLOUR,
};


/* global functions */
int opt_parse(int argc, char **argv){
	int r;
	int opt;
	int long_optind;
	char const optstr[] = ":r:f:l:b:s:e:nuxcvh";
	struct option const long_opt[] = {
		{ .name = "rc",				.has_arg = required_argument,	.flag = 0,	.val = 'r' },
		{ .name = "thr-func",		.has_arg = required_argument,	.flag = 0,	.val = 'f' },
		{ .name = "thr-lines",		.has_arg = required_argument,	.flag = 0,	.val = 'l' },
		{ .name = "thr-branches",	.has_arg = required_argument,	.flag = 0,	.val = 'b' },
		{ .name = "source",			.has_arg = required_argument,	.flag = 0,	.val = 's' },
		{ .name = "exclude",		.has_arg = required_argument,	.flag = 0,	.val = 'e' },
		{ .name = "no-recursion",	.has_arg = no_argument,			.flag = 0,	.val = 'n' },
		{ .name = "uncovered",		.has_arg = no_argument,			.flag = 0,	.val = 'u' },
		{ .name = "excluded",		.has_arg = no_argument,			.flag = 0,	.val = 'x' },
		{ .name = "no-colour",		.has_arg = no_argument,			.flag = 0,	.val = 'c' },
		{ .name = "version",		.has_arg = no_argument,			.flag = 0,	.val = 'v' },
		{ .name = "help",			.has_arg = no_argument,			.flag = 0,	.val = 'h' },
		{ 0, 0, 0, 0}
	};


	/* allocations */
	r = 0;
	r |= vector_init(&opts.src_dirs, 4);
	r |= vector_init(&opts.excludes, 4);

	if(r != 0){
		fprintf(stderr, "allocation error %s\n", strerror(errno));
		return -1;
	}

	/* set colour flag */
	if(file_typefd(fileno(stdout)) != F_CHAR)
		opts.colour = false;

	/* parse arguments (checking for rc-file) */
	r = 0;

	while((opt = getopt_long(argc, argv, optstr, long_opt, &long_optind)) != -1){
		if(opt == 'r'){
			opts.rc_file = optarg;
			r = 1;
			break;
		}
	}

	if(file_type(opts.rc_file) != F_ERROR){
		if(rcparse(opts.rc_file) != 0)
			return -1;
	}
	else if(r == 1){
		fprintf(stderr, "invalid rc-file \"%s\"\n", opts.rc_file);
		return -1;
	}

	/* parse arguments */
	optind = 1;

	while((opt = getopt_long(argc, argv, optstr, long_opt, &long_optind)) != -1){
		switch(opt){
		case 'r':	break;
		case 'f':	if(parse_thr(optarg, &opts.thr_func) != 0) return -1; break;
		case 'l':	if(parse_thr(optarg, &opts.thr_lines) != 0) return -1; break;
		case 'b':	if(parse_thr(optarg, &opts.thr_branches) != 0) return -1; break;
		case 's':	if(parse_dirs(optarg, &opts.src_dirs) != 0) return -1; break;
		case 'e':	if(parse_dirs(optarg, &opts.excludes) != 0) return -1; break;
		case 'n':	opts.recursive = false; break;
		case 'u':	opts.list_uncovered = true; break;
		case 'x':	opts.list_excluded = true; break;
		case 'c':	opts.colour = false; break;
		case 'v':	version(); return argc;
		case 'h':	(void)help(0x0); return argc;

		case ':':	return help("missing argument to \"%s\"", argv[optind - 1]);
		case '?':	return help("invalid option \"%s\"", argv[optind - 1]);
		default:	return help("unknown error");
		}
	}

	if(opts.src_dirs.size == 0){
		if(parse_dirs(DEFAULT_SRC_DIR, &opts.src_dirs) != 0)
			return -1;
	}

	/* opt checks */
	if(dirs_verify(&opts.src_dirs, false) != 0 || dirs_verify(&opts.excludes, true) != 0)
		return help("invalid configuartion");

	if(cov_thresholds_verify() != 0)
		return help("invalid coverage threshold configuration, valid range [0, 100]");

	return optind;
}

void opts_cleanup(void){
	char *e;


	vector_for_each(&opts.src_dirs, e)
		free(e);

	vector_destroy(&opts.src_dirs);

	vector_for_each(&opts.excludes, e)
		free(e);

	vector_destroy(&opts.excludes);
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
		"usage: %s [options] {<gcov-file> | <directory>}\n"
		"\n"
		"    Print function, line and branch coverage information for the given gcov-files\n"
		"    and all gcov-files contained in the given directories.\n"
		"\n"
		"Options:\n"
		"    %-35.35s    %s\n"
		"\n"
		"    %-35.35s    %s\n"
		"    %-35.35s    %s\n"
		"    %-35.35s    %s\n"
		"\n"
		"    %-35.35s    %s\n"
		"    %-35.35s    %s\n"
		"    %-35.35s    %s\n"
		"    %-35.35s    %s\n"
		"    %-35.35s    %s\n"
		"    %-35.35s    %s\n"
		"    %-35.35s    %s\n"
		"\n"
		"    %-35.35s    %s\n"
		"    %-35.35s    %s\n"
		,
		PROGNAME,
		"-r, --rc=<rc-file>", "use <rc-file> as base configuration " DEFAULT(DEFAULT_RC_FILE),
		"-f, --func-thr=<red,yellow>", "function coverage thresholds [%] " DEFAULT_THR(),
		"-l, --line-thr=<red,yellow>", "line coverage thresholds [%] " DEFAULT_THR(),
		"-b, --branch-thr=<red,yellow>", "branch coverage thresholds [%] " DEFAULT_THR(),
		"-u, --uncovered", "list source files (.c, .cc) without coverage data " DEFAULT(DEFAULT_LIST_UNCOVERED),
		"-s, --source={<dir>:}", "list of directories to search for uncovered files " DEFAULT(DEFAULT_SRC_DIR),
		"-e, --exclude={[<dir>|<file>]:}", "list of files and directories to exclude from coverage analysis",
		"", "and to ignore when checking for uncovered files",
		"-x, --excluded", "list files and directories that are currently excluded",
		"-n, --no-recursion", "do not recurse into sub-directories " DEFAULT(DEFAULT_RECURSIVE),
		"-c, --no-colour", "disable coloured output " DEFAULT(DEFAULT_NOCOLOUR),
		"-v, --version", "print version information",
		"-h, --help", "print this help message"
	);

	return (err == 0x0) ? 0 : -1;
}

static void version(void){
	printf(PROGNAME" version info:\n%s\n", VERSION);
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

static int parse_dirs(char *cfg, vector_t *tgt){
	char *e;


	for(e=cfg; ; e++){
		if(*e == SRC_DIR_SEPARATOR || *e == 0){
			if(vector_add(tgt, stralloc(cfg, e - cfg)) != 0)
				return -1;

			if(*e == 0)
				break;

			cfg = e + 1;
		}
	}

	return 0;
}

static int dirs_verify(vector_t *files, bool allow_file){
	char *file;


	vector_for_each(files, file){
		if(!(file_type(file) == F_DIR || (allow_file && file_type(file) == F_FILE))){
			fprintf(stderr, "\"%s\" is no %sdirectory\n", file, (allow_file ? "file or " : ""));
			return -1;
		}
	}

	return 0;
}
