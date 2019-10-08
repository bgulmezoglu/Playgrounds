#include "spectre_helper.h"
#include "fnr_helper.h"
#include <stdio.h>

// SPECTRE V8

void victim_function(size_t x) {
	temp &= array[buffer[x < bufferSize ? x : 0] * 4096];
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
}