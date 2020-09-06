/**
 * Copyright (C) 2020 Jan Nowotsch
 * Author Jan Nowotsch	<jan.nowotsch@gmail.com>
 *
 * Released under the terms of the GNU GPL v2.0
 */



#ifndef OPT_H
#define OPT_H


#include <stdbool.h>
#include <util/vector.h>


/* macros */
#define DEFAULT_RC_FILE			".gcoveredrc"
#define DEFAULT_THR_RED			100.0
#define DEFAULT_THR_YELLOW		100.0
#define DEFAULT_SRC_DIR			"./"
#define DEFAULT_RECURSIVE		true
#define DEFAULT_LIST_UNCOVERED	false
#define DEFAULT_LIST_EXCLUDED	false
#define DEFAULT_NOCOLOUR		false

#define SRC_DIR_SEPARATOR		':'

#define PROGNAME				"gcovered"


/* types */
typedef struct{
	double red,
		   yellow;
} threshold_t;

typedef struct{
	char const *rc_file;

	threshold_t thr_func,
				thr_lines,
				thr_branches;

	vector_t src_dirs,
			 excludes;

	bool recursive,
		 list_uncovered,
		 list_excluded,
		 colour;
} opt_t;


/* prototypes */
int opt_parse(int argc, char **argv);
void opts_cleanup(void);


/* external variables */
extern opt_t opts;


#endif // OPT_H
