/* 
 * ppmtrans.c
 * by Georgios Pesmazoglou, Alexandru Ungureanu
 * October 7th, 2016
 *
 * This program reads in from the command line to set the type of mapping
 * the kind of transformation of the picture, the name of the picture file
 * whether timing data is needed and where to store it. Then reads in the 
 * picture using the pnm interface. It calls the appropriate transformation
 * function using the interface transformations.h and prints out the modified
 * file. 
 */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "assert.h"
#include "pnm.h"
#include "transformations.h"

#define SET_METHODS(METHODS, MAP, WHAT) do {                    \
        methods = (METHODS);                                    \
        assert(methods != NULL);                                \
        map = methods->MAP;                                     \
        if (map == NULL) {                                      \
                fprintf(stderr, "%s does not support "          \
                                WHAT "mapping\n",               \
                                argv[0]);                       \
                exit(1);                                        \
        }                                                       \
} while (0)

static void
usage(const char *progname)
{
        fprintf(stderr, "Usage: %s [-rotate <angle>] "
                        "[-{row,col,block}-major] [filename]\n",
                        progname);
        exit(1);
}

// struct that holds all the necessary parameters for  transform function
struct data {
        char* operation;
        char* time_file_name;
        char* direction;
        int rotation;
        FILE* image_file;
        A2Methods_mapfun* map;
        A2Methods_T* methods;
};

  /*
   * Reads picture uses transform interface and prints picture
   * Parameters: struct data
   * 
   */ 
static void transform (struct data args)
{
        Pnm_ppm pic;

        pic = Pnm_ppmread(args.image_file, *args.methods);


        if(strcmp(args.operation,"-rotate") == 0) {
                rotate(pic, args.map, args.rotation, args.time_file_name);
        }
        else if(strcmp(args.operation,"-flip") == 0) {
                flip(pic, args.map, args.direction, args.time_file_name);
        }
        else if(strcmp(args.operation,"-transpose") == 0) {
                transpose(pic, args.map, args.time_file_name);
        }

        Pnm_ppmwrite(stdout, pic);

        Pnm_ppmfree(&pic);
}

/*
 * main function
 * for-loop for reading in command line arguments
 * Then calls transform function.
 */
int main(int argc, char *argv[]) 
{
        char *time_file_name = NULL;
        int   rotation       = 0;
        int   i;
        char* operation = "";
        char* direction = NULL;

        /* default to UArray2 methods */
        A2Methods_T methods = uarray2_methods_plain; 
        assert(methods);

        /* default to best map */
        A2Methods_mapfun* map = methods->map_default; 
        assert(map);

        for (i = 1; i < argc; i++) {
                if (strcmp(argv[i], "-row-major") == 0) {
                        SET_METHODS(uarray2_methods_plain, map_row_major,
                         "row-major");
                } else if (strcmp(argv[i], "-col-major") == 0) {
                        SET_METHODS(uarray2_methods_plain, map_col_major,
                         "column-major");
                } else if (strcmp(argv[i], "-block-major") == 0) {
                        SET_METHODS(uarray2_methods_blocked, map_block_major,
                                    "block-major");
                } else if (strcmp(argv[i], "-rotate") == 0) {

                        operation = argv[i];
                        if (!(i + 1 < argc)) {      /* no rotate value */
                                usage(argv[0]);
                        }
                        char *endptr;
                        rotation = strtol(argv[++i], &endptr, 10);
                        if (!(rotation == 0 || rotation == 90 ||
                            rotation == 180 || rotation == 270)) {
                                fprintf(stderr,
                                 "Rotation must be 0, 90 180 or 270\n");
                                usage(argv[0]);
                        }
                        if (!(*endptr == '\0')) {    /* Not a number */
                                usage(argv[0]);
                        }

                } else if (strcmp(argv[i], "-time") == 0) {
                        time_file_name = argv[++i];
                } else if(strcmp(argv[i], "-transpose") == 0){
                    operation = argv[i];
                } else if (strcmp(argv[i], "-flip") == 0) {
                        operation = argv[i];
                        if (strcmp(argv[i+1], "horizontal") != 0 &&
                                strcmp(argv[i+1],"vertical") != 0) {
                                fprintf(stderr,
                                "Rotation must be horizontal or vertical \n");
                                usage(argv[0]);
                        }
                        i++;
                        direction = argv[i];
                } else if (*argv[i] == '-') {
                        fprintf(stderr, "%s: unknown option '%s'\n",
                                 argv[0], argv[i]);              
                } else if (argc - i > 1) {
                        fprintf(stderr, "Too many arguments\n");
                        usage(argv[0]);
                } else {
                        break;
                }
        }
        if (i == argc) {
                struct data arguments = {operation,time_file_name, direction,
                        rotation, stdin, map, &methods};
                transform(arguments);
        } else{
                FILE *fp = fopen(argv[i], "r");
                if(fp == NULL){
                        fprintf(stderr,
                            "File name cannot be opened for reading\n");
                        exit (EXIT_FAILURE);
                }
                struct data arguments = {operation,time_file_name, direction,
                                  rotation, fp, map, &methods};
                if(operation != NULL) {
                        transform(arguments);
                }
                fclose(fp);
        }
        exit (EXIT_SUCCESS);
}
