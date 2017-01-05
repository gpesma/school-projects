/*
 * uarray2b.c
 * by George Pesmazoglou, Alexandru Ungureanu
 * Date: October 6th, 2016
 *
 * Source file of the uarray2b interface
 * In order to represent a 2D unblocked polymorphic array, we are using a
 * UArray2 of blocks. The implementation has access to the UArray 2 interface
 * Each of the blocks is represented as a Hanson UArray so that the cells in the
 * block are stored in a contiguos array in memory.
 *
 * Invariants: 
 *      - at each point, an element in the blocked 2D array is the actual data 
 *        to be used, and not a pointer to the data (unboxed repr. invariant)
 *      - in order to translate a [col, row] cell in the array to its position
 *        in our representation, we use the following formulas:
 *        block position in the UArray2 = [col / blocksize, row / blocksize]
 *        position in the UArray of the block = (row % blocksize) * blocksize +
 *                                                        (col % blocksize)
 *      - in order to translate a cell position in our representation to a 
 *        [col, row] position in the 2D blocked array we use the following:
 *        row = pos in block / blocksize + row of block * blocksize
 *        col = pos in block % blocksize + col of block * blocksize
 */


#include <stdio.h>
#include <stdlib.h>
#include "uarray2.h"
#include "uarray.h"
#include "assert.h"
#include "uarray2b.h"
#include "mem.h"
#include <math.h>


#define T UArray2b_T
int KILO = 1024;




/*
 * representation of the blocked array as a structure
 * fields: the width and height of the array (in cells), the size of an element,
 *         the size of one block (in cells), and the UArray2 of blocks
 */
struct T {
        int width;
        int height;
        int size;
        int blocksize;
        UArray2_T grid;
};

struct map_cl {
        T *uarray2b;
        void (*apply)(int col, int row, T uarray2b, void *elem, void *cl);
        void *cl;
};

/*
 * free_block function to be used when mapping
 * parameters: the column and row numbers representing the position of the 
 * block in the UArray2 of blocks, the UArray2, the block element, and the 
 * closure frees the space allocated for the block at the given position 
 */
static void free_block(int, int, UArray2_T, void *, void *);

/*
 * set_blocks function to be used when mapping 
 * parameters: the column and row numbers representing the position of the 
 * block in the UArray2 of blocks, the blocked array, the block element, and 
 * the closure
 * creates a new UArray to represent the block at the given position in the
 * UArray2 of blocks
 */
static void set_blocks(int, int, UArray2_T, void *, void *); 

/*
 * apply helper function used to map through each block
 * parameters: the column and row numbers representing the position of the block
 *             in the UArray2 of blocks, the UArray2 of blocks, the element 
 *             (block) at the position, and theclosure
 */
static void block_apply(int, int, UArray2_T, void *, void *);

/*
 * UArray2b_new functions: creates a new 2D blocked array
 * parameters: the width and height of the new array (in cells), the size
 * of one element, and the size of one block
 * returns the newly created blocked 2D array (a UArray2 of blocks)
 * # of complete blocks in a UArray2 row = # of cells in row / size of block
 * # of complete blocks in a UArray2 col = # of cells in a col / size of block
 * size of block = # of cells in block * size of one cell element
 */
T UArray2b_new(int width, int height, int size, int blocksize)
{
        T uarray2b;
        NEW(uarray2b);
        assert(uarray2b);
        int blocked_width = width / blocksize;
        if (width % blocksize > 0) {
                /* one incomplete block at the end of each row */
                blocked_width++;
        }
        int blocked_height = height / blocksize;
        if (height % blocksize > 0) {
                /* one incomplete block at the end of each column */
                blocked_height++;
        } 
        uarray2b->width = width;
        uarray2b->height = height;
        uarray2b->size = size;
        uarray2b->blocksize = blocksize;
        /* there are blocksize * blocksize cells in a block */
        uarray2b->grid = UArray2_new(blocked_width, blocked_height,
                                size * blocksize * blocksize);
        /* allocate space for each of the blocks in the array */
        UArray2_map_row_major(uarray2b->grid, set_blocks, &uarray2b);

        return uarray2b;
}

/*
 * UArray2b_new functions: creates a new 2D blocked array with blocks as big 
 * as possible but not more than 64Kb
 * parameters: the width and height of the new array (in cells), the size
 * of one element
 * returns the newly created blocked 2D array (a UArray2 of blocks)
 * if one element has a size that is more than 64Kb, automatically set the 
 * size of one block to 1
 */
T UArray2b_new_64K_block(int width, int height, int size)
{
        int blocksize;
        if(size > 64 * KILO) {
                blocksize = 1;
        } 
        else {
                blocksize = (int) sqrt((64 * KILO) / size);
        }
        /* create a new blocked array with the maximized block size */
        return UArray2b_new(width, height, size, blocksize);
}

/*
 * UArray2b_free function: frees the memory for the given blocked 2D array 
 * parameter: pointer to the 2D unblocked array
 */
void UArray2b_free(T *uarray2b)
{
        /* free each block in the array of blocks, and then the array */
        assert(uarray2b);
        UArray2_map_row_major((*uarray2b)->grid, free_block, NULL);
        UArray2_free(&((*uarray2b)->grid));
        FREE(*uarray2b);
}

/*
 * UArray2b_width function: returns width (cells) of the given blocked array
 */
int UArray2b_width (T uarray2b)
{
        return uarray2b->width;
}

/*
 * UArray2b_height function: returns height (cells) of the given blocked array
 */
int UArray2b_height (T uarray2b)
{
        return uarray2b->height;
}

/*
 * UArray2b_size function: returns size of one element of given blocked array
 */
int UArray2b_size (T uarray2b)
{
        return uarray2b->size;
}

/*
 * UArray2b_blocksize function: returns the size of one complete block
 */
int UArray2b_blocksize (T uarray2b)
{
        return uarray2b->blocksize;
}

/*
 * UArray2b_at function: gives access to an element based on its cell-position
 * parameters: the blocked 2D array, the column and row numbers representing
 *             the position of the element in the blocked array
 * returns: pointer to the element at the specified position
 */  
void *UArray2b_at(T uarray2b, int col, int row)
{

        assert(col < uarray2b->width && row < uarray2b->height);
        int blocksize = uarray2b->blocksize;
        /* get the block that contains the given element */
        UArray_T *block = UArray2_at(uarray2b->grid, col / blocksize, 
                                        row / blocksize);
        /* compute the column and row numbers of the block in the UArray2 */
        int col_in_block = col % blocksize;
        int row_in_block = row % blocksize;
        /* get the element at the determined [col, row] position in the block */
        return UArray_at(*block, row_in_block * blocksize + col_in_block);
}

/*
 * UArray2b_map function: visits each element and applies the given function
 * to it; the array is "traversed" block by block 
 * parameters: the blocked 2D array, the function to be applied, the closure
 */
void UArray2b_map (T uarray2b, void apply (int col, int row, T uarray2b, 
                                            void *elem, void *cl), void *cl)
{
        struct map_cl my_cl = { &uarray2b, apply, cl };
        UArray2_map_row_major(uarray2b->grid, block_apply, &my_cl);
}

/* implementation of the helper functions */

void set_blocks(int col, int row, UArray2_T uarray2, void *block, void *cl)
{
        (void) uarray2;
        (void) row;
        (void) col;
        /* pointer to the 2D unboxed block array */
        T *map = (T*) cl;
        int length = (*map)->blocksize * (*map)->blocksize;      
        * (UArray_T*) block = UArray_new(length, UArray2b_size(*map));  
}


void free_block(int col, int row, UArray2_T uarray2, void *block, void *cl)
{
        (void) col;
        (void) row;
        (void) uarray2;
        (void) cl;
        UArray_free((UArray_T *) block);
}

void block_apply(int col_of_block, int row_of_block, UArray2_T array2, 
                                                    void *block, void *cl)
{
        UArray_T block_array = *(UArray_T *) block;
        struct map_cl *my_cl = (struct map_cl *) cl;
        T uarray2b = *(my_cl->uarray2b);
        for (int i = 0; i < UArray_length(block_array); i++) {
                /* find the [col, row] position of the cell */
                int row = i / uarray2b->blocksize + row_of_block * 
                                    uarray2b->blocksize;
                int col = i % uarray2b->blocksize + col_of_block * 
                                              uarray2b->blocksize;
                 void *elem = UArray_at(block_array, i);
                /* if the cell is not out of bounds, call the apply function */
                if(row < uarray2b->height && col < uarray2b->width) {
                        my_cl->apply(col, row, uarray2b, elem, my_cl->cl);
                }
        }
        (void) array2;
}