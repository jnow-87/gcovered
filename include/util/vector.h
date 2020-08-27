/**
 * Copyright (C) 2020 Jan Nowotsch
 * Author Jan Nowotsch	<jan.nowotsch@gmail.com>
 *
 * Released under the terms of the GNU GPL v2.0
 */



#ifndef VECTOR_H
#define VECTOR_H


#include <sys/types.h>


/* macros */
#define VECTOR_INIT() { \
	.data = 0x0, \
	.size = 0, \
	.capa = 0, \
}

#define vector_for_each(v, e) \
	for(size_t _i=0; _i<(v)->size && ((e=(v)->data[_i]) == ((v)->data[_i])); _i++)


/* types */
typedef struct{
	void **data;

	size_t size,
		   capa;
} vector_t;


/* prototypes */
int vector_init(vector_t *v, size_t capa);
void vector_destroy(vector_t *v);

int vector_add(vector_t *v, void *data);
int vector_merge(vector_t *tgt, vector_t *src);


#endif // VECTOR_H
