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


/* global variables */
opt_t opts = {
	.thr_func = { .red = DEFAULT_THR_RED, .yellow = DEFAULT_THR_YELLOW },
	.thr_lines = { .red = DEFAULT_THR_RED, .yellow = DEFAULT_THR_YELLOW },
	.thr_branches = { .red = DEFAULT_THR_RED, .yellow = DEFAULT_THR_YELLOW },
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
	while((opt = getopt_long(argc, argv, ":f:l:b:nuch", long_opt, &long_optind)) != -1){
		switch(opt){
		case 'f':	if(parse_thr(optarg, &opts.thr_func) != 0) return -1; break;
		case 'l':	if(parse_thr(optarg, &opts.thr_lines) != 0) return -1; break;
		case 'b':	if(parse_thr(optarg, &opts.thr_branches) != 0) return -1; break;
		case 'n':	opts.recursive = false; break;
		case 'u':	opts.list_uncovered = true; break;
		case 'c':	opts.colour = false; break;
		case 'h':	(void)help(0x0); return argc;

		case ':':	return help("missing argument to \"%s\"", argv[optind - 1]);
		case '?':	return help("invalid option \"%s\"", argv[optind - 1]);
		default:	return help("unknown error");
		}
	}

	/* opt checks */
	if(stats_thresholds_verify() != 0)
		return help("invalid coverage threshold configuration, valid range [0, 100]");

	return optind;
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
		"\n"
		"%35.35s    %s\n"
		,
		"-f, --func-thr=<red,yellow>", "function coverage thresholds [%] " DEFAULT_THR(),
		"-l, --line-thr=<red,yellow>", "line coverage thresholds [%] " DEFAULT_THR(),
		"-b, --branch-thr=<red,yellow>", "branch coverage thresholds [%] " DEFAULT_THR(),
		"-n, --no-recursion", "do not recurse into sub-directories " DEFAULT(DEFAULT_RECURSIVE),
		"-u, --uncovered", "list source files (.c, .cc) without coverage data " DEFAULT(DEFAULT_LIST_UNCOVERED),
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
