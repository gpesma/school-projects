/*
 * Georgios Pesmazoglou & Ramchandaran
 * 
 * io_device.c
 * Uses c library functions to read a character
 * from a file stream or prints a byte to
 * standard output
 */


#include <stdlib.h>
#include <stdio.h>
#include "io_device.h"
#include <stdint.h>


uint8_t readbyte(FILE *fp)
{
        char c = fgetc(fp);
        return (uint8_t) c;
}

void printbyte(uint8_t c)
{
        printf("%c",(char) c);
}
