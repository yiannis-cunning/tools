#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <stdbool.h>
#include <stddef.h>

#include "/tools/compiler/database/dtb.h"
#include "/tools/compiler/include/cx86.h"

static void test(bool cond, char *name){
    printf("Testing %s: \n", name);
    if(cond){
        printf("\tPASSED\n");
    }
    else{
        printf("\tFAILED\n");
    }
}


#ifdef TEST
int main(int argc, char **argv){

    // 1) batoi
    char *str = "10001100011000110100000000000000111"; // 8c 63 40 00 = 1000 1100 0110 0011 0100 0000 0000 0000
    uint64_t ans = 0x40638c;
    uint64_t out = batoi(str, 32);
    printf("%lx\n", out);
    test(ans == out, "batoi() - testcase 1");

    str = "1001110001000001010";        // 9c 41 = 1001 1100 0100 0001
    ans = 0x419c;
    out = batoi(str, 16);
    printf("%lx\n", out);
    test(ans == out, "batoi() - testcase 2");

    str = "1011011111";        // 10 1101 = 32 + 13 = 45
    ans = 45;
    out = batoi(str, 6);
    printf("%lx\n", out);
    test(ans == out, "batoi() - testcase 3");

    // 2) 
    symbol_t *symb;
    str = "0101reg";
    symb = match_field(str);
    test(symb == &symbols[0], "match_field() on 0/1");
    test(match_field(str+4) == &symbols[6], "match_field() on reg");



    return 0;
}
#endif