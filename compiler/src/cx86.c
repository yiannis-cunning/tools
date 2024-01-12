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

#include "/tools/compiler/database/dtb.h"
#include "/tools/compiler/include/cx86.h"



void passert(bool tf, char *pnt){
	if(!tf){
        printf("%s", pnt);
	    exit(1);
	}
}

/*// 1) Take in any add instuction and print out what the ASM instuction is?
register1 to register2      0000 000w : 11 reg1 reg2
register2 to register1      0000 001w : 11 reg1 reg2
memory to register          0000 001w : mod reg r/m
register to memory          0000 000w : mod reg r/m
immediate to register       1000 00sw : 11 000 reg : immediate data
immediate to AL, AX, or EAX 0000 010w : immediate data
immediate to memory         1000 00sw : mod 000 r/m : immediate data

// Instunction is 
OPCODE = ADD SUB MULT ORR AND INC DEC...
OPERAND = EAX ESP AX RBX EFLAGS [EAX] immm16 disp32 [EAX + immm16]*/


/*
Opcode consists of 
    (1) Prefixes -> delt with
    (2) Opcode -> everything until reg1/reg2/reg3/rm/mod
    (3) SIB -> --
    (4) Displacment
    (5) Immediate
*/



// Given a string pointer to 0/1 bit stream, return int value for n_bits of that stream 
// Values are little-endian
uint64_t batoi(char *str, int n_bits){
    int n = n_bits;
    if(n > 8){n = 8;}
    uint64_t ans = 0;
    for(int i = 0; i < n; i++){
        ans += (str[i] == '1')*((1 << (n-1)) >> i);
    }

    if(n_bits <= 8){
        return ans;
    }
    else{
        return ans + (batoi(str + 8, n_bits - 8) << 8);
    }

}

// Ensure first n bits of a and b are equal
int strcmpn(char *a, char *b, int n){
    for(int i = 0; i < n; i++){
        if(a[i] != b[i]){
            return false;
        }
    }
    return true;
}

/*
// return what symbol is being used in the instuc_dtb
symbol_t *match_symbol(char *buff_inst_code){
    for(int j = 0; j < n_fields; j ++){
        if(strcmpn(buff_inst_code, fields[j]->str, fields[j]->str_len)){
            return &fields[j];
        }
    }
    return NULL;
}

// Set all fields to -1, or 0 for unisned
field_t *mkparms(){
    field_t *parms = (field_t *)calloc(sizeof(field_t), 1);
    int8_t *p = parms;
    for(int i = 0; i < n_fields + 5; i++){
        p[i] = -1;
    }
    parms->disp = 0;
    parms->immd = 0;
    parms->op_sz = 0;
    parms->ad_sz = 0;

    parms->total_bit_size = 0;
    return parms;
}
*/







/*
field_t *extract_from_inst(char *buff_in, instruction_entry_t *inst_code){
    field_t *parms = (field_t *)calloc(sizeof(field_t), 1);
    opad_sz_t op_sz = BIT32;
    opad_sz_t ad_sz = BIT32;
    int16_t *vals = parms->w;

    // 1) Just read the Opcode field + modr/m
    char *inst = inst_code->inst_bits;
    int j = 0;
    int i = 0;
    while(inst[j] != '\0'){
        if(inst[j] == ':'){
            passert(i % 8 == 0, "Error checking byte alignment...");
            j += 1;
            continue;
        }

        symbol_t *field = match_symbol(inst + j);
        passert(field_j != -1, "Error matching buffer in to instruction...");
        if((field->str[0] == "0" || field->str[0] == "1") && buff_in[i] != field->str[0]){
            free(parms); return NULL;
        }

        vals[field->indx >> 1] = batoi(buff_in + i, field->sz);

        j += field->str_len;
        i += field->sz;
    }

    if(parms->w == 0){op_sz = BIT8;}

    // 2) Read the SIB field
    if(parms->ad_sz == BIT32 && parms->mod != 3 && parms->rm == 4){
        vals[scale] = batoi(buff_in + i, 2);
        vals[index] = batoi(buff_in + i + 2, 3);
        vals[base] =  batoi(buff_in + i + 4, 3);
        i += 8;
    }

    // 3) Read the DISP field
    parms->disp_sz = (parms->is_disp8 == 0) ? 8 : (parms->is_disp16 == 0 ? 16 : (parms->is_disp == 0 ? 32 : 0)); // DONT know what fulldisp implies???
    if(parms->disp_sz == 0 && (parms->mod != -1 || parms->rm != -1)){
        // SIB overide to 32 when 1) is SIB w base 5, 2) mod = 0 (implies rm = 4)
        parms->disp_sz = (parms->base == 5 && parms->mod == 0 ) ? (32) : disp_size[ad_sz][mod][rm];
    }

    parms->disp = batoi(buff_in + i, parms->disp_sz);
    i += parms->disp_sz;


    // 4) Read the IMMD field
    parms->immd_sz = (parms->is_immd8) ? 8 : 0;
    if(parms->is_immd){
        parms->immd_sz = (parms->s == 1) ? 8 : (op_sz == BIT16)*16 + (op_sz == BIT32)*32 + (op_sz == BIT8)*8;
    }
    parms->immd = batoi(buff_in + i, parms->immd_sz);
    i += parms->immd_sz;

    parms->op_sz = op_sz;
    parms->ad_sz = ad_sz;


    // 5) Read unsigned offset selector - 4 / 6 bytes
    if(parms->is_offsel == 0){
        parms->code_seg_ind = batoi(buff_in + i, 16);
        i += 16;
        parms->offset = batoi(buff_in + i, (op_sz == BIT16)*16 + (op_sz != BIT16)*32);
        i += (op_sz == BIT16)*16 + (op_sz != BIT16)*32;
    }


    if(parms->modA != -1 && parms->modA != 3){
        free(parms); return NULL;
    }
    field->total_bit_size = i+1;

    return parms;
}




void print_inst(instruction_entry_t *inst_code, field_t *parms){
    // 1) Instruction numonic
    printf("%s", inst_code->numonic);
    if(parms->tttn != -1){
        printf("%s", cond_dtb[parms->tttn][0]);
    }
    printf(" ");

    // 2) registers
    if(parms->reg1 != -1 && parms->reg2 != -1){ // reg1 reg2
        printf("%s, %s ", register_dtb[parms->op_sz][parms->reg1], register_dtb[parms->op_sz][parms->reg2]);
    }
    else{
        if(parms->reg != -1){   // reg
            printf("%s ", register_dtb[parms->op_sz][parms->reg]);
        }
        if(parms->mod != -1 && parms->rm != -1)
        {
            if(parms->scale != -1) // w SIB
            {
                printf("%s ", scaled_index_dtb[parms->scale][parms->index]);
                printf("%s ", base_dtb[parms->mod][parms->base]);
            }
            else
            {
                if(parms->mod == 3){ 
                    printf("%s ", register_dtb[parms->op_sz][parms->rm]);
                }
                else{   // [reg] + disp ...
                    printf("%s ", memory_dtb[parms->ad_sz][parms->mod][parms->rm]);
                }
            }
            
            // Displacemnet 
            switch(parms->disp_sz){
                case(8):
                    printf("+ %dx%X ", 8, (int8_t)parms->disp);
                    break;
                case(16):
                    printf("+ %dx%X", 16, (int16_t)parms->disp);
                    break;
                case(32):
                    printf("+ %dx%X", 32, (int32_t)parms->disp);
                    break;
                default:
                    break;
            }

        }
    }
    // 3) sreg/eee - in dtb,     portnumber/type - uint8, offsel - uint16, uint16/32
    if(parms->sreg2 != -1){printf("seg:%s ", sreg2_dtb[parms->sreg2]);}
    if(parms->sreg3 != -1){printf("seg:%s ", sreg3_dtb[parms->sreg3]);}
    if(parms->eee != -1){printf("special:%s ", special_reg_dtb[parms->eee]);}

    if(parms->type != -1){printf("inttype:8d%d ", (uint8_t)parms->type);}
    if(parms->portnumber != -1){printf("port:8d%d ", (uint8_t)parms->portnumber);}

    if(parms->is_offsel != -1){
        printf("segment:16x%X ", (uint16_t)parms->code_seg_ind);
        if(parms->op_sz == BIT16){printf("offset:16x%X ", (uint16_t)parms->offset);}
        else{printf("offset:32x%X ", (uint32_t)parms->offset);}
    }


    // 3) immediate
    switch(parms->immd_sz){
        case(8):
            if(parms->s == 0){printf(", %dx%X", 8, (uint8_t)parms->immd);}
            else{printf(", %dx%X", 8, (int8_t)parms->immd);}
            break;
        case(16):
            printf(", %dx%X", 16, (int16_t)parms->immd);
  
        case(32):
            printf(", %dx%X", 32, (int32_t)parms->immd);
            break;
        case(8):
            printf(", %dx%X", 8, (int8_t)parms->immd);
            break;
    }

}
*/
#define N_MAX_PREFIX 4
typedef struct {
    bool is_prefix;
    uint8_t num_prefix;
    int prefix_value[N_MAX_PREFIX];

    uint8_t opcode_size;
    uint8_t opcode_value[3];
    //opcode_t opcode;

    bool is_modrsm;
    uint8_t modrsm_value;

    bool is_SIB;
    uint8_t SIB_value;

    bool is_disp;
    uint8_t disp_size;
    uint64_t disp_value;

    bool is_immd;
    uint8_t immd_size;
    uint64_t immd_value;

} instruction_t;


int match_prefix(char *buffer, instruction_t *inst){
    int npref = 0;
    uint8_t value = batoi(buffer, 8);
    
    bool match = true;

    while(match == true){
        match = false;
        value = batoi(buffer + 8*npref, 8);
        if(npref >= N_MAX_PREFIX){break;}

        for(int j =0; j < n_pref_dtb; j++){
            if(value == pref_dtb[j]){
                inst->prefix_value[npref] = j;
                npref += 1;
                match = true;
            }
        }
    }
    inst->is_prefix = true;
    inst->num_prefix = npref;
    return npref;
}



int match_opcode(char *buffer, instruction_t *inst){

}



int match_to_instruction(char *buffer){
    instruction_t *inst = (instruction_t *)calloc(sizeof(instruction_t), 1);

    int bytes_used;

    // 1) Prefixes
    bytes_used = match_prefix(buffer, inst);
    //printf("%d, %d, %d, %d\n", inst->prefix_value[0], inst->prefix_value[1], inst->prefix_value[2],inst->prefix_value[3]);

    buffer = buffer + bytes_used*8;
    // 2) opcodes

    return 4;

}



/*
int print_instruction(char *buffer){
    field_t *extracted_fields = NULL;

    // Check the buffer agianst all the instructions
    for(int j = 0; j < n_instructions; j++){
        extracted_fields = match_to_instruction(buffer, instruction[j]);
        if(extracted_fields != NULL){
            break;
        }
    }

    if(extracted_fields == NULL){
        return 0;
    }

    print_inst(inst_code, extracted_fields);
    passert(extracted_fields->total_bit_size % 8 == 0, "used non-byte multiple amount of bits");
    return extracted_fields->total_bit_size >> 3;   

}*/








bool all_zeros(char *buffer, int n){
    for(int i = 0; i < n; i++){
        if(buffer[i] != '0'){
            return false;
        }
    }
    return true;
}

int get_n(int fd, char *buffer, int n){
    int tlt = 0;
    int j = 0;

    while(tlt < n){
        j = read(0, buffer + tlt, 1);
        if(j != 0 && j != 1){return -1;}
        tlt += j;
    }
    return tlt;
}


int main(int argc, char **argv){
    
    if(argc != 1){
        printf("Invalid run opts\n");
        return -1;
    }

    char char_buffer[128]; // 16  bytes
    int n, bytes_used;

    // 1. Read first 128 bits / 16 bytes into buffer
    passert(get_n(0, char_buffer, 128) == 128, "Failed to read from stdin");
    while(all_zeros(char_buffer, 128) == false)
    {
        // 2. Print the instuction (returns how many bytes that instuction was)
        bytes_used = match_to_instruction(char_buffer);
        passert(bytes_used > 0, "Could not decode instuction.");
        //write(1, char_buffer, bytes_used*8);
        // 3. shift bits -- bits_used -> 127 -- to 0 -> 127-bits_used
        for(int j = 0; j < 128 - bytes_used*8; j++){
            char_buffer[j] = char_buffer[bytes_used*8 + j];
        }

        n = get_n(0, char_buffer + 128 - bytes_used*8, bytes_used*8);

        passert(n == bytes_used*8, "Failed to read more from stdin");
    }
    
};

/*
typedef struct {
    bool is_prefix;
    uint8_t num_prefix;
    prefix_t prefix_value[N_MAX_PREFIX];

    uint8_t opcode_size;
    uint8_t opcode_value[3];
    opcode_t opcode;

    bool is_modrsm;
    uint8_t modrsm_value;

    bool is_SIB;
    uint8_t SIB_value;

    bool is_disp;
    uint8_t disp_size;
    uint64_t disp_value;

    bool is_immd;
    uint8_t immd_size;
    uint64_t immd_value;

} instruction_t;


int main{

    
}*/