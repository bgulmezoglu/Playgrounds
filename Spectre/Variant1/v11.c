#include "spectre_helper.h"
#include "fnr_helper.h"
#include <stdio.h>
#include <string.h>

// SPECTRE 11
/**
 * TODO
 * 
 * it is failing
*/ 
void victim_function(size_t x) {
    if (x < bufferSize)
        temp = memcmp(&temp, buffer + (array[x] *4096 ), 1);
}