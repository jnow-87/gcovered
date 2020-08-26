/**
 * Copyright (C) 2020 Jan Nowotsch
 * Author Jan Nowotsch	<jan.nowotsch@gmail.com>
 *
 * Released under the terms of the GNU GPL v2.0
 */



#include <stdlib.h>
#include <stdarg.h>
#include <sys/types.h>
#include <list.h>
#include <file.h>
#include <covdata.h>
#include <gcov/parser.tab.h>
#include <gcov.h>


/* local/static prototypes */
static int process(char const *file, ...);
static int vprocess(char const *file, va_list args);


/* global functions */
file_cov_t *gcov_parse(char const **files, size_t n){
	int r;
	size_t i;
	file_cov_t *cov;


	r = 0;
	cov = 0x0;

	for(i=0; r==0 && i<n; i++){
		switch(file_type(files[i])){
		case F_DIR:		r = dir_apply(files[i], (char const*[]){"gcov", 0x0}, vprocess, &cov); break;
		case F_FILE:	r = process(files[i], &cov); break;
		default:		r = -1; break;
		}
	}

	if(r == 0)
		return cov;

	gcov_cleanup(cov);

	return 0x0;
}

void gcov_cleanup(file_cov_t *cov){
	file_cov_t *c;


	list_for_each(cov, c){
		free((void*)c->name);
		free(c);
	}
}


/* local functions */
static int process(char const *file, ...){
	int r;
	va_list lst;


	va_start(lst, file);
	r = vprocess(file, lst);
	va_end(lst);

	return r;
}

static int vprocess(char const *file, va_list args){
	file_cov_t cov;
	file_cov_t *p;
	file_cov_t **files;


	files = va_arg(args, file_cov_t**);

	if(gcovparse(file, &cov) != 0)
		return -1;

	p = malloc(sizeof(file_cov_t));

	if(p == 0x0)
		return -1;

	memcpy(p, &cov, sizeof(file_cov_t));
	list_add_tail(files, p);

	return 0;
}
