#include <stdlib.h>
#include <malloc.h>

/**
 * Simple pointer test
 * Try to reach a location pointed by pointer
 * Increment the value at that memory location 
*/ 

int main(){
    printf("Pointer test\n");

    int *ptr = (int* ) malloc(sizeof(int) * 2);
    ptr[0] = 10;
    ptr[1] = 20;

    printf("Initial values:\n");
    printf("\tptr0 :%d  \n", ptr[0]);
    printf("\t&ptr0:%p\n",&ptr[0]);
    printf("\tptr1 :%d  \n", ptr[1]);
    printf("\t&ptr1:%p\n", &ptr[1]);
    
    int *ptr2 = ptr;
    printf("\tptr2 :%p\n",  ptr2);
    printf("\t*ptr2:%d\n", *ptr2);
    
    asm volatile(
        "add $4, %0"
        :"=b"(ptr2)
        :"b"(ptr2)
    );
    printf("After shift\n");
    printf("\tptr2 :%p\n", ptr2);
    printf("\t*ptr2:%d\n", *ptr2);
    return 0;
}
