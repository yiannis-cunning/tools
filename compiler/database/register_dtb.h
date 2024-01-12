


// reg_dtb[op_sz][reg]
static char *register_dtb[4][8] = 
{
{"AL", "CL", "DL", "BL", "AH", "CH", "DH", "BH"},
{"AX", "CX", "DX", "BX", "SP", "BP", "SI", "DI"},
{"EAX", "ECX", "EDX", "EBX", "ESP", "EBP", "ESI", "EDI"},
{"RAX", "RCX", "RDX", "RBX", "RSP", "RBP", "RSI", "RDI"}
};

// special_reg_dtb[eee][isdebug]
static char *special_reg_dtb[8][2] = {
    {"CR0",         "DR0"},
    {"Reserved",   "DR1"},
    {"CR2",         "DR2"},
    {"CR3",         "DR3"},
    {"CR4",         "Reserved"},
    {"Reserved",    "Reserved"},
    {"Reserved",    "DR6"},
    {"Reserved",    "DR7"}
};

// sreg2_dtb[sreg2]
static char *sreg2_dtb[4] = {"ES", "CS", "SS", "DS"};

// sreg3_dtb[sreg3]
static char *sreg3_dtb[8] = {"ES", "CS", "SS", "DS", "FS", "GS", "RESERVED", "RESERVED"};
