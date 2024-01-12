// given addressing mode 16 v 32, uint8_t mod, uint8_t rm, addr
// Assume already know 1) if have disp 2) what size disp 3) if there is SIB 4) if mod = 3


// disp_size_dtb[addrsz][mod][rm] - Table only inaccurate when there is a SIB overide (from 0 to 32)
static uint8_t disp_size_dtb[2][4][8] = {
    {   {0, 0, 0, 0, 0, 0, 16, 0},
        {8, 8, 8, 8, 8, 8, 8, 8},
        {16, 16, 16, 16, 16, 16, 16, 16},
        {0, 0, 0, 0, 0, 0, 0, 0}},
    {   {0, 0, 0, 0, 0, 32, 0, 0},
        {8, 8, 8, 8, 8, 8, 8, 8},
        {32, 32, 32, 32, 32, 32, 32, 32},
        {0, 0, 0, 0, 0, 0, 0, 0}}
};

// memory_dtb[addsz][mod][rm] // just reg when rm = b11
static char *memory_dtb[2][3][8] = {
    {   {"[BX + SI]", "[BX + DI]", "[BP + SI]", "[BP + DI]", "[SI]", "[DI]", "0",    "[BX]"},
        {"[BX + SI]", "[BX + DI]", "[BP + SI]", "[BP + DI]", "[SI]", "[DI]", "[BP]", "[BX]"},
        {"[BX + SI]", "[BX + DI]", "[BP + SI]", "[BP + DI]", "[SI]", "[DI]", "[BP]", "[BX]"}},
    {   {"[EAX]", "[ECX]", "[EDX]", "[EBX]", "SIB", "0",     "[ESI]", "[EDI]"},
        {"[EAX]", "[ECX]", "[EDX]", "[EBX]", "SIB", "[EBP]", "[ESI]", "[EDI]"},
        {"[EAX]", "[ECX]", "[EDX]", "[EBX]", "SIB", "[EBP]", "[ESI]", "[EDI]"}}
};


// scaled_index_dtb[scale][index]
static char *scaled_index_dtb[4][8] = {
    {"[EAX]",  "[ECX]",  "[EDX]",  "[EBX]",  "none","[EBP]",  "[ESI]",  "[EDI]"},
    {"[EAX*2]","[ECX*2]","[EDX*2]","[EBX*2]","none","[EBP*2]","[ESI*2]","[EDI*2]"},
    {"[EAX*4]","[ECX*4]","[EDX*4]","[EBX*4]","none","[EBP*4]","[ESI*4]","[EDI*4]"},
    {"[EAX*8]","[ECX*8]","[EDX*8]","[EBX*8]","none","[EBP*8]","[ESI*8]","[EDI*8]"}
};


// base_dtb[mod][base]
static char *base_dtb[4][8] = {
    {" + [EAX]", " + [ECX]"," + [EDX]"," + [EBX]"," + [ESP]",        ""," + [ESI]"," + [EDI]"},
    {" + [EAX]", " + [ECX]"," + [EDX]"," + [EBX]"," + [ESP]"," + [EBP]"," + [ESI]"," + [EDI]"},
    {" + [EAX]", " + [ECX]"," + [EDX]"," + [EBX]"," + [ESP]"," + [EBP]"," + [ESI]"," + [EDI]"},
    {" + [EAX]", " + [ECX]"," + [EDX]"," + [EBX]"," + [ESP]"," + [EBP]"," + [ESI]"," + [EDI]"}
};

