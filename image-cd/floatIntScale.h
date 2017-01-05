/*
 * floatIntScale.h
 * Header file of the floatIntScale interface
 * October 21st, 2016
 * Georgios Pesmazoglou, Alexandru Ungureanu
 *
 * Interface responsible for scaling floating-point numbers to signed/unsigned
 * integers of a specific size in bits. Also used to unscale signed/unsigned
 * integers to floating-point numbers in specific bounds
 */


#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

/*
 * scales a floating-point number to an unsigned integer given the bound 
 * (maximum absolute value in the number's range) and the size in bits of the 
 * new unsigned integer to be created
 * checked runtime error if the bound is not positive or size is 0
 */
extern uint64_t scale_to_unsigned(float x, float bound, unsigned size);

/*
 * scales a floating-point number to a signed integer given the bound 
 * (maximum absolute value in the number's range) and the size in bits of the 
 * new signed integer to be created
 * checked runtime error if the bound is not positive or size is 0
 */
extern int64_t scale_to_signed(float x, float bound, unsigned size);

/*
 * unscales an unsigned integer to a floating-point number givent the bound
 * the floating-point number will have (for instance the [0, 0.5] interval 
 * is depicted by bound == 0.5) and the size of the unsigned integer in bits 
 * checked runtime error if the bound is not positive or size is 0
 */
extern float unscale_unsigned(uint64_t n, float bound, unsigned size);

/*
 * unscales a signed integer to a floating-point number givent the bound
 * the floating-point number will have (for instance the [-0.7, 0.7] interval 
 * is depicted by bound == 0.7) and the size of the signed integer in bits 
 * checked runtime error if the bound is not positive or size is 0
 */
extern float unscale_signed(int64_t n, float bound, unsigned size);