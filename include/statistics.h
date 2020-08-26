/**
 * Copyright (C) 2020 Jan Nowotsch
 * Author Jan Nowotsch	<jan.nowotsch@gmail.com>
 *
 * Released under the terms of the GNU GPL v2.0
 */



#ifndef STATISTICS_H
#define STATISTICS_H


#include <stdbool.h>
#include <sys/types.h>


/* prototypes */
void stats_print(file_cov_t *cov);
void stats_uncovered(char const **files, size_t n, file_cov_t *cov);
bool stats_check_thresholds(file_cov_t *cov);


#endif // STATISTICS_H
