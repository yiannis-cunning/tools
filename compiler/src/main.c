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

} instruction_t;*/



bool all_zeros(char *buffer, int n){
    for(int i = 0; i < n; i++){
        if(buffer[i] != "0"){
            return false;
        }
    }
    return true;
}

int main(int argc, char **argv){
    
    if(argc != 1){
        printf("Invalid run opts\n");
        return -1;
    }

    char *char_buffer[128]; // 16  bytes
    int n, bytes_used;

    // 1. Read first 128 bits / 16 bytes into buffer
    passert(read(0, char_buffer, 128) == 128, "Failed to read more from stdin");
    while(all_zeros(char_buffer, 128) == false)
    {
        // 2. Print the instuction (returns how many bytes that instuction was)
        bytes_used = print_instruction(char_buffer);
        passert(bytes_used > 0, "Could not decode instuction.");
        
        // 3. shift bits -- bits_used -> 127 -- to 0 -> 127-bits_used
        for(int j = 0; j < 128 - bytes_used; j++){
            char_buffer[j] = char_buffer[bytes_used*8 + j];
        }
                                   
        n = read(0, char_buffer + bytes_used*8, 128 - bytes_used*8);
        passert(n == 128-bytes_used*8, "Failed to read more from stdin");
    }
    
};