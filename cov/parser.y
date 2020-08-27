/**
 * Copyright (C) 2020 Jan Nowotsch
 * Author Jan Nowotsch	<jan.nowotsch@gmail.com>
 *
 * Released under the terms of the GNU GPL v2.0
 */



%define api.prefix {gcov}
%locations

/* header */
%{
	#include <sys/types.h>
	#include <string.h>
	#include <errno.h>
	#include <util/escape.h>
	#include <util/string.h>
	#include <cov/cov.h>
	#include <cov/lexer.lex.h>


	/* macros */
	// extended error messages
	#define YYERROR_VERBOSE

	/* static variables */
	static FILE *fp = 0x0;


	/* prototypes */
	static void cleanup(void);
	static int gcoverror(char const *file, file_cov_t *cov, char const *s);
%}

/* parse paramters */
%parse-param { char const *file }
%parse-param { file_cov_t *cov }

/* init code */
%initial-action{
	/* reset coverage data */
	cov_init(cov);

	/* open config file */
	fp = fopen(file, "r");

	if(fp == 0){
		fprintf(stderr, "error reading gcov-file \"%s\" -- %s\n", file, strerror(errno));
		return 1;
	}

	/* start lexer */
	gcovrestart(fp);
}

/* cleanup code */
%destructor{
	cleanup();
} start

/* parser union type */
%union{
	struct{
		char *s;
		unsigned int len;
	} str;

	struct{
		unsigned int line;
		int cnt;
	} cov;
}

/* terminals */
// specifier
%token <cov> FUNC_COV
%token <cov> LINE_COV
%token <cov> BRANCH_COV
%token <str> FILE_NAME

/* non-terminals */
%type <cov> start
%type <cov> line



%%


/* start */
start :	%empty					{ $$.line = 0; }
	  | error					{ $$.line = 0; YYABORT; }
	  | start line				{ $$ = $1; }
	  ;

line :	FUNC_COV				{ $$ = $1; cov_update(&cov->functions, $1.cnt); }
	 |	LINE_COV				{ $$ = $1; cov_update(&cov->lines, $1.cnt); }
	 |	BRANCH_COV				{ $$ = $1; cov_update(&cov->branches, $1.cnt); }
	 |	FILE_NAME				{ cov->name = stralloc($1.s, $1.len); }
	 ;


%%


static void cleanup(void){
	gcovlex_destroy();
	fclose(fp);
}

static int gcoverror(char const *file, file_cov_t *cov, char const *s){
	fprintf(stderr, "%s%s%s:%s%d:%d%s token \"%s\" -- %s\n",
		FG_VIOLETT,
		file,
		RESET_ATTR,
		FG_GREEN,
		gcovlloc.first_line,
		gcovlloc.first_column,
		RESET_ATTR,
		gcovtext,
		s
	);

	return 0;
}
