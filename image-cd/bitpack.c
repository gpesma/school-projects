/*
 * bitpack.c
 * Implementation (source file) of the bitpack interface
 * October 21st, 2016
 * Georgios Pesmazoglou, Alexandru Ungureanu
 */

#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include "except.h"
#include "bitpack.h"
#include "assert.h"

Except_T Bitpack_Overflow = {"Overflow packing bits"};
const unsigned WORD_BITS = 64;


/*
 * public method that checks if a 64-bits unsigned integer can be represented
 * with at most "width" bits
 * parameters: the unsigned integer, the width
 * returns true if it can be represented or false otherwise
 */
bool Bitpack_fitsu(uint64_t n, unsigned width)
{
        /* 2 ^ width - 1 is the biggest width-bit unsigned integer */
        if (n < ((uint64_t) 1 << width)) {
                return true;
        }
        else {
                return false;
        }
}

/*
 * public method that checks if a 64-bits signed integer can be represented
 * with at most "width" bits
 * parameters: the signed integer, the width
 * returns true if it can be represented or false otherwise
 */
bool Bitpack_fitss(int64_t n , unsigned width)
{
        /* 
         * signed width-bit integers are between -(2 ^ (width - 1)) and 
         * 2 ^ (width - 1) - 1
         */
        if (n >= -(int64_t) ((uint64_t) 1 << (width - 1)) &&
                  n < (int64_t) ((uint64_t) 1 << (width - 1))) {
                return true;
        }
        else {
                return false;
        }
}

/*
 * public method that gets extract specifics bits from a 64-bit unsigned integer
 * parameters: the unsigned integer (word), the number of bits to be extracted, 
 *             the least significant bit (position within the initial word where
 *             the sequence of bits to be extracted ends)
 * returns the unsigned integer that corresponds to the extracted bits sequence
 */
uint64_t Bitpack_getu(uint64_t word, unsigned width, unsigned lsb)
{
        assert(width <= WORD_BITS);
        assert(lsb + width <= WORD_BITS);
        if (width == 0) {
                return 0;
        }
        uint64_t mask = ~0;
        mask = mask >> (WORD_BITS - width) << lsb;
        return (word & mask) >> lsb;
}

/*
 * public method that gets extract specifics bits from a 64-bit unsigned integer
 * parameters: the unsigned integer (word), the number of bits to be extracted, 
 *             the least significant bit (position within the initial word where
 *             the sequence of bits to be extracted ends)
 * returns the signed integer that corresponds to the extracted bits sequence
 */
int64_t Bitpack_gets(uint64_t word, unsigned width, unsigned lsb)
{
        /* get the sequence to be extracted as an unsigned integer at first */
        uint64_t bits = Bitpack_getu(word, width, lsb);
        int64_t mask;
        /* if "bits" is negative, it starts with a 1, else it starts with a 0 */
        if (bits >= (uint64_t) (1 << (width - 1))) {
                mask = (~0) << width;
                return (mask | bits);
        }
        else {
                return (int64_t) bits;
        }
}

/*
 * replaces a specific sequence of bits within a 64-bit unsigned integer with
 * another sequence of bits of the same width that corresponds to another 
 * unsigned integer
 * parameters: the 64-bit word to be updated, the number of bits to be 
 *             replaced, the least significant bit (position within the initial
 *             word where the sequence of bits to be replaced ends), and the
 *             unsigned value that corresponds to the sequence of bits to 
 *             be added
 * returns the updated 64-bits unsigned integer
 */
uint64_t Bitpack_newu(uint64_t word, unsigned width, unsigned lsb, uint64_t 
                                                                         value)
{
        assert(width <= WORD_BITS);
        assert(width + lsb <= WORD_BITS);
        if (Bitpack_fitsu(value, width) == false) {
                RAISE(Bitpack_Overflow);
                exit (EXIT_FAILURE);
        }
        if (width == 0) {
                return word;
        }
        uint64_t mask = ~0;
        mask = mask >> (WORD_BITS - width) << lsb;
        mask = ~mask;
        return ((word & mask) | (value << lsb)); 
}

/*
 * replaces a specific sequence of bits within a 64-bit unsigned integer with
 * another sequence of bits of the same width that corresponds to a signed 
 * integer
 * parameters: the 64-bit word to be updated, the number of bits to be 
 *             replaced, the least significant bit (position within the initial
 *             word where the sequence of bits to be replaced ends), and the
 *             signed value that corresponds to the sequence of bits to be added
 * returns the updated 64-bits unsigned integer
 */
uint64_t Bitpack_news(uint64_t word, unsigned width, unsigned lsb, int64_t 
                                                                        value)
{
        assert(width <= WORD_BITS);
        assert(width + lsb <= WORD_BITS);
        if (Bitpack_fitss(value, width) == false) {
                RAISE(Bitpack_Overflow);
                exit (EXIT_FAILURE);
        }
        if (width == 0) {
                return word;
        }

        uint64_t mask = ~0;
        mask = mask >> (WORD_BITS - width) << lsb;
        mask = ~mask;

        /* the second mask has 1-bits from least significant bit to the end */
        uint64_t mask_for_value = ~0;
        mask_for_value = mask_for_value >> (WORD_BITS - width);
        value = (value & mask_for_value);
 
        return ((word & mask) | (value << lsb));    
}

