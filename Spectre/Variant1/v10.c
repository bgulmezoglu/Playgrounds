#include "spectre_helper.h"
#include "fnr_helper.h"
#include <stdio.h>


// SPECTRE 10
/**
 * Null pointer access and signal handler is added 
 * to improve accuracy.
 * Without them, there is no hit in the cache
*/ 
void victim_function(size_t x, uint8_t k) {   
	if (x < bufferSize) {
		if (buffer[x] == k){
			temp &= array[3 * 4096];
		}
	}
}