/* 
 * transformations.h
 * by Georgios Pesmazoglou, Alexandru Ungureanu
 * October 7th, 2016
 *
 * Header file of the transformations interface.
 * The interface includes function that transform an image. The possible 
 * transformations are: rotating by a given angle (0, 90, 180 or 270), flipping
 * the image vertically or horizontally or transposing the image
 */

#include "a2methods.h"
#include "a2plain.h"
#include "a2blocked.h"
#include "pnm.h"
#include "string.h"

/*
 * rotates the given image by a given angle (0, 90, 180, 270) by using the given
 * map function to "traverse" the pixels, and records the total time of the
 * transformation and the average time per pixel in the file with the given name
 */
extern void rotate(Pnm_ppm pic, A2Methods_mapfun *map, int angle, char *file);

/*
 * flips the given image horizontally or vertically dependeing on the string 
 * given as parameter; uses the given map function to "traverse" the pixels, and
 * records the total time of the transformation and the average time per pixel 
 * in the file with the given name
 */
extern void flip(Pnm_ppm pic, A2Methods_mapfun *map, char* direction, 
                                                             char *file);

/*
 * transposes the given image by using the given map function to "traverse" the 
 * pixels and recors the total time of the transformation and the average time
 * per pixel in the file with the given name
 */
extern void transpose(Pnm_ppm pic, A2Methods_mapfun *map, char *file);