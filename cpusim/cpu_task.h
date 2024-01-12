#pragma once
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>



typedef struct {
    // General purpose registers
	uint64_t gp_rax;    // Acumalator for operands and results
    uint64_t gp_rbx;    // Pointer to data - in DS segment
    uint64_t gp_rcx;    // Counter for string and loop opperations
    uint64_t gp_rdx;    // I/O pointer
    uint64_t gp_rsi;    // Pointer to data - in DS segment
    uint64_t gp_rdi;    // Pointer to data - in ES segment
    uint64_t gp_rsp;    // Stack pointer - in SS segment
    uint64_t gp_rbp;    // Pointer to data on the stack - in SS segment
    uint64_t r8, r9, r10, r11, r12, r13, r14, r15;

    uint64_t *rax, rbx, rcx, rdx, rsi, rdi, rsp, rbp;
    uint32_t *eax, ebx, ecx, edx, esi, edi, esp, ebp;
    uint16_t *ax, bx, cx, dx, si, di, sp, bp;
    uint8_t *al, bl, cl, dl, ah, bh, ch, dh;
    
    // Segment registers - (also a hidden cache register)
    uint16_t cs, ds, ss, es, fs, gs;

    // table registers ( +hidden cache registers)
    uint64_t gdtr_base,     idtr_base,  ldtr_base,                  tr_base;
    uint16_t gdtr_limit,    idtr_limit, ldtr_segment, ldtr_limit,   tr_segment;

    // EFLAGS registers
    uint64_t eflags;

    // Control registers
    uint64_t cr0, cr1, cr2, cr3, cr4;

    // Istruciton pointer register
    uint32_t EIP;
    
} cpu_reg_t;


uint64_t mask_getbits(uint64_t t, int lb, int rb);

void print_segment(uint16_t *seg);

void print_eflags(uint64_t eflags);

cpu_reg_t *init_cpu_regs();


