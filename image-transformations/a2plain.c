/*
 * a2plain.c
 * by George Pesmazoglou, Alexandru Ungureanu
 * Date: October 6th, 2016
 *
 * Source file of the a2plain interface
 * An A2Methods_T is a structure of functions that can be applied to a 2D 
 * unboxed array, regardless of the way the array is implemented. The a2plain
 * interface is used to implement the a2methods interface using an UArray2. 
 * The header file of a2plain contains the declaration of a structure of type 
 * A2Methods_T. This source file implements a structure of private functions 
 * that is then attributed to the public A2Methods_T structure.
 */

#include <stdlib.h>

#include <a2plain.h>
#include "uarray2.h"

/*
 * the type attributed to a 2D array used by the a2methods functions
 * in this implementation, an A2Methods_UArray2 is actually an UArray2 
 */

typedef A2Methods_UArray2 A2;

/*
 * new: creates a new 2D unboxed polymorphic array
 * parameters: width, height and size of an element of the new array
 * returns: the newly created 2D unboxed array
 */
static A2 new(int width, int height, int size)
{
        return UArray2_new(width, height, size);
}

/*
 * since a plain 2D unboxed array has no blocks, this function is the same 
 * as the previous one
 */
static A2Methods_UArray2 new_with_blocksize(int width, int height, int size,
                                            int blocksize)
{
        (void)blocksize;
        return UArray2_new(width, height, size);
}

/*
 * free: frees the space allocated for the 2D unboxed array
 * parameter: a pointer to the array
 */
static void a2free(A2 *array2p)
{
        UArray2_free((UArray2_T *) array2p);
}

/*
 * width: returns the width of the 2D array given as parameter
 */
static int width(A2 array2)
{
        return UArray2_width(array2);
}

/*
 * height: returns the height of the 2D array given as parameter
 */
static int height(A2 array2)
{
        return UArray2_height(array2);
}

/*
 * size: returns the size of one element of the 2D array given as parameter
 */
static int size(A2 array2)
{
        return UArray2_size(array2);
}

/*
 * since a plain 2D array has no blocks, returns 1 (equivalent to 1-cell blocks)
 */
static int blocksize(A2 array2)
{
        (void) array2;
        return 1;
}

/*
 * at: returns a pointer to the element at position [i,j] in the given 2D array 
 */
static A2Methods_Object *at(A2 array2, int i, int j)
{
        return UArray2_at(array2, i, j);
}

typedef void applyfun(int i, int j, UArray2_T array2, void *elem, void *cl);

/*
 * map function that applies the given apply function to each element by
 * traversing the array row by row
 * parameters: the 2D unboxed array, the apply function, the closure pointer 
 */
static void map_row_major(A2 array2, A2Methods_applyfun apply, void *cl)
{
        UArray2_map_row_major(array2, (applyfun *) apply, cl);
}

/*
 * map function that applies the given apply function to each element by
 * traversing the array column by column
 * parameters: the 2D unboxed array, the apply function, the closure pointer 
 */
static void map_col_major(A2 array2, A2Methods_applyfun apply, void *cl)
{
        UArray2_map_col_major(array2, (applyfun *) apply, cl);
}

/*
 * need to pass both the apply function and the original closure pointer 
 */
struct small_closure {
        A2Methods_smallapplyfun *apply;
        void *cl;
};

/*
 * apply function that can be called for a 2D unboxed plain array and applies
 * the original "small" apply function given in the structure-closure pointer
 */
static void apply_small(int i, int j, UArray2_T array2, void *elem, void *vcl)
{
        struct small_closure *cl = vcl;
        (void)i;
        (void)j;
        (void)array2;
        cl->apply(elem, cl->cl);
}

/*
 * map function that applies the given apply function to each element by
 * traversing the array column by column
 * parameters: the 2D unboxed array, the apply function, the closure pointer 
 * the difference is that the apply function only has the element that is 
 * "traversed" at that point and the closure pointer
 */
static void small_map_col_major(A2 a2, A2Methods_smallapplyfun apply,
                                  void *cl)
{
        /* pass the apply function and the original closure pointer */
        struct small_closure mycl = { apply, cl };
        UArray2_map_col_major(a2, apply_small, &mycl);
}

/*
 * map function that applies the given apply function to each element by
 * traversing the array row by row
 * parameters: the 2D unboxed array, the apply function, the closure pointer 
 * the difference is that the apply function only has the element that is 
 * "traversed" at that point and the closure pointer
 */
static void small_map_row_major(A2 a2, A2Methods_smallapplyfun apply,
                                  void *cl)
{
        /* pass the apply function and the original closure pointer */
        struct small_closure mycl = { apply, cl };
        UArray2_map_row_major(a2, apply_small, &mycl);
}

/*
 * create a structure containing the functions defined in the implementation
 * this structure will be attributed to the A2Methods_T structure declared
 * in the interface to complete the implementation of a2methods using plain
 * 2D unboxed arrays
 */
static struct A2Methods_T uarray2_methods_plain_struct = {
        new,                                      
        new_with_blocksize,
        a2free,
        width,
        height,
        size,
        blocksize,
        at,
        map_row_major,
        map_col_major,
        NULL,           // map_block_major 
        map_row_major,  // map_default
        small_map_row_major,
        small_map_col_major,
        NULL,                   // small_map_block_major
        small_map_row_major,    // small_map_default
};

        A2Methods_T uarray2_methods_plain = &uarray2_methods_plain_struct;