

// Assuming
// - SS points to a valid stack segment (expand-down, read-write, ... dont know size tho)
// - CS is the current code segment we are in (expand up, execute perm, size given by data segments?) 
// - DS exists as a segment and contains the data sections (expand up, read-write -> size given by data segments?)
// Assuming read-addressing modes.. you will use a offset in to data segment to refer to that 
// ES contains a valid segment and rdi will point to data segment inside that 

/*
  - registers ax, cx, dx are usable -> all other GP registers have to be saved before use

  - Running in protected mode
  - Memmory maping may be on
    - if so global UEFI tables are identiy mapped
  - selectors are flat? - (segments not used?)

  - guaranteed to be 128KB+ of stack space...




Normal windows code...
  - Specify stack + heap allocation + limits in PE file
    - stack given in SP, heap given somwhere else?
  
  - dll will be imported based off of the import table
    - will get the address of what to call?
    - then just follow x86/x64 calling conventions?


_cprintf
*/


// simple loop
3000 <.text>:
 
sub     $0x8,%rsp
<.loop>
xor     %rax,%rax
add     $0x8,%rax
sub     $0x8,%rax
cmp     %rax,%rax
je     <.loop>

add     $0x8,%rsp
ret



3000:     48 83 ec 08
3004:     48 31 c0                  // 0100 1R0B : 0011 0001 : 11 qwordreg1 qwordreg2
3007:     48 81 d0 08 00 00 00      // 0100 100B : 1000 0001 : 11 010 qwordreg : imm 
//                                  // 0100 100B : 1000 0001 : 11 010 qwordreg : imm
300e:     48 81 e8 08 00 00 00      // 0100 100B : 1000 0001 : 11 101 qwordreg : imm32
3015:     48 39 c0                  // 0100 1R0B : 0011 1001 : 11 qwordreg1 qwordreg2
3018:     74 xx (ea)                     // 0111 tttn : 8-bit displacement tttn = 0100 for eq 
301a:     48 83 c4 08          	              //add    $0x8,%rsp
301e:     c3





// print a value
1000 <.text>:
 
sub    $0x8,%rsp
lea    0x1ff5(%rip),%rax        # 140004000 <.rdata>  // put hello world in ac
mov    %rax,%rcx
mov    0xeb(%rip),%rax        # 140008204 <func>
call   *%rax

<.loop>
xor     %rax,%rax
add     $0x8,%rax
sub     $0x8,%rax
cmp     %rax,%rax
je     <.loop>

add     $0x8,%rsp
ret

0x1300<.func>
jmp    *0x5c46(%rip)        # 140008204 <__imp__cprintf>



2000 <.idata>
2038 <_cprintf addr>

3000 <.rdata>
"output"
0x140008204
0x1ff5 -> offset of rdata





1000:     48 83 ec 08               // sub    $0x8,%rsp
1004:     48 8d 05 f5 1f 00 00      // lea    0x1ff5(%rip),%rax
100b:     48 89 c1                 	// mov    %rax,%rcx
100e:     48 8b 05 eb 00 00 00 	    // mov    0xeb(%rip),%rax 0xeb
1015:     ff d0                	    // call   *%rax

1017:     48 31 c0                  // 0100 1R0B : 0011 0001 : 11 qwordreg1 qwordreg2
101a:     48 81 d0 08 00 00 00      // 0100 100B : 1000 0001 : 11 010 qwordreg : imm 
//                                  // 0100 100B : 1000 0001 : 11 010 qwordreg : imm
1021:     48 81 e8 08 00 00 00      // 0100 100B : 1000 0001 : 11 101 qwordreg : imm32
1028:     48 39 c0                  // 0100 1R0B : 0011 1001 : 11 qwordreg1 qwordreg2
102b:     74 xx (ea)                     // 0111 tttn : 8-bit displacement tttn = 0100 for eq 
102d:     48 83 c4 08          	              //add    $0x8,%rsp
1031:     c3

...

1100:     ff 25 32 0f 00 00    	    // jmp    *0x5c46(%rip) 0xf32
1106:     


they jumped 0x5c46 + 1400025b6

0x1400081fc + 10*8 + 9*8


0x140008164
 -> 152 bytes into the table
 -> 19th entry






































// print a value
1000 <.text>:
 
sub    $0x8,%rsp
lea    0x1ff5(%rip),%rax        # 140004000 <.rdata>  // put hello world in ac
mov    %rax,%rcx
mov    0xeb(%rip),%rax        # 140008204 <func>

cmp   rax, value
je <afterloop>

<.loop>
xor     %rax,%rax
add     $0x8,%rax
sub     $0x8,%rax
cmp     %rax,%rax
je     <.loop>

add     $0x8,%rsp
ret

0x1300<.func>
jmp    *0x5c46(%rip)        # 140008204 <__imp__cprintf>



2000 <.idata>
2038 <_cprintf addr>

3000 <.rdata>




1000:     48 83 ec 08               // sub    $0x8,%rsp
1004:     48 8d 05 f5 1f 00 00      // lea    0x1ff5(%rip),%rax
100b:     48 89 c1                 	// mov    %rax,%rcx
100e:     48 8b 05 23 10 00 00 	    // mov    0xeb(%rip),%rax 0xeb
1015:     48 81 f8 00 00 00 00 00 00 00 00 // cmp 0100 100B 1000 0001 : 11 111 qwordreg : imm64
1020:     74 xx (16)                  // 0111 tttn : 8-bit displacement tttn = 0100 for eq 

1022:     48 31 c0                  // 0100 1R0B : 0011 0001 : 11 qwordreg1 qwordreg2
1025:     48 81 d0 08 00 00 00      // 0100 100B : 1000 0001 : 11 010 qwordreg : imm 
//                                  // 0100 100B : 1000 0001 : 11 010 qwordreg : imm
102c:     48 81 e8 08 00 00 00      // 0100 100B : 1000 0001 : 11 101 qwordreg : imm32
1033:     48 39 c0                  // 0100 1R0B : 0011 1001 : 11 qwordreg1 qwordreg2
1036:     74 xx (ea)                     // 0111 tttn : 8-bit displacement tttn = 0100 for eq 
1038:     48 83 c4 08          	              //add    $0x8,%rsp
103c:     c3                        // ret

...

1100:     ff 25 32 0f 00 00    	    // jmp    *0x5c46(%rip) 0xf32
1106:     