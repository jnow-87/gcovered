/**
 * Copyright (C) 2020 Jan Nowotsch
 * Author Jan Nowotsch	<jan.nowotsch@gmail.com>
 *
 * Released under the terms of the GNU GPL v2.0
 */



#ifndef ESCAPE_H
#define ESCAPE_H


#include <opt.h>


/* macros */
#define ATTR_WRAPPER(attr)	(opts.colour ? (attr) : "")

// foreground colours
#define FG_BLACK		ATTR_WRAPPER("\033[30m")
#define FG_RED			ATTR_WRAPPER("\033[31m")
#define FG_GREEN		ATTR_WRAPPER("\033[32m")
#define FG_YELLOW		ATTR_WRAPPER("\033[33m")
#define FG_BLUE			ATTR_WRAPPER("\033[34m")
#define FG_VIOLETT		ATTR_WRAPPER("\033[35m")
#define FG_KOBALT		ATTR_WRAPPER("\033[36m")
#define FG_WHITE		ATTR_WRAPPER("\033[37m")

// background colours
#define BG_BLACK		ATTR_WRAPPER("\033[40m")
#define BG_RED			ATTR_WRAPPER("\033[41m")
#define BG_GREEN		ATTR_WRAPPER("\033[42m")
#define BG_YELLOW		ATTR_WRAPPER("\033[43m")
#define BG_BLUE			ATTR_WRAPPER("\033[44m")
#define BG_VIOLETT		ATTR_WRAPPER("\033[45m")
#define BG_KOBALT		ATTR_WRAPPER("\033[46m")
#define BG_WHITE		ATTR_WRAPPER("\033[47m")

// cursor positioning
#define POS_SAFE		ATTR_WRAPPER("\033[s")
#define POS_RESTORE		ATTR_WRAPPER("\033[u")
#define POS_UP(n)		ATTR_WRAPPER("\033["#n"A")
#define POS_DOWN(n)		ATTR_WRAPPER("\033["#n"B")
#define POS_LEFT(n)		ATTR_WRAPPER("\033["#n"D")
#define POS_RIGHT(n)	ATTR_WRAPPER("\033["#n"C")

// controls
#define CLEAR			ATTR_WRAPPER("\033[2J")
#define CLEARLINE		ATTR_WRAPPER("\033[K")

#define BOLD			ATTR_WRAPPER("\033[1m")
#define UNDERLINE		ATTR_WRAPPER("\033[4m")
#define BLINK			ATTR_WRAPPER("\033[5m")
#define INVERSE			ATTR_WRAPPER("\033[7m")
#define INVISIBLE		ATTR_WRAPPER("\033[8m")

#define RESET_ATTR		ATTR_WRAPPER("\033[0m")


#endif // ESCAPE_H
