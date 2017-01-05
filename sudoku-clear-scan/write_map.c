/*
 *    write_map.c
 *    by George Pesmazoglou, Alexandru Ungureanu
 *    September 27, 2016
 * 
 *        Implementation of the write_map interface.
 */

#include <stdio.h>
#include <stdlib.h>
#include "bit2.h"
#include "print_map.h"

/*
 * helper function; writes a bit at a given position into the file
 * parameters: -the column and row (integers) - position of the bit
 *             -the 2D bit array where the bit is located
 *             -the file where the bit is written
 */
static void write_bit(int, int, Bit2_T, int, void *);

/*
 * writes the given 2D bit map (parameter) into the given file (parameter)
 */ 
void write_map(FILE *fp, Bit2_T *map)
{
                /* pbm format header: P1, and width and height on the next line */
                fprintf(fp, "P1\n");
                fprintf(fp, "%d %d\n", Bit2_width(*map), Bit2_height(*map));

                /* write each bit by "traversing" the 2D bit map row by row */                          
                Bit2_map_row_major(*map, write_bit, fp);
}


static void write_bit(int col, int row, Bit2_T bit2, int value, void *file)
{
                fprintf(file, "%d", Bit2_get(bit2, col, row));
                (void) value;
                if(col == Bit2_width(bit2) - 1){
                            /* if end of row reached, move to the next line */
                                fprintf(file, "\n");
                }
}