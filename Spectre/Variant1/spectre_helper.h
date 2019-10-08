#include <stdint.h>
#include <stdio.h>

// To avoid fetching previous line
#define DELTA 1024
#define CACHE_HIT_THRESHOLD 120

extern uint8_t array[256 * 4096];
extern uint8_t temp;
extern int bufferSize;
extern int bufferSizeMask;


// TODO
// If we declare buffer size as 16 instead of 160, it works, but with lower resolution
// WHY?
extern uint8_t buffer[160];
// TODO
// If the secret is short, like below, Spectre won't work! WHY?
// char secret[] = "It's the secret.";
// unreachable but known address
extern char secret[];

// imported by spectre_helper.c
void find_cached_index(int* scores);
void find_highest_score(int* scores);
void train_victim();
void steal_byte(int* scores, int larger_x);

// imported by v1. v2 ...c
#ifdef __SPECTRE__V7
size_t* victim_function(size_t x);
#else
void victim_function(size_t x);
#endif