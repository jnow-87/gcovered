/**
 * Copyright (C) 2020 Jan Nowotsch
 * Author Jan Nowotsch	<jan.nowotsch@gmail.com>
 *
 * Released under the terms of the GNU GPL v2.0
 */



#include <opt.h>
#include <covdata.h>
#include <gcov.h>
#include <statistics.h>


/* global functions */
int main(int argc, char **argv){
	int r;
	int optind;
	file_cov_t *cov;


	/* command line processing */
	optind = opt_parse(argc, argv);

	if(optind < 0)				return -1;
	else if(argc - optind == 0)	return 0;

	/* parse gcov files */
	cov = gcov_parse((char const**)(argv + optind), argc - optind);

	if(cov == 0x0)
		return -1;

	r = stats_check_thresholds(cov) ? 0 : -1;

	/* print coverage statistics */
	stats_print(cov);

	/* check for uncovered files */
	if(opts.list_uncovered)
		stats_uncovered((char const**)(argv + optind), argc - optind, cov);

	/* cleanup */
	gcov_cleanup(cov);

	return r;
}
