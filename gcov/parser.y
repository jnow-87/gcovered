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
	#include <gcov/lexer.lex.h>
	#include <string.h>
	#include <errno.h>
	#include <escape.h>
	#include <covdata.h>


	/* macros */
	// extended error messages
	#define YYERROR_VERBOSE

	/* local/static variables */
	static FILE *fp = 0x0;


	/* prototypes */
	static void cleanup(void);
	static int gcoverror(char const *file, file_data_t *covdata, char const *s);
	static char *stralloc(char *s, size_t len);
%}

/* parse paramters */
%parse-param { char const *file }
%parse-param { file_data_t *covdata }

/* init code */
%initial-action{
	/* reset coverage data */
	cov_file_init(covdata);

	/* open config file */
	fp = fopen(file, "r");

	if(fp == 0){
		fprintf(stderr, "error reading config file \"%s\" -- %s\n", file, strerror(errno));
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
	  | error					{ cleanup(); $$.line = 0; YYABORT; }
	  | start line				{ $$ = $1; }
	  ;

line :	FUNC_COV				{ $$ = $1; cov_data_update(&covdata->functions, $1.cnt); }
	 |	LINE_COV				{ $$ = $1; cov_data_update(&covdata->lines, $1.cnt); }
	 |	BRANCH_COV				{ $$ = $1; cov_data_update(&covdata->branches, $1.cnt); }
	 |	FILE_NAME				{ covdata->name = stralloc($1.s, $1.len); }
	 ;


%%


static void cleanup(void){
	gcovlex_destroy();
	fclose(fp);
}

static int gcoverror(char const *file, file_data_t *covdata, char const *s){
	fprintf(stderr, FG_VIOLETT "%s" RESET_ATTR ":" FG_GREEN "%d:%d" RESET_ATTR " token \"%s\" -- %s\n",
		file,
		gcovlloc.first_line,
		gcovlloc.first_column,
		gcovtext,
		s
	);

	return 0;
}

static char *stralloc(char *_s, size_t len){
	char *s;


	s = malloc(len + 1);

	if(s != 0x0)
		strcpy(s, _s);

	return s;
}
