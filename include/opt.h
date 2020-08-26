/**
 * Copyright (C) 2020 Jan Nowotsch
 * Author Jan Nowotsch	<jan.nowotsch@gmail.com>
 *
 * Released under the terms of the GNU GPL v2.0
 */



#ifndef OPT_H
#define OPT_H


#include <stdbool.h>


/* macros */
#define DEFAULT_THR_FUNCTION	100.0
#define DEFAULT_THR_LINES		100.0
#define DEFAULT_THR_BRANCHES	100.0


/* types */
typedef struct{
} opt_t;


/* prototypes */
int opt_parse(int argc, char **argv);


/* external variables */
extern opt_t opts;


#endif // OPT_H
