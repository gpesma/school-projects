/*
 * compressed_io.h
 * Header file of the compressed_io interface
 * October 21st, 2016
 * Georgios Pesmazoglou, Alexandru Ungureanu
 *
 * Interface responsible for the input-output of a compressed image 
 * Contains functions that print/read the image header (contains info
 * about the width and height of the image).
 * Also contains functions that print/read a 32-bit word of the compressed
 * image file; a word encodes the characterisitics of the pixels of the image
 */

#include <stdio.h>
#include "pnm.h"
#include <stdint.h>

/*
 * prints the header of the compressed file in the required format
 * parameters: width and height of the image, output file 
 */
extern void print_header(unsigned width, unsigned height, FILE *ouptut);

/* 
 * prints a 32-bit word as a 4-character word
 * parameters: the word, the output file
 */
extern void print_word(uint32_t word, FILE *output);

/*
 * reads the header of the compressed file (in the required format)
 * anc creates an instace of the pnm interface that contains information
 * about the dimensions of the image
 * parameters: the input file, the methods "instance" to be used for the pnm
 * returns the pnm "instance" with an empty 2D array of pixels field
 */
extern Pnm_ppm read_header(FILE *input, A2Methods_T *methods);

/* 
 * reads a 32-bit word stored in the input file by reading the 4 characters
 * of the word; parameters: the input file
 */
extern uint32_t read_word(FILE *input);