#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <stdbool.h>
#include <time.h>
#include "PEfile.h"

typedef struct{
    bool disp_coff_header;
    bool disp_shared_header;
    bool disp_windows_header;
    bool disp_section_header;
    bool disp_code_content;
    int code_section_nums[10];
    bool disp_string_table;
    bool disp_virtual_code;
    bool ram_contents;
    bool import_table;
    bool relocation_table;
} flags_t;



PE_file_t *parse_PE_file();

void free_PE_file(PE_file_t *pe);

void print_PE_file(PE_file_t *pe, flags_t *flags);

void print_section_headers(PE_file_t *pe);

void print_windows_specific_header(PE_file_t *pe);

void get_code_range(PE_file_t *pe, uint32_t *code_start, uint32_t *code_end, uint32_t *code_size);

void print_code(PE_file_t *pe, flags_t *flags);



void passert(bool tf, char *pnt){
	if(!tf){
        perror("(errno)");
        printf("%s\n", pnt);
	    exit(1);

	}
}




// Asuming argv first entry is a flag
flags_t *parse_flags(char *argv[], int argc){
    flags_t *ans = (flags_t *)calloc(sizeof(flags_t), 1);

    int j = 0;
    int i = 0;
    while(j < argc){
        // Skip first 2 args
        if(j == 0 || j == 1){j += 1; continue;}
        if(strcmp(argv[j], "-c") == 0){ans->disp_coff_header = 1;}
        else if(strcmp(argv[j], "-s") == 0){ans->disp_shared_header = 1;}
        else if(strcmp(argv[j], "-w") == 0){ans->disp_windows_header = 1;}
        else if(strcmp(argv[j], "-e") == 0){ans->disp_section_header = 1;}
        else if(strcmp(argv[j], "-t") == 0){ans->disp_string_table = 1;}
        else if(strcmp(argv[j], "-t") == 0){ans->disp_string_table = 1;}
        else if(strcmp(argv[j], "-q") == 0){ans->disp_virtual_code = 1;}
        else if(strcmp(argv[j], "-v") == 0){ans->ram_contents = 1;}
        else if(strcmp(argv[j], "-i") == 0){ans->import_table = 1;}
        else if(strcmp(argv[j], "-r") == 0){ans->relocation_table = 1;}
        else if(strcmp(argv[j], "-a") == 0){
            if(ans->disp_code_content == 1){free(ans);return NULL;}
            ans->disp_code_content = 1;
            j += 1;
            while(j < argc && argv[j][0] != '-'){
                if(i < 9){
                    ans->code_section_nums[i] = atoi(argv[j]);
                    if(ans->code_section_nums[i] == 0){free(ans);return NULL;}
                    i += 1;
                }
                j += 1;
            }
            ans->code_section_nums[i] = -1;
            if(i == 0){free(ans);return NULL;}
            continue;
        }
        else {free(ans); return NULL;}
        j += 1;
    }
    return ans;
    
}