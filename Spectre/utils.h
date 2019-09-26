#include <stdio.h>
#include <stdint.h>

#ifndef UTILS_H_INCLUDED
#define UTILS_H_INCLUDED

void clflush_element(uint64_t * addr){
	printf("addr:%ln, *addr:%ld, &addr:%p\n", addr, *addr, &addr);
	asm volatile("clflush (%0)" 
		// out
		:
		// in
		:"r"(addr));
}

void clflush_array(uint64_t *addr,int len){
	for(int i = 0; i < len; i++){
		clflush_element(&addr[i]);
	}
}

#endif