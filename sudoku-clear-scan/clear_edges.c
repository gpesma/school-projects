/*
 *    clear_edges.c
 *    by George Pesmazoglou, Alexandru Ungureanu
 *    September 27, 2016
 * 
 *        Implementation of the clear_edges interface. 
 *    Representations used:
 *   
 *    1. A 2D bit array that stores the pbm bits.
 *    2. A Hanson Stack used to find the black edge bits.
 *    3. A struct used to store the position of a bit (fields row and col) 
 */

#include <stdio.h>
#include <stdlib.h>
#include "bit2.h"
#include "read_file.h"
#include "write_map.h"
#include "assert.h"
#include "clear_edges.h"
#include "stack.h"

/*
 * unblacks all the black edge pixels that can be reached through a "path" 
 * starting from a specific bit. A "path" of bits is a sequence of bits in
 * each 2 consecutive bits are neighbors
 * parameters: the 2D bit array, 2 integers representing the position of the 
 * bit at the beginning of the path
 */
static void unblack(Bit2_T, int, int);

/*
 * goes through each bit that is on the margin of the bit map (in the first
 * or final row or in the first or final column) and starts a path of black
 * edge pixels from each margin pixel that is 1
 * parameter: the 2D bit array
 */
static void unblack_margins(Bit2_T);

/*
 * goes through each bit in the row given as parameter and starts a path of 
 * black edge pixels from that pixel; other parameter: pointer to the bit map
 */
static void unblack_row(int, Bit2_T *);

/*
 * goes through each bit in the row given as parameter and starts a path of 
 * black edge pixels from that pixel; other parameter: pointer to the bit map
 */
static void unblack_col(int, Bit2_T *);

/*
 * adds the position determined by the 2 integers given as parameters to the 
 * given stack 
 */
static void push_position(Stack_T *, int, int);

struct Position {
                int row;
                int col;
};

/*
 * clears the black edges in a pbm file given as input and writes the unblacked
 * file in an output file as a pbm
 */
void clear_edges(FILE *in, FILE *op)
{
                Bit2_T map;
                read_file(in, &map);
                unblack_margins(map);
                write_map(op, &map);
                Bit2_free(&map);
}


void unblack(Bit2_T map, int col, int row)
{
                Stack_T stack  = Stack_new();
                /* push the start position and turn the pixel white */
            push_position(&stack, col, row);
                Bit2_put(map, col, row, 0);


                while (1) {
                                if (row - 1 >= 0 && Bit2_get(map, col, row - 1) == 1) {
                                                /* push "north" pixel and unblack it */
                                                push_position(&stack, col, row - 1);
                                                Bit2_put(map, col, row-1, 0);
                                                row -= 1;
                                                continue;
                                }
                                if (row + 1 < Bit2_height(map) && Bit2_get(map, col, row + 1) \
                                                                                     == 1) {
                                                /* push "south" pixel and unblack it */
                                                push_position(&stack, col, row + 1);
                                                Bit2_put(map, col, row + 1, 0);
                                                row += 1;
                                                continue;
                                }
                                if (col + 1 < Bit2_width(map) && Bit2_get(map, col + 1, row) \
                                                                                    == 1) {
                                                /* push "east" pixel and unblack it */
                                    push_position(&stack, col + 1, row);
                                    Bit2_put(map, col + 1, row, 0);
                                                col += 1;
                                                continue;
                                }
                                if (col - 1 >= 0 && Bit2_get(map, col - 1, row) == 1) {
                                                /* push "west" pixel and unblack it */
                                                push_position(&stack, col - 1, row);
                                                Bit2_put(map, col-1, row, 0);
                                                col -= 1;
                                                continue;
                                }

                        /* if no neighboring black pixel for current black edge pixel, 
                                                                             pop it */
                                struct Position *p = Stack_pop(stack);
                                row = p->row;
                                col = p->col;
                                free(p);

                                if(Stack_empty(stack) == 1){
                                                /* if stack is empty, done with the paths for this start 
                                                                                             position */
                                                break;
                                }
                }
                Stack_free(&stack);
}


void unblack_margins(Bit2_T map)
{
                unblack_row(0, &map);
                unblack_row(Bit2_height(map) - 1, &map);
                unblack_col(0, &map);
                unblack_col(Bit2_width(map) - 1, &map);
}


void unblack_row(int row, Bit2_T *map)
{
                for (int i = 0; i < Bit2_width(*map); i++) {
                                if (Bit2_get(*map, i, row) == 1) {
                                                unblack(*map, i, row);
                                }
                }
}

void unblack_col(int col, Bit2_T *map)
{
                for (int i = 0; i < Bit2_height(*map);i++){
                                if (Bit2_get(*map,col,i) == 1){
                                                unblack(*map, col, i);
                                }
                }
}

void push_position(Stack_T *stack, int col, int row)
{
                struct Position *p = malloc(sizeof(struct Position));
                if (p == NULL){
                                fprintf(stderr, "Error: canoot allocate memory");
                }
                p->row = row;
                p->col = col;
                Stack_push(*stack, p);
}