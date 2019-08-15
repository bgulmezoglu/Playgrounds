#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include "../../../helper/stubs.h"
#include <x86intrin.h>
#include <malloc.h>



/**
 * Returns current tick
 */
uint64_t currentTick(){
    uint64_t t1;
    unsigned int ui;
    t1 = __rdtscp(&ui);
    
    //printf("%ld ticks\n", t1);
    //printf("TSC_AUX was %x\n", ui);
    
    return t1;
}

/**
 * See description of test9
 */
void findTimeDiffTest9(uint32_t * samples, int clflushFlag){
	
	// Loop runLimit times
	int runLimit = 4096;

	uint64_t* differenceArray = (uint64_t *) malloc (runLimit * sizeof(uint64_t));

	int other = 1;

	for(int i = 0; i < runLimit; i++){
		
		if(clflushFlag){
			clflush(samples);
		}
		

		for (int j = 0; j < 17; j++)
		{
			samples[0] = j;
		}

		uint64_t t1;
		t1 = currentTick();
		
		for (int j = 0; j < 15; j++)
		{
			samples[j] = other;
		}
		
		uint64_t t2;
		t2 = currentTick();
		
		other = !other;
		
		differenceArray[i] = t2-t1;
	}

	double average = 0;
	for(int i = 0; i < runLimit; i++){
		average += differenceArray[i];
	}
	average = average / runLimit;


	printf("address of samples: %p\n", samples);
	// Write differences to the file
	if(clflushFlag){
		printf("Average of clflush execution: %f\n", average);
		
		
	}
	else{
		printf("Average of normal execution: %f\n", average);
		/*		
		for(int i = 0; i < runLimit; i++){
			printf("%ld ", differenceArray[i]);
		}
		printf("\n");
		*/
	}

}

/**
 * Test9
 * Initially, samples array is in the cache. Both cases include clflush
 * 
 * samples[0] = j, X times, 19 is the key point, WHY?
 * Start timer
 * samples[0:15] = other
 * End timer
 * 
 * 
 * 
 * clflush(samples)
 * samples[0] = j, X times, 19 is the key point, WHY?
 * Start timer
 * samples[0:15] = other
 * End timer
 */
void test9(uint32_t* samples){
	printf("Beginning test9...\n");

	int clflushFlag = 0;

	findTimeDiffTest9(&samples[0], clflushFlag);

	findTimeDiffTest9(&samples[0], !clflushFlag);
}


int main (int argc, char * argv[])
{
    
    	int sample_count = 1024; // uint is 32 bit, 4byte. total 4KB = 1024 * 4
    	int delay_loop_count = 1024;

	// What does it do?
    	warmup();

	uint32_t * samples = (uint32_t *) memalign(4096, sample_count * sizeof(uint32_t));

	// memset the whole array
    	// fill with 0's
    	memset(samples, 0, sample_count * sizeof(uint32_t));

	test9(samples);
	
	free(samples);
}


