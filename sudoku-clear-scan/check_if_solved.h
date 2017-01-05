/*
 *    readmap.h
 *    by George Pesmazoglou, Alexandru Ungureanu
 *    September 27, 2016
 * 
 *    Header file of the check_if_solved interface. 
 *    The interface is responsible for checking if a sudoku solution
 *    given in a graymap file is correct.
 */

#include <stdio.h>
#include <stdlib.h>

/*
 * Controls the flow of the program
 * makes the 2D array, uses readmap interface
 * to pass store the sudoku checks if it is solved and returns
 * 0 if yes or 1 if no.
 * If only one criterium is wrong then the entire sudoku
 * is unsolved
 */

extern int check_if_solved(FILE *fp);
