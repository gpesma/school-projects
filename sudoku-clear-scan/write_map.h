/*
 *    write_map.h
 *    by George Pesmazoglou, Alexandru Ungureanu
 *    September 27, 2016
 * 
 *        Header file for the write_map interface.
 *    The interface can be used to write 2D bit map in a file in a protable 
 *    bitmap (pbm) format.
 */


#include <stdio.h>
#include <stdlib.h>
#include "bit2.h"

/*
 * parameters: file where the bitmap will be written, pointer to 2D bit array
 */
extern void write_map(FILE *fp, Bit2_T *map);