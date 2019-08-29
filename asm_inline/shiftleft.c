#include <malloc.h>
#include <stdlib.h>
#include <stdio.h>

void print_usage_message(){
    printf("Usage:\t ./shl NUMBER\n");
    printf("Number should be between 0-255\n");
    
}

/**
 * Try to read a value from so called kernel
 * Shift the value by 2, i.e. multiply by 4
 * 
*/ 
int main(int argc, char *argv[]){

    if(argc != 2){
        print_usage_message();
        return 1;
    }

    int testelement = atoi(argv[1]);
    if(testelement > 255 || testelement < 0){
        print_usage_message();
    }   

    printf("Shiftleft test\n");
    
    printf("We are shifting the value by 2 bit,i.e. multiplying with 4\n");


    printf("kernel[i]:2*i\n");
    printf("Entered val:%d\n", testelement);
    // imagine this is a kernel location
    int *kernel = (int* ) malloc(sizeof(int) * 256);
    for (int i = 0; i < 256; i++)
    {
        kernel[i] = i + i;
    }
    // assume that kernel_pointer is the kernel address
    // *kernel_pointer is the value

    // Assume that the entered value is 20
    // the 20th value of the kernel should be 40
    int *kernel_pointer = &kernel[testelement];

    int out;
    asm volatile(
        "mov (%1), %%edx\n"
        "shl $2, %%edx"
        :"=d"(out)
        :"b"(kernel_pointer)
    );
    // it should print 160, since we are shifting by 4
    // the pointed location. 40*4 = 160
    printf("kernel[%d * 4]:%d\n",testelement, out);

    free(kernel_pointer);
    
    return 0;
}