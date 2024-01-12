#include "asb.h"





// return -1 for buffer overflow, 0 for success, 1 for reached EOF, crash if error with file
int read_until_nl(int fd, char *buffer, int buf_sz){

    int i = 0;
    int n = 0;


    n  = read(fd, buffer + i, 1);
    passert(n == 0 || n == 1, "error reading from file");
    if(n == 0){*(buffer + i) = '\0'; return 1;}
    while(*(buffer + i) != '\n'){
        i += 1;
        if(i == buf_sz){buffer[buf_sz - 1] = '\0'; return -1;}
        n  = read(fd, buffer + i, 1);
        if(n == 0){*(buffer + i) = '\0'; return 1;}
    }
    *(buffer + i) = '\0';
    return 0;
}

int pass_whitespace(char *buffer){
    int j = 0;
    while(buffer[j] == ' ' || buffer[j] == '\t'){
        j += 1;
    }
    return j;
}

// returns 0 on no match, -1 on a non-space ending match, indx of first non-white char after space if space-ending
int match_to_directive(char *buffer, char *str){
    int j = 0;

    j += pass_whitespace(buffer);
    if(strncmp(buffer + j, str, strlen(str)) == 0){
        j += strlen(str);
        if(*(buffer + j) != ' '){
            return -1;
        }
        j += pass_whitespace(buffer + j);
        return j;
    }
    return 0;
}


int read_line(char *buffer, asm_file_t *parsed, int line_num){
    
    
    
    int j = 0;
    // returns indx of first char in arg in buffer
    j = match_to_directive(buffer, ".machine");
    passert_syntax(j != -1, "spacing error", buffer, line_num);
    if(j != 0){
        passert_syntax(parsed->machine_type == 0, "multiple definitions of machine type", buffer, line_num);
        int machine_type = 0;
        if(strncmp("x86", buffer + j, 3) == 0){
            machine_type = 86;
        }
        if(strncmp("x64", buffer + j, 3) == 0){
            machine_type = 64;
        }
        //printf("arg: %s\n", buffer + j);
        passert_syntax(*(buffer + j + 3 + pass_whitespace(buffer + j + 3)) == '\0', "cannot read args for machine type", buffer, line_num );
        parsed->machine_type = machine_type;
        return 1;
    }

    j = match_to_directive(buffer, ".file_alignment");
    passert_syntax(j != -1, "spacing error", buffer, line_num);
    if(j != 0){
        passert_syntax(parsed->file_alignment == 0, "multiple definitions of file alignment", buffer, line_num);
        int file_alignment = 0;
        file_alignment = atoi(buffer + j);
        passert_syntax(file_alignment != 0, "invalid file alignment", buffer, line_num);

        //passert_syntax(*(buffer + j + 3 + pass_whitespace(buffer + j + 3)) == '\0', "cannot read args for machine type on line %d", line_num );
        parsed->file_alignment = file_alignment;
        return 1;
    }

    j = match_to_directive(buffer, ".section_alignment");
    passert_syntax(j != -1, "spacing error", buffer, line_num);
    if(j != 0){
        passert_syntax(parsed->section_alignment == 0, "multiple definitions of section alignment", buffer, line_num);
        int section_alignment = 0;
        section_alignment = atoi(buffer + j);
        passert_syntax(section_alignment != 0, "invalid section alignment", buffer, line_num);

        //passert_syntax(*(buffer + j + 3 + pass_whitespace(buffer + j + 3)) == '\0', "cannot read args for machine type on line %d", line_num );
        parsed->section_alignment = section_alignment;
        return 1;
    }

    return 0;

}






int main(int argc, char *argv[]){

    passert(argc == 2, "usage: asb <filename.asm>");
    
    int fd = open(argv[1], O_RDONLY);
    passert(fd != -1, "ERROR: cannot open file given");





    char buffer[4096];
    asm_file_t *parsed = (asm_file_t *)calloc(sizeof(asm_file_t), 1);



    int line_num = 1;
    int ret = 0;
    int j, i;

    while(1){
        ret = read_until_nl(fd, buffer, 4096);
        passert(ret != -1, "Buffer overflow error!")
        if(ret == 1){break;}
        line_num += 1;



        //printf("%s\n", buffer);
        if(read_line(buffer, parsed, line_num) == 1){
            continue;
        }       
        if(*(buffer + pass_whitespace(buffer)) == '\0'){
            continue; // empty line
        }

        j = strncmp(buffer + pass_whitespace(buffer), ".section", strlen(".section"));
        if(j == 0){

            // 1) find out code vs data section
            int is_code = 0;
            j = pass_whitespace(buffer) + strlen(".section");
            if(strncmp(buffer + j, ".code", 5) == 0){
                    is_code = 1;
            }
            passert_syntax(strncmp(buffer + j, ".data", 5) == 0, "need to declare code/data section", buffer, line_num);
            j += 5

            // 2) extract name
            i = j;
            // want to confirm: letters + less than 7 long, no other args.
            while(buffer[i] != ' ' && buffer[i] != '\0'){
                i += 1;
            }
            passert_syntax(i - j <= 7, "section name too long", buffer, line_num);
            passert_syntax( *(buffer + pass_whitespace(buffer + i)) == '\0', "invalid section start", buffer, line_num);
            

            // 3) make new section block to store infor
            section_t *new_section = (section_t *)calloc(sizeof(section_t), 1);
            memcpy(new_section->name, buffer + j, i - j);
            new_section->name[i-j] = '\0';
            new_section->next = NULL;

            // 3) copy bytes of data section

        }


// .section.text
//         j    i
//         3    8
    }




}


/*
directives:

1) .machine str
    - accepted str: x86, x64


*/