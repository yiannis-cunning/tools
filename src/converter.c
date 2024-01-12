#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


/*
Reads in real binary, outputs char string
*/

int main(int argc, char *argv[]){
    
    uint8_t mask = 1;
    uint8_t bin_in;
    char char_out;
    int n;
    

    n = read(0, &bin_in, 1);
    while(n == 1){
        for(int i = 0; i < 8; i++){
            char_out = '0' + (((1 << i) & bin_in) != 0);
            n = write(1, &char_out, 1);
        }
        n = read(0, &bin_in, 1);
    }


    return 0;
}