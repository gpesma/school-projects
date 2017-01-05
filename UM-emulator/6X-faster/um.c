/*
 * Georgios Pesmazoglou & Vivek Ramchandran
 * Universal Machine Emulator
 * 
 * Faster version of emulator
 * main data strucure is now an unboxed array instead of
 * of boxed
 * simple functions are static inlined
 * 
 * bit operation occur at the same file
 *
 */

#include <stdlib.h>
#include <stdint.h>
#include <sys/stat.h>
#include <stdio.h>


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
        registers[ra] = seg[registers[rb]].array[registers[rc]];
        (void) ra;
        (void) rb;
        (void) rc;
}

static inline void store(uint8_t ra, uint8_t rb, uint8_t rc)
{
        seg[registers[ra]].array[registers[rb]] = registers[rc];
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
        if(seg_size >= seg_capacity){
                seg_capacity *= 2;
                seg = realloc(seg, seg_capacity * sizeof(struct elements));
                for(unsigned i = seg_size; i < seg_capacity; i++){
                        seg[i].array = NULL;
                }
        }
        seg_size++;
        uint32_t id;
        if(s_index == 0){
                id = 0;
        } else {
                s_index--;
                id = stack[s_index];
        }
        if(id != 0){
                uint32_t *array = calloc(registers[rc], sizeof(uint32_t));
                seg[id].size = registers[rc];
                seg[id].array = array;
                registers[rb] = id;
        } else {
                uint32_t *array = calloc(registers[rc], sizeof(uint32_t));
                seg[seg_size - 1].size = registers[rc];
                seg[seg_size - 1].array = array;
                registers[rb] = (seg_size - 1);
        }
        (void) ra;
        (void) rb;
        (void) rc;  
}
static inline void unmap_segment(uint8_t ra, uint8_t rb, uint8_t rc)
{
        if(seg[registers[rc]].array != NULL){
                free(seg[registers[rc]].array);
                seg[registers[rc]].array = NULL;
                if(s_index >= s_capacity){
                        s_capacity *= 2;
                        stack = realloc(stack, s_capacity * sizeof(uint32_t));
                }
                stack[s_index] = registers[rc];
                s_index++;
        }
        (void) ra;
        (void) rb;
        (void) rc;       
}

static inline void output(uint8_t ra, uint8_t rb, uint8_t rc)
{
        printf("%c",(char) registers[rc]);
        (void) ra;
        (void) rb;
        (void) rc;
}
static inline void input(uint8_t ra, uint8_t rb, uint8_t rc)
{
        uint8_t c = fgetc(stdin);
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
                free(seg[0].array);
                seg[0].array = seg[registers[rb]].array;
                seg[0].size = seg[registers[rb]].size;
        }
       
        counter = registers[rc];
        program_size = seg[FIRST].size;
        (void) ra;
        (void) rb;
        (void) rc;
}
static inline void load_value(uint32_t word)
{
        uint8_t ra = (word << 36) >> 61;
        registers[ra] = (word << 39) >> 39;     
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


        /* get_program*/
        uint8_t c;
        uint32_t word = 0;
       /* setup_memory*/
        seg = malloc(size/4 * sizeof(struct elements));
        seg_size = 1;
        seg_capacity = 1;
        uint32_t *array = calloc(size/4, sizeof(uint32_t));
        seg[0].size = size/4;
        seg[0].array = array;
        stack = malloc(sizeof(uint32_t));
        s_capacity = 1;
        s_index = 0;


        /* size is teh number of bytes, so nmber of words in size/4*/
        for(int i = 0; i < size/4; i++){
                for(int n = 24; n >= 0; n -= 8){
                        c = fgetc(fp);
                        word = ((word >> (8 + n)) << (8 + n)) |
                                (word << (64 - n) >> (64 - n));
                }
                seg[0].array[i] = word;
               word = 0;
        }

        /* setup_registers*/
        for(int i = 0; i < 8 ; i++){
                registers[i] = 0;
        }
        counter = 0;

        /* process */

        uint8_t ra = 0;
        uint8_t rb = 0;
        uint8_t rc = 0;
        uint8_t opcode = 0;
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
        program_size  = seg[FIRST].size;;
        while(counter < program_size && opcode != halt){
                word = seg[0].array[counter];
                counter++;
                opcode = (word << 32) >> 60;
                if(opcode <= 12){
                        ra = (word << 55) >> 61;
                        rb = (word << 58) >> 61;
                        rc =  (word << 61) >> 61;
                        if(opcode == 7){
                                break;
                        }
                        if(opcode <= 12) {
                                functions[opcode](ra,rb,rc);
                        }
                } else if(opcode == 13){
                        load_value(word);
                }
        }


        fclose(fp);
        
        for(unsigned i = 1; i < seg_size - 1; i++){
                if (seg[i].array != NULL && seg[i].array != seg[0].array) {
                        free(seg[i].array);
                        seg[i].array = NULL;
                }
        }
        free(seg[0].array);
        free(seg);
        free(stack);
        (void) argc;
        (void) argv;
        return 0;

}