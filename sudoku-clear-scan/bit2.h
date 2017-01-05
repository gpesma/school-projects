/*
 *    bit2.h
 *    by George Pesmazoglou, Alexandru Ungureanu
 *    September 27, 2016
 * 
 *    Header file of the bit2 interface
 *    A Bit2_T is a 2D bit array and has a height and a width
 *    
 *    Clients can insert and get values from the bit 2D array, and also apply
 *    a certain function to each element by "traversing" the map row by row or
 *    column by column.
 */

#ifndef BIT2_INCLUDED
#define BIT2_INCLUDED

#define T Bit2_T
typedef struct T *T;

/*
 * creates a new bit 2D array with the given width and height and
 * returns the newly created bit array
 */
extern T Bit2_new (int width, int height);

/*
 * frees the memory allocated for the bit array whose pointer
 * is given as a parameter
 */
extern void Bit2_free (T *bit2);

/*
 * returns the width of the bit array given as parameter 
 */
extern int Bit2_width (T bit2);

/*
 * returns the height of the bit array given as parameter
 */
extern int Bit2_height (T bit2);

/*
 * inserts the given bit value (0 or 1) in the bit array 
 * at the given position 
 */
extern int Bit2_put (T bit2, int col, int row, int value);

/*
 * returns the bit value at the given position in the bit array
 */
extern int Bit2_get (T bit2, int col, int row);

/*
 * map functions used to apply the function given as parameter
 * for each element in the given bit array; Bit2_map_col_major
 * goes through the elements of each column, while Bit2_map_row_major
 * goes through the elements of each row
 */
extern void Bit2_map_col_major (T bit2, void apply(int col, int row, 
                        T bit2, int value, void *cl), void *cl);
extern void Bit2_map_row_major (T bit2, void apply(int col, int row, 
                        T bit2, int value, void *cl), void *cl);

#undef T
#endif
