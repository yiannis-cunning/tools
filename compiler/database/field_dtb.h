

typedef struct{
    int16_t w, s, reg, reg1, reg2, modA, mod, rm, is_immd8, is_immd, is_disp8, is_disp16, is_disp, dsave;
    int16_t portnumber, type, sreg2, sreg3, tttn, eee, is_offsel;

    int8_t disp_sz, immd_sz;
    int8_t scale, index, byte;

    uint64_t disp;
    uint64_t immd;

    uint16_t code_seg_ind;
    uint64_t offset;

    opad_sz_t op_sz;
    opad_sz_t ad_sz;

    uint8_t total_bit_size;

} field_t;

typedef struct {
    char *str;
    int str_len;
    int sz;
    int indx;
}symbol_t;

// Order matters
static symbol_t symbols[] = {
    {"0", sizeof("0"), 1, offsetof(field_t, dsave)},
    {"1", sizeof("1"), 1, offsetof(field_t, dsave)},
    {"w", sizeof("w"), 1, offsetof(field_t, w)}, 
    {"s", sizeof("s"), 1, offsetof(field_t, s)}, 
    {"reg1", sizeof("reg1"), 3, offsetof(field_t, reg1)}, 
    {"reg2", sizeof("reg2"), 3, offsetof(field_t, reg2)}, 
    {"reg", sizeof("reg"), 3, offsetof(field_t, reg)}, 
    {"modA", sizeof("modA"), 2, offsetof(field_t, modA)}, 
    {"mod", sizeof("mod"), 2, offsetof(field_t, mod)}, 
    {"r/m", sizeof("r/m"), 3, offsetof(field_t, rm)}, 
    {"portnumber", sizeof("portnumber"), 8, offsetof(field_t, portnumber)}, 
    {"type", sizeof("type"), 8, offsetof(field_t, type)}, 
    {"sreg2", sizeof("sreg2"), 2, offsetof(field_t, sreg2)}, 
    {"sreg3", sizeof("sreg3"), 3, offsetof(field_t, sreg3)}, 
    {"tttn", sizeof("tttn"), 4, offsetof(field_t, tttn)},
    {"eee", sizeof("eee"), 3, offsetof(field_t, eee)}, 
    {"--", sizeof("--"), 2, offsetof(field_t, dsave)}, 

    {"immediatedata", sizeof("immediatedata"), 0, offsetof(field_t, is_immd)}, 
    {"imm8data", sizeof("imm8data"), 0, offsetof(field_t, is_immd8)}, 
    {"fulldisplacment", sizeof("fulldisplacment"), 0, offsetof(field_t, is_disp)}, 
    {"8-bitdisplacment", sizeof("8-bitdisplacment"), 0, offsetof(field_t, is_disp8)}, 
    {"16-bitdisplacment", sizeof("16-bitdisplacment"), 0, offsetof(field_t, is_disp16)}, 

    {"unisignedfulloffset,selector", sizeof("unisignedfulloffset,selector"), 0, offsetof(field_t, is_offsel)}

};

static int n_symbols = sizeof(symbols) / sizeof(symbols[0]);