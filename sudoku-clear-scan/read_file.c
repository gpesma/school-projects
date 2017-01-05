/*
 *    read_file.c
 *    by George Pesmazoglou, Alexandru Ungureanu
 *    September 27, 2016
 * 
 *    Implementation of the read_file interface.
 *        Uses the interface pnmrdr to read the bits in the file.
 */


#include <stdio.h>
#include <stdlib.h>
#include "bit2.h"
#include "read_file.h"
#include "pnmrdr.h"
#include "assert.h"
#define EXCEPT_INCLUDED

/*
 * helper function; throws errors if the file type is incorrect
 * parameters: a pointer to a pnm reader and the file to be read
 */
static void check_file_type(Pnmrdr_T *, FILE *);

/* helper function; responsible for reading each bit in the map and storing
 * the bits in a 2D bit array
 * parameters: a pointer to a pnm reader, pointer to the 2D bit array
 */
static void readmap(Pnmrdr_T *, Bit2_T *);


/*
 * checks if the file is a valid pbm, creates the 2D bit array and reads from
 * the file
 * parameters: the file to be read, pointer to the 2D bit array
 */
void read_file(FILE *fp, Bit2_T *map)
{
                Pnmrdr_T rdr;
                
                check_file_type(&rdr, fp);
                *map = Bit2_new(Pnmrdr_data(rdr).width, Pnmrdr_data(rdr).height);
                readmap(&rdr, map);
                Pnmrdr_free(&rdr);
}

static void readmap(Pnmrdr_T *rdr,Bit2_T *map)
{
                int input;
                for(unsigned i = 0; i < Pnmrdr_data(*rdr).height; i++){
                                for(unsigned j = 0; j < Pnmrdr_data(*rdr).width; j++) {
                                                TRY
                                                                input = Pnmrdr_get(*rdr);
                                                EXCEPT(Pnmrdr_Badformat)
                                                                fprintf(stderr, "Error: Bad format\n");
                                                exit(EXIT_FAILURE);                             
                                                EXCEPT(Pnmrdr_Count)
                                                                fprintf(stderr, "Error: Count error");
                                                exit(EXIT_FAILURE);     
                                    END_TRY;
                                    if(input != 0 && input != 1) {
                                                fprintf(stderr, "Error: incorrect input.");
                                                exit(EXIT_FAILURE);
                                    } 
                                    if(input != 0 && input != 1){
                                                fprintf(stderr, "Error: incorrect input.");
                                                exit(EXIT_FAILURE);
                                    } 
                                                Bit2_put(*map, j, i, input); 
                                }
                }
}

static void check_file_type(Pnmrdr_T *rdr, FILE *fp)
{
                TRY
                                *rdr = Pnmrdr_new(fp);
            EXCEPT(Pnmrdr_Badformat)
                        fprintf(stderr, "Error: bad format\n");
                        exit(EXIT_FAILURE);
            END_TRY;

            if(Pnmrdr_data(*rdr).type != 1) {
                        fprintf(stderr, "Error: wrong map type. \n");
                        exit(EXIT_FAILURE);
            }
}