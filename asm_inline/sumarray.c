#include <stdio.h>
#include <stdlib.h>
/**
 * 
 * Try to sum elements of the array to the register result
 * With the help of pointer
 * 
*/
int main(){
    printf("Try to sum elements of the array using pointer\n");
    int arr[8] = {1,3,5,7,9,5,30,90};
    
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
        ptr++;
    }

    printf("result:%d\n", result);
    return 0;
}