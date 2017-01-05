#include <stdlib.h>
#include <stdint.h>
#include <sys/stat.h>
#include "bitpack.h"
#include <stdio.h>
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

uint32_t registers[8];
uint32_t counter;
uint32_t program_size;

#define FIRST 0
#define LOAD_VAL 13
#define halt 7

static inline uint8_t readbyte(FILE *fp)
{
        char c = fgetc(fp);
        return (uint8_t) c;
}

static inline void printbyte(uint8_t c)
{
        printf("%c",(char) c);
}
static inline uint32_t next_instruction(uint32_t counter)
{
        return seg[0].array[counter];
}

static inline uint32_t pop()
{
    if(s_index == 0){
        return 0;
    } 

    s_index--;
    return stack[s_index];
}
static inline void expand()
{
    seg_capacity *= 2;
    seg = realloc(seg, seg_capacity * sizeof(struct elements));
    for(unsigned i = seg_size; i < seg_capacity; i++){
        seg[i].array = NULL;
    }
}
static inline void s_expand()
{
    s_capacity *= 2;
    stack = realloc(stack, s_capacity * sizeof(uint32_t));
}
static inline void push(uint32_t value)
{
    if(s_index >= s_capacity){
        s_expand();
    }
    stack[s_index] = value;
    s_index++;
}



static inline void setup_element(unsigned length, unsigned index)
{
    uint32_t *array = calloc(length, sizeof(uint32_t));
    seg[index].size = length;
    seg[index].array = array;
}
static inline uint32_t map(unsigned length)
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

static inline void unmap(uint32_t id)
{
    if(seg[id].array != NULL){
        free(seg[id].array);
        seg[id].array = NULL;
        push(id);
    }
}


static inline void put(uint32_t m, uint32_t n, uint32_t value)
{
    seg[m].array[n] = value;
}

static inline uint32_t get(uint32_t m, uint32_t n)
{
    return seg[m].array[n];
}

static inline uint32_t segment_size(uint32_t id)
{
    return seg[id].size;
}


static inline void free_memory()
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
 


static inline void put_program(uint32_t to, uint32_t from)
{
    free(seg[to].array);
    seg[to].array = seg[from].array;
    seg[to].size = seg[from].size;
}
/* 
 * The following are the 14 instruction functions
 */
static inline void move(uint8_t ra, uint8_t rb, uint8_t rc)
{
    
        if(registers[rc] != 0){
                registers[ra] = registers[rb];
        }
        (void) ra;
        (void) rb;
        (void) rc;
}

static inline void load_segment(uint8_t ra, uint8_t rb, uint8_t rc)
{
        registers[ra] = get(registers[rb], registers[rc]);
}

static inline void store(uint8_t ra, uint8_t rb, uint8_t rc)
{
        put(registers[ra], registers[rb], registers[rc]);
        (void) ra;
        (void) rb;
        (void) rc;


}

static inline void add(uint8_t ra, uint8_t rb, uint8_t rc)
{
        registers[ra] = registers[rb] + registers[rc];
        (void) ra;
        (void) rb;
        (void) rc;
}

static inline void mult(uint8_t ra, uint8_t rb, uint8_t rc)
{
        registers[ra] = registers[rb] * registers[rc];
        (void) ra;
        (void) rb;
        (void) rc;      
}

static inline void divide(uint8_t ra, uint8_t rb, uint8_t rc)
{
        assert(registers[rc] != 0);
        registers[ra] = registers[rb] / registers[rc];
        (void) ra;
        (void) rb;
        (void) rc;
}

static inline void nand(uint8_t ra, uint8_t rb, uint8_t rc)
{
        registers[ra] = ~(registers[rb] & registers[rc]);
        (void) ra;
        (void) rb;
        (void) rc;
}


static inline void map_segment(uint8_t ra, uint8_t rb, uint8_t rc)
{
        registers[rb] = map(registers[rc]);
        (void) ra;
        (void) rb;
        (void) rc;
}
static inline void unmap_segment(uint8_t ra, uint8_t rb, uint8_t rc)
{
        unmap(registers[rc]);
        (void) ra;
        (void) rb;
        (void) rc;
}

static inline void output(uint8_t ra, uint8_t rb, uint8_t rc)
{
        printbyte((uint8_t) registers[rc]);
        (void) ra;
        (void) rb;
        (void) rc;
}
static inline void input(uint8_t ra, uint8_t rb, uint8_t rc)
{
        uint8_t c = readbyte(stdin);
        if((char) c == EOF){
                registers[rc] = ~0;
        } else{
                registers[rc] = c;
        }
        (void) ra;
        (void) rb;
        (void) rc;

}
static inline void load_program(uint8_t ra, uint8_t rb, uint8_t rc)
{

        if(registers[rb] != 0){
                put_program(0, registers[rb]);
        }
       
        counter = registers[rc];
        program_size = segment_size(FIRST);
        (void) ra;
        (void) rb;
        (void) rc;
}
static inline void load_value(uint32_t word)
{
        uint8_t ra = Bitpack_getu(word, 3, 25);
        registers[ra] = Bitpack_getu(word, 25, 0);
}

static inline void process()
{
        uint32_t word;
        uint8_t ra, rb, rc, opcode;
        opcode = 0;
        ra = 0; 
        rb = 0;
        rc = 0;
        void (*functions[13]) (uint8_t ra, uint8_t rb, uint8_t rc);
        functions[0] = move;
        functions[1] = load_segment;
        functions[2] = store;
        functions[3] = add;
        functions[4] = mult;
        functions[5] = divide;
        functions[6] = nand;
        functions[7] = NULL;
        functions[8] = map_segment;
        functions[9] = unmap_segment;
        functions[10] = output;
        functions[11] = input;
        functions[12] = load_program;
        program_size  = segment_size(FIRST);
        while(counter < program_size && opcode != halt){
             word = next_instruction(counter);
                counter++;
                opcode = (uint8_t) Bitpack_getu(word, 4, 28);
               if(opcode <= 12){
                        ra = (uint8_t) Bitpack_getu(word, 3, 6);
                        rb = (uint8_t) Bitpack_getu(word, 3, 3);
                        rc = (uint8_t) Bitpack_getu(word, 3, 0);
                        if(opcode == 7){
                            break;
                        }
                        if(opcode <= 12) {
                            functions[opcode](ra,rb,rc);
                        }
                        //call_instruction(ra, rb, rc, opcode, functions);
                } else if(opcode == 13){
                        load_value(word);
                }
        }
        (void) functions;
}
static inline void setup_registers()
{
        for(int i = 0; i < 8 ; i++){
                registers[i] = 0;
        }
        counter = 0;
}
static inline void setup_memory(unsigned length)
{
    seg = malloc(length * sizeof(struct elements));
    seg_size = 1;
    seg_capacity = 1;
    setup_element(length, 0);
    stack = malloc(sizeof(uint32_t));
    s_capacity = 1;
    s_index = 0;
    
}

static inline void get_program(FILE *fp, int size)
{
        uint8_t c;
        uint32_t word = 0;
        setup_memory(size/4);
        /* size is teh number of bytes, so nmber of words in size/4*/
        for(int i = 0; i < size/4; i++){
                for(int n = 24; n >= 0; n -= 8){
                        c = readbyte(fp);
                        word = Bitpack_newu(word, 8, n, c);
                }
               put(0,i,word);
               word = 0;
        }
}
int main(int argc, char * argv[])
{
        FILE *fp;
        if(argc != 1){
                fp = fopen(argv[1], "r");
        }
        struct stat st;
        stat(argv[1], &st);
        if (stat(argv[1], &st)){
                exit(EXIT_FAILURE);
        }
        int size = st.st_size;
        get_program(fp, size);
        setup_registers();
        process();
        fclose(fp);
        
        free_memory();
        (void) argv;
        (void) argc;
        return 0;

}








