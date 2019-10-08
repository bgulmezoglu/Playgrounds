#include "spectre_helper.h"
#include "fnr_helper.h"
#include <stdio.h>

// SPECTRE V2
void leakByteLocalFunction(uint8_t k) { temp &= array[(k)* 4096]; }

void victim_function(size_t x) {
	if(x < bufferSize) {
		leakByteLocalFunction(buffer[x]);
	}
}


