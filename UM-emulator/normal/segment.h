/*
 * 
 * Interface of Virtual Memory interface
 *
 */


#include <stdint.h>
#include <stdlib.h>


extern uint32_t map (unsigned length);
extern uint32_t get(uint32_t m, uint32_t n);
extern void unmap(uint32_t id);
extern void put(uint32_t m, uint32_t n, uint32_t value);
extern uint32_t segment_size(uint32_t id);
extern uint32_t next_instruction(unsigned counter);
extern void free_memory();
extern void setup_memory();
extern void put_program(uint32_t to, uint32_t from);