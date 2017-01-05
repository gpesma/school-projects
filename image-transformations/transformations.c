/* 
 * transformations.c
 * by Georgios Pesmazoglou, Alexandru Ungureanu
 * October 7th, 2016
 *
 * Implementation of the transformations interface.
 */


#include "transformations.h"
#include <string.h>
#include <stdlib.h>
#include "a2methods.h"
#include "pnm.h"
#include <string.h>
#include "cputiming.h"

/* 
 * helper function used as apply function in a map to update each pixel so
 * that the array is rotated by 90 degrees
 * parameters: the column and row positions of the cell, the 2D unboxed array
 * a pointer to the element in the array, the closure pointer
 */
static void rotate90(int, int, A2Methods_UArray2, void *, void *);

/* 
 * helper function used as apply function in a map to update each pixel so
 * that the array is rotated by 180 degrees
 * parameters: the column and row positions of the cell, the 2D unboxed array
 * a pointer to the element in the array, the closure pointer
 */
static void rotate180(int, int, A2Methods_UArray2, void *, void *);

/* 
 * helper function used as apply function in a map to update each pixel so
 * that the array is rotated by 270 degrees
 * parameters: the column and row positions of the cell, the 2D unboxed array
 * a pointer to the element in the array, the closure pointer
 */
static void rotate270(int, int, A2Methods_UArray2, void *, void *);

/* 
 * helper function used as apply function in a map to update each pixel so
 * that the array is transposed
 * parameters: the column and row positions of the cell, the 2D unboxed array
 * a pointer to the element in the array, the closure pointer
 */
static void trans(int, int, A2Methods_UArray2, void *, void *);

/* 
 * helper function used as apply function in a map to update each pixel so
 * that the array is flipped horizontally
 * parameters: the column and row positions of the cell, the 2D unboxed array
 * a pointer to the element in the array, the closure pointer
 */
static void flip_horizontal(int, int, A2Methods_UArray2, void *, void *);

/* 
 * helper function used as apply function in a map to update each pixel so
 * that the array is flipped vertically
 * parameters: the column and row positions of the cell, the 2D unboxed array
 * a pointer to the element in the array, the closure pointer
 */
static void flip_vertical(int, int, A2Methods_UArray2, void *, void *);

/* 
 * helper function that initializes a new array from a ppm picture given as
 * parameter; if the integer parameter is 1, the new array needs to have the
 * values for width and height switched (from the original array in the image) 
 * or 0 if the new array will have the same dimensions as the one in the image
 * returns the newly created 2D unboxed array
 */ 
static A2Methods_UArray2 init_new_array(Pnm_ppm, int);

/*
 * helper function that modifies the given ppm image given as parameter to 
 * replace its array of pixels with the 2D array given as parameter
 */
static void update_pic(Pnm_ppm, A2Methods_UArray2);

/*
 * responsible for writing the timing of a transformation in a file
 * parameters: the ppm picture, the total time of the transformation (double),
               the name of the file used for output
 */ 
static void report_time(Pnm_ppm, double, char *);

/*
 * flip: flips an image
 * parameters: the ppm picture, the map method to be used for "traversing"
 * the pixels array, the type of the flip-transformation (can be "vertical" or 
 * "horizontal") and the name of the file used for writing the timing
 */
void flip(Pnm_ppm pic, A2Methods_mapfun* map, char* direction, char* file)
{
        /* new array that will represent the flipped array (same dimensions) */
        A2Methods_UArray2 flipped = init_new_array(pic, 0);
        CPUTime_T timer = CPUTime_New();
        CPUTime_Start(timer);
        if(strcmp(direction,"vertical") == 0) {
                map(flipped, flip_vertical , pic);
        } 
        else {
                map(flipped, flip_horizontal, pic);
        }
        double time_used = CPUTime_Stop(timer);
        CPUTime_Free(&timer);
        /* update the image's pixel array to be the flipped one */
        update_pic(pic, flipped);
        if (file != NULL) {
                /* write the timing information */
                report_time(pic, time_used, file);
        }
}

/*
 * rotate: rotates an image
 * parameters: the ppm picture, the map method to be used for "traversing"
 * the pixels array, the angle of the rotation (can be 0, 90, 180 or 270) and 
 * the name of the file used for writing the timing
 */
void rotate(Pnm_ppm pic, A2Methods_mapfun* map, int angle, char* file)
{
        if (angle == 0) {
                /* rotation by 0 means the image stays the same */
                return;
        }

        CPUTime_T timer = CPUTime_New();

        A2Methods_UArray2 rotated;
        if (angle == 180) {
                /* new array that will represent the flipped array 
                                               same dimensions) */
                rotated = init_new_array(pic, 0);
                CPUTime_Start(timer);
                map(rotated, rotate180, pic);
        }
        else {
                /* new array that will represent the flipped array 
                                           (switched dimensions) */
                rotated = init_new_array(pic, 1);
                CPUTime_Start(timer);
                if (angle == 90) {
                        map(rotated, rotate90, pic);
                }
                else {
                        map(rotated, rotate270, pic);
                }
        }
        double time_used = CPUTime_Stop(timer);
        CPUTime_Free(&timer);
        /* update the image's pixel array to be the rotated one */
        update_pic(pic, rotated);
        if (file != NULL) {
                /* write the timing information */
                report_time(pic, time_used, file);
        }
}

/*
 * transpose: transposes an image
 * parameters: the ppm picture, the map method to be used for "traversing"
 * the pixels array and the name of the file used for writing the timing
 */
void transpose(Pnm_ppm pic, A2Methods_mapfun *map, char* file)
{
        /* new array that will represent the transposed array 
                                     (switched dimensions) */
        A2Methods_UArray2 transposed = init_new_array(pic, 1);
        CPUTime_T timer = CPUTime_New();
        CPUTime_Start(timer);
        map(transposed, trans, pic);
        double time_used = CPUTime_Stop(timer);
        CPUTime_Free(&timer);
        /* update the image's pixel array to be the rotated one */
        update_pic(pic, transposed);
        if (file != NULL) {
                /* write the timing information */
                report_time(pic, time_used, file);
        }

}

/* implementation of the helper functions */

void trans(int i, int j, A2Methods_UArray2 array2, 
                                void *elem, void *cl)
{
                (void) array2;
                Pnm_ppm pic = cl;
                const struct A2Methods_T *mets = pic->methods;
                *(Pnm_rgb) elem = *(Pnm_rgb) mets->at(pic->pixels, j, i);
}

A2Methods_UArray2 init_new_array(Pnm_ppm pic, int reversed)
{
        const struct A2Methods_T *mets = pic->methods;
        int height = mets->height(pic->pixels);
        int width = mets->width(pic->pixels);
        int size = mets->size(pic->pixels);
        if (reversed == 1) {
                int temp = height;
                height = width;
                width = temp;
        }
        A2Methods_UArray2 modified = mets->new(width, height ,size);
        return modified;
}

void update_pic(Pnm_ppm pic, A2Methods_UArray2 modified)
{
        const struct A2Methods_T *mets = pic->methods;
        int height = mets->height(modified);
        int width = mets->width(modified);
        mets->free(&(pic->pixels));
        pic->height = (unsigned) height;
        pic->width = (unsigned) width;
        pic->pixels = modified;
}


void rotate90(int i, int j, A2Methods_UArray2 array2, void *elem, void *cl)
{ 
        (void) array2;
        Pnm_ppm pic = cl;
        const struct A2Methods_T *mets = pic->methods;
        int height = mets->height(pic->pixels);
        *(Pnm_rgb) elem = *(Pnm_rgb) mets->at(pic->pixels, j, height- i - 1);
}

void rotate270(int i, int j, A2Methods_UArray2 array2, void *elem, void *cl)
{ 
        (void) array2;
        Pnm_ppm pic = cl;
        const struct A2Methods_T *mets = pic->methods;
        int width = mets->width(pic->pixels);
        *(Pnm_rgb) elem = *(Pnm_rgb) mets->at(pic->pixels, width - j - 1, i);
}

void rotate180(int i, int j, A2Methods_UArray2 array2, void *elem, void *cl)
{
        (void) array2;
        Pnm_ppm pic = cl;
        const struct A2Methods_T *mets = pic->methods;
        int w = mets->width(array2);
        int h = mets->height(array2);
        *(Pnm_rgb) elem = *(Pnm_rgb) mets->at(pic->pixels, w - i - 1,
                                                           h - j - 1);
}


void flip_horizontal(int i, int j, A2Methods_UArray2 array2, void *elem, 
                                                                   void *cl)
{
        (void) array2;
        Pnm_ppm pic = cl;
        const struct A2Methods_T *mets = pic->methods;
        int width = mets->width(array2);
        *(Pnm_rgb) elem = *(Pnm_rgb) mets->at(pic->pixels, width - i - 1, j);
}


void flip_vertical(int i, int j, A2Methods_UArray2 array2, void *elem, void *cl)
{
        (void) array2;
        Pnm_ppm pic = cl;
        const struct A2Methods_T *mets = pic->methods;
        int height = mets->height(array2);
        *(Pnm_rgb) elem = *(Pnm_rgb) mets->at(pic->pixels, i , height - j -1);
}

/*
 * Number of instructions are the same as the time in nanoseconds
 * per pixel as in one seconds 1,000,000,000 instructions
 */
void report_time(Pnm_ppm pic, double total_time, char* file)
{
        FILE *opFile = fopen(file, "a");
        const struct A2Methods_T *mets = pic->methods;
        double num_pixels = (mets->width(pic->pixels)) *
                                (mets->height(pic->pixels));
        double time_per_pix = total_time / num_pixels;
        fprintf(opFile, "Total time of transformation: %0.5f\n", total_time);
        fprintf(opFile, "Average time per pixel: %0.5f\n", time_per_pix);
        fprintf(opFile, "num of instructions: %0.0f\n", time_per_pix);
        fclose(opFile);
}