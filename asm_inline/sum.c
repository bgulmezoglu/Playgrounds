#include <stdio.h>
#include <stdlib.h>
/**
 * Try to add 2 elements together
 * and store it in result
*/
int main(){
    int first = 24;
    int second = 10;
    int result = 0;

    printf("To learn add instruction in asm\n");

    printf("Previous values:\n\tfirst:%d second:%d result:%d \n", first, second, result);

    asm volatile(
        "movl %[first], %[result]\n"
        "addl %[second], %[result]"
        :[result] "=r"(result) // output
        :[first] "r"(first), [second] "r"(second) // input
        );

    printf("Changed values:\n\tfirst:%d second:%d result:%d \n", first, second, result);


    return 0;
}