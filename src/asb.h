#include "common.h"
#include "PEfile.h"








static COFF_header_t coff_header = {0};
static Shared_header_t shared_header = {0};
static PE32_plus_header_t pe_plus_header = {0};
static Section_header_t sh = {0};
static import_directory_entry_t import_table = {0};



typedef struct{
    char name[8];
    uint32_t raw_data_size;
    uint8_t *raw_data_p;

    section_t *next;
} section_t;



typedef struct{
    uint32_t machine_type;
    uint32_t file_alignment;
    uint32_t section_alignment;

    uint32_t n_section;
    section_t *sections;

} asm_file_t;




// return next biggest number ailigned with algnmt
uint32_t align(uint32_t algnmt, uint32_t num){
    if(num % algnmt == 0){
        return num;
    }
    uint32_t t = num/algnmt;
    return num - num%algnmt + algnmt;
}


void passert_syntax(bool tf, char *reason, char *line, int line_num){
    if(tf){
        return;
    }
    printf("Compilation error on line %d", line_num);
    if(line != NULL){
        printf("\t\"%s\"\n", line);
    }
    printf("Reason: %s\n", reason);
    exit(1);
}
