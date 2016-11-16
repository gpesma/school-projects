/*
 * floatIntScale.c
 * Implementation (source file) of the floatIntScale interface
 * October 21st, 2016
 * Georgios Pesmazoglou, Alexandru Ungureanu
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <math.h>
#include "bitpack.h"
#include "floatIntScale.h"
#include <stdbool.h>
#include "assert.h"

/*
 * rounds the floating-point number given as parameter to the closest integer
 * returns the closest integer to the value
 */
static int round_value(float);

/*
 * computes the scale appropriate for a floating-point interval to be scaled
 * to an integer interval (with a maximum number of bits)
 * parameters: - the bound of the floating-point interval (maximum absolute
 *               value of a number in the interval
 *             - the size of an integer in the new(scaled) interval
 *             - a boolean that is true if the scaled interval is for unsigned
 *               integers or false if it is for scaled integer
 * returns the scale; checked runtime error if bound is not positive
 */               
static float get_scale(float, unsigned, bool);


/*
 * scales a floating-point value to an unsigned integer of specific size
 * parameters: - the floating-point value to be scaled
 *             - the current bound of the floating-point number
 *             - the size the scaled integer should have (in bits)
 * returns the scaled unsigned integer
 * checked runtime error if bound is not positive or size is 0
 */
uint64_t scale_to_unsigned(float x, float bound, unsigned size)
{
        assert(size != 0);
        float scale = get_scale(bound, size, true);
        return round_value(scale * x);
}

/*
 * scales a floating-point number to a signed integer of specific size
 * parameters: - the floating-point value to be scaled
 *             - the current bound of the floating-point number
 *             - the size the scaled integer should have
 * returns the scaled signed integer
 * checked runtime error if bound is not positive or size is 0
 */
int64_t scale_to_signed(float x, float bound, unsigned size)
{
        assert(size != 0);
        float scale = get_scale(bound, size, false);
        return round_value(scale * x);
}

/*
 * unscales an unsigned integer to a floating-point number
 * parameters: - the unsigned integer to be unscaled
 *             - the bound of the new (unscaled) value (largest absolute
 *               value it can have)
 *             - the size in bits of the unsigned integer
 * returns the floating-point unscaled value (can't be negative)
 * checked runtime error if bound is not positive or size is 0
 */
float unscale_unsigned(uint64_t n, float bound, unsigned size)
{
        assert(size != 0);
        float scale = get_scale(bound, size, true);
        return (n / scale);
}

/*
 * unscales a signed integer to a floating-point number
 * parameters: - the signed integer to be unscaled
 *             - the bound of the new (unscaled) value 
 *             - the size in bits of the signed integer
 * returns the floating-point unscaled value
 * checked runtime error if bound is not positive or size is 0
 */
float unscale_signed(int64_t n, float bound, unsigned size)
{
        assert(size != 0);
        float scale = get_scale(bound, size, false);
        return (n / scale);
}

float get_scale(float bound, unsigned size, bool is_unsigned) 
{
        assert(bound > 0);
        float new_bound;
        /* compute the new (integer) bound */
        if (is_unsigned == true) {
                new_bound = pow(2, (float)size) - 1;  
        } 
        else {
                new_bound = pow(2, (float) (size - 1)) - 1;
        }
        return (new_bound / bound);
}

int round_value(float x)
{
        if (x - floor(x) < 0.5) {
                return floor(x);
        }
        else {
                return ceil(x);
        }
}

