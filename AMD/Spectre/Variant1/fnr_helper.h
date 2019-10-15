
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

