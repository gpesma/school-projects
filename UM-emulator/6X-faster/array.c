#include <stdlib.h>
#include <stdint.h>
#include "array.h"
#include "assert.h"
#include <stdio.h>


unsigned capacity;
unsigned size;

elements seg;

elements a_new(unsigned length)
{
	/*if(length ==0){
		return NULL;
	}*/
	seg = malloc(length* sizeof(struct elements));
	size = 0;
	capacity = length; 
	return seg;
}

uint32_t a_length()
{
	return size;
}

elements a_get(uint32_t index)
{
	assert(index <= size);
	//printf("a_get %u, %p, %p", index, (void*) seg, (void*) &seg[index]);
	return &seg[index];
}


void a_free()
{
	elements el;
	uint32_t *temp;
	for(unsigned i = 0; i < size; i++){
			el = &seg[i];
			temp = el->array;
			free(temp);
			free(el);
	}
}

elements expand()
{
	capacity *= 2;
	elements el = malloc(sizeof(struct elements) * capacity);
	for(unsigned i = 0; i < size; i++){
		el[i] = seg[i];
	}
	free(seg);
	return seg;
}