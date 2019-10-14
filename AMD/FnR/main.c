#include <stdio.h>

#include <stdlib.h>
u_int8_t array[256 * 4096];

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


int main(){
	printf("fnr started\n");
	// initialize the array
	for (int i = 0; i < 256 * 4096; i++){
		array[i] = i % 256;
	}
	clflush_array(array);

	u_int8_t temp = 1;

	temp &= array[4096];

	u_int64_t time_dif;
	int mix_i;
	u_int64_t times[256];
	for(int i = 0; i < 256; i++){
		mix_i = ((i * 167) + 13) & 255;

		time_dif = 0;
		measure_time(time_dif, &array[mix_i * 4096]);

		times[mix_i] = time_dif;
	}

	for(int i = 0; i < 256; i++){
		printf("times[%d]:\t%ld\n", i, times[i]);
	}

	return 0;
}
