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
#include <gcov/parser.tab.h>


/* local/static prototypes */
static void stat_header(void);
static void stat_line(file_data_t *covdata);
static float per(cov_data_t *data);


/* global functions */
int main(int argc, char **argv){
	int i;
	int r;
	file_data_t covdata,
				total;


	/* init */
	cov_file_init(&total);
	total.name = "total";

	/* print header */
	stat_header();

	/* print file statistics */
	for(i=1; i<argc; i++){
		r = gcovparse(argv[i], &covdata);

		if(r == 0){
			stat_line(&covdata);

			cov_data_add(&total.functions, &covdata.functions);
			cov_data_add(&total.lines, &covdata.lines);
			cov_data_add(&total.branches, &covdata.branches);

			free((void*)covdata.name);
		}
		else
			printf("parser ret: %d\n", r);
	}

	/* print total */
	printf("\n");
	stat_line(&total);

	return 0;
}


/* local functions */
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
