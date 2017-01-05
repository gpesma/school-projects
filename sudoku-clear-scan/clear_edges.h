/*
 *    clear_edges.h
 *    by George Pesmazoglou, Alexandru Ungureanu
 *    September 27, 2016
 * 
 *        Header file for the clear_edges interface.
 *    The interface is used to clear the black edges in a pbm file given as 
 *    input and write the unblacked file in an output file as a pbm.
 */

#include <stdio.h>
#include <stdlib.h>

/*
 * parameters: input file and output file
 */
extern void clear_edges(FILE *in, FILE *of);