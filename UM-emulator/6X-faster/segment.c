#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include "segment.h"
#include <seq.h>
#include "fmt.h"
#include "bitpack.h"
#include "assert.h"


typedef struct elements{
	uint32_t* array;
	int size;
}* elements;


//Seq_T seq;
//elements seg;
unsigned seg_capacity;
unsigned seg_size;
elements seg;
Seq_T queue;
/*
typedef struct elements{
	uint32_t* array;
	int size;
}* elements;*/

static uint32_t queue_status();
static void init_array(uint32_t* array, int length);
static void put_segment(int length, uint32_t index);
static void a_new(unsigned length);
static uint32_t a_length();
static void a_free();
static void expand();

uint32_t map(int length)
{
	uint32_t id = queue_status();
	if(id != 0){
			put_segment(length, id);
			init_array(seg[id].array, length);
			return id;
	} else {
		if(seg_size >= seg_capacity){
			expand();
		}
		put_segment(length, seg_size);
		init_array(seg[seg_size].array, length);
		seg_size++;
		return seg[seg_size-1].size;
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
	//printf("unmap %u", id);
	assert(id != 0);
	assert(seg[id].array != NULL);
	free(seg[id].array);
	seg[id].size = 0;
	uint32_t *old_id = malloc(sizeof(uint32_t));
	*old_id = id;
	Seq_addhi(queue, old_id);
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
	a_new(1);
	put_segment(length, 0);
	queue = Seq_new(0);
}
void put(uint32_t m, uint32_t n, uint32_t value)
{
	if(m > seg_size){
		printf("put error: %u, %u %u %u\n", m, n, seg_size, seg_capacity);
	}
	seg[m].array[n] = value;
}

uint32_t get(uint32_t m, uint32_t n)
{
	//printf("get:seg %u seg: %u\n", m, n);
	assert(m < a_length());
	assert(n < (uint32_t) seg[m].size);
	return seg[m].array[n];
}

uint32_t segment_size(uint32_t id)
{
	//printf("segment_size: %u\n",id);
	return (uint32_t) seg[id].size;
}
uint32_t next_instruction(uint32_t *counter)
{
	//printf("next_instruction: %u %u\n", *counter, seg[0].array[(*counter)]);
	*counter = *counter + 1;
	return seg[0].array[(*counter)-1];
}
void free_memory()
{
	int length = Seq_length(queue);
	a_free();
	for(int i = 0; i < length; i++){
		free(Seq_remhi(queue));
	}
	Seq_free(&queue);
}



void put_program(uint32_t to, uint32_t from)
{
	//printf("put_pprogram: %u %u\n", to, from);
	assert(from < (uint32_t) a_length());
	assert(seg[to].array != NULL);
	free(seg[0].array);
	seg[to].size = seg[from].size;
	uint32_t *array = malloc(seg[from].size*sizeof(uint32_t));
	seg[to].array = array;
	for(int i = 0; i < seg[from].size; i++){
		seg[to].array[i] = seg[from].array[i];
	}
}
void put_segment(int length, uint32_t index)
{
	//printf("put_segment: %u %d\n", index, length);
	//if(index > seg_size){
//expand();
	//}
	uint32_t *array = malloc(length * sizeof(uint32_t));
// 	printf("length and index %u, %d, %u\n", length, index, seg_size);
	seg[index].size = length;
	seg[index].array = array;

}
void a_new(unsigned length)
{
	/*if(length ==0){
		return NULL;
	}*/
	seg = malloc(length * sizeof(struct elements));
	seg_size = 1;
	seg_capacity = length; 
}
uint32_t a_length()
{
	return seg_size;
}
void a_free()
{
	for(unsigned i = 0; i < seg_size; i++){
			free(seg[i].array);
	}
	free(seg);
}

void expand()
{
	seg_capacity = seg_capacity * 2;
	elements el = malloc(sizeof(struct elements) * seg_capacity);
	for(unsigned i = 0; i < seg_size; i++){
		el[i] = seg[i];
	}
	free(seg);
	seg = el;
}
