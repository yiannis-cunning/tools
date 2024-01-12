#include "cpu_task.h"

static int test_num = 0;

void test(bool tf){
    if(tf == false){
        printf("Test %d failed\n", test_num);
    }
    printf("Test %d passed\n", test_num);
    test_num += 1;
}


int main(int argc, char *argv[]){

    uint64_t n, ans;

    n = 0x03030030;
    ans = mask_getbits(n, 19, 16);
    test(ans == 3);
    //printf("%x\n", ans);

    n = 0xF3030030;
    ans = mask_getbits(n, 19,16);
    test(ans == 3);
    //printf("%x\n", ans);

    n = 0xF0030030;
    ans = mask_getbits(n, 28,16);
    test(ans == 0x1003);
    //printf("%x\n", ans);

    uint64_t eflags = 0x2;
    print_eflags(eflags);
}