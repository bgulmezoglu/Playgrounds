#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <malloc.h>
// each execution will be done RUNCOUNT times
#define RUNCOUNT 4096


/**
 *
 * Does empty read to make sure the array is in the cache 
*/
void do_empty_read(uint32_t * samples){
    for (int i = 0; i < 1024; i++)
    {
        if(samples[i] != 0)
            printf("i:%d samples[i]:%d\n", i, samples[i]);
    }
}


/**
 * 
 * Calculates average of the time difference array 
*/
double calculate_average(uint64_t  * arr){
    double average = 0;
	for(int i = 0; i < RUNCOUNT; i++){
		average += arr[i];
	}
	average = average / RUNCOUNT;

    return average;
}



/**
 * 
 * Test 3
 * Clflush execution
 * clflush samples0
 * Read samples0
 * Start time
 * Read samples0 again
 * End time 
 * 
*/
void test3_c(uint32_t * samples){
    printf("Test3, clflush execution\n");
    do_empty_read(samples);

    uint64_t* differenceArray = (uint64_t *) malloc (RUNCOUNT * sizeof(uint64_t));

    for (int i = 0; i < RUNCOUNT; i++)
    {
        register uint64_t delta;
        
        asm volatile(
            // flush the samples0
            "clflush %[samples0]\n"
            // serialize it 
            "mfence\n"
            // Read samples 0
            "movl %[samples0], %%eax\n"
            // serialize it 
            "mfence\n"
            // start timer
            "rdtscp\n"
            // move counter to the r10
            "mov %%rax, %%r10\n"
            // try to read first element of the samples to the eax
            "movl %[samples0], %%eax\n"
            // read the timer again
            "rdtscp;\n"
            // find the difference between measurements
            "sub %%r10, %%rax\n"
            "mov %%rax, %[delta]"
            // output
            : [delta]"=a"(delta)
            // input
            : [samples0]"rm"(samples)
        );

        // store the difference
        differenceArray[i] = delta;
    }
    
    printf("Average: %f\n", calculate_average(differenceArray));
    free(differenceArray);
}

/**
 * 
 * Test 3
 * Normal execution
 * Read samples0
 * Start time
 * Read samples0 again
 * End time 
 * 
*/
void test3_n(uint32_t * samples){
    printf("Test3, normal execution\n");
    do_empty_read(samples);


    uint64_t* differenceArray = (uint64_t *) malloc (RUNCOUNT * sizeof(uint64_t));

    for (int i = 0; i < RUNCOUNT; i++)
    {
        register uint64_t delta;
        
        asm volatile(
            // Read samples 0
            "movl %[samples0], %%eax\n"
            // serialize it 
            "mfence\n"
            // start timer
            "rdtscp\n"
            // move counter to the r10
            "mov %%rax, %%r10\n"
            // try to read first element of the samples to the eax
            "movl %[samples0], %%eax\n"
            // read the timer again
            "rdtscp;\n"
            // find the difference between measurements
            "sub %%r10, %%rax\n"
            "mov %%rax, %[delta]"
            // output
            : [delta]"=a"(delta)
            // input
            : [samples0]"rm"(samples)
        );

        // store the difference
        differenceArray[i] = delta;
    }
    
    printf("Average: %f\n", calculate_average(differenceArray));
    free(differenceArray);
}

/**
 * 
 * Test 0
 * Clflush execution 
 * 
 * Clflush first elements
 * Start timer
 * Read first element of samples
 * End timer
*/
void test0_c(uint32_t * samples){
    printf("Test0, clflush execution\n");
    do_empty_read(samples);

    uint64_t* differenceArray = (uint64_t *) malloc (RUNCOUNT * sizeof(uint64_t));

    for (int i = 0; i < RUNCOUNT; i++)
    {
        register uint64_t delta;
        
        asm volatile(
            // flush the samples0
            "clflush %[samples0]\n"
            // serialize it 
            "mfence\n"
            // start timerflushed_addr
            "rdtscp\n"
            // move counter to the r10
            "mov %%rax, %%r10\n"
            // try to read first element of the samples to the eax
            "movl %[samples0], %%eax\n"
            // read the timer again
            "rdtscp;\n"
            // find the difference between measurements
            "sub %%r10, %%rax\n"
            "mov %%rax, %[delta]"
            // output
            : [delta]"=a"(delta)
            // input
            : [samples0]"rm"(samples)
        );
        
        // If you do this:
        // "movl %[samples0], %%eax\n"
        // .. some code ..
        // // input
        // : [samples0]"rm"(*samples)

        // It will try to move a pointer value to the register, which is not our purpose
        // Our purpose is to read a memory location.


        // Also, if you do this:
        // "movl (%[samples0]), %%eax\n"
        // .. some code ..
        // // input
        // : [samples0]"rm"(*samples)

        // The value of the *samples is 0
        // And we are trying to access to the 0th memory location and move it
        // to the register, which will end up in segmentation fault.





        // TODO
        // The difference between?
        
        // "sub %%r10, %[delta]"
        // : [delta]"=a"(delta)
        
        // and
        
        //  "sub %%r10, %%rax\n"
        //  "mov %%rax, %[delta]"
        // : [delta]"=a"(delta)
        
        // store the difference
        differenceArray[i] = delta;
    }
    
    printf("Average: %f\n", calculate_average(differenceArray));
    free(differenceArray);
}


/**
 * 
 * Test 0
 * Normal execution 
 * 
 * Start timer
 * Read first element of samples
 * End timer
 * 
 * 
*/
void test0_n(uint32_t * samples){
    printf("Test0, normal execution\n");
    do_empty_read(samples);


    uint64_t* differenceArray = (uint64_t *) malloc (RUNCOUNT * sizeof(uint64_t));

    for (int i = 0; i < RUNCOUNT; i++)
    {
        register uint64_t delta;
        
        asm volatile(
            // serialize it 
            "mfence\n"
            // start timer
            "rdtscp\n"
            // move counter to the r10
            "mov %%rax, %%r10\n"
            // try to read first element of the samples to the eax
            "movl %[samples0], %%eax\n"
            // read the timer again
            "rdtscp;\n"
            // find the difference between measurements
            "sub %%r10, %%rax\n"
            "mov %%rax, %[delta]"
            // output
            : [delta]"=a"(delta)
            // input
            : [samples0]"rm"(samples)
        );

        // store the difference
        differenceArray[i] = delta;
    }
    
    printf("Average: %f\n", calculate_average(differenceArray));
    free(differenceArray);
}

int main(){
    
    int sample_count = 1024; // uint is 32 bit, 4byte. total 4KB = 1024 * 4

    uint32_t * samples = (uint32_t *) memalign(4096, sample_count * sizeof(uint32_t));

    // memset the whole array
    // fill with 0's
    memset(samples, 0, sample_count * sizeof(uint32_t));

    //test0_n(samples);
    //test0_c(samples);


    test3_c(samples);
    test3_c(samples);

    free(samples);
    return 0;
}