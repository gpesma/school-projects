/*
 * compress40.c
 * Implementation (source file) of the compress40 interface
 * October 21st, 2016
 * Georgios Pesmazoglou, Alexandru Ungureanu
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include "compress40.h"
#include "pnm.h"
#include "bitpack.h"
#include "assert.h"
#include "compressed_io.h"
#include "pixConversions.h"
#include "a2methods.h"
#include "a2plain.h"
#include "floatIntScale.h"
#include "arith40.h"

FILE *fp;
/*
 * structure of pixels' coefficients to be encoded in a word
 */
struct components {
        float a;
        float b;
        float c;
        float d;
        float pb_avg;
        float pr_avg;
};

/*
 * closure to be passed in an apply function containing a pointer to a file,
 * a pixmap, and a pointer to the methods "instance" to be used for the pixmap
 */
struct closure{
        FILE *fp;
        Pnm_ppm pic;
        A2Methods_T *methods;
};

/* 
 * static array that holds the sizes of the a, b, c, d cosine coefficients, and
 * Pb_avg, Pr_avg average chroma values within a 32-bit code word
 * a, b, c, d, Pb_avg, Pr_avg sizes correspond to width[0], width[1], width[2],
 * width[3], width[4] and width[5] respectively
 */
static const unsigned WIDTHS[6] = {6, 6, 6, 6, 4, 4};

/* 
 * static array that holds the least significant bits of the a, b, c, d cosine 
 * coefficients, and Pb_avg, Pr_avg average chroma values within a 32-bit word
 * the least significant bit positions of a, b, c, d, Pb_avg, Pr_avg correspond 
 * to width[0], width[1], width[2], width[3], width[4] and width[5] respectively
 */
static const unsigned LSBS[6] = {26, 20, 14, 8, 4, 0};

static const unsigned RGB_SCALE = 255;               /* RGB color space scale */
const float A_RANGE_MAX = 1; /* the maximum absolute value of "a" coefficient */
const float BCD_RANGE_MAX = 0.3; /* the max. absolute value of b, c, d coeffs */

/* 
 * private "apply" function; takes a 2x2 block of pixels and transforms it
 * to a word, compressing the square; the word is written on standard output
 * parameters: column and row of current element, the array of pixels, a pointer
 * to the current element, and the closure pointer
 */
static void compress_block(int, int, A2Methods_UArray2, void *, void *);

/*
 * private function that "cuts" the last row/column if necessary in order
 * for the image to have even width and height values
 * parameter: the pixmap of the image
 */
static void trim_pic(Pnm_ppm);

/* 
 * private "apply" function; takes the next unread word in the compressed file
 * and converts it to a 2x2 block of RGB pixels, decompressing the image; the 
 * pixels are added to the given array of pixels of the pixmap
 * parameters: the current element's colum and row, the array of pixels, a 
 * pointer to the current element, and the closure pointer
 */
static void decompress_block(int, int, A2Methods_UArray2, void *, void *);

/* 
 * private function used to encode a structure of components into a word
 * parameter: the structure of components: a, b, c, d, Pb_avg, Pr_avg
 * returns a 32-bit word that encodes pixel information 
 * checked runtime error if the coefficients don't have the required size
 */
static uint32_t encode_word(struct components);

/* 
 * private function used to decode a 32-bit word into cosine coefficients and 
 * average chroma values; parameter: the 32-bit word
 * returns a structure of pixels coefficietns (decoded from the word)
 */
static struct components decode_word(uint32_t);



/*
 * public method; "controller" function of the compression process
 * parameter: the input file of the image to be compressed (ppm format)
 * the function reads the ppm image, and compresses the image, printing
 * the new, compressed image, on standard output
 */
void compress40(FILE *input)
{
	fp = fopen("compressed.txt", "w+");
        assert(input != NULL);
        A2Methods_T methods = uarray2_methods_plain;
        Pnm_ppm pic = Pnm_ppmread(input, methods);
        trim_pic(pic);
        print_header(pic->width, pic->height, fp);
        /* "traverse" the array of pixels, compressing each 2x2 square */
        methods->map_row_major(pic->pixels, compress_block, pic);
        Pnm_ppmfree(&pic);
        fclose(fp);
}

/*
 * public method; "controller" function of the decompression process
 * parameter: the input file of the image to be decompressed (format in spec)
 * the function reads the compressed image, and decompresses the image, printing
 * the new, decompressed image (ppm) on standard output
 */
void decompress40(FILE *input)
{
        assert(input != NULL);
        A2Methods_T methods = uarray2_methods_plain;
        Pnm_ppm pic = read_header(input, &methods);
        struct closure cl = {input, pic, &methods};
        /* "traverse" the array of pixels, decompressing each word to a 2x2 */
        methods->map_row_major(pic->pixels, decompress_block, &cl); 
        Pnm_ppmwrite(stdout, pic);
        Pnm_ppmfree(&pic);
}


void compress_block(int col, int row, A2Methods_UArray2 array2, void *elem, 
                                                                       void *cl)
{
        /* the pixmap of the image to be compressed */
        Pnm_ppm pic = (Pnm_ppm) cl;
        /*
         * if the element has position [x, y] (even values), decompress the
         * word and compress the 4 pixels [x, y], [x + 1, y], 
         * [x, y + 1], [x + 1, y + 1] (2 x 2 square of pixels)
         */
        if(col % 2 == 1 || row % 2 == 1 || (unsigned) col >= pic->width || 
                                              (unsigned) row >= pic->height) {
                return;
        }
        struct Pnm_rgb pix1 = *(struct Pnm_rgb *) elem;
        struct Pnm_rgb pix2 = *((struct Pnm_rgb*)pic->methods->at(
                                                     array2, col + 1, row));
        struct Pnm_rgb pix3 = *((struct Pnm_rgb*)pic->methods->at(
                                                     array2, col, row + 1));
        struct Pnm_rgb pix4 = *((struct Pnm_rgb*)pic->methods->at(
                                                  array2, col + 1,row + 1));
        /* convert from RGB to video component color space */
        struct video comp1 = RGB_to_video(pix1, pic->denominator);
        struct video comp2 = RGB_to_video(pix2, pic->denominator);
        struct video comp3 = RGB_to_video(pix3, pic->denominator);
        struct video comp4 = RGB_to_video(pix4, pic->denominator);
        /* scale the chroma values to average scaled chroma values */
        float pb_avg = (comp1.pb + comp2.pb + comp3.pb + comp4.pb) / 4;
        float pr_avg = (comp1.pr + comp2.pr + comp3.pr + comp4.pr) / 4;
        struct intensities ys = {comp1.y, comp2.y, comp3.y, comp4.y};
        /* convert intensities to cosine coefficients */
        struct cosines cos = Y_to_cos(ys);

        struct components values = {cos.a, cos.b, cos.c, cos.d, pb_avg, pr_avg};
        /* encode the values in a word and write it in the file */
        print_word(encode_word(values), fp);

}

void decompress_block(int col, int row, A2Methods_UArray2 array2, void *elem, 
                                                                       void *cl)
{
        struct closure my_cl = *(struct closure*) cl;
        /* the pixmap to be updated by decompressing the compressed file */
        Pnm_ppm pic = my_cl.pic;
        /* the output file */
        FILE* fp = my_cl.fp;
        /* the methods "instance" to be used for the pixmap */
        A2Methods_T mets = *(my_cl.methods);
        /*
         * if the element has position [x, y] (even values), decompress the
         * word and compute the RGB values of pixels [x, y], [x + 1, y], 
         * [x, y + 1], [x + 1, y + 1] (2 x 2 square of pixels)
         */
        if(col % 2 == 1 || row % 2 == 1 || (unsigned) col >= pic->width || 
                                            (unsigned) row >=   pic->height) {
                return;
        }
        uint32_t word = read_word(fp);
        struct components values = decode_word(word);
        
        struct cosines cos = {values.a, values.b, values.c, values.d};
        struct intensities ys = cos_to_Y(cos);
        /* the video component-coefficients of the 4 pixels */
        struct video comp1 = {ys.y1, values.pb_avg, values.pr_avg};
        struct video comp2 = {ys.y2, values.pb_avg, values.pr_avg};
        struct video comp3 = {ys.y3, values.pb_avg, values.pr_avg};
        struct video comp4 = {ys.y4, values.pb_avg, values.pr_avg};
        
        *(struct Pnm_rgb*) elem = video_to_RGB(comp1, RGB_SCALE);
        *(struct Pnm_rgb*) (mets->at(array2, col + 1, row)) = 
                                  video_to_RGB(comp2, RGB_SCALE);   
        *(struct Pnm_rgb*) (mets->at(array2, col, row + 1)) = 
                                  video_to_RGB(comp3, RGB_SCALE);
        *(struct Pnm_rgb*) (mets->at(array2, col + 1, row + 1)) = 
                                  video_to_RGB(comp4, RGB_SCALE);
}


void trim_pic(Pnm_ppm pic)
{
        assert(pic != NULL);
        if(pic->height % 2 == 1) {
                pic->height--;
        }
        if(pic->width % 2 == 1) {
                pic->width--;
        }
}

uint32_t encode_word(struct components values)
{
        uint64_t a, pb_avg, pr_avg;
        int64_t b, c, d;
        uint32_t word = 0;
        
        /* 
         * scale each component to the approriate size it needs to have within
         * the code word
         */
        a = scale_to_unsigned(values.a, A_RANGE_MAX, WIDTHS[0]);
        b = scale_to_signed(values.b, BCD_RANGE_MAX, WIDTHS[1]);
        c = scale_to_signed(values.c, BCD_RANGE_MAX, WIDTHS[2]);
        d = scale_to_signed(values.d, BCD_RANGE_MAX, WIDTHS[3]);
        pb_avg = (uint64_t) Arith40_index_of_chroma(values.pb_avg); 
        pr_avg = (uint64_t) Arith40_index_of_chroma(values.pr_avg);

        /* 
         * use the bitpack public methods to insert each scaled coefficient
         * in the appropriate position in the code word
         */
        word = Bitpack_newu(word, WIDTHS[0], LSBS[0], a);
        word = Bitpack_news(word, WIDTHS[1], LSBS[1], b);
        word = Bitpack_news(word, WIDTHS[2], LSBS[2], c);
        word = Bitpack_news(word, WIDTHS[3], LSBS[3], d);
        word = Bitpack_newu(word, WIDTHS[4], LSBS[4], pb_avg);
        word = Bitpack_newu(word, WIDTHS[5], LSBS[5], pr_avg); 
        return word;   
}

struct components decode_word(uint32_t word)
{
        uint64_t a, pb_avg, pr_avg;
        int64_t b, c, d;
        struct components comps;

        
        /* exctract each scaled coefficient from the appropriate position */
        a = Bitpack_getu(word, WIDTHS[0], LSBS[0]);
        b = Bitpack_gets(word, WIDTHS[1], LSBS[1]);
        c = Bitpack_gets(word, WIDTHS[2], LSBS[2]);
        d = Bitpack_gets(word, WIDTHS[3], LSBS[3]);
        pb_avg = Bitpack_getu(word, WIDTHS[4], LSBS[4]);
        pr_avg = Bitpack_getu(word, WIDTHS[5], LSBS[5]);
              
        /*
         * unscale each coefficient to get the actual values of the block's
         * cosine coefficients and average chroma value
         */
        comps.a = unscale_unsigned(a, A_RANGE_MAX, WIDTHS[0]);
        comps.b = unscale_signed(b, BCD_RANGE_MAX, WIDTHS[1]);
        comps.c = unscale_signed(c, BCD_RANGE_MAX, WIDTHS[2]);
        comps.d = unscale_signed(d, BCD_RANGE_MAX, WIDTHS[3]);
        comps.pb_avg = Arith40_chroma_of_index(pb_avg);
        comps.pr_avg = Arith40_chroma_of_index(pr_avg);
        return comps;
}
