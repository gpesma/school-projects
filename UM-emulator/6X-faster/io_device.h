/*
 * interface for the io devic
 * prints and reads a byte
 */

#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
extern uint8_t readbyte(FILE *fp);
extern void printbyte(uint8_t c);