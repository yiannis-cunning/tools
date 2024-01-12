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


void passert(bool tf, char *pnt){
	if(!tf){
        //perror("(errno)");
        printf("%s\n", pnt);
	    exit(1);
	}
}

int pawrite(int fd, void *ptr, int n){
    passert(write(fd, ptr, n) == n, "Error writing bytes to file");
    return n;
}

// return next biggest number ailigned with algnmt
uint32_t align(uint32_t algnmt, uint32_t num){
    if(num % algnmt == 0){
        return num;
    }
    uint32_t t = num/algnmt;
    return num - num%algnmt + algnmt;
}


#define IMAGE_FILE_EXECUTABLE_IMAGE  0x2
#define IMAGE_FILE_DEBUG_STRIPPED 0x200
#define IMAGE_FILE_RELOCS_STRIPPED 0x1


uint8_t code[31] = {0x48, 0x83, 0xec, 0x08, 0x48, 0x31, 0xc0, 0x48, 0x81, 0xd0, 0x08, 0x00, 0x00, 0x00, 0x48, 0x81, 0xe8, 0x08, 0x00, 0x00, 0x00, 0x48, 0x39, 0xc0, 0x74, 0xea, 0x48, 0x83, 0xc4, 0x08, 0xc3};
uint8_t code2[50] = {0x48, 0x83, 0xec, 0x20, 0x48, 0x8d, 0x05, 0xf5, 0x1f, 0x00, 0x00, 0x48, 0x89, 0xc1, 0x48, 0x8b, 0x05, 0x23, 0x10, 0x00, 0x00, 0x90, 0x90, 0x48, 0x31, 0xc0, 0x48, 0x81, 0xd0, 0x08, 0x00, 0x00, 0x00, 0x48, 0x81, 0xe8, 0x08, 0x00, 0x00, 0x00, 0x48, 0x39, 0xc0, 0x74, 0xea, 0x48, 0x83, 0xc4, 0x20, 0xc3};
//                                     0x08                                                                                                         0xff 0xd0                                                                                                                                                                0x08

uint8_t cmploop[59] = {0x48, 0x83, 0xec, 0x08, 0x48, 0x8d, 0x05, 0xf5, 0x1f, 0x00, 0x00, 0x48, 0x89, 0xc1, 0x48, 0x8b, 0x05, 0xeb, 0x0f, 0x00, 0x00, 0x48, 0x81, 0xf8, 0x6c, 0x20, 0x00, 0x00, 0x74, 0x18, 0xff, 0xd0, 0x48, 0x31, 0xc0, 0x48, 0x81, 0xd0, 0x08, 0x00, 0x00, 0x00, 0x48, 0x81, 0xe8, 0x08, 0x00, 0x00, 0x00, 0x48, 0x39, 0xc0, 0x74, 0xea, 0x48, 0x83, 0xc4, 0x08, 0xc3};
//                                                                                                                        0x23->0xeb 0x10->0x0f                        v0x206c                  ff d0                	    // call   *%rax
uint8_t code1100[6] = {0xff, 0x25, 0x32, 0x0f, 0x00, 0x00};

static uint16_t dos_header[] = {0x5a4d, 0x0090, 0x0003, 0x0000, 0x0004, 0x0000, 0xffff, 0x0000, 0x00b8, 0x0000, 0x0000, 0x0000, 0x0040, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0080, 0x0000, 0x1f0e, 0x0eba, 0xb400, 0xcd09, 0xb821, 0x4c01, 0x21cd, 0x6854, 0x7369, 0x7020, 0x6f72, 0x7267, 0x6d61, 0x6320, 0x6e61, 0x6f6e, 0x2074, 0x6562, 0x7220, 0x6e75, 0x6920, 0x206e, 0x4f44, 0x2053, 0x6f6d, 0x6564, 0x0d2e, 0x0a0d, 0x0024, 0x0000, 0x0000, 0x0000};
static COFF_header_t coff_header = {0};
static Shared_header_t shared_header = {0};
static PE32_plus_header_t pe_plus_header = {0};
static Section_header_t sh = {0};
static import_directory_entry_t import_table = {0};




int main(int argc, char *argv[]){
    passert(argc == 2, "Invalid cmd args");
    int fd = open(argv[1], O_CREAT | O_WRONLY);
    passert(fd != -1, "Cant create/open file");


    

    /*
        - Assuming 3 code sections that
            1) take up less than file_alignment of space on disk
            2) reserved less than section alignment each in memory
    */
    int file_alignment = 512;
    int section_alignment = 4096;
    int sz_headers = 128 +  sizeof(COFF_header_t) + sizeof(Shared_header_t) + sizeof(PE32_plus_header_t) + 3*sizeof(Section_header_t);


    int code_start_rva = align(section_alignment, sz_headers);
    int code_start_file = align(file_alignment, sz_headers);
    int file_size = code_start_file + file_alignment*3;       // file goes 1) header 2) sections (all aligned with file alignemt)
    
    
    int rva_size = code_start_rva + section_alignment*3;

    


    int j = 0;
    j += pawrite(fd, dos_header, 128);

    coff_header.zeros                   = 0x000;
    coff_header.signature              = 0x4550;               // PE signature
    coff_header.machine                = 0x8664;               // x014c;0x8664 -> 8684 is for x64 processors
    coff_header.section_count          = 3;
	coff_header.time_date_stamp        = 1689805076;
	coff_header.symbol_table_offset    = 0;                    // 0 means no symbol table
	coff_header.symbol_count           = 0;
	coff_header.optional_header_size   = sizeof(Shared_header_t) + sizeof(PE32_plus_header_t);
	coff_header.characteristics        = IMAGE_FILE_EXECUTABLE_IMAGE | IMAGE_FILE_DEBUG_STRIPPED  | IMAGE_FILE_RELOCS_STRIPPED | 0x4;
    j += pawrite(fd, &coff_header, sizeof(COFF_header_t));


    shared_header.magic                     = 0x20b;                 // 0x20b for PE32+
    shared_header.major_linker_version      = 2;                     // 
    shared_header.minor_linker_version      = 39;                    //
    shared_header.code_size                 = 0x200;                 // Size on disk of .text section
    shared_header.initialized_data_size     = 0x400;                 // Size on disk of all (initialized) data sections
    shared_header.uninitialized_data_size   = 0;                     // ^ but un-initialized type data
    shared_header.entry_point_address       = code_start_rva;        // RVA of entry point -> RVA of .text + offset into text
    shared_header.base_of_code              = code_start_rva;        // RVA of code section
    // base_of_data ... only in PE files (non +)
    j += pawrite(fd, &shared_header, sizeof(Shared_header_t));



    pe_plus_header.image_base               = 0x00400000;           // EFI might ignore this... (where RVA starts)
    pe_plus_header.section_alignment        = section_alignment;    // Aligment of sections onece loaded into memory
    pe_plus_header.file_alignment           = file_alignment;       // Alignment of sections in file on disk
    pe_plus_header.image_size               = rva_size;             // Size of image after being loaded into memory (rounded to section alignment)
    pe_plus_header.header_size              = code_start_file;      // size of headers aligned to file alignment
    pe_plus_header.checksum                 = 0x0000;               // unknoenw
    pe_plus_header.subsystem                = 3;                    // 10 for efi, 3 for normal windows?
    pe_plus_header.major_operating_system_version = 4;
    pe_plus_header.minor_operating_system_version = 0;
    pe_plus_header.major_image_version      = 2;
    pe_plus_header.minor_image_version      = 0;
    pe_plus_header.major_subsystem_version  = 5;
    pe_plus_header.minor_subsystem_version  = 2;
    pe_plus_header.reserved1                = 0;
    pe_plus_header.dll_characteristics      = 0;
	pe_plus_header.stack_reserve_size       = 0x200000;             // MAX amount of stack size that could be allocated
	pe_plus_header.stack_commit_size        = 0x1000;               // Starting amount (1 page)
	pe_plus_header.heap_reserve_size        = 0x100000;             // MAX reservable for heap
	pe_plus_header.heap_commit_size         = 0x1000;               // starting allocated for heap
	pe_plus_header.loader_flags             = 0;                    // Reserved
	pe_plus_header.number_of_rva_and_sizes  = 16;

    uint32_t *table_ptr = &pe_plus_header.export_table_rva;
    for(int i = 0; i < pe_plus_header.number_of_rva_and_sizes; i ++){
        table_ptr[i] = 0;
    }

    pe_plus_header.import_table_rva        = 0x2018; //code_start_rva + align(section_alignment, 50);
    pe_plus_header.import_table_size       = 115;
    pe_plus_header.iat_rva                 = 0x2000; //pe_plus_header.import_table_rva + sizeof(import_directory_entry_t)*2 + sizeof(uint64_t)*2;
    pe_plus_header.iat_size                = sizeof(uint64_t)*3;

    j += pawrite(fd, &pe_plus_header, sizeof(PE32_plus_header_t));
    


    // Code headers
    strcpy(sh.name, ".text");
    sh.virtual_size                        = 0x106;                           // Size after being loaded into memory
    sh.virtual_address                     = code_start_rva;               // RVA of start of section
    sh.size_of_raw_data                    = align(file_alignment, 50);    // Size of raw code aligned to file aligment
    sh.pointer_to_raw_data                 = code_start_file;              // file pointer to start of this section
    sh.characteristics                     = 0x60500060;                   // Characteristic of section
    sh.pointer_to_relocations              = 0;
    sh.pointer_to_linenumbers              = 0;
    sh.number_of_relocations               = 0;
    sh.number_of_linenumbers               = 0;
    j += pawrite(fd, &sh, sizeof(Section_header_t));

    strcpy(sh.name, ".idata");
    sh.virtual_size                        = 139;
    sh.virtual_address                     = code_start_rva + section_alignment;
    sh.size_of_raw_data                    = align(file_alignment, 139);
    sh.pointer_to_raw_data                 = code_start_file + file_alignment;
    sh.characteristics                     = 0xc0300040;
    sh.pointer_to_relocations              = 0;
    sh.pointer_to_linenumbers              = 0;
    sh.number_of_relocations               = 0;
    sh.number_of_linenumbers               = 0;
    j += pawrite(fd, &sh, sizeof(Section_header_t));

    strcpy(sh.name, ".rdata");
    sh.virtual_size                        = 65;
    sh.virtual_address                     = code_start_rva + section_alignment*2;
    sh.size_of_raw_data                    = align(file_alignment, 75);
    sh.pointer_to_raw_data                 = code_start_file + file_alignment*2;
    sh.characteristics                     = 0x40000040;
    sh.pointer_to_relocations              = 0;
    sh.pointer_to_linenumbers              = 0;
    sh.number_of_relocations               = 0;
    sh.number_of_linenumbers               = 0;
    j += pawrite(fd, &sh, sizeof(Section_header_t));



    uint8_t zero = 0;
    uint8_t nop = 0x90;
    while(j < code_start_file){
        j += pawrite(fd, &zero, 1);
    }

    // code + data section starts...
    // Section 1: code (where the entry point is)
    //j += pawrite(fd, code2, 50);
    j += pawrite(fd, cmploop, 59);
    
    while(j < code_start_file + 0x100){
        j += pawrite(fd, &nop, 1);
    }
    
    j += pawrite(fd, code1100, 6);

    while(j%file_alignment != 0){
        j += pawrite(fd, &zero, 1);
    }
    passert(j == file_alignment + code_start_file, "ERROR: code/idata is not aligned as expected");



    uint8_t idata[0x200];

/*     All this will go to the idata section
  .idata                -> code_start_rva + section_alignment; = 0x2000
import_table:
    - entry1            -> 
    - 0's               -> sizeof(import_directory_entry_t)*2 = 40
import_lookup_table"
    - entry1            ->
    - 0's               -> sizeof(uint64_t)*2 = 16
import_address_table
    - entry1            -> 
    - 0's               -> sizeof(uint64_t)*2 = 16
hint/name table:        -> 
    - name1 = _cprintf  -> 2 + 9 + 1 = 12 strlen -> 10
string:
    - str = msvcrt.dll  -> 11
*/
/*
    uint32_t import_table_rva       = code_start_rva + align(section_alignment, 50);
    uint32_t ilt1_rva               = import_table_rva + sizeof(import_directory_entry_t)*2;
    uint32_t iat_rva                = ilt1_rva + sizeof(uint64_t)*2;
    uint32_t ref1_name_rva          = iat_rva + sizeof(uint64_t)*2;
    uint32_t dll_name_rva           = ref1_name_rva + 12;
    
    int j_start = j;
    import_table.iprt_lookup_table_rva      = ilt1_rva;                  // just past the text section -> start of idata section
    import_table.time                       = 0;                         // set by loader after dll bound
    import_table.forwarder_chain            = 0;                         // no clue?
    import_table.name_rva                   = dll_name_rva;
    import_table.iprt_addr_table_rva        = iat_rva;
    uint64_t dll1lookup_entry               = ref1_name_rva & 0x00000000ffffffff;
    uint64_t zero_64                        = 0;
    uint8_t hint_entry[12]                  = {0x00, 0x00,'_', 'c', 'p', 'r', 'i', 'n', 't', 'f', '\0', '\0'};

    // 1) import table
    j += pawrite(fd, &import_table, sizeof(import_directory_entry_t));
    for(int i = 0; i < sizeof(import_directory_entry_t); i+=1){j += pawrite(fd, &zero, 1);}
    passert(j - j_start == ilt1_rva - import_table_rva, "ERROR: with lookup table 1");

    // 2) dll1 lookup table
    j += pawrite(fd, &dll1lookup_entry, sizeof(uint64_t));
    j += pawrite(fd, &zero_64,          sizeof(uint64_t));
    passert(j - j_start == iat_rva - import_table_rva, "ERROR: with address table");

    // 3) returned address table - expecting 1 pointer back
    j += pawrite(fd, &zero_64,          sizeof(uint64_t));
    j += pawrite(fd, &zero_64,          sizeof(uint64_t));
    passert(j - j_start == ref1_name_rva - import_table_rva, "ERROR: with hint table");

    // 4) hint table/function name
    j += pawrite(fd, &hint_entry, 12);
    passert(j - j_start == dll_name_rva - import_table_rva, "ERROR: with aligning with dll string");

    // 5) write dll name str
    j += pawrite(fd, "msvcrt.dll", 11);
    passert(j - j_start == 95, "ERROR: with aligning with dll string");


*/
/*
.idata = 0x2000
import_address_table:   (0x2000)
    - entry 1.a                                 = 0x206c 
    - zeros*2
import_table:           (0x2018)
    - entry 1
        - (location of import_lookup table 1)   = 0x2054
        - time                                  = 0
        - forwarder_chain                       = 0
        - char *dll                             = 0x2080
        - import addrese table 1                = 0x2000
    - zeros x 2
import_lookup table:    (0x2054)
    - entry 1.a                                 = 0x206c
    - zeros*2
hint_name_table:        (0x206c)
    - 0x0101, "_cprintf", '\0'? = 0x1, 0x1, '_' , ... -> 12 bytes
    - 0x00*8                                          -> 8 bytes
other_strings:          (0x2080)
    - "msvcrt.dll"                                    -> 11 bytes
    zeros
*/

    passert(j == 0x400, "Error aligning to start of idata section");
    uint64_t temp64;
    uint8_t temp8;
    uint16_t temp16;
    uint32_t temp32;

    temp64 = 0x206c; j += pawrite(fd, &temp64, sizeof(uint64_t));
    temp64 = 0; j += pawrite(fd, &temp64, sizeof(uint64_t));
    temp64 = 0; j += pawrite(fd, &temp64, sizeof(uint64_t));

    temp32 = 0x2054; j += pawrite(fd, &temp32, sizeof(uint32_t));
    temp32 = 0; j += pawrite(fd, &temp32, sizeof(uint32_t));
    temp32 = 0; j += pawrite(fd, &temp32, sizeof(uint32_t));
    temp32 = 0x2080; j += pawrite(fd, &temp32, sizeof(uint32_t));
    temp32 = 0x2000; j += pawrite(fd, &temp32, sizeof(uint32_t));
    
    for(int q=0; q < 10; q+= 1){temp32 = 0; j += pawrite(fd, &temp32, sizeof(uint32_t));}
    passert(j == 0x454, "Import lookup table not alighned in idata section");

    temp64 = 0x206c; j += pawrite(fd, &temp64, sizeof(uint64_t));
    temp64 = 0; j += pawrite(fd, &temp64, sizeof(uint64_t));
    temp64 = 0; j += pawrite(fd, &temp64, sizeof(uint64_t));

    passert(j == 0x46c, "Error aligning with end of import_lookup table");


    temp16 = 0x0101; j += pawrite(fd, &temp16, sizeof(uint16_t));
    j += pawrite(fd, "_cprintf", 9);
    temp8 = 0; j += pawrite(fd, &temp8, sizeof(uint8_t));
    temp64 = 0; j += pawrite(fd, &temp64, sizeof(uint64_t));
    
    passert(j == 0x480, "Error aligning with end of name hint table");
    j += pawrite(fd, "msvcrt.dll", 11);
    passert(j == 0x48b, "Error aligning with end of idata section");




    while(j%file_alignment != 0){
        j += pawrite(fd, &zero, 1);
    }

    passert(j == code_start_file + file_alignment*2, "ERROR aligning end of idata section");


    pawrite(fd, "HelloWorld", 11);


    // Section 3 (data sections)
    while(j < file_size){
        j += pawrite(fd, &zero, 1);
    }

    close(fd);


}