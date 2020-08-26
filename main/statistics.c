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
#include <escape.h>
#include <opt.h>
#include <covdata.h>


/* local/static prototypes */
static void cov_header(void);
static void cov_separator(void);
static void cov_line(file_cov_t *cov, char const *file_color);
static void cov_total(file_cov_t *cov, file_cov_t *total);

static double cov_per(cov_data_t *data);

static int uncovered(char const *file, va_list args);
static void uncovered_header(void);
static void uncovered_line(char const *file);


/* global functions */
void stats_print(file_cov_t *cov){
	file_cov_t total;
	file_cov_t *c;


	cov_header();

	list_for_each(cov, c)
		cov_line(c, "");

	cov_separator();
	cov_total(cov, &total);
	cov_line(&total, FG_VIOLETT);
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
	if(cov_per(&cov->functions) < opts.thr_func.red
	|| cov_per(&cov->lines) < opts.thr_lines.red
	|| cov_per(&cov->branches) < opts.thr_branches.red
	)
		return false;

	return true;
}


/* local functions */
static void cov_header(void){
	printf(BOLD "Coverage Info" RESET_ATTR "\n");
	printf(UNDERLINE BOLD "%30.30s %25.25s %25.25s %25.25s" RESET_ATTR "\n", "file", "function", "cov_line", "branch");
}

static void cov_separator(void){
	printf(UNDERLINE BOLD "%108.108s" RESET_ATTR "\n", "");
}

static void cov_line(file_cov_t *cov, char const *file_color){
	size_t i;
	char line[26];
	char const *color;
	double per;
	cov_data_t *data[] = {
		&cov->functions,
		&cov->lines,
		&cov->branches,
	};
	threshold_t thr[] = {
		opts.thr_func,
		opts.thr_lines,
		opts.thr_branches,
	};


	printf(file_color);
	printf("%30.30s", cov->name);
	printf(RESET_ATTR);

	for(i=0; i<sizeof(data)/sizeof(data[0]); i++){
		if(data[i]->total != 0){
			per = cov_per(data[i]);

			if(per < thr[i].red)			color = FG_RED;
			else if(per < thr[i].yellow)	color = FG_YELLOW;
			else							color = FG_GREEN;

			snprintf(line, sizeof(line) - 1, "%u/%u", data[i]->covered, data[i]->total);
			printf(" %17.17s", line);
			printf("%s%7.2f" RESET_ATTR "%%", color, per);
		}
		else
			printf(" %25.25s", "none");
	}

	printf("\n");
}

static void cov_total(file_cov_t *cov, file_cov_t *total){
	file_cov_t *c;


	cov_init(total);
	total->name = "total";

	list_for_each(cov, c){
		cov_add(&total->functions, &c->functions);
		cov_add(&total->lines, &c->lines);
		cov_add(&total->branches, &c->branches);
	}
}

static double cov_per(cov_data_t *data){
	if(data->total == 0.0)
		return 0.0;
	return ((float)data->covered * 100) / data->total;
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
	printf("\n" BOLD "Uncovered Files" RESET_ATTR "\n");
}

static void uncovered_line(char const *file){
	printf("    %s\n", file);
}
