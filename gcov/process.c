/**
 * Copyright (C) 2020 Jan Nowotsch
 * Author Jan Nowotsch	<jan.nowotsch@gmail.com>
 *
 * Released under the terms of the GNU GPL v2.0
 */



#include <stdlib.h>
#include <covdata.h>
#include <gcov/parser.tab.h>
#include <statistics.h>


/* external variables */
extern file_data_t total;


/* global functions */
int gcov_process(char const *file){
	file_data_t covdata;


	if(gcovparse(file, &covdata) != 0)
		return -1;

	stat_line(&covdata);

	cov_data_add(&total.functions, &covdata.functions);
	cov_data_add(&total.lines, &covdata.lines);
	cov_data_add(&total.branches, &covdata.branches);

	free((void*)covdata.name);

	return 0;
}
