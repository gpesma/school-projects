/*
 *    read_file.h
 *    by George Pesmazoglou, Alexandru Ungureanu
 *    September 27, 2016
 * 
 *    Header file for the read_file interface.
 *        Reads a pbm (portable bitmap) and stores the contents in a 2D Bit Array.
 */



#include <stdio.h>
#include <stdlib.h>
#include "bit2.h"

/*
 * reads the given file and stores the bits in a 2D Bit Array
 */
extern void read_file(FILE *fp, Bit2_T *map);