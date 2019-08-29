#include <stdio.h>
#include <stdlib.h>

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
int main(){
    int arr[6] = {0,11,22,33,44,55};
    printf("TODO TODO TODO\n");
    printf("It is not working as intended\n");
    printf("I want to add i to the every element, but it is not doing that\n");
    printf("To learn asm code inside a loop\n");
    printf("Change the first element of the array, add 1 to 5\n");
    
    printf("Previous array\n");
    for (int i = 0; i < 6;i++)
    {
        printf("arr[%d]:%d\n", i, arr[i]);
    }

    int *ptr = &arr[0];

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
    printf("After array\n");
    for (int i = 0; i < 6;i++)
    {
        printf("arr[%d]:%d\n", i, arr[i]);
    }
    
    return 0;
}
