/**
 * Copyright (C) 2020 Jan Nowotsch
 * Author Jan Nowotsch	<jan.nowotsch@gmail.com>
 *
 * Released under the terms of the GNU GPL v2.0
 */



%define api.prefix {rc}
%locations

/* header */
%{
	#include <sys/types.h>
	#include <stdbool.h>
	#include <string.h>
	#include <errno.h>
	#include <util/vector.h>
	#include <util/string.h>
	#include <util/escape.h>
	#include <rc/lexer.lex.h>
	#include <opt.h>


	/* macros */
	// extended error messages
	#define YYERROR_VERBOSE

	// error wrapper
	#define ABORT_ON_ERROR(expr){ \
		if((expr) != 0){ \
			rcerror(file, strerror(errno)); \
			YYABORT; \
		} \
	}

	/* static variables */
	static FILE *fp = 0x0;


	/* prototypes */
	static void cleanup(void);
	static int rcerror(char const *file, char const *s);
%}

/* parse paramters */
%parse-param { char const *file }

/* init code */
%initial-action{
	/* open config file */
	fp = fopen(file, "r");

	if(fp == 0){
		fprintf(stderr, "error reading rc file \"%s\" -- %s\n", file, strerror(errno));
		return 1;
	}

	/* start lexer */
	rcrestart(fp);
}

/* cleanup code */
%destructor{
	cleanup();
} start

/* parser union type */
%union{
	int i;
	float f;
	bool b;

	struct{
		char *s;
		unsigned int len;
	} str;

	vector_t dirs;
}

/* terminals */
// sections
%token SEC_THR
%token SEC_SRC
%token SEC_EXCL
%token SEC_GEN

// keys
%token KEY_FUNC
%token KEY_LINE
%token KEY_BRANCH
%token KEY_UNCOVERED
%token KEY_COLOUR
%token KEY_RECURSIVE

// values
%token <b> BOOL
%token <i> INT
%token <f> FLOAT
%token <str> STRING

/* non-terminals */
%type <i> start



%%


/* start */
start :	%empty										{ $$ = 0; }
	  | error										{ $$ = 1; YYABORT; }
	  | start section								{ $$ = 0; (void)$1; }
	  ;

section :	'[' SEC_THR ']'	sec-thr					{ }
		|	'[' SEC_SRC ']' sec-src					{ }
		|	'[' SEC_EXCL ']' sec-excl				{ }
		|	'[' SEC_GEN ']' sec-gen					{ }
		;

sec-thr :	%empty									{}
		|	sec-thr KEY_FUNC '=' FLOAT ',' FLOAT	{ opts.thr_func.red = $4; opts.thr_func.yellow = $6; }
		|	sec-thr KEY_LINE '=' FLOAT ',' FLOAT	{ opts.thr_lines.red = $4; opts.thr_lines.yellow = $6; }
		|	sec-thr KEY_BRANCH '=' FLOAT ',' FLOAT	{ opts.thr_branches.red = $4; opts.thr_branches.yellow = $6; }
		;

sec-src :	%empty									{ }
		|	sec-src STRING							{ ABORT_ON_ERROR(vector_add(&opts.src_dirs, stralloc($2.s, $2.len))); }
		;

sec-excl :	%empty									{ }
		 |	sec-excl STRING							{ ABORT_ON_ERROR(vector_add(&opts.excl_dirs, stralloc($2.s, $2.len))); }
		 ;

sec-gen :	%empty									{ }
		|	sec-gen KEY_UNCOVERED '=' BOOL			{ opts.list_uncovered = $4; }
		|	sec-gen KEY_COLOUR '=' BOOL				{ opts.colour = $4; }
		|	sec-gen KEY_RECURSIVE '=' BOOL			{ opts.recursive = $4; }
		;


%%


static void cleanup(void){
	rclex_destroy();
	fclose(fp);
}

static int rcerror(char const *file, char const *s){
	fprintf(stderr, "%s%s%s:%s%d:%d%s token \"%s\" -- %s\n",
		FG_VIOLETT,
		file,
		RESET_ATTR,
		FG_GREEN,
		rclloc.first_line,
		rclloc.first_column,
		RESET_ATTR,
		rctext,
		s
	);

	return 0;
}
