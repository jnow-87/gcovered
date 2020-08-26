/**
 * Copyright (C) 2020 Jan Nowotsch
 * Author Jan Nowotsch	<jan.nowotsch@gmail.com>
 *
 * Released under the terms of the GNU GPL v2.0
 */



#include <stdio.h>
#include <sys/types.h>
#include <list.h>
#include <file.h>
#include <opt.h>
#include <covdata.h>


/* local/static prototypes */
static void cov_header(void);
static void cov_line(file_cov_t *cov);
static void cov_total(file_cov_t *cov, file_cov_t *total);

static int uncovered(char const *file, va_list args);
static void uncovered_header(void);
static void uncovered_line(char const *file);

static double per(cov_data_t *data);


/* global functions */
void stats_print(file_cov_t *cov){
	file_cov_t total;
	file_cov_t *c;


	cov_header();

	list_for_each(cov, c)
		cov_line(c);

	cov_total(cov, &total);
	cov_line(&total);
}

void stats_uncovered(char const **files, size_t n, file_cov_t *cov){
	size_t i;


	uncovered_header();

	for(i=0; i<n; i++){
		if(file_type(files[i]) != F_DIR)
			continue;

		(void)dir_apply(files[i], (char const*[]){"c", "cc", 0x0}, uncovered, files[i], cov);
	}
}

bool stats_check_thresholds(file_cov_t *cov){
	if(per(&cov->functions) < opts.thr_func
	|| per(&cov->lines) < opts.thr_lines
	|| per(&cov->branches) < opts.thr_branches
	)
		return false;

	return true;
}


/* local functions */
static void cov_header(void){
	printf("Coverage Info\n");
	printf("%20.20s %25.25s %25.25s %25.25s\n", "file", "function", "cov_line", "branch");
}

static void cov_line(file_cov_t *cov){
	size_t i;
	char cov_line[26];
	cov_data_t *data[] = {
		&cov->functions,
		&cov->lines,
		&cov->branches,
	};


	printf("%20.20s", cov->name);

	for(i=0; i<sizeof(data)/sizeof(data[0]); i++){
		if(data[i]->total != 0)
			snprintf(cov_line, sizeof(cov_line) - 1, "%u/%u (%.2f\%)", data[i]->covered, data[i]->total, per(data[i]));
		else
			snprintf(cov_line, sizeof(cov_line) - 1, "none");

		printf(" %25.25s", cov_line);
	}

	printf("\n");
}

static void cov_total(file_cov_t *cov, file_cov_t *total){
	file_cov_t *c;


	cov_init(total);
	total->name = "= total";

	list_for_each(cov, c){
		cov_add(&total->functions, &c->functions);
		cov_add(&total->lines, &c->lines);
		cov_add(&total->branches, &c->branches);
	}
}

static int uncovered(char const *_file, va_list args){
	char const *dir,
			   *file;
	file_cov_t *cov,
				*c;


	dir = va_arg(args, char const*);
	cov = va_arg(args, file_cov_t*);

	file = _file + strlen(dir);

	list_for_each(cov, c){
		if(strcmp(c->name, file) == 0)
			return 0;
	}

	uncovered_line(file);

	return 0;
}

static void uncovered_header(void){
	printf("\nUncovered Files\n");
}

static void uncovered_line(char const *file){
	printf("    %s\n", file);
}

static double per(cov_data_t *data){
	if(data->total == 0.0)
		return 0.0;
	return ((float)data->covered * 100) / data->total;
}
