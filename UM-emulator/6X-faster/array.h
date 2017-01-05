#include <stdint.h>
#include <stdlib.h>

typedef struct elements{
	uint32_t* array;
	int size;
}* elements;

extern elements a_new(unsigned length);
extern uint32_t a_length();
extern elements a_get(uint32_t index);
extern void a_free();
extern elements expand();