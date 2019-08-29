#include <stdio.h>
#include <stdlib.h>
/**
 * Test2
 * To learn the similarity between renaming and % operand in asm
 * 
 * 
*/
int main(){

    int c = 1;
    int d1; // not initialized
    int *e = &c;
    printf("To learn similarity between renaming and %% operands in asm\n\n");

    printf("Starting without renaming\n");
    printf("Prev. values:\n\t c:%d e:%d d1: is not initialized\n", c, *e);

    // rm means either register or memory, whichever is faster
    // decision is up to compiler
    asm volatile(
        "movl %1, %0"
        :"=rm"(d1)
        :"r"(*e)
        :
    );

    printf("Chan. values:\n\t c:%d e:%d d1:%d\n\n", c, *e, d1);

    printf("Starting renaming\n");
    printf("Prev. values:\n\tc:%d e:%d d2: is not initialized\n", c, *e);

    int d2; // not initialized

    // instead of keep tracking of the order of the operands
    // we can rename them
    // renamed *e to [e]
    asm volatile(
        "movl %[e], %[d2]"
        :[d2]"=rm"(d2)
        :[e]"rm"(*e));
    
    printf("Chan. values:\n\tc:%d e:%d d2:%d\n", c, *e, d2);
    return 0;
}
