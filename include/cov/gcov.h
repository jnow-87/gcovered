/**
 * Copyright (C) 2020 Jan Nowotsch
 * Author Jan Nowotsch	<jan.nowotsch@gmail.com>
 *
 * Released under the terms of the GNU GPL v2.0
 */



#ifndef GCOV_H
#define GCOV_H


#include <sys/types.h>
#include <cov/cov.h>


/* prototypes */
file_cov_t *gcov_parse(char const **files, size_t n);
void gcov_cleanup(file_cov_t *cov);


#endif // GCOV_H
