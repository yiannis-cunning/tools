#include "cpu_task.h"

cpu_reg_t *init_cpu_regs(){
    cpu_reg_t *ans = (cpu_reg_t *)calloc(sizeof(cpu_reg_t), 1);
    if(ans == NULL){return ans;}

    ans->eflags = 0x2;
    for(int j = 0; j < 8; j ++){
        *(&ans->rax + j) = &(ans->gp_rax) + j;
        *(&ans->eax + j) = (uint32_t *)(&(ans->gp_rax) + j) + 1;
        *(&ans->ax + j) = (uint16_t *)(&(ans->gp_rax) + j) + 3;
        *(&ans->al + j) = (uint8_t *)(&(ans->gp_rax) + j) + (6 + (j < 4)*1);
    }
    ans->gdtr_limit = 0xFFFF;
    ans->ldtr_limit = 0xFFFF;
    ans->idtr_limit = 0xFFFF;
    ans->tr_limit = 0xFFFF;

    return ans;
}



uint64_t mask_getbits(uint64_t t, int lb, int rb){
    if(lb < rb || lb >= 64 || lb < 0 || rb >= 64 || rb < 0){printf("bad bit translation");exit(1);}
    uint64_t l_mask, r_mask;

    l_mask = ~(-1 << lb + 1);
    r_mask = -1 << rb;

    l_mask = l_mask & r_mask;
    return (t & l_mask) >> rb;
}



void print_segment(uint16_t *seg){
    uint64_t seg_ext = (uint64_t) *seg;
    // bit 0,1 are priviledge level
    printf("Requested privlidege level(lower is higher priviledge): %ld\n", mask_getbits(seg_ext, 1, 0));

    // bit 2 is GDT vs LDT
    if(mask_getbits(seg_ext, 2, 2) == 0 ){printf("GDT\t");}
    else{printf("LDT\t");}

    // bits 15 - 3 are offset into table
    printf("index into table: %ld\n", mask_getbits(seg_ext, 15, 3));
}


void print_eflags(uint64_t eflags){
    printf("Section 3.4.3 for more info...\n");
    printf("\nSYSTEM flags - set after an operation\n");
    printf("Virtual interrupt pending: %ld\n", mask_getbits(eflags, 20, 20));
    printf("Virtual 8086 mode: %ld\n", mask_getbits(eflags, 17, 17));
            printf("\t Set this to enable 8086 mode\n");
    printf("IO priviledge level: %ld\n", mask_getbits(eflags, 13, 12));
        printf("\t If current privilege level <= IO-PL to acess I/O address space\n");
        printf("\t Modify this with POPF, IRET when operating at CPL of 0\n");
    printf("Interupt enable flag: %ld\n", mask_getbits(eflags, 9, 9));
    printf("Trap flag: %ld\n", mask_getbits(eflags, 8, 8));

    printf("\nControl flags - set after an operation\n");
    printf("Direction flag: %ld\n", mask_getbits(eflags, 10, 10));
    printf("\tused for string instructions - set with STD, CLD\n");
    printf("\nSTATUS flags - set after an operation\n");
    printf("Overflow flag: %ld\n", mask_getbits(eflags, 11, 11));
    printf("Sign flag: %ld\n", mask_getbits(eflags, 7, 7));
    printf("Zero flag: %ld\n", mask_getbits(eflags, 6, 6));
    printf("Carry flag: %ld\n", mask_getbits(eflags, 6, 6));
}



void print_IP(uint64_t IP){
    // - Offset into current code segment of NEXT instruction
    // - Every instruction will increment this by some value -> by time inst is done IP is set to next
    // - Change this with JMP, Jcc, CALL, RET, IRET

}


void print_segment_descriptor(uint64_t entry){
    // Stores the info about known segments - section 3.4.5 (book 3a)
    // A segment pointed to by SS is automatically a 'stack' segment (type of data segment)
    //  - ss HAS to be a read/write segment or exception occurs when loaded - #GP
    int expand_down_segment = 0;

    // 1) What priviledge does this segment have - 0 = most privileged
    printf("Privilege level (lower is more priv.): %ld\n", mask_getbits(entry, 32 + 14, 32 + 13));
    int is_present  = mask_getbits(entry, 32 + 15, 32 + 15);
    int type        = mask_getbits(entry, 32 + 11, 32 + 8);
    int s_bit       = mask_getbits(entry, 32 + 12, 32 + 12);

    // 2) What type is this segment? - s_bit determins how to interpret the type feild
    if(s_bit == 0){
        printf("This is a \'System\' segment\n");
        switch(type){
            case(2):
                printf("This is a LDT segment -> data segment + grows up + RW access?\n");
                break;
        }
    } else {
        if(type >= 8){
            printf("This is a \'Code\' segment\n");
            printf("\t - Excute permission = 1\n");
            printf("\t - Read Access = %ld (writable depends on mode)\n", mask_getbits(entry, 32 + 9, 32 + 9));
            printf("\t - Conforming section = %ld\n", mask_getbits(entry, 32 + 10, 32 + 10));
            printf("\t\t (Can this segment be called from lower privilege areas)");
        } else{   
            printf("This is a \'Data\' segment\n");
            printf("\t - Read Access = 1\n");
            printf("\t - Write Access = %ld\n", mask_getbits(entry, 32 + 9, 32 + 9));
            printf("\t - Expand down segment = %ld\n", mask_getbits(entry, 32 + 10, 32 + 10));
            printf("\t - Conforming = 1 (You can access code sections with lower priority but not higher)\n");
            expand_down_segment = mask_getbits(entry, 32 + 10, 32 + 10);
        }
        printf("\t - Accessed section = %ld\n", mask_getbits(entry, 32 + 8, 32 + 8));
        printf("\t\t (Set every time segment is loaded - context switch?)");
    }

    if(is_present == 0){
        printf("This segment is not loaded.\n");
        return;
    }

    // 3) What is the base + range of the segment?
    printf("Segment base: %lx\n", mask_getbits(entry, 31, 16) + mask_getbits(entry, 7 + 32, 32) << 16 + mask_getbits(entry, 32 + 31, 32 + 24) << 24);
    if(mask_getbits(entry, 32 + 23, 32 + 23) == 0){ // Granularity bit
        printf("Size of segment: %ld B\n", mask_getbits(entry, 19 + 32, 16 + 32) << 16 + mask_getbits(entry, 0, 15) );
    }else{
        printf("Size of segment: %ld KB\n", (mask_getbits(entry, 19 + 32, 16 + 32) << 16 + mask_getbits(entry, 0, 15))*4 );
    }
    
    int b_bit = mask_getbits(entry, 22 + 32, 22 + 32);
    if(!expand_down_segment){
        printf("Effective address = base + offset. Ensure base + offset < base + limit\n");
    } else{
        printf("Effective address = base - (UPPERBOUND - offset). Ensure UPPERBOUND > offset > limit + 1\n");
        if(b_bit){printf("Upper bound of segment - 32xFFFFFFFF");}
        else{printf("Upper bound of segment - 16xFFFF");}
    }

    // 4) What is defualt operation size/operation mode/address size - For code sections only
    int l_bit = mask_getbits(entry, 21 + 32, 21 + 32);
    if(type >= 8 && s_bit == 1){
        if(l_bit == 0){
            printf("Default register/operand size: %ld-bit\n", mask_getbits(entry, 22 + 32, 22 + 32)*16 + 16);
        } else {    // if l_bit is set it is assumed b_bit is not set
            printf("Operating in 64-bit mode\n");
        }
    }

}



void print_table_registers(cpu_reg_t *cpu){
    // (this is 10 bytes long for x64)
    // - LDT belongs inside a pre-defined LDT-type segment (segment defined in GDT)

    printf("Global Descriptor Table Register - GDTR\n");
    printf("GDT located at %lx\n", cpu->gdtr_base);
    printf("GDT size %d Bytes\n", cpu->gdtr_limit);
    // Empty on start up -> must be initalized before use of protected mode

    printf("Local Descriptor Table Register - LDTR\n");
    printf("LDT segment #%ld in GDT\n", cpu->gdtr_base);
    printf("LDT located at %x\n", cpu->gdtr_limit);
    // can be loaded from segment descritor with LLDT + SLDT


    printf("Interrupt Descriptor Table Register - IDTR\n");
    printf("IDT located at %lx\n", cpu->idtr_base);
    printf("IDT size %d Bytes\n", cpu->idtr_limit);
}


void print_control_registers(cpu_reg_t *cpu){
    // see vol 3A 2-15
    // - changed with MOV CRn commands

    // Control register 0
    uint64_t ccr = cpu->cr0;
    printf("PG, Paging enabled: %ld\n", mask_getbits(ccr, 31, 31));
    printf("CD, Disable all caching: %ld\n", mask_getbits(ccr, 30, 30));
    printf("NW, Disable write back caching: %ld\n", mask_getbits(ccr, 29, 29));
    printf("Enable alignment checks: %ld\n", mask_getbits(ccr, 18, 18));
    printf("WP, catch writes to read-only pages in supervisor mode: %ld\n", mask_getbits(ccr, 16, 16));
    printf("NE: Enable FPU errors: %ld\n", mask_getbits(ccr, 5, 5));
    printf("TS: delay saving FPU and other specialist registers on context switches: %ld\n", mask_getbits(ccr, 3, 3));
    printf("EM: Dont have access to FPU unit: %ld\n", mask_getbits(ccr, 2, 2));
    // These will determine behavior when FPU inst are used -> might work, might generate exception
    printf("PE: Protected mode enable/(disbale real-address mode): %ld\n", mask_getbits(ccr, 0, 0));
    printf("\t\t - in real-address mode segments are disabled\n");

    // Control register 2
    ccr = cpu->cr2;
    printf("Address that casued the page fault: %ld", ccr);


    // Control register 3
    ccr = cpu->cr3;
    printf("\n");
    printf("Root page table address: %lx", mask_getbits(ccr, 63, 12) << 12)
    printf("PCD: memory type to access root page table: %ld\n", mask_getbits(ccr, 4, 4));
    printf("PWT: Write through: %ld\n", mask_getbits(ccr, 3, 3));

    // Control register 4
    ccr = cpu->cr4;
    printf("\n");
    printf("VME: Enable virtual interupts in protected mode: %ld\n", mask_getbits(ccr, 0, 0));
    printf("PSE: Enable larger page sizes of 1MB: %ld\n", mask_getbits(ccr, 4, 4));
    printf("PAE: Enable physical addresses larger than 32 bits (after paging transaltions): %ld\n", mask_getbits(ccr, 5, 5));
    printf("PGE: Enable use of pages to be globally accessable: %ld\n", mask_getbits(ccr, 7, 7));
    printf("LA57: use 57 bit linear addresses (else 48 bits are used in IA32 mode): %ld\n", mask_getbits(ccr, 12, 12));

}








