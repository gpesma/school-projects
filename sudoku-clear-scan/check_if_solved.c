/*
 *    readmap.c
 *    by George Pesmazoglou, Alexandru Ungureanu
 *    September 27, 2016
 * 
 *    Implementation of the check_if_solved interface. 
 */

#include <stdio.h>
#include <stdlib.h>
#include "uarray2.h"
#include "check_if_solved.h"
#include "readmap.h"
#include "verify_condition.h"
#include "assert.h"

static const int SIDE_LENGTH = 9;

static int check_rows(UArray2_T *);
static int check_cols(UArray2_T *);
static int check_squares(UArray2_T *);
static int square_helper(UArray2_T *, int, int);
/*
 * Parameter: file pointer
 * returns 0 if sudoku is solved, 1 otherwise
 * fails if 2D array not allocated correctly
 */
int check_if_solved(FILE *fp)
{
        
                UArray2_T game_map = UArray2_new(SIDE_LENGTH,SIDE_LENGTH,sizeof(int));
                readmap(fp,&game_map);
                assert(UArray2_height(game_map) == SIDE_LENGTH);
                assert(UArray2_width(game_map) == SIDE_LENGTH);

                /* if rows, columns and 3 x 3 squares verify the condition, solved */
                if((check_rows(&game_map) != 0) || 
                                (check_cols(&game_map) != 0) ||
                                (check_squares(&game_map) != 0)){
                                UArray2_free(&game_map);
                                return 1;
                }
                UArray2_free(&game_map);
                return 0;
}

/*
 * Helper function, checks if rows verify the condition
 * Parameter: 2D array
 * returns 0/1
 */
static int check_rows(UArray2_T *game_map)
{
                int row_array[SIDE_LENGTH];
                for (int i = 0; i < UArray2_height(*game_map); i++) {
                                for (int j = 0; j < UArray2_width(*game_map); j++) {
                                                row_array[j] = *(int*) UArray2_at(*game_map, j, i);
                                }
                                if (verify_condition(row_array) != 0) {
                                                return 1;
                                }
                }
                return 0;
}

/*
 * Helper function, checks cols
 * Parameter: 2D array
 * returns 0/1
 */
static int check_cols(UArray2_T *game_map)
{
                int col_array[SIDE_LENGTH];

                for (int i = 0; i < UArray2_width(*game_map); i++) {
                                for (int j = 0; j < UArray2_height(*game_map); j++) {
                                                col_array[j] = *(int*) UArray2_at(*game_map, i, j);
                                }
                                if (verify_condition(col_array) != 0) {
                                                return 1;
                                }
                }
                return 0;
}

/*
 * Helper function, checks 3X3 squares
 * Parameter: 2D array
 * returns 0/1
 * loops are incremented by 3 as we use the coordinates
 * of the upper right value of the 3X3 squares
 */
static int check_squares(UArray2_T *game_map)
{
                for(int i = 0; i < SIDE_LENGTH; i += 3) {
                                for(int j = 0; j < SIDE_LENGTH ; j += 3) {
                                                if(square_helper(game_map, i, j) != 0) {
                                                                return 1;
                                                }
                                }
                }
                return 0;
}

/*
 * check squares sends in as a parameter
 * position of the upper right number that corresponds
 * to the square that has to be checked
 * this function created the square 3X3 and checks if it is solved
 */
static int square_helper(UArray2_T *game_map, int start_col,int start_row)
{
                int sq_array[SIDE_LENGTH];
                int count = 0;
                for(int n = start_row; n < (start_row + 3); n++) {
                                for(int i = start_col; i < (start_col + 3); i++) {
                                                sq_array[count++] = *(int*) UArray2_at(*game_map, i, n);
                                }
                }
                if (verify_condition(sq_array) != 0) {
                                return 1;
                }
                return 0;
}