#include <stdio.h>
#include <stdlib.h>
/**
 * Try to add a register content to another register content 
 * 
*/ 
int main(){
    printf("Add a register content to another register\n");
    int first = 20;
    int second = 40;

    asm volatile(
        "add %1, %0"
        // out
        // it should be + instead of =
        // because, we are reading the register, then updating it
        : "+c"(second)
        // in
        : "b"(first)
    );

    printf("first:%d, second:%d\n", first, second);
    return 0;
}
