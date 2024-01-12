#include "readPE.h"

#define IMAGE_SCN_CNT_CODE 0x00000020
#define IMAGE_SCN_CNT_INITIALIZED_DATA 0x00000040
#define IMAGE_SCN_CNT_UNINITIALIZED_DATA 0x00000080



// Print the contents of tte PE file
void print_PE_file(PE_file_t *pe, flags_t *flags){
    int sec_start = 0;
    int sec_end = 0;

    passert(pe != NULL, "PE obj does not exist");
    passert(pe->dos_header != NULL && pe->coff_header != NULL && pe->shared_header != NULL, "PE header objects do not exist");
    if(pe->isplus){
        passert(pe->pe_plus_header != NULL, "PE+ header does not exist");
    } else{
        passert(pe->pe_header != NULL, "PE header does not exist");
    }
    passert(pe->section_headers != NULL && pe->code_pointers != NULL, "Code section headers or content does not exist");
    if(pe->coff_header->symbol_table_offset != 0 ){
        passert(pe->string_table != NULL, "String table should exist but does not");
    }


    int code_sec = -1;
    // DOS
    sec_end = pe->dos_header->offset;
    if(pe->isplus){
        printf("Printing PE32+ file...\n\n");
    }else{
        printf("Printing PE32 file...\n\n");
    }
    printf("All adresses given are in file offsets\n\n");

    printf("0x0 - 0x%x: DOS header\n", sec_end);
    printf("\t Offset to COFF: 0x%x\n", pe->dos_header->offset);
    printf("\t...\n");

    // COFF
    sec_start = sec_end;
    sec_end = sec_start + sizeof(COFF_header_t);
    printf("\n0x%x - 0x%x: COFF header\n", sec_start, sec_end);
    if(flags->disp_coff_header){
        printf("\t Signature/exe type:        0x %c%c\n", *(char *)&pe->coff_header->signature, *((char *)&pe->coff_header->signature + 1));
        printf("\t ASM arch.:                 0x %x (0x14c = i386)\n", pe->coff_header->machine);
        printf("\t Section count:             0d %d\n", pe->coff_header->section_count);
        printf("\t Time:                      0x %s", ctime( (time_t * )&pe->coff_header->time_date_stamp));
        printf("\t Symbol table offset:       0x %x\n", pe->coff_header->symbol_table_offset);
        printf("\t Num of Symbols:            0d %d\n", pe->coff_header->symbol_count);
        printf("\t Optional header size:      0d %d\n", pe->coff_header->optional_header_size);
        printf("\t EXE Characteristics:       0x %x\n", pe->coff_header->characteristics);
    } 
    else{ printf("\t... <c>\n");}

    // SHARED
    sec_start = sec_end;
    sec_end = sec_start + sizeof(Shared_header_t) + 4*(!pe->isplus);
    printf("\n0x%x - 0x%x: Shared/Optional header\n", sec_start, sec_end);
    if(flags->disp_shared_header){
        printf("\t Magic number:              0x %x\n", pe->shared_header->magic);
        printf("\t Maj + Min Linker ver.:     0x %x/%x\n", pe->shared_header->major_linker_version, pe->shared_header->minor_linker_version);
        printf("\t Code size (text section):  0x %x\n", pe->shared_header->code_size);
        printf("\t Initialized data size:     0x %x\n", pe->shared_header->initialized_data_size);
        printf("\t Un-initialized data size:  0x %x\n", pe->shared_header->uninitialized_data_size);
        printf("\t Entry point address:       0x %x\n", pe->shared_header->entry_point_address);
        printf("\t Base of code:              0x %x\n", pe->shared_header->base_of_code);
        if(!pe->isplus){printf("\t Base of data:              0x %x\n", pe->base_of_data);}
    }
    else{printf("\t... <s>\n");}

    // WINDOWS
    sec_start = sec_end;
    sec_end = pe->dos_header->offset + sizeof(COFF_header_t) + pe->coff_header->optional_header_size;
    printf("\n0x%x - 0x%x: Windows specific\n", sec_start, sec_end);
    if(flags->disp_windows_header){
        print_windows_specific_header(pe);
    }
    else{printf("\t... <w>\n");}




    // SECTION HEADERS
    uint32_t code_start, code_end, code_size;
    get_code_range(pe, &code_start, &code_end, &code_size);
    sec_start = sec_end;
    sec_end = sec_start + pe->n_sections*sizeof(Section_header_t);
    printf("\n0x%x - 0x%x: Section headers x%d\n", sec_start, sec_end, pe->n_sections);
    if(flags->disp_section_header){
        print_section_headers(pe);
    }
    else{
        printf("\t... <e>\n");
    }




    // CODE CONTENT
    sec_start = 0;
    while(sec_start < sec_end){
        if(!pe->isplus){
            sec_start += pe->pe_header->file_alignment;
        } else {
            sec_start += pe->pe_plus_header->file_alignment;
        }
    }
    sec_end = sec_start + code_size;
    printf("\n0x%x - 0x%x: Raw code content\n", sec_start, sec_end);
    if(flags->disp_code_content && pe->n_sections > flags->code_section_nums[0] - 1){
        code_sec = flags->code_section_nums[0] - 1;
        Section_header_t *sh = (pe->section_headers + code_sec);
        sec_start = sh->pointer_to_raw_data;
        sec_end = sec_start + sh->size_of_raw_data;
        printf("\n0x%x - 0x%x: Code section %d\n", sec_start, sec_end, code_sec);
        for(int k = 0; k < sh->size_of_raw_data && k < 200; k+=8){
            printf("\t ");
            for(int l = 0; l < 8; l ++){
                printf("%02x ", pe->code_pointers[code_sec][k+l]);
            }
            printf("\n");
        }
    }
    else{
        printf("\t... <a #>\n");
    }



    if(pe->coff_header->symbol_table_offset != 0 ){
        // SYMBOL
        sec_start = pe->coff_header->symbol_table_offset;
        sec_end = sec_start + pe->coff_header->symbol_count*18 - 1;
        printf("\n0x%x - 0x%x: Symbol table\n", sec_start, sec_end);
        printf("\t...\n");

        // STRING
        sec_start = sec_end + 1;
        sec_end = sec_start + pe->sz_string_table;
        int pos = 0;
        int i = 1;
        printf("\n0x%x - 0x%x: String table\n", sec_start, sec_end);
        if(flags->disp_string_table){
            while(pos < pe->sz_string_table){ //pe->sz_string_table){
                printf("\t String %d: %s\n", i, pe->string_table + 4 + pos);
                pos += strlen(pe->string_table + pos) + 1;
                i += 1;
            }
        }
        else{
            printf("\t... <t>\n");
        }
    } else{
        printf("\nNo symbol or string tables\n");
    }

    printf("\n");
    if(flags->disp_virtual_code && false){
        print_code(pe,flags);
    }
    else{
        printf("Virtual view: <v>\n");
    }
}


void print_code(PE_file_t *pe, flags_t *flags){
    // List how sections are put into code
    //if(pe->isplus){printf("Printing PE32+ file code...\n\n");}else{printf("Printing PE32 file code...\n\n");}
    
    Section_header_t *sh_top = pe->section_headers;
    Section_header_t *sh, *sh_nxt;
    uint64_t rva = 0;
    uint64_t lowest = -1;
    uint32_t image_base = 0;
    if(pe->isplus){
        image_base = pe->pe_plus_header->image_base;
    } else{
        image_base = pe->pe_header->image_base;
    }

    while(1){
        sh_nxt = NULL;
        lowest = -1;
        for(int i = 0; i < pe->n_sections; i++){
            sh = sh_top + i;
            if(sh->virtual_address < lowest && sh->virtual_address > rva){
                lowest = sh->virtual_address;
                sh_nxt = sh;
            }
        }
        // Print sh_nxt;
        if(sh_nxt == NULL){break;}
        sh = sh_nxt;
        rva = sh->virtual_address;

        printf("VA: 0x%lx - 0x%lx: (amt in file: %x)", image_base + rva, image_base + rva + sh->virtual_size, sh->size_of_raw_data);
        if(sh->name[0] != '/'){
            printf("%s", sh->name);
        }else{
            printf("%s", pe->string_table + atoi(sh->name + 1));
        }
        printf("\n");

        printf("\t 0x%x \n", sh->characteristics);

        if((sh->characteristics & IMAGE_SCN_CNT_CODE) != 0){
            printf("\t Code section..\n");
        }
        if( (sh->characteristics & IMAGE_SCN_CNT_INITIALIZED_DATA) != 0){
            printf("\t Initialized data section..\n");
        }
        
        if( (sh->characteristics & IMAGE_SCN_CNT_UNINITIALIZED_DATA) != 0){
            printf("\t Uninitalized data section..\n");
        }

    }
}

PE_file_t *parse_PE_file(int fd){
    // 1) Init the PE file object
    PE_file_t *pe = (PE_file_t *)calloc(1, sizeof(PE_file_t));
    DOS_header_t *dos_header = (DOS_header_t *)calloc(1, sizeof(DOS_header_t));
    COFF_header_t *coff_header = (COFF_header_t *)calloc(1, sizeof(COFF_header_t));
    Shared_header_t *shared_header = (Shared_header_t *)calloc(1, sizeof(Shared_header_t));

    pe->pe_header = NULL;
    pe->pe_plus_header = NULL;
    pe->dos_header = dos_header;
    pe->coff_header = coff_header;
    pe->shared_header = shared_header;


    // 2) Read all headers
    // DOS header is at start of file
	passert(read(fd, dos_header, sizeof(DOS_header_t)) == sizeof(DOS_header_t), "Failed to read DOS header\n");
	
	// COFF header is offset into file
	passert(lseek(fd, dos_header->offset, SEEK_SET) != -1, "Failed to seek to the COFF header.\n");
	passert(read(fd, coff_header, sizeof(COFF_header_t)) == sizeof(COFF_header_t), "Failed to read the COFF header.\n");

    // Optional header follows COFF header (size given)
    passert(coff_header->signature == 0x4550, "Image not in PE/PE+ format\n");
    passert(coff_header->optional_header_size > sizeof(Shared_header_t), "No optional header in Image file\n");
    //printf("offset symb %x, size symb %d, header size %d\n", coff_header->symbol_table_offset, coff_header->symbol_count, coff_header->optional_header_size);
    // Read the Shared_header_t
    passert(read(fd, shared_header, sizeof(Shared_header_t)) == sizeof(Shared_header_t), "Failed to read the shared header.\n");

    // Check if PE32/PE32+ (make sure its one of those)
    passert(shared_header->magic == 0x10b || shared_header->magic == 0x20b, "Error in optional header magic number\n");
    pe->isplus = shared_header->magic == 0x20b;

    if(!pe->isplus){
        passert(read(fd, &(pe->base_of_data), sizeof(pe->base_of_data)) == sizeof(pe->base_of_data), "Failed to read the base of data (PE32+).\n");
     }


    // Read the windows specific header
    PE32_plus_header_t *pe_plus_header = NULL;
    PE32_header_t *pe_header = NULL;
    pe->pe_plus_header = NULL;
    pe->pe_header = NULL;
    if(pe->isplus){
        pe_plus_header = (PE32_plus_header_t *)calloc(sizeof(PE32_plus_header_t), 1); // Windows specific
        pe->pe_plus_header = pe_plus_header;
        passert(read(fd, pe_plus_header, sizeof(PE32_plus_header_t)) == sizeof(PE32_plus_header_t), "Failed to read the plus specific header.\n"); // Windows specific

    }
    else{
        pe_header = (PE32_header_t *)calloc(sizeof(PE32_header_t), 1); // Windows specific
        pe->pe_header = pe_header;
        passert(read(fd, pe_header, sizeof(PE32_header_t)) == sizeof(PE32_header_t), "Failed to read the non-plus specific header.\n"); // Windows specific
    }

    
    // Read the string tables
    if(coff_header->symbol_table_offset != 0){
        passert(lseek(fd, coff_header->symbol_table_offset + 18*coff_header->symbol_count, SEEK_SET) != -1, "Failed to seek to the string table.\n");
        passert(read(fd, &(pe->sz_string_table), sizeof(uint32_t)) == sizeof(uint32_t), "Failed to read string table size.\n");
        //printf("string table size: %d\n", pe->sz_string_table);
        char *string_table = (char *)calloc(pe->sz_string_table, 1);
        uint32_t *sttbl = (uint32_t *)string_table;
        *sttbl = pe->sz_string_table;
        pe->string_table = string_table;
        passert(read(fd, string_table + 4, sizeof(char)*pe->sz_string_table - 4) == sizeof(char)*pe->sz_string_table - 4, "Failed to read string table contents.\n");
    }


    // Now can jump to the section headers
    passert(lseek(fd, coff_header->optional_header_size + dos_header->offset + sizeof(COFF_header_t), SEEK_SET) != -1, "Failed to seek to the first section header.\n");
    pe->n_sections = coff_header->section_count;
    Section_header_t *section_headers = (Section_header_t *)calloc(pe->n_sections, sizeof(Section_header_t));
    pe->section_headers = section_headers;
    for(int i = 0; i < pe->n_sections; i++){
        passert(read(fd, section_headers + i, sizeof(Section_header_t)) == sizeof(Section_header_t), "Failed to read the shared header.\n");
    }

    pe->code_pointers = (uint8_t **)calloc(pe->n_sections, sizeof(uint8_t *));
    // Seek and save all section code
    for(int i = 0; i < pe->n_sections; i ++){
        pe->code_pointers[i] = (uint8_t *)calloc(1, sizeof(uint8_t) * (section_headers + i)->size_of_raw_data);
        passert(lseek(fd, (section_headers + i)->pointer_to_raw_data, SEEK_SET) != -1, "Failed to seek to section code\n");
        passert(read(fd, pe->code_pointers[i], sizeof(uint8_t) * (section_headers + i)->size_of_raw_data) == sizeof(uint8_t) * (section_headers + i)->size_of_raw_data, "Failed to copy code from header.\n");
    }


	close(fd);
    return pe;
}


void free_PE_file(PE_file_t *pe){
    if(pe->dos_header != NULL){
        free(pe->dos_header);
    }
    if(pe->coff_header != NULL){
        free(pe->coff_header);
    }
    if(pe->shared_header != NULL){
        free(pe->shared_header);
    }
    if(pe->section_headers != NULL){
        free(pe->section_headers);
    }
    if(pe->string_table != NULL){
        free(pe->string_table);
    }
    for(int i = 0; i < pe->n_sections; i++){
        free(pe->code_pointers[i]);
    }
    if(pe-> pe_header!= NULL){
        free(pe->pe_header);
    }
    if(pe->pe_plus_header != NULL){
        free(pe->pe_plus_header);
    }
    free(pe);
}


void print_windows_specific_header(PE_file_t *pe){
    if(!pe->isplus){
        printf("\t\t Num data pointers:     0d %d\n", pe->pe_header->number_of_rva_and_sizes);
        printf("\t\t Image base pref.:      0x %x\n", pe->pe_header->image_base);
        printf("\t\t Section Alignment:     0d %d\n", pe->pe_header->section_alignment);
        printf("\t\t File Alignment:        0d %d\n", pe->pe_header->file_alignment);
        printf("\t\t RESERVED(1,2,3):       0d %d, %d, %d\n", pe->pe_header->reserved1, pe->pe_header->reserved2_rva,  pe->pe_header->reserved3_rva);
        printf("\t\t subsytem:              0x %x\n", pe->pe_header->subsystem);
        
        printf("\t\t image size:            0d %d\n", pe->pe_header->image_size);
        printf("\t\t header size:           0d %d\n", pe->pe_header->header_size);
        printf("\t\t check sum:             0d %d\n", pe->pe_header->checksum);
        printf("\t\t subsytem:              0d %d\n", pe->pe_header->subsystem);
        printf("\t\t loader_flags:          0d %d\n", pe->pe_header->loader_flags);

        printf("\t\t Maj/Min - Op-sys:      0d %d/%d\n", pe->pe_header->major_operating_system_version, pe->pe_header->minor_operating_system_version);
        printf("\t\t Maj/Min - Image:       0d %d/%d\n", pe->pe_header->major_image_version, pe->pe_header->minor_image_version);
        printf("\t\t Maj/Min - Sub-sys:     0d %d/%d\n", pe->pe_header->major_subsystem_version, pe->pe_header->minor_subsystem_version);
    
        printf("\n");
        printf("\t\t stack_reserve_size:    0x %x\n", pe->pe_header->stack_reserve_size);
        printf("\t\t stack_commit_size:     0x %x\n", pe->pe_header->stack_commit_size);
        printf("\t\t heap_reserve_size:     0x %x\n", pe->pe_header->heap_reserve_size);
        printf("\t\t heap_commit_size:      0x %x\n", pe->pe_header->heap_commit_size);
        printf("\t\t number of rva entries: 0d %d\n", pe->pe_header->number_of_rva_and_sizes);
        if(pe->pe_header->number_of_rva_and_sizes <= 16){
            for(int i = 0; i < pe->pe_header->number_of_rva_and_sizes; i+=1){
                if(*((&pe->pe_header->export_table_rva) + i*2) != 0){
                    printf("\t\t directory table/entry %s exists\n", directories_names[i]);
                } else{
                    printf("\t\t directory table/entry %s does not exist\n", directories_names[i]);    
                }
            }
        }
  
    } else {
        printf("\t\t Image base pref.:      0x %lx\n", pe->pe_plus_header->image_base);
        printf("\t\t Section Alignment:     0d %d\n", pe->pe_plus_header->section_alignment);
        printf("\t\t File Alignment:        0d %d\n", pe->pe_plus_header->file_alignment);
        printf("\t\t RESERVED(1,2,3):       0d %d, %d, %d\n", pe->pe_plus_header->reserved1, pe->pe_plus_header->reserved2_rva,  pe->pe_plus_header->reserved3_rva);

        printf("\t\t Maj/Min - Op-sys:      0d %d/%d\n", pe->pe_plus_header->major_operating_system_version, pe->pe_plus_header->minor_operating_system_version);
        printf("\t\t Maj/Min - Image:       0d %d/%d\n", pe->pe_plus_header->major_image_version, pe->pe_plus_header->minor_image_version);
        printf("\t\t Maj/Min - Sub-sys:     0d %d/%d\n", pe->pe_plus_header->major_subsystem_version, pe->pe_plus_header->minor_subsystem_version);
        printf("\t\t image size:            0d %d\n", pe->pe_plus_header->image_size);
        printf("\t\t header size:           0d %d\n", pe->pe_plus_header->header_size);
        printf("\t\t check sum:             0d %d\n", pe->pe_plus_header->checksum);
        printf("\t\t subsytem:              0d %d\n", pe->pe_plus_header->subsystem);

        printf("\t\t dll_characteristics:   0d %d\n", pe->pe_plus_header->dll_characteristics);
        printf("\t\t stack_reserve_size:    0d %ld\n", pe->pe_plus_header->stack_reserve_size);
        printf("\t\t stack_commit_size:     0d %ld\n", pe->pe_plus_header->stack_commit_size);
        printf("\t\t heap_reserve_size:     0d %ld\n", pe->pe_plus_header->heap_reserve_size);
        printf("\t\t heap_commit_size:      0d %ld\n", pe->pe_plus_header->heap_commit_size);
        printf("\t\t loader_flags:          0d %d\n", pe->pe_plus_header->loader_flags);


        if(pe->pe_plus_header->number_of_rva_and_sizes < 16){
            for(int i = 0; i < pe->pe_plus_header->number_of_rva_and_sizes; i+=1){
                if((&pe->pe_plus_header->export_table_rva) + i*2 != 0){
                    printf("\t\t directory table/entry %s exists", directories_names[i]);
                } else{
                    printf("\t\t directory table/entry %s does not exist", directories_names[i]);    
                }
            }
        }

    }
}

void print_section_headers(PE_file_t *pe){
    Section_header_t *sh_top = pe->section_headers;
    Section_header_t *sh;

    for(int i = 0; i < pe->n_sections; i++){
        sh = sh_top + i;
        printf("\t Section #%d\n", i + 1);
        if(sh->name[0] != '/')
        {
        printf("\t\t Name:                %s\n", sh->name);
        }
        else
        {
        printf("\t\t Name:                %s\n", pe->string_table + atoi(sh->name + 1));
        }
        printf("\t\t Virtual size:        0x %x\n", sh->virtual_size);
        printf("\t\t Virtual address:     0x %x\n", sh->virtual_address);
        printf("\t\t Section size:        0x %x\n", sh->size_of_raw_data);
        printf("\t\t Section start:       0x %x\n", sh->pointer_to_raw_data);
        printf("\t\t Characteristics:     0x %08x\n", sh->characteristics);
        printf("\t\t Relocation pointer:  0x %x\n", sh->pointer_to_relocations);
        printf("\t\t Line numbers:        0x %x\n", sh->pointer_to_linenumbers);
        printf("\t\t Number of reloc.:    0x %x\n", sh->number_of_relocations);
        printf("\t\t Number of line nums: 0x %x\n", sh->number_of_linenumbers);
    }
}

void get_code_range(PE_file_t *pe, uint32_t *code_start, uint32_t *code_end, uint32_t *code_size){
    Section_header_t *sh_top = pe->section_headers;
    Section_header_t *sh;

    *code_start = -1;
    *code_end = 0;
    *code_size = 0;
    for(int i = 0; i < pe->n_sections; i++){
        sh = sh_top + i;
        if(sh->pointer_to_raw_data + pe->dos_header->offset < *code_start){
            *code_start = sh->pointer_to_raw_data + pe->dos_header->offset;
        }
        if(sh->pointer_to_raw_data + pe->dos_header->offset + sh->size_of_raw_data > *code_end){
            *code_end = sh->pointer_to_raw_data + pe->dos_header->offset + sh->size_of_raw_data;
        }
        *code_size += sh->size_of_raw_data;
    }
}



void print_data_directories(PE_file_t *pe, flags_t * flags){
    int n = 0;
    uint32_t *base_ptr = NULL;
    uint64_t image_base, base_of_entry, size_of_entry;
    if(pe->isplus){
        n = pe->pe_plus_header->number_of_rva_and_sizes;
        base_ptr = (uint32_t *)&pe->pe_plus_header->export_table_rva;
        image_base =  pe->pe_plus_header->image_base;
    } else{
        n = pe->pe_header->number_of_rva_and_sizes;
        base_ptr = (uint32_t *)&pe->pe_header->export_table_rva;
        image_base =  pe->pe_header->image_base;
    }

    for(int i = 0; i < n; i++){
        size_of_entry = *(base_ptr + 2*i + 1);
        base_of_entry = *(base_ptr + 2*i);
        if(size_of_entry != 0){
            printf("\n0x%lx - 0x%lx: %s", image_base + base_of_entry, image_base + base_of_entry + size_of_entry, directories_names[i]);
        }
    }
    printf("\n");
}


bool empty_struct(uint8_t *p, uint32_t size){
    
    for(int i =0; i < size; i++){
        if(p[i] != 0){
            return false;
        }    
    }

    return true;
}

uint8_t *rva_to_pointer(PE_file_t *pe, uint64_t rva, int valid_range){
    
    Section_header_t *sh = NULL;
    uint8_t *ans = NULL;
    
    for(int i = 0; i < pe->n_sections; i++){
        sh = pe->section_headers + i;
        if(rva >= sh->virtual_address && rva < sh->virtual_address + sh->virtual_size){
            if(rva + valid_range > sh->virtual_address + sh->virtual_size){
                return NULL;
            }
            passert(rva - sh->virtual_address <= sh->size_of_raw_data, "Accessing uninitialized virtual data");
            ans = (uint8_t *)(pe->code_pointers[i] + (rva - sh->virtual_address));
            return ans;
        }
    }
    return NULL;

}


void print_reloc_table(PE_file_t *pe, flags_t *flags){
    uint32_t reloc_rva = 0;
    uint32_t reloc_size = 0;

    if(pe->isplus){
        reloc_rva = pe->pe_plus_header->base_relocation_table_rva;
        reloc_size = pe->pe_plus_header->base_relocation_table_size;
    } else{
        reloc_rva = pe->pe_header->base_relocation_table_rva;
        reloc_size = pe->pe_header->base_relocation_table_size;
    }

    if(reloc_rva == 0 || reloc_size == 0){
        printf("No base relocation table\n");
    }

    uint16_t entry;
    uint32_t blk_size;
    uint8_t *p = rva_to_pointer(pe, reloc_rva, reloc_size);
    uint8_t *end = p + reloc_size;
    
    int n = 0;
    while(p < end){
        printf("Page relocation #%d", n);
        printf("\t\t Applies to page at RVA: 0x%x\n", *(uint32_t *)p);
        p += 4;
        blk_size = *(uint32_t *)p;
        p += 4;
        printf("Block size = %d\n", blk_size);
        passert(blk_size%4 == 0, "Error with relocation table - blocks must be aligned to 32b size");
        for(int i = 0; i < blk_size/2; i++){
            entry = *(uint16_t *)p;
            printf("ENTRY %x\n", entry);
            p += 2;
            if((entry & 0xF000) == 0){continue;}
            printf("\t\t\t Entry #%d: type: 0d%d, offset: 0x%x\n", i, (entry & 0xF000), (entry & 0xFFF));
        }

    }

}





void print_import_data_table(PE_file_t *pe, flags_t *flags){
    uint32_t idt_rva = 0;
    uint32_t ida_size = 0;

    if(pe->isplus){
        idt_rva = pe->pe_plus_header->import_table_rva;
        ida_size = pe->pe_plus_header->import_table_size;
    } else{
        idt_rva = pe->pe_header->import_table_rva;
        ida_size = pe->pe_header->import_table_size;

    }
    if(idt_rva == 0){
        printf("No import directory table\n");
        return ;
    }

    import_directory_entry_t *table_pointer;
    table_pointer = (import_directory_entry_t *)rva_to_pointer(pe, idt_rva, ida_size);
    char *str;

    uint32_t ilt_entry;
    void *ptr;
    int i = 0;
    //printf("%x", idt_rva);
    passert(table_pointer != NULL, "ERROR: Could not find raw section code for import data table");
    int n = 1;
    printf("\nPrinting Import address table/DLLs...\n");
    while(!empty_struct((uint8_t *) table_pointer, sizeof(import_directory_entry_t))){
        
        
        printf("\t DLL #%d\n", n);
        printf("\t\t Import lookup table RVA:       0x%x, (One entry for every import in this dll)\n", table_pointer->iprt_lookup_table_rva);
        
        if(!pe->isplus){
            i = 0;
            ilt_entry = 1;
            while(ilt_entry != 0){
                
                ptr = (void *)rva_to_pointer(pe, table_pointer->iprt_lookup_table_rva + 4*i, 4);
                passert(ptr != NULL, "ERROR: Could not find import lookup table from given RVA");
                ilt_entry = *(uint32_t *)ptr;
                if(ilt_entry == 0){break;}
                
                printf("\t\t\t DLL link #%d",i);
                if((0x80000000 & ilt_entry) == 0){    // import by name
                    printf("\t\t\t\t Name: %s\n", rva_to_pointer(pe, ilt_entry + 2, 0));
                    printf("\t\t\t\t Hint: %d\n", *(uint16_t *)rva_to_pointer(pe, ilt_entry, 0));
                } else{
                    printf("\t\t\t\t Name by ordinal... %d\n", ilt_entry & 0xFFFF);
                }
                passert(i < 200, "ERROR: limit of dll function entries reached");
                i += 1;

            }
        } else {
            i = 0;
            uint64_t ilt_entry_64 = 1;
            uint64_t *prt_64 = NULL;
            while(ilt_entry_64 != 0){
                
                prt_64 = (void *)rva_to_pointer(pe, table_pointer->iprt_lookup_table_rva + 8*i, 8);
                passert(prt_64 != NULL, "ERROR: Could not find import lookup table from given RVA");
                ilt_entry_64 = *(uint64_t *)prt_64;
                if(ilt_entry_64 == 0){break;}
                
                printf("\t\t\t DLL link #%d",i);
                if((0x8000000000000000 & ilt_entry_64) == 0){    // import by name
                    printf("\t\t\t\t Name: %s\n", rva_to_pointer(pe, ilt_entry_64 + 2, 0));
                } else{
                    printf("\t\t\t\t Name by ordinal... %ld\n", ilt_entry_64 & 0xFFFF);
                }
                passert(i < 200, "ERROR: limit of dll function entries reached");
                i += 1;

            }
        }

        printf("\t\t Forwarder chain:               0x%x\n", table_pointer->forwarder_chain);
        printf("\t\t Name pointer RVA:              0x%x, name: %s\n", table_pointer->name_rva, rva_to_pointer(pe, table_pointer->name_rva, 0));
        printf("\t\t Import addres table RVA:       0x%x\n", table_pointer->iprt_addr_table_rva);


        n += 1;
        table_pointer += 1;
        passert(n < 100, "ERROR: limit of import table entries reached");
    }

}







void print_virtual_sections(PE_file_t *pe, flags_t *flags){


    int sec_start;
    int sec_end;
    uint32_t image_base, entry_addr, header_end, base_of_code, image_top;

    if(pe->isplus){
        image_base = pe->pe_plus_header->image_base;
        image_top = image_base + pe->pe_plus_header->image_size;
        header_end = image_base + pe->pe_plus_header->header_size;
    } else {
        image_base = pe->pe_header->image_base;
        image_top = image_base + pe->pe_header->image_size;
        header_end = image_base + pe->pe_header->header_size;
    }
    entry_addr = image_base + pe->shared_header->entry_point_address;
    base_of_code = image_base + pe->shared_header->base_of_code;



    printf("Addresses given in virtual address form in RAM\n");
    printf("\n0x%x - 0x%x: Full image range\n", image_base, image_top);

    printf("\n0x%x - 0x%x: Headers\n", image_base, header_end);    
    printf("\n0x%x: Base of code\n", base_of_code);
    printf("\n0x%x: Entry point RVA\n", entry_addr);

    print_data_directories(pe, flags);

    printf("\n");
    if(flags->disp_virtual_code){
        print_code(pe, flags);
    } else{
        printf("print all code/data sections <q>\n");
    }

    if(flags->import_table){
        print_import_data_table(pe, flags);
    } else{
        printf("print import address table <i>\n");
    }
    if(flags->relocation_table){
        print_reloc_table(pe, flags);
    } else{
        printf("print relocation table <r>\n");
    }
}


int main(int argc, char *argv[]) {

	// Check if arguments are vaild
	if (argc < 2) {
	    printf("Usage: %s <filename> <-flags>\n", argv[0]);
	    return -1;
	}

    // Open .exe file
    int fd_in = open(argv[1], O_RDONLY);
    passert(fd_in != -1, "Failed to open the file.\n");
    
    // Parse the input flags
    flags_t *flags = parse_flags(argv, argc);
    passert(flags != NULL, "Failed to read cmd args");

    // Parse and print the PE file
    PE_file_t *pe = parse_PE_file(fd_in);
    
    if(flags->ram_contents){
        print_virtual_sections(pe, flags);
    } else {
        print_PE_file(pe, flags);
    }

    free_PE_file(pe);
    free(flags);

	return 0;
}

/*
    - In windows, PE files have to be loaded in from the disk : bytes will be transfered to memeory before executing
        - File offset is a address from the base of the file on disk.
    - The next step is to load it into memory
        - It will start at the virtual address of 'image base'
        - things can then be addressed by the 'real virtual addresses'
            - entry point of code -> addressed with real virtual address
        - Headers are also loaded into memory!
            - this means you cant have code sections with a RVA < header size


*/


