#include <stdio.h>


/**
 * prints 2 empty lines 
*/
void print_empty_lines(){
    printf("\n\n");
}

/**
 * Test1
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
void asm_test1(){
    
    printf("Test1, to learn %% sign in asm\n");

    int i1 = 25;
    int i2 = 35;
    printf("Previous values:\t i1:%d i2:%d\n", i1, i2);

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


    printf("Changed values:\t\t i1:%d i2:%d\n", i1,i2);

    print_empty_lines();
}


/**
 * Test2
 * To learn the similarity between renaming and % operand in asm
 * 
 * 
*/
void asm_test2(){

    int c = 1;
    int d1; // not initialized
    int *e = &c;
    printf("Test2, to learn similarity between renaming and %% operands in asm\n");

    printf("Starting without renaming\n");
    printf("Previous values:\t c:%d e:%d d1: is not initialized\n", c, *e);

    // rm means either register or memory, whichever is faster
    // decision is up to compiler
    asm volatile(
        "movl %1, %0"
        :"=rm"(d1)
        :"r"(*e)
        :
    );

    printf("Changed values:\t\t c:%d e:%d d1:%d\n", c, *e, d1);

    printf("Starting renaming\n");
    printf("Previous values:\t c:%d e:%d d2: is not initialized\n", c, *e);

    int d2; // not initialized

    // instead of keep tracking of the order of the operands
    // we can rename them
    // renamed *e to [e]
    asm volatile(
        "movl %[e], %[d2]"
        :[d2]"=rm"(d2)
        :[e]"rm"(*e));
    
    printf("Changed values:\t\t c:%d e:%d d2:%d\n", c, *e, d2);

    print_empty_lines();
}

int main(){
    printf("Asm test is starting...\n");


    asm_test1();

    asm_test2();

    return 0;
}
