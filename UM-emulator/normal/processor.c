/*
 * Georgios Pesmazoglou & Vivek Ramchandaran
 *
 * processor.c
 *
 * "procesor" of UM machine
 * Extracts one instruction at a time from segment 0
 * Decodes ra,rb,rc values and opcode
 * calls appriate function to perform the instruction
 *
 */


#include <stdlib.h>
#include <stdint.h>
#include "bitpack.h"
#include "processor.h"
#include "segment.h"
#include "assert.h"
#include "io_device.h"
#include "assert.h"

#define FIRST 0
#define LOAD_VAL 13
#define halt 7


/*static uint8_t get_ra(uint32_t word);
static uint8_t get_rb(uint32_t word);
static uint8_t get_rc(uint32_t word);
static uint8_t get_opcode(uint32_t word);
*/
//static void call_instruction(uint8_t ra,uint8_t rb,uint8_t rc, uint8_t opcode, 
//    void (*functions[13]) (uint8_t ra, uint8_t rb, uint8_t rc));
static void move(uint8_t ra, uint8_t rb, uint8_t rc);
static void load_segment(uint8_t ra, uint8_t rb, uint8_t rc);
static void store(uint8_t ra, uint8_t rb, uint8_t rc);
static void add(uint8_t ra, uint8_t rb, uint8_t rc);
static void mult(uint8_t ra, uint8_t rb, uint8_t rc);
static void divide(uint8_t ra, uint8_t rb, uint8_t rc);
static void nand(uint8_t ra, uint8_t rb, uint8_t rc);
static void map_segment(uint8_t ra, uint8_t rb, uint8_t rc);
static void unmap_segment(uint8_t ra, uint8_t rb, uint8_t rc);
static void output(uint8_t ra, uint8_t rb, uint8_t rc);
static void input(uint8_t ra, uint8_t rb, uint8_t rc);
static void load_program(uint8_t ra, uint8_t rb, uint8_t rc);
static void load_value(uint32_t word);

uint32_t registers[8];
uint32_t counter;
uint32_t program_size;

/*
 *  first function that loops through instructions
 */
void process()
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
/*

void call_instruction(uint8_t ra,uint8_t rb,uint8_t rc, uint8_t op,
            void (*functions[13]) (uint8_t ra, uint8_t rb, uint8_t rc))
{
        if(op <= 12) {
                functions[op](ra,rb,rc);
        }
}
*/
void setup_registers()
{
        for(int i = 0; i < 8 ; i++){
                registers[i] = 0;
        }
        counter = 0;
}

/* 
 * The following are the 14 instruction functions
 */
void move(uint8_t ra, uint8_t rb, uint8_t rc)
{
    
        if(registers[rc] != 0){
                registers[ra] = registers[rb];
        }
        (void) ra;
        (void) rb;
        (void) rc;
}

void load_segment(uint8_t ra, uint8_t rb, uint8_t rc)
{
        registers[ra] = get(registers[rb], registers[rc]);
}

void store(uint8_t ra, uint8_t rb, uint8_t rc)
{
        put(registers[ra], registers[rb], registers[rc]);
        (void) ra;
        (void) rb;
        (void) rc;


}

void add(uint8_t ra, uint8_t rb, uint8_t rc)
{
        registers[ra] = registers[rb] + registers[rc];
        (void) ra;
        (void) rb;
        (void) rc;
}

void mult(uint8_t ra, uint8_t rb, uint8_t rc)
{
        registers[ra] = registers[rb] * registers[rc];
        (void) ra;
        (void) rb;
        (void) rc;      
}

void divide(uint8_t ra, uint8_t rb, uint8_t rc)
{
        assert(registers[rc] != 0);
        registers[ra] = registers[rb] / registers[rc];
        (void) ra;
        (void) rb;
        (void) rc;
}

void nand(uint8_t ra, uint8_t rb, uint8_t rc)
{
        registers[ra] = ~(registers[rb] & registers[rc]);
        (void) ra;
        (void) rb;
        (void) rc;
}


void map_segment(uint8_t ra, uint8_t rb, uint8_t rc)
{
        registers[rb] = map(registers[rc]);
        (void) ra;
        (void) rb;
        (void) rc;
}
void unmap_segment(uint8_t ra, uint8_t rb, uint8_t rc)
{
        unmap(registers[rc]);
        (void) ra;
        (void) rb;
        (void) rc;
}

void output(uint8_t ra, uint8_t rb, uint8_t rc)
{
        printbyte((uint8_t) registers[rc]);
        (void) ra;
        (void) rb;
        (void) rc;
}
void input(uint8_t ra, uint8_t rb, uint8_t rc)
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
void load_program(uint8_t ra, uint8_t rb, uint8_t rc)
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
void load_value(uint32_t word)
{
        uint8_t ra = Bitpack_getu(word, 3, 25);
        registers[ra] = Bitpack_getu(word, 25, 0);
}

uint64_t Bitpack_getu(uint64_t word, unsigned width, unsigned lsb)
{
        unsigned hi = lsb + width; /* one beyond the most significant bit */
        assert(hi <= 64);
        /* different type of right shift */
        return shr(shl(word, 64 - hi), 64 - width); 
}


uint64_t Bitpack_newu(uint64_t word, unsigned width, unsigned lsb,
                      uint64_t value)
{
        unsigned hi = lsb + width; /* one beyond the most significant bit */
        assert(hi <= 64);
        if (!Bitpack_fitsu(value, width))
                RAISE(Bitpack_Overflow);
        return shl(shr(word, hi), hi)                 /* high part */
                | shr(shl(word, 64 - lsb), 64 - lsb)  /* low part  */
}

ra = (uint8_t) Bitpack_getu(word, 3, 6);
               rb = (uint8_t) Bitpack_getu(word, 3, 3);
        rc = (uint8_t) Bitpack_getu(word, 3, 0);