#include <stdlib.h>
#include "io_device.h"
#include <stdint.h>
#include <sys/stat.h>
#include "segment.h"
#include "bitpack.h"
#include "processor.h"
#include <stdio.h>

extern void get_program(FILE *fp, int size);
int main(int argc, char * argv[])
{
        FILE *fp;
        if(argc != 1){
                fp = fopen(argv[1], "r");
        }
        struct stat st;
        stat(argv[1], &st);
        if (stat(argv[1], &st)){
                exit(EXIT_FAILURE);
        }
        int size = st.st_size;
        get_program(fp, size);
        setup_registers();
        process();
        fclose(fp);
        
        free_memory();
        (void) argv;
        (void) argc;
        return 0;

}


void get_program(FILE *fp, int size)
{
        uint8_t c;
        uint32_t word = 0;
        setup_memory(size/4);
        /* size is teh number of bytes, so nmber of words in size 4*/
        for(int i = 0; i < size/4; i++){
                for(int n = 24; n >= 0; n -= 8){
                        c = readbyte(fp);
                        word = Bitpack_newu(word, 8, n, c);
                }
               put(0,i,word);
               word = 0;
        }
}

