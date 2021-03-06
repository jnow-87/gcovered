/**
 * Copyright (C) 2020 Jan Nowotsch
 * Author Jan Nowotsch	<jan.nowotsch@gmail.com>
 *
 * Released under the terms of the GNU GPL v2.0
 */



#include <cov/cov.h>
#include <cov/gcov.h>
#include <opt.h>


/* global functions */
int main(int argc, char **argv){
	int r;
	int optind;
	file_cov_t *cov;


	/* command line processing */
	optind = opt_parse(argc, argv);

	if(optind < 0){
		opts_cleanup();
		return -1;
	}
	else if(argc - optind == 0)
		return 0;

	/* parse gcov files */
	cov = gcov_parse((char const**)(argv + optind), argc - optind);

	if(cov == 0x0)
		return -1;

	r = cov_thresholds_apply(cov);

	/* print coverage statistics */
	cov_stats_print(cov);

	/* check for uncovered files */
	if(opts.list_uncovered)
		cov_uncovered(cov);

	/* list excluded */
	if(opts.list_excluded)
		cov_excluded();

	/* cleanup */
	gcov_cleanup(cov);
	opts_cleanup();

	return r;
}
