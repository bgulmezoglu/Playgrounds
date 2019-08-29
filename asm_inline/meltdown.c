#include <stdio.h>
#include <stdint.h>
#include <malloc.h>
#include <stdlib.h>
uint64_t* preparekernel(){
    // imagine this is a kernel location
    uint64_t *kernel = (uint64_t* ) malloc(sizeof(uint64_t) * 256);
    for (int i = 0; i < 256; i++)
    {
        kernel[i] = i;
    }
    return kernel;
}

uint64_t* prepare_probe(){
    uint64_t * probe_array = (uint64_t *) memalign(4096, 256 * 512 * sizeof(uint64_t));

    // set memory
    for(int i = 0; i < 256 * 512; i++){
        probe_array[i] = i + i;
    }
    return probe_array;
}

void print_usage_message(){
    printf("Usage:\t ./md NUMBER\n");
    printf("Number should be between 0-255\n");    
}

/**
 * 
 * A little simulation of meltdown
 * only using user space
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

    printf("Meltdown with own kernel\n");
    
    uint64_t *kernel = preparekernel();
    printf("kernel[i]:i\n");

    uint64_t* probe_array = prepare_probe();
    printf("probe array[i]:2*i\n");
    
    
    printf("We will access to the 512*val of the probe array\n");
    
    printf("before asm,\n");
    printf("\tTest element:%d\n", testelement); 
    printf("\tprobe_arr[%d * 512]:%ld\n", testelement, probe_array[testelement * 512]);
    
    // pointer to do operations
    uint64_t *kernel_pointer = &kernel[testelement];
    uint64_t *free_probe_ptr = &probe_array[0];

    // if we try to read 1st element of the probe array
    // we need to access 1 * 512th element of the probe array
    // each element is 8byte, 512*8= 4KB
    // that is why we are shifting by 12 bit

    // if the value is 4
    // we need to access 4*512*8 of the probe array
    // shift 4 by 12 bits, i.e. multiply by 4096
    

    // read the prob array's testelement*512th element to readval register
    uint64_t readval;

    
    asm volatile(
        // read the value that is pointed by kernel pointer
        // to the rax register
        "mov $0, %%rax\n"
        "mov (%2), %%rax\n"
        // multiply by 4096
        "shl $12, %%rax\n"
        // compute the probe array address
        "add %%rax, %%rcx\n"
        "mov (%%rcx), %%rdx"
        :"=d"(readval), "+c"(&probe_array[0])
        :"b"(kernel_pointer)
        :"rax"
    );
    
    printf("Value in the probe array's 512*val element:%ld\n", readval);

    free(kernel);
    free(free_probe_ptr);
    return 0;
}