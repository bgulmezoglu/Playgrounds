#include "spectre_helper.h"
#include "fnr_helper.h"
#include <stdio.h>
#include <string.h>

// SPECTRE 11
void victim_function(size_t x) {
    if (x < bufferSize)
        temp = memcmp(&temp, array + (buffer[x] * 4096), 1);
}