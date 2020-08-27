/**
 * Copyright (C) 2020 Jan Nowotsch
 * Author Jan Nowotsch	<jan.nowotsch@gmail.com>
 *
 * Released under the terms of the GNU GPL v2.0
 */



#ifndef COVDATA_H
#define COVDATA_H


/* types */
typedef struct{
	unsigned int total,
				 covered;
} cov_data_t;

typedef struct file_cov_t{
	char const *name;
	cov_data_t functions,
			   lines,
			   branches;

	struct file_cov_t *prev,
					  *next;
} file_cov_t;


/* prototypes */
void cov_init(file_cov_t *data);

void cov_update(cov_data_t *data, int cnt);
void cov_add(cov_data_t *lhs, cov_data_t *rhs);


#endif // COVDATA_H
