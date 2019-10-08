#include "spectre_helper.h"
#include "fnr_helper.h"
#include <stdio.h>

// SPECTRE V1

void victim_function(size_t x){
	if(x < bufferSize){
		// read the array's element to temp
		temp &= array[buffer[x] * 4096];
	}
}
