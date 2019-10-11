#include "spectre_helper.h"
#include "fnr_helper.h"
#include <stdio.h>

// SPECTRE V8
/**
 * 
 * While trying to access to the index out of bounds,
 * it returns 0. Which means buffer[0] = 1, and we are accessing
 * to the 1st element of the array. That is why I have modified
 * find cached index function to start from index 2, instead of 0.
 * 
 * Accuracy is low!
 */ 
void victim_function(size_t x) {
	temp &= array[buffer[x < bufferSize ? x : 0] * 4096];	
}
/*
	// Below works, but with low accuracy.
	// Multiple instructions for checking
	if(x < bufferSize){
		temp &= array[buffer[x] * 4096];
	}
	else{
		temp &= array[0 * 4096];
	}
*/