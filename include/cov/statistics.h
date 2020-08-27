/**
 * Copyright (C) 2020 Jan Nowotsch
 * Author Jan Nowotsch	<jan.nowotsch@gmail.com>
 *
 * Released under the terms of the GNU GPL v2.0
 */



#ifndef STATISTICS_H
#define STATISTICS_H


#include <sys/types.h>
#include <covdata.h>


/* prototypes */
void stats_print(file_cov_t *cov);
void stats_uncovered(file_cov_t *cov);

int stats_thresholds_verify(void);
int stats_thresholds_apply(file_cov_t *cov);


#endif // STATISTICS_H
