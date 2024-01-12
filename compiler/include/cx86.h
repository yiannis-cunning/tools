#pragma once


void passert(bool tf, char *pnt);

// Given a string pointer to 0/1 bit stream, return int value for n_bits of that stream 
uint64_t batoi(char *str, int n_bits);

// Ensure first n bits of a and b are equal
int strcmpn(char *a, char *b, int n);

// return what symbol is being used in the instuc_dtb
symbol_t *match_symbol(char *buff_inst_code);

// Set all fields to -1, or 0 for unsigned
field_t *mkparms();

field_t *extract_from_inst(char *buff_in, instruction_entry_t *inst_code);

void print_inst(instruction_entry_t *inst_code, field_t *parms);

int print_instruction(char *buffer);

bool all_zeros(char *buffer, int n);



