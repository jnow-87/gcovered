/**
 * Copyright (C) 2020 Jan Nowotsch
 * Author Jan Nowotsch	<jan.nowotsch@gmail.com>
 *
 * Released under the terms of the GNU GPL v2.0
 */



#ifndef FILE_H
#define FILE_H


#include <stdarg.h>
#include <stdio.h>


/* types */
typedef enum{
	F_ERROR = -1,
	F_UNKNOWN = 0,
	F_DIR,
	F_FILE,
} file_type_t;


/* prototypes */
file_type_t file_type(char const *file);
char const *file_ext(char const *file);

int dir_apply(char const *root, char const **exts, int (*op)(char const *file, va_list args), ...);
int dir_vapply(char const *root, char const **exts, int (*op)(char const *file, va_list args), va_list op_args);


#endif // FILE_H
