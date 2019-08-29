#include <stdlib.h>
#include <stdio.h>

/**
 * To learn the meaning of %0 and %1 in the asm language 
 *  
 * Inline asm code structure
 * 
 * asm volatile( template aka the code
 *      : Output
 *      : Input
 *      : Clobber
 * )
*/
int main(){
    
    printf("To learn %% sign in asm\n");

    int i1 = 25;
    int i2 = 35;
    printf("Prev. values:\n\t i1:%d i2:%d\n", i1, i2);

    // %0 is first variable passed to the operand
    // %1 is the second variable passed to the operand
    // i2 is %0, it passed first
    // i1 is %1, it passed second
    // mov %1, %0 = mov i1, i2 =   i2=i1, i2 will be 25

    // output will be i2
    // input is i1
    // clobber is empty
    // =r means store it in the register, then to i2
    asm volatile("movl %1, %0" : "=r"(i2) : "r"(i1) :);


    printf("Chan. values:\n\t i1:%d i2:%d\n", i1,i2);

    return 0;
}