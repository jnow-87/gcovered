/**
 * Copyright (C) 2020 Jan Nowotsch
 * Author Jan Nowotsch	<jan.nowotsch@gmail.com>
 *
 * Released under the terms of the GNU GPL v2.0
 */



#include <stdlib.h>
#include <string.h>
#include <sys/types.h>


/* global functions */
size_t strcnt(char const *s, char c){
	size_t n;


	for(n=0; *s; s++){
		if(*s == c)
			n++;
	}

	return n;
}

char *stralloc(char *_s, size_t len){
	char *s;


	s = malloc(len + 1);

	if(s != 0x0){
		strncpy(s, _s, len);
		s[len] = 0;
	}

	return s;
}
