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
	unsigned size;
}* elements;

elements seg;
unsigned seg_size;
unsigned seg_capacity;
uint32_t* stack;
unsigned s_index;
unsigned s_capacity;

static void setup_element(unsigned length, unsigned index);
static void expand();
static uint32_t pop();
static void push(uint32_t id);
static void s_expand();

uint32_t map(unsigned length)
{
	if(seg_size >= seg_capacity){
		expand();
	}
	seg_size++;
	uint32_t id = pop();
	if(id != 0){
		setup_element(length, id);
		return id;
	} else {
		setup_element(length, seg_size -1);
		return seg_size -1;
	}
}

void unmap(uint32_t id)
{
//	assert(id != 0);
	if(seg[id].array != NULL){
		free(seg[id].array);
		seg[id].array = NULL;
		push(id);
	}
}
uint32_t pop()
{
	if(s_index == 0){
		return 0;
	} 

	s_index--;
	return stack[s_index];
}

void push(uint32_t value)
{
	if(s_index >= s_capacity){
		s_expand();
	}
	stack[s_index] = value;
	s_index++;
}

void setup_memory(unsigned length)
{
	seg = malloc(length * sizeof(struct elements));
	seg_size = 1;
	seg_capacity = 1;
	setup_element(length, 0);
	stack = malloc(sizeof(uint32_t));
	s_capacity = 1;
	s_index = 0;
	
}
void put(uint32_t m, uint32_t n, uint32_t value)
{
	seg[m].array[n] = value;
}

uint32_t get(uint32_t m, uint32_t n)
{
	return seg[m].array[n];
}

uint32_t segment_size(uint32_t id)
{
	return seg[id].size;
}
uint32_t next_instruction(uint32_t counter)
{
		return seg[0].array[counter];
}

void free_memory()
{

	for(unsigned i = 1; i < seg_size - 1; i++){
			if (seg[i].array != NULL && seg[i].array != seg[0].array) {
				free(seg[i].array);
				seg[i].array = NULL;
			}
	}
	free(seg[0].array);
	free(seg);
	free(stack);
}
 


void put_program(uint32_t to, uint32_t from)
{
	free(seg[to].array);
	seg[to].array = seg[from].array;
	seg[to].size = seg[from].size;
}
void setup_element(unsigned length, unsigned index)
{
	uint32_t *array = calloc(length, sizeof(uint32_t));
	seg[index].size = length;
	seg[index].array = array;
}

void expand()
{
	seg_capacity *= 2;
	seg = realloc(seg, seg_capacity * sizeof(struct elements));
	for(unsigned i = seg_size; i < seg_capacity; i++){
		seg[i].array = NULL;
	}
//	assert(seg != NULL);
}

void s_expand()
{
	s_capacity *= 2;
	stack = realloc(stack, s_capacity * sizeof(uint32_t));
//	assert(stack != NULL);
}