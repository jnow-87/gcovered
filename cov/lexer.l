/**
 * Copyright (C) 2020 Jan Nowotsch
 * Author Jan Nowotsch	<jan.nowotsch@gmail.com>
 *
 * Released under the terms of the GNU GPL v2.0
 */



%option prefix="gcov"
%option noyywrap
%option nounput
%option noinput

%{
	#include <cov/cov.h>
	#include <cov/parser.tab.h>


	/* define YY_USER_ACTION, executed for every token, before executing its
	 * action code
	 */
	#define YY_USER_ACTION { \
		/* update location based on current token */ \
		gcovlloc.first_line = gcovlloc.last_line = gcov_line; \
		gcovlloc.first_column = gcov_col; \
		gcovlloc.last_column = gcov_col + gcovleng; \
		gcov_col += gcovleng; \
	}
%}

/* start states */
%x NONSRC_LINE
%x PREAMBLE_FILE
%x SRC_LINE
%x FUNC_LINE
%x BRANCH_LINE
%x IGNORE


%%


%{
	/* code that goes to the beginning of yylex() */
	static unsigned int gcov_col = 1;
	static unsigned int gcov_line = 1;
%}

	/* figure out start state */
[0-9]+\*?:						{ BEGIN SRC_LINE; gcovlval.cov.cnt = atoi(gcovtext); }
[#=$%]{5}:						{ BEGIN SRC_LINE; gcovlval.cov.cnt = -1; }
-:								{ BEGIN NONSRC_LINE; }
"function".*"called" 			{ BEGIN FUNC_LINE; }
"branch"						{ BEGIN BRANCH_LINE; }
.								{ }

	/* coverage and preamble data */
<SRC_LINE>[0-9]+:				{ BEGIN IGNORE; gcovlval.cov.line = atoi(gcovtext); return LINE_COV; }

<NONSRC_LINE>"Source:"			{ BEGIN PREAMBLE_FILE; }
<NONSRC_LINE>.					{ }
<PREAMBLE_FILE>.*				{ gcovlval.str.s = gcovtext; gcovlval.str.len = gcovleng; return FILE_NAME; }

<FUNC_LINE>[0-9]+				{ BEGIN IGNORE; gcovlval.cov.cnt = atoi(gcovtext); return FUNC_COV; }
<FUNC_LINE>.					{ }

<BRANCH_LINE>"taken 0%"			{ gcovlval.cov.cnt = 0; return BRANCH_COV; }
<BRANCH_LINE>"taken ".*"%"		{ gcovlval.cov.cnt = 1; return BRANCH_COV; }
<BRANCH_LINE>"never executed"	{ gcovlval.cov.cnt = 0; return BRANCH_COV; }
<BRANCH_LINE>.					{ }

	/* skip remainder of the line */
<IGNORE>.*						{ }

	/* general */
<*>\r?\n						{ BEGIN INITIAL; gcov_col = 1; ++gcov_line; }
<*>[ \t]						{ }
