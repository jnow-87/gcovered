/**
 * Copyright (C) 2020 Jan Nowotsch
 * Author Jan Nowotsch	<jan.nowotsch@gmail.com>
 *
 * Released under the terms of the GNU GPL v2.0
 */



#include <stdio.h>
#include <covdata.h>
#include <statistics.h>
#include <file.h>
#include <gcov.h>


/* static variables */
file_data_t total;


/* global functions */
int main(int argc, char **argv){
	int i;
	int r;


	/* init */
	cov_file_init(&total);
	total.name = "total";

	/* print header */
	stat_header();

	/* process input */
	r = 0;

	for(i=1; r==0 && i<argc; i++){
		switch(file_type(argv[i])){
		case F_DIR:
			r = dir_process(argv[i], gcov_process);
			break;

		case F_GCOV:
			r = gcov_process(argv[i]);
			break;

		default:
			printf("unsupported file %s\n", argv[i]);
			return -1;
		}
	}

	/* print total */
	printf("\n");
	stat_line(&total);

	return 0;
}
