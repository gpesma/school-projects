#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include "segment.h"
#include <seq.h>
#include "fmt.h"
#include "bitpack.h"
#include "assert.h"


unsigned seq_size;
unsigned seq_capacity;
unsigned queue_size;
unsigned queue_capacity;

typedef struct elements{
	uint32_t* array;
	int size;
}* elements;

static uint32_t queue_status();
static void init_array(uint32_t* array, int length);
static elements setup_element(int length);
static elements Seq_new(uint32_t length);

uint32_t map(int length)
{
	elements el = setup_element(length);
	init_array(el->array, el->size);
	assert(el->array != NULL);
	uint32_t id = queue_status();
	if(id != 0){
			Seq_put(seq, id, el);
			return id;
	} else {
		Seq_addhi(seq, el);
		return Seq_length(seq) -1;
	}
}

void init_array(uint32_t *array, int length)
{
	for(int i = 0; i < length ; i++){
		array[i] = 0;
	}
}
void unmap(uint32_t id)
{
	assert(id != 0);
	elements el = (elements) Seq_get(seq, id);
	assert(el->array != NULL);
	free(el->array);
	free(el);
	uint32_t *old_id = malloc(sizeof(uint32_t));
	*old_id = id;
	Seq_addhi(queue, old_id);
	Seq_put(seq, id, NULL);
}


uint32_t queue_status()
{
	if((int) Seq_length(queue)  == 0){
		return 0;
	} else {
		uint32_t* id = (uint32_t*) Seq_remlo(queue);
		uint32_t return_id = *id;
		free(id);
		return return_id;
	}
}


void setup_memory(int length)
{
	// dont use el
	seq = Seq_new(0);
	elements el = setup_element(length);
	Seq_addhi(seq, el);
	queue = Seq_new(0);
}
void put(uint32_t m, uint32_t n, uint32_t value)
{
	assert(m < (uint32_t) Seq_le
ngth(seq));
	elements el = (elements) (Seq_get(seq, m));
	assert(n < (uint32_t) el->size);
	el->array[n] = value;
}

uint32_t get(uint32_t m, uint32_t n)
{
	if((uint32_t) m > (uint32_t) Seq_length(seq)){
		printf("exit %d\n", (int) m);
	}
	assert(m < (uint32_t) Seq_length(seq));
	elements el = Seq_get(seq, m);
	assert(n < (uint32_t) el->size);
	return el->array[n];
}

uint32_t segment_size(uint32_t id)
{
	elements el = (elements) Seq_get(seq,id);
	return (uint32_t) el->size;
}
uint32_t next_instruction(uint32_t *counter)
{
	elements el = (elements) Seq_get(seq, 0);
	*counter = *counter + 1;
	return el->array[(*counter)-1];
}
void free_memory()
{
	elements el;
	int size = Seq_length(seq);
	for(int i = 0; i < size; i++){
			el = (elements) Seq_remhi(seq);
			free(el->array);
			free(el);
	}
	size = Seq_length(queue);
	for(int i = 0; i < size; i++){
		free(Seq_remhi(queue));
	}
	Seq_free(&seq);
	Seq_free(&queue);
}



void put_program(uint32_t to, uint32_t from)
{
	assert(from < (uint32_t) Seq_length(seq));
	elements el = (elements) Seq_get(seq, to);
	assert(el->array != NULL);
	free(el->array);
	free(el);
	el = (elements) Seq_get(seq, from);
//	Seq_getup_element(el->size);
	Seq_put(seq, to, el);
}
elements setup_element(int length)
{
	uint32_t *array = malloc(length*sizeof(uint32_t));
	elements el = malloc(sizeof(struct elements));
	el->size = length;
	el->array = array;
	return el;
}

