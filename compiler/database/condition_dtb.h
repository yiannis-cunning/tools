typedef struct{
    char *numonic;
    char *description;
} cond_t;

// cond_dtb[tttn][0]
static cond_t cond_dtb[] = {
    {"O", "Overflow"},
    {"NO", "No overflow"},
    {"B", "NAE Below, Not above or equal"},
    {"NB", "AE Not below, Above or equal"},
    {"E", "Z Equal, Zero"},
    {"NE", "NZ Not equal, Not zero"},
    {"BE", "NA Below or equal, Not above"},
    {"NBE", "A Not below or equal, Above"},
    {"S", "Sign"},
    {"NS", "Not sign"},
    {"P", "PE Parity, Parity Even"},
    {"NP", "PO Not parity, Parity Odd"},
    {"L", "NGE Less than, Not greater than or equal to"},
    {"NL", "GE Not less than, Greater than or equal to"},
    {"LE", "NG Less than or equal to, Not greater than"},
    {"NLE", "G Not less than or equal to, Greater than"}
};