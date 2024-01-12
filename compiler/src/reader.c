#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <stdbool.h>
#include <stddef.h>


int get_nxt_char(int fd, char *buf){
    int n = 0;
    *buf = '\n';
    while(*buf == '\n' || *buf == ' ' || *buf == '\t'){
        n = read(fd, buf, 1);
        if(n == 0){
            return -1;
        }
    }
    return 0;
}



// take in hex formated and output "0"/"1" stream
int main(int argc, char *argv[]){
    char buf;
    int eof = 0;
    uint8_t bin_num;

    while(eof == 0){
        eof = get_nxt_char(0, &buf);
        if(eof == -1){break;}

        // f
        if(buf >= '0' && buf <= '9'){bin_num = buf - '0';}
        else if(buf >= 'a' && buf <= 'f'){bin_num = buf - 'a' + 10;}
        else if(buf >= 'A' && buf <= 'F'){bin_num = buf - 'A' + 10;}
        else{break;}


        if((bin_num & 8) != 0){buf = '1';}
        else{buf = '0';}
        if(write(1, &buf, 1) != 1){break;}

        if((bin_num & 4) != 0){buf = '1';}
        else{buf = '0';}
        if(write(1, &buf, 1) != 1){break;}

        if((bin_num & 2) != 0){buf = '1';}
        else{buf = '0';}
        if(write(1, &buf, 1) != 1){break;}

        if((bin_num & 1) != 0){buf = '1';}
        else{buf = '0';}
        if(write(1, &buf, 1) != 1){break;}

    }


}