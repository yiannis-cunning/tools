

#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "this.h"

typedef struct vector_t{
	int a;
	int b;
} vector_t;

static vector_t vec = {1, 2};


int main(int argc, char *argv[]){

	char this = 'a';
	uint8_t zero = 0;

	write(1, &this, 1);
	write(1, &zero, 1);
	write(1, &this, 1);
	
	printf("%d", vec.a);
	

	return 0;
}
