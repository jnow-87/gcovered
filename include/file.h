/**
 * Copyright (C) 2020 Jan Nowotsch
 * Author Jan Nowotsch	<jan.nowotsch@gmail.com>
 *
 * Released under the terms of the GNU GPL v2.0
 */



#ifndef FILE_H
#define FILE_H


#include <stdio.h>


/* types */
typedef enum{
	F_ERROR = -1,
	F_UNKNOWN = 0,
	F_DIR,
	F_GCOV,
} file_type_t;

/* prototypes */
file_type_t file_type(char const *file);

int dir_process(char const *root, int (*op)(char const *file));


#endif // FILE_H
