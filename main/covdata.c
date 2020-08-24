/**
 * Copyright (C) 2020 Jan Nowotsch
 * Author Jan Nowotsch	<jan.nowotsch@gmail.com>
 *
 * Released under the terms of the GNU GPL v2.0
 */



#include <string.h>
#include <covdata.h>


/* global functions */
void cov_file_init(file_data_t *data){
	memset(data, 0x0, sizeof(file_data_t));
}

void cov_data_init(cov_data_t *data){
	memset(data, 0x0, sizeof(cov_data_t));
}

void cov_data_update(cov_data_t *data, int cnt){
	data->total++;

	if(cnt > 0) \
		data->covered++;
}

void cov_data_add(cov_data_t *lhs, cov_data_t *rhs){
	lhs->total += rhs->total;
	lhs->covered += rhs->covered;
}
