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


static uint8_t get_ra(uint32_t word);
static uint8_t get_rb(uint32_t word);
static uint8_t get_rc(uint32_t word);
static uint8_t get_opcode(uint32_t word);
static void call_instruction(uint8_t ra,uint8_t rb,uint8_t rc, uint8_t opcode, 
    void (*functions[13]) (uint8_t ra, uint8_t rb, uint8_t rc));
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
uint32_t *counter;

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

        int map_counter = 0;
        int unmap_counter = 0;
        while(*counter < segment_size(FIRST) && opcode != halt){
                word = next_instruction(counter);
               // printf("instructions:  %x\n",word);
                opcode = get_opcode(word);
                if(opcode == 8){
                    map_counter++;
                    printf("map_counter:%d\n", map_counter);
                }
                if(opcode == 9){
                    unmap_counter++;
                    printf("unmap_counter:%d\n", unmap_counter);
                }
                if(opcode != LOAD_VAL){
                        ra = get_ra(word);
                        rb = get_rb(word);
                        rc = get_rc(word);
                        if(opcode == 7){
                            exit (EXIT_SUCCESS);
                        }
                        call_instruction(ra, rb, rc, opcode, functions);
                } else {
                        load_value(word);
                }
              //  printf("counter %u", *counter);
        }
        free(counter);
        (void) functions;
}


void call_instruction(uint8_t ra,uint8_t rb,uint8_t rc, uint8_t op,
            void (*functions[13]) (uint8_t ra, uint8_t rb, uint8_t rc))
{
        if(op <= 12) {
                functions[op](ra,rb,rc);
        }
        /*
        switch(opcode){
                case 0:
                        move(ra, rb, rc);
                        break;
                case 1:
                        load_segment(ra, rb, rc);
                        break;
                case 2:
                        store(ra, rb, rc);
                        break;
                case 3:
                        add(ra, rb, rc);
                        break;
                case 4:
                        mult(ra, rb, rc);
                        break;
                case 5:
                        divide(ra, rb, rc);
                        break;
                case 6:
                        nand(ra, rb, rc);
                        break;
                case 7:
                        break;
                case 8:
                        map_segment(ra, rb, rc);
                        break;
                case 9:
                        unmap_segment(ra, rb, rc);
                        break;
                case 10:
                        output(ra, rb, rc);
                        break;
                case 11:
                        input(ra, rb, rc);
                        break;
                case 12:
                        load_program(ra, rb, rc);
                        break;
                default:
                        exit (EXIT_FAILURE);
        }
        */
}

uint8_t get_ra(uint32_t word){
        return (uint8_t) Bitpack_getu(word, 3, 6);
}

uint8_t get_rb(uint32_t word){
        return (uint8_t) Bitpack_getu(word, 3, 3);
}

uint8_t get_rc(uint32_t word){
        return (uint8_t) Bitpack_getu(word, 3, 0);
}

uint8_t get_opcode(uint32_t word){
        return (uint8_t) Bitpack_getu(word, 4, 28);
}
void setup_registers()
{
        for(int i = 0; i < 8 ; i++){
                registers[i] = 0;
        }
        counter = malloc(sizeof(uint32_t));
        *counter = 0;
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
        (void) ra;
        (void) rb;
        (void) rc;
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
        *counter = registers[rc];
        (void) ra;
        (void) rb;
        (void) rc;
}
void load_value(uint32_t word)
{
        uint8_t ra = Bitpack_getu(word, 3, 25);
        registers[ra] = Bitpack_getu(word, 25, 0);
}
