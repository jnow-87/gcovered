/**
 * Copyright (C) 2020 Jan Nowotsch
 * Author Jan Nowotsch	<jan.nowotsch@gmail.com>
 *
 * Released under the terms of the GNU GPL v2.0
 */



#include <stdio.h>
#include <covdata.h>


/* local/static prototypes */
static float per(cov_data_t *data);


/* global functions */
void stat_header(void){
	printf("%20.20s %25.25s %25.25s %25.25s\n", "file", "function", "line", "branch");
}

void stat_line(file_data_t *covdata){
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


/* local functions */
static float per(cov_data_t *data){
	if(data->total == 0.0)
		return 0.0;
	return ((float)data->covered * 100) / data->total;
}
