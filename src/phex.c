#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdint.h>

int main(int argc, char *argv[]){
	// Open file as fd
   	if(argc != 2){
    	return 1;
    }
    int fd = open(argv[1], O_RDONLY);
	char buffer[4096];

	// Read through file
	uint8_t ch;
	uint32_t base = 0;
	int j = 16;
	int n = read(fd, buffer, 4096);
	while(n > 0){
		for(int i = 0; i < n; i++){
			ch = buffer[i];
			// For each line
			if(j >= 16){
				if(base != 0){printf("\n");}
				printf("0x%X:\t", base);
				j = 0;
				base += 16;
			}
			// For each 4 chars
			if((j) % 4 == 0){printf("  ");}
			else{printf("_");}
			// For each char
			printf("%02x", ch);
			j += 1;
			
		}


		n = read(fd, buffer, 4096);
	}

	printf("\n");
	close(fd);


}
