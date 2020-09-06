/**
 * Copyright (C) 2020 Jan Nowotsch
 * Author Jan Nowotsch	<jan.nowotsch@gmail.com>
 *
 * Released under the terms of the GNU GPL v2.0
 */



#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include <util/escape.h>
#include <util/list.h>
#include <util/file.h>
#include <cov/cov.h>


/* local/static prototypes */
static void cov_header(void);
static void cov_separator(void);
static void cov_line(file_cov_t *cov, char const *file_colour);
static void cov_total(file_cov_t *cov, file_cov_t *total);

static double cov_per(cov_data_t *data);

static int uncovered(char const *file, va_list args);
static void uncovered_header(void);
static void uncovered_line(char const *file);

static void excluded_header(void);
static void excluded_line(char const *file);


/* global functions */
void cov_init(file_cov_t *data){
	memset(data, 0x0, sizeof(file_cov_t));
}

void cov_update(cov_data_t *data, int cnt){
	data->total++;

	if(cnt > 0) \
		data->covered++;
}

void cov_add(cov_data_t *lhs, cov_data_t *rhs){
	lhs->total += rhs->total;
	lhs->covered += rhs->covered;
}

void cov_stats_print(file_cov_t *cov){
	file_cov_t total;
	file_cov_t *c;


	cov_header();

	list_for_each(cov, c)
		cov_line(c, "");

	cov_separator();
	cov_total(cov, &total);
	cov_line(&total, FG_VIOLETT);
}

void cov_uncovered(file_cov_t *cov){
	char const *e;


	uncovered_header();

	vector_for_each(&opts.src_dirs, e)
		(void)dir_apply(e, (char const*[]){"c", "cc", 0x0}, uncovered, e, cov);
}

void cov_excluded(void){
	char const *e;


	excluded_header();

	vector_for_each(&opts.excludes, e)
		excluded_line(e);
}

int cov_thresholds_verify(void){
	threshold_t **thr;
	threshold_t *thrs[] = {
		&opts.thr_func,
		&opts.thr_lines,
		&opts.thr_branches,
		0x0,
	};

	for(thr=thrs; *thr!=0x0; thr++){
		if((*thr)->red < 0.0 || (*thr)->red > 100.0
		|| (*thr)->yellow < 0.0 || (*thr)->yellow > 100.0)
			return -1;
	}

	return 0;
}

int cov_thresholds_apply(file_cov_t *cov){
	file_cov_t *c;


	list_for_each(cov, c){
		if(cov_per(&c->functions) < opts.thr_func.red
		|| cov_per(&c->lines) < opts.thr_lines.red
		|| cov_per(&c->branches) < opts.thr_branches.red
		)
			return -1;
	}

	return 0;
}


/* local functions */
static void cov_header(void){
	printf("%sCoverage Info%s\n", BOLD, RESET_ATTR);
	printf("  %s%s%30.30s %25.25s %25.25s %25.25s%s\n", UNDERLINE, BOLD, "file             ", "function     ", "line     ", "branch      ", RESET_ATTR);
}

static void cov_separator(void){
	printf("  %s%s%108.108s%s\n", UNDERLINE, BOLD, "", RESET_ATTR);
}

static void cov_line(file_cov_t *cov, char const *file_colour){
	size_t i;
	char line[26];
	char const *colour;
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


	printf("  %s", file_colour);
	printf("%30.30s", cov->name);
	printf(RESET_ATTR);

	for(i=0; i<sizeof(data)/sizeof(data[0]); i++){
		if(data[i]->total != 0){
			per = cov_per(data[i]);

			if(per < thr[i].red)			colour = FG_RED;
			else if(per < thr[i].yellow)	colour = FG_YELLOW;
			else							colour = FG_GREEN;

			snprintf(line, sizeof(line) - 1, "%u/%u", data[i]->covered, data[i]->total);
			printf(" %17.17s", line);
			printf("%s%7.2f%s%%", colour, per, RESET_ATTR);
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
		return 100.0;
	return ((float)data->covered * 100) / data->total;
}

static int uncovered(char const *_file, va_list args){
	char const *dir,
			   *file;
	char const *excl;
	file_cov_t *cov,
				*c;


	dir = va_arg(args, char const*);
	cov = va_arg(args, file_cov_t*);

	file = _file + strlen(dir);

	if(*file == '/')
		file++;

	vector_for_each(&opts.excludes, excl){
		if(strncmp(_file, excl, strlen(excl)) == 0
		|| strncmp(_file + 2, excl, strlen(excl)) == 0
		){
			return 0;
		}
	}

	list_for_each(cov, c){
		if(strcmp(c->name, file) == 0)
			return 0;
	}

	uncovered_line(file);

	return 0;
}

static void uncovered_header(void){
	printf("\n%sUncovered Files%s\n", BOLD, RESET_ATTR);
}

static void uncovered_line(char const *file){
	printf("    %s\n", file);
}

static void excluded_header(void){
	printf("\n%sExcluded Files/Directories%s\n", BOLD, RESET_ATTR);
}

static void excluded_line(char const *file){
	printf("    %s\n", file);
}
