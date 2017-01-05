/*
 *    bit2.c
 *    by George Pesmazoglou, Alexandru Ungureanu
 *    September 27, 2016
 * 
 *    Implementation of the bit2 interface
 *    The 2D Bit Array is represented as a structure containing the following
 *    fields:
 *        - the width and the height (integers)
 *        - an array of bit 1D arrays (each bit array represents a column)
 */


#include <stdio.h>
#include <stdlib.h>
#include "bit2.h"
#include "bit.h"
#include "mem.h"
#include "assert.h"

#define T Bit2_T

struct T {
                int width;
                int height;
                Bit_T *cols;
};

/*
 *    creates a new 2D bit array
 *    parameters: width and height of the new bit map(integers)
 *    returns: the new bit 2D array
 */
T Bit2_new(int width, int height)
{
                T bit2;
                NEW(bit2);
                /* runtime error if space cannot be allocated */
                assert(bit2 != NULL);

                /* the columns array contains width UArrays of length height */
                bit2->cols = calloc(width, height * sizeof(unsigned long));
                for(int i = 0; i < width; i++){
                                bit2->cols[i] = Bit_new(height);
                }
                bit2->width = width;
                bit2->height = height;

                return bit2;
}

/*
 *    frees the given bit 2D array
 *    parameter: pointer to a 2D bit array
 */
void Bit2_free(T *bit2)
{
                assert(bit2 != NULL && *bit2 != NULL);
                for( int i = 0; i < (*bit2)->width; i++){
                        /* free each column - bit array */
                                Bit_free(((*bit2)->cols)+i);
                }
                /* free the array that stored the columns */
                free((*bit2)->cols);

                free(*bit2);
}

/*
 *    parameter: a 2D bit array
 *    returns: the height of the bit 2D array
 */
int Bit2_height(T bit2)
{
                assert(bit2 != NULL);
                return bit2->height;
}

/*
 *    parameter: a 2D bit array
 *    returns: the width of the bit 2D array
 */
int Bit2_width(T bit2)
{
                assert(bit2 != NULL);
                return bit2->width;
}

/*
 *    inserts a value at the given position 
 *    parameters: -a 2D bit array
 *                -the column and row (integers)
 *                -the value to be inserted at that position (integer - 0 or 1)
 *    returns: the previous value at that position
 */
int Bit2_put(T bit2, int col, int row, int value)
{
                assert(row < (bit2->height) && row >= 0);
                assert(col < (bit2->width) && col >= 0);
                /* previous value */
                int prev = Bit2_get(bit2, col, row);
                assert(bit2 != NULL);
                /* put the value in column col at position row */
                Bit_put(bit2->cols[col], row, value);
                return prev;
}

/*
 *    parameters: -a 2D bit array
 *                -the column and row (integers)
 *    returns the value at that position
 */
int Bit2_get(T bit2, int col, int row)
{
                assert(bit2 != NULL);
                return Bit_get(bit2->cols[col], row);
}

/*
 *     Loops through bit map row by row and applies a given function to each
 *     element parameters: the 2D bit array, the function to be applied, and a 
 *     pointer to the closure that is passed at each step
 */
void Bit2_map_col_major (T bit2, void apply(int col, int row, 
                        T bit2, int value, void *cl), void *cl)
{
                for(int col = 0; col < bit2->width; col++) {
                                for(int row = 0; row < bit2->height; row++) {
                                                int val =
                                                 Bit2_get(bit2, col, row);
                                                apply(col, row, bit2, val, cl);
                                }
                }
}

/*
 *    Loops through bit map column by column and applies a given function to 
 *    each element
 *    parameters: 2D bit array, the function to be applied, and a pointer to
 *    the closure that is passed at each step
 */
void Bit2_map_row_major (T bit2, void apply(int col, int row, 
                        T bit2, int value, void *cl), void *cl)
{
                for(int row = 0; row < bit2->height; row++) {
                                for(int col = 0; col < bit2->width; col++) {
                                                int val =
                                                 Bit2_get(bit2, col, row);
                                                apply(col, row, bit2, val, cl);
                                }
                }
}
