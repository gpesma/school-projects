/*
 *    readmap.c
 *    by George Pesmazoglou, Alexandru Ungureanu
 *    September 27, 2016
 * 
 *    Implementation of the readmap interface. 
 *    Uses the pnmrdr.h interface to read the grayscale file.
 *    
 */

#include <stdio.h>
#include <stdlib.h>
#include "uarray2.h"
#include "readmap.h"
#include "pnmrdr.h"
#include "assert.h"
#define EXCEPT_INCLUDED

/*
 * Parameters: Pointer to instance of Pnmrdr and file pointer
 * fails if file is in bad or wrong format
 */
static void check_file_type(Pnmrdr_T *, FILE *);

/* 
 * Helper function that passes the sudoku to the 2D array
 * Parameters: instace of Pnmrdr and pointer to 2D array
 * fails in case of badformat or input count is wrong
 */
static void read(Pnmrdr_T *, UArray2_T *);


void readmap(FILE *fp, UArray2_T *map)
{
                Pnmrdr_T rdr;
                check_file_type(&rdr,fp);
                read(&rdr, map);
                Pnmrdr_free(&rdr);
}


static void read(Pnmrdr_T *rdr,UArray2_T *map)
{
                int input;
                for(int i = 0; i < 9; i++){
                                for(int n = 0; n < 9; n++){
                                        /* throw errors if file in bad format or count failure */
                                        TRY
                                                        input = Pnmrdr_get(*rdr);

                                        EXCEPT(Pnmrdr_Badformat)
                                                        fprintf(stderr, "Error: Badformat");
                                                        exit (EXIT_FAILURE);

                                        EXCEPT(Pnmrdr_Count)
                                                        fprintf(stderr, "Error: Count error");
                                        exit(EXIT_FAILURE);  

                            END_TRY; 
                            /* sudoku numbers must be from 1 to 9 */
                                    assert(input > 0 && input < 10);
                                        *(int*) UArray2_at(*map,n,i) = input;
                                }
                }
}


static void check_file_type(Pnmrdr_T *rdr, FILE *fp)
{
                TRY
                                *rdr = Pnmrdr_new(fp);
            EXCEPT(Pnmrdr_Badformat)
                        printf("bad format\n");
                        exit(EXIT_FAILURE);
            END_TRY;

            if(Pnmrdr_data(*rdr).type != 2) {
                    /* not correct file type */
                        printf("data type \n");
                        exit(EXIT_FAILURE);
            }
}