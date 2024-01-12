#include "cpu_task.h"

// EFLAGS
static uint32_t eflags = 0x2; // (inits to 2)

// Global table registers
static uint8_t gdtr[6];
static uint8_t ldtr[10];

// Segment registers
static uint16_t seg_cs = 0x0;
static uint16_t seg_ds = 0x0;
static uint16_t seg_ss = 0x0;
static uint16_t seg_es = 0x0;
static uint16_t seg_fs = 0x0;
static uint16_t seg_gs = 0x0;

// General Purpose registers
static uint64_t gp_rax = 0x0;
static uint64_t gp_rbx = 0x0;
static uint64_t gp_rcx = 0x0;
static uint64_t gp_rdx = 0x0;
static uint64_t gp_rsi = 0x0;
static uint64_t gp_rdi = 0x0;
static uint64_t gp_rsp = 0x0;
static uint64_t gp_rbp = 0x0;
static uint64_t gp_r8[8] = {0};

// first four addressable to byte level
static uint64_t *rax = &gp_rax;
static uint32_t *eax = (uint32_t *)rax + 1;
static uint16_t *ax = (uint16_t *)eax + 1;
static uint8_t *al = (uint8_t *)ax + 1;
static uint8_t *ah = (uint8_t *)ax;

static uint64_t *rbx = &gp_rbx;
static uint32_t *ebx = (uint32_t *)rbx + 1;
static uint16_t *bx = (uint16_t *)ebx + 1;
static uint8_t *bl = (uint8_t *)bx + 1;
static uint8_t *bh = (uint8_t *)bx;

static uint64_t *rcx = &gp_rcx;
static uint32_t *ecx = (uint32_t *)rcx + 1;
static uint16_t *cx = (uint16_t *)ecx + 1;
static uint8_t *cl = (uint8_t *)cx + 1;
static uint8_t *ch = (uint8_t *)cx;

static uint64_t *rdx = &gp_rdx;
static uint32_t *edx = (uint32_t *)rdx + 1;
static uint16_t *dx = (uint16_t *)edx + 1;
static uint8_t *dl = (uint8_t *)dx + 1;
static uint8_t *dh = (uint8_t *)dx;

    // second four addressable to word level
static uint64_t *rsi = &gp_rsi;
static uint32_t *esi = (uint32_t *)rsi + 1;
static uint16_t *si = (uint16_t *)esi + 1;

static uint64_t *rsi = &gp_rdi;
static uint32_t *edi = (uint32_t *)rdi + 1;
static uint16_t *di = (uint16_t *)edi + 1;

static uint64_t *rsp = &gp_rsp;
static uint32_t *esp = (uint32_t *)rsp + 1;
static uint16_t *sp = (uint16_t *)esp + 1;

static uint64_t *rbp = &gp_rbp;
static uint32_t *ebp = (uint32_t *)rbp + 1;
static uint16_t *bp = (uint16_t *)ebp + 1;

    // Last 8 are just quadruple word addresable
static uint64_t *r8 = gp_r8;
static uint64_t *r9 = gp_r8 + 1;
static uint64_t *r10 = gp_r8 + 2;
static uint64_t *r11 = gp_r8 + 3;
static uint64_t *r12 = gp_r8 + 4;
static uint64_t *r13 = gp_r8 + 5;
static uint64_t *r14 = gp_r8 + 6;
static uint64_t *r15 = gp_r8 + 7;


typedef struct {
    // General purpose registers
	uint64_t gp_rax, gp_rbx, gp_rcx, gp_rdx, gp_rsi, gp_rdi, gp_rsp, gp_rbp;
    uint64_t r8, r9, r10, r11, r12, r13, r14, r15;

    uint64_t *rax, rbx, rcx, rdx, rsi, rdi, rsp, rbp;
    uint32_t *eax, ebx, ecx, edx, esi, edi, esp, ebp;
    uint16_t *ax, bx, cx, dx, si, di, sp, bp;
    uint8_t *al, bl, cl, dl, ah, bh, ch, dh;
    
    // Segment registers
    uint16_t cs, ds, ss, es, fs, gs;

    // GDT registers
    uint8_t gdtr[10];
    uint8_t ldtr[10];

    // EFLAGS registers
    uint64_t eflags;
    
} cpu_reg_t;



cpu_reg_t *init_reg(){
    cpu_reg_t *ans = (cpu_reg_t *)calloc(sizeof(cpu_reg_t), 1);
        


}