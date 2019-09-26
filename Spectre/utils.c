#include <stdio.h>
#include <stdint.h>


#define mfence() asm volatile("mfence;");
#define lfence() asm volatile("lfence;");

void clflush_element(uint64_t * addr);


void clflush_array(uint64_t *addr,int len);

