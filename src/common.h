#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <stdbool.h>
#include <errno.h>
#include <time.h>





void passert(bool tf, char *pnt){
	if(!tf){
        //perror("(errno)");
        if(errno != 0){
            perror("(errno)");
        }
        printf("%s\n", pnt);
	    exit(1);
	}
}

void passert_line(bool tf, char *pnt, int arg1){
	if(!tf){
        //perror("(errno)");
        if(errno != 0){
            perror("(errno)");
        }
        printf(pnt, arg1);
        printf("\n");
	    exit(1);
	}
}




int pawrite(int fd, void *ptr, int n){
    passert(write(fd, ptr, n) == n, "Error writing bytes to file");
    return n;
}

int paread(int fd, void *ptr, int n){
    passert(write(fd, ptr, n) == n, "Error writing bytes to file");
    return n;
}