/*
 *    sudoku.c
 *    by George Pesmazoglou, Alexandru Ungureanu
 *    September 27, 2016
 * 
 *    Source file containing the main function.
 *    The main function is responsible for handling the arguments/standard 
 *    input. The program can take in maximum one file. 
 *    
 */

#include <stdio.h>
#include <stdlib.h>
#include "check_if_solved.h"
#include "assert.h"

/*
 * Checks input arguments and calls
 * check if solved which returns 1 or 0 
 * depending if the sudoku is solved
 */
int main(int argc, char *argv[])
{
                int works;
                assert(argc == 1 || argc == 2);
                if (argc == 1) {
                                return check_if_solved(stdin);
                }
                else {
                                FILE *fp = fopen(argv[1], "r");
                                assert(fp != NULL);
                                works = check_if_solved(fp);
                                fclose(fp);
                                return works;
                }
}