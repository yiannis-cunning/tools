#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


int main(int argc, char *argv[]){
	int fd = 0;
	int byline = 0;

	for(int j = 0; j < argc; j++){
		if(argv[j][0] == '-'){
			// flag
			if(strcmp(argv[j], "-l") == 0){
				byline = 1;
			}
			argc = argc - 1;
		}
	}

	if(argc == 3){
		fd = open(argv[2], O_RDONLY);
		if(fd == -1){
			exit(1);
		}
	}

	int max_c = atoi(argv[1]);
	if(max_c == -1){
		exit(1);
	}


	//int fd = open(argv[1], O_RDONLY);
	char buffer[4096];
	int n;
	int line_count = 0;
	if(byline == 0){
		// Read through file
		n = read(fd, buffer, 4096);
		while(n > 0 && max_c > 0){
			if(n <= max_c){
				write(1, buffer, n);
			}
			else{
				write(1, buffer, max_c);
				break;
			}
			n = read(fd, buffer, 4096);
		}
	}
	else{
		n = read(fd, buffer, 4096);
		while(n > 0){
			for(int j = 0; j < n; j++){
				if(buffer[j] == '\n'){
					line_count += 1;
				}
				if(line_count >= max_c){
					write(1, buffer, j + 1);
					exit(1);
				}
			}
			write(1, buffer, n);

			n = read(fd, buffer, 4096);
		}
	}

	printf("\n");
	if(argc == 3){
		close(fd);
	}


}
