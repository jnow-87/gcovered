/**
 * Copyright (C) 2020 Jan Nowotsch
 * Author Jan Nowotsch	<jan.nowotsch@gmail.com>
 *
 * Released under the terms of the GNU GPL v2.0
 */



#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <escape.h>
#include <covdata.h>
#include <file.h>
#include <gcov/parser.tab.h>


/* local/static prototypes */
static int gcov_process(char const *file);
static void stat_header(void);
static void stat_line(file_data_t *covdata);
static float per(cov_data_t *data);


/* static variables */
static file_data_t total;


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


/* local functions */
static int gcov_process(char const *file){
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

static void stat_header(void){
	printf("%20.20s %25.25s %25.25s %25.25s\n", "file", "function", "line", "branch");
}

static void stat_line(file_data_t *covdata){
	size_t i;
	char line[26];
	cov_data_t *data[] = {
		&covdata->functions,
		&covdata->lines,
		&covdata->branches,
	};


	printf("%20.20s", covdata->name);

	for(i=0; i<sizeof(data)/sizeof(data[0]); i++){
		if(data[i]->total != 0)
			snprintf(line, sizeof(line) - 1, "%u/%u (%.2f\%)", data[i]->covered, data[i]->total, per(data[i]));
		else
			snprintf(line, sizeof(line) - 1, "none");

		printf(" %25.25s", line);
	}

	printf("\n");
}

static float per(cov_data_t *data){
	if(data->total == 0.0)
		return 0.0;
	return ((float)data->covered * 100) / data->total;
}
