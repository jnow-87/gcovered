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


/* macros */
#define STRINGIFY(v)		#v
#define STRINGIFY_VAL(v)	STRINGIFY(v)


/* local/static prototypes */
static int help(char const *prog_name, char const *err, ...);


/* global variables */
opt_t opts = {
	.recursive = true,
};


/* global functions */
int opt_parse(int argc, char **argv){
	int opt;
	int long_optind;
	struct option const long_opt[] = {
		{ .name = "no-recursion",	.has_arg = no_argument,			.flag = 0,	.val = 'n' },
		{ .name = "help",			.has_arg = no_argument,			.flag = 0,	.val = 'h' },
		{ 0, 0, 0, 0}
	};


	/* parse arguments */
	while((opt = getopt_long(argc, argv, ":nh", long_opt, &long_optind)) != -1){
		switch(opt){
		case 'n':	opts.recursive = false; break;
		case 'h':	(void)help(argv[0], 0x0); return argc;

		case ':':	return help(argv[0], "missing argument to \"%s\"", argv[optind - 1]);
		case '?':	return help(argv[0], "invalid option \"%s\"", argv[optind - 1]);
		default:	return help(argv[0], "unknown error");
		}
	}

	return optind;
}


/* local functions */
static int help(char const * prog_name, char const *err, ...){
	va_list lst;


	if(err != 0x0 && *err != 0){
		va_start(lst, err);

		printf(FG_RED "error" RESET_ATTR ": ");
		vprintf(err, lst);
		printf("\n\n");

		va_end(lst);
	}

	printf(
		"usage: %s [options] {<gcov-file | directory>}\n"
		"\n"
		"    Print function, line and branch coverage information for the given gcov-files\n"
		"    and all gcov-files contained in the given directories.\n"
		"\n"
		"Options:\n"
		"%30.30s    %s\n"
		"\n"
		"%30.30s    %s\n"
		,
		prog_name,
		"-n, --no-recursion", "do not recurse into sub-directories (default: false)",
		"-h, --help", "print this help message"
	);

	return (err == 0x0) ? 0 : -1;
}
