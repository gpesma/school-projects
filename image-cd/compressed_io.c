/* 
 * compressed_io.c
 * Implementation (source file) of the compressed_io interface
 * October 22nd, 2016
 * Georgios Pesmazoglou, Alexandru Ungureanu
 */

#include <stdlib.h>
#include <stdio.h>
#include "compressed_io.h"
#include <string.h>
#include "bitpack.h"
#include "assert.h"

static const unsigned RGB_DENOMINATOR = 255;
static const unsigned CHAR_SIZE = 8;
static const unsigned WORD_SIZE = 32;

/*
 * prints the header of the compressed file in the required format
 * parameters: width and height of the image, output file 
 */
void print_header(unsigned width, unsigned height, FILE *output)
{
        assert(output != NULL);
        fprintf(output, "COMP40 Compressed image format 2\n%u %u\n", width, 
                                                                   height);
}

/* 
 * prints a 32-bit word as a 4-character word
 * parameters: the word, the output file
 */
void print_word(uint32_t word, FILE *output)
{
        assert(output != NULL);
        int i;
        for (i = WORD_SIZE / CHAR_SIZE - 1; i >= 0; i--) {
                fputc((char) Bitpack_getu(word, CHAR_SIZE, i * CHAR_SIZE), 
                                                                  output);
        }
}

/* 
 * reads a 32-bit word stored in the input file by reading the 4 characters
 * of the word; parameters: the input file
 */
uint32_t read_word(FILE *input)
{
        assert(input != NULL);
        unsigned i;
        unsigned lsb, byte_val;
        uint32_t word = 0;
        for (i = 0; i < WORD_SIZE/CHAR_SIZE; i++) {
                lsb = WORD_SIZE - (i + 1) * CHAR_SIZE;
                /* value of the 8-bit character to be inserted in the word */
                byte_val = (uint64_t) fgetc(input);
                assert(byte_val != (unsigned) EOF);
                word = Bitpack_newu((uint64_t) word, CHAR_SIZE, lsb, byte_val);
        }

        return (uint32_t) word;
}

/*
 * reads the header of the compressed file (in the required format)
 * anc creates an instace of the pnm interface that contains information
 * about the dimensions of the image
 * parameters: the input file, the methods "instance" to be used for the pnm
 * returns the pnm "instance" with an empty 2D array of pixels field
 */
Pnm_ppm read_header(FILE *input, A2Methods_T *methods)
{
        assert(input != NULL);
        assert(methods != NULL);
        unsigned height, width, size_of_rgb;
        int read = fscanf(input, "COMP40 Compressed image format 2\n%u %u",
                                                          &width, &height);
        assert(read == 2);
        int c = getc(input);
        assert(c == '\n');
        size_of_rgb = sizeof(struct Pnm_rgb);
        /* a new empty array of pixels */
        A2Methods_UArray2 array2 = (*methods)->new(width, height, size_of_rgb);
        Pnm_ppm pixmap = (Pnm_ppm) malloc(sizeof(struct Pnm_ppm));
        assert(pixmap != NULL);
        /* attribute the field values to the new pixmap */
        pixmap->width = width;
        pixmap->height = height;
        pixmap->denominator = 255;
        pixmap->pixels = array2;
        pixmap->methods = *methods;
        return pixmap;
} 


