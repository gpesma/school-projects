/*
 *    unblackedges.c
 *    by George Pesmazoglou, Alexandru Ungureanu
 *    September 27, 2016
 * 
 *    Contains the main function of the unblackedges program. 
 *    The main function handles the inputs given from the command line: either
 *    a file or the standard input.
 */


#include <stdio.h>
#include <stdlib.h>
#include "clear_edges.h"


int main(int argc, char *argv[])
{
                if(argc != 1 && argc != 2){
                                /* runtime error if more than 2 files given */
                                fprintf(stderr, "Error: Too many arguments. ");
                                exit(EXIT_FAILURE);
                }

                
                if (argc == 1) {
                                clear_edges(stdin,stdout);
                } else {
                                FILE *fp = fopen(argv[1], "r");
                                if(fp == NULL){
                                                fprintf(stderr,
                                                 "Error: Canoot open file");
                                                exit(EXIT_FAILURE);
                                }

                                clear_edges(fp, stdout);
                                fclose(fp);
                }
                return 0;
}

