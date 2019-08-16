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


/**
 * Test3
 * 
 * Try to add 2 elements together
 * and store it in result
 * 
 * 
*/
void asm_test3(){

    int first = 24;
    int second = 10;
    int result = 0;

    printf("Test3, to learn add instruction in asm\n");

    printf("Previous values:\t first:%d second:%d result:%d \n", first, second, result);

    asm volatile(
        "movl %[first], %[result]\n"
        "addl %[second], %[result]"
        :[result] "=r"(result) // output
        :[first] "r"(first), [second] "r"(second) // input
        );

    printf("Changed values: \t first:%d second:%d result:%d \n", first, second, result);
    
    print_empty_lines();
}


/**
 * 
 * Helper function for test4 
 * 
*/
void asm_for_loop_test4(int i, int* arr){
    asm volatile(
        "addl %[i], %[arr]"
        // output
        :[arr] "=m"(*arr) 
        // input
        :"m"(*arr), [i]"r"(i)
    );
}

/**
 * Test4
 * 
 * Try to add elements in the array using loop
 * Asm statement is inside the loop
 * 
 * Important point:
 * If the access element of the array is type of 'r', meaning register
 * it will not be affected correctly.
 * 
 * To fix that, we must access to the memory location of the array
 * using :[arr] "=m"(*arr) 
 * 
*/
void asm_test4(){

    int arr[6] = {0,11,22,33,44,55};
    printf("Test4, to learn asm code inside a loop\n");
    printf("Change the first element of the array, add 1 to 5\n");
    
    printf("Previous array\n");
    for (int i = 0; i < 6;i++)
    {
        printf("arr[%d]:%d\n", i, arr[i]);
    }

    for (int i = 0; i < 6;i++)
    {
        asm_for_loop_test4(i, &arr[0]);

        /**
         * NOT WORKING
            asm volatile(
                "addl %[arr], %[result]"
                
                // output
                :[result] "=r"(result)
                // input
                :[arr]"r"(*(arr +1))
            ); 

            printf("result:%d arr[0]:%d, arr[1]:%d\n", result, arr[0], arr[1]);
        */

        /**
         * NOT WORKING
            asm volatile(
                // asm statements
                //"addl %[arr], %[result]"
                "addl $1, %[i_res]\n"
                "movl %[arr[i_res]], %[result]"
                //:[result] "=r"(result) // output
                :[i_res] "=r"(i), [result] "=r"(result)
                :"r"(i),[arr]"r"(*arr)
                //:[arr] "r"(*(ptr + 4)) // input
            );

            printf("i:%d j:%d\n", i , j);
            //printf("i:%d result:%d\n",i, result);
            //printf("*(arr + i)%d\n", *(arr + i));
        */
    
    }
    
    printf("After adding 1 to 6 to the first element\n");
    for (int i = 0; i < 6;i++)
    {
        printf("arr[%d]:%d\n", i, arr[i]);
    }

    print_empty_lines();
}


/**
 * 
 * Test5 
 * 
 * Try to sum elements of the array to the register result
 * With the help of pointer
 * 
*/
void asm_test5(){

    int arr[8] = {2,8,20,40,60,70, 80, 120};
    //printf("Test5, try to sum of elements of an array using asm inside a loop\n");
    //printf("With the help of pointer\n");

    int result = 0;
    int *ptr = arr;

    for (int i = 0; i < 8;i++)
    {
        // if the result is set to the 'r'
        // the asm compiler wont assign a register to the result
        // we must assing a register to the 'result' manually

        asm volatile(
            "add %[ptr], %[result]"
            // output
            : [result]"=b"(result)
            // input
            : [ptr]"rm"(*ptr)
        );

        /**
         * a means eax, b means ebx...
         *  NOT WORKING
            asm volatile(
                "add %[ptr], %[result]"
                // output
                : [result]"=a"(result)
                // input
                : [ptr]"b"(*ptr)
            );
        */
        //printf("arr[%d]:%d \t result:%d \t *arr:%d \t *ptr:%d \n",i, arr[i], result, *arr, *ptr);

        ptr++;
    }

    printf("result:%d\n", result);
    print_empty_lines();
}

int main(){
    printf("Asm test is starting...\n");

    asm_test1();

    asm_test2();

    asm_test3();

    asm_test4();

    asm_test5();

    return 0;
}
