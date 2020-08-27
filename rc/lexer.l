/**
 * Copyright (C) 2020 Jan Nowotsch
 * Author Jan Nowotsch	<jan.nowotsch@gmail.com>
 *
 * Released under the terms of the GNU GPL v2.0
 */



%option prefix="rc"
%option noyywrap

%{
	#include <stdbool.h>
	#include <util/vector.h>
	#include <rc/parser.tab.h>


	/* define YY_USER_ACTION, executed for every token, before executing its
	 * action code
	 */
	#define YY_USER_ACTION { \
		/* update location based on current token */ \
		rclloc.first_line = rclloc.last_line = rc_line; \
		rclloc.first_column = rc_col; \
		rclloc.last_column = rc_col + rcleng; \
		rc_col += rcleng; \
	}
%}


%%


%{
	/* code that goes to the beginning of yylex() */
	static unsigned int rc_col = 1;
	static unsigned int rc_line = 1;
%}

	/* sections */
"thresholds"					{ return SEC_THR; }
"source-dirs"					{ return SEC_SRC; }
"exclude-dirs"					{ return SEC_EXCL; }
"general"						{ return SEC_GEN; }

	/* keys */
"function"						{ return KEY_FUNC; }
"line"							{ return KEY_LINE; }
"branch"						{ return KEY_BRANCH; }
"uncovered"						{ return KEY_UNCOVERED; }
"colour"						{ return KEY_COLOUR; }
"recursive"						{ return KEY_RECURSIVE; }

	/* values */
"true"							{ rclval.b = true; return BOOL; }
"false"							{ rclval.b = false; return BOOL; }

\".+\"							{ rclval.str.s = rctext + 1; rclval.str.len = rcleng - 2; return STRING; }
[0-9]+							{ rclval.i = atoi(rctext); return INT; }
[0-9\.]+						{ rclval.f = atof(rctext); return FLOAT; }
[ \t]							{ }
\r?\n							{ rc_col = 1; ++rc_line; }
.								{ return rctext[0]; }
