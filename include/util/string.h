/**
 * Copyright (C) 2020 Jan Nowotsch
 * Author Jan Nowotsch	<jan.nowotsch@gmail.com>
 *
 * Released under the terms of the GNU GPL v2.0
 */



#ifndef STRING_H
#define STRING_H


#include <sys/types.h>


/* prototypes */
size_t strcnt(char const *s, char c);
char *stralloc(char *s, size_t len);


#endif // STRING_H
