/*
 *    verify_condition.c
 *    by George Pesmazoglou, Alexandru Ungureanu
 *    September 27, 2016
 * 
 *    Implementation of the verify_condition interface    
 */

#include <stdio.h>
#include <stdlib.h>
#include "verify_condition.h"
#include "assert.h"


/*
 * Parameter: static int array of size 9
 * returns 0 if condition verified or 1 otherwise
 */
int verify_condition(int array[])
{
                for(int i = 0; i < 8; i++){
                                for(int n = i+1 ; n < 9 ; n++) {
                                                if(array[i] == array[n]) {
                                                                return 1;
                                                }
                                }
                }
                return 0;
}