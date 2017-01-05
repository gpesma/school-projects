#include <stdio.h>
#include <stdlib.h>
#include "bit2.h"
#include "print_map.h"


static void print_map(int col, int row, Bit2_T bit2, int value, void *cl);

extern void write_file(FILE *fp, Bit2_T *map)
{
        fprintf("P1\n")
                        
        Bit2_map_row_major(map,print_map,NULL);
}
static void print_map(int col, int row, Bit2_T bit2, int value, void *cl)
{
        printf("%d", Bit2_get(bit2,col,row));
        (void) cl;
        (void) value;
        if(col == Bit2_width(bit2)-1){
                printf("\n");
        }
}