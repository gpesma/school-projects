/*
 *    readmap.h
 *    by George Pesmazoglou, Alexandru Ungureanu
 *    September 27, 2016
 * 
 *    Header file of the readmap interface. 
 *    The interface is responsible for reading a graymap file and storing its 
 *    contents in a 2D unboxed array.
 *    
 */

#include <stdio.h>
#include <stdlib.h>
#include "uarray2.h"

/*
 * This interface is passed in a file 
 * and a 2D array, uses pnmrdr to pass in
 * the sudoku values from the file to the sudoku
 */ 

extern void readmap(FILE *fp, UArray2_T *map);