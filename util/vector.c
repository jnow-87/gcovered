/**
 * Copyright (C) 2020 Jan Nowotsch
 * Author Jan Nowotsch	<jan.nowotsch@gmail.com>
 *
 * Released under the terms of the GNU GPL v2.0
 */



#include <stdlib.h>
#include <string.h>
#include <util/vector.h>


/* global functions */
int vector_init(vector_t *v, size_t capa){
	v->capa = 0;
	v->data = malloc(sizeof(void*) * capa);

	if(v->data == 0x0)
		return -1;

	v->size = 0;
	v->capa = capa;

	return 0;
}

void vector_destroy(vector_t *v){
	free(v->data);
	v->capa = 0;
}

int vector_add(vector_t *v, void *data){
	if(v->size == v->capa){
		v->capa *= 2;
		v->data = realloc(v->data, sizeof(void*) * v->capa);

		if(v->data == 0x0){
			v->capa = 0;
			return -1;
		}
	}

	v->data[v->size++] = data;

	return 0;
}

int vector_merge(vector_t *tgt, vector_t *src){
	if(tgt->capa < tgt->size + src->size){
		tgt->capa += src->size;
		tgt->data = realloc(tgt->data, sizeof(void*) * tgt->capa);

		if(tgt->data == 0x0){
			tgt->capa = 0;
			return -1;
		}
	}

	memcpy(tgt->data + tgt->size, src->data, sizeof(void*) * src->size);
	tgt->size += src->size;

	return 0;
}
