/*
 *    verify_condition.h
 *    by George Pesmazoglou, Alexandru Ungureanu
 *    September 27, 2016
 * 
 *    Header file of the verify_condition interface. 
 *    The interface is responsible for verifying if an array of 9 numbers
 *    verifies the condition that each 2 elements are distinct.
 *    
 */

#include <stdio.h>
#include <stdlib.h>

/*
 * This function takes in as  a parameter an int array
 * of length 9(number of digits in a row, col or 3X3 square)
 * and checks if there is a number twice
 * if there is returns 1 - unsolved 
 * else returns 0 - solved 
 */
extern int verify_condition(int array[]);